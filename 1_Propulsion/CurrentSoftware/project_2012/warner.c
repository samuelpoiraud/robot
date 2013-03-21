/*
 *  Club Robot ESEO 2006 - 2011
 *
 *  Fichier : Avertisseur.c
 *  Package : Propulsion
 *  Description : L'avertisseur averti une carte qui l'a demand� qu'on est proche d'un angle o� d'une position
 *  Auteur : Nirgal,FanFan
 *  Version 201203
 */




#include "warner.h"
#include "QS/QS_CANmsgList.h"
#include "secretary.h"
#include "calculator.h"
#include "copilot.h"
#include "pilot.h"

/*
L'avertisseur est une sorte de klaxon tr�s utile pour certaines actions de match....

Certes, durant les matchs, la carte strat�gie demande � la carte propulsion une position r�guli�re...
Mais, pour certaines mesures de capteurs, il est imp�ratif d'obtenir des instants tr�s pr�cis (pour par exemple prendre une photo avec un t�l�m�tre)
afin de r�cup�rer ces informations de position en temps r�el, on utilise l'avertisseur.


Il existes diff�rents avertisseurs :
-> Avertisseur en x : "pr�viens moi quand la position en X est �gale � ..."  (la pr�cision d�pend de la vitesse max du robot entre deux IT...)
-> Avertisseur en y : "pr�viens moi quand la position en Y est �gale � ..." 
-> Avertisseur en teta : "pr�viens moi quand la position en teta est �gale � ..." 
-> Avertisseur p�riodique "pr�viens moi p�riodiquement tout les ..."  (forc�ment un multiple de 5ms)
-> Avertisseur en translation "pr�viens moi A CHAQUE FOIS que tu t'es d�plac� de ...mm"
-> Avertisseur en rotation "pr�viens moi A CHAQUE FOIS que tu t'es d�plac� de ...rad4096"

ce module fonctionne de la facon suivante :

- dans l'initialisation : appel de la fonction d'init de l'avertisseur.
- en IT : surveillances li�es aux avertisseurs actifs
- dans le MAIN : envoi d'un message can BROADCAST_POSITION avec la raison de l'envoi
- sur r�ception des messages can de demande d'avertissement : appel des fonctions qui activent les avertisseurs

*/

volatile Uint16 warnings;


volatile Sint16 warner_teta;
volatile Sint16 warner_x;
volatile Sint16 warner_y;
static Uint32 warner_translation;
static Sint16 warner_rotation;
static Uint16 warner_timer;

volatile position_t position_saved;

volatile Sint16 position_saved_time;
volatile Uint16 temps;

volatile Sint16 delta_teta_pour_envoi;
volatile Sint16 count_D_prev;
volatile Sint16 count_G_prev;

//Cette fonction DOIT �tre appel�e � l'initialisation. (apr�s l'init de l'odom�trie !)
void WARNER_init()
{
	
	warnings = WARNING_NO;
	

	
	//D�sactiver tout les avertisseurs...
	warner_teta = 0;
	warner_x = 0;
	warner_y = 0;
	warner_timer=0;
	warner_translation = 0;
	warner_rotation = 0;
	
	//RAZ param�tres
		position_saved = global.position;

	position_saved_time = 0;
	temps = 0;	//[5ms]
}

volatile static bool_e flag_arrived = FALSE;
volatile static bool_e flag_brake = FALSE;
volatile static bool_e flag_error = FALSE;
volatile static bool_e flag_calibration = FALSE;
volatile static Uint8 error_byte = 0x00;

//cette fonction sert � avertir, en envoyant des messages CAN de position si n�cessaire
void WARNER_process_main(void)
{
	if(flag_arrived)
	{
		flag_arrived = FALSE;
		SECRETARY_process_send(CARTE_P_TRAJ_FINIE,(Uint8)(warnings), error_byte);
	}	

	if(flag_calibration)
	{
		flag_calibration = FALSE;
		SECRETARY_process_send(CARTE_P_ROBOT_CALIBRE,0, error_byte);
	}

	if(flag_error)
	{
		flag_error = FALSE;
		SECRETARY_process_send(CARTE_P_ASSER_ERREUR,0, error_byte);
	}
	
	if(flag_brake)
	{
		flag_brake = FALSE;
		SECRETARY_process_send(CARTE_P_ROBOT_FREINE,0,error_byte);
	}	
		
	if(warnings != WARNING_NO)
	{
			SECRETARY_process_send(CARTE_P_POSITION_ROBOT,(Uint8)(warnings & 0xFF), error_byte);
			warnings = WARNING_NO; 	//WORK DONE !!!
	}
}

