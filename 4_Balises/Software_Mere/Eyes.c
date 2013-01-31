/*
 *	Club Robot ESEO 2012 - 2013
 *
 *	Fichier : Eyes.c
 *	Package : Balises
 *	Description : Acquisition et traitement des donn�es IR en provenance des balises autour du terrain.
 *	Auteur : Nirgal
 *	Version 201208
 */


#include "Eyes.h"
		
#include "QS/QS_extern_it.h"
#include "QS/QS_timer.h"
#include <timer.h>
#include "Brain_IR.h"
#include <InCap.h>
	
	
/*
Chaque cycle dure 100ms et d�marre � l'instant de synchro.
Pendant un cycle, l'une des balises �mettrice arrose le terrain d'un spot IR tournant � la vitesse de 17 tours par secondes. (58,8 ms /tour)
Ainsi, on obtient 1,7 tours moteurs pendant ces 100ms. On est ainsi sur de voir le signal au moins deux fois sur au moins l'une des balises.

La proc�dure est la suivante.

Pendant le cycle :
Pour chaque balise r�ceptrice : 

- acquisition des t�ches (un t�che se caract�rise par un d�but et une fin... qui donnent et un point central (moyenne des deux) et une dur�e (diff�rence).

Apr�s le cycle (donc pendant le suivant...)

- On retient les deux t�ches les plus importantes (ou la t�che la plus importante).
- S'il y'en a deux et qu'elles sont espac�es d'environ 58,8ms, on garde les deux. sinon on ne garde que la plus grande des deux.

A l'issue de ce traitement, on isole "un tour moteur" avec les t�ches espac�es de 58,8ms. Et on calcule les angles obtenus.
On consulte ensuite le tableau pr�calcul� qui nous donne une position en x et y.


Pendant ce processus, on v�rifie les conditions qui doivent �tre vraies afin d'affiner le coeff de fiabilit� :
- pendant un tour moteur, les deux autres balises doivent voir une fois le spot.
- les t�ches doivent �tre constitu�es de motifs � 3,33khz (point trait� pendant l'acquisition !)

Ainsi, si une balise est obstru�e et n'a rien vu, aucune mesure ne sera renvoy�e (o� plut�t l'octet de fiabilit� la rendra inutilis�e).






Pseudo-Code :
Un timer est (re)lanc� par le module de synchro chaque d�but de 100ms (lanc� pour une p�riode + grande => son IT ne s'execute jamais...
Il sert d'horloge...
Le timer2 est utilis� � cet effet. Sa valeur courante, TMR2 s'exprime en [25us] !

void it_timer_100ms(void)
{
	debug_printf("ERROR : IT timer chrono !");
	LED_ERROR = 1; //NEVER HAPPEN !
	//flag = 0;
}

interrupt_INT1
{
	//Si montant
	{	
		//active it front descendant
		//detection_event(BEACON_...., RISING_EDGE, TMRn, );	//On informe la fonction unique qui traite l'�v�nement
	}
	//sinon
	{
		//active it front descendant
		//..........................FALLING_EDGE........
	}
	//flag = 0
}




*/




static adversary_eyes_buffers_t adversaries_eyes_buffers[ADVERSARY_NUMBER];





static volatile adversary_e current_adversary;
/*
CODE REUTILISABLE POUR LA GESTION DES DETECTIONS.	
static adversary_beacons_detections_t adversaries_beacons_detections[ADVERSARY_NUMBER];

volatile beacon_detections_t * current_adversary_beacons_detections;


void EYES_init_adversary_beacons_detections(adversary_beacons_detections_t * adversary_beacons_detection)
{
	beacon_detections_t * p_beacon_detection;
	Uint8 beacon_id, index;
	
	for(beacon_id = 0; beacon_id <= BEACON_ID_MIDLE; beacon_id++)
	{
		p_beacon_detection = &adversary_beacons_detection->beacons_detections[beacon_id];
		
		for (index = 0; index < 3; index++)
		{
			p_beacon_detection->detections[index].end = 0;
			p_beacon_detection->detections[index].begin = 0;
			p_beacon_detection->detections[index].duration = 0;
		}
		adversary_beacons_detection->beacons_detections[beacon_id].current_detection_index = 0;
		adversary_beacons_detection->beacons_detections[beacon_id].more_than_3_detections = FALSE;
	}
	adversary_beacons_detection->initialized = TRUE;
	
}	
*/

volatile static Uint16 buffer_mother[400];
volatile static Uint16 buffer_index = 0;


void EYES_init_adversary_eyes_buffers(adversary_eyes_buffers_t * adversary_eyes_buffers)
{
	Uint8 beacon_id;
	Uint16 index;
	for(beacon_id = 0; beacon_id <= BEACON_ID_MIDLE; beacon_id++)
	{
		adversary_eyes_buffers->buffers[beacon_id].index = 0;
		for(index = 0; index<EYE_SIGNAL_BUFFER_SIZE; index++)
			adversary_eyes_buffers->buffers[beacon_id].buffer[index] = 0;
	}	
	adversary_eyes_buffers->initialized = TRUE;
}	

