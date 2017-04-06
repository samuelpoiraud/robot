#include "elements.h"
#include "environment.h"

#define LOG_PREFIX "element: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_ELEMENTS
#include "QS/QS_outputlog.h"
#include "QS/QS_IHM.h"
#include "QS/QS_can_over_xbee.h"
#include "QS/QS_watchdog.h"
#include "utils/actionChecker.h"
#include "QS/QS_stateMachineHelper.h"
#include "utils/generic_functions.h"

typedef struct{
	Uint8 nbCurrentModules;
	moduleType_e moonbaseModules[MAX_MODULE_MOONBASE];
}moduleMoonbaseInfo_s;

typedef struct{
	moduleType_e stockModules[MAX_MODULE_STOCK];
	Uint8 nbCurrentModules;
	Uint8 nbModulesMulticolor;
	Uint8 nbModulesMonocolor;
	moduleTypeDominating_e dominatingModules;
}moduleStockInfo_s;

typedef struct{
	moduleType_e rocketModules[MAX_MODULE_ROCKET];
	Uint8 nbCurrentModules;
}moduleRoketInfo_s;

typedef struct{
	bool_e sending;
	bool_e receiving;
	time32_t lastUpdate;
	bool_e flag;
}hardflag_s;

#define HARDFLAGS_NB  (F_ELEMENTS_HRDFLAGS_END - F_ELEMENTS_HARDFLAGS_START - 1)
#define HARDFLAGS_PERIOD  	(1000)
#define HARDFLAGS_TIMEOUT  	(HARDFLAGS_PERIOD + 200)

static volatile bool_e elements_flags[F_ELEMENTS_FLAGS_NB];
static volatile moduleMoonbaseInfo_s moduleMoonbaseInfo[NB_MOONBASES] = {0};
static volatile moduleStockInfo_s moduleStockInfo[NB_STOCKS] = {0};
static volatile moduleRoketInfo_s moduleRocketInfo[NB_ROCKETS] = {0};
static volatile hardflag_s elements_hardflags[HARDFLAGS_NB];
static void ROCKETS_init();
static void STOCKS_init();
static void MOONBASES_init();


const module_zone_characteristics_s module_zone[NB_MOONBASES] = {
		{.xmin=0, .xmax=2000, .ymin=0, .ymax=3000, .enable_zone=FALSE, .nb_cylinder_max=6},		// MODULE_DROP_MIDDLE
		{.xmin=0, .xmax=2000, .ymin=0, .ymax=3000, .enable_zone=FALSE, .nb_cylinder_max=6},		// MODULE_DROP_NORTH_CENTER
		{.xmin=0, .xmax=2000, .ymin=0, .ymax=3000, .enable_zone=FALSE, .nb_cylinder_max=4},		// MODULE_DROP_SOUTH_CENTER
		{.xmin=0, .xmax=2000, .ymin=0, .ymax=3000, .enable_zone=FALSE, .nb_cylinder_max=6},		// MODULE_DROP_NORTH
		{.xmin=0, .xmax=2000, .ymin=0, .ymax=3000, .enable_zone=FALSE, .nb_cylinder_max=4}		// MODULE_DROP_SOUTH
};

const module_zone_characteristics_s central = {.xmin=0, .xmax=2000, .ymin=0, .ymax=3000, .enable_zone=FALSE, .nb_cylinder_max=6};

const module_zone_characteristics_s our_diagonal = {.xmin=0, .xmax=2000, .ymin=0, .ymax=3000, .enable_zone=FALSE, .nb_cylinder_max=6};

const module_zone_characteristics_s our_side = {.xmin=0, .xmax=2000, .ymin=0, .ymax=3000, .enable_zone=FALSE, .nb_cylinder_max=4};

const module_zone_characteristics_s adv_diagonal = {.xmin=0, .xmax=2000, .ymin=0, .ymax=3000, .enable_zone=FALSE, .nb_cylinder_max=6};

const module_zone_characteristics_s adv_side = {.xmin=0, .xmax=2000, .ymin=0, .ymax=3000, .enable_zone=FALSE, .nb_cylinder_max=4};

