
/*  Club Robot ESEO 2012 - 2013
 *	Krusty
 *
 *	Fichier : Lift.c
 *	Package : Carte actionneur
 *	Description : Gestion des ascenseurs
 *  Auteur : Alexis
 *  Version 20130314
 *  Robot : Krusty
 */

#include "Arm_data.h"

ARM_motor_data_t arm_motors[ARM_NB_ACT] = {

};

arm_state_t arm_states[ARM_ST_NUMBER] = {
	{0, 0, 0, 0},
	{0, 0, 0, 0}
};

Uint8 arm_states_transitions[ARM_ST_NUMBER][ARM_ST_NUMBER] = {
	{0, 1},
	{1, 0}
};
