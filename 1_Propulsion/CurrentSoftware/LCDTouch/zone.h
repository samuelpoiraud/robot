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
#include "../QS/QS_all.h"



/**
 * \enum mode_analyse_zone_e
 * \brief mode d'analyse pour les zone
*/
typedef enum{
	PASSAGE=0,	/*!<analyse d'un passage ou non peut importe le temps*/
	DURATION,	/*!<analyse du temps de passage*/
	LOOKING_Y	/*!<on recherche une valeur précise de y*/
}mode_analyse_zone_e;

/**
 * \struct zones_t
 * \brief Objet Zone Physique
 *
 * zones_t définition d'une zone avec sa taille et son identité
 * ainsi que son mode d'analyse
*/
typedef struct{
	uint8_t id_zone; 				/*!<identifiant de chaque zone*/
    uint16_t x_min;					/*!<coordonées x minimum*/
    uint16_t x_max;					/*!<coordonées x maximum*/
    uint16_t y_min;					/*!<coordonées y minimum*/
    uint16_t y_max;					/*!<coordonées y maximum*/
    mode_analyse_zone_e analyse;	/*!<mode d'annalyse suivant l'interet de la zone*/
}zones_t;


/**
 * \struct zone_t
 * \brief Objet Zone Virtuel
 *
 * zone_t ce qui nous intéresse dans cette zone,
 * avec ses données qui sont utilisées en fonction de son mode d'annalyse
 */
typedef struct{
	 uint8_t id_zone;		/*!< id de la zone conecerne */
	 uint16_t y_recherche;	/*!< position en y recherche */
	 uint32_t temps;		/*!< temps passe dans la zone */
	 bool_e current;		/*!< un robot est il encore la */
	 bool_e exited;			/*!< un robot est il parti */
	 bool_e entered;		/*!< un robot est il entre */
	 uint8_t robot;			/*!< robot concerné */
}zone_t;

zone_t zone_tableau[14]; /*!< tableau de donnée des zones */

void init_zone(void);
const zones_t * get_zones(void);
void test_zones(uint16_t x,uint16_t y,uint8_t robot);
void ZONE_process_10ms(void);


#endif /* ZONE_H_ */
