/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech - CHOMP, Shark & Fish
 *
 *	Fichier : actions_tests_krusty.h
 *	Package : Carte Principale
 *	Description : Test des actions réalisables par le robot
 *	Auteur : Cyril, modifié par Gonsevi
 *	Version 2012/01/14
 */

#include "../QS/QS_all.h"

#ifndef ACTIONS_TESTS_KRUSTY_H
#define ACTIONS_TESTS_KRUSTY_H
	
#include "actions.h"
#include "../asser_functions.h"
#include "../act_functions.h"
#include "actions_tests_krusty_micro.h"

void K_Strat_Pour_Les_Nuls(void);
void Deplacement_Pierre_Strat_Samuel(void);

void K_Strat_Coupe(void);
error_e Assiete_2_4_lanceur(void);
error_e Assiete_5_lanceur(void);

void K_test_strat_unitaire(void);

void TEST_STRAT_ALEXIS();
void TEST_STRAT_ALEXIS_FINALE();

void TEST_STRAT_K_homologation(void);
void K_TEST_STRAT_avoidance(void);
void TEST_STRAT_kdo(void);
void TEST_STRAT_K_homologation(void);

void TEST_START_avoidance_distance(); //Test de la detection d'adversaire, utilisez docklight pour voir si Foe 1 ou 2 est detecté
void K_TEST_STRAT_avoidance(void);//
void TEST_STRAT_kdo(void);
void TEST_STRAT_assiettes(void);
void TEST_STRAT_verres(void);


void TEST_STRAT_assiettes_evitement(void);
void TEST_STRAT_assiettes_lanceur(void);
void TEST_Launcher_ball(void);

void Test_STRAT_homolo(void);
void Test_STRAT_COUPE(void);
void Test_STRAT_COUPE_2(void);
error_e Assiete1_lanceur(void);
error_e Assiete2_lanceur(void);
error_e Assiete3_lanceur(void);
error_e Assiete4_lanceur(void);
error_e Assiete5_lanceur(void);
error_e Cadeau(void);

void TEST_STRAT_K_homologation(void);
void TEST_STRAT_avoidance(void);//
void TEST_STRAT_kdo(void);
void TEST_STRAT_assiettes(void);
void TEST_STRAT_verres(void);


void TEST_STRAT_assiettes_evitement(void);
void TEST_STRAT_assiettes_lanceur(void);
void TEST_Launcher_ball(void);

void Test_STRAT_homolo(void);
void Test_STRAT_COUPE(void);
void Test_STRAT_COUPE_2(void);
error_e Assiete1_lanceur(void);
error_e Assiete2_lanceur(void);
error_e Assiete3_lanceur(void);
error_e Assiete4_lanceur(void);
error_e Assiete5_lanceur(void);
error_e Cadeau(void);

#endif /* ACTIONS_TESTS_KRUSTY_H */
