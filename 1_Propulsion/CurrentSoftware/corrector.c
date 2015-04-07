/*
 *  Club Robot ESEO 2009 - 2010
 *  Chomp
 *
 *  Fichier : moteur.c
 *  Package : Asser
 *  Description : commande des moteurs
 *  Auteur : Nirgal (2009)
 *  Version 201005
 */



#include "motors.h"
#include "corrector.h"
#include "pilot.h"
#include "QS/QS_who_am_i.h"

volatile bool_e corrector_pd_rotation = TRUE;
volatile bool_e corrector_pd_translation = TRUE;
volatile static Sint32 coefs[PROPULSION_NUMBER_COEFS];

void CORRECTOR_init(void)
{
	if(QS_WHO_AM_I_get()==SMALL_ROBOT)
	{
		//SMALL
		coefs[CORRECTOR_COEF_KP_TRANSLATION] =  SMALL_KP_TRANSLATION;
		coefs[CORRECTOR_COEF_KD_TRANSLATION] = SMALL_KD_TRANSLATION;
		coefs[CORRECTOR_COEF_KV_TRANSLATION] = SMALL_KV_TRANSLATION;
		coefs[CORRECTOR_COEF_KA_TRANSLATION] = SMALL_KA_TRANSLATION;
		coefs[CORRECTOR_COEF_KP_ROTATION] = SMALL_KP_ROTATION;
		coefs[CORRECTOR_COEF_KD_ROTATION] = SMALL_KD_ROTATION;
		coefs[CORRECTOR_COEF_KV_ROTATION] = SMALL_KV_ROTATION;
		coefs[CORRECTOR_COEF_KA_ROTATION] = SMALL_KA_ROTATION;
 	}
 	else
 	{
	 	//BIG
	 	coefs[CORRECTOR_COEF_KP_TRANSLATION] =  BIG_KP_TRANSLATION;
		coefs[CORRECTOR_COEF_KD_TRANSLATION] = BIG_KD_TRANSLATION;
		coefs[CORRECTOR_COEF_KV_TRANSLATION] = BIG_KV_TRANSLATION;
		coefs[CORRECTOR_COEF_KA_TRANSLATION] = BIG_KA_TRANSLATION;
		coefs[CORRECTOR_COEF_KP_ROTATION] = BIG_KP_ROTATION;
		coefs[CORRECTOR_COEF_KD_ROTATION] = BIG_KD_ROTATION;
		coefs[CORRECTOR_COEF_KV_ROTATION] = BIG_KV_ROTATION;
		coefs[CORRECTOR_COEF_KA_ROTATION] = BIG_KA_ROTATION;
	}	 
	MOTORS_reset();	//RAZ des sorties Moteur.
	CORRECTOR_PD_enable(CORRECTOR_ENABLE);
}
	

void CORRECTOR_set_coef(PROPULSION_coef_e coef, Sint32 value)
{
	coefs[coef] = value;
}

Sint32 CORRECTOR_get_coef(PROPULSION_coef_e coef)
{
	return coefs[coef];
}

	//fonction d'activation/d�sactivation du correcteur PD.
	//Rotation = FALSE -> d�sactive la correction en rotation
	//Distance = FALSE -> d�sactive la correction en translation
	//et vice versa...
void CORRECTOR_PD_enable(corrector_e corrector)
{
	//si le pd �tait d�sactiv�, on consid�re que notre destination est notre position actuelle, pour �viter qu'on ne panique et tombe en erreur...
	if(corrector_pd_rotation == FALSE ||  corrector_pd_translation == FALSE)
		PILOT_referential_init();
	switch(corrector)
	{
		case CORRECTOR_ENABLE:
			corrector_pd_rotation = TRUE;
			corrector_pd_translation = TRUE;
		break;
		case  CORRECTOR_ROTATION_ONLY:
			corrector_pd_rotation = TRUE;
			corrector_pd_translation = FALSE;
		break;
		case CORRECTOR_TRANSLATION_ONLY:
			corrector_pd_rotation = FALSE;
			corrector_pd_translation = TRUE;
		break;
		case CORRECTOR_DISABLE:
			corrector_pd_rotation = FALSE;
			corrector_pd_translation = FALSE;
		break;
		default:
		break;
	}
}

bool_e CORRECTOR_PD_enable_get_rotation(void)
{
	return corrector_pd_rotation;
}

bool_e CORRECTOR_PD_enable_get_translation(void)
{
	return corrector_pd_translation;
}

