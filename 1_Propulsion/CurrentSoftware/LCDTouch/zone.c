/**
 * \file zone.c
 * \brief Définition, initialisation et teste de Zone
 * \author Amaury.Gaudin Edouard.Thyebaut
 * \version 1
 * \date 4 décembre 2013
 *
 * Fonctions de créations des 14 zones importantes du plateau de jeux ainsi que l'initialisation de ces zones
 *  Puis le teste de la présence ou non d'un robot dans ces zones avec les informations à retourner en fonction des zones
 */

#include "zone.h"

#if defined (SIMULATION_VIRTUAL_PERFECT_ROBOT)


volatile uint32_t zone_t_10ms = 0;
#define ZONE_NUMBER	14
//zone importantes du terrain {id,xmin,xmax,ymin,ymax,systeme d'annalyse} (cm)
const zones_t zone[ZONE_NUMBER]=
{
	{1,0,28,110,189,LOOKING_Y}, 	//fresque
	{2,75,110,115,150,DURATION},	//foyer central quart mamouth-rouge
	{3,110,145,115,150,DURATION},	//foyer central quart arbre-rouge
	{4,85,135,65,115,PASSAGE},		//torche rouge
	{5,100,160,0,40,PASSAGE},		//arbre rouge-depart
	{6,160,199,0,40,DURATION},		//foyer rouge
	{7,160,199,40,100,PASSAGE},		//arbre rouge
	{8,160,199,100,201,PASSAGE},	//feu central
	{9,160,199,201,260,PASSAGE},	//arbre jaune
	{10,160,199,260,299,DURATION},	//foyer jaune
	{11,100,160,260,299,PASSAGE},	//arbre jaune-depart
	{12,85,135,185,235,PASSAGE},	//torche jaune
	{13,75,110,150,185,DURATION},	//foyer central quart mamouth-jaune
	{14,110,145,150,185,DURATION}	//foyer central quart arbre-jaune
};


/**
 * \fn void init_zone(void)
 * \brief Initialise les 14 zones
 *
 *
 */
void init_zone(void){
	Uint8 i;
	for (i=0;i<ZONE_NUMBER;i++){
		zone_tableau[i].current=FALSE;
		zone_tableau[i].entered=FALSE;
		zone_tableau[i].exited=FALSE;
		zone_tableau[i].id_zone=i+1;
		zone_tableau[i].temps=0;
		zone_tableau[i].y_recherche=0;
		zone_tableau[i].robot=0;
	}
}



/**
 * \fn * get_zones(void)
 * \brief Initialise les 14 zones
 *
 * \return zone demandé
 */
const zones_t * get_zones(void){
	return zone;
}
/**
 * \fn  void test_zones(uint16_t x,uint16_t y,uint8_t robot)
 * \brief teste si un robot est présent dans une zone et modifie les variables de cette zone en fonction de cela
 * \param x position en x d'un des robot
 * \param y position en y d'un des robot
 * \param robot numéro du robot testé
 */

void test_zones(uint16_t x,uint16_t y,uint8_t robot){
	int i=0;
	static uint32_t current_timer1=0,last_timer1=0;
	static uint32_t current_timer2=0,last_timer2=0;
	static uint16_t x_prec=200;
	for (i=0;i<ZONE_NUMBER;i++){
		if (zone_tableau[i].robot==robot || zone_tableau[i].robot==0){
			if (x>=zone[i].x_min && x<=zone[i].x_max && y>=zone[i].y_min && y<=zone[i].y_max){
				zone_tableau[i].robot=robot;
				zone_tableau[i].entered = TRUE;
				switch (zone[i].analyse){
					case PASSAGE:
						break;
					case DURATION:
						if (zone_tableau[i].robot==1){
							current_timer1=zone_t_10ms;
							if (zone_tableau[i].current)zone_tableau[i].temps+=((current_timer1-last_timer1));
							last_timer1=current_timer1;
						}else {
							current_timer2=zone_t_10ms;
							if (zone_tableau[i].current)zone_tableau[i].temps+=((current_timer2-last_timer2));
							last_timer2=current_timer2;
						}

						break;
					case LOOKING_Y:
						if (x_prec>x) {
							zone_tableau[i].y_recherche=y;
							x_prec=x;
						}
						break;
					default :
						break;
				}
			zone_tableau[i].current = TRUE;
			}else {
				zone_tableau[i].robot=0;
				zone_tableau[i].current = FALSE;
				if(zone_tableau[i].entered) zone_tableau[i].exited=TRUE;
				switch (zone[i].analyse){
					case PASSAGE:
						break;
					case DURATION:
						break;
					case LOOKING_Y:
						break;
					default:
						break;
				}
			}
		}
	}
}
// IT d'interruption
//TODO : renommer cette fonction en ZONE_process_it_10ms
void ZONE_process_10ms(void)
{
	zone_t_10ms+=1;
}

#endif
