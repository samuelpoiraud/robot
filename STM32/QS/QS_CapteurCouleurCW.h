/*
 *	Club Robot ESEO 2012 - 2013
 *	Krusty & Tiny
 *
 *	Fichier : QS_CapteurCouleurCW.h
 *	Package : Qualit� Soft
 *	Description : Driver du capteur couleur Tri-tronics ColorWise
 *  Auteurs : Alexis
 *  Version 20130207
 */

#ifndef QS_CAPTEURCOULEURCW_H
#define	QS_CAPTEURCOULEURCW_H

/** D�finir pour activer le driver.
 */
//#ifndef USE_CW_SENSOR
//#define USE_CW_SENSOR
//#undef  USE_CW_SENSOR
//#endif

/** Nombre de capteur � g�rer.
 */
//#ifndef CW_SENSOR_NUMBER
//#define CW_SENSOR_NUMBER
//#undef  CW_SENSOR_NUMBER
//#endif

#include "QS_all.h"


#ifdef USE_CW_SENSOR

/** D�crit un port d'entr�e/sortie */
typedef struct {
	volatile unsigned int* port;
	Uint8 bit_number;
	bool_e is_inverted_logic; //TRUE s'il y a detection quand le port est au niveau 0
} CW_port_t;

/** Composante couleurs. Utilis� pour la lecture des valeurs analogiques. */
typedef enum {
	CW_AC_XYZ_X,  //Couleur XYZ RGB (voir http://en.wikipedia.org/wiki/CIE_1931_color_space)
	CW_AC_XYZ_Y,
	CW_AC_XYZ_Z,
	CW_AC_xyY_x = CW_AC_XYZ_X,  //Couleurs xyY
	CW_AC_xyY_y = CW_AC_XYZ_Y,
	CW_AC_xyY_Y = CW_AC_XYZ_Z
} CW_analog_color_e;

/** Constante des ports pour la configuration.
 * Ces constantes sont � utiliser comme indice du tableau digital_ports dans la structure #CW_config_t
 * @see CW_config_t
 */
typedef enum {
	CW_PP_Channel0 = 0,
	CW_PP_Channel1 = 1,
	CW_PP_Channel2 = 2,
	CW_PP_Channel3 = 3,
	CW_PP_Gate = 4,
	CW_PP_RemoteControl = 5,
	CW_PP_MAXPORTNUM = 6
} CW_port_e;

/** Informations de configuration d'un capteur.
 * Cette structure est utilis� pour indiquer les emplacements des ports sur la carte.
 * Pour digital_ports, il est pr�f�rable d'utiliser les �num�rations dans #CW_port_e de cette fa�on:
 * digital_ports[CW_PP_Gate] pour configurer le port Gate.
 * dans "digital_ports", "port" est un pointeur vers le port du pic ou le capteur est connect�
 * et "bit_number" le num�ro du bit du port.
 * @see CW_port_e
 * @see CW_UNUSED_ANALOG_PORT
 * @see CW_UNUSED_PORT
 * @see CW_config_sensor
 */
typedef struct {
	CW_port_t digital_ports[CW_PP_MAXPORTNUM];
	Uint8 analog_X;
	Uint8 analog_Y;
	Uint8 analog_Z;
} CW_config_t;

/** Constante utilis� quand un port analogique n'est pas utilis�.
 * @see CW_config_t
 */
#define CW_UNUSED_ANALOG_PORT 0xFF

/** Constante utilis� quand un port digital n'est pas utilis�.
 * Cette constante doit �tre affect�e � la variable "port"
 * @see CW_config_t
 */
#define CW_UNUSED_PORT NULL

/**
 * Initialise le driver des capteurs couleur Tri-Tronics ColorWise CW-1/2.
 * Cette fonction doit �tre appel�e avant l'utilisation de ce driver.
 */
void CW_init();

/**
 * Configure un capteur couleur pour pouvoir l'utiliser.
 * @param id_sensor Numero du capteur � configurer. Ce nombre doit �tre entre 0 (inclu) et #CW_SENSOR_NUMBER (exclu).
 * @param config
 */
void CW_config_sensor(Uint8 id_sensor, CW_config_t* config);

/**
 * Detection d'une couleur pr�configur�e sur le capteur.
 *
 * Chaque canal doit �tre configur� sur le capteur a l'aide des bouton Select et Next.
 * Cette fonction renvoie TRUE si la couleur � �t� detect�e.
 * @param id_sensor Numero du capteur. Ce nombre doit �tre entre 0 (inclu) et #CW_SENSOR_NUMBER (exclu).
 * @param canal Numero du canal � checker entre 0 et 3 inclu
 * @return TRUE si la couleur a �t� detect�e sinon FALSE
 */
bool_e CW_is_color_detected(Uint8 id_sensor, Uint8 canal);

/**
 * Recup�ration des composantes de la couleur de l'objet vis�.
 *
 * Cette fonction utilise des ports analogique et les pin analog_*.
 * La valeur renvoy�e est l'image de la composante indiqu�e.
 * Si les couleurs renvoy�es sont en mode "C", les composantes X, Y, Z sont les couleurs RGB.
 * Si les couleurs renvoy�es sont en mode "CI", les composantes sont x, y, Y et correspondent
 * aux coordon�es x et y de la couleur et � l'intensit�e lumineuse Y.
 * @param id_sensor Numero du capteur. Ce nombre doit �tre entre 0 (inclu) et #CW_SENSOR_NUMBER (exclu).
 * @param composante Couleur � checker
 * @return Intensit� de la composante entre 0 et 1024
 */
Uint16 CW_get_color_intensity(Uint8 id_sensor, CW_analog_color_e composante);


#endif //USE_CW_SENSOR

#endif	/* QS_CAPTEURCOULEURCW_H */

