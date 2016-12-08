/*
 *  Club Robot ESEO 2016 - 2017
 *
 *  Fichier : stmpe811.c
 *  Package : Beacon Eye
 *  Description : Driver STMPE811 de l'écran tactile
 *  Auteur : Guillaume Berland inspiré du code d'Arnaud Guilmet
 *  Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20161208
 */

#include "stmpe811.h"
#include "ssd2119.h"
#include "../../QS/QS_i2c.h"
#include "../../QS/QS_external_it.h"
#include "../../stm32f4xx/stm32f4xx_i2c.h"


