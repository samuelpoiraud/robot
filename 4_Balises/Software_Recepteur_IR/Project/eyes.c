/*
 *	Club Robot ESEO 2009 - 2010 - 2012
 *	
 *
 *	Fichier : eyes.c
 *	Package : Balises 2010
 *	Description : gestion des TSOP récepteurs infrarouge
 *	Auteur : Nirgal
 *	Version 201203
 */


#include "eyes.h"
#include "brain.h"
#include "secretary.h"
#include "QS/QS_timer.h"
#include "QS/QS_CANmsgList.h"			
#include "QS/QS_outputlog.h"		

	Uint16 EYES_process_read_tsop(void);
	void EYES_next_adversary(adversary_e previous_adversary, adversary_e next_adversary);	
	Uint16 EYES_process_read_tsop(void);
	void EYES_process_record(Uint16 reception);


	static volatile Uint8 step = 0;
	static EYES_adversary_detections_t adversary_detections[ADVERSARY_NUMBER];
	static volatile adversary_e current_adversary;
	static volatile Uint16 time = 0;
	
	//fonction d'initialisation, éxécutée une fois en début de code.
	void EYES_init(void)
	{
		step = 0;
		time = 0;
		
		adversary_detections[ADVERSARY_1].adversary = ADVERSARY_1;	//On renseigne sur qui est qui.
		adversary_detections[ADVERSARY_2].adversary = ADVERSARY_2;
		EYES_init_adversary_detection(&(adversary_detections[ADVERSARY_1]));
		EYES_init_adversary_detection(&(adversary_detections[ADVERSARY_2]));

		current_adversary = ADVERSARY_1;
		
				// la frequence mini vitale est 6,66kHz = 150µs...(shannon)....
		//je choisis environ 7,7kHz	 > 6,66...
		TIMER1_run_us(CONFIG_TIMER_ECHANTILLONAGE);	//130us
		TIMER4_run(100); //100ms	//utilisé en absence de synchro.

	}	
	
	
	volatile bool_e request_reset_step = FALSE;
	
	
	void EYES_step_init(void)
	{
		TIMER4_stop();
		request_reset_step = TRUE;
	}	

	
	