#ifdef USE_HARDFLAGS
	static void ELEMENTS_receive_hardflags();
	static void ELEMENTS_send_hardflags();
	static void ELEMENTS_send_hardflags_to_xbee();
#endif

void ELEMENTS_init(){
	Uint8 i;

	for(i=0;i<F_ELEMENTS_FLAGS_NB;i++)
	{
		elements_flags[i] = FALSE;
	}

	#ifdef USE_HARDFLAGS
		for(i=0;i<HARDFLAGS_NB;i++)
		{
			elements_hardflags[i].sending = FALSE;
			elements_hardflags[i].receiving = FALSE;
			elements_hardflags[i].lastUpdate = global.absolute_time;
		}
	#endif

	// Initialisation des fusées
	ROCKETS_init();

	// Initialisation des stocks des robots
	STOCKS_init();

	// Initialisation des bases de construction lunaire
	MOONBASES_init();
}

void ELEMENTS_process_main(){
	#ifdef USE_HARDFLAGS
		ELEMENTS_send_hardflags();
		ELEMENTS_receive_hardflags();
	#endif
}

bool_e ELEMENTS_get_flag(elements_flags_e flag_id)
{
	assert(flag_id < F_ELEMENTS_FLAGS_NB);
	return elements_flags[flag_id];
}

void ELEMENTS_set_flag(elements_flags_e flag_id, bool_e new_state)
{
	assert(flag_id < F_ELEMENTS_FLAGS_NB);
	elements_flags[flag_id] = new_state;

#ifdef USE_SYNC_ELEMENTS
	if(flag_id < F_ELEMENTS_FLAGS_END_SYNCH)
	{
		CAN_msg_t msg;
		msg.sid = XBEE_SYNC_ELEMENTS_FLAGS;
		msg.size = SIZE_XBEE_SYNC_ELEMENTS_FLAGS;
		msg.data.xbee_sync_elements_flags.flagId = flag_id;
		msg.data.xbee_sync_elements_flags.flag = new_state;
		CANMsgToXbee(&msg,FALSE);
	}
#endif

#ifdef USE_HARDFLAGS
	if(flag_id > F_ELEMENTS_HARDFLAGS_START && flag_id < F_ELEMENTS_HRDFLAGS_END)
	{
		elements_hardflags[flag_id - F_ELEMENTS_HARDFLAGS_START - 1].sending = new_state;
	}
#endif
}


//################################## SYNCHRONISATION #################################
#ifdef USE_SYNC_ELEMENTS
void ELEMENTS_receive_flags(CAN_msg_t* msg)
{
	if(msg->data.xbee_sync_elements_flags.flagId < F_ELEMENTS_FLAGS_END_SYNCH){
		//debug_printf("\n\n\n\nReception message Xbee\n\n\n\n");
		elements_flags[msg->data.xbee_sync_elements_flags.flagId] = msg->data.xbee_sync_elements_flags.flag;
	}
}
#endif

#define	TIMEOUT_ANSWER	200

error_e ELEMENTS_check_communication(CAN_msg_t * msg)
{
#ifdef USE_SYNC_ELEMENTS
	CREATE_MAE(INIT,
			SEND_REQUEST,
			WAIT_FOR_ANSWER,
			WAIT_TIMEOUT,
			TIMEOUT,
			ANSWER_RECEIVED,
			END);
	static bool_e watchdog_flag = FALSE;
	static watchdog_id_t watchdog_id = 0;
	static time32_t local_time = 0;

	switch(state)
	{
		case INIT:
			if(msg == NULL)	//On s'autorise à continuer seulement si ce n'est pas la réception d'un message qui nous active...
				state = SEND_REQUEST;
			break;
		case SEND_REQUEST:{
			CAN_msg_t request;
			request.sid = XBEE_COMMUNICATION_AVAILABLE;
			request.size = 0;
			CANMsgToXbee(&request,FALSE);
			state = WAIT_FOR_ANSWER;
			break;}
		case WAIT_FOR_ANSWER:
			if(entrance)
			{
				watchdog_id = WATCHDOG_create_flag(TIMEOUT_ANSWER,&watchdog_flag);
			}
			if(watchdog_flag)
			{
				state = WAIT_TIMEOUT;
			}
			else if(msg)
			{
				if(msg->sid == XBEE_COMMUNICATION_RESPONSE)
				{
					ELEMENTS_set_flag(F_COMMUNICATION_AVAILABLE, TRUE);
					state = ANSWER_RECEIVED;
					WATCHDOG_stop(watchdog_id);
				}
			}
			break;
		case ANSWER_RECEIVED:
			state = END;
			break;

		case WAIT_TIMEOUT:
			if(entrance){
				local_time = global.absolute_time;
			}
			if(global.absolute_time > local_time + 1000){
				state = TIMEOUT;
			}
			break;
		case TIMEOUT:
			RESET_MAE();
			ELEMENTS_set_flag(F_COMMUNICATION_AVAILABLE, FALSE);
			return END_WITH_TIMEOUT;
			break;
		case END:
			RESET_MAE();
			return END_OK;
			break;
		default:
			RESET_MAE();
			break;
	}
	#endif
	return IN_PROGRESS;

}

