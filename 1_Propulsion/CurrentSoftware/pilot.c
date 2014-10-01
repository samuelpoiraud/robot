/*
 *  Club Robot ESEO 2009 - 2010
 *  Chomp
 *
 *  Fichier : point_fictif.h
 *  Package : Propulsion
 *  Description : prototypes fonctions de gestion du point fictif
 *  Auteur : Nirgal 2010
 *  Version 201001
 */



#include "pilot.h"
#include "QS/QS_macro.h"
#include "roadmap.h"
#include "copilot.h"
#include "corrector.h"
#include "supervisor.h"
#include "motors.h"
#include "QS/QS_maths.h"
#include "calculator.h"
#include "joystick.h"
#include "QS/QS_who_am_i.h"

typedef enum {
	ACCELERATION_NULLE,
	ACCELERATION_POSITIVE,
	ACCELERATION_NEGATIVE,
	ACCELERATION_FOR_DECREASE_SPEED,
	ACCELERATION_FOR_INCREASE_SPEED
} e_acceleration;

typedef enum {
	AUCUN,
	ACCELERATION,
	DECELERATION
} relative_acceleration_e;

void PILOT_update_acceleration_state(void);
void PILOT_update_acceleration_translation_and_rotation(void);

void PILOT_update_speed_translation(void);
void PILOT_update_speed_rotation(void);

void PILOT_update_position_translation(void);
void PILOT_update_position_rotation(void);
void PILOT_set_extra_braking_translation(bool_e enable);
void PILOT_set_extra_braking_rotation(bool_e enable);


//Coordonn�es de la destination dans le r�f�rentiel IT
volatile Sint32 destination_rotation;		//[rad/4096] <<12
volatile Sint32 destination_translation;	//[mm/4096] <<12

volatile bool_e extra_braking_translation = FALSE;
volatile bool_e extra_braking_rotation = FALSE;
volatile Sint32	rotation_frein;

volatile e_acceleration etat_acceleration_translation;
volatile e_acceleration etat_acceleration_rotation;

volatile Sint32 vitesse_translation_max;
volatile Sint32 vitesse_rotation_max;

volatile Sint32 coefs[PILOT_NUMBER_COEFS];

#define SOUS_QUANTIFICATION (0)	//Simplification de la courbe (permet de r�duire le nombre de calculs de sinus..) Doit �tre une puissance de 2...

void PILOT_init(void)
{
	CORRECTOR_init();
	MOTORS_init();
	PILOT_set_speed(FAST);
	PILOT_referential_init();
	if(QS_WHO_AM_I_get()==SMALL_ROBOT)
	{
		coefs[PILOT_ACCELERATION_NORMAL] 				= SMALL_ACCELERATION_NORMAL;
		coefs[PILOT_ACCELERATION_ROTATION_TRANSLATION] 	= SMALL_ACCELERATION_ROTATION_TRANSLATION;
		coefs[PILOT_TRANSLATION_SPEED_LIGHT] 			= SMALL_TRANSLATION_SPEED_LIGHT;
		coefs[PILOT_TRANSLATION_SPEED_MAX] 				= SMALL_TRANSLATION_SPEED_MAX;
		coefs[PILOT_TRANSLATION_SPEED_LOW] 				= SMALL_TRANSLATION_SPEED_LOW;
		coefs[PILOT_TRANSLATION_SPEED_VERY_LOW] 		= SMALL_TRANSLATION_SPEED_VERY_LOW;
		coefs[PILOT_TRANSLATION_SPEED_SNAIL] 			= SMALL_TRANSLATION_SPEED_SNAIL;
		coefs[PILOT_ROTATION_SPEED_LIGHT] 				= SMALL_ROTATION_SPEED_LIGHT;
		coefs[PILOT_ROTATION_SPEED_MAX] 				= SMALL_ROTATION_SPEED_MAX;
		coefs[PILOT_ROTATION_SPEED_LOW] 				= SMALL_ROTATION_SPEED_LOW;
		coefs[PILOT_ROTATION_SPEED_VERY_LOW] 			= SMALL_ROTATION_SPEED_VERY_LOW;
		coefs[PILOT_ROTATION_SPEED_SNAIL] 				= SMALL_ROTATION_SPEED_SNAIL;
	}
	else
	{
		coefs[PILOT_ACCELERATION_NORMAL] 				= BIG_ACCELERATION_NORMAL;
		coefs[PILOT_ACCELERATION_ROTATION_TRANSLATION] 	= BIG_ACCELERATION_ROTATION_TRANSLATION;
		coefs[PILOT_TRANSLATION_SPEED_LIGHT] 			= BIG_TRANSLATION_SPEED_LIGHT;
		coefs[PILOT_TRANSLATION_SPEED_MAX] 				= BIG_TRANSLATION_SPEED_MAX;
		coefs[PILOT_TRANSLATION_SPEED_LOW] 				= BIG_TRANSLATION_SPEED_LOW;
		coefs[PILOT_TRANSLATION_SPEED_VERY_LOW] 		= BIG_TRANSLATION_SPEED_VERY_LOW;
		coefs[PILOT_TRANSLATION_SPEED_SNAIL] 			= BIG_TRANSLATION_SPEED_SNAIL;
		coefs[PILOT_ROTATION_SPEED_LIGHT] 				= BIG_ROTATION_SPEED_LIGHT;
		coefs[PILOT_ROTATION_SPEED_MAX] 				= BIG_ROTATION_SPEED_MAX;
		coefs[PILOT_ROTATION_SPEED_LOW] 				= BIG_ROTATION_SPEED_LOW;
		coefs[PILOT_ROTATION_SPEED_VERY_LOW] 			= BIG_ROTATION_SPEED_VERY_LOW;
		coefs[PILOT_ROTATION_SPEED_SNAIL] 				= BIG_ROTATION_SPEED_SNAIL;
	}
}

