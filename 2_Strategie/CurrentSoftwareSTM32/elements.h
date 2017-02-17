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
		STOCK_POS_1_TO_OUT = 0,
		STOCK_POS_2_TO_OUT = 1,
		STOCK_POS_3_TO_OUT = 2,
		STOCK_POS_4_TO_OUT = 3,
		STOCK_POS_ELEVATOR = 4,
		STOCK_POS_ENTRY = 5
	}moduleStockPosition_e;

	typedef enum{
		STOCK_PLACE_ENTRY_TO_ELEVATOR,
		STOCK_PLACE_ELEVATOR_TO_CONTAINER_IN,
		STOCK_PLACE_CONTAINER_IN_TO_DISPENSER,
		STOCK_PLACE_DISPENSER_TO_CONTAINER_OUT
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

	extern const module_zone_characteristics_s module_zone[NB_MOONBASES];
	// COMMENTAIRE POUR COCO : module_zone[first_zone].xmin

	#define MAX_MODULE_MOONBASE		6
	#define MAX_MODULE_STOCK		6
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
void STOCKS_addModule(moduleType_e type, moduleStockLocation_e storage);
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

#endif // ELEMENTS_H