#define PERIODE_FLASH	50 //[2ms] => un émetteur est actif pour 100ms.
	
	void EYES_next_adversary(adversary_e previous_adversary, adversary_e next_adversary)
	{
		//cette fonction étant appelée environ toutes les 100ms :
		SECRETARY_process_it_100ms();
		
		if(current_adversary != next_adversary)	//C'est toujours le cas, sauf peut être au début !
			BRAIN_task_add(&(adversary_detections[previous_adversary]));
		
		//On vérifie que la structure utilisée actuellement est bien à notre disposition, et initialisée.
		//Si ce n'est pas le cas, c'est à cause
		if(adversary_detections[next_adversary].initialized==FALSE)	//N'est jamais censé se produire !
		{
			debug_printf("GRAVE_ERREUR : main bloqué ou trop lent\n");
			EYES_init_adversary_detection(&adversary_detections[next_adversary]);	//On initialise tout de même
			adversary_detections[next_adversary].initialized = FALSE;				
		}
		
		time = 0;
		current_adversary = next_adversary;	
	}	
	
	
	void EYES_next_step(void)
	{
		if(request_reset_step == TRUE && step > 0 && step <99)
			step = 0;	
		else
			step = (step==2*PERIODE_FLASH-1)?0:step+1;		//Période de 100ms...* nb balises émettrices = 200ms = (5 mesures par seconde !) 
		request_reset_step = FALSE;

		if(step == 0)
			EYES_next_adversary(ADVERSARY_2,ADVERSARY_1);

		if(step == PERIODE_FLASH)
			EYES_next_adversary(ADVERSARY_1,ADVERSARY_2);

	}	

	void _ISR _T4Interrupt()
	{
		//La synchro n'étant pas encore arrivée,
		//On suppose qu'il n'y a qu'un adversaire.
		//Mais malgré cela, on fait comme si il y en avait deux, pour ne pas se marcher dessus dans la mémoire !
		EYES_next_adversary(current_adversary,(current_adversary == ADVERSARY_1)?ADVERSARY_2:ADVERSARY_1);
		IFS1bits.T4IF	= 0;
	}	

	
	//Timer1 > ce timer est l'échantillonnage très régulier des récepteurs...
	// sur chaque déclenchement de ce timer, on fait une mesure de tout les récepteurs + une analyse...
	void _ISR _T1Interrupt()
	{
		LED_RUN = 0;
		//la gestion des envois can peut avoir besoin d'un petit avertissement à chaque nouvelle valeur...
		//CAN_process_increment();
		
		//acquisition brute et analyse des résultats.
		EYES_process_record(EYES_process_read_tsop());
		time++;
		LED_RUN = 1;
		IFS0bits.T1IF=0;
	}


	

	Uint16 EYES_process_read_tsop(void)
	{
		static Uint16 port_maintenant;
		static Uint16 port_precedent;
		static Uint16 retour;
		//L'idée est la suivante :
		//on fournit un entier sur 16 bits. chacun des bits concerne un tsop...
		// un bit vaudra 1 si le tsop vient de franchir un état de 0 vers 1.
		// sinon, il vaudra 0.
		
		//ACQUISITION
		port_maintenant = (~PORT_TSOP);
		retour = port_maintenant & (~port_precedent);	//sont à 1 les bits qui étaient précédemment à 0 et 1 maintenant.
		port_precedent = port_maintenant;
		return retour;
		//ATTENTION, le '~' est un NON bit à bit, car chaque TSOP renvoi un 0 lorsqu'il recoit un signal.
		//A partir de cette ligne, un '1' est considéré comme une réception.
		//Les 4 bits de poids faibles sont inutilisés.
	}
	

	void EYES_process_record(Uint16 reception)
	{
		detection_t * 	detections 	= adversary_detections[current_adversary].detections;
		Uint8 		*	p_index 	= &(adversary_detections[current_adversary].index);
		Uint8			index		= *p_index;
		//on passe par un entier local 'reception' passé en argument pour éviter des accès trop long... (gain de 10µs environ...)
			
		if(reception != 0)	//Si l'un des récepteurs à recu...
		{								
		//TODO cas où la réception est super longue ! et dépasse la durée mini entre deux détections !			
			if(time - detections[index].end > MINIMUM_DURATION_BETWEEN_TWO_DETECTIONS)
			{
				//CREATION NOUVELLE DETECTION	
				index++;
				if(index >= MAX_DETECTIONS_NUMBER)
				{
					index = MAX_DETECTIONS_NUMBER-1;
					adversary_detections[current_adversary].error |= TROP_DE_SIGNAL;
				}	
					
				*p_index = index;
					
				//Ecriture INSTANT DEBUT DETECTION
				detections[index].begin = time;
			}		
				
			//MAJ INSTANT FIN DETECTION	
			detections[index].end = time;

			
			//MAJ des compteurs...
			//On pourrait trouver une belle boucle... mais le but est de pas trainer à calculer des i ou des trucs longs à calculer...
			//ICI, il faut pédaler dur ! (la récup du pointeur ci-dessous permet de gagner du temps !)
			Uint8 * p_counts = detections[index].counts;

			if(reception & 0b0000000000000001)	p_counts[0]++;
			if(reception & 0b0000000000000010)	p_counts[1]++;
			if(reception & 0b0000000000000100)	p_counts[2]++;
			if(reception & 0b0000000000001000)	p_counts[3]++;
			if(reception & 0b0000000000010000)	p_counts[4]++;
			if(reception & 0b0000000000100000)	p_counts[5]++;
			if(reception & 0b0000000001000000)	p_counts[6]++;
			if(reception & 0b0000000010000000)	p_counts[7]++;
			if(reception & 0b0000000100000000)	p_counts[8]++;
			if(reception & 0b0000001000000000)	p_counts[9]++;
			if(reception & 0b0000010000000000)	p_counts[10]++;
			if(reception & 0b0000100000000000)	p_counts[11]++;
			if(reception & 0b0001000000000000)	p_counts[12]++;
			if(reception & 0b0010000000000000)	p_counts[13]++;
			if(reception & 0b0100000000000000)	p_counts[14]++;
			if(reception & 0b1000000000000000)	p_counts[15]++;		
		}	
	}	
	
	
	
	
	
	void EYES_init_adversary_detection(volatile EYES_adversary_detections_t * adversary_detection)
	{
		//Préparation du tableau pour la prochaine acquisition...
		detection_t * 	detections;	
		Uint8 i, tsop;
		detections = (detection_t *)adversary_detection->detections;
		adversary_detection->index = 0;
		adversary_detection->error = AUCUNE_ERREUR;
		for(i=0;i<MAX_DETECTIONS_NUMBER; i++)
		{
			detections[i].begin = 0;
			detections[i].end = 0;
			for(tsop=0;tsop<NOMBRE_TSOP;tsop++)
				detections[i].counts[tsop] = 0;	
		}
		adversary_detection->initialized	= TRUE;
	}
	
	
		