//################################## HARDFLAGS #################################
#ifdef USE_HARDFLAGS
	static void ELEMENTS_send_hardflags_to_xbee(){
		CAN_msg_t request;
		request.sid = XBEE_ELEMENTS_HARDFLAGS;
		Uint8 i, j;

		// Send flags by XBee
		i = 0;
		while(i < HARDFLAGS_NB){
			j = 0;
			while(j < 8){
				if(elements_hardflags[i].sending) {
					request.data.xbee_elements_hardflags.flagId[j] = i;
					j++;
				}
				i++;
			}
			if(j > 0){
				request.size = j;
				CANMsgToXbee(&request,FALSE);
			}
		}
	}

	static void ELEMENTS_send_hardflags()
	{
		CREATE_MAE(SEND_REQUEST,
					WAIT_FOR_NEXT_REQUEST
					);
		static time32_t timeLastSending = 0;

		switch(state)
		{
			case SEND_REQUEST:
				ELEMENTS_send_hardflags_to_xbee();
				timeLastSending = global.absolute_time;
				state = WAIT_FOR_NEXT_REQUEST;
				break;
			case WAIT_FOR_NEXT_REQUEST:
				if(global.absolute_time > timeLastSending + HARDFLAGS_PERIOD){
					state = SEND_REQUEST;
				}
				break;
			default:
				RESET_MAE();
				break;
		}
	}

	void ELEMENTS_receive_hardflags_from_xbee(CAN_msg_t * msg)
	{
		Uint8 i;
		Uint8 index = 0;
		if(msg->sid == XBEE_ELEMENTS_HARDFLAGS) {
			for(i = 0; i < msg->size; i++){
				index = msg->data.xbee_elements_hardflags.flagId[i];
				elements_hardflags[index].receiving = TRUE;
				elements_hardflags[i].lastUpdate = global.absolute_time;
				if(elements_hardflags[index].sending){
					error_printf("ERROR Hardflag : This hardflag %d could not be used by both robots\n", index);
				}
			}
		}
	}

	static void ELEMENTS_receive_hardflags()
	{
		Uint8 i;
		for(i = 0; i < HARDFLAGS_NB; i++){
			if(elements_hardflags[i].receiving){
				if(!elements_hardflags[i].flag){
					elements_hardflags[i].flag = TRUE;
					ELEMENTS_set_flag(F_ELEMENTS_HARDFLAGS_START + 1 + i, TRUE);
				}
				elements_hardflags[i].receiving = FALSE;
			}else if(elements_hardflags[i].lastUpdate > HARDFLAGS_TIMEOUT){
				if(elements_hardflags[i].flag){
					elements_hardflags[i].flag = FALSE;
					ELEMENTS_set_flag(F_ELEMENTS_HARDFLAGS_START + 1 + i, FALSE);
				}
			}
		}
	}
#endif


//###################################  STOCKAGE DES MODULES DANS LE ROBOT ################################

// Initialisation des stocks du robot
static void STOCKS_init(){
	Uint8 i, j;
	for(i = 0; i < NB_STOCKS; i++){
		moduleStockInfo[i].nbCurrentModules = 0;
		moduleStockInfo[i].nbModulesMonocolor = 0;
		moduleStockInfo[i].nbModulesMulticolor = 0;
		moduleStockInfo[i].dominatingModules = NO_DOMINATING;

		for(j = 0; j < MAX_MODULE_STOCK; j++){
			moduleStockInfo[i].stockModules[j] = MODULE_EMPTY;
		}
	}
}

