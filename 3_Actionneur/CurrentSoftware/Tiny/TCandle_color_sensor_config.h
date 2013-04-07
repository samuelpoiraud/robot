/*  Club Robot ESEO 2012 - 2013
 *	Tiny
 *
 *  Fichier : KPlate_config.h
 *  Package : Actionneur
 *  Description : Configuration du bras qui souffle les bougies
 *  Auteur : Alexis
 *  Version 20130505
 *  Robot : Tiny
 */

#ifndef TCANDLE_COLOR_SENSOR_CONFIG_H
#define	TCANDLE_COLOR_SENSOR_CONFIG_H

	//Detection digitale des couleurs (entre 0 et 3 inclus)
	//#define CANDLECOLOR_CW_USE_DIGITAL   //Si défini, on utilise la detection digitale des couleurs. Si aucune n'est detectée, une detection analogique sera faite si activé
	#define CANDLECOLOR_CW_CHANNEL_BLUE         0
	#define CANDLECOLOR_CW_CHANNEL_RED          1
//	#define CANDLECOLOR_CW_CHANNEL_WHITE        2
//	#define CANDLECOLOR_CW_CHANNEL_YELLOW       3

	//Detection analogique des couleurs
	#define CANDLECOLOR_CW_USE_ANALOG   //Si défini, on utilise la detection analogique des couleurs (seulement si aucune couleur n'ont té detecté par le mode digital)
	//Pour les couleurs, voir http://www.efg2.com/Lab/Graphics/Colors/CIE1931.jpg
	//Les valeurs sont en pourcentage*1024
	//Defines dans l'ordre de detection, la premiere est prioritaire
	//TODO: ajuster les valeurs
//	#define CANDLECOLOR_CW_YELLOW_MIN_xy        409   //valeur mini de x et y pour pouvoir detecter du jaune (CANDLECOLOR_CW_YELLOW_MAX_DIFF_xy doit corespondre aussi pour detecter du jaune)
//	#define CANDLECOLOR_CW_YELLOW_MAX_DIFF_xy   51    //différence maximale entre x et y  pour pouvoir detecter du jaune
	#define CANDLECOLOR_CW_RED_MIN_x            512   //valeur mini de x pour detecter du rouge (x+y+z=1)
	#define CANDLECOLOR_CW_BLUE_MIN_z           512   //valeur mini de z pour detecter du bleue
//	#define CANDLECOLOR_CW_GREEN_MIN_y          512   //valeur mini de y pour detecter du vert (considéré comme OTHER car on n'utilise pas de vert)
	#define CANDLECOLOR_CW_WHITE_MIN_Y          512   //Valeur mini de Y pour detecter du blanc (il ne faut pas qu'on detecte une autre couleur pour le detecter)
//	#define CANDLECOLOR_CW_WHITE_MIN_L          512   //Valeur mini de L (X+Y+Z) pour detecter du blanc (a tester VS Y pour la valeur la plus pertinante)

	#define CANDLECOLOR_AX12_TIMEOUT            30  //en centaine de ms, une valeur de 20 correspond à 2secondes
	#define CANDLECOLOR_AX12_POS_EPSILON        5
	#define CANDLECOLOR_AX12_HIGH_CANDLE_POS    0  //TODO: a changer
	#define CANDLECOLOR_AX12_LOW_CANDLE_POS     0
	#define CANDLECOLOR_AX12_ANGLE_MIN          0
	#define CANDLECOLOR_AX12_ANGLE_MAX          300
	#define CANDLECOLOR_AX12_MAX_TORQUE_PERCENT 100

	#define CANDLECOLOR_CW_DEBUG_COLOR

#endif	/* TCANDLE_COLOR_SENSOR_CONFIG_H */