#ifdef MODE_REGLAGE_KV
#include "QS/QS_outputlog.h"
void CORRECTOR_mode_reglage_kv(void)
{

		#warning "vous compilez en mode R�glage KV... Ce mode affiche des printf r�guli�rement qui vous permettrons de r�gler les Kv."
		Sint32 commande_translation;		//[% moteurs]
		Sint32 commande_rotation;			//[% moteurs]
		commande_translation = -(		(global.acceleration_translation						 * coefs[CORRECTOR_COEF_KA_TRANSLATION])	+
										(global.vitesse_translation 							 * coefs[CORRECTOR_COEF_KV_TRANSLATION]) 	+
										(global.ecart_translation 								 * coefs[CORRECTOR_COEF_KP_TRANSLATION])/16 	+
										(global.ecart_translation-global.ecart_translation_prec) * coefs[CORRECTOR_COEF_KD_TRANSLATION]
									  )>>12;

		commande_rotation 	= (			(global.acceleration_rotation							 * coefs[CORRECTOR_COEF_KA_ROTATION])		+
										(global.vitesse_rotation								 * coefs[CORRECTOR_COEF_KV_ROTATION])/2		+
										(global.ecart_rotation 									 * coefs[CORRECTOR_COEF_KP_ROTATION])		+
										((global.ecart_rotation-global.ecart_rotation_prec)		 * coefs[CORRECTOR_COEF_KD_ROTATION])
									  )>>10;

		debug_printf("T:%d Vt*Kvt=%d%% | R:%d Vr*Kvr=%d%%\n",
						(Sint16) commande_translation,
						(Sint16) (((100 * global.vitesse_translation * coefs[CORRECTOR_COEF_KV_TRANSLATION])/((commande_translation)?commande_translation:1))>>12),
						(Sint16) commande_rotation,
						(Sint16) (((100 * global.vitesse_rotation * coefs[CORRECTOR_COEF_KV_ROTATION]/2)/((commande_rotation)?commande_rotation:1))>>10)
					);
			//Le pourcentage affich� doit �tre proche de 100 en moyenne sur la trajectoire pour consid�rer que le Kv est bien calibr�.
			//Dans ce cas, c'est bien le Kv qui est l'auteur de la commande... cette commande �tant CORRIGEE par les autres coefs.

}
#endif

void CORRECTOR_update(void)
{
	Sint16 duty_left, duty_right;
	Sint32 commande_translation;		//[% moteurs]
	Sint32 commande_rotation;			//[% moteurs]
	//CALCUL DU PD (non, pas le pr�sident directeur !!! il s'agit bien du le proportionnel d�riv� !) 
	//... le moment ultime...
	//Calcul effectif du PD !!!  (ecart * Kp) + (d�riv�e(�cart)* Kd)	
	//k*[mm/4096]/4096 et k*[rad/4096]/1024
	//On ajoute une anticipation de vitesse si les coeffs sont non nuls... (MODE_ANTICIPATION_DE_VITESSE)

/*	
	//Code utilis� sur Archi'tech en 2009... Utile si les roues sont loin du centre de gravit� !
	global.coef_kp_translation = (global.ecart_rotation > 0)?KP_ROTATION_POSITIF:KP_ROTATION_NEGATIF;
	global.coef_kd_translation = (global.ecart_rotation > 0)?KD_ROTATION_POSITIF:KD_ROTATION_NEGATIF;		
*/
	// Calcul des �carts entre CONSIGNE (donn�es par MAJ...) et REEL (Mesur�, cpld...)  [mm/4096] et [rad/4096]
	global.ecart_translation = global.position_translation - global.real_position_translation; // positif si le robot doit avancer
	global.ecart_rotation = global.position_rotation - (global.real_position_rotation >> 10);  //positif pour rotation dans le sens trigo


	commande_translation = -(	(global.acceleration_translation						 * coefs[CORRECTOR_COEF_KA_TRANSLATION])  +
									(global.vitesse_translation 							 * coefs[CORRECTOR_COEF_KV_TRANSLATION]) 	+ 
									(global.ecart_translation 								 * coefs[CORRECTOR_COEF_KP_TRANSLATION])/16 	+
									(global.ecart_translation-global.ecart_translation_prec) * coefs[CORRECTOR_COEF_KD_TRANSLATION] 
								  )>>12;
								  
	commande_rotation 	= (	(global.acceleration_rotation							 * coefs[CORRECTOR_COEF_KA_ROTATION])		+
									(global.vitesse_rotation								 * coefs[CORRECTOR_COEF_KV_ROTATION])/2	+
									(global.ecart_rotation 									 * coefs[CORRECTOR_COEF_KP_ROTATION]) 	+ 
									((global.ecart_rotation-global.ecart_rotation_prec)		 * coefs[CORRECTOR_COEF_KD_ROTATION])
								  )>>10;

#if 0	//Code a tester								  
	if(COPILOT_get_trajectory() == TRAJECTORY_ROTATION)
		commande_translation = commande_translation * 4;	//Augmentation artificielle de la correction en translation lors d'une rotation pour tourner mieux par rapport au centre du robot.
#endif							  
		//sauvegarde des valeurs actuelles pour la prochaine boucle
	global.ecart_translation_prec = global.ecart_translation;
	global.ecart_rotation_prec = global.ecart_rotation;
		duty_right = 0;
		duty_left = 0;
		if(corrector_pd_translation == TRUE)
	{
		duty_right += commande_translation;
		duty_left += commande_translation;
	}

	//PD ROTATION ON - on ajoute la commande rotation
	if(corrector_pd_rotation == TRUE)
	{
		duty_right += commande_rotation;
		duty_left -= commande_rotation;
	}
	
	MOTORS_update(duty_left,duty_right);
		
}
/*
	[rad/4096 * kprot] = [mm/4096 * kptrans]/4
roue de propulsion : unit� de kptrans : 60mm/4=245760/4=61440 
					unit� de kprot : =12868 rad/4096
					facteur 5 ! (kptrans = 5* kprot)
*/