Sint32 PILOT_get_coef(PILOT_coef_e id)
{
	return coefs[id];
}

//Attention... a utiliser avec des valeurs pertinentes......
void PILOT_set_coef(PILOT_coef_e id, Sint32 value)
{
	coefs[id] = value;
}

void PILOT_referential_init(void)
{
	global.vitesse_rotation = 0;
	global.vitesse_translation = 0;
	COPILOT_reset_absolute_destination();
}



//remise a z�ro du r�f�rentiel IT... attention a ne pas le faire n'importe quand...
//le point fictif est ici ramen� au point actuel... attention, �a peut �tre violent (freinage brusque...)
void PILOT_referential_reset()
{
	global.position_translation = 0;
	global.position_rotation = 0;

	global.real_position_translation = 0;
	global.real_position_rotation = 0;

	global.ecart_translation_prec = 0;
	global.ecart_rotation_prec = 0;

	//pas de discontinuit� de vitesse...
}

void PILOT_process_it(void)
{


	PILOT_update_acceleration_translation_and_rotation();

	PILOT_update_speed_translation();
	PILOT_update_speed_rotation();

	PILOT_update_position_translation();
	PILOT_update_position_rotation();

	CORRECTOR_update();
}


void PILOT_set_destination_rotation(Sint32 dest)
{
	destination_rotation = dest;
}

void PILOT_set_destination_translation(Sint32 dest)
{
	destination_translation = dest;
}

Sint32 PILOT_get_destination_translation(void)
{
	return destination_translation;
}

Sint32 PILOT_get_destination_rotation(void)
{
	return destination_rotation;
}


				//[en de puissance de 2...] ! (si = 7 : on sous quantifie de 64...)
				//NE PAS CHANGER CETTE VALEUR... sans connaissance absolue de toutes les unit�s et limites des variables de cette fonctions.
