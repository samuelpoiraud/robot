/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi'Tech
 *
 *	Fichier : Beacon_it.c
 *	Package : Balises
 *	Description : Fonctions d'exploitation des balises - code éxécuté en IT
 *	Auteur : Nirgal
 *	Version 200910
 */

 
#define BEACON_IT_C

#include "Beacon_it.h"
		

	
/*
{

ALGO 2010 TEST NIRGAL...
(activation des détections IT UP ou DOWN non notées ici, mais évidentes !)


IT1_détection_ON
	si timer3 > 3/4tour moteur 
		sauvegarde de timer3, validation des détections, lancement des calculs, raz timer3 && tout le reste !
	sinon 
		raz timer2
IT1_detection_OFF
	si timer 2 > 300µs, 
		validation de la détection, on passe à la détection suivante si elle existe !
	sinon
		t1last = timer3 //maj durée de détection en cours !

IT2_detection_ON
	si (timer3 - derniere detection off > 300µs   &&   derniere_detection_off != -1)
		validation de la détection ! passage à la suivante
	sinon
		si début détection n'existe pas (-1), alors création du début et attribution
IT2_detection_OFF
	si (timer3 - derniere detection on > 300µs)
		PROBLEME ! trop longtemps ON ! ......> mode erreur !
	sinon
		maj fin de détection courante

INSTANT 0...
	sauvegarde du timer3
	recherche des durées les plus longues de chaque tableau, et détermination des bons...
	
	
	
	si l'it1 a sa durée la plus longue qui n'est pas la première durée, 
		ET (que les dernières détections sur it2 et it3 sont non validables (durée <300µs de maintenant)
			OU que l'une des différences est négative)
	alors on bazarde et on met le timer3 avec pour valeur le début de cette détection... il faudra aussi bazarder la suivante !!!!!!
		

	on a nos trois instants de détection
	
}
*/	
	//cette fonction est appelée automatiquement quand les it externes sont détectées (fronts montant et descendants.....)
	
	



	
	void initialiser_detections(void)
	{
		Uint8 balise,j;
		for(balise=0;balise<NOMBRE_BALISES;balise++)
		{
			for(j=0;j<NOMBRE_DETECTIONS_PAR_BALISE;j++)	
			{
				global.detections[balise][j].debut = -1;
				global.detections[balise][j].fin = -1;
			}
			global.index_detection[balise] = 0;
		}
	}	
	
	void initialiser_cycle(Uint16 timer_initial)
	{
		initialiser_detections();
		TIMER3_run(200);
		TMR1 = timer_initial;	
		//TODO attention au cas où on initialise le timer3 à l'instant de début d'une autre détection que la première !
		
		#ifdef TEST_OSCILLO_4_VOIES
			test_oscillo_4voies(TRUE);
		#endif

	}	
	
	#ifdef TEST_OSCILLO_4_VOIES
		
		void test_oscillo_4voies(Uint8 init)
		{
			extern volatile Uint32 moyenne_instants[3], duree_cycle_moyen;
			Uint16 duree;
			static Uint8 state;
			static bool_e on;
			if(init)
				state = 0;
			else
				state++;
	
			switch(state)
			{
				case 0:
					on = TRUE;
					duree = 40;
				break;
				case 1:
					on = FALSE;
					duree = moyenne_instants[0]/NOMBRE_CYCLES_SAUVES - 40;
				break;
				case 2:
					on = TRUE;
					duree = 20;
				break;
				case 3:
					on = FALSE;
					duree = (moyenne_instants[1] - moyenne_instants[0])/NOMBRE_CYCLES_SAUVES - 20;
				break;
				case 4:
					on = TRUE;
					duree = 20;
				break;
				case 5:
					on = FALSE;
					duree = (moyenne_instants[2] - moyenne_instants[1])/NOMBRE_CYCLES_SAUVES - 20;
				break;
				case 6:
					on = TRUE;
					duree = 20;
				break;
				case 7:
					on = FALSE;
					duree = 8000;//(duree_cycle_moyen - moyenne_instants[2])/NOMBRE_CYCLES_SAUVES - 20;
				break;
				case 8:
					duree = 8000;
				default:
				break;
			}	
			
	
			PORTEbits.RE3 = on;
	
			TRISEbits.TRISE3 = 0;
	
			OpenTimer3
				(
				T3_ON &					/* Timer3 ON */ 
				T3_IDLE_CON &			/* operate during sleep */
				T3_GATE_OFF &			/* Timer Gate time accumulation disabled */
				T3_PS_1_256 &			/* Prescaler 1:256 */
				T3_SOURCE_INT,			/* Internal clock source */
				duree
				);	/* periode en us */
			EnableIntT3;
		}
	
	#endif		
		
	void fin_de_cycle(void)
	{
		/*
		sauvegarde du timer3
	recherche des durées les plus longues de chaque tableau, et détermination des bons...
		
	si l'it1 a sa durée la plus longue qui n'est pas la première durée, 
		ET (que les dernières détections sur it2 et it3 sont non validables (durée <300µs de maintenant)
			OU que l'une des différences est négative)
	alors on bazarde et on met le timer3 avec pour valeur le début de cette détection... il faudra aussi bazarder la suivante !!!!!!
		

	on a nos trois instants de détection

	*/
		Uint16 timer_initial;
		Uint16 timer3_save;
		Uint8 balise,idetection;
		Uint16 duree, duree_max;
		Sint8 detection_principale[NOMBRE_BALISES];
		
		timer3_save = TMR1;
		
		for(balise=0;balise<NOMBRE_BALISES;balise++)
		{
			detection_principale[balise] = -1;
			duree_max = 0;
			idetection = 0;
			while((global.detections[balise][idetection].debut != -1 || global.detections[balise][idetection].fin != -1) && idetection < 4)
			{
				duree = global.detections[balise][idetection].fin - global.detections[balise][idetection].debut;
				if(duree > duree_max)
				{
					duree_max = duree;
					detection_principale[balise] = idetection;
				}	
				idetection++;	
			}	
			if(detection_principale[balise] == -1)
				nop();//TODO attention, pas de détection > poubelle !	
		}	

		//TODO ne faire la ligne suivante que si on a pas d'erreurs !!!!!!		
		sauvegardes_instants_detections_balises
				(
					(global.detections[0][detection_principale[0]].fin + global.detections[0][detection_principale[0]].debut)/2,
					(global.detections[1][detection_principale[1]].fin + global.detections[1][detection_principale[1]].debut)/2,
					(global.detections[2][detection_principale[2]].fin + global.detections[2][detection_principale[2]].debut)/2,
					timer3_save
				);
				//sauvegarde des données récupérées dans le buffer....
				
		//On arrete ici pour l'it, le reste sera fait en tâche de fond !
		global.flag_nouveau_cycle = TRUE;
	
	/*	if(detection_principale[0] != 0)		//balise 0 mal synchronisée
			timer_initial = detections[0][detection_principale[0]].debut;	//TODO ajouter un autre test pour assouplir celui ci, cf commentaires de l'algo !
		else
	*/		timer_initial = 0;

		initialiser_cycle(timer_initial);		//cycle suivant !
	}
	

	

	
	//Attention, il y a une différence de type entre le Sint16 des instants de détection, et les instants présentés ici. pas de problèmes en pratique.
	void sauvegardes_instants_detections_balises(Uint16 instant_balise0, Uint16 instant_balise1, Uint16 instant_balise2, Uint16 duree_cycle)
	{
		
		//cette fonction mémorise le dernier cycle dans un buffer circulaire
		
		global.index_cycle = (global.index_cycle + 1)%NOMBRE_CYCLES_SAUVES;
		
		global.buffer_instants[0][global.index_cycle] = instant_balise0;
		global.buffer_instants[1][global.index_cycle] = instant_balise1;
		global.buffer_instants[2][global.index_cycle] = instant_balise2;
		global.buffer_instants[3][global.index_cycle] = duree_cycle;
	}		
		

	
	void it_on(Uint8 balise)
	{
		if(balise == 0)
		{
			//Cas de la balise 0 de synchronisation !!!
				if(TMR1 > DUREE_MINI_TOUR_MOTEUR)
				{
					//GROS TRAITEMENT DE FOU	
					fin_de_cycle();
				}	
		}	
		
		
		if(global.detections[balise][global.index_detection[balise]].fin != -1)			//si un off a déjà été rencontré
		{
			if(TMR1 - global.detections[balise][global.index_detection[balise]].fin > DUREE_MINI_ENTRE_DEUX_DETECTIONS)	//et que c'était il y a plus de 300µs
			{
				global.index_detection[balise]++;							//validation de la détection !
				global.detections[balise][global.index_detection[balise]].debut = TMR1;	//Maj instant début de détection.
			}	
			//sinon, rien de spécial, on est pendant une détection !
		}
		else	//c'est notre première détection depuis le début du cycle !!! champomy !
			global.detections[balise][global.index_detection[balise]].debut = TMR1;		//Maj instant début de détection.
				
	}
	
	void it_off(Uint8 balise)
	{
		//maj position courante...
		global.detections[balise][global.index_detection[balise]].fin = TMR1;				
	}
	/*

	void _ISR _INT1Interrupt(void)
	{		
		if(_INT1EP)
		{								//IT1 descente  - détection ON
		//	LED_USER = 0;
			EXTERN_IT_enable(INT1,0);	
			it_on(0);	
		}
		else 	
		{	//IT1 montante - détection OFF
		//	LED_USER = 1;
			EXTERN_IT_enable(INT1,1); //on s'attends à la prochaine IT descendante !
			it_off(0);
		}	
		_INT1IF = 0;	
	}	
		
	
	void _ISR _INT2Interrupt(void)
	{	
		if(_INT2EP)
		{								//IT2 descente - détection ON
			LED_USER2 = 0;
			EXTERN_IT_enable(INT2,0);
			it_on(1);				
		}	
		else 	
		{	//IT2 montante - détection OFF
			LED_USER2 = 1;
			EXTERN_IT_enable(INT2,1); //on s'attends à la prochaine IT descendante !
			it_off(1);
		}	
		_INT2IF = 0;	
	}	
	
	
	void _ISR _INT3Interrupt(void)
	{
		if(_INT3EP)
		{	//IT3 descente  - détection ON
			LED_UART = 0;
			EXTERN_IT_enable(INT3,0);	
			it_on(2);
		}
		else 	
		{	//IT3 montante - détection OFF
			LED_UART = 1;
			EXTERN_IT_enable(INT3,1); //on s'attends à la prochaine IT descendante !
			it_off(2);
		}	
		_INT3IF = 0;	
	}	
	
	
	void _ISR _T3Interrupt()
	{
		//debug_printf("\ntimer3!!!\n");
		global.flags.timer3 = TRUE;
		IFS0bits.T3IF	= FALSE;
	}	
	

	void _ISR _T2Interrupt()
	{
		global.flags.timer2 = TRUE;
		IFS0bits.T2IF = FALSE;
	}
*/

	#ifdef TEST_OSCILLO_4_VOIES
			
		void _ISR _T3Interrupt()
		{
			
			test_oscillo_4voies(FALSE);
			IFS0bits.T3IF = FALSE;
		}
	#endif
			
	/*CODE 2009..................................
	
	
	
//dans l'initialisation :
//		global.temps12 = 0;
//		global.temps23 = 0;
//		global.temps31 = 0;
//		global.somme=0;
//		global.compteur_interruption = 1;
//		global.moy_x =0;
//		global.moy_y=0;
//		global.compteur_precision = 0;


void _ISR _INT1Interrupt(void)
	{
		
		if(_INT1EP)
		{								//IT1 descente
			LED_1 = 0;
			IT_Externe_Enable(INT1,0);	
			
			if(global.compteur_interruption==1)		//On détecte pour la première fois avec la balise 1
			{
				//On a enfin vu le spot avec la balise 1, on valide la durée 3->1

				global.temps23 = global.temps23_partiel + global.duree_detection3 / 2;	//La donnée temps23 est enfin prête !
				global.flag_envoi2 = TRUE;	//On valide la donnée 23
				
				global.temps31_partiel = TMR1 - global.duree_detection3 / 2 ;	//On commence a préparer la donnée 31
				
				global.flags.it = TRUE;
				
				
				global.compteur_interruption = 2; //J'attends l'it 2... sauf si la 1 se pointe encore
				TIMER3_run(50);		//50 : a priori, c'est qu'on a un problème si pendant la durée d'environ 1 tour, on a pas détecté la balise suivante.
			}	
			else		
			{
				//Ce n'est pas la première fois avec la balise 1... on attends toujours la détection balise 2.
				nop();	//Pour l'instant, on ne fait rien dans ce cas là
				
			}	
				
		}
		
	
		else 	
		{	//IT1 montante
			LED_1 = 1;
			IT_Externe_Enable(INT1,1); //on s'attends à la prochaine IT descendante !
			//Sauvegarde de la valeur de TIMER3 
			global.duree_detection1 = TMR3;
		}	
		_INT1IF = 0;	
	}	
	
	
	
	
	void _ISR _INT2Interrupt(void)
	{
		
		if(_INT2EP)
		{								//IT2 descente
			LED_2 = 0;
			IT_Externe_Enable(INT2,0);	
			
			if(global.compteur_interruption==2)		//On détecte pour la première fois avec la balise 2
			{
				//On a enfin vu le spot avec la balise 2, on valide la durée 1->2

				global.temps31 = global.temps31_partiel + global.duree_detection1 / 2;	//La donnée temps31 est enfin prête !
				global.flag_envoi3 = TRUE;	//On valide la donnée 31
				
				global.temps12_partiel = TMR3 - global.duree_detection1 / 2 ;	//On commence a préparer la donnée 12
				
				global.flags.it = TRUE;
				
				
				global.compteur_interruption = 3; //J'attends l'it 3... sauf si la 2 se pointe encore
				TIMER3_run(50);		//50 : a priori, c'est qu'on a un problème si pendant la durée d'environ 1 tour, on a pas détecté la balise suivante.
			}	
			else		
			{
				//Ce n'est pas la première fois avec la balise 2... on attends toujours la détection balise 3.
				nop();	//Pour l'instant, on ne fait rien dans ce cas là
				
			}	
				
		}
		
	
		else 	
		{	//IT2 montante
			LED_2 = 1;
			IT_Externe_Enable(INT2,1); //on s'attends à la prochaine IT descendante !
			//Sauvegarde de la valeur de TIMER3 
			global.duree_detection2 = TMR3;
		}	
		_INT2IF = 0;	
	}	
	
	
	void _ISR _INT3Interrupt(void)
	{
		
		if(_INT3EP)
		{								//IT3 descente
			LED_3 = 0;
			IT_Externe_Enable(INT3,0);	
			
			if(global.compteur_interruption==3)		//On détecte pour la première fois avec la balise 3
			{
				//On a enfin vu le spot avec la balise 3, on valide la durée 2->3

				global.temps12 = global.temps12_partiel + global.duree_detection2 / 2;	//La donnée temps12 est enfin prête !
				global.flag_envoi1 = TRUE;	//On valide la donnée 12
				
				global.temps23_partiel = TMR3 - global.duree_detection2 / 2 ;	//On commence a préparer la donnée 23
				
				global.flags.it = TRUE;
					
				global.compteur_interruption = 1; //J'attends l'it 1... sauf si la 3 se pointe encore
				TIMER3_run(50);		//50 : a priori, c'est qu'on a un problème si pendant la durée d'environ 1 tour, on a pas détecté la balise suivante.
			}	
			else		
			{
				//Ce n'est pas la première fois avec la balise 3... on attends toujours la détection balise 1.
				nop();	//Pour l'instant, on ne fait rien dans ce cas là
				
			}	
				
		}
		
	
		else 	
		{	//IT3 montante
			LED_3 = 1;
			IT_Externe_Enable(INT3,1); //on s'attends à la prochaine IT descendante !
			//Sauvegarde de la valeur de TIMER3 
			global.duree_detection3 = TMR3;
		}	
		_INT3IF = 0;	
	}	
	
	
	*/
