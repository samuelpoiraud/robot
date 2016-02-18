/*
 * hokuyo_functions.c
 *
 *  Created on: 8 déc. 2013
 *      Author: HoObbes
 */
#if 0
#include "hokuyo.h"
	#include "QS/QS_outputlog.h"
	#include "QS/QS_maths.h"
	#include "QS/QS_sys.h"
	#include "usb_host/Core/usbh_core.h"
	#include "usb_host/usbh_usr.h"
	#include "usb_host/OTG/usb_hcd_int.h"
	#include "usb_host/OTG/usb_bsp.h"
	#include "usb_host/Class/CDC/usbh_cdc_core.h"
	#include "usb_host/Class/MSC/usbh_msc_core.h"
	#include "usb_host/Class/usbh_class_switch.h"
	#include "uart_via_usb.h"


typedef enum{
	HOKUYO_ACQ_DISTANCE_LIGHT_3_BYTE,
	HOKUYO_ACQ_DISTANCE_3_BYTE,
	HOKUYO_ACQ_DISTANCE_2_BYTE,
    HOKUYO_ACQ_LAST_DISTANCE_3_BYTE,
    HOKUYO_ACQ_LAST_DISTANCE_2_BYTE,
    HOKUYO_ACQ_SWITCH_ON,                           // Led verte clignotante : OFF / Led verte constante : ON
    HOKUYO_ACQ_SWITCH_OFF,
    HOKUYO_ACQ_RESET,                               // Reset : Vitesse moteur / Bit rate / Internal timer | Exctinction du laser
    HOKUYO_ACQ_TIME_ADJUST,
    HOKUYO_ACQ_BIT_RATE_CHANGE,
    HOKUYO_ACQ_MOTOR_SPEED_ADJUST,
    HOKUYO_ACQ_SWITCH_LIGHT_SENSIBILITY,            // Firmware > 3.2.00 | Hautes sensibilitées peut générer des erreurs.
    HOKUYO_ACQ_ERROR_SIMULATION,
    HOKUYO_ACQ_ASK_TECHNICAL_INFORMATION,
    HOKUYO_ACQ_ASK_TECHNICAL_SHEET,
    HOKUYO_ACQ_ASK_STATE,
	NUMBER_HOKUYO_COMMAND
}hokuyo_command_type_e;

char * hukoyo_command_text[NUMBER_HOKUYO_COMMAND] = {
    {"ME"},
    {"MD"},
    {"MS"},
    {"GD"},
    {"GS"},
    {"BM"},
    {"QT"},
    {"RS"},
    {"TM"},
    {"SS"},
    {"CR"},
    {"HS"},
    {"DB"},
    {"VV"},
    {"PP"},
    {"II"}
};

typedef struct{

	hokuyo_command_type_e command;

    union{

        struct{
            Uint16 startingStep;
            Uint16 endStep;
            Uint8 clusterCount;
            Uint8 scanInterval;
            Uint8 numberOfScan;
        }distance_light_3_byte;

        struct{
            Uint16 startingStep;
            Uint16 endStep;
            Uint8 clusterCount;
            Uint8 scanInterval;
            Uint8 numberOfScan;
        }distance_3_byte;

        struct{
            Uint16 startingStep;
            Uint16 endStep;
            Uint8 clusterCount;
            Uint8 scanInterval;
            Uint8 numberOfScan;
        }distance_2_byte;

        struct{
            Uint16 startingStep;
            Uint16 endStep;
            Uint8 clusterCount;
        }last_distance_3_byte;

        struct{
            Uint16 startingStep;
            Uint16 endStep;
            Uint8 clusterCount;
        }last_distance_2_byte;

	}cmd_u;

}hokuyo_command_s;

#define VERBOSE_DEBUG_HOKUYO

// Temps d'acquisition
// MS : 45 - 50  ms
// ME : 70 - 100 ms
	//#define USE_COMMAND_ME	// afin de récupérer la distance et l'intensité, sinon récupére juste la distance

	#define HOKUYO_BUFFER_READ_TIMEOUT	500		// ms

