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
		RIGHT,
		LEFT
	}ELEMENTS_side_e;

	typedef enum{
		OUR_SIDE,
		ADV_SIDE
	}ELEMENTS_side_match_e;


	typedef struct{
		GEOMETRY_point_t pos;
		ELEMENTS_property_e property;
	}COQUILLAGES_t;

	typedef enum{
	   NO_COQUILLAGES_CONFIG = 0,
	   CONFIG_COQUILLAGES_1,
	   CONFIG_COQUILLAGES_2,
	   CONFIG_COQUILLAGES_3,
	   CONFIG_COQUILLAGES_4,
	   CONFIG_COQUILLAGES_5
	}COQUILLAGES_config_e;

	typedef enum{
		MODULE_EMPTY = 0,
		MODULE_BLUE,
		MODULE_YELLOW,
		MODULE_POLY
	}moduleType_e;

	typedef enum{
		MODULE_DROP_MIDDLE,
		MODULE_DROP_OUR_CENTER,
		MODULE_DROP_ADV_CENTER,
		MODULE_DROP_OUR_SIDE,
		MODULE_DROP_ADV_SIDE,
		NB_MODULE_LOCATION,
	}moduleDropLocation_e;

	typedef enum{
		MODULE_STORAGE_LEFT,
		MODULE_STORAGE_RIGHT,
		NB_PLACE_STORAGE
	}moduleStorageLocation_e;

	typedef enum{
		NO_DOMINATING,
		MODULE_MONO_DOMINATING,
		MODULE_POLY_DOMINATING,
	}moduleTypeDominating_e;

	typedef enum{
		FUSEE_MULTI_OUR_SIDE,
		FUSEE_MULTI_ADV_SIDE,
		FUSEE_MONO_OUR_SIDE,
		NB_FUSEE
	}moduleFuseeLocation_e;

	extern const module_zone_characteristics_s module_zone[NB_MODULE_LOCATION];
	// COMMENTAIRE POUR COCO : module_zone[first_zone].xmin

void ELEMENTS_init();
void ELEMENTS_process_main();

bool_e ELEMENTS_get_flag(elements_flags_e flag_id);
void ELEMENTS_set_flag(elements_flags_e flag_id, bool_e new_state);

error_e ELEMENTS_check_communication(CAN_msg_t * msg);

#ifdef USE_SYNC_ELEMENTS
void ELEMENTS_receive_flags(CAN_msg_t* msg);
#endif

// Hardflags
void ELEMENTS_receive_hardflags_from_xbee(CAN_msg_t * msg);


//Fonctions pour le stockage
moduleTypeDominating_e dominatingTypeStock(moduleStorageLocation_e storage);

bool_e moduleStockPlaceIsEmpty(Uint8 place, moduleStorageLocation_e storage);

Uint8 nbModulesStock(moduleStorageLocation_e storage);

void addModuleStock(moduleType_e type, moduleStorageLocation_e storage);

moduleType_e releaseModuleStock(moduleType_e type, moduleStorageLocation_e storage);

// Fonction pour le vidage de la fusee

void remplirFusee(moduleFuseeLocation_e fusee);

Uint8 nbModulesFusee(moduleFuseeLocation_e fusee);

void enleveModuleFusee(moduleStorageLocation_e fusee);


//Fonctions pour la dépose

bool_e modulePlaceIsEmpty(Uint8 place, moduleDropLocation_e location);

Uint8 getNbDrop(moduleDropLocation_e location);

moduleType_e getModuleType(Uint8 place, moduleDropLocation_e location);

void addModule(moduleType_e type, moduleDropLocation_e location);

#endif // ELEMENTS_H