// Calul du type de modules dominant
static void STOCKS_calculModuleDominant(moduleStockLocation_e storage){
	if(moduleStockInfo[storage].nbModulesMonocolor > moduleStockInfo[storage].nbModulesMulticolor){
		moduleStockInfo[storage].dominatingModules = MODULE_MONO_DOMINATING;
	}
	else if (moduleStockInfo[storage].nbModulesMonocolor < moduleStockInfo[storage].nbModulesMulticolor){
		moduleStockInfo[storage].dominatingModules = MODULE_POLY_DOMINATING;
	}
	else{
		moduleStockInfo[storage].dominatingModules = NO_DOMINATING;
	}
}

// Getter du type de modules dominant
moduleTypeDominating_e STOCKS_getDominatingModulesType(moduleStockLocation_e storage){
	return moduleStockInfo[storage].dominatingModules;
}

// Getter du nombre de module dans un des stocks du robot
Uint8 STOCKS_getNbModules(moduleStockLocation_e storage){
	return moduleStockInfo[storage].nbCurrentModules;
}

// Permet de savoir si un des stocks du robot est plein
// On ne prend pas en compte les positions STOCK_POS_COLOR et STCOK_POS_ARM_DISPOSE
bool_e STOCKS_isFull(moduleStockLocation_e storage){
	if(moduleStockInfo[storage].stockModules[STOCK_POS_ENTRY] != MODULE_EMPTY
			&& moduleStockInfo[storage].stockModules[STOCK_POS_ELEVATOR] != MODULE_EMPTY
			&& moduleStockInfo[storage].stockModules[STOCK_POS_SLOPE] != MODULE_EMPTY
			&& moduleStockInfo[storage].stockModules[STOCK_POS_CONTAINER] != MODULE_EMPTY
			&& moduleStockInfo[storage].stockModules[STOCK_POS_BALANCER] != MODULE_EMPTY
	){
		return TRUE;
	}else{
		return FALSE;
	}
}

// Permet de savoir si un des stocks du robot est vide
bool_e STOCKS_isEmpty(moduleStockLocation_e storage){
	return moduleStockInfo[storage].nbCurrentModules == 0;
}

bool_e STOCKS_moduleStockPlaceIsEmpty(moduleStockPosition_e place, moduleStockLocation_e storage){
	assert(place < MAX_MODULE_STOCK);
	if(moduleStockInfo[storage].stockModules[place] != MODULE_EMPTY){
		return FALSE;
	} else {
		return TRUE;
	}
}

// Permet d'ajouter un module dans un des stocks du robot
void STOCKS_addModule(moduleType_e type, moduleStockPosition_e position, moduleStockLocation_e storage){

	if((position == STOCK_POS_ENTRY || position == STOCK_POS_ELEVATOR) && moduleStockInfo[storage].stockModules[position] == MODULE_EMPTY){
		if(type == MODULE_BLUE || type == MODULE_YELLOW){
			moduleStockInfo[storage].nbModulesMonocolor++;
		}
		else if(type == MODULE_POLY){
			moduleStockInfo[storage].nbModulesMulticolor++;
		}

		//Stockage du module en position STOCK_POS_ENTRY ou STOCK_POS_ELEVATOR
		moduleStockInfo[storage].stockModules[position] = type;
		moduleStockInfo[storage].nbCurrentModules++;

		// Recalcul du type de modules dominant après ajout de ce nouveau module
		STOCKS_calculModuleDominant(storage);
	}else{
		// En cas d'erreur, on essaie de faire au mieux
		moduleStockInfo[storage].stockModules[position] = type;
		STOCKS_calculModuleDominant(storage);

		error_printf("ERROR STOCKAGE MODULE in STOCK_addModule\n");
	}
}

