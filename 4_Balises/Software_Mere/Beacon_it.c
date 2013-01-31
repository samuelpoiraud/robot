/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi'Tech
 *
 *	Fichier : Beacon_it.c
 *	Package : Balises
 *	Description : Fonctions d'exploitation des balises - code �x�cut� en IT
 *	Auteur : Nirgal
 *	Version 200910
 */

 
#define BEACON_IT_C

#include "Beacon_it.h"
		

	
/*
{

ALGO 2010 TEST NIRGAL...
(activation des d�tections IT UP ou DOWN non not�es ici, mais �videntes !)


IT1_d�tection_ON
	si timer3 > 3/4tour moteur 
		sauvegarde de timer3, validation des d�tections, lancement des calculs, raz timer3 && tout le reste !
	sinon 
		raz timer2
IT1_detection_OFF
	si timer 2 > 300�s, 
		validation de la d�tection, on passe � la d�tection suivante si elle existe !
	sinon
		t1last = timer3 //maj dur�e de d�tection en cours !

IT2_detection_ON
	si (timer3 - derniere detection off > 300�s   &&   derniere_detection_off != -1)
		validation de la d�tection ! passage � la suivante
	sinon
		si d�but d�tection n'existe pas (-1), alors cr�ation du d�but et attribution
IT2_detection_OFF
	si (timer3 - derniere detection on > 300�s)
		PROBLEME ! trop longtemps ON ! ......> mode erreur !
	sinon
		maj fin de d�tection courante

INSTANT 0...
	sauvegarde du timer3
	recherche des dur�es les plus longues de chaque tableau, et d�termination des bons...
	
	
	
	si l'it1 a sa dur�e la plus longue qui n'est pas la premi�re dur�e, 
		ET (que les derni�res d�tections sur it2 et it3 sont non validables (dur�e <300�s de maintenant)
			OU que l'une des diff�rences est n�gative)
	alors on bazarde et on met le timer3 avec pour valeur le d�but de cette d�tection... il faudra aussi bazarder la suivante !!!!!!
		

	on a nos trois instants de d�tection
	
}
*/	
	//cette fonction est appel�e automatiquement quand les it externes sont d�tect�es (fronts montant et descendants.....)
	
	



	
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
		//TODO attention au cas o� on initialise le timer3 � l'instant de d�but d'une autre d�tection que la premi�re !
		
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
	recherche des dur�es les plus longues de chaque tableau, et d�termination des bons...
		
	si l'it1 a sa dur�e la plus longue qui n'est pas la premi�re dur�e, 
		ET (que les derni�res d�tections sur it2 et it3 sont non validables (dur�e <300�s de maintenant)
			OU que l'une des diff�rences est n�gative)
	alors on bazarde et on met le timer3 avec pour valeur le d�but de cette d�tection... il faudra aussi bazarder la suivante !!!!!!
		

	on a nos trois instants de d�tection

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
				nop();//TODO attention, pas de d�tection > poubelle !	
		}	

		//TODO ne faire la ligne suivante que si on a pas d'erreurs !!!!!!		
		sauvegardes_instants_detections_balises
				(
					(global.detections[0][detection_principale[0]].fin + global.detections[0][detection_principale[0]].debut)/2,
					(global.detections[1][detection_principale[1]].fin + global.detections[1][detection_principale[1]].debut)/2,
					(global.detections[2][detection_principale[2]].fin + global.detections[2][detection_principale[2]].debut)/2,
					timer3_save
				);
				//sauvegarde des donn�es r�cup�r�es dans le buffer....
				
		//On arrete ici pour l'it, le reste sera fait en t�che de fond !
		global.flag_nouveau_cycle = TRUE;
	
	/*	if(detection_principale[0] != 0)		//balise 0 mal synchronis�e
			timer_initial = detections[0][detection_principale[0]].debut;	//TODO ajouter un autre test pour assouplir celui ci, cf commentaires de l'algo !
		else
	*/		timer_initial = 0;

		initialiser_cycle(timer_initial);		//cycle suivant !
	}
	

	

	
	//Attention, il y a une diff�rence de type entre le Sint16 des instants de d�tection, et les instants pr�sent�s ici. pas de probl�mes en pratique.
	void sauvegardes_instants_detections_balises(Uint16 instant_balise0, Uint16 instant_balise1, Uint16 instant_balise2, Uint16 duree_cycle)
	{
		
		//cette fonction m�morise le dernier cycle dans un buffer circulaire
		
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
		
		
		if(global.detections[balise][global.index_detection[balise]].fin != -1)			//si un off a d�j� �t� rencontr�
		{
			if(TMR1 - global.detections[balise][global.index_detection[balise]].fin > DUREE_MINI_ENTRE_DEUX_DETECTIONS)	//et que c'�tait il y a plus de 300�s
			{
				global.index_detection[balise]++;							//validation de la d�tection !
				global.detections[balise][global.index_detection[balise]].debut = TMR1;	//Maj instant d�but de d�tection.
			}	
			//sinon, rien de sp�cial, on est pendant une d�tection !
		}
		else	//c'est notre premi�re d�tection depuis le d�but du cycle !!! champomy !
			global.detections[balise][global.index_detection[balise]].debut = TMR1;		//Maj instant d�but de d�tection.
				
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
		{								//IT1 descente  - d�tection ON
		//	LED_USER = 0;
			EXTERN_IT_enable(INT1,0);	
			it_on(0);	
		}
		else 	
		{	//IT1 montante - d�tection OFF
		//	LED_USER = 1;
			EXTERN_IT_enable(INT1,1); //on s'attends � la prochaine IT descendante !
			it_off(0);
		}	
		_INT1IF = 0;	
	}	
		
	
	void _ISR _INT2Interrupt(void)
	{	
		if(_INT2EP)
		{								//IT2 descente - d�tection ON
			LED_USER2 = 0;
			EXTERN_IT_enable(INT2,0);
			it_on(1);				
		}	
		else 	
		{	//IT2 montante - d�tection OFF
			LED_USER2 = 1;
			EXTERN_IT_enable(INT2,1); //on s'attends � la prochaine IT descendante !
			it_off(1);
		}	
		_INT2IF = 0;	
	}	
	
	
	void _ISR _INT3Interrupt(void)
	{
		if(_INT3EP)
		{	//IT3 descente  - d�tection ON
			LED_UART = 0;
			EXTERN_IT_enable(INT3,0);	
			it_on(2);
		}
		else 	
		{	//IT3 montante - d�tection OFF
			LED_UART = 1;
			EXTERN_IT_enable(INT3,1); //on s'attends � la prochaine IT descendante !
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
			
			if(global.compteur_interruption==1)		//On d�tecte pour la premi�re fois avec la balise 1
			{
				//On a enfin vu le spot avec la balise 1, on valide la dur�e 3->1

				global.temps23 = global.temps23_partiel + global.duree_detection3 / 2;	//La donn�e temps23 est enfin pr�te !
				global.flag_envoi2 = TRUE;	//On valide la donn�e 23
				
				global.temps31_partiel = TMR1 - global.duree_detection3 / 2 ;	//On commence a pr�parer la donn�e 31
				
				global.flags.it = TRUE;
				
				
				global.compteur_interruption = 2; //J'attends l'it 2... sauf si la 1 se pointe encore
				TIMER3_run(50);		//50 : a priori, c'est qu'on a un probl�me si pendant la dur�e d'environ 1 tour, on a pas d�tect� la balise suivante.
			}	
			else		
			{
				//Ce n'est pas la premi�re fois avec la balise 1... on attends toujours la d�tection balise 2.
				nop();	//Pour l'instant, on ne fait rien dans ce cas l�
				
			}	
				
		}
		
	
		else 	
		{	//IT1 montante
			LED_1 = 1;
			IT_Externe_Enable(INT1,1); //on s'attends � la prochaine IT descendante !
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
			
			if(global.compteur_interruption==2)		//On d�tecte pour la premi�re fois avec la balise 2
			{
				//On a enfin vu le spot avec la balise 2, on valide la dur�e 1->2

				global.temps31 = global.temps31_partiel + global.duree_detection1 / 2;	//La donn�e temps31 est enfin pr�te !
				global.flag_envoi3 = TRUE;	//On valide la donn�e 31
				
				global.temps12_partiel = TMR3 - global.duree_detection1 / 2 ;	//On commence a pr�parer la donn�e 12
				
				global.flags.it = TRUE;
				
				
				global.compteur_interruption = 3; //J'attends l'it 3... sauf si la 2 se pointe encore
				TIMER3_run(50);		//50 : a priori, c'est qu'on a un probl�me si pendant la dur�e d'environ 1 tour, on a pas d�tect� la balise suivante.
			}	
			else		
			{
				//Ce n'est pas la premi�re fois avec la balise 2... on attends toujours la d�tection balise 3.
				nop();	//Pour l'instant, on ne fait rien dans ce cas l�
				
			}	
				
		}
		
	
		else 	
		{	//IT2 montante
			LED_2 = 1;
			IT_Externe_Enable(INT2,1); //on s'attends � la prochaine IT descendante !
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
			
			if(global.compteur_interruption==3)		//On d�tecte pour la premi�re fois avec la balise 3
			{
				//On a enfin vu le spot avec la balise 3, on valide la dur�e 2->3

				global.temps12 = global.temps12_partiel + global.duree_detection2 / 2;	//La donn�e temps12 est enfin pr�te !
				global.flag_envoi1 = TRUE;	//On valide la donn�e 12
				
				global.temps23_partiel = TMR3 - global.duree_detection2 / 2 ;	//On commence a pr�parer la donn�e 23
				
				global.flags.it = TRUE;
					
				global.compteur_interruption = 1; //J'attends l'it 1... sauf si la 3 se pointe encore
				TIMER3_run(50);		//50 : a priori, c'est qu'on a un probl�me si pendant la dur�e d'environ 1 tour, on a pas d�tect� la balise suivante.
			}	
			else		
			{
				//Ce n'est pas la premi�re fois avec la balise 3... on attends toujours la d�tection balise 1.
				nop();	//Pour l'instant, on ne fait rien dans ce cas l�
				
			}	
				
		}
		
	
		else 	
		{	//IT3 montante
			LED_3 = 1;
			IT_Externe_Enable(INT3,1); //on s'attends � la prochaine IT descendante !
			//Sauvegarde de la valeur de TIMER3 
			global.duree_detection3 = TMR3;
		}	
		_INT3IF = 0;	
	}	
	
	
	*/
