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
	#include "QS/QS_maths.h"
	#include "hokuyo.h"
	#include "calculator.h"
	#include "secretary.h"
	#include "detection.h"
	#include "odometry.h"
	#include "QS/QS_maths.h"




#ifdef USE_HOKUYO

	#define HOKUYO_OFFSET_ANGLE_RAD4096 (9651) //135 degrés
	#define HOKUYO_ANGLE_ROBOT_TERRAIN 0
	#define HOKUYO_DETECTION_MARGE 130
	#define HOKUYO_EVITEMENT_MIN 150
	#define HOKUYO_MARGIN_FIELD_SIDE_IGNORE 100

	#define GROS_ROBOT_HOKUYO_TOO_CLOSE_DISTANCE_IGNORE		250	//Distance d'un point trop proche de nous qui doit être ignoré.
	#define PETIT_ROBOT_HOKUYO_TOO_CLOSE_DISTANCE_IGNORE	150	//Distance d'un point trop proche de nous qui doit être ignoré.

	#define ROBOT_COORDX 150
	#define ROBOT_COORDY 150
	#define DISTANCE_POINTS_IN_THE_SAME_OBJECT 150

	#define NB_BYTES_FROM_HOKUYO	5000
	#define PERIOD_SEND_ADVERSARIES_DATAS	175	//[ms]

	__ALIGN_BEGIN USB_OTG_CORE_HANDLE      USB_OTG_Core __ALIGN_END;
	__ALIGN_BEGIN USBH_HOST                USB_Host __ALIGN_END;




	static Uint8 HOKUYO_datas[NB_BYTES_FROM_HOKUYO];				//Données brutes issues du capteur HOKUYO
	static Uint32 datas_index=0;									//Index pour ces données

	#define NB_DETECTED_VALID_POINTS	1000
	static HOKUYO_adversary_position detected_valid_points[NB_DETECTED_VALID_POINTS];	//Points valides détectés par le capteur (X, Y, teta, distance)
	static Uint16 nb_valid_points=0;								//Nombre de points valides détectés

	static HOKUYO_adversary_position hokuyo_adversaries[HOKUYO_MAX_FOES];	//Positions des adversaires détectés
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

	#ifdef TRIANGULATION
		void Hokuyo_validPointsAndBeacons();
		#define ECART_BALISE 50
		#define ECART_FIABILITE 80  //marge de la zone où les mesures sont considérées comme non-fiable
		#define ECART_POSITION  100  //marge au delà de laquelle un point n'est pas pris en compte dans le calcul de la position (=erreur)
		static position robot, currentRobot[10];
		static HOKUYO_adversary_position beacon1, beacon2, beacon3;
		static Uint8 B1detected, B2detected, B3detected;
		static HOKUYO_adversary_position points_beacons[3][50];
		static Uint8 nb_points_B1=0,nb_points_B2=0,nb_points_B3=0;
	#endif


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
				state=ASK_NEW_MEASUREMENT;
		break;
		case REMOVE_LF:
			hokuyo_format_data();
			state=TREATMENT_DATA;
		break;
		case TREATMENT_DATA:
			hokuyo_find_valid_points();
			#ifdef TRIANGULATION
			if(global.match_over){
				Hokuyo_validPointsAndBeacons();
				tri_points();
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

	if(QS_WHO_AM_I_get() == BIG_ROBOT)
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
				#define MAMOUTH_RECTANGLES_Y_START	350
				#define MAMOUTH_RECTANGLES_Y_STOP	1150
				#define CORNER_SQUARE				150
				#define MARGIN						100

				//On va éliminer certaines zones volontairement.
				if(x_absolute > FIELD_SIZE_X - MARGIN || x_absolute < MARGIN || absolute(x_absolute - FIELD_SIZE_X/2) < MARGIN)
					if(y_absolute < MARGIN || y_absolute > FIELD_SIZE_Y - MARGIN)	//Les 4 coins et deux balises fixes
						point_filtered = TRUE;	//on refuse les points
				if(x_absolute < MAMOUTH_RECTANGLES_X)
					if (  	(y_absolute > MAMOUTH_RECTANGLES_Y_START && y_absolute < MAMOUTH_RECTANGLES_Y_STOP) 	//zones de dépose devant mamouths
						|| 	(y_absolute > FIELD_SIZE_Y - MAMOUTH_RECTANGLES_Y_STOP && y_absolute < FIELD_SIZE_Y - MAMOUTH_RECTANGLES_Y_START)	)
						point_filtered = TRUE;	//on refuse les points

				//if((x_absolute < 500 && x_absolute	> 50) && ((y_absolute < 400 && y_absolute >50) || (y_absolute < 2950 && y_absolute >2600)))
				//		point_filtered = TRUE;	//on refuse les points de la zone de départ...


				if(angle < 100*5 || angle > 100*265)//on retire les 5 premiers degrés et les 5 derniers
					point_filtered = TRUE;

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
				if(adversaries_number < HOKUYO_MAX_FOES - 1)
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
			if(adversaries_number < HOKUYO_MAX_FOES - 1)
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
	if(adversaries_number < HOKUYO_MAX_FOES - 1)
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

void refresh_adversaries(void){
	DETECTION_new_adversary_position(NULL, hokuyo_adversaries, adversaries_number);
}

//Retourne si le module logiciel HOKUYO a envoyé des positions adverses récemment = preuve de bon fonctionnement pour le selftest de la propulsion
bool_e HOKUYO_is_working_well(void)
{
	if(time_since_last_sent_adversaries_datas < 2*PERIOD_SEND_ADVERSARIES_DATAS)
		return TRUE;
	else
		return FALSE;
}


#ifdef TRIANGULATION

void Hokuyo_validPointsAndBeacons(){
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
	color_e color= ODOMETRY_get_color();

	if(QS_WHO_AM_I_get() == BIG_ROBOT)
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

			if(		(x_absolute 	< 	FIELD_SIZE_X - HOKUYO_MARGIN_FIELD_SIDE_IGNORE &&     //Un point est retenu s'il est sur le terrain
					x_absolute	>	HOKUYO_MARGIN_FIELD_SIDE_IGNORE &&
					y_absolute 	< 	FIELD_SIZE_Y - HOKUYO_MARGIN_FIELD_SIDE_IGNORE &&
					y_absolute 	>	HOKUYO_MARGIN_FIELD_SIDE_IGNORE)
					|| (color==RED && x_absolute<ECART_BALISE && x_absolute>-(62+ECART_BALISE) && y_absolute<ECART_BALISE && y_absolute>-(62+ECART_BALISE) )  //un point est retenu s'il fait partie d'une balise fixe de sa couleur sur le bord du terrain
					|| (color==RED && x_absolute<2062+ECART_BALISE && x_absolute>2000-ECART_BALISE && y_absolute<ECART_BALISE && y_absolute>-(62+ECART_BALISE) )
					|| (color==RED && x_absolute<1031+ECART_BALISE && x_absolute>979+ECART_BALISE && y_absolute<3062+ECART_BALISE && y_absolute>3000-ECART_BALISE )
					|| (color==YELLOW && x_absolute<ECART_BALISE && x_absolute>-62+ECART_BALISE && y_absolute<3062+ECART_BALISE && y_absolute>3000-ECART_BALISE )  //un point est retenu s'il fait partie d'une balise fixe de sa couleur sur le bord du terrain
					|| (color==YELLOW && x_absolute<2062+ECART_BALISE && x_absolute>2000-ECART_BALISE && y_absolute<3062+ECART_BALISE && y_absolute>3000-ECART_BALISE )
					|| (color==YELLOW && x_absolute<1031+ECART_BALISE && x_absolute>979+ECART_BALISE && y_absolute<ECART_BALISE && y_absolute>-(62+ECART_BALISE) )
				)
			{

				point_filtered = FALSE;	//On suppose que le point n'est pas filtré

				//if((x_absolute < 500 && x_absolute	> 50) && ((y_absolute < 400 && y_absolute >50) || (y_absolute < 2950 && y_absolute >2600)))
				//		point_filtered = TRUE;	//on refuse les points de la zone de départ...


				if(angle < 100*5 || angle > 100*265)//on retire les 5 premiers degrés et les 5 derniers
					point_filtered = TRUE;

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

void triangulation(){
	Sint32 x1, y1, x2, y2;
	Uint32 a1, a2; //abscisse, ordonnée et angle
	robot.teta = global.position.teta;
	robot.x = global.position.x;
	robot.y = global.position.y;

	//Repérage à une balise (nécessite d'avoir l'angle du robot)
	//balise 1
	if(B1detected==1){
		x1 = -cos(robot.teta+beacon1.teta)*beacon1.dist-62;
		y1 = -beacon1.dist*sin(robot.teta+beacon1.teta)-62;

		x2 = cos(robot.teta+beacon1.teta)*beacon1.dist-62;
		y2 = beacon1.dist*sin(robot.teta+beacon1.teta)-62;

		if(x1>0 && x1<2000 && y1>0 && y1<3000){
			currentRobot[0].x = x1;
			currentRobot[0].y = y1;
		}else{
			currentRobot[0].x = x2;
			currentRobot[0].y = y2;
		}
		currentRobot[0].teta=robot.teta;
		currentRobot[0].weight=5;
	}

	//balise 2
	if(B2detected==1){
		x1 = -( ( puissance(tan(1/2.0*robot.teta+1/2.0*beacon2.teta),2)*(beacon2.dist-1000)-beacon2.dist-1000) / (puissance(tan(1/2.0*robot.teta+1/2.0*beacon2.teta),2)+1) );
		y1 = -2*( (beacon2.dist*tan(1/2.0*robot.teta+1/2.0*beacon2.teta)-1531*puissance(tan(1/2.0*robot.teta+1/2.0*beacon2.teta),2)-1531) / (puissance(tan(1/2.0*robot.teta+1/2.0*beacon2.teta),2)+1) );

		x2 =  ( puissance(tan(1/2.0*robot.teta+1/2.0*beacon2.teta),2)*(beacon2.dist+1000)-beacon2.dist+1000) / (puissance(tan(1/2.0*robot.teta+1/2.0*beacon2.teta),2)+1) ;
		y2 = 2*( (beacon2.dist*tan(1/2.0*robot.teta+1/2.0*beacon2.teta)+1531*puissance(tan(1/2.0*robot.teta+1/2.0*beacon2.teta),2)+1531) / (puissance(tan(1/2.0*robot.teta+1/2.0*beacon2.teta),2)+1) );

		if(x1>0 && x1<2000 && y1>0 && y1<3000){
			currentRobot[1].x = x1;
			currentRobot[1].y = y1;
		}else{
			currentRobot[1].x = x2;
			currentRobot[1].y = y2;
		}
		currentRobot[1].teta=robot.teta;
		currentRobot[1].weight=5;

	}

	//balise 3
	if(B3detected==1){
		x1 = -beacon3.dist*cos(robot.teta+beacon3.teta)+2062;
		y1 = -beacon3.dist*sin(robot.teta+beacon3.teta)-62;

		x2 = beacon3.dist*cos(robot.teta+beacon3.teta)+2062;
		y2 = beacon3.dist*sin(robot.teta+beacon3.teta)-62;

		if(x1>0 && x1<2000 && y1>0 && y1<3000){
			currentRobot[2].x = x1;
			currentRobot[2].y = y1;
		}else{
			currentRobot[2].x = x2;
			currentRobot[2].y = y2;
		}
		currentRobot[2].teta=robot.teta;
		currentRobot[2].weight=5;
	}

	//Repérage à deux balises
	//balises 1 et 2
	if(B1detected==1 && B2detected==1){
		x1=(531/10887220.0)*(puissance(beacon1.dist,2)-puissance(beacon2.dist,2))+469-(781/5443610.0)*sqrt(-puissance(beacon1.dist,4)+2*puissance(beacon1.dist,2)*puissance(beacon2.dist,2)-puissance(beacon2.dist,4)+21774440*(puissance(beacon1.dist,2)+puissance(beacon2.dist,2))-118531559328400);
		y1=(781/5443610.0)*(puissance(beacon1.dist,2)-puissance(beacon2.dist,2))+1500+(531/10887220.0)*sqrt(-puissance(beacon1.dist,4)+2*puissance(beacon1.dist,2)*puissance(beacon2.dist,2)-puissance(beacon2.dist,4)+21774440*(puissance(beacon1.dist,2)+puissance(beacon2.dist,2))-118531559328400);
		a1=(beacon1.teta>beacon2.teta)? 3*PI4096/2.0-atan4096(y1/(x1*1.0))-atan2_4096(1000-x1,3000-y1): PI4096/2.0+atan4096(y1/(x1*1.0))+atan2_4096(1000-x1,3000-y1) ;

		x2=(531/10887220.0)*(puissance(beacon1.dist,2)-puissance(beacon2.dist,2))+469+(781/5443610.0)*sqrt(-puissance(beacon1.dist,4)+2*puissance(beacon1.dist,2)*puissance(beacon2.dist,2)-puissance(beacon2.dist,4)+21774440*(puissance(beacon1.dist,2)+puissance(beacon2.dist,2))-118531559328400);
		y2=(781/5443610.0)*(puissance(beacon1.dist,2)-puissance(beacon2.dist,2))+1500-(531/10887220.0)*sqrt(-puissance(beacon1.dist,4)+2*puissance(beacon1.dist,2)*puissance(beacon2.dist,2)-puissance(beacon2.dist,4)+21774440*(puissance(beacon1.dist,2)+puissance(beacon2.dist,2))-118531559328400);
		a2=(beacon1.teta>beacon2.teta)? 3*PI4096/2.0-atan4096(y2/(x2*1.0))-atan2_4096(1000-x2,3000-y2): PI4096/2.0+atan4096(y2/(x2*1.0))+atan2_4096(1000-x2,3000-y2) ;

		//la mesure des angles permet de déterminer quelle position est correcte
		if(fabs(fabs(beacon1.teta-beacon2.teta)-a1)<fabs(fabs(beacon1.teta-beacon2.teta)-a2)){
			currentRobot[3].x=x1;
			currentRobot[3].y=y1;
			currentRobot[3].teta=a1;
			currentRobot[3].weight=10;
		}else{
			currentRobot[3].x=x2;
			currentRobot[3].y=y2;
			currentRobot[3].teta=a2;
			currentRobot[3].weight=10;
		}
	}
	//balises 1 et 3
	if(B1detected==1 && B3detected==1){
		x1=(1/4248.0)*(puissance(beacon1.dist,2)-puissance(beacon3.dist,2))+1000;
		y1=-62+(1/4248.0)*sqrt(-puissance(beacon1.dist,4)+2*puissance(beacon1.dist,2)*puissance(beacon3.dist,2)-puissance(beacon3.dist,4)+9022752*(puissance(beacon1.dist,2)+puissance(beacon3.dist,2))-20352513413376);
		a1=(beacon3.teta>beacon1.teta) ? atan4096(x1/y1)+atan4096((2000-x1)/y1): 2*PI4096-atan4096(x1/y1)-atan4096((2000-x1)/y1);

		x2=x1;//l'abscisse est identique
		y2=-62-(1/4248.0)*sqrt(-puissance(beacon1.dist,4)+2*puissance(beacon1.dist,2)*puissance(beacon3.dist,2)-puissance(beacon3.dist,4)+9022752*(puissance(beacon1.dist,2)+puissance(beacon3.dist,2))-20352513413376);
		a2=atan4096(x2/y2)+atan4096((2000-x2)/y2);

		//la mesure des angles permet de déterminer quelle position est correcte
		if(fabs(beacon3.teta-beacon1.teta-a1)<fabs(beacon3.teta-beacon1.teta-a2)){
			currentRobot[4].x=x1;		//logiquement la première position est toujours la bonne??
			currentRobot[4].y=y1;
			currentRobot[4].teta=a1;
			currentRobot[4].weight=10;
		}else{
			currentRobot[4].x=x2;
			currentRobot[4].y=y2;
			currentRobot[4].teta=a2;
			currentRobot[4].weight=10;
		}
	}
	//balises 2 et 3
	if(B2detected==1 && B3detected==1){
		x1=(531/10887220.0)*(puissance(beacon2.dist,2)-puissance(beacon3.dist,2))+1531+(781/5443610.0)*sqrt(-puissance(beacon2.dist,4)+2*puissance(beacon2.dist,2)*puissance(beacon3.dist,2)-puissance(beacon3.dist,4)+21774440*(puissance(beacon2.dist,2)+puissance(beacon3.dist,2))-118531559328400);
		y1=-(781/5443610.0)*(puissance(beacon2.dist,2)-puissance(beacon3.dist,2))+1500+(531/10887220.0)*sqrt(-puissance(beacon2.dist,4)+2*puissance(beacon2.dist,2)*puissance(beacon3.dist,2)-puissance(beacon3.dist,4)+21774440*(puissance(beacon2.dist,2)+puissance(beacon3.dist,2))-118531559328400);
		a1=(beacon3.teta>beacon2.teta)? 2*PI4096-atan4096(y1/x1)-atan2_4096(1000-x1,3000-y1): atan4096(y1/x1)+atan2_4096(1000-x1,3000-y1);

		x2=(531/10887220.0)*(puissance(beacon2.dist,2)-puissance(beacon3.dist,2))+1531-(781/5443610.0)*sqrt(-puissance(beacon2.dist,4)+2*puissance(beacon2.dist,2)*puissance(beacon3.dist,2)-puissance(beacon3.dist,4)+21774440*(puissance(beacon2.dist,2)+puissance(beacon3.dist,2))-118531559328400);
		y2=-(781/5443610.0)*(puissance(beacon2.dist,2)-puissance(beacon3.dist,2))+1500-(531/10887220.0)*sqrt(-puissance(beacon2.dist,4)+2*puissance(beacon2.dist,2)*puissance(beacon3.dist,2)-puissance(beacon3.dist,4)+21774440*(puissance(beacon2.dist,2)+puissance(beacon3.dist,2))-118531559328400);
		a2=(beacon3.teta>beacon2.teta)? 2*PI4096-atan4096(y2/x2)-atan2_4096(1000-x2,3000-y1): atan4096(y2/x2)+atan2_4096(1000-x2,3000-y2);

		//la mesure des angles permet de déterminer quelle position est correcte
		if(fabs(fabs(beacon3.teta-beacon2.teta)-a1)<fabs(fabs(beacon3.teta-beacon2.teta)-a2)){
			currentRobot[5].x=x1;
			currentRobot[5].y=y1;
			currentRobot[5].teta=a1;
			currentRobot[5].weight=10;
		}else{
			currentRobot[5].x=x2;
			currentRobot[5].y=y2;
			currentRobot[5].teta=a2;
			currentRobot[5].weight=10;
		}
	}
}

void findCorrectPosition(){
	//Gestion des erreurs: un point trop éloigné ne doit pas être pris en compte lorsqu'on a pluseurs mesures disponibles (2 ou 3 balises)
	if(B1detected==1 && B2detected==1){
		if(sqrt((currentRobot[0].x-currentRobot[1].x)*(currentRobot[0].x-currentRobot[1].x)+(currentRobot[0].y-currentRobot[1].y)*(currentRobot[0].y-currentRobot[1].y))>ECART_POSITION
			&& sqrt((currentRobot[0].x-currentRobot[3].x)*(currentRobot[0].x-currentRobot[3].x)+(currentRobot[0].y-currentRobot[3].y)*(currentRobot[0].y-currentRobot[3].y))>ECART_POSITION ){
			currentRobot[0].weight=0;
		}
		if(sqrt((currentRobot[0].x-currentRobot[1].x)*(currentRobot[0].x-currentRobot[1].x)+(currentRobot[0].y-currentRobot[1].y)*(currentRobot[0].y-currentRobot[1].y))>ECART_POSITION
			&& sqrt((currentRobot[1].x-currentRobot[3].x)*(currentRobot[1].x-currentRobot[3].x)+(currentRobot[1].y-currentRobot[3].y)*(currentRobot[1].y-currentRobot[3].y))>ECART_POSITION ){
			currentRobot[1].weight=0;
		}
		if(sqrt((currentRobot[3].x-currentRobot[1].x)*(currentRobot[3].x-currentRobot[1].x)+(currentRobot[3].y-currentRobot[1].y)*(currentRobot[3].y-currentRobot[1].y))>ECART_POSITION
			&& sqrt((currentRobot[0].x-currentRobot[3].x)*(currentRobot[0].x-currentRobot[3].x)+(currentRobot[0].y-currentRobot[3].y)*(currentRobot[0].y-currentRobot[3].y))>ECART_POSITION ){
			currentRobot[3].weight=0;
		}
	}
	if(B1detected==1 && B3detected==1){
		if(sqrt((currentRobot[0].x-currentRobot[2].x)*(currentRobot[0].x-currentRobot[2].x)+(currentRobot[0].y-currentRobot[2].y)*(currentRobot[0].y-currentRobot[2].y))>ECART_POSITION
			&& sqrt((currentRobot[0].x-currentRobot[4].x)*(currentRobot[0].x-currentRobot[4].x)+(currentRobot[0].y-currentRobot[4].y)*(currentRobot[0].y-currentRobot[4].y))>ECART_POSITION ){
			currentRobot[0].weight=0;
		}
		if(sqrt((currentRobot[2].x-currentRobot[0].x)*(currentRobot[2].x-currentRobot[0].x)+(currentRobot[2].y-currentRobot[0].y)*(currentRobot[2].y-currentRobot[0].y))>ECART_POSITION
			&& sqrt((currentRobot[2].x-currentRobot[4].x)*(currentRobot[2].x-currentRobot[4].x)+(currentRobot[2].y-currentRobot[4].y)*(currentRobot[2].y-currentRobot[4].y))>ECART_POSITION ){
			currentRobot[2].weight=0;
		}
		if(sqrt((currentRobot[4].x-currentRobot[0].x)*(currentRobot[4].x-currentRobot[0].x)+(currentRobot[4].y-currentRobot[0].y)*(currentRobot[4].y-currentRobot[0].y))>ECART_POSITION
			&& sqrt((currentRobot[4].x-currentRobot[2].x)*(currentRobot[4].x-currentRobot[2].x)+(currentRobot[4].y-currentRobot[2].y)*(currentRobot[4].y-currentRobot[2].y))>ECART_POSITION ){
			currentRobot[4].weight=0;
		}
	}
	if(B2detected==1 && B3detected==1){
		if(sqrt((currentRobot[1].x-currentRobot[2].x)*(currentRobot[1].x-currentRobot[2].x)+(currentRobot[1].y-currentRobot[2].y)*(currentRobot[1].y-currentRobot[2].y))>ECART_POSITION
			&& sqrt((currentRobot[1].x-currentRobot[5].x)*(currentRobot[1].x-currentRobot[5].x)+(currentRobot[1].y-currentRobot[5].y)*(currentRobot[1].y-currentRobot[5].y))>ECART_POSITION ){
			currentRobot[1].weight=0;
		}
		if(sqrt((currentRobot[2].x-currentRobot[1].x)*(currentRobot[2].x-currentRobot[1].x)+(currentRobot[2].y-currentRobot[1].y)*(currentRobot[2].y-currentRobot[1].y))>ECART_POSITION
			&& sqrt((currentRobot[2].x-currentRobot[5].x)*(currentRobot[2].x-currentRobot[5].x)+(currentRobot[2].y-currentRobot[5].y)*(currentRobot[2].y-currentRobot[5].y))>ECART_POSITION ){
			currentRobot[2].weight=0;
		}
		if(sqrt((currentRobot[5].x-currentRobot[1].x)*(currentRobot[5].x-currentRobot[1].x)+(currentRobot[5].y-currentRobot[1].y)*(currentRobot[5].y-currentRobot[1].y))>ECART_POSITION
			&& sqrt((currentRobot[5].x-currentRobot[2].x)*(currentRobot[5].x-currentRobot[2].x)+(currentRobot[5].y-currentRobot[2].y)*(currentRobot[5].y-currentRobot[2].y))>ECART_POSITION ){
			currentRobot[5].weight=0;
		}
	}
	if(B1detected==1 && B2detected==1 && B3detected==1){
			if(sqrt((currentRobot[0].x-currentRobot[1].x)*(currentRobot[0].x-currentRobot[1].x)+(currentRobot[0].y-currentRobot[1].y)*(currentRobot[0].y-currentRobot[1].y))>ECART_POSITION
				&& sqrt((currentRobot[0].x-currentRobot[2].x)*(currentRobot[0].x-currentRobot[2].x)+(currentRobot[0].y-currentRobot[2].y)*(currentRobot[0].y-currentRobot[2].y))>ECART_POSITION ){
				currentRobot[0].weight=0; //la mesure avec la balise B1 a raté
				currentRobot[3].weight=0;
				currentRobot[4].weight=0;
			}
			if(sqrt((currentRobot[1].x-currentRobot[0].x)*(currentRobot[1].x-currentRobot[0].x)+(currentRobot[1].y-currentRobot[0].y)*(currentRobot[1].y-currentRobot[0].y))>ECART_POSITION
				&& sqrt((currentRobot[1].x-currentRobot[2].x)*(currentRobot[1].x-currentRobot[2].x)+(currentRobot[1].y-currentRobot[2].y)*(currentRobot[1].y-currentRobot[2].y))>ECART_POSITION ){
				currentRobot[1].weight=0; //la mesure avec la balise B2 a raté
				currentRobot[3].weight=0;
				currentRobot[5].weight=0;
			}
			if(sqrt((currentRobot[2].x-currentRobot[0].x)*(currentRobot[2].x-currentRobot[0].x)+(currentRobot[2].y-currentRobot[0].y)*(currentRobot[2].y-currentRobot[0].y))>ECART_POSITION
				&& sqrt((currentRobot[2].x-currentRobot[1].x)*(currentRobot[2].x-currentRobot[1].x)+(currentRobot[2].y-currentRobot[1].y)*(currentRobot[2].y-currentRobot[1].y))>ECART_POSITION ){
				currentRobot[2].weight=0; //la mesure avec la balise B3 a raté
				currentRobot[4].weight=0;
				currentRobot[5].weight=0;
			}
	}

	//Le poids des points mesurées dans les zones sensibles à chaque balises sont diminués
	if(B1detected==1 && (currentRobot[0].x<ECART_FIABILITE || currentRobot[0].y<ECART_FIABILITE )){
		currentRobot[0].weight=1;
	}
	if(B2detected==1 && (currentRobot[0].x>2000-ECART_FIABILITE || currentRobot[0].y<ECART_FIABILITE )){
		currentRobot[1].weight=1;
	}
	if(B3detected==1 && ((currentRobot[0].x>1000-ECART_FIABILITE && currentRobot[0].x<1000+ECART_FIABILITE) || currentRobot[0].y>3000-ECART_FIABILITE )){
		currentRobot[2].weight=1;
	}

	robot.x=(currentRobot[0].x*currentRobot[0].weight+currentRobot[1].x*currentRobot[1].weight+currentRobot[2].x*currentRobot[2].weight+currentRobot[3].x*currentRobot[3].weight+currentRobot[4].x*currentRobot[4].weight+currentRobot[5].x*currentRobot[5].weight)/(1.0*(currentRobot[0].weight+currentRobot[1].weight+currentRobot[2].weight+currentRobot[3].weight+currentRobot[4].weight+currentRobot[5].weight));
	robot.y=(currentRobot[0].y*currentRobot[0].weight+currentRobot[1].y*currentRobot[1].weight+currentRobot[2].y*currentRobot[2].weight+currentRobot[3].y*currentRobot[3].weight+currentRobot[4].y*currentRobot[4].weight+currentRobot[5].y*currentRobot[5].weight)/(1.0*(currentRobot[0].weight+currentRobot[1].weight+currentRobot[2].weight+currentRobot[3].weight+currentRobot[4].weight+currentRobot[5].weight));
	if(B1detected==1 && B2detected==0 && B3detected==0){
		robot.teta=currentRobot[0].teta;
	}else if(B1detected==0 && B2detected==1 && B3detected==0){
		robot.teta=currentRobot[1].teta;
	}else if(B1detected==0 && B2detected==0 && B3detected==1){
		robot.teta=currentRobot[2].teta;
	}else{
		robot.teta=(currentRobot[3].teta*currentRobot[3].weight+currentRobot[4].teta*currentRobot[4].weight+currentRobot[5].teta*currentRobot[5].weight)/(1.0*(currentRobot[3].weight+currentRobot[4].weight+currentRobot[5].weight));
	}
}


void tri_points(){
	Uint8 i;
	for(i=0;i<nb_valid_points;i++){
		if(detected_valid_points[i].coordX>-62-ECART_BALISE && detected_valid_points[i].coordX<ECART_BALISE
		   && detected_valid_points[i].coordY>-62-ECART_BALISE && detected_valid_points[i].coordY<ECART_BALISE){
			points_beacons[0][nb_points_B1]=detected_valid_points[i];
			nb_points_B1++;
		}else if(detected_valid_points[i].coordX>969-ECART_BALISE && detected_valid_points[i].coordX<969+ECART_BALISE
				 && detected_valid_points[i].coordY>3000-ECART_BALISE && detected_valid_points[i].coordY<3062+ECART_BALISE){
				  points_beacons[1][nb_points_B2]=detected_valid_points[i];
				  nb_points_B2++;
		}else if(detected_valid_points[i].coordX>2000-ECART_BALISE && detected_valid_points[i].coordX<2062+ECART_BALISE
				 && detected_valid_points[i].coordY>-62-ECART_BALISE && detected_valid_points[i].coordY<ECART_BALISE){
				  points_beacons[2][nb_points_B3]=detected_valid_points[i];
				  nb_points_B3++;
		}
	}
	debug_printf("Points balise 1 \n\nx= ");
	for(i=0;i<nb_points_B1;i++){
		debug_printf("%ld  ",points_beacons[0][i].coordX) ;
	}
	debug_printf("\n\nPoints balise 1 \n\ny= ");
	for(i=0;i<nb_points_B1;i++){
		debug_printf("%ld  ", points_beacons[0][i].coordY) ;
	}
	debug_printf("\n\nPoints balise 2 \n\nx= ");
	for(i=0;i<nb_points_B2;i++){
		debug_printf("%ld  ", points_beacons[1][i].coordX) ;
	}
	debug_printf("\n\nPoints balise 2 \n\ny= ");
	for(i=0;i<nb_points_B2;i++){
		debug_printf("%ld  ", points_beacons[1][i].coordY) ;
	}
	debug_printf("Points balise 3 \n\nx= ");
	for(i=0;i<nb_points_B3;i++){
		debug_printf("%ld  ",points_beacons[2][i].coordX) ;
	}
	debug_printf("Points balise 3 \n\ny= ");
	for(i=0;i<nb_points_B3;i++){
		debug_printf("%ld  ",points_beacons[2][i].coordY) ;
	}
}

//Fonction servant à détecter le centre des balises
void dectect_centre_beacons(){

}
#endif