void PILOT_update_acceleration_state(void)
{
	static Sint32 vrot;			//[rad/4096/1024/5ms]
	static Sint32 vtrans;		//[mm/4096/5ms]
	static Sint16 alpha;		//[rad/4096]
	static Sint32 d;			//[mm/4096]
	static Sint32 vx;			//[mm/4096/5ms]
	static Sint32 vy;			//[mm/4096/5ms]
	static Sint32 px;			//[mm/4096]
	static Sint32 py;			//[mm/4096]
	static Sint32 destx;		//[mm/4096]
	static Sint32 desty;		//[mm/4096]
	static Sint16 sin, cos;	//[/4096]
	bool_e bdroite = FALSE, bgauche = FALSE, loop = TRUE, must_do_loop = TRUE;

	//Init des sorties...
	etat_acceleration_rotation = ACCELERATION_NULLE;
	etat_acceleration_translation = ACCELERATION_NULLE;

	//Conditions d'entr�e.
	vrot = global.vitesse_rotation;
	vtrans = global.vitesse_translation;
	alpha = destination_rotation;	//rad/4096
	d = destination_translation >> 12;	//mm

	alpha = CALCULATOR_modulo_angle(alpha);

	//Calcul de la destination dans le rep�re local cart�sien
	if(((alpha<-HALF_PI4096 || alpha > HALF_PI4096) && COPILOT_get_way() == ANY_WAY) || COPILOT_get_way() == BACKWARD)
	{
		alpha += PI4096;
		vtrans=-vtrans;
	}

	alpha = CALCULATOR_modulo_angle(alpha);

	COS_SIN_4096_get(alpha, &cos, &sin);
	destx = ((d)*(cos));	//mm/4096
	desty = ((d)*(sin));	//mm/4096

	if(global.vitesse_translation == 0)	//Cas de la vitesse nulle.
	{
		if(!COPILOT_is_arrived_rotation())
			etat_acceleration_rotation = ACCELERATION_FOR_INCREASE_SPEED;
		else
		{
			if(!COPILOT_is_arrived_translation())
				etat_acceleration_translation = ACCELERATION_FOR_INCREASE_SPEED;
		}
		must_do_loop = FALSE;
	}

	if(COPILOT_is_arrived_rotation())
		must_do_loop = FALSE;


	if ((destination_translation > global.position_translation
		 && global.vitesse_translation < 0)
			|| (destination_translation < global.position_translation
				&& global.vitesse_translation > 0))
	{
		etat_acceleration_translation = ACCELERATION_FOR_DECREASE_SPEED;
		//Je donne tout � la translation pour qu'elle freine !
		must_do_loop = FALSE;
	}

	if(must_do_loop)
	{
		//Calcul de la courbe de gauche.
		vrot = global.vitesse_rotation;

		alpha = 0;	//rad/4096
		px = 0;
		py = 0;
		loop = TRUE;
		while (loop)
		{
			//Calculs
			if(vrot < (Sint32)(vitesse_rotation_max))
			{
				vrot += (Sint32)(coefs[PILOT_ACCELERATION_NORMAL] * coefs[PILOT_ACCELERATION_ROTATION_TRANSLATION]) << SOUS_QUANTIFICATION;
			}
			if(vrot > (Sint32)(vitesse_rotation_max))
			{
				vrot = (Sint32)(vitesse_rotation_max);
			}

			alpha += vrot >> (10-SOUS_QUANTIFICATION);	//[rad/4096] = [rad/4096/1024/5ms] *SOUS_QUANTIFICATION = [rad/4096/5ms] >> 3 lorsque SOUS_QUANTIFICATION vaut 7

			alpha = CALCULATOR_modulo_angle(alpha);

			COS_SIN_4096_get(alpha, &cos, &sin);

			vy = (vtrans * sin)>>12;	//mm/4096/5ms
			vx = (vtrans * cos)>>12;	//mm/4096/5ms

			px += vx << SOUS_QUANTIFICATION;	//mm/4096
			py += vy << SOUS_QUANTIFICATION;	//mm/4096
			//debug_printf("gauche : alpha %ld |vrot %ld |vx %ld |vy %ld |px %ld |py %ld\n", alpha, vrot, vx, vy, px, py);

			if(alpha > HALF_PI4096)	//On a d�pass� le haut de la courbe...
			{
				loop = FALSE;
				//	debug_printf("gauche : haut de courbe");
				bgauche = TRUE;	//La destination est forc�ment au dessus... (sinon d�tect�e avant ou ci dessous !)
			}

			if(px > destx)	//La courbe a d�pass� l'ordonn�e de la destination
			{
				loop = FALSE;	//On arr�te la boucle

				if(desty < py)
				{
					//	debug_printf("gauche : point a droite");
					bgauche = TRUE;	//Le point est � droite du pire des cas, c'est bon �a !
				}
				else
					bgauche = FALSE;	//Le point est � gauche du pire des cas, c'est con �a !
			}
		}

		//Calcul de la courbe de droite.
		vrot = global.vitesse_rotation;

		alpha = 0;	//rad/4096/1024
		px = 0;
		py = 0;
		alpha = 0;
		loop = TRUE;
		while (loop)
		{
			//Calculs
			if(vrot > -(Sint32)(vitesse_rotation_max))
			{
				vrot -= (Sint32)(coefs[PILOT_ACCELERATION_NORMAL] * coefs[PILOT_ACCELERATION_ROTATION_TRANSLATION]) << SOUS_QUANTIFICATION;
			}
			if(vrot < -(Sint32)(vitesse_rotation_max))
			{
				vrot = -(Sint32)(vitesse_rotation_max);
			}

			alpha += vrot >> (10-SOUS_QUANTIFICATION);	//[rad/4096] = [rad/4096/1024/5ms] *SOUS_QUANTIFICATION = [rad/4096/5ms] >> 3 lorsque SOUS_QUANTIFICATION vaut 7

			alpha = CALCULATOR_modulo_angle(alpha);
			//Alpha exprim� de 0 � TWO_PI4096 !
			COS_SIN_4096_get(alpha, &cos, &sin);

			vy = (vtrans * sin)>>12;	//mm/4096/5ms
			vx = (vtrans * cos)>>12;	//mm/4096/5ms

			px += vx << SOUS_QUANTIFICATION;
			py += vy << SOUS_QUANTIFICATION;
			//debug_printf("droite : alpha %ld | vx %ld | vy %ld | px %ld | py %ld\n", alpha, vx, vy, px, py);

			//TODO enrichir le cas o� la vitesse en rotation est grande, et engendre une courbe pire... je me comprends
			// relu un an apr�s, je me comprends plus...
			if(alpha < -HALF_PI4096)	//On a d�pass� le haut de la courbe...
			{
				//	debug_printf("droite :haut de courbe");
				bdroite = TRUE;	//La destination est forc�ment au dessus... (sinon d�tect�e avant ou ci dessous !)
				loop = FALSE;
			}

			if(px > destx)	//La courbe a d�pass� l'ordonn�e de la destination
			{
				loop = FALSE;	//On arr�te la boucle
				if(desty > py)
				{
					//		debug_printf("droite :point a gauche");
					bdroite = TRUE;	//Le point est � gauche du pire des cas, c'est bon �a !
				}
				else
					bdroite = FALSE;	//Le point est � droite du pire des cas, c'est con �a !
			}
		}

		if(bdroite && bgauche)	//Cool ! Le point est accessible
			etat_acceleration_rotation = ACCELERATION_FOR_INCREASE_SPEED;
		else{					//Le point n'est pas accessible sans freiner... => on doit freiner
			etat_acceleration_translation = ACCELERATION_FOR_DECREASE_SPEED;
			etat_acceleration_rotation = ACCELERATION_FOR_DECREASE_SPEED;
		}


		//		fprintf(fgabarit,"%ld\t%ld\t%c\n", destx/4096, desty/4096, (bdroite && bgauche)?'+':'-');
		//						fclose (fgabarit);

	}

	//Attention, l'ordre des lignes ci-dessous est important.

	if (COPILOT_braking_rotation_get() == BRAKING)
		etat_acceleration_rotation = ACCELERATION_FOR_DECREASE_SPEED;


	if (COPILOT_is_arrived_rotation())
	{
		//Il ne reste plus qu'� avancer !
		etat_acceleration_translation = ACCELERATION_FOR_INCREASE_SPEED;
		etat_acceleration_rotation = ACCELERATION_NULLE;
	}

	if (COPILOT_braking_translation_get() == BRAKING)
		etat_acceleration_translation = ACCELERATION_FOR_DECREASE_SPEED;

	if (COPILOT_is_arrived_translation())
		etat_acceleration_translation = ACCELERATION_NULLE;

	//Ecretage vitesse si la vitesse max a chang�...
	if (absolute(global.vitesse_rotation) > absolute(vitesse_rotation_max))
		etat_acceleration_rotation = ACCELERATION_FOR_DECREASE_SPEED;

	if (absolute(global.vitesse_translation) > absolute(vitesse_translation_max))
		etat_acceleration_translation = ACCELERATION_FOR_DECREASE_SPEED;

	if(COPILOT_get_trajectory() == TRAJECTORY_STOP)
	{
		etat_acceleration_translation = ACCELERATION_FOR_DECREASE_SPEED;
		etat_acceleration_rotation = ACCELERATION_FOR_DECREASE_SPEED;
	}
	if(SUPERVISOR_get_state() == SUPERVISOR_ERROR)
	{
		etat_acceleration_translation = ACCELERATION_NULLE;
		etat_acceleration_rotation = ACCELERATION_NULLE;
	}
}



