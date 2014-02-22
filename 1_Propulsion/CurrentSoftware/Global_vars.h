/*
 *  Club Robot ESEO 2008 - 2011
 *  Archi-Tech',Chomp,CheckNorris
 *
 *  Fichier : Global_vars.h
 *  Package : Standard_Project
 *  Description : Variables globales d�finies specifiquement pour
					le code de la carte l'executant.
 *  Auteur : Jacen (inspir� du package QS d'Axel Voitier) - Nirgal 2009
 *  Version 201101
 */

#ifndef GLOBAL_VARS_H
	#define GLOBAL_VARS_H

	#ifndef QS_GLOBAL_VARS_H
		#error "Il est interdit d'inclure directement Global_vars.h, lire le CQS."
	#endif

	/*  Note : Variables globales communes � tous les codes
	 *
	 *	Buffer de reception de l'UART 1
	 *	Uint8 u1rxbuf[UART_RX_BUF_SIZE];
	 *	Position du dernier octet recu
	 *	volatile Uint32 u1rxnum;
	 *
	 *	Buffer de reception de l'UART 2
	 *	Uint8 u2rxbuf[UART_RX_BUF_SIZE];
	 *	Position du dernier octet recu
	 *	volatile Uint32 u2rxnum;
	 *
	 *	Buffer de reception des messages du bus CAN
	 *	CAN_msg_t can_buffer[CAN_BUF_SIZE];
	 *	Position du dernier message recu
	 *	volatile Uint16 can_rx_num;
	 *
	 *	Buffer de reception des messages du bus CAN2
	 *	CAN_msg_t can2_buffer[CAN_BUF_SIZE];
	 *	Position du dernier message recu
	 *	volatile Uint16 can2_rx_num;
	 *
	 *	Tableau des r�sulats du dernier echantillonage.
	 *	Uint16 ADCResult[16];
	 *
	 */
		volatile bool_e flag_recouvrement_IT;

		///REFERENTIEL GENERAL/////////(x, y, teta...)///////////////////

		//Position actuelle du robot (x, y, teta)
		//Dans le r�f�rentiel G�n�ral
		volatile position_t position;
		// x 		[mm]
		// y 		[mm]
		// teta 	[rad/4096]		<<12


		///REFERENTIEL IT///////////////////////////////////////////

				/*
				 TODO reconsid�rer l'id�e
				 typedef struct
				{
					Sint32 translation;
					Sint32 rotation;
				}polar_t;
		//////////////////////////////////////////////////////////
				polar_t real_position;*/
		//////////////////////////////////////////////////////////
		 //MESURES...

		//Vitesse r�elle du robot mesur�e pour 5ms
		volatile Sint32 real_speed_translation;		//[mm/4096/5ms]		<<12 /5ms
		volatile Sint32 real_speed_rotation;		// [rad/4096/1024/5ms]	<<22

		//Distance et Angle r�ellement parcourue par le robot mesur�e pour 5ms
		//Dans le r�f�rentiel IT
		volatile Sint32 real_position_translation;	//[mm/4096]  <<12
		volatile Sint32 real_position_rotation;	//[rad/4096/1024] <<22

		//Ces �carts correspondent � la diff�rence entre les coordonn�es du point fictif et les coordonn�es r�elles du robot.
		volatile Sint32 ecart_translation;		//[mm/4096]  <<12
		volatile Sint32 ecart_rotation;			//[rad/4096] <<12
		volatile Sint32 ecart_translation_prec;	//[mm/4096]  <<12
		volatile Sint32 ecart_rotation_prec;	//[rad/4096] <<12

		//////////////////////////////////////////////////////////

		// Caract�ristiques de la trajectoire en cours... Acc, Vit, Pos

		//Acc�l�rations, vitesses, positions DU POINT FICTIF calcul�es par la gestion des trajectoires

		volatile Sint32 acceleration_translation;	//[mm/4096/5ms/5ms]
		volatile Sint32 vitesse_translation;		//[mm/4096/5ms]
		volatile Sint32 position_translation; 		//[mm/4096]


		volatile Sint32 acceleration_rotation;		//[rad/4096/1024/5ms/5ms]
		volatile Sint32 vitesse_rotation;			//[rad/4096/1024/5ms]
		volatile Sint32 position_rotation;			//[rad/4096]


			//derni�res vitesse avant erreur
		volatile Sint32 vitesse_translation_erreur;		//[mm/5ms]   /5ms
		volatile Sint32 vitesse_rotation_erreur;		// [rad/1024]   <<10

		//////////////////////////////////////////////////////////

		// Etat alimentation

		volatile bool_e alim;
		volatile Uint16 alim_value;			// en mV



#endif /* ndef GLOBAL_VARS_H */
