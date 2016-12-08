/*
 * hokuyo_functions.c
 *
 *  Created on: 8 déc. 2013
 *      Author: HoObbes
 */

#include "Hokuyo.h"


#include "main.h"
	#include "QS/QS_ports.h"
	#include "QS/QS_uart.h"
	#include "QS/QS_buttons.h"
	#include "QS/QS_who_am_i.h"
	#include "QS/QS_outputlog.h"
	#include "QS/QS_maths.h"
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
	#include "calculator.h"
	#include "zone.h"
	#include "environment.h"

	#define HOKUYO_BUFFER_READ_TIMEOUT	500  //ms
	#define HOKUYO_ECHO_READ_TIMEOUT	100  //ms

	static Sint32 HOKUYO_OFFSET_ANGLE=4500; //4500 centiemes de degres rajouter 180000 soit 180 degres pour passer du coté JAUNE

	#define HOKUYO_ANGLE_ROBOT_TERRAIN 0
	#define HOKUYO_DETECTION_MARGE 130
	#define HOKUYO_EVITEMENT_MIN 150
	#define HOKUYO_MARGIN_FIELD_SIDE_IGNORE 50 // Bords du terrain a ignorer

	#define GROS_ROBOT_HOKUYO_TOO_CLOSE_DISTANCE_IGNORE		100	//Distance d'un point trop proche de nous qui doit être ignoré.
	#define PETIT_ROBOT_HOKUYO_TOO_CLOSE_DISTANCE_IGNORE	100	//Distance d'un point trop proche de nous qui doit être ignoré.



	#define DISTANCE_POINTS_IN_THE_SAME_OBJECT 450  //mm
	#define NB_MAX_ADVERSARIES	16
	#define NB_BYTES_FROM_HOKUYO	5000
	#define PERIOD_SEND_ADVERSARIES_DATAS	200	//[ms]
	#define FIELD_SIZE_Y 3000	//[mm]
	#define FIELD_SIZE_X 2000	//[mm]
	#define BEACON_BASE_SIZE 82 //[mm]


	#define CORNER_SQUARE				150
	#define MARGIN						100

	#define CABANE_PURPLE_RECT_X			0
	#define	CABANE_PURPLE_RECT_Y			250
	#define	CABANE_PURPLE_RECT_WIDTH		100		// Largeur en x
	#define	CABANE_PURPLE_RECT_HEIGHT		400		// Longueur en y

	#define CABANE_GREEN_RECT_X				0
	#define	CABANE_GREEN_RECT_Y				2350
	#define	CABANE_GREEN_RECT_WIDTH			100		// Largeur en x
	#define	CABANE_GREEN_RECT_HEIGHT		400 	// Longueur en y

	#define PLEXI_RECT_X				    750
	#define	PLEXI_RECT_Y					1475
	#define	PLEXI_RECT_WIDTH				600		// Largeur en x
	#define	PLEXI_RECT_HEIGHT				50 	// Longueur en y

	//La dune pour être safe
	#define DUNE_RECT_X				        0
	#define	DUNE_RECT_Y				        800
	#define	DUNE_RECT_WIDTH			        100		    // Largeur en x
	#define	DUNE_RECT_HEIGHT		        1400 		// Longueur en y




	__ALIGN_BEGIN USB_OTG_CORE_HANDLE      USB_OTG_Core __ALIGN_END;
	__ALIGN_BEGIN USBH_HOST                USB_Host __ALIGN_END;





	static Uint8 HOKUYO_datas[NB_BYTES_FROM_HOKUYO];				//Données brutes issues du capteur HOKUYO
	static Uint32 datas_index=0;									//Index pour ces données

	#define NB_DETECTED_VALID_POINTS	1080
	static HOKUYO_adversary_position detected_valid_points[NB_DETECTED_VALID_POINTS];	//Points valides détectés par le capteur (X, Y, teta, distance)
	static Uint16 nb_valid_points=0;								//Nombre de points valides détectés

	static HOKUYO_adversary_position hokuyo_adversaries[NB_MAX_ADVERSARIES];	//Positions des adversaires détectés
	static Uint8 adversaries_number=0;											//Nombre d'adversaires détectés

	static bool_e hokuyo_initialized = FALSE;						//Module initialisé - sécurité.
	volatile bool_e flag_device_disconnected = FALSE;				//Flag levé en callback lorsque le capteur vient d'être débranché
	volatile Uint16 time_since_last_sent_adversaries_datas = 0;		//[ms]

	static HOKUYO_adversary_position detected_beacon_points[50];

	static Uint16 BEACON_OFFSET_X=0;
	static Uint16 BEACON_OFFSET_Y=0;
	static Uint16 BEACON_OFFSET_X_BOT = 0;
	static Uint16 BEACON_OFFSET_Y_BOT = 0;


	//HOKUYO
	void hokuyo_write_command(Uint8 tab[]);
	int hokuyo_write_uart_manually(void);
	//void hokuyo_read_buffer(void);
	bool_e hokuyo_read_buffer(void);
	void hokuyo_format_data(void);
	void hokuyo_find_valid_points(void);
	Sint32 hokuyo_dist_min(Uint16 compt);

	//VERBOSE
	void Say_Adversaries_pos(void);
	void Say_Valid_pos(void);

	//ALGORITHME DE DETECTION
	//void ReconObjet(void);
	void DetectRobots(void);
	//void send_adversaries_datas(void);
	//void Compute_dist_and_teta(void);

	//SELFTEST
	void beacon_selftest(void);

	bool_e checkHokuyoData(Uint8* str, uint8_t length);


	//Fonction d'initialisation du périphérique USB
