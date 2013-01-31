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

volatile bool_e corrector_pd_rotation = TRUE;
volatile bool_e corrector_pd_translation = TRUE;
volatile static Sint32 coefs[CORRECTOR_NUMBER_COEFS];

void CORRECTOR_init(void)
{
	coefs[CORRECTOR_COEF_KP_TRANSLATION] =  KP_TRANSLATION;
	coefs[CORRECTOR_COEF_KD_TRANSLATION] = KD_TRANSLATION;
	coefs[CORRECTOR_COEF_KV_TRANSLATION] = KV_TRANSLATION;
	coefs[CORRECTOR_COEF_KA_TRANSLATION] = KA_TRANSLATION;
	coefs[CORRECTOR_COEF_KP_ROTATION] = KP_ROTATION;
	coefs[CORRECTOR_COEF_KD_ROTATION] = KD_ROTATION;
	coefs[CORRECTOR_COEF_KV_ROTATION] = KV_ROTATION;
	coefs[CORRECTOR_COEF_KA_ROTATION] = KA_ROTATION;
  
	MOTORS_reset();	//RAZ des sorties Moteur.
	CORRECTOR_PD_enable(CORRECTOR_ENABLE);
}
	

void CORRECTOR_set_coef(CORRECTOR_coef_e coef, Sint32 value)
{
	coefs[coef] = value;
}

	//fonction d'activation/désactivation du correcteur PD.
	//Rotation = FALSE -> désactive la correction en rotation
	//Distance = FALSE -> désactive la correction en translation
	//et vice versa...
void CORRECTOR_PD_enable(corrector_e corrector)
{
	//si le pd était désactivé, on considère que notre destination est notre position actuelle, pour éviter qu'on ne panique et tombe en erreur...
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
	return corrector_pd_rotation;
}

void CORRECTOR_update(void)
{
	Sint16 duty_left, duty_right;
	Sint32 commande_translation;		//[% moteurs]
	Sint32 commande_rotation;			//[% moteurs]
	//CALCUL DU PD (non, pas le président directeur !!! il s'agit bien du le proportionnel dérivé !) 
	//... le moment ultime...
	//Calcul effectif du PD !!!  (ecart * Kp) + (dérivée(écart)* Kd)	
	//k*[mm/4096]/4096 et k*[rad/4096]/1024
	//On ajoute une anticipation de vitesse si les coeffs sont non nuls... (MODE_ANTICIPATION_DE_VITESSE)

/*	
	//Code utilisé sur Archi'tech en 2009... Utile si les roues sont loin du centre de gravité !
	global.coef_kp_translation = (global.ecart_rotation > 0)?KP_ROTATION_POSITIF:KP_ROTATION_NEGATIF;
	global.coef_kd_translation = (global.ecart_rotation > 0)?KD_ROTATION_POSITIF:KD_ROTATION_NEGATIF;		
*/
	// Calcul des écarts entre CONSIGNE (données par MAJ...) et REEL (Mesuré, cpld...)  [mm/4096] et [rad/4096]
	global.ecart_translation = global.position_translation - global.real_position_translation; // positif si le robot doit avancer
	global.ecart_rotation = global.position_rotation - (global.real_position_rotation >> 10);  //positif pour rotation dans le sens trigo


	commande_translation = (	(global.acceleration_translation						 * coefs[CORRECTOR_COEF_KA_TRANSLATION])  +
									(global.vitesse_translation 							 * coefs[CORRECTOR_COEF_KV_TRANSLATION]) 	+ 
									(global.ecart_translation 								 * coefs[CORRECTOR_COEF_KP_TRANSLATION]) 	+ 
									(global.ecart_translation-global.ecart_translation_prec) * coefs[CORRECTOR_COEF_KD_TRANSLATION] 
								  )/4096;
								  
	commande_rotation 	= (	(global.acceleration_rotation							 * coefs[CORRECTOR_COEF_KA_ROTATION])		+
									(global.vitesse_rotation								 * coefs[CORRECTOR_COEF_KV_ROTATION])/2	+
									(global.ecart_rotation 									 * coefs[CORRECTOR_COEF_KP_ROTATION]) 	+ 
									((global.ecart_rotation-global.ecart_rotation_prec)		 * coefs[CORRECTOR_COEF_KD_ROTATION])
								  )/1024;

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
roue de propulsion : unité de kptrans : 60mm/4=245760/4=61440 
					unité de kprot : =12868 rad/4096
					facteur 5 ! (kptrans = 5* kprot)
*/