void EYES_init(void)
{
	Uint16 i;
	
	TIMER2_run(150);	// > 100, pour que l'IT ne se d�clenche jamais...
	TIMER3_run_us(1000);	//1ms, pas plus de 4 fronts possible sur cette p�riode.

/*	adversaries_beacons_detections[ADVERSARY_1].adversary = ADVERSARY_1;	//On renseigne sur qui est qui.
	adversaries_beacons_detections[ADVERSARY_2].adversary = ADVERSARY_2;
	EYES_init_adversary_beacons_detections(&(adversaries_beacons_detections[ADVERSARY_1]));
	EYES_init_adversary_beacons_detections(&(adversaries_beacons_detections[ADVERSARY_2]));
*/
	current_adversary = ADVERSARY_1;
	adversaries_eyes_buffers[ADVERSARY_1].adversary = ADVERSARY_1;
	adversaries_eyes_buffers[ADVERSARY_2].adversary = ADVERSARY_2;
	EYES_init_adversary_eyes_buffers((adversary_eyes_buffers_t *)&adversaries_eyes_buffers[ADVERSARY_1]);
	EYES_init_adversary_eyes_buffers((adversary_eyes_buffers_t *)&adversaries_eyes_buffers[ADVERSARY_2]);
	
	//initialisation des IT externes.
/*	EXTERN_IT_set_priority(INT1, 6);
	EXTERN_IT_set_priority(INT2, 6);
	EXTERN_IT_set_priority(INT3, 6);

	EXTERN_IT_enable(INT1,1);
	EXTERN_IT_enable(INT2,1);
	EXTERN_IT_enable(INT3,1);
	*/
		
	OpenCapture4(IC_IDLE_CON & IC_TIMER2_SRC & IC_INT_4CAPTURE & IC_EVERY_FALL_EDGE);
	ConfigIntCapture4(IC_INT_OFF & IC_INT_PRIOR_6);	//D�sactivation des IT input capture, on pr�f�re utiliser un timer !
}	
	
void _ISR _IC4Interrupt(void)
{
	//Ne doit jamais se produire.
	debug_printf("interrupt input capture\n");
	IFS1bits.IC4IF = 0;
}	

void _ISR _T3Interrupt(void)
{
	Uint8 i;
	Uint16 buf[10];
	
	//ReadCapture4((unsigned int *)&buffer_mother[buffer_index]);
	while (IC4CONbits.ICBNE)
	{
//		adversaries_eyes_buffers[current_adversary].buffers[BEACON_ID_MOTHER].buffer[adversaries_eyes_buffers[current_adversary].buffers[BEACON_ID_MOTHER].index] = IC4BUF; /* reads the input capture buffer */
	//	adversaries_eyes_buffers[current_adversary].buffers[BEACON_ID_MOTHER].index++;
	}
	
	
	//on place l'index sur la prochaine case vide.
/*	for(i=0;i<4;i++)
	{
		if(buffer_mother[buffer_index] != 0 && buffer_index<400)
			buffer_index++;		
		else
			break;
	}	
*/	
	if(IC4CONbits.ICOV)
		debug_printf("overflow input capture\n"); //Ne doit jamais se produire.
		
	IFS0bits.T3IF = 0;
}	
	
	void EYES_step(adversary_e new_adversary)
	{
		Uint16 i;
		adversary_e previous_adversary = (new_adversary==ADVERSARY_1)?ADVERSARY_2:ADVERSARY_1;

		TMR2 = 0;	//RAZ de notre horloge...
		
		if(current_adversary != new_adversary)	//C'est toujours le cas, sauf peut �tre au d�but !
			BRAIN_IR_task_add((adversary_eyes_buffers_t *)&(adversaries_eyes_buffers[previous_adversary]));
		adversaries_eyes_buffers[previous_adversary].initialized = FALSE;
		
		//On v�rifie que la structure utilis�e actuellement est bien � notre disposition, et initialis�e.
		//Si ce n'est pas le cas, c'est � cause
		if(adversaries_eyes_buffers[new_adversary].initialized==FALSE)	//N'est jamais cens� se produire !
		{
			//debug_printf("GRAVE_ERREUR : main bloqu� ou trop lent\n");
			EYES_init_adversary_eyes_buffers((adversary_eyes_buffers_t *)&adversaries_eyes_buffers[new_adversary]);	//On initialise tout de m�me			
		}
	
		current_adversary = new_adversary;
	}


	void EYES_process_main(void)
	{
	
	}	


	void _ISR _T2Interrupt()
	{
		//N'est jamais cens� se produire lorsque toutes les balises recoivent le signal..
		//debug_printf("TMR2!");
		IFS0bits.T2IF = FALSE;
	}
////////////////////////////////////////////////////////////////////////////////////////////
static time16_t detections_test_begin[100];
static time16_t detections_test_end[100];
static Uint8 index_detections_test = 0;


