/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech - CHOMP, Shark & Fish
 *
 *	Fichier : actions_tests.h
 *	Package : Carte Principale
 *	Description : Test des actions réalisables par le robot
 *	Auteur : Cyril, modifié par Gonsevi
 *	Version 2012/01/14
 */

#include "QS/QS_all.h"

#ifndef ACTIONS_TESTS_H
#define ACTIONS_TESTS_H
	
#include "actions.h"
#include "Asser_functions.h"
#include "act_functions.h"


#ifdef MODE_HOMOLOGATION
/**
* marque les points d'une bouteille et parcours ensuite les noeuds pour l'évitement
*
*/
//void TEST_STRAT_homolagation();
#endif

#ifdef ACTIONS_TEST_C
 	#include "telemeter.h"
        #include "QS/QS_CANmsgList.h"
#endif /* def ACTION_TEST_C */



//Prototypes de fonctions
//Homologation
void TEST_STRAT_HOMOLOGATION(void);
void TEST_STRAT_homolagation_police(void);
error_e Test_Homologation_Sortie_Base(void); 
error_e Test_Homologation_Point(void);


void test_qui_sert_pas();
//Tests
void TEST_STRAT_strat_selector_1(void);
void TEST_STRAT_strat_selector_2(void);
void TEST_STRAT_strat_selector_3(void);
void TEST_STRAT_lever_le_kiki(void);
void TEST_STRAT_premier_deplacement(void);
error_e TEST_STRAT_gift1(void);  //begin by micky
error_e TEST_STRAT_gift2(void);
error_e TEST_STRAT_gift3(void);
error_e TEST_STRAT_gift4(void);
//end by micky

//par amaury
void test_strat_verrre(void);

error_e TEST_ACT(void);
void TEST_STRAT_assiettes_evitement(void);
error_e TEST_STRAT_PREMIERE_ASSIETTE_EVITEMENT(void);
error_e TEST_STRAT_SECONDE_ASSIETTE_EVITEMENT(void);
error_e TEST_STRAT_TROISIEME_ASSIETTE_EVITEMENT(void);
error_e TEST_STRAT_QUATRIEME_ASSIETTE_EVITEMENT(void);
error_e TEST_STRAT_CINQUIEME_ASSIETTE_EVITEMENT(void);
error_e TEST_LAUNCH_BALL(void);
error_e TEST_STRAT_CADEAU_1(void);
error_e TEST_STRAT_CADEAU_2(void);
error_e TEST_STRAT_CADEAU_3(void);
error_e TEST_STRAT_CADEAU_4(void);
//fini par amaury

//begin by micky
error_e TEST_STRAT_verres(void);
void TEST_STRAT_gateau(void);
void TEST_STRAT_gatoacsuba(void);
error_e TEST_STRAT_in_da_wall(void);
error_e TEST_STRAT_first_half_cake(void);
error_e TEST_STRAT_second_half_cake(void);
void set_calage(void);
void levi(void);  //end by micky

//Alexis, test act_function
void TEST_STRAT_balllauncher_run();

void strat_verre_assiettes_et_gateau(); //begin by Julien

error_e TEST_STRAT_VERRE1(void);
error_e TEST_STRAT_VERRE2(void);

error_e STRAT_DEBUT_ASSIETTE(void);
error_e STRAT_FIN_ASSIETTE(void);
error_e STRAT_PREMIERE_ASSIETTE(void);
error_e STRAT_SECONDE_ASSIETTE(void);
error_e STRAT_TROISIEME_ASSIETTE(void);
error_e STRAT_DERNIERE_ASSIETTE(void);

error_e STRAT_TIMEOUT_ASSIETTE(void); //end by Julien

#endif /* ACTIONS_TESTS_H */
