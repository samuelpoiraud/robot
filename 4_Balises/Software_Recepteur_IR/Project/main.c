 /*
 *	Club Robot ESEO 2009 - 2010
 *	
 *
 *	Fichier : main.h
 *	Package : Balises 2010
 *	Description : MAIN
 *	Auteur : Nirgal
 *	Version 201203
 */
#include "main.h"
#include "Synchro.h"
#include "eyes.h"
#include "brain.h"
#include "secretary.h"
#include "QS/QS_ports.h"
#include "QS/QS_uart.h"
#include "QS/QS_timer.h"

		
#ifdef CARTE_2012
	void debug_print(void);	

	void init(void)
	{
		PORTS_init();
		TIMER_init();
		UART_init();	
		
		BRAIN_init();
		EYES_init();
		Synchro_init();
		
		SECRETARY_init();
		debug_printf("BeaconIR>i'm alive\n");
	
		LED_RUN=0;	//Allumée en IT pour dire que tout va bien...
	}
	


	int main (void)
	{
		init();
		while (1)
		{	
			if(!BOUTON2)
				debug_print();
			SECRETARY_process_main();	//Gestion de la communication CAN et UART.
			BRAIN_process_main();		//Analyse des mesure obtenues après chaque cycle de 100ms
			Synchro_process_main();		//Correction de la déviation de la synchro
		}
		return 0; //satisfaction de la perversité du compilateur, ligne jamais atteinte...
	}


	void debug_print(void)
	{
		volatile adversary_location_t * p_adversary_location;
		p_adversary_location = BRAIN_get_adversary_location();
		if(global.flag_100ms)
		{
			global.flag_100ms = FALSE;
			debug_printf("e=%x|a=%d°\t|d=%d\t\n",
							p_adversary_location[ADVERSARY_1].error, 
							(Sint16)(((Sint32)p_adversary_location[ADVERSARY_1].angle*180)/PI4096),
							((Uint16)p_adversary_location[ADVERSARY_1].distance)*2
						);	
		}				
	}
#endif	//def CARTE_2012

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////RETROCOMPATIBILITE POUR LES CARTES 2010 ET 2011 !!!//////////////////

/*
void initialisation(void)
{
	PORTS_init();
	TIMER_init();
	UART_init();

	
	EYES_init();
	CAN_balise_init();
	
	LED_RUN=0;	//Allumée en IT pour dire que tout va bien...
}

void tests(void)
{
	debug_printf("Carte Balise : \"I'm stil alive.\"\n");
	
	#ifdef MODE_TEST_DIODES
		TIMER1_stop();
		TIMER2_run(50);
		//Les diodes clignoteront les unes après les autres
		//Le reste du code est désactivé.
		while(1);
	#endif
	
	#ifdef MODE_RECEPTION_SIMPLE
		TIMER1_stop();
		TIMER2_stop();
		//affichage sur les leds des récepteurs qui recoivent qch !
		//Le reste du code est désactivé !
		while(1)
			process_affichage_leds_reception_simple();
	#endif
	
}



	
int main (void)
{
	initialisation();
	tests();	//ATTENTION : Peut être bloquant dans certains modes de tests !
	
	while (1)
	{	
		//La gestion de la communication se fait en tache de fond
		//Les réceptions sur TSOP sont gérées en IT.
		if(!BOUTON2)
			debug_printf("e=%x|a=%d°\t|d=%d\t|x=%d\t|y=%d\n",global.localisation_erreur, (Sint16)(((Sint32)global.adverse_angle*180)/PI4096), (Uint16)global.adverse_distance/10, global.adverse_x, global.adverse_y);

		CAN_process_msg_recus();		
		
		CAN_process_gestion_envoi();	
		
		if(global.flag_nouveau_cycle)
		{

			process_affichage_leds();

	//		debug_printf("%x|%d° %d\n",global.localisation_erreur, (Sint16)(((Sint32)global.adverse_angle*180)/PI4096),(Uint16)global.adverse_distance/10);
			global.localisation_erreur = 0;
		}	

		localisation_process_main();
*/		
	/*	Uint16 i;
		if(global.index_recalage > 37)
		{
			for(i = 0; i<37 ; i++)
				debug_printf("%d\n",global.recalage[i]);
			global.index_recalage = 0;
		}
		*/
	/*	Uint16 i,j;
		if(global.b_lock_save_detections == TRUE)
		{
			for(i=0;i<=global.save_index_detections;i++)
			{
				debug_printf("dét:%d, déb:%d, f:%d, ", i, global.save_detections[i].debut, global.save_detections[i].fin);
				for(j=0;j<16;j++)
					debug_printf("%d:%d\n", j, global.save_detections[i].compteurs[j]);
			}		
			global.b_lock_save_detections = FALSE;	
		}	
	*/		