void STOCKS_makeModuleProgressTo(moduleStockPlace_e place, moduleStockLocation_e storage){
	switch(place){
		case STOCK_PLACE_ENTRY_TO_ELEVATOR:
			if(moduleStockInfo[storage].stockModules[STOCK_POS_ELEVATOR] == MODULE_EMPTY){
				// On change le module de place
				moduleStockInfo[storage].stockModules[STOCK_POS_ELEVATOR] = moduleStockInfo[storage].stockModules[STOCK_POS_ENTRY];
				moduleStockInfo[storage].stockModules[STOCK_POS_ENTRY] = MODULE_EMPTY;
			}else{
				error_printf("ERROR STOCKAGE MODULE in STOCK_makeModuleProgressTo STOCK_PLACE_ELEVATOR\n");
			}
			break;

		case STOCK_PLACE_ELEVATOR_TO_CONTAINER:
			if(moduleStockInfo[storage].stockModules[STOCK_POS_SLOPE] == MODULE_EMPTY){
				// On change le module de place
				// Considération : On considère que le BALANCER est la plupart du temps en position rentrée dans le robot
				if(moduleStockInfo[storage].stockModules[STOCK_POS_BALANCER] == MODULE_EMPTY){
					moduleStockInfo[storage].stockModules[STOCK_POS_BALANCER] = moduleStockInfo[storage].stockModules[STOCK_POS_ELEVATOR];
				}else if(moduleStockInfo[storage].stockModules[STOCK_POS_CONTAINER] == MODULE_EMPTY){
					moduleStockInfo[storage].stockModules[STOCK_POS_CONTAINER] = moduleStockInfo[storage].stockModules[STOCK_POS_ELEVATOR];
				}else{	// STOCK_POS_SLOPE == MODULE_EMPTY
					moduleStockInfo[storage].stockModules[STOCK_POS_SLOPE] = moduleStockInfo[storage].stockModules[STOCK_POS_ELEVATOR];
				}
				moduleStockInfo[storage].stockModules[STOCK_POS_ELEVATOR] = MODULE_EMPTY;
			}else{
				error_printf("ERROR STOCKAGE MODULE in STOCK_makeModuleProgressTo STOCK_PLACE_CONTAINER\n");
			}
			break;

		case STOCK_PLACE_CONTAINER_TO_BALANCER:
			// Ce "if" est important pour la considération du "case" précédent
			if(moduleStockInfo[storage].stockModules[STOCK_POS_BALANCER] == MODULE_EMPTY){
				moduleStockInfo[storage].stockModules[STOCK_POS_BALANCER] = moduleStockInfo[storage].stockModules[STOCK_POS_CONTAINER];
				moduleStockInfo[storage].stockModules[STOCK_POS_CONTAINER] = moduleStockInfo[storage].stockModules[STOCK_POS_SLOPE];
				moduleStockInfo[storage].stockModules[STOCK_POS_SLOPE] = MODULE_EMPTY;
			}else{
				error_printf("ERROR STOCKAGE MODULE in STOCK_makeModuleProgressTo STOCK_PLACE_CONTAINER_IN_TO_DISPENSER\n");
			}
			break;

		case STOCK_PLACE_BALANCER_TO_COLOR:
			if(moduleStockInfo[storage].stockModules[STOCK_POS_COLOR] == MODULE_EMPTY){
				moduleStockInfo[storage].stockModules[STOCK_POS_COLOR] = moduleStockInfo[storage].stockModules[STOCK_POS_BALANCER];
				moduleStockInfo[storage].stockModules[STOCK_POS_BALANCER] = MODULE_EMPTY;
			}else{
				error_printf("ERROR STOCKAGE MODULE in STOCK_makeModuleProgressTo STOCK_PLACE_DISPENSER_TO_CONTAINER_OUT\n");
			}
			break;


		case STOCK_PLACE_COLOR_TO_ARM_DISPOSE:
			if(moduleStockInfo[storage].stockModules[STOCK_POS_ARM_DISPOSE] == MODULE_EMPTY){
				moduleStockInfo[storage].stockModules[STOCK_POS_ARM_DISPOSE] = moduleStockInfo[storage].stockModules[STOCK_POS_COLOR];
				STOCKS_removeModule(storage);	// On supprime le cylindre du stock
			}else{
				error_printf("ERROR STOCKAGE MODULE in STOCK_makeModuleProgressTo STOCK_CONTAINER_OUT_TO_OUTSIDE\n");
			}
			break;

		case STOCK_PLACE_CLEAR_ARM_DISPOSE:
			moduleStockInfo[storage].stockModules[STOCK_POS_ARM_DISPOSE] = MODULE_EMPTY;
			break;

		default:
			error_printf("ERROR STOCKAGE MODULE in STOCK_makeModuleProgressTo default case\n");
			break;
	}
}



