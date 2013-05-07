/*  Club Robot ESEO 2012 - 2013
 *	Tiny
 *
 *	Fichier : Plier_config.h
 *	Package : Carte actionneur
 *	Description : Gestion des bras horizontaux de tiny
 *  Auteur : Alexis
 *  Version 20130312
 *  Robot : Tiny
 */

#ifndef TPLIER_CONFIG_H
#define	TPLIER_CONFIG_H

	#define PLIER_LEFT_AX12_MAX_TORQUE_PERCENT   50
	#define PLIER_LEFT_AX12_OPEN_POS             40  //TODO: à regler !!
	#define PLIER_LEFT_AX12_CLOSED_POS           223  //TODO: à regler !!

	#define PLIER_RIGHT_AX12_MAX_TORQUE_PERCENT  50
	#define PLIER_RIGHT_AX12_OPEN_POS            49  //TODO: à regler !!
	#define PLIER_RIGHT_AX12_CLOSED_POS          237  //TODO: à regler !!


	#define PLIER_AX12_ASSER_POS_EPSILON         5
	#define PLIER_AX12_ASSER_TIMEOUT             20  //en ms*100, 20 == 2 sec
	#define PLIER_AX12_POS_LARGE_EPSILON         50



#endif	/* TPLIER_CONFIG_H */