/*	}

	return 0; //securité
}



	void process_affichage_leds(void)
	{
		Uint8 index_cycles;
		index_cycles = global.index_cycles;
		LED_0 = (global.cycles[index_cycles].compteurs[0]>4)?1:0;
		LED_1 = (global.cycles[index_cycles].compteurs[1]>4)?1:0;
		LED_2 = (global.cycles[index_cycles].compteurs[2]>4)?1:0;
		LED_3 = (global.cycles[index_cycles].compteurs[3]>4)?1:0;
		LED_4 = (global.cycles[index_cycles].compteurs[4]>4)?1:0;
		LED_5 = (global.cycles[index_cycles].compteurs[5]>4)?1:0;
		LED_6 = (global.cycles[index_cycles].compteurs[6]>4)?1:0;
		LED_7 = (global.cycles[index_cycles].compteurs[7]>4)?1:0;
		LED_8 = (global.cycles[index_cycles].compteurs[8]>4)?1:0;
		LED_9 = (global.cycles[index_cycles].compteurs[9]>4)?1:0;
		LED_10 = (global.cycles[index_cycles].compteurs[10]>4)?1:0;
		LED_11 = (global.cycles[index_cycles].compteurs[11]>4)?1:0;
		LED_12 = (global.cycles[index_cycles].compteurs[12]>4)?1:0;
		LED_13 = (global.cycles[index_cycles].compteurs[13]>4)?1:0;
		LED_14 = (global.cycles[index_cycles].compteurs[14]>4)?1:0;
		LED_15 = (global.cycles[index_cycles].compteurs[15]>4)?1:0;
		
		//Utilisé pour calibrer en 2010 la puissance d'émission...
//		if( global.adverse_distance > 50 && global.adverse_distance < 70)
//			LED_RUN = !LED_RUN;
//		else LED_RUN = 1;
	}



#ifdef MODE_TEST_DIODES

//Le timer 2 sert à cadencer les envois de messages CAN/UART, il intervient à des intervalles assez grands, de l'ordre de 100ms à 1s par exemple...
void _ISR _T2Interrupt()
{
	process_next_diode();	//test des diodes...

	
	IFS0bits.T2IF=0;
}

#endif
	

	#ifdef MODE_TEST_DIODES
		void process_next_diode(void)
		{
			//test des diodes
			static Uint8 leds_compteur = 4;
			
			//RAZ diodes
			LED_0 = FALSE;
			LED_1 = FALSE;
			LED_2 = FALSE;
			LED_3 = FALSE;
			LED_4 = FALSE;
			LED_5 = FALSE;
			LED_6 = FALSE;
			LED_7 = FALSE;
			LED_8 = FALSE;
			LED_9 = FALSE;
			LED_10 = FALSE;
			LED_11 = FALSE;
			LED_12 = FALSE;
			LED_13 = FALSE;
			LED_14 = FALSE;
			LED_15 = FALSE;
	
			leds_compteur++;
			if(leds_compteur > 15)
				leds_compteur = 0;
			
			switch(leds_compteur)
			{
				case(0):	LED_0 = TRUE;	break;
				case(1):	LED_1 = TRUE;	break;
				case(2):	LED_2 = TRUE;	break;
				case(3):	LED_3 = TRUE;	break;
				case(4):	LED_4 = TRUE;	break;
				case(5):	LED_5 = TRUE;	break;
				case(6):	LED_6 = TRUE;	break;
				case(7):	LED_7 = TRUE;	break;
				case(8):	LED_8 = TRUE;	break;
				case(9):	LED_9 = TRUE;	break;
				case(10):	LED_10 = TRUE;	break;
				case(11):	LED_11 = TRUE;	break;
				case(12):	LED_12 = TRUE;	break;
				case(13):	LED_13 = TRUE;	break;
				case(14):	LED_14 = TRUE;	break;
				case(15):	LED_15 = TRUE;	break;
				default:					break;
			}		
		}
	#endif //def MODE_TEST_DIODES

	#ifdef MODE_RECEPTION_SIMPLE
		void process_affichage_leds_reception_simple(void)
		{
			//Cette fonction peut être appelée à très haute vitesse lors d'un test de réception simple.
			LED_0 = !TSOP_0;
			LED_1 = !TSOP_1;
			LED_2 = !TSOP_2;
			LED_3 = !TSOP_3;
			LED_4 = !TSOP_4;
			LED_5 = !TSOP_5;
			LED_6 = !TSOP_6;
			LED_7 = !TSOP_7;
			LED_8 = !TSOP_8;
			LED_9 = !TSOP_9;
			LED_10 = !TSOP_10;
			LED_11 = !TSOP_11;
			LED_12 = !TSOP_12;
			LED_13 = !TSOP_13;
			LED_14 = !TSOP_14;
			LED_15 = !TSOP_15;
		}
	#endif

#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/