void HOKUYO_init(void)
{
	UNUSED_VAR(detected_beacon_points)

	if(!hokuyo_initialized)
	{

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
		debug_printf("\nFin Init usb for Hokuyo\n");
	hokuyo_initialized = TRUE;
}




//Machine a état de l'hokuyo
//Process main
void HOKUYO_process_main(void)
{
	typedef enum
	{
		INIT=0,
		HOKUYO_WAIT,
		ASK_NEW_MEASUREMENT,
		BUFFER_READ,
		PROCESS_DATA,
		DETECT_ADVERSARIES,
		AFFICHAGE,
		ZONE_DETECTION,
		SEND_ADVERSARIES_DATAS,
		ERROR,
		RESET_HOKUYO,
		RESET_ACKNOWLEDGE,
		TURN_ON_LASER,
		TURN_OFF_LASER,
		DONE,
		WAIT_ECHO_COMMAND
	}state_e;
	static state_e state = INIT, last_state = INIT;
	bool_e entrance;
	static time32_t buffer_read_time_begin = 0;

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
			hokuyo_write_command((Uint8*)"MS0000108001001"); // reglage de l'hokuyo pour aller de 45° à 180° ((Uint8*)"MS0000108001001");
			datas_index=0;
			state = WAIT_ECHO_COMMAND;
		break;
		case WAIT_ECHO_COMMAND:
			if(entrance){
				buffer_read_time_begin = global.absolute_time;
			}
			if(hokuyo_read_buffer()){
				if(HOKUYO_datas[datas_index-2]==0x0A && HOKUYO_datas[datas_index-1]==0x0A && datas_index>=21){
					state=BUFFER_READ;
				}
				else if(datas_index>30)
					state=ASK_NEW_MEASUREMENT;
			}
			if(global.absolute_time - buffer_read_time_begin > HOKUYO_ECHO_READ_TIMEOUT)
			{
				debug_printf("TimeOut Hokuyo echo\n");
				flag_device_disconnected = TRUE;
			}
			break;
		case BUFFER_READ:
			if(entrance){
				buffer_read_time_begin = global.absolute_time;
			}
			if(hokuyo_read_buffer()){
				if(datas_index>2278)
					state=ASK_NEW_MEASUREMENT;
				else if(HOKUYO_datas[datas_index-2]==0x0A && HOKUYO_datas[datas_index-1]==0x0A && datas_index>=2274){
					if(HOKUYO_datas[37] == 0x39 && HOKUYO_datas[38]== 0x39){
						state=PROCESS_DATA;
					}else
						debug_printf("Hokuyo erreur : %c%c\n",HOKUYO_datas[37],HOKUYO_datas[38]);
				}

			}
			if(global.absolute_time - buffer_read_time_begin > HOKUYO_BUFFER_READ_TIMEOUT)
			{
				//i_planted = TRUE;
				debug_printf("TimeOut Hokuyo\n");
				flag_device_disconnected = TRUE;
			}

		break;
		case PROCESS_DATA:
			hokuyo_find_valid_points();
			state=DETECT_ADVERSARIES;
		break;
		case DETECT_ADVERSARIES:
			DetectRobots();
			state=ZONE_DETECTION;
		break;
		case ZONE_DETECTION:
			ZONE_set_datas_updated();
			state=AFFICHAGE;
		break;
		case AFFICHAGE:



			//Say_Adversaries_pos();
			//Say_Valid_pos();
			state=ASK_NEW_MEASUREMENT;

		break;



		case SEND_ADVERSARIES_DATAS:
			if(time_since_last_sent_adversaries_datas > PERIOD_SEND_ADVERSARIES_DATAS)
			{
				time_since_last_sent_adversaries_datas = 0;
				//send_adversaries_datas();
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

//TODO : modifier en ajoutant des protections
//Fonction qui permet de lire le buffer
/*void hokuyo_read_buffer(void)
{
	while(!UART_USB_isRxEmpty())
	{
		HOKUYO_datas[datas_index] = UART_USB_read();
		debug_printf("%c",HOKUYO_datas[datas_index]);
		//UART1_putc(tab[*i]);
		if(datas_index < NB_BYTES_FROM_HOKUYO)
			datas_index++;
		else
			fatal_printf("HOKUYO : overflow in hokuyo_read_buffer !\n");
	}
}*/
bool_e hokuyo_read_buffer(void)
{
	static Uint8 previous_data = 0;
	Uint8 data;
	while(!UART_USB_isRxEmpty())
	{
		data = UART_USB_read();
		HOKUYO_datas[datas_index] = data;
		//debug_printf("%c",HOKUYO_datas[datas_index]);
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
		debug_printf("%c",HOKUYO_datas[i]);
		i++;
		j++;
	}
	datas_index = i-1;
	debug_printf("\ndata_index = %ld\n",datas_index);*/
}

//Fonction qui renvoi les coordonnées des points detectés sur le terrain
void hokuyo_find_valid_points(void)
{
	Uint16 romeo,juliette; // oui j'ai craqué dsl
	Uint16 i;
	Sint32 distance;
	Sint32 angle=0;		//[°*100] centièmes de degrés
	Sint16 angle_offset_rad;
	Sint16 teta_relative;	//[rad4096]
	Sint16 teta_absolute;
	Sint32 x_absolute;
	Sint32 y_absolute;
	Sint16 cos;
	Sint16 sin;
	Sint32 to_close_distance;
	bool_e point_filtered;
	nb_valid_points = 0;	//RAZ des points valides.

	if(ENV_get_color() == BOT_COLOR)	//Magenta
	{
		BEACON_OFFSET_X =1000;
		BEACON_OFFSET_Y = 0;
		HOKUYO_OFFSET_ANGLE=4500;
	}
	else
	{
		BEACON_OFFSET_X = 1000;
		BEACON_OFFSET_Y = 3000;
		HOKUYO_OFFSET_ANGLE=4500;
	}

	to_close_distance = 100;

	//TODO mesurer la durée d'exécution de cet algo...
	for(i = 47; i<datas_index-3;)	//Les données commencent  l'octet 47...
	{
		if(HOKUYO_datas[i+1] == '\n') i+=2;
		if(HOKUYO_datas[i] == '\n')	break;  // FIN DES DONNEES

		romeo = (Uint16)HOKUYO_datas[i++];
		juliette = (Uint16)HOKUYO_datas[i++];

		distance = ((romeo-0x30)<<6)+((juliette-0x30)&0x3f);	//cf datasheet de l'hokuyo... pour comprendre comment les données sont codées.
		if(distance	> to_close_distance)	//On élimine est distances trop petites (ET LES CAS DE REFLEXIONS TORP GRANDE OU LE CAPTEUR RENVOIE 1 !)
		{
			if(angle > 9500  && distance < 2000){
				int t = 400;
			}

			angle_offset_rad = ((((Sint32)(HOKUYO_OFFSET_ANGLE))*183)>>8);
			teta_relative = ((((Sint32)(angle))*183)>>8) - angle_offset_rad;	//Angle relatif au robot, du point en cours, en rad4096
			//teta_relative = CALCULATOR_modulo_angle(teta_relative);

			teta_absolute = CALCULATOR_modulo_angle(teta_relative);				//angle absolu par rapport au terrain, pour le pt en cours, en rad4096

			COS_SIN_4096_get(teta_absolute,&cos,&sin);

			if(ENV_get_color() == BOT_COLOR)	//Magenta
			{
				x_absolute = BEACON_OFFSET_X + (distance*(Sint32)(cos))/4096;
				y_absolute = BEACON_OFFSET_Y + (distance*(Sint32)(sin))/4096;
			}
			else
			{
				x_absolute = BEACON_OFFSET_X - (distance*(Sint32)(cos))/4096;
				y_absolute = BEACON_OFFSET_Y - (distance*(Sint32)(sin))/4096;
			}



			if(		x_absolute 	< 	FIELD_SIZE_X - HOKUYO_MARGIN_FIELD_SIDE_IGNORE &&
					x_absolute	>	HOKUYO_MARGIN_FIELD_SIDE_IGNORE &&
					y_absolute 	< 	FIELD_SIZE_Y - HOKUYO_MARGIN_FIELD_SIDE_IGNORE &&
					y_absolute 	>	HOKUYO_MARGIN_FIELD_SIDE_IGNORE)
			{		//Un point est retenu s'il est sur le terrain

				point_filtered = FALSE;	//On suppose que le point n'est pas filtré

				//On va éliminer certaines zones volontairement.
				if(x_absolute > FIELD_SIZE_X - MARGIN || x_absolute < MARGIN || absolute(x_absolute - FIELD_SIZE_X/2) < MARGIN)
					if(y_absolute < MARGIN || y_absolute > FIELD_SIZE_Y - MARGIN)	//Les 4 coins et deux balises fixes
						point_filtered = TRUE;	//on refuse les points

				if(x_absolute < CABANE_GREEN_RECT_X + CABANE_GREEN_RECT_WIDTH
						&& y_absolute > CABANE_GREEN_RECT_Y - MARGIN
						&& y_absolute < CABANE_GREEN_RECT_Y + CABANE_GREEN_RECT_HEIGHT + MARGIN) 	//zone des cabanes vertes
					point_filtered = TRUE;	//on refuse les points

				if(x_absolute < CABANE_PURPLE_RECT_X + CABANE_PURPLE_RECT_WIDTH
						&& y_absolute > CABANE_PURPLE_RECT_Y - MARGIN
						&& y_absolute < CABANE_PURPLE_RECT_Y + CABANE_PURPLE_RECT_HEIGHT + MARGIN) 	//zone des cabanes violettes
					point_filtered = TRUE;	//on refuse les points

				if(x_absolute < PLEXI_RECT_X + PLEXI_RECT_WIDTH + MARGIN
						&& x_absolute > PLEXI_RECT_X  - MARGIN
						&& y_absolute > PLEXI_RECT_Y - MARGIN
						&& y_absolute < PLEXI_RECT_Y + PLEXI_RECT_HEIGHT + MARGIN) 	//zone centrale où se trouve le plexi
					point_filtered = TRUE;	//on refuse les points

				if(x_absolute < DUNE_RECT_X + DUNE_RECT_WIDTH
						&& y_absolute > DUNE_RECT_Y
						&& y_absolute < DUNE_RECT_Y + DUNE_RECT_HEIGHT) 	//zone de la dune
					point_filtered = TRUE;	//on refuse les points

				if(angle < 100*5 || angle > 100*265)//on retire les 5 premiers degrés et les 5 derniers
					point_filtered = TRUE;

				if(point_filtered == FALSE)
				{
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

/*void Save_Adversaries_pos(void){
	Uint8 i;
	for(i=0;i<adversaries_number;i++){
		hokuyo_adversaries[i].coordX_prec=hokuyo_adversaries[i].coordX;
		hokuyo_adversaries[i].coordY_prec=hokuyo_adversaries[i].coordY;
	}
}*/

void Say_Adversaries_pos(void){
	Uint8 i;
	for(i=0;i<adversaries_number;i++){
		debug_printf("\nAdv n%d X=[%ld]",i,hokuyo_adversaries[i].coordX);
		debug_printf(" and Y=[%ld]\n",hokuyo_adversaries[i].coordY);
	}
}

void Say_Valid_pos(void){
	Uint8 i;
	for(i=0;i<nb_valid_points;i++){
		debug_printf("\nAdv n%d X=[%ld]",i,detected_valid_points[i].coordX);
		debug_printf(" and Y=[%ld]\n",detected_valid_points[i].coordY);
	}
}

Uint8 HOKUYO_get_adversaries_number(void)
{
	return adversaries_number;
}

HOKUYO_adversary_position * HOKUYO_get_valid_points(void)
{
	return detected_valid_points;
}

Uint16 HOKUYO_get_nb_valid_points(void)
{
	return nb_valid_points;
}

HOKUYO_adversary_position * HOKUYO_get_adversary_position(Uint8 i)
{
	assert(i < adversaries_number);
	return &hokuyo_adversaries[i];
}


void user_callback_DeviceDisconnected(void)
{
	flag_device_disconnected = TRUE;
}

/*void Compute_dist_and_teta(void)
{
	Uint8 i;
	Sint16 teta;
	for(i=0;i<adversaries_number;i++)
	{
		hokuyo_adversaries[i].dist = CALCULATOR_distance(		robot_position_during_measurement.x,robot_position_during_measurement.y,hokuyo_adversaries[i].coordX,hokuyo_adversaries[i].coordY);
		teta = CALCULATOR_viewing_angle(	robot_position_during_measurement.x,robot_position_during_measurement.y,hokuyo_adversaries[i].coordX,hokuyo_adversaries[i].coordY);
		hokuyo_adversaries[i].teta = CALCULATOR_modulo_angle(teta - robot_position_during_measurement.teta);
	}
}*/

/*void send_adversaries_datas(void)
{
	Uint8 i;
	for(i=0;i<adversaries_number;i++)
		SECRETARY_send_adversary_position((i==adversaries_number-1)?TRUE:FALSE,i, hokuyo_adversaries[i].coordX, hokuyo_adversaries[i].coordY, hokuyo_adversaries[i].teta, hokuyo_adversaries[i].dist, ADVERSARY_DETECTION_FIABILITY_ALL);
}*/



void Triangulation(void){ //beurk
	Sint32 B1dist= 2974 ;
	Sint32 B2dist=1850 ;
	Sint32 B3dist= 212;

	Sint32 B1X= 1000;
	Sint32 B2X= 2000;
	Sint32 B3X= 0;

	Sint32 B1Y= 3000 ;
	Sint32 B2Y= 0;
	Sint32 B3Y= 0;

	Sint32 v=B1dist*B1dist+B3Y*B3Y+B3X*B3X;
	Sint32 w=B3dist*B3dist+B1Y*B1Y+B1X*B1X;
	Sint32 u=B2dist*B2dist+B2Y*B2Y+B2X*B2X;

	Sint32 delta=2*((B2X-B1X)*(B3Y-B1Y)-(B3X-B1X)*(B2Y-B1Y));


	Sint32 Xt=(B2X*(v-w)+B3X*(u-v)+B1X*(w-u))/delta;
	Sint32 Yt=(B1Y*(u-w)+B2Y*(w-v)+B1Y*(v-u))/delta;

	debug_printf("X du robot= %ld\n",Xt);
	debug_printf("Y du robot= %ld\n",Yt);
}

bool_e checkHokuyoData(Uint8* str, uint8_t length)
{
	uint8_t i=0;
	for(i=length; i>0; i--){
		if(HOKUYO_datas[datas_index-i-1]>0){
			if(HOKUYO_datas[datas_index-i-1]!=str[length-i]){
				return FALSE;
			}
		}else{
			return FALSE;
		}
	}
	return TRUE;
}




