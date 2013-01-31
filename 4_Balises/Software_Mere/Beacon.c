/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi'Tech
 *
 *	Fichier : Beacon.c
 *	Package : Balises
 *	Description : Fonctions d'exploitation des balises - code éxécuté en tache de fond
 *	Auteur : Nirgal
 *	Version 200910
 */

 
#define BEACON_C

#include "Beacon.h"
		

volatile Sint16 valeur_a_envoyer_x;
volatile Sint16 valeur_a_envoyer_y;
volatile Sint16 current_x;
volatile Sint16 current_y;
//Les 3 angles entre les balises
volatile Uint8 angles[3];

void Beacon_init(void)
{
		initialiser_instants_detections_balises();
		initialiser_detections();
		global.flag_nouveau_cycle = FALSE;	

		global.flag_envoi = FALSE;

		global.color = 0;
		
		current_x = 0;
		current_y = 0;
		valeur_a_envoyer_x = 0;
		valeur_a_envoyer_y = 0;


		TIMER3_run(200);
		TIMER2_run(250);	//timer d'envoi vers carte P...
		EXTERN_IT_enable(INT1,1);
		EXTERN_IT_enable(INT2,1);
		EXTERN_IT_enable(INT3,1);
}

	void initialiser_instants_detections_balises(void)	
	{
		Uint8 balise, cycle;
		for(balise = 0;balise<NOMBRE_BALISES;balise++)
			for(cycle = 0;cycle<NOMBRE_CYCLES_SAUVES;cycle++)
				global.buffer_instants[balise][cycle] = 0;
		global.index_cycle = 0;
	}
	
	
	bool_e test_moyenne_xy(void)
	{
		static Sint32 x,y;
		static Uint8 i;
		
		i = (i + 1) % 16;
		x += current_x;
		y += current_y;
		if(!i)
		{
			x/=160;
			y/=160;
			debug_printf("x:%d,y:%d\n",(Sint16)x,(Sint16)y);
			x = 0;
			y = 0;
		}	
		
		if(!i)
			return TRUE;
		else
			return FALSE;
	}
	
		
	
	void process_beacon(void)
	{
		bool_e valid = TRUE;
		
		//cette fonction, exécutée très régulièrement en tache de fond, 
		// réalise les calculs nécessaire à l'issu de chaque cycle !
		if(global.flag_nouveau_cycle == TRUE)
		{
			//un nouveau cycle est apparu.
			global.flag_nouveau_cycle = FALSE;
			
			//calcul des angles d'après les mesures brutes
			calcul_angles();
				//debug_printf("a:%d,b:%d\n",(Uint8)angles[0],(Uint8)angles[2]);
			
			valid &= detection_erreur_angles();
			
			//calculs des coordonnées xy
			calcul_xy();
				//debug_printf("x:%d,y:%d\n",current_x,current_y);
			
			valid &= detection_erreur_xy();
				
			valid &= test_moyenne_xy();
			
			if(valid == TRUE)
				global.flag_envoi = TRUE;
		}		
	}

	volatile Uint32 moyenne_instants[3], duree_cycle_moyen;
	
	void calcul_angles(void)
	{
		
		//moyenne instant contiendra la somme des instants de détection sur tout les cycles mémorisés....
		//comme un moyenne sans division.
		//La durée du cycle moyen est la somme des durées enregistrées.
		
		
		Uint8 icycle, balise;
		//Calcul de la durée moyenne des cycles... (combien fait un tour moteur !)
		duree_cycle_moyen = 0;
		for(icycle=0;icycle<NOMBRE_CYCLES_SAUVES;icycle++)
			duree_cycle_moyen += (Uint32)(global.buffer_instants[NOMBRE_BALISES][icycle]);
		//ATTENTION, cette moyenne est faite sans la division...
//		debug_printf("duree_cycle %li\n",duree_cycle_moyen);
		//TODO peut etre faudrait il plutot faire des médiannes ???		
		
		for(balise = 0;balise < NOMBRE_BALISES;balise++)
		{
			moyenne_instants[balise] = 0;
			for(icycle=0;icycle<NOMBRE_CYCLES_SAUVES;icycle++)
				moyenne_instants[balise] += (Uint32)(global.buffer_instants[balise][icycle]);

			//ATTENTION, cette moyenne est faite sans la division par le nombre de valeurs..
			//il est important faire le *128 avant la division par la durée du cycle !
		}
		//TODO détection d'erreur......
		//TODO IMPORTANT : si l'une des valeur est -1, ne pas la prendre en compte, si toutes les valeurs sont -1, ERREUR !!!

		angles[0] = (Uint8)(((moyenne_instants[1] - moyenne_instants[0])*128)/duree_cycle_moyen);
		angles[1] = (Uint8)(((moyenne_instants[2] - moyenne_instants[1])*128)/duree_cycle_moyen);
		angles[2] = (Uint8)128 - angles[0] - angles[1];
//		debug_printf("moyenne instant 1 %li\n",moyenne_instants[1]);
//		debug_printf("moyenne instant 2 %li\n",moyenne_instants[2]);
//		debug_printf("moyenne instant 0 %li\n",moyenne_instants[0]);
//		debug_printf("duree_cycle %d\n",(int)duree_cycle_moyen);
	}	
		
		
	bool_e detection_erreur_angles(void)
	{
		//	if(angles[0] > )
		//TODO !!!!
	
		return TRUE;
	}
	
		
	void calcul_xy(void)
	{
		#ifdef ACTIVER_TABLEAU_C //Le désactiver permet d'avoir un programme plus léger lorsqu'on réalise des tests qui n'ont rien à voire avec cette fonctionnalité !
		current_x = find_x(angles[2],angles[0]);
		current_y = find_y(angles[2],angles[0]);
		#endif
	}
	
	
	bool_e detection_erreur_xy(void)
	{
		
		if(current_x > 3500)
			return FALSE;
		if(current_y > 2500)
			return FALSE;
		if(current_x < -500)
			return FALSE;	
		if(current_y < -500)
			return FALSE;
			
				
		return TRUE;
	}


	
