/**
 * \file zone.h
 * \brief Définition d'enum,de structure et de fonction
 * \author Amaury.Gaudin Edouard.Thyebaut
 * \version 1
 * \date 27 novembre 2013
 */

#ifndef ZONE_H_
#define ZONE_H_
#include "stm32f4xx.h"
#include "QS/QS_all.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_CANmsgList.h"


/**
 * \enum mode_analyse_zone_e
 * \brief mode d'analyse pour les zone
*/
typedef enum
{
	NORMAL_MODE = 0,	/*!<analyse d'un passage ou non peut importe le temps*/
	FRESCO_LOCATOR,	/*!<on recherche une valeur précise de y*/
	DISPOSE_ON_HEART_POSITION_LOCATOR
}specific_analyse_mode_e;


typedef enum
{
	NOBODY_IN_ZONE = 0,
	SOMEONE_IS_IN,							//Un ou des objets sont	 	actuellement dans la zone
	SOMEONE_IS_IN_SINCE_MIN_DETECTION_TIME,	//Un ou des objets sont 	actuellement dans la zone depuis au moins le temps mini de détection
	SOMEONE_GET_OUT						//Un ou des objets sortent 	actuellement de la zone
}busy_state_e;


typedef Uint8 zone_enable_t;
#define ZONE_NOT_ENABLED			0x00
#define ZONE_ENABLE_FOR_BIG			0x01
#define ZONE_ENABLE_FOR_SMALL 		0x02
#define ZONE_ENABLE_FOR_BIG_AND_SMALL (ZONE_ENABLE_FOR_BIG | ZONE_ENABLE_FOR_SMALL)

/**
 * \struct zones_t
 * \brief Objet Zone Physique
 *
 * zones_t définition d'une zone avec sa taille et son identité
 * ainsi que son mode d'analyse
*/
typedef struct{
    Uint16 x_min;					/*!<coordonées x minimum*/
    Uint16 x_max;					/*!<coordonées x maximum*/
    Uint16 y_min;					/*!<coordonées y minimum*/
    Uint16 y_max;					/*!<coordonées y maximum*/

    bool_e someone_is_here;			//Il y a un ou plusieurs objets dans la zone

    zone_enable_t enable;
    zone_event_t events;					//Evenements que l'on observe... (cumul possible)
    zone_event_t events_for_big;					//Evenements que l'on observe... (cumul possible)
    zone_event_t events_for_small;					//Evenements que l'on observe... (cumul possible)

    time32_t min_detection_time;	//Temps nécessaire de présence d'un objet pour lever un évènement de présence. 0 pour lever immédiatement l'évènement.
    specific_analyse_mode_e specific_analyse_mode;		//Mode de traitement spécific de la zone

    time32_t input_time;			//Instant de l'entrée d'un objet dans la zone
    time32_t presence_duration;		//Durée de présence d'un objet dans la zone
    busy_state_e busy_state;
    Uint16 specific_analyse_param_x;
    Uint16 specific_analyse_param_y;

    bool_e alert_was_send;	//Un message a été envoyé à propos de cette zone (essentiellement destiné à l'affichage sur l'écran)
    bool_e initialized;
}zones_t;


void ZONE_init(void);
void ZONE_clean_all_detections(void);
void ZONE_enable(robot_id_e robot_id, zone_e i, zone_event_t events);
void ZONE_disable(robot_id_e robot_id, zone_e i);
void ZONE_disable_all(void);
void test_zones(void);
zones_t * ZONE_get_pzone(Uint8 i);
void ZONE_set_datas_updated(void);
void ZONE_process_main(void);
void ZONE_new_color(color_e c);
void ZONE_process_it_1ms(void);

#endif /* ZONE_H_ */
