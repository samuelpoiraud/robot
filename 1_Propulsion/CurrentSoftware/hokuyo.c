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

	#define HOKUYO_BUFFER_READ_TIMEOUT	500		// ms

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
		static position robot, currentRobot[10]; //robot est la position stocké en mémoire et currentRobot correspond aux positions calculées par triangulation
		static HOKUYO_adversary_position beacon1, beacon2, beacon3; //position des balises mesurées centre de la balise, angle
		static Uint8 B1detected=0, B2detected=0, B3detected=0; //variables indiquant si la balise est détectée
		static HOKUYO_adversary_position points_beacons[3][NB_MESURES_HOKUYO*20]; // mesures réalisées par l'hokuyo
		static Uint8 nb_points_B1=0,nb_points_B2=0,nb_points_B3=0;  //le nombre de points de mesures par balises
		static Uint8 nb_balayages=0; //correspond aux nombres de balayage de 270° réalisé par l'hokuyo
		//static Uint8 droiteRegression1[3], droiteRegression2[3];
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
			hokuyo_write_command((Uint8*)"MS0000108001001");
			robot_position_during_measurement = global.position;	//On garde en mémoire l'angle du robot au moment où on lance la mesure Hokuyo. (est-ce le moment le plus pertinent...?)
			datas_index=0;
			state=BUFFER_READ;
		break;
		case BUFFER_READ:
			if(entrance)
				buffer_read_time_begin = global.absolute_time;
			hokuyo_read_buffer();
			if(HOKUYO_datas[datas_index-2]==0x0A && HOKUYO_datas[datas_index-1]==0x0A && datas_index>=2274)
				state=REMOVE_LF;
			else if(datas_index>2278)
				state=ASK_NEW_MEASUREMENT;
			else if(global.absolute_time - buffer_read_time_begin > HOKUYO_BUFFER_READ_TIMEOUT)
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
				debug_printf("\nEntrer dans TRI POINTS \n");
				tri_points();
				if(nb_balayages==NB_MESURES_HOKUYO){
					debug_printf("\n##########Debut détection centres balises##########\n");
					find_beacons_centres();
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
					|| (color==BOT_COLOR && x_absolute<ECART_BALISE && x_absolute>-(62+ECART_BALISE) && y_absolute<ECART_BALISE && y_absolute>-(62+ECART_BALISE) )  //un point est retenu s'il fait partie d'une balise fixe de sa couleur sur le bord du terrain
					|| (color==BOT_COLOR && x_absolute<2062+ECART_BALISE && x_absolute>2000-ECART_BALISE && y_absolute<ECART_BALISE && y_absolute>-(62+ECART_BALISE) )
					|| (color==BOT_COLOR && x_absolute<1031+ECART_BALISE && x_absolute>979+ECART_BALISE && y_absolute<3062+ECART_BALISE && y_absolute>3000-ECART_BALISE )
					|| (color==TOP_COLOR && x_absolute<ECART_BALISE && x_absolute>-62+ECART_BALISE && y_absolute<3062+ECART_BALISE && y_absolute>3000-ECART_BALISE )  //un point est retenu s'il fait partie d'une balise fixe de sa couleur sur le bord du terrain
					|| (color==TOP_COLOR && x_absolute<2062+ECART_BALISE && x_absolute>2000-ECART_BALISE && y_absolute<3062+ECART_BALISE && y_absolute>3000-ECART_BALISE )
					|| (color==TOP_COLOR && x_absolute<1031+ECART_BALISE && x_absolute>979+ECART_BALISE && y_absolute<ECART_BALISE && y_absolute>-(62+ECART_BALISE) )
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
	debug_printf("####TRIANGULATION####");
	debug_printf("B1_detected=%d \n", B1detected);
	debug_printf("B2_detected=%d \n", B2detected);
	debug_printf("B3_detected=%d \n", B3detected);

	//Repérage à une balise (nécessite d'avoir l'angle du robot)
	//balise 1
	if(B1detected==1){
		debug_printf("Triangulation B1\n");
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
		debug_printf("FIN\n");
	}

	//balise 2
	if(B2detected==1){
		debug_printf("Triangulation B2\n");
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
		debug_printf("FIN\n");

	}

	//balise 3
	if(B3detected==1){
		debug_printf("Triangulation B3\n");
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
		debug_printf("FIN\n");
	}

	//Repérage à deux balises
	//balises 1 et 2
	if(B1detected==1 && B2detected==1){
		debug_printf("Triangulation B1 & B2\n");
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
		debug_printf("FIN\n");
	}
	//balises 1 et 3
	if(B1detected==1 && B3detected==1){
		debug_printf("Triangulation B1 & B3\n");
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
		debug_printf("FIN\n");
	}
	//balises 2 et 3
	if(B2detected==1 && B3detected==1){
		debug_printf("Triangulation B2 & B3\n");
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
	debug_printf("FIN\n");
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
	debug_printf("POSITION ROBOT: x=%ld   y=%ld \n", robot.x, robot.y);
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

		nb_balayages++;
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

	if(nb_points_B1 > 7) B1detected=1;
	if(nb_points_B2 > 7) B2detected=1;
	if(nb_points_B3 > 7) B3detected=1;


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

//Fonction de tri des tableaux de mesures (tri à bulles)
void tri_tableau(HOKUYO_adversary_position points_beacons[3][NB_MESURES_HOKUYO*20], Uint16 nb_points, Uint8 numero_beacon){
	bool_e change = 0;
	Uint8 i;
	int aux;

	while(change==1){
		change=0;
		for(i=0 ; i<nb_points-1; i++){
			if(points_beacons[numero_beacon-1][i].teta>points_beacons[numero_beacon-1][i+1].teta){
				aux=points_beacons[numero_beacon-1][i+1].teta;
				points_beacons[numero_beacon-1][i+1].teta=points_beacons[numero_beacon-1][i].teta;
				points_beacons[numero_beacon-1][i].teta=aux;
				change=1;
			}
		}
	}
}

//Fonction prenant la moyenne des points mesurés pour chaque angle donné (ie tout les 0,25°)
void moyenne_mesures(HOKUYO_adversary_position points_beacons[3][NB_MESURES_HOKUYO*20], Uint16 nb_points, Uint8 numero_beacon){
	Uint32 nb_points_moyenne=1, nb_points_new=0, i, moyenneX, moyenneY;
	moyenneX=points_beacons[numero_beacon][0].coordX;
	moyenneX=points_beacons[numero_beacon][0].coordY;

	for(i=1 ; i<nb_points ; i++){
		if(fabs(points_beacons[numero_beacon][i].teta-points_beacons[numero_beacon][i-1].teta)<0.001){ //on teste l'égalité des deux angles
			moyenneX =  (moyenneX*nb_points_moyenne + points_beacons[numero_beacon][i].coordX)/(nb_points_moyenne+1.0);
			moyenneY =  (moyenneY*nb_points_moyenne + points_beacons[numero_beacon][i].coordY)/(nb_points_moyenne+1.0);
			nb_points_moyenne++;
		}else{
			points_beacons[numero_beacon][nb_points_new].coordX = moyenneX;  //on stocke la moyenne effectuée pour un angle donné
			points_beacons[numero_beacon][nb_points_new].coordY = moyenneY;
			nb_points_new++;

			moyenneX=points_beacons[numero_beacon][i].coordX; //on initialise les variables pour le calcul de l'angle suivant
			moyenneX=points_beacons[numero_beacon][i].coordY;
			nb_points_moyenne=1;
		}
	}

	//on stocke le dernier point
	points_beacons[numero_beacon][nb_points_new].coordX = moyenneX;  //on stocke la moyenne effectuée pour un angle donné
	points_beacons[numero_beacon][nb_points_new].coordY = moyenneY;
	nb_points = nb_points_new+1;

	debug_printf("Moyenne Points \n\nx= ");
	for(i=0;i<nb_points;i++){
		debug_printf("%ld  ",points_beacons[numero_beacon-1][i].coordX) ;
	}
	debug_printf("Moyenne Points \n\ny= ");
	for(i=0;i<nb_points;i++){
		debug_printf("%ld  ",points_beacons[numero_beacon-1][i].coordY) ;
	}
}

//Fonction réalisant une régression circulaire d'un nuage de points
HOKUYO_adversary_position regression_circulaire(HOKUYO_adversary_position points_beacons[3][NB_MESURES_HOKUYO*20], Uint16 nb_points, Uint8 numero_beacon){
	//Uint8 i,j,k;
	//float exprX=0, exprY=0;
	HOKUYO_adversary_position beacon;
	//METHODE 1
	/*#################################################################################################################
	 for(i=1;i<nb_points-2;i++){
		for(j=i+1;j<nb_points-1;j++){
			for(k=j+1;j<nb_points;k++){
				if( ( (puissance(points_beacons[numero_beacon-1][i].coordX,2)*(points_beacons[numero_beacon-1][j].coordY-points_beacons[numero_beacon-1][k].coordY)
					   +puissance(points_beacons[numero_beacon-1][j].coordX,2)*(points_beacons[numero_beacon-1][k].coordY-points_beacons[numero_beacon-1][i].coordY)
					   +puissance(points_beacons[numero_beacon-1][k].coordX,2)*(points_beacons[numero_beacon-1][i].coordY-points_beacons[numero_beacon-1][j].coordY))
					  -((points_beacons[numero_beacon-1][i].coordY-points_beacons[numero_beacon-1][j].coordY)*
					   (points_beacons[numero_beacon-1][j].coordY-points_beacons[numero_beacon-1][k].coordY)*
					   (points_beacons[numero_beacon-1][k].coordY-points_beacons[numero_beacon-1][i].coordY)) )
				   /  (points_beacons[numero_beacon-1][i].coordX*(points_beacons[numero_beacon-1][j].coordY-points_beacons[numero_beacon-1][k].coordY)
					 +points_beacons[numero_beacon-1][j].coordX*(points_beacons[numero_beacon-1][k].coordY-points_beacons[numero_beacon-1][i].coordY)
					 +points_beacons[numero_beacon-1][k].coordX*(points_beacons[numero_beacon-1][i].coordY-points_beacons[numero_beacon-1][j].coordY))!=0){
				exprX = exprX + ( (puissance(points_beacons[numero_beacon-1][i].coordX,2)*(points_beacons[numero_beacon-1][j].coordY-points_beacons[numero_beacon-1][k].coordY)
								   +puissance(points_beacons[numero_beacon-1][j].coordX,2)*(points_beacons[numero_beacon-1][k].coordY-points_beacons[numero_beacon-1][i].coordY)
								   +puissance(points_beacons[numero_beacon-1][k].coordX,2)*(points_beacons[numero_beacon-1][i].coordY-points_beacons[numero_beacon-1][j].coordY))
								  -((points_beacons[numero_beacon-1][i].coordY-points_beacons[numero_beacon-1][j].coordY)*
								   (points_beacons[numero_beacon-1][j].coordY-points_beacons[numero_beacon-1][k].coordY)*
								   (points_beacons[numero_beacon-1][k].coordY-points_beacons[numero_beacon-1][i].coordY)) )
							   /  (points_beacons[numero_beacon-1][i].coordX*(points_beacons[numero_beacon-1][j].coordY-points_beacons[numero_beacon-1][k].coordY)
								 +points_beacons[numero_beacon-1][j].coordX*(points_beacons[numero_beacon-1][k].coordY-points_beacons[numero_beacon-1][i].coordY)
								 +points_beacons[numero_beacon-1][k].coordX*(points_beacons[numero_beacon-1][i].coordY-points_beacons[numero_beacon-1][j].coordY));
				}else{
					debug_printf("Division par 0 \n");
				}

				if(( (puissance(points_beacons[numero_beacon-1][i].coordY,2)*(points_beacons[numero_beacon-1][j].coordX-points_beacons[numero_beacon-1][k].coordX)
					  +puissance(points_beacons[numero_beacon-1][j].coordY,2)*(points_beacons[numero_beacon-1][k].coordX-points_beacons[numero_beacon-1][i].coordX)
					  +puissance(points_beacons[numero_beacon-1][k].coordY,2)*(points_beacons[numero_beacon-1][i].coordX-points_beacons[numero_beacon-1][j].coordX))
					 -((points_beacons[numero_beacon-1][i].coordX-points_beacons[numero_beacon-1][j].coordX)*
					  (points_beacons[numero_beacon-1][j].coordX-points_beacons[numero_beacon-1][k].coordX)*
					  (points_beacons[numero_beacon-1][k].coordX-points_beacons[numero_beacon-1][i].coordX)) )
				  /  (points_beacons[numero_beacon-1][i].coordY*(points_beacons[numero_beacon-1][j].coordX-points_beacons[numero_beacon-1][k].coordX)
					+points_beacons[numero_beacon-1][j].coordY*(points_beacons[numero_beacon-1][k].coordX-points_beacons[numero_beacon-1][i].coordX)
					+points_beacons[numero_beacon-1][k].coordY*(points_beacons[numero_beacon-1][i].coordX-points_beacons[numero_beacon-1][j].coordX))!=0){

				exprY = exprY + ( (puissance(points_beacons[numero_beacon-1][i].coordY,2)*(points_beacons[numero_beacon-1][j].coordX-points_beacons[numero_beacon-1][k].coordX)
								   +puissance(points_beacons[numero_beacon-1][j].coordY,2)*(points_beacons[numero_beacon-1][k].coordX-points_beacons[numero_beacon-1][i].coordX)
								   +puissance(points_beacons[numero_beacon-1][k].coordY,2)*(points_beacons[numero_beacon-1][i].coordX-points_beacons[numero_beacon-1][j].coordX))
								  -((points_beacons[numero_beacon-1][i].coordX-points_beacons[numero_beacon-1][j].coordX)*
								   (points_beacons[numero_beacon-1][j].coordX-points_beacons[numero_beacon-1][k].coordX)*
								   (points_beacons[numero_beacon-1][k].coordX-points_beacons[numero_beacon-1][i].coordX)) )
							   /  (points_beacons[numero_beacon-1][i].coordY*(points_beacons[numero_beacon-1][j].coordX-points_beacons[numero_beacon-1][k].coordX)
								 +points_beacons[numero_beacon-1][j].coordY*(points_beacons[numero_beacon-1][k].coordX-points_beacons[numero_beacon-1][i].coordX)
								 +points_beacons[numero_beacon-1][k].coordY*(points_beacons[numero_beacon-1][i].coordX-points_beacons[numero_beacon-1][j].coordX));
				}else{
					debug_printf("Division par 0 \n");
				}
			}

		}
	}
	beacon.coordX = factorielle(3)*factorielle(nb_points-3)*exprX/(2.*factorielle(nb_points));
	beacon.coordX = factorielle(3)*factorielle(nb_points-3)*exprY/(2.*factorielle(nb_points));
	*/
	//METHODE 2
/*########################################################################################################################
	float moyenneX=0, moyenneY=0,    varX=0,  varY=0,      covXY=0,           covX2Y=0,           covXY2=0,         covXX2=0,       covYY2=0;
	//       espérence de X et Y , variance de X et Y,  covariance(X,Y),  covariance(X²,Y), covariance(X,Y²), covariance(X,X²), covariance(Y,Y²)
	float  exprA, exprB, exprC, exprD, exprE; // différentes expressions notées A, B, C ,D et E qui servent d'intermédiaire aux calculs
	Uint8 i;


	debug_printf("\nregression circulaire :");
	//Calculs de probabilités
	for(i=0 ; i<nb_points ; i++){
		moyenneX += points_beacons[numero_beacon-1][i].coordX;
		moyenneY += points_beacons[numero_beacon-1][i].coordY;
	}
	moyenneX = moyenneX/(1.*nb_points);
	moyenneY = moyenneY/(1.*nb_points);
	display_float(moyenneX)
	display_float(moyenneY)
	display(puissance(4,2));
	for(i=0 ; i<nb_points ; i++){
		varX   += puissance(points_beacons[numero_beacon-1][i].coordX-moyenneX , 2);
		varY   += puissance(points_beacons[numero_beacon-1][i].coordY-moyenneY , 2);
		covXY  += (points_beacons[numero_beacon-1][i].coordX-moyenneX)*(points_beacons[numero_beacon-1][i].coordY - moyenneY);
		covX2Y += (puissance(points_beacons[numero_beacon-1][i].coordX,2)-puissance(moyenneX,2))*(points_beacons[numero_beacon-1][i].coordY -moyenneY);
		covXY2 += (points_beacons[numero_beacon-1][i].coordX-moyenneX)*(puissance(points_beacons[numero_beacon-1][i].coordY,2) - puissance(moyenneY,2));
		covXX2 += (points_beacons[numero_beacon-1][i].coordX-moyenneX)*(puissance(points_beacons[numero_beacon-1][i].coordX,2)-puissance(moyenneX,2));
		covYY2 += (points_beacons[numero_beacon-1][i].coordY-moyenneY)*(puissance(points_beacons[numero_beacon-1][i].coordY,2)-puissance(moyenneY,2));
	}


	varX = varX/(1.*nb_points);
	varY = varY/(1.*nb_points);
	covXY = covXY/(1.*nb_points);
	covX2Y = covX2Y/(1.*nb_points);
	covXY2 = covXY2/(1.*nb_points);
	covXX2 = covXX2/(1.*nb_points);
	covYY2 = covYY2/(1.*nb_points);

	display_float(varX)
	display_float(varY)
	display_float(covXY)
	display_float(covX2Y)
	display_float(covXY2)
	display_float(covXX2)
	display_float(covYY2)


	//Calculs des différentes expressions
	exprA = nb_points*(nb_points-1)*varX;
	exprB = nb_points*(nb_points-1)*covXY;
	exprC = nb_points*(nb_points-1)*varY;
	exprD = 0.5*nb_points*(nb_points-1)*(covXX2+covXY2);
	exprE = 0.5*nb_points*(nb_points-1)*(covX2Y+covYY2);

	display_float(exprA);
	display_float(exprB);
	display_float(exprC);
	display_float(exprD);
	display_float(exprE);


	//calcul du centre de la balise
	beacon.coordX = (exprD*exprC - exprB*exprE)/(1.*(exprA*exprC - exprB*exprB));
	beacon.coordY = (exprA*exprE - exprB*exprD)/(1.*(exprA*exprC - exprB*exprB));
	beacon.dist   = sqrt(puissance(robot.x-beacon.coordX,2) + puissance(robot.x-beacon.coordX,2));
	beacon.teta   = atan((beacon.coordX-robot.x)/(beacon.coordY-robot.y))- robot.teta + PI4096;
	if(beacon.teta < -PI4096)  beacon.teta += 2*PI4096;
	if(beacon.teta >  PI4096)  beacon.teta -= 2*PI4096;*/

	//METHODE 3
//######################################################################################################################
	float sumX=0, sumY=0, sumX2=0, sumY2=0, sumX3=0, sumY3=0, sumXY=0, sumX2Y=0, sumXY2=0; //les sommes de 0 à nb_points-1 des coordonnées. ex: sumX=somme de toutes les abscisses
	float c11=0, c20=0, c30=0, c21=0, c02=0, c03=0, c12=0; //des coefficients intermédiaires de calculs
	uint8 i;

	//Calcul des sommes
	for(i=0;i<nb_points; i++){
		sumX=sumX+points_beacons[numero_beacon-1][i].coordX;
		sumY=sumY+points_beacons[numero_beacon-1][i].coordY;
		sumX2=sumX2+puissance(points_beacons[numero_beacon-1][i].coordX,2);
		sumY2=sumY2+puissance(points_beacons[numero_beacon-1][i].coordY,2);
		sumX3=sumX3+puissance(points_beacons[numero_beacon-1][i].coordX,3);
		sumY3=sumY3+puissance(points_beacons[numero_beacon-1][i].coordY,3);
		sumXY=sumXY+points_beacons[numero_beacon-1][i].coordX*points_beacons[numero_beacon-1][i].coordY;
		sumX2Y=sumX2Y+puissance(points_beacons[numero_beacon-1][i].coordX,2)*points_beacons[numero_beacon-1][i].coordY;
		sumXY2=sumXY2+points_beacons[numero_beacon-1][i].coordX*puissance(points_beacons[numero_beacon-1][i].coordY,2);
	}

	//Calcul des coefficients
	c11=nb_points*sumXY-sumX*sumY;
	c20=nb_points*sumX2-sumX*sumX;
	c30=nb_points*sumX3-sumX2*sumX;
	c21=nb_points*sumX2Y-sumX2*sumY;
	c02=nb_points*sumY2-sumY*sumY;
	c03=nb_points*sumY3-sumY2*sumY;
	c12=nb_points*sumXY2-sumX*sumY2;


	//calcul du centre de la balise
	beacon.coordX = ((c30+c12)*c02-(c03+c21)*c11)/(2.*(c20*c02-c11*c11));
	beacon.coordY = ((c03+c21)*c20-(c30+c12)*c11)/(2.*(c20*c02-c11*c11));
	beacon.dist   = sqrt(puissance(robot.x-beacon.coordX,2) + puissance(robot.x-beacon.coordX,2));
	beacon.teta   = atan((beacon.coordX-robot.x)/(beacon.coordY-robot.y))- robot.teta + PI4096;
	if(beacon.teta < -PI4096)  beacon.teta += 2*PI4096;
	if(beacon.teta >  PI4096)  beacon.teta -= 2*PI4096;


	return beacon;
}

//Fonction trouvant le centre des balises
void find_beacons_centres(){
	robot.x = global.position.x;
	robot.y = global.position.y;
	robot.teta = global.position.teta;
	robot.weight = 1;
	//Uint8 i;


	if(B1detected==1){
		tri_tableau(points_beacons, nb_points_B1, 1);
		moyenne_mesures(points_beacons, nb_points_B1, 1);
		beacon1 = regression_circulaire(points_beacons, nb_points_B1,1);
		debug_printf("\nCentre balise 1: x=%ld  y=%ld \n\n", beacon1.coordX, beacon1.coordY);
	}

	if(B2detected==1){
		tri_tableau(points_beacons, nb_points_B2, 2);
		moyenne_mesures(points_beacons, nb_points_B2, 2);
		beacon2 = regression_circulaire(points_beacons, nb_points_B2,2);
		debug_printf("\nCentre balise 2: x=%ld  y=%ld \n\n", beacon2.coordX, beacon2.coordY);
	}

	if(B3detected==1){
		tri_tableau(points_beacons, nb_points_B3, 3);
		moyenne_mesures(points_beacons, nb_points_B3, 3);
		beacon3 = regression_circulaire(points_beacons, nb_points_B3,3);
		debug_printf("\nCentre balise 3: x=%ld  y=%ld \n\n", beacon3.coordX, beacon3.coordY);
	}
	triangulation();
	debug_printf("FIN TRIANGULATION\n");
	nb_balayages=0;

	nb_points_B1=0;
	nb_points_B2=0;
	nb_points_B3=0;
	B1detected=0;
	B2detected=0;
	B3detected=0;
}

/*
//Fonction réalisant la regression linéaire d'un nuage de points
void regression_linéaire(HOKUYO_adversary_position tableauM[], Uint8 nb_points, Uint8 droiteRegression[]){
	Uint8 i, moyenneX=0, moyenneY=0, covariance=0, varX=0, varY=0;  //moyenne en X et en Y, covariance de X et Y, variance en X et en Y

	for(i=0 ; i<nb_points ; i++){
		moyenneX += tableauM[i].coordX;
		moyenneY += tableauM[i].coordY;
	}
	moyenneX = moyenneX/(1.*nb_points);
	moyenneY = moyenneY/(1.*nb_points);

	for(i=0 ; i<nb_points ; i++){
		covariance += (tableauM[i].coordX-moyenneX) * (tableauM[i].coordY-moyenneY);
		varX += puissance(tableauM[i].coordX-moyenneX , 2);
		varY += puissance(tableauM[i].coordY-moyenneY , 2);
	}
	covariance = covariance/(1.*nb_points);
	varX = varX/(1.*nb_points);
	varY = varY/(1.*nb_points);

	//regression en Y
	droiteRegression[0]=0; //La première case stocke le type de régression linéaire '0' pour une régression en X et '1' en Y
	droiteRegression[1]=covariance/varX;
	if(droiteRegression[1]>-30 && droiteRegression[0]<30){
		droiteRegression[2] = moyenneY-droiteRegression[0]*moyenneX;
	}else{  //la regression linéaire en X est plus adaptée
		droiteRegression[0]=1;
		droiteRegression[1]=covariance/varY;                         // x= droiteRegression[0]*y + droiteRegression[1] ?? à voir pour le tableau
		droiteRegression[2]=moyenneX-droiteRegression[0]*moyenneY;
	}
}

//Fonction trouvant le centre d'une balise
void find_beacon_centre(Uint8 numero_beacon, HOKUYO_adversary_position tableauM[], Uint8 nb_points){
	Uint8 x, y, vectX, vectY, xA, yA; //A est un point de la droite de régression linéaire
	Uint8 extrem1X, extrem1Y, extrem
	//Faire affectation regression_linéaire
	if(){
		if(droiteRegression1[0]==0){
			xA=tableauM[0].coordX;
			yA=droiteRegression1[1]*xA+droiteRegression1[2];
			vectX=droiteRegression1[1]-xA;
			vectY=droiteRegression1[2]-yA;
			//faire le projeté orthogonal => milieu
			//et pareil en Y
		}


	}else if(/){
		if(droiteRegression1[0]==0 && droiteRegression2[0]==0){  //deux régressions en Y
			on doit résoudre un système du type y=ax+b   et  y=cx+d
			 L'intersection donne un sommet de la balise carré
			x=(droiteRegression2[2]-droiteRegression1[2])/(droiteRegression1[1]-droiteRegression2[1]);
			y=droiteRegression1[1]*x+droiteRegression1[2];
		}else if(droiteRegression1[0]==0 && droiteRegression2[0]==1){  //une régression en Y et une en X (cas qui devrait se produire 50% du temps)
			on doit résoudre un système du type y=ax+b   et  x=cy+d
			 L'intersection donne un sommet de la balise carré
			x=(droiteRegression2[1]*droiteRegression1[2]+droiteRegression2[2])/(1-droiteRegression1[1]*droiteRegression2[1]);
			y=droiteRegression1[1]*x+droiteRegression1[2];
		}else if(droiteRegression1[0]==1 && droiteRegression2[0]==0){  //une régression en Y et une en X (cas qui devrait se produire 50% du temps)
			on doit résoudre un système du type x=ay+b   et  y=cx+d
			 L'intersection donne un sommet de la balise carré
			x=(droiteRegression1[1]*droiteRegression2[2]+droiteRegression1[2])/(1-droiteRegression2[1]*droiteRegression1[1]);
			y=droiteRegression2[1]*x+droiteRegression2[2];
		}else if(droiteRegression1[0]==1 && droiteRegression2[0]==1){  //deux régressions en X
			on doit résoudre un système du type x=ay+b   et  x=cy+d
			 L'intersection donne un sommet de la balise carré
			y=(droiteRegression2[2]-droiteRegression1[2])/(droiteRegression1[1]-droiteRegression2[1]);
			x=droiteRegression1[1]*y+droiteRegression1[2];
		}
	}
}
*/

#endif