void PILOT_update_acceleration_translation_and_rotation(void) {
	/*ratios pour la réalisaton d'une courbure*/
	Sint32 prot, ptrans;

	PILOT_update_acceleration_state();

	ptrans = 0;
	prot = 0;

	switch (etat_acceleration_translation)
	{
		case ACCELERATION_FOR_DECREASE_SPEED:
			if (global.vitesse_translation < 0)
			{
				ptrans = 64;
			}
			else
			{
				if (global.vitesse_translation > 0)
				{
					ptrans = -64;
				}
				else
				{
					ptrans = 0;
				}
			}
			break;
		case ACCELERATION_FOR_INCREASE_SPEED:
			if (destination_translation < global.position_translation)
				ptrans = -64;
			else if (destination_translation > global.position_translation)
				ptrans = 64;
			else
				ptrans = 0;
			break;
		default:
			ptrans = 0;
			break;
	}

	switch (etat_acceleration_rotation) {
		case ACCELERATION_FOR_DECREASE_SPEED:
			if (global.vitesse_rotation < 0)
			{
				prot = 64;
			}
			else
			{
				if (global.vitesse_rotation > 0)
				{
					prot = -64;
				}
				else
				{
					prot = 0;
				}
			}
			break;
		case ACCELERATION_FOR_INCREASE_SPEED:
			if (destination_rotation < global.position_rotation)
			{
				prot = -64;
			}
			else
			{
				if (destination_rotation > global.position_rotation)
				{
					prot = 64;
				}
				else
				{
					prot = 0;
				}
			}
			break;
		default:
			prot = 0;
			break;
	}

	if(	(ptrans == 64 && global.vitesse_translation >= vitesse_translation_max)  ||
			(ptrans == -64 && global.vitesse_translation <= -vitesse_translation_max)	)
		ptrans = 0;	//On ne d�passe pas la vitesse max

	if(	(prot == 64 && global.vitesse_rotation >= vitesse_rotation_max)  ||
			(prot == -64 && global.vitesse_rotation <= -vitesse_rotation_max)	)
		prot = 0;	//On ne d�passe pas la vitesse max

	if(COPILOT_braking_rotation_get() == BRAKING && extra_braking_rotation)
	{
		prot = prot * 2; // 3/2
	}
	if(COPILOT_braking_translation_get() == BRAKING && extra_braking_translation)
	{
		ptrans = ptrans * 2; // 3/2
	}

	if(COPILOT_get_trajectory() == TRAJECTORY_STOP && QS_WHO_AM_I_get() == BIG_ROBOT)
	{
		ptrans = ptrans * 2;
		prot = prot * 2;
	}

	global.acceleration_translation = (coefs[PILOT_ACCELERATION_NORMAL] * ptrans) / 64;
	global.acceleration_rotation = (coefs[PILOT_ACCELERATION_NORMAL] * prot) / 64;

	//ce coeff d�pend de la distance entre les roues de propulsions...
	global.acceleration_rotation *= coefs[PILOT_ACCELERATION_ROTATION_TRANSLATION];
}

