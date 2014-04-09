/*
 * hokuyo_functions.c
 *
 *  Created on: 8 déc. 2013
 *      Author: HoObbes
 */

#include "main.h"
	#include "QS/QS_ports.h"
	#include "QS/QS_uart.h"
	#include "QS/QS_buttons.h"
	#include "QS/QS_who_am_i.h"
	#include "QS/QS_outputlog.h"
	#include "QS/QS_CANmsgList.h"
	#include "QS/QS_who_am_i.h"
	#ifdef STM32F40XX
		#include "QS/QS_sys.h"
	#endif
	#include "usb_host/Core/usbh_core.h"
	#include "usb_host/usbh_usr.h"
	#include "usb_host/OTG/usb_hcd_int.h"
	#include "usb_host/OTG/usb_bsp.h"
	#include "usb_host/Class/CDC/usbh_cdc_core.h"
	#include "usb_host/Class/MSC/usbh_msc_core.h"
	#include "usb_host/Class/usbh_class_switch.h"
	#include "uart_via_usb.h"
	#include "cos_sin.h"
	#include "hokuyo.h"
	#include "calculator.h"
	#include "secretary.h"

#ifdef USE_HOKUYO

	#define HOKUYO_OFFSET_ANGLE_RAD4096 (9651) //135 degrés
	#define HOKUYO_ANGLE_ROBOT_TERRAIN 0
	#define HOKUYO_DETECTION_MARGE 130
	#define HOKUYO_EVITEMENT_MIN 150
	#define HOKUYO_MARGIN_FIELD_SIDE_IGNORE 80

	#define GROS_ROBOT_HOKUYO_TOO_CLOSE_DISTANCE_IGNORE		200	//Distance d'un point trop proche de nous qui doit être ignoré.
	#define PETIT_ROBOT_HOKUYO_TOO_CLOSE_DISTANCE_IGNORE	150	//Distance d'un point trop proche de nous qui doit être ignoré.

	#define ROBOT_COORDX 150
	#define ROBOT_COORDY 150
	#define DISTANCE_POINTS_IN_THE_SAME_OBJECT 150
	#define NB_MAX_ADVERSARIES	16
	#define NB_BYTES_FROM_HOKUYO	5000
	#define PERIOD_SEND_ADVERSARIES_DATAS	200	//[ms]

	__ALIGN_BEGIN USB_OTG_CORE_HANDLE      USB_OTG_Core __ALIGN_END;
	__ALIGN_BEGIN USBH_HOST                USB_Host __ALIGN_END;

	typedef struct{
		Sint32 dist;
		int teta;
		Sint32 coordX;
		Sint32 coordY;
	}HOKUYO_adversary_position;


	static Uint8 HOKUYO_datas[NB_BYTES_FROM_HOKUYO];				//Données brutes issues du capteur HOKUYO
	static Uint32 datas_index=0;									//Index pour ces données

	#define NB_DETECTED_VALID_POINTS	1000
	static HOKUYO_adversary_position detected_valid_points[NB_DETECTED_VALID_POINTS];	//Points valides détectés par le capteur (X, Y, teta, distance)
	static Uint16 nb_valid_points=0;								//Nombre de points valides détectés

	static HOKUYO_adversary_position hokuyo_adversaries[NB_MAX_ADVERSARIES];	//Positions des adversaires détectés
	static Uint8 adversaries_number=0;											//Nombre d'adversaires détectés

	static bool_e hokuyo_initialized = FALSE;						//Module initialisé - sécurité.
	volatile bool_e flag_device_disconnected = FALSE;				//Flag levé en callback lorsque le capteur vient d'être débranché
	volatile Uint16 time_since_last_sent_adversaries_datas = 0;		//[ms]
	static position_t robot_position_during_measurement;

	void hokuyo_write_command(Uint8 tab[]);
	int hokuyo_write_uart_manually(void);
	void hokuyo_read_buffer(void);
	void hokuyo_format_data(void);
	void hokuyo_find_valid_points(void);
	Sint32 hokuyo_dist_min(Uint16 compt);
	void Compute_dist_and_teta(void);

	//Deux fonctions pour detecter des regroupements de points
	void hokuyo_detection_ennemis(void);
	//void ReconObjet(void);
	void DetectRobots(void);
	void send_adversaries_datas(void);