void process_envoi(void)
{
	CAN_msg_t other_can_msg;
		
	global.flags.timer2=0; //Flag timer envoi carte P.

	global.flag_envoi = FALSE;


	if(global.color==0)   // si nous sommes de couleur verte
	{
		valeur_a_envoyer_x = current_x;//global.Medx[9];
		valeur_a_envoyer_y = current_y;//global.Medy[9];
	}
	else
	{	// si nous sommes de couleur rouge
		valeur_a_envoyer_x = current_x - 2000;//global.Medx[9]-2000;
		valeur_a_envoyer_y = current_y - 3000;//global.Medy[9]-3000;					
	}
	
	#ifndef BEACON_ADVERSARY_POSITION_US_AROUND
		#define BEACON_ADVERSARY_POSITION_US_AROUND 0x252
		#warning "mettre ceci en QS"
	#else
		#warning "virer ceci"
	#endif
	other_can_msg.sid=BEACON_ADVERSARY_POSITION_US_AROUND;
	other_can_msg.data[0] = (Uint8)(valeur_a_envoyer_x>>8);
	other_can_msg.data[1] = (Uint8)(valeur_a_envoyer_x);
	other_can_msg.data[2] = (Uint8)(valeur_a_envoyer_y>>8);
	other_can_msg.data[3] = (Uint8)(valeur_a_envoyer_y);
	other_can_msg.size = 4;

	#ifdef ENVOYER_SUR_UART2_COMME_MSG_CAN
		CANmsgToU2tx(&other_can_msg);
	#endif
	//Attention, ceci n'est pas forcément un debug_printf.. c'est l'usage normal du code balise qui envoie des données sur uart !
	#ifdef ENVOYER_SUR_UART1
		printf("x:%d ",valeur_a_envoyer_x);
		printf("y:%d\n",valeur_a_envoyer_y);
	#endif			
}
//	volatile Sint16 temps12, temps23, temps31;	//Intervales de temps entre les détections
//	volatile Sint32 somme;					//somme des trois temps de détections

