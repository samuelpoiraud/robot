/*
 *	Club Robot ESEO 2014 - 2015
 *	Holly & Wood
 *
 *	Fichier : QS_can_verbose.h
 *	Package : Qualité Soft
 *	Description : Verbosité des messages CAN sur uart
 *	Auteur : Arnaud
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100111
 */

#ifndef QS_CAN_VERBOSE_H
#define	QS_CAN_VERBOSE_H

#include "QS_all.h"
#include "../QS/QS_CANmsgList.h"

typedef enum{
	VERB_INPUT_MSG,
	VERB_OUTPUT_MSG,
	VERB_LOG_MSG
}QS_VERBOSE_msg_type_e;

/*
	Affiche le message can sous la forme d'un texte intelligible.
*/
void QS_CAN_VERBOSE_can_msg_print(CAN_msg_t * can_msg, QS_VERBOSE_msg_type_e verbose_msg_type);


#endif	/* def QS_CAN_VERBOSE_H*/