void EYES_detection_event(beacon_id_e beacon_id, edge_e edge, Uint16 time)
{
	Uint8 index;
	if(index_detections_test == 99)
	{
		index_detections_test = 0;
		for(index = 0; index < 100; index++)
			debug_printf("%2d: %4d->%4d = %4d\n", index, detections_test_begin[index], detections_test_end[index], detections_test_end[index]-detections_test_begin[index]);
	}	
	else
	{
		
		//if(edge == FALLING_EDGE)
	//	{
			if(time - detections_test_end[index_detections_test] > 12) //=300us	//on consulte la d�tection concern�e pour savoir o� elle en est.
			{	
				index_detections_test++;
				detections_test_begin[index_detections_test] = time;
			}	
	//	}	
		detections_test_end[index_detections_test] = time;	
	}	
	
	
	
	
/*	Uint8 index;
	beacon_detections_t * p_beacon_detection = &(adversaries_beacons_detections[current_adversary].beacons_detections[beacon_id]);
	detection_t * p_current_detection = &(p_beacon_detection->detections[p_beacon_detection->current_detection_index]);
	index = p_beacon_detection->current_detection_index;
	if(index>=10)
	{
		debug_printf("index ovflw\n");
		return;
	}	
	if(edge == FALLING_EDGE) //ATTENTION : Un front descendant signifie le DEBUT d'une d�tection... !!!
	{
		//recherche pr�alable du pointeur vers la balise concern�e...
		if(time - p_current_detection->end > 20) //=500us	//on consulte la d�tection concern�e pour savoir o� elle en est.
		{ 
			//si le dernier front montant date de + de 300us :	
			//On d�marre une nouvelle d�tection
			
*/			//Recherche du nouvel index 
		/*	if(p_beacon_detection->more_than_3_detections == FALSE && p_beacon_detection->current_detection_index < 2)
			{
				p_beacon_detection->current_detection_index++;
			}
			else
			{
				p_beacon_detection->more_than_3_detections = TRUE;	//C'est peut �tre d�j� le cas, peu importe.
				//Recherche de la d�tection la plus petite pour �crasement
				Uint8 index;
				if(p_beacon_detection->detections[0].duration < p_beacon_detection->detections[1].duration)
				{
					if(p_beacon_detection->detections[0].duration < p_beacon_detection->detections[2].duration)
						index = 0;
					else
						index = 2;
				}
				else
				{
					if(p_beacon_detection->detections[1].duration < p_beacon_detection->detections[2].duration)
						index = 1;
					else
						index = 2;
				}
				p_beacon_detection->current_detection_index = index;
			
			}
		*/
/*			p_beacon_detection->current_detection_index++;
			index++;
			
			if(index<10)
			{
				p_current_detection = &(p_beacon_detection->detections[index]);
				p_current_detection->begin = time;
				p_current_detection->end = time;
				p_current_detection->duration = 0;
			}			
		}
		else
		{
			//=> on met � jour la fin de la d�tection
			p_current_detection->end = time;
			p_current_detection->duration = time - p_current_detection->begin;
		}
	}
	else
	{
		p_current_detection->end = time;
		p_current_detection->duration = time - p_current_detection->begin;
			//CAS DU RISING_EDGE
			//on repousse le "end"... pour etre parfaitement sym�trique !
	}
*/
}




	void _ISR _INT1Interrupt(void)
	{		
		Uint16 time;
		time = TMR2;	//On fait ca au tout d�but pour avoir toujours la m�me mesure du temps relativement au front du signal entrant.
		//debug_printf("1");
		_INT1IF = 0;
		_INT1EP = !_INT1EP;	//EXTERN_IT_enable(INT1,(_INT1EP)?0:1);
	//	EYES_detection_event(BEACON_ID_CORNER, (_INT1EP)?RISING_EDGE:FALLING_EDGE, time);	
			
			
	}	
		
	
	void _ISR _INT2Interrupt(void)
	{	
		Uint16 time;
		time = TMR2;	//On fait ca au tout d�but pour avoir toujours la m�me mesure du temps relativement au front du signal entrant.
		//debug_printf("2");
		_INT2IF = 0;
		_INT2EP = !_INT2EP;	//EXTERN_IT_enable(INT2,(_INT2EP)?0:1);
	//	EYES_detection_event(BEACON_ID_MIDLE, (_INT2EP)?RISING_EDGE:FALLING_EDGE, time);	
		
		
			
	}	
	
	
	void _ISR _INT3Interrupt(void)
	{
		Uint16 time;
		time = TMR2;	//On fait ca au tout d�but pour avoir toujours la m�me mesure du temps relativement au front du signal entrant.
		//debug_printf("3");
		_INT3IF = 0;	
		//_INT3EP = !_INT3EP;	//EXTERN_IT_enable(INT3,(_INT3EP)?0:1);
		EYES_detection_event(BEACON_ID_MOTHER, !_INT3EP, time);		
	}