#ifdef USE_COMMAND_ME
	#define NB_BYTES_FROM_HOKUYO	6750
#else
	#define NB_BYTES_FROM_HOKUYO	2500
#endif

	__ALIGN_BEGIN USB_OTG_CORE_HANDLE      USB_OTG_Core __ALIGN_END;
	__ALIGN_BEGIN USBH_HOST                USB_Host __ALIGN_END;


	static Uint8 HOKUYO_datas[NB_BYTES_FROM_HOKUYO]__attribute__((section(".ccm")));				//Données brutes issues du capteur HOKUYO
	static Uint32 datas_index=0;									//Index pour ces données

	static bool_e hokuyo_initialized = FALSE;						//Module initialisé - sécurité.
	volatile bool_e flag_device_disconnected = FALSE;				//Flag levé en callback lorsque le capteur vient d'être débranché
	volatile Uint16 time_since_last_sent_adversaries_datas = 0;		//[ms]

	void hokuyo_write_command(Uint8 tab[]);
	int hokuyo_write_uart_manually(void);
	bool_e hokuyo_read_buffer(void);
	void hokuyo_format_data(void);


//Fonction d'initialisation du périphérique USB
void HOKUYO_init(void)
{

	if(!hokuyo_initialized)
	{
		debug_printf("\nInit usb for Hokuyo\n");

		UART_USB_init(19200);
		USBH_Init(&USB_OTG_Core, USB_OTG_FS_CORE_ID, &USB_Host, &USBH_CDC_cb, &USR_cb);
	}
	hokuyo_initialized = TRUE;
}


void HOKUYO_process_it(Uint8 ms)
{
	time_since_last_sent_adversaries_datas += ms;	//on assume le débordement au bout de 65 secondes si l'hokuyo est inopérant... c'est pas grave..
}