// Permet de supprimer un module dans un des stocks du robot
moduleType_e STOCKS_removeModule(moduleStockLocation_e storage){
	if(moduleStockInfo[storage].stockModules[STOCK_POS_COLOR] != MODULE_EMPTY){
		moduleType_e module = moduleStockInfo[storage].stockModules[STOCK_POS_COLOR];

		if(module == MODULE_BLUE || module == MODULE_YELLOW){
			moduleStockInfo[storage].nbModulesMonocolor--;
		}
		else if(module == MODULE_POLY){
			moduleStockInfo[storage].nbModulesMulticolor--;
		}
		// Le module sur le retourneur de couleur vient d'être déposé
		moduleStockInfo[storage].stockModules[STOCK_POS_COLOR] = MODULE_EMPTY;
		moduleStockInfo[storage].nbCurrentModules--;
		return module;
	}
	else{
		// On vient de déposer un cylindre qui n'est pas là, on ne fait rien
		return MODULE_EMPTY;
	}
}

void STOCKS_print(moduleStockLocation_e storage){
	Uint8 i;
	debug_printf("\n\n Affichage STOCK :\n");
	for(i = 0; i < MAX_MODULE_STOCK; i++){
		switch(moduleStockInfo[storage].stockModules[i]){
			case MODULE_EMPTY:
				debug_printf(" - MODULE_EMPTY\n");
				break;
			case MODULE_BLUE:
				debug_printf(" - MODULE_BLUE\n");
				break;
			case MODULE_YELLOW:
				debug_printf(" - MODULE_YELLOW\n");
				break;
			case MODULE_POLY:
				debug_printf(" - MODULE_POLY\n");
				break;
		}
	}
	debug_printf("\n");
}

//###################################  GESTION DES MODULES DANS LES FUSEES ###############################

// Initialisation des fusées
static void ROCKETS_init(){
	color_e color = global.color;
	for(Uint8 rocket = 0; rocket < NB_ROCKETS; rocket++){
		switch(rocket){
			case MODULE_ROCKET_MULTI_OUR_SIDE:
			case MODULE_ROCKET_MULTI_ADV_SIDE:
				for(Uint8 i=0; i < MAX_MODULE_ROCKET; i++){
					moduleRocketInfo[rocket].rocketModules[i] = MODULE_POLY;
				}
				moduleRocketInfo[rocket].nbCurrentModules = MAX_MODULE_ROCKET;
				break;
			case MODULE_ROCKET_MONO_OUR_SIDE:
				for(Uint8 i=0; i < MAX_MODULE_ROCKET; i++){
					if(color == BLUE){
						moduleRocketInfo[rocket].rocketModules[i] = MODULE_BLUE;
					}else{
						moduleRocketInfo[rocket].rocketModules[i] = MODULE_YELLOW;
					}
				}
				moduleRocketInfo[rocket].nbCurrentModules = MAX_MODULE_ROCKET;
				break;
		}
	}
}

// Getter du nombre de modules dansn une fusée
Uint8 ROCKETS_getNbModules(moduleRocketLocation_e rocket){
	return moduleRocketInfo[rocket].nbCurrentModules;
}

// Permet de savoir si une fusée est vide
bool_e ROCKETS_isEmpty(moduleRocketLocation_e rocket){
	return moduleRocketInfo[rocket].nbCurrentModules == 0;
}

// Enlève un module de la fusée
void ROCKETS_removeModule(moduleRocketLocation_e rocket){
	moduleRocketInfo[rocket].nbCurrentModules--;
	moduleRocketInfo[rocket].rocketModules[moduleRocketInfo[rocket].nbCurrentModules] = MODULE_EMPTY;
}