/*	
void process_it(void)
{
	global.flags.it = FALSE;
	//Une valeur trouvée, mise a jour de la somme et des angles...
	somme = temps12 + temps23 + temps31;
	if(somme) //Eviter les divisions par 0 !!!
	{
		//	debug_printf("somme:%d\n ",somme);
		if(somme<3500)
		{
			global.angle_a = (Sint16)(((Sint32)temps31)*128/somme);
			global.angle_b = (Sint16)(((Sint32)temps12)*128/somme);
						
								
			current_x = find_x(global.angle_a,global.angle_b);
			current_y = find_y(global.angle_a,global.angle_b);
						
			
			if(global.compteur_precision < TAILLE_TABLEAU_BUBBLE_SORT) 
			{		
				global.tabx[global.compteur_precision] = current_x;	
				global.taby[global.compteur_precision] = current_y;
		
				global.moy_x=0;
				global.moy_y=0;
				for(global.i=0; global.i < TAILLE_TABLEAU_BUBBLE_SORT; global.i++)
				{
					global.Medx[global.i] = global.tabx[global.i];
				 	global.Medy[global.i] = global.taby[global.i];
				}
								
				bubblesort();
				#ifdef AFFICHER_POINT_DU_BUBBLE_DE_TEMPS_EN_TEMPS
				
					static Uint16 compteur_nombre_de_passages;
					compteur_nombre_de_passages++;
					if(compteur_nombre_de_passages > 200)
					{
						IT_Externe_Disable(INT1);
						IT_Externe_Disable(INT2);
						IT_Externe_Disable(INT3);
						printf("sum %ld\n",somme);
						compteur_nombre_de_passages = 0;
						for(global.i=0; global.i < TAILLE_TABLEAU_BUBBLE_SORT; global.i++)
						{
							debug_printf("x%d:%d ",global.i,global.Medx[global.i]/10);
						 	debug_printf("y%d:%d\n ",global.i,global.Medy[global.i]/10);
						}
						IT_Externe_Enable(INT1,1);
						IT_Externe_Enable(INT2,1);
						IT_Externe_Enable(INT3,1);
					}	
				#endif							
				global.compteur_precision++;
			}
						
							
			if (global.compteur_precision >= TAILLE_TABLEAU_BUBBLE_SORT)  
				global.compteur_precision = 0;
			
		}
	}
}
*/


/*	
	void initialiser bubblesort
	{
		//		int i;
		for(i=0;i<TAILLE_TABLEAU_BUBBLE_SORT;i++)
		{
			global.tabx[i]=0;
			global.taby[i]=0;
			global.Medx[i]=0;
			global.Medy[i]=0;
		}
		
		}


	void bubblesort(void)
	{
		Uint16 i;
		bool_e flag_bubble;
		Sint16 temp;
		do
		{
			flag_bubble = FALSE;
			for(i=1;i<TAILLE_TABLEAU_BUBBLE_SORT;i++)
			{
				if(global.Medx[i-1]<global.Medx[i])
				{
					temp = global.Medx[i-1];
					global.Medx[i-1] = global.Medx[i];
					global.Medx[i] = temp;
					flag_bubble = TRUE;
				}	
			}		
		}while(flag_bubble==TRUE);
		
		do
		{
			flag_bubble = FALSE;
			for(i=1;i<TAILLE_TABLEAU_BUBBLE_SORT;i++)
			{
				if(global.Medy[i-1]<global.Medy[i])
				{
					temp = global.Medy[i-1];
					global.Medy[i-1] = global.Medy[i];
					global.Medy[i] = temp;
					flag_bubble = TRUE;
				}	
			}		
		}while(flag_bubble==TRUE);		
	}

*/


