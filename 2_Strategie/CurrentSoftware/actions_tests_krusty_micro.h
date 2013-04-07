/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech - CHOMP, Shark & Fish
 *
 *	Fichier : actions_tests_krusty_micro.h
 *	Package : Carte Principale
 *	Description : Test des actions réalisables par le robot
 *	Auteur : Cyril, modifié par Gonsevi
 *	Version 2012/01/14
 */

#include "QS/QS_all.h"

#ifndef ACTIONS_TESTS__KRUSTY_MICRO_H
#define ACTIONS_TESTS__KRUSTY_MICRO_H

#include "actions.h"
#include "Asser_functions.h"
#include "act_functions.h"

//Insérez ici vos micro_strat et découpes de strat
error_e K_CADEAU1(void);
error_e K_CADEAU2(void);
error_e K_CADEAU3(void);
error_e K_CADEAU4(void);

error_e K_ASSIETE1(void);
error_e K_ASSIETE2(void);
error_e K_ASSIETE3(void);
error_e K_ASSIETE4(void);
error_e K_ASSIETE5(void);

error_e K_verres(void);
error_e K_push_half_row_glasses(void);


error_e TEST_Launcher_ball_mid(void);
error_e TEST_Launcher_ball_cadeau(void);
error_e TEST_Launcher_ball_gateau(void);
#endif /* ACTIONS_TESTS__KRUSTY_MICRO_H */