// Enlève tous les modules de la fusée
void ROCKETS_removeAllModules(moduleRocketLocation_e rocket){
	moduleRocketInfo[rocket].nbCurrentModules = 0;
	for(Uint8 i=0; i < MAX_MODULE_ROCKET; i++){
		moduleRocketInfo[rocket].rocketModules[i] = MODULE_EMPTY;
	}
}

void ROCKETS_print(moduleRocketLocation_e rocket){
	Uint8 i;
	debug_printf("\n\n Affichage ROCKET :\n");
	for(i = 0; i < MAX_MODULE_ROCKET; i++){
		switch(moduleRocketInfo[rocket].rocketModules[i]){
			case MODULE_EMPTY:
				debug_printf(" - MODULE_EMPTY\n");
				break;
			case MODULE_BLUE:
				debug_printf(" - MODULE_BLUE\n");
				break;
			case MODULE_YELLOW:
				debug_printf(" - MODULE_YELLOW\n");
				break;
			case MODULE_POLY:
				debug_printf(" - MODULE_POLY\n");
				break;
		}
	}
	debug_printf("\n");
}



//############################  STOCKAGE DES MODULES DANS LES BASES DE CONSTRUCTION ######################

// Initialisation des bases de construction lunaire
static void MOONBASES_init(){
	Uint8 i;
	for(i = 0; i < NB_MOONBASES; i++){
		moduleMoonbaseInfo[i].nbCurrentModules = 0;
	}
}

// Permet de savoir si un emplacement d'une base de construction lunaire est vide
bool_e MOONBASES_modulePlaceIsEmpty(Uint8 place, moduleMoonbaseLocation_e location){
	assert(place < MAX_MODULE_MOONBASE);
	assert(location < NB_MOONBASES);
	return (moduleMoonbaseInfo[location].moonbaseModules[place] == MODULE_EMPTY);
}

// Getter du nombre de modules dans une base de construction lunaire
Uint8 MOONBASES_getNbModules(moduleMoonbaseLocation_e location){
	return moduleMoonbaseInfo[location].nbCurrentModules;
}

// Getter du type d'un module à un emplacement d'une base de construction lunaire
moduleType_e MOONBASES_getModuleType(Uint8 place, moduleMoonbaseLocation_e location){
	assert(place < MAX_MODULE_MOONBASE);
	assert(location < NB_MOONBASES);
	return moduleMoonbaseInfo[location].moonbaseModules[place];
}

// Permet d'ajouter un module dans une base de construction lunaire
void MOONBASES_addModule(moduleType_e type, moduleMoonbaseLocation_e location){
	Uint8 i;
	for(i=0; i < MAX_MODULE_MOONBASE - 1; i++){
		moduleMoonbaseInfo[location].moonbaseModules[i+1] = moduleMoonbaseInfo[location].moonbaseModules[i];
	}
	moduleMoonbaseInfo[location].moonbaseModules[0] = type;
}


elements_flags_e ELEMENTS_get_flag_module(ELEMENTS_modules_numbers_e module){
	assert(module <= 7);
	elements_flags_e flag = 0;
	switch(module){
		case MODULE_OUR_START :
			flag = FLAG_OUR_MULTICOLOR_START_IS_TAKEN;
			break;
		case MODULE_OUR_SIDE :
			flag = FLAG_OUR_MULTICOLOR_SIDE_IS_TAKEN;
			break;
		case MODULE_OUR_MID :
			flag = FLAG_OUR_MULTICOLOR_NEAR_DEPOSE_IS_TAKEN;
			break;
		case MODULE_ADV_START :
			flag = FLAG_ADV_MULTICOLOR_START_IS_TAKEN;
			break;
		case MODULE_ADV_SIDE :
			flag = FLAG_ADV_MULTICOLOR_SIDE_IS_TAKEN;
			break;
		case MODULE_ADV_MID :
			flag = FLAG_ADV_MULTICOLOR_NEAR_DEPOSE_IS_TAKEN;
			break;
		case MODULE_OUR_ORE_UNI :
			flag = FLAG_OUR_UNICOLOR_SOUTH_IS_TAKEN;
			break;
		case MODULE_OUR_START_ZONE_UNI :
			flag = FLAG_OUR_UNICOLOR_NORTH_IS_TAKEN;
			break;
		default:
			break;
	}
	return ELEMENTS_get_flag(flag);
}
