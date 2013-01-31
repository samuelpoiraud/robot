/*
 *	Club Robot ESEO 2009 - 20010
 *	Chomp
 *
 *	Fichier : Can.h
 *	Package : Clickodrome
 *	Description : Permet de rendre lisible, propre et aisément évoluable la partie relative au CAN
 *	Auteur : Aurélie Ottavi
 *	Version 20091201
 */

#ifndef CAN_H
#define CAN_H

//RS232
#define MAX_NUM_PORT 32 //numéro maximum de port

//Délimiteur CAN
#define CAN_SOH 0x01
#define CAN_EOT 0x04

//Miltipoint
#define CAN_MULTI_POINTS 0x30

//Vitesses
#define CAN_LENT 0x02

//commande ASSER_TELL_POSITION sur 2 octets
#define CAN_ASSER_TELL_POSITION_1 0x01
#define CAN_ASSER_TELL_POSITION_2 0x05
//Timer ASSER_TELL_POSITION en ms
#define TIME_TELL_POSITION 500

//commande BEACON_POS sur 2 octets
#define CAN_BEACON_POS_1 0x02
#define CAN_BEACON_POS_2 0x50
//Timer BEACON_POS en ms
#define TIME_BEACON_POS 500

//commande GO_POSITION sur 2 octets
#define CAN_ASSER_GO_POSITION_1 0x01
#define CAN_ASSER_GO_POSITION_2 0x55


//commande GO_ANGLE sur 2 octets
#define CAN_ASSER_GO_ANGLE_1 0x01
#define CAN_ASSER_GO_ANGLE_2 0x77


#endif
