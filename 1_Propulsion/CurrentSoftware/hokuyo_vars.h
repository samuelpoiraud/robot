/*
 * hokuyo_vars.h
 *
 *  Created on: 8 déc. 2013
 *      Author: HoObbes
 */

#ifndef HOKUYO_VARS_H_
#define HOKUYO_VARS_H_

	#define HOKUYO_OFFSET 4500 //45 degrés
	#define HOKUYO_ANGLE_ROBOT_TERRAIN 0
	#define HOKUYO_DETECTION_MARGE 130
	#define HOKUYO_EVITEMENT_MIN 150
	#define HOKUYO_BORDS_TERRAIN 80
	#define ROBOT_COORDX 130
	#define ROBOT_COORDY 180

	typedef struct{
		Sint32 dist;
		int teta;
		Sint32 coordX;
		Sint32 coordY;
	}HOKUYO_data;

	typedef struct{
			Sint32 coordX;
			Sint32 coordY;
	}HOKUYO_ennemy;


	HOKUYO_data hokuyo_sushi[1080];
	HOKUYO_ennemy hokuyo_ennemi[1080];


	void HOKUYO_process_main(void);

	void hokuyo_usb_init(void);
	void HOKUYO_kikujiro(void);
	void hokuyo_write_command(Uint8 tab[]);
	int hokuyo_write_uart_manually(void);
	void hokuyo_read_buffer(char *tab,Uint32 *i);
	void hokuyo_format_data(char *tab);
	void hokuyo_traitement_data(char *tabvaleurs,Uint16 *compt_sushis);

	Sint32 hokuyo_dist_min(Uint16 compt);

	//affichage du robot sur le terrain
	void affichage_robot(void);

	//Deux fonctions pour detecter des regroupements de points
	void hokuyo_detection_ennemis(Uint16 compt_sushis,int *nb_ennemi);
	void ReconObjet(Uint16 compt_sushis,int *nb_ennemi);
	void DetectRobots(Uint16 compt_sushis,int *nb_ennemi);

	//affichage des ennemis
	void affichage_ennemis(int nb_ennemi);

	//affichage de tous les pts
	void affichage_pts(Uint16 nb_pts);









#endif /* HOKUYO_VARS_H_ */