void PILOT_update_speed_translation() {

	if(etat_acceleration_translation == ACCELERATION_FOR_DECREASE_SPEED
			&& absolute(global.vitesse_translation) < absolute(global.acceleration_translation))
		global.vitesse_translation = 0;	//Derni�re �tape d'une diminution de vitesse...
	else 	//mise a jour de la vitesse de translation
		global.vitesse_translation += global.acceleration_translation;

	if (COPILOT_is_arrived_translation())
		global.vitesse_translation = 0;
}

void PILOT_update_speed_rotation() {

	if(etat_acceleration_rotation == ACCELERATION_FOR_DECREASE_SPEED
			&& absolute(global.vitesse_rotation) < absolute(global.acceleration_rotation))
		global.vitesse_rotation = 0;	//Derni�re �tape d'une diminution de vitesse...
	else 	//mise a jour de la vitesse de rotation...
		global.vitesse_rotation += global.acceleration_rotation;

	if (COPILOT_is_arrived_rotation())
		global.vitesse_rotation = 0;
}

void PILOT_update_position_translation()
{
	//mise � jour des positions voulues suivant les vitesses voulues calcul�es plus haut...

	if (COPILOT_is_arrived_translation())
	{
		//si on est ARRIVE en traj d'arret, on ne fait rien a la position de l'angle du point fictif
		if (COPILOT_get_trajectory() == TRAJECTORY_STOP)
			return;
		else	//si on est ARRIVE en traj normale, on prend la position du point final
		{
			if(COPILOT_get_border_mode() == NOT_BORDER_MODE)	//Sauf en mode bordure, on acc�pte la position du point final !
				global.position_translation = destination_translation;
		}
	}
	else	//si on est pas arriv�, on continue...
		global.position_translation += global.vitesse_translation;
}

