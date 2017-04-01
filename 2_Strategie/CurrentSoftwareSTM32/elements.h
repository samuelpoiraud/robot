#ifndef ELEMENTS_H
#define ELEMENTS_H

#include "QS/QS_all.h"
#include "Supervision/SD/SD.h"
#include "propulsion/movement.h"


	typedef struct{
		Sint16 xmin;
		Sint16 xmax;
		Sint16 ymin;
		Sint16 ymax;
		bool_e enable_zone;
		Uint8 nb_cylinder_max;
	}module_zone_characteristics_s;

	typedef enum{
		NO_ELEMENT,
		OUR_ELEMENT,
		ADV_ELEMENT,
		NEUTRAL_ELEMENT
	}ELEMENTS_property_e;

	typedef enum{
		NO_SIDE,
		RIGHT,
		LEFT
	}ELEMENTS_side_e;

	typedef enum{
		OUR_SIDE,
		ADV_SIDE
	}ELEMENTS_side_match_e;

	typedef enum{
		MODULE_EMPTY = 0,
		MODULE_BLUE,
		MODULE_YELLOW,
		MODULE_POLY
	}moduleType_e;

	typedef enum{
		MODULE_OUR_START,
		MODULE_OUR_SIDE,
		MODULE_OUR_MID,
		MODULE_ADV_START,
		MODULE_ADV_SIDE,
		MODULE_ADV_MID,
		MODULE_OUR_ORE_UNI,
		MODULE_OUR_START_ZONE_UNI
	}ELEMENTS_modules_numbers_e;

	typedef enum{
		MODULE_MOONBASE_MIDDLE,
		MODULE_MOONBASE_OUR_CENTER,
		MODULE_MOONBASE_ADV_CENTER,
		MODULE_MOONBASE_OUR_SIDE,
		MODULE_MOONBASE_ADV_SIDE,
		NB_MOONBASES,
	}moduleMoonbaseLocation_e;

	typedef enum{
		MODULE_STOCK_LEFT,
		MODULE_STOCK_RIGHT,
		NB_STOCKS
	}moduleStockLocation_e;

	typedef enum{
		STOCK_POS_ENTRY = 0,
		STOCK_POS_ELEVATOR = 1,
		STOCK_POS_SLOPE = 2,
		STOCK_POS_CONTAINER = 3,
		STOCK_POS_BALANCER = 4,
		STOCK_POS_COLOR = 5,
		STOCK_POS_ARM_DISPOSE = 6,
	}moduleStockPosition_e;

	typedef enum{
		STOCK_PLACE_ENTRY_TO_ELEVATOR,
		STOCK_PLACE_ELEVATOR_TO_CONTAINER,
		STOCK_PLACE_CONTAINER_TO_BALANCER,
		STOCK_PLACE_BALANCER_TO_COLOR,
		STOCK_PLACE_COLOR_TO_ARM_DISPOSE,
		STOCK_PLACE_CLEAR_ARM_DISPOSE
	}moduleStockPlace_e;

	typedef enum{
		NO_DOMINATING,
		MODULE_MONO_DOMINATING,
		MODULE_POLY_DOMINATING,
	}moduleTypeDominating_e;

	typedef enum{
		MODULE_ROCKET_MULTI_OUR_SIDE,
		MODULE_ROCKET_MULTI_ADV_SIDE,
		MODULE_ROCKET_MONO_OUR_SIDE,
		NB_ROCKETS
	}moduleRocketLocation_e;

	typedef struct{
		ELEMENTS_modules_numbers_e numero;
		ELEMENTS_side_e side;
	}get_this_module_s;

	// Type défini pour la machine à état de dépose des cylindres
	typedef enum{
		ARG_DISPOSE_ONE_CYLINDER_FOLLOW_BY_ANOTHER,
		ARG_DISPOSE_ONE_CYLINDER_AND_FINISH,
		ARG_STORE_ARM
	}arg_dipose_mae_e;

//DEPOSE COTE:===========================================================================================
//MES VARIABLES:
	typedef enum{
	   OUEST = 0,
	   EST = 1
	}endroit_depose_config_e;
	endroit_depose_config_e endroit_depose;
	//

	typedef enum{
		NORD = 0,
		SUD = 1
	}POINT_DE_ROTATION_config_e;
	POINT_DE_ROTATION_config_e mon_point_de_rotation;


	typedef enum{
		DROITE = 0,
		GAUCHE = 1
	}COTE_DE_DEPOSE_config_e;
	COTE_DE_DEPOSE_config_e cote_depose;

	extern const module_zone_characteristics_s module_zone[NB_MOONBASES];
	// COMMENTAIRE POUR COCO : module_zone[first_zone].xmin

	#define MAX_MODULE_MOONBASE		6
	#define MAX_MODULE_STOCK		7
	#define MAX_MODULE_ROCKET       4

// Fonctions pour la gestion des flags
void ELEMENTS_init();
void ELEMENTS_process_main();
bool_e ELEMENTS_get_flag(elements_flags_e flag_id);
void ELEMENTS_set_flag(elements_flags_e flag_id, bool_e new_state);

// Fonctions pour la synchronisation
error_e ELEMENTS_check_communication(CAN_msg_t * msg);
#ifdef USE_SYNC_ELEMENTS
void ELEMENTS_receive_flags(CAN_msg_t* msg);
#endif

// Fonctions pour la gestion des hardflags
void ELEMENTS_receive_hardflags_from_xbee(CAN_msg_t * msg);


//Fonctions pour la gestion du stockage des modules
moduleTypeDominating_e STOCKS_getDominatingModulesType(moduleStockLocation_e storage);
Uint8 STOCKS_getNbModules(moduleStockLocation_e storage);
bool_e STOCKS_isFull(moduleStockLocation_e storage);
bool_e STOCKS_isEmpty(moduleStockLocation_e storage);
bool_e STOCKS_moduleStockPlaceIsEmpty(moduleStockPosition_e place, moduleStockLocation_e storage);
void STOCKS_addModule(moduleType_e type, moduleStockPosition_e position, moduleStockLocation_e storage);
void STOCKS_makeModuleProgressTo(moduleStockPlace_e place, moduleStockLocation_e storage);
moduleType_e STOCKS_removeModule(moduleStockLocation_e storage);
void STOCKS_print(moduleStockLocation_e storage);

// Fonction pour le vidage de la fusee
Uint8 ROCKETS_getNbModules(moduleRocketLocation_e rocket);
bool_e ROCKETS_isEmpty(moduleRocketLocation_e rocket);
void ROCKETS_removeModule(moduleRocketLocation_e rocket);
void ROCKETS_removeAllModules(moduleRocketLocation_e rocket);
void ROCKETS_print(moduleRocketLocation_e rocket);

//Fonctions pour la dépose
bool_e MOONBASES_modulePlaceIsEmpty(Uint8 place, moduleMoonbaseLocation_e location);
Uint8 MOONBASES_getNbModules(moduleMoonbaseLocation_e location);
moduleType_e MOONBASES_getModuleType(Uint8 place, moduleMoonbaseLocation_e location);
void MOONBASES_addModule(moduleType_e type, moduleMoonbaseLocation_e location);


// Retourne le flag correspondant au module passé en paramètre
elements_flags_e ELEMENTS_get_flag_module(ELEMENTS_modules_numbers_e module);

#endif // ELEMENTS_H