//Process main
void HOKUYO_process_main(void)
{
	typedef enum
	{
		INIT=0,
		HOKUYO_WAIT,
		ASK_NEW_MEASUREMENT,
		BUFFER_READ,
		REMOVE_LF,
		TREATMENT_DATA,
		DETECTION_ADVERSARIES,
		SEND_ADVERSARIES_DATAS,
		ERROR,
		RESET_HOKUYO,
		RESET_ACKNOWLEDGE,
		TURN_ON_LASER,
		TURN_OFF_LASER,
		DONE
	}state_e;
	static state_e state = INIT, last_state = INIT;
	bool_e entrance;
	static time32_t buffer_read_time_begin = 0;
	static bool_e i_planted = FALSE;

	if((state == INIT && last_state == INIT) || state != last_state)
		entrance = TRUE;
	else
		entrance = FALSE;

	last_state = state;

	//Process main du périphérique USB.
	USBH_Process(&USB_OTG_Core, &USB_Host);

	if(flag_device_disconnected)
	{
		flag_device_disconnected = FALSE;
		hokuyo_initialized = FALSE;
		HOKUYO_init();
		state = INIT;
	}

	switch(state)	//MAE.
	{
		case INIT:
			if(hokuyo_initialized)	//Si pas initialisé, on reste ici..
				state = HOKUYO_WAIT;
		break;
		case HOKUYO_WAIT:
			if(USBH_CDC_is_ready_to_run())
				state=ASK_NEW_MEASUREMENT;
		break;
		case ASK_NEW_MEASUREMENT:

#ifdef USE_COMMAND_ME
			hokuyo_write_command((Uint8*)"ME0000108001001"); // Distance et intensité Codé sur 3 code encoding chacun
#else
			hokuyo_write_command((Uint8*)"MS0000108001001"); // Distance Codé sur 2 code encoding
#endif
			robot_position_during_measurement = global.position;	//On garde en mémoire l'angle du robot au moment où on lance la mesure Hokuyo. (est-ce le moment le plus pertinent...?)
			datas_index=0;
			state=BUFFER_READ;
		break;
		case BUFFER_READ:
			if(entrance)
				buffer_read_time_begin = global.absolute_time;
			if(hokuyo_read_buffer())
			{
				if(datas_index>=2257)//2274)
					state = REMOVE_LF;
				else
				{
					//Uint8 i;
#ifdef VERBOSE_DEBUG_HOKUYO
					/*debug_printf("datas_index = %ld\n",datas_index);
					for(i=0;i<datas_index;i++)
					{
						debug_printf("%c",HOKUYO_datas[i]);
					}*/
					debug_printf("ec%d\n",HOKUYO_datas[17]);
#endif
					state=ASK_NEW_MEASUREMENT;
				}
			}

#ifdef USE_COMMAND_ME
#warning "le code ci dessous, dans le mode ME n'est pas débogué... il est dégeu."
			if(datas_index > 1 && HOKUYO_datas[datas_index-2]==0x0A && HOKUYO_datas[datas_index-1]==0x0A && datas_index>=6730) // 0x0A -> LF (en ASCII)
				state=REMOVE_LF;
			else if(datas_index>6738)
				state=ASK_NEW_MEASUREMENT;
			else if(global.absolute_time - buffer_read_time_begin > HOKUYO_BUFFER_READ_TIMEOUT)
				state=ASK_NEW_MEASUREMENT;
#else
			/*if(datas_index > 1 && HOKUYO_datas[datas_index-2]==0x0A && HOKUYO_datas[datas_index-1]==0x0A && datas_index>=2274)
				state=REMOVE_LF;
			else if(datas_index>2278)
			{
				//i_planted = TRUE;
				state=ASK_NEW_MEASUREMENT;
			}
			*/
			else if(global.absolute_time - buffer_read_time_begin > HOKUYO_BUFFER_READ_TIMEOUT)
			{
				//i_planted = TRUE;
				state=ASK_NEW_MEASUREMENT;
				//flag_device_disconnected = TRUE;
			}
#endif
		break;
		case REMOVE_LF:
			if(i_planted)
			{
				i_planted = FALSE;
			}
			hokuyo_format_data();
			state=TREATMENT_DATA;
		break;
		case TREATMENT_DATA:
			hokuyo_find_valid_points();
			#ifdef TRIANGULATION
			//debug_printf("Pouet %s\n", (global.match_over)?"over":"");
			//debug_printf("nbpoints1=%ld  nbpoints2=%ld  nbpoints3=%ld \n", nb_points_B1, nb_points_B2, nb_points_B3);
			//display(nb_passage);
			if(global.match_over){
				//Hokuyo_validPointsAndBeacons();
				debug_printf("\n##############Entrer dans TRI POINTS############## \n");
				nb_balayages++;
				debug_printf("nb_balayages=%d\n", nb_balayages );
				tri_points();

				if(nb_balayages==NB_MESURES_HOKUYO){
					debug_printf("\n##########Debut détection centres balises##########\n");
					find_beacons_centres();
					//nb_passage++;
					//nb_balayages=0;

				}
			}
			#endif
			state=DETECTION_ADVERSARIES;
		break;
		case DETECTION_ADVERSARIES:
			//hokuyo_dist_min(nb_pts_terrain);
			//hokuyo_detection_ennemis(nb_pts_terrain,&nb_ennemis);
			DetectRobots();
			Compute_dist_and_teta();
			//ReconObjet(nb_pts_terrain);
			state=SEND_ADVERSARIES_DATAS;
		break;
		case SEND_ADVERSARIES_DATAS:
			if(time_since_last_sent_adversaries_datas > PERIOD_SEND_ADVERSARIES_DATAS)
			{
				time_since_last_sent_adversaries_datas = 0;
				send_adversaries_datas();
			}
			refresh_adversaries();
			state=ASK_NEW_MEASUREMENT;
			break;
		case ERROR:	//Never Happen !!!
			debug_printf("ERROR SEQUENCE INITIALIZING");
			state=RESET_HOKUYO;
		break;
		case RESET_HOKUYO:
			hokuyo_write_command((Uint8*)"RS");
			datas_index = 0;
			state=RESET_ACKNOWLEDGE;
		break;
		case RESET_ACKNOWLEDGE:
			hokuyo_read_buffer();
			if(HOKUYO_datas[datas_index-2]==0x0A && HOKUYO_datas[datas_index-1]==0x0A)
				state=TURN_ON_LASER;
			else
				state=TURN_OFF_LASER;
		break;
		case TURN_ON_LASER:
			hokuyo_write_command((Uint8*)"BM");
			state=DONE;
		break;
		case TURN_OFF_LASER:
			hokuyo_write_command((Uint8*)"QT");
			state=DONE;
		break;
		case DONE:
		break;
		default:
		break;
	}
}