//Fonction d'initialisation du périphérique USB
void HOKUYO_init(void)
{

	if(!hokuyo_initialized)
	{
		//USBH_Process(&USB_OTG_Core, &USB_Host);
		UART_USB_init(19200);
		debug_printf("\nInit usb for Hokuyo\n");
		USBH_Init(&USB_OTG_Core,
		  #ifdef USE_USB_OTG_FS
			  USB_OTG_FS_CORE_ID,
		  #else
			  USB_OTG_HS_CORE_ID,
		  #endif
			  &USB_Host,
			  &USBH_CDC_cb,
			  &USR_cb);
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
	static state_e state = INIT;

	//Process main du périphérique USB.
	USBH_Process(&USB_OTG_Core, &USB_Host);


	if(flag_device_disconnected)
	{
		flag_device_disconnected = FALSE;
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
			hokuyo_write_command((Uint8*)"MS0000108001001");
			robot_position_during_measurement = global.position;	//On garde en mémoire l'angle du robot au moment où on lance la mesure Hokuyo. (est-ce le moment le plus pertinent...?)
			datas_index=0;
			state=BUFFER_READ;
		break;
		case BUFFER_READ:
			hokuyo_read_buffer();
			if(HOKUYO_datas[datas_index-2]==0x0A && HOKUYO_datas[datas_index-1]==0x0A && datas_index>=2274)
				state=REMOVE_LF;
			else if(datas_index>2278)
				state=ERROR;
		break;
		case REMOVE_LF:
			hokuyo_format_data();
			state=TREATMENT_DATA;
		break;
		case TREATMENT_DATA:
			hokuyo_find_valid_points();
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
			state=ASK_NEW_MEASUREMENT;
			break;
		case ERROR:
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
void hokuyo_read_buffer(void)
{
	while(!UART_USB_isRxEmpty())
	{
		HOKUYO_datas[datas_index] = UART_USB_read();
		//debug_printf("%c",tab[*i]);
		//UART1_putc(tab[*i]);
		if(datas_index < NB_BYTES_FROM_HOKUYO)
			datas_index++;
		else
			fatal_printf("HOKUYO : overflow in hokuyo_read_buffer !\n");
	}
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

//Fonction qui renvoi les coordonnées des points detectés sur le terrain
void hokuyo_find_valid_points(void)
{
	Uint16 a,b;
	Uint16 i;
	Sint32 distance;
	Sint32 angle=0;		//[°*100] centièmes de degrés
	Sint16 teta_relative;	//[rad4096]
	Sint16 teta_absolute;
	Sint32 x_absolute;
	Sint32 y_absolute;
	Sint16 cos;
	Sint16 sin;
	Sint32 to_close_distance;
	bool_e point_filtered;
	nb_valid_points = 0;	//RAZ des points valides.
	
	if(QS_WHO_AM_I_get() == PIERRE)
		to_close_distance = GROS_ROBOT_HOKUYO_TOO_CLOSE_DISTANCE_IGNORE;
	else
		to_close_distance = PETIT_ROBOT_HOKUYO_TOO_CLOSE_DISTANCE_IGNORE;

	//TODO mesurer la durée d'exécution de cet algo...
	for(i = 47; i<datas_index-3;)	//Les données commencent  l'octet 47...
	{
		if(HOKUYO_datas[i+1] == '\n')
			i+=2;
		if(HOKUYO_datas[i] == '\n')	//FIN DES DONNEES !!
			break;
		a = (Uint16)HOKUYO_datas[i++];
		b = (Uint16)HOKUYO_datas[i++];

		distance = ((a-0x30)<<6)+((b-0x30)&0x3f);	//cf datasheet de l'hokuyo... pour comprendre comment les données sont codées.
		if(distance	> to_close_distance)	//On élimine est distances trop petites (ET LES CAS DE REFLEXIONS TORP GRANDE OU LE CAPTEUR RENVOIE 1 !)
		{
			teta_relative = ((((Sint32)(angle))*183)>>8) + HOKUYO_OFFSET_ANGLE_RAD4096;	//Angle relatif au robot, du point en cours, en rad4096
			teta_relative = CALCULATOR_modulo_angle(teta_relative);
			teta_absolute = CALCULATOR_modulo_angle(teta_relative + robot_position_during_measurement.teta);				//angle absolu par rapport au terrain, pour le pt en cours, en rad4096

			COS_SIN_4096_get(teta_absolute,&cos,&sin);
			x_absolute = (distance*(Sint32)(cos))/4096 + robot_position_during_measurement.x;
			y_absolute = (distance*(Sint32)(sin))/4096 + robot_position_during_measurement.y;

			if(		x_absolute 	< 	FIELD_SIZE_X - HOKUYO_MARGIN_FIELD_SIDE_IGNORE &&
					x_absolute	>	HOKUYO_MARGIN_FIELD_SIDE_IGNORE &&
					y_absolute 	< 	FIELD_SIZE_Y - HOKUYO_MARGIN_FIELD_SIDE_IGNORE &&
					y_absolute 	>	HOKUYO_MARGIN_FIELD_SIDE_IGNORE)
			{		//Un point est retenu s'il est sur le terrain

				point_filtered = FALSE;	//On suppose que le point n'est pas filtré

				#define MAMOUTH_RECTANGLES_X		300
				#define MAMOUTH_RECTANGLES_Y_START	400
				#define MAMOUTH_RECTANGLES_Y_STOP	1100
				#define CORNER_SQUARE				150

				//On va éliminer certaines zones volontairement.
				if(x_absolute > FIELD_SIZE_X - CORNER_SQUARE)
					if(y_absolute < CORNER_SQUARE || y_absolute > FIELD_SIZE_Y - CORNER_SQUARE)	//Foyers des deux cotés
						point_filtered = TRUE;	//on refuse les points
				if(x_absolute < MAMOUTH_RECTANGLES_X)
					if (  	(y_absolute > MAMOUTH_RECTANGLES_Y_START && y_absolute < MAMOUTH_RECTANGLES_Y_STOP) 	//zones de dépose devant mamouths
						|| 	(y_absolute > FIELD_SIZE_Y - MAMOUTH_RECTANGLES_Y_STOP && y_absolute < FIELD_SIZE_Y - MAMOUTH_RECTANGLES_Y_START)	)
						point_filtered = TRUE;	//on refuse les points
				if(point_filtered == FALSE)
				{
					detected_valid_points[nb_valid_points].dist = distance;
					detected_valid_points[nb_valid_points].teta = teta_relative;	//L'angle enregistré permet l'évitement, c'est l'angle relatif !!!!!
					detected_valid_points[nb_valid_points].coordX = x_absolute;
					detected_valid_points[nb_valid_points].coordY = y_absolute;
					if(nb_valid_points < NB_DETECTED_VALID_POINTS)
						nb_valid_points++;
				}
			}
		}
		angle+=25;	//Centième de degré
	}
	//debug_printf("val tot [%d]\n",compt_sushis);
	//debug_printf("val bizarres [%d]\n",nb_val_bizarres);
}

//fonction qui renvoie la plus petite distance
Sint32 hokuyo_dist_min(Uint16 compt)
{
	Uint16 i;
	Sint32 dist_min;
	dist_min = detected_valid_points[0].dist;
	for(i=1;i<compt;i++)
	{
		if(detected_valid_points[i].dist < dist_min)
			dist_min = detected_valid_points[i].dist;
	}
	debug_printf("dist min [%ld mm] \n",dist_min);
	//if(dist_min < HOKUYO_EVITEMENT_MIN)
		//LCD_DisplayStringLineColon(10,LCD_LINE_27,"WARNING ENNEMY DETECTED");
	return dist_min;
}

//fonctions de detection des ennemis
void hokuyo_detection_ennemis(void){
	Uint16 i;
	Sint32 x_comp;
	Sint32 y_comp;
	Sint32 moyenne_x;
	Sint32 moyenne_y;

	x_comp=detected_valid_points[0].coordX;
	y_comp=detected_valid_points[0].coordY;
	adversaries_number = 0;
	for(i=0;i<nb_valid_points;i++)
	{
		if(	!(	detected_valid_points[i].coordX <= (x_comp+HOKUYO_DETECTION_MARGE)  	&&
				detected_valid_points[i].coordX >= (x_comp-HOKUYO_DETECTION_MARGE)	) &&
			!(detected_valid_points[i].coordY <= (y_comp+HOKUYO_DETECTION_MARGE)  	&&
				detected_valid_points[i].coordY >= (y_comp-HOKUYO_DETECTION_MARGE)	))
		{
				moyenne_x=(x_comp+detected_valid_points[i-1].coordX)/2;
				moyenne_y=(y_comp+detected_valid_points[i-1].coordY)/2;
				hokuyo_adversaries[adversaries_number].coordX=moyenne_x;
				hokuyo_adversaries[adversaries_number].coordY=moyenne_y;
				if(adversaries_number < NB_MAX_ADVERSARIES - 1)
					adversaries_number++;
				x_comp=detected_valid_points[i].coordX;
				y_comp=detected_valid_points[i].coordY;
		}
	}
}

/*
void ReconObjet(Uint16 compt_sushis)
{
	Uint16 i;
	Sint32 nb_close_points;
	Sint32 sumX;
	Sint32 sumY;

	sumX=detected_points[0].coordX;
	sumY=detected_points[0].coordY;
	nb_close_points=1;
	for(i=1;i<compt_sushis;i++)
	{
		if((detected_points[i].coordX > detected_points[i-1].coordX+HOKUYO_DETECTION_MARGE && detected_points[i].coordX < detected_points[i-1].coordX-HOKUYO_DETECTION_MARGE) && (detected_points[i].coordY > detected_points[i-1].coordY+HOKUYO_DETECTION_MARGE && detected_points[i].coordY < detected_points[i-1].coordY-HOKUYO_DETECTION_MARGE) )
		{
			hokuyo_adversaries[adversaries_number].coordX=sumX/nb_close_points;
			hokuyo_adversaries[adversaries_number].coordY=sumY/nb_close_points;
			nb_close_points=1;
			adversaries_number++;
			sumX=detected_points[i].coordX;
			sumY=detected_points[i].coordY;
		}
		else
		{
			sumX+=detected_points[i].coordX;
			sumY+=detected_points[i].coordY;
			nb_close_points++;
			hokuyo_adversaries[adversaries_number].coordX=sumX/nb_close_points;
			hokuyo_adversaries[adversaries_number].coordY=sumY/nb_close_points;
		}
	}
	debug_printf("il y a  %d mechant(s)\n",*nb_ennemi);

	for(i=0;i<=adversaries_number;i++)
	{
		debug_printf(" ennemi numero %d x=[%ld mm]",i,hokuyo_adversaries[i].coordX);
		debug_printf(" et y=[%ld mm]\n",hokuyo_adversaries[i].coordY);
	}
}
*/

void DetectRobots(void)
{
	Uint16 i;
	int nb_pts;
	Uint16 distance;
	Sint32 x_comp, y_comp, sumX, sumY;

	adversaries_number = 0;

	if(nb_valid_points < 1)	//Pas de point observé : rien à faire.
		return;

	//Le premier point débute le premier objet.
	x_comp=detected_valid_points[0].coordX;
	y_comp=detected_valid_points[0].coordY;
	sumX=detected_valid_points[0].coordX;
	sumY=detected_valid_points[0].coordY;
	nb_pts=1;

	for(i=1;i<nb_valid_points;i++)	//Pour tout les points valides à partir du second.
	{
		distance = CALCULATOR_manhattan_distance(detected_valid_points[i].coordX, detected_valid_points[i].coordY, x_comp, y_comp);

		if(distance <= DISTANCE_POINTS_IN_THE_SAME_OBJECT)	//Le point est dans l'objet, on l'intègre à l'objet
		{
			sumX+=detected_valid_points[i].coordX;
			sumY+=detected_valid_points[i].coordY;
			nb_pts++;
		}
		else	//Si la distance est plus grande (le point n'appartient pas à l'objet), on clos l'objet en court.
		{
			if(adversaries_number < NB_MAX_ADVERSARIES - 1)
			{
				hokuyo_adversaries[adversaries_number].coordX=sumX/nb_pts;
				hokuyo_adversaries[adversaries_number].coordY=sumY/nb_pts;
				adversaries_number++;
			}
			nb_pts=1;
			x_comp=detected_valid_points[i].coordX;		//Nouveau départ pour le nouvel objet
			y_comp=detected_valid_points[i].coordY;
			sumX=detected_valid_points[i].coordX;
			sumY=detected_valid_points[i].coordY;
		}
	}
	//Il nous faut maintenant clore le dernier objet.
	if(adversaries_number < NB_MAX_ADVERSARIES - 1)
	{
		hokuyo_adversaries[adversaries_number].coordX=sumX/nb_pts;
		hokuyo_adversaries[adversaries_number].coordY=sumY/nb_pts;
		adversaries_number++;
	}
}
#endif	//def USE_HOKUYO

void user_callback_DeviceDisconnected(void)
{
#ifdef USE_HOKUYO
	flag_device_disconnected = TRUE;
#endif
}

void Compute_dist_and_teta(void)
{
	Uint8 i;
	Sint16 teta;
	for(i=0;i<adversaries_number;i++)
	{
		hokuyo_adversaries[i].dist = CALCULATOR_distance(		robot_position_during_measurement.x,robot_position_during_measurement.y,hokuyo_adversaries[i].coordX,hokuyo_adversaries[i].coordY);
		teta = CALCULATOR_viewing_angle(	robot_position_during_measurement.x,robot_position_during_measurement.y,hokuyo_adversaries[i].coordX,hokuyo_adversaries[i].coordY);
		hokuyo_adversaries[i].teta = CALCULATOR_modulo_angle(teta - robot_position_during_measurement.teta);
	}
}

void send_adversaries_datas(void)
{
	Uint8 i;
	if(adversaries_number==0)
		SECRETARY_send_adversary_position(TRUE,0, 0, 0, 0, 0, 0x0000);
	else
	{
		for(i=0;i<adversaries_number;i++)
			SECRETARY_send_adversary_position((i==adversaries_number-1)?TRUE:FALSE,i, hokuyo_adversaries[i].coordX, hokuyo_adversaries[i].coordY, hokuyo_adversaries[i].teta, hokuyo_adversaries[i].dist, ADVERSARY_DETECTION_FIABILITY_ALL);
	}
}

//Retourne si le module logiciel HOKUYO a envoyé des positions adverses récemment = preuve de bon fonctionnement pour le selftest de la propulsion
bool_e HOKUYO_is_working_well(void)
{
	if(time_since_last_sent_adversaries_datas < 2*PERIOD_SEND_ADVERSARIES_DATAS)
		return TRUE;
	else
		return FALSE;
}