/////////////////////////////////////////////////////////////////////////////


void WARNER_inform(WARNER_state_t new_warnings, SUPERVISOR_error_source_e error_source)
{
	error_byte = ((Uint8)(COPILOT_get_trajectory()) << 5 | (Uint8)(COPILOT_get_way())) << 3 | (Uint8)(error_source);
	switch(new_warnings)
	{
		case WARNING_ARRIVED:
			flag_arrived = TRUE;
		break;
		case WARNING_BRAKE:
			flag_brake = TRUE;
		break;
		case WARNING_ERROR:
			flag_error = TRUE;
		break;
		case WARNING_CALIBRATION_FINISHED:
			flag_calibration = TRUE;
		break;
		default:
			warnings |= new_warnings;	//Seul les 8 bits faibles seront transmis !
		break;
	}	
	
}


/////////////////////////////////////////////////////////////////////////////

//Ces fonctions sont appel�es pour activer l'avertisseur, aux positions indiqu�es...

void WARNER_arm_teta(Sint16 teta)
{
	warner_teta = teta;
}

void WARNER_arm_x(Sint16 x)
{
	warner_x = x;
}

void WARNER_arm_y(Sint16 y)
{
	warner_y = y;
}

void WARNER_arm_timer(Uint16 duree) //Duree en [ms]
{
	warner_timer = duree;
}	

void WARNER_arm_translation(Uint32 translation)	//en mm
{
	warner_translation = translation * translation;
}

void WARNER_arm_rotation(Sint16 rotation)	//en rad4096
{
	warner_rotation = rotation;
}	

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//cette fonction doit �tre appel�e en IT
//elle est le coeur de l'avertisseur, et l�ve les flags n�cessaires	
void WARNER_process_it(void)
{
	Sint32 delta_x, delta_y;
	Uint32 delta_translation;
	Sint16 delta_teta;
	
	//
	//Pour louper le point, il faudrait aller a plus de  10mm/5ms, c'est � dire 2m/s !
	//explication : (VITESSE_TRANSLATION_LUMIERE/4096)*2 )
	//	la vitesse max qu'on ajoute est la vitesse lumi�re, en mm, il faut diviser par 4096, le *2 est une s�curit�, pour �tre sur de pas louper le point !
	//
	//	
	
	if(warner_teta)
		if( abs(global.position.teta - warner_teta) <=  (PILOT_get_coef(PILOT_ROTATION_SPEED_MAX)/1024)*2)
		{
			warner_teta = 0;
			warnings |= WARNING_REACH_TETA;
		}
	
	if(warner_x)
		if( abs(global.position.x - warner_x) < (PILOT_get_coef(PILOT_TRANSLATION_SPEED_LIGHT)/4096)*2 )
		{
			warner_x = 0;
			warnings |= WARNING_REACH_X;
		}	
			
	if(warner_y)
		if( abs(global.position.y - warner_y) < (PILOT_get_coef(PILOT_TRANSLATION_SPEED_LIGHT)/4096)*2 )
		{
			warner_y = 0;
			warnings |= WARNING_REACH_Y;
		}
		
	if(warner_timer)	//le minuteur est actif.
	{
		temps+=5;	//on ajoute 5ms !
		if( temps >= warner_timer)
			warnings |= WARNING_TIMER;
	}
	
	if(warner_translation)
	{
		delta_x = global.position.x - position_saved.x;
		delta_y = global.position.y - position_saved.y;
		delta_translation = delta_x * delta_x + delta_y * delta_y;		//calcul de la translation (ou du moins son carr� !)
		if (delta_translation >= warner_translation)
			warnings |= WARNING_TRANSLATION;	//Envoi de type
	}
	
	if(warner_rotation)
	{
		delta_teta = abs(CALCULATOR_modulo_angle(global.position.teta -  position_saved.teta));
		if(delta_teta > warner_rotation)
			warnings |= WARNING_ROTATION;
	}

	if(warnings)	//S'il faut avertir...
	{
		//SAVE pour le prochaine passage....
		temps = 0;
		if(warnings & (WARNING_ROTATION | WARNING_TRANSLATION))
		{
					position_saved = global.position;

		}
	}	
}

/////////////////////////////////////////////////////////////////////////////