/**
  * @brief  OTG_FS_IRQHandler
  *          This function handles USB-On-The-Go FS global interrupt request.
  *          requests.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_OTG_FS
void OTG_FS_IRQHandler(void)
#else
void OTG_HS_IRQHandler(void)
#endif
{
	if (USB_OTG_IsHostMode(&USB_OTG_Core)) /* ensure that we are in device mode */
	{
		USBH_OTG_ISR_Handler(&USB_OTG_Core);
	}
	else
	{
		//USB Device.
		//USBD_OTG_ISR_Handler(&USB_OTG_Core);
	}
}




//Fonction qui permet de communiquer avec l'Hokuyo
void hokuyo_write_command(Uint8 tab[])
{
	Uint32 i;
	for(i=0;tab[i];i++)
		UART_USB_write(tab[i]);
	UART_USB_write(0x0A);
}

//Manual mode
int hokuyo_write_uart_manually(void)
{
	int ret=0;
	while(UART1_data_ready())
	{
		UART_USB_write(UART1_get_next_msg());
		ret++;
	}
	return ret;
}

//Fonction qui permet de lire le buffer
bool_e hokuyo_read_buffer(void)
{
	static Uint8 previous_data = 0;
	Uint8 data;
	while(!UART_USB_isRxEmpty())
	{
		data = UART_USB_read();
		HOKUYO_datas[datas_index] = data;
		if(datas_index < NB_BYTES_FROM_HOKUYO)
			datas_index++;
		else
			fatal_printf("HOKUYO : overflow in hokuyo_read_buffer !\n");
		if(data == 0x0A && previous_data == 0x0A)	//On interrompt la réception des octets lorsque l'on reçoit deux LF consécutifs.
			return TRUE;
		previous_data = data;	//On sauvegarde la data pour le prochain passage dans cette boucle.
			//NB : cette sauvegarde n'est pas faite, mais est inutile si on a effectué le return TRUE. (inutile car previous_data vaut déjà 0x0A)
	}
	return FALSE;
}

//Fonction qui formate les données afin d'etre traitées.
void hokuyo_format_data(void)
{
	/*int j=47;
	int i=0;
	while(j<=2274)
	{
		if(HOKUYO_datas[j+1]=='\n')
			j+=2;
		HOKUYO_datas[i]=HOKUYO_datas[j];
		//debug_printf("%c",tab[i]);
		i++;
		j++;
	}
	datas_index = i-1;*/
}

void user_callback_DeviceDisconnected(void)
{
	flag_device_disconnected = TRUE;
}

//Retourne si le module logiciel HOKUYO a envoyé des positions adverses récemment = preuve de bon fonctionnement pour le selftest de la propulsion
bool_e HOKUYO_is_working_well(void)
{
	if(time_since_last_sent_adversaries_datas < 2*PERIOD_SEND_ADVERSARIES_DATAS)
		return TRUE;
	else
		return FALSE;
}

#endif