void PILOT_update_position_rotation()
{
	if (COPILOT_is_arrived_rotation())
	{
		//si on est ARRIVE en traj d'arret, on ne fait rien a la position de l'angle du point fictif
		if (COPILOT_get_trajectory() == TRAJECTORY_STOP)
			return;
		else //si on est ARRIVE en traj normale, on prend la position du point final
			global.position_rotation = destination_rotation;
	} else	//si on est pas arriv�, on continue...
		global.position_rotation += global.vitesse_rotation >> 10;
	global.position_rotation = CALCULATOR_modulo_angle(global.position_rotation);
}



void PILOT_set_speed(PROP_speed_e speed)
{
	switch(speed)
	{
		case FAST:
			vitesse_rotation_max = coefs[PILOT_ROTATION_SPEED_MAX];
			vitesse_translation_max = coefs[PILOT_TRANSLATION_SPEED_MAX];
			break;
		case EXTREMELY_VERY_SLOW:
			vitesse_rotation_max = coefs[PILOT_ROTATION_SPEED_SNAIL];
			vitesse_translation_max = coefs[PILOT_TRANSLATION_SPEED_SNAIL];
			break;
		case SLOW_TRANSLATION_AND_FAST_ROTATION:
			//Tr�s lent est utilis� pour du calage, la rotation peut etre rapide ?
			vitesse_rotation_max = coefs[PILOT_ROTATION_SPEED_MAX];
			vitesse_translation_max = coefs[PILOT_TRANSLATION_SPEED_VERY_LOW];
			break;
		case FAST_TRANSLATION_AND_SLOW_ROTATION:
			vitesse_rotation_max = coefs[PILOT_ROTATION_SPEED_LOW];
			vitesse_translation_max = coefs[PILOT_TRANSLATION_SPEED_MAX];
			break;
		case SLOW:
			vitesse_rotation_max = coefs[PILOT_ROTATION_SPEED_LOW];
			vitesse_translation_max = coefs[PILOT_TRANSLATION_SPEED_LOW];
			break;
		case CUSTOM:
		default:
			vitesse_translation_max = (Sint32)(speed-8) * 128;	//"vitesse" varie de 8 � 255 et la vitesse finallement obtenue varie de 0 � 32702
			vitesse_rotation_max = (Sint32)(speed-8) * 1024;	//"vitesse" varie de 8 � 255 et la vitesse finallement obtenue varie de 0 � 252928 (c'est trop ! mais il y a un �cretage ensuite)
			if(vitesse_translation_max > coefs[PILOT_TRANSLATION_SPEED_LIGHT])
				vitesse_translation_max = coefs[PILOT_TRANSLATION_SPEED_LIGHT];
			if(vitesse_rotation_max > coefs[PILOT_ROTATION_SPEED_LIGHT])
				vitesse_rotation_max = coefs[PILOT_ROTATION_SPEED_LIGHT];
			break;
	}
}


void PILOT_set_extra_braking_rotation(bool_e enable)
{
	extra_braking_rotation = enable;
}

void PILOT_set_extra_braking_translation(bool_e enable)
{
	extra_braking_translation = enable;
}

