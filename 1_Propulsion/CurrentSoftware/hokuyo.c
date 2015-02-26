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
	#include "QS/QS_maths.h"
	#include "QS/QS_can_over_uart.h"
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
	#include "hokuyo.h"
	#include "calculator.h"
	#include "secretary.h"
	#include "detection.h"
	#include "odometry.h"




#ifdef USE_HOKUYO

// Temps d'acquisition
// MS : 45 - 50  ms
// ME : 70 - 100 ms
	#define USE_COMMAND_ME	// afin de récupérer la distance et l'intensité, sinon récupére juste la distance

	#define HOKUYO_BUFFER_READ_TIMEOUT	500		// ms

	#define HOKUYO_OFFSET_ANGLE_RAD4096 (9651) //135 degrés
	#define HOKUYO_ANGLE_ROBOT_TERRAIN 0
	#define HOKUYO_DETECTION_MARGE 130
	#define HOKUYO_EVITEMENT_MIN 150
	#define HOKUYO_MARGIN_FIELD_SIDE_IGNORE 100

	#define HOKUYO_OFFSET_BIG_POS_X			4.5
	#define HOKUYO_OFFSET_BIG_POS_Y			0
	#define HOKUYO_OFFSET_SMALL_POS_X		0
	#define HOKUYO_OFFSET_SMALL_POS_Y		0

	#define GROS_ROBOT_HOKUYO_TOO_CLOSE_DISTANCE_IGNORE		250	//Distance d'un point trop proche de nous qui doit être ignoré.
	#define PETIT_ROBOT_HOKUYO_TOO_CLOSE_DISTANCE_IGNORE	150	//Distance d'un point trop proche de nous qui doit être ignoré.

	#define ROBOT_COORDX 150
	#define ROBOT_COORDY 150
	#define DISTANCE_POINTS_IN_THE_SAME_OBJECT 150


	#define CORNER_SQUARE				150
	#define MARGIN						100

	#define MARCHE_RECT_X				0
	#define	MARCHE_RECT_Y				967
	#define	MARCHE_RECT_WIDTH			580			// Largeur en x
	#define	MARCHE_RECT_HEIGHT			1066		// Longueur en y

	#define BEGIN_ZONE_Y1_RECT_X		778
	#define	BEGIN_ZONE_Y1_RECT_Y		0
	#define	BEGIN_ZONE_Y1_WIDTH			444			// Largeur en x
	#define	BEGIN_ZONE_Y1_RECT_HEIGHT	580			// Longueur en y

	#define BEGIN_ZONE_Y2_RECT_X		840
	#define	BEGIN_ZONE_Y2_RECT_Y		580
	#define	BEGIN_ZONE_Y2_WIDTH			320			// Largeur en x
	#define	BEGIN_ZONE_Y2_RECT_HEIGHT	70			// Longueur en y

	#define BEGIN_ZONE_G1_RECT_X		778
	#define	BEGIN_ZONE_G1_RECT_Y		2420
	#define	BEGIN_ZONE_G1_WIDTH			444			// Largeur en x
	#define	BEGIN_ZONE_G1_RECT_HEIGHT	580			// Longueur en y

	#define BEGIN_ZONE_G2_RECT_X		840
	#define	BEGIN_ZONE_G2_RECT_Y		2350
	#define	BEGIN_ZONE_G2_WIDTH			320			// Largeur en x
	#define	BEGIN_ZONE_G2_RECT_HEIGHT	70			// Longueur en y


#ifdef USE_COMMAND_ME
	#define NB_BYTES_FROM_HOKUYO	6750
#else
	#define NB_BYTES_FROM_HOKUYO	2500
#endif

	#define PERIOD_SEND_ADVERSARIES_DATAS	120	//[ms]

	__ALIGN_BEGIN USB_OTG_CORE_HANDLE      USB_OTG_Core __ALIGN_END;
	__ALIGN_BEGIN USBH_HOST                USB_Host __ALIGN_END;


	static Uint8 HOKUYO_datas[NB_BYTES_FROM_HOKUYO]__attribute__((section(".ccm")));				//Données brutes issues du capteur HOKUYO
	static Uint32 datas_index=0;									//Index pour ces données

	#define NB_DETECTED_VALID_POINTS	1100
	static HOKUYO_point_position detected_valid_points[NB_DETECTED_VALID_POINTS]__attribute__((section(".ccm")));	//Points valides détectés par le capteur (X, Y, teta, distance)
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
	Sint32 hokuyo_get_detected_valid_point_dist(Uint16 point);

	//Deux fonctions pour detecter des regroupements de points
	void hokuyo_detection_ennemis(void);
	//void ReconObjet(void);
	void DetectRobots(void);
	void send_adversaries_datas(void);

	#define ECART_HOKUYO_A_DROITE 20 //matérialise le fait que l'hokuyo est décalé d'une certaine distance à doite par rapport au centre du robot
	#define ECART_BALISE 200
	HOKUYO_point_position points_beacons_valid[NB_DETECTED_VALID_POINTS]__attribute__((section(".ccm")));
	static Uint8 nb_valid_points_beacons=0;

	#ifdef TRIANGULATION
		void Hokuyo_validPointsAndBeacons();
		#define ECART_FIABILITE 80  //marge de la zone où les mesures sont considérées comme non-fiable
		#define ECART_POSITION  100  //marge au delà de laquelle un point n'est pas pris en compte dans le calcul de la position (=erreur)
		#define SEUIL_INTENSITE 1000 //le seuil d'intensité à partir de laquelle on détecte une balise
		static position robot, currentRobot[10]; //robot est la position stocké en mémoire et currentRobot correspond aux positions calculées par triangulation
		HOKUYO_adversary_position beacon1, beacon2, beacon3; //position des balises mesurées centre de la balise, angle
		static Uint8 B1detected=0, B2detected=0, B3detected=0; //variables indiquant si la balise est détectée
		HOKUYO_point_position points_beacons[3][NB_MESURES_HOKUYO*20]; // mesures réalisées par l'hokuyo
		static Uint16 nb_points_B1=0,nb_points_B2=0,nb_points_B3=0;  //le nombre de points de mesures par balises
		static Uint8 nb_balayages=0; //correspond aux nombres de balayage de 270° réalisé par l'hokuyo
		//static Uint8 droiteRegression1[3], droiteRegression2[3];

	#endif


//Fonction d'initialisation du périphérique USB
void HOKUYO_init(void)
{

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
			hokuyo_read_buffer();

#ifdef USE_COMMAND_ME
			if(datas_index > 1 && HOKUYO_datas[datas_index-2]==0x0A && HOKUYO_datas[datas_index-1]==0x0A && datas_index>=6730) // 0x0A -> LF (en ASCII)
				state=REMOVE_LF;
			else if(datas_index>6738)
				state=ASK_NEW_MEASUREMENT;
			else if(global.absolute_time - buffer_read_time_begin > HOKUYO_BUFFER_READ_TIMEOUT)
				state=ASK_NEW_MEASUREMENT;
#else
			if(datas_index > 1 && HOKUYO_datas[datas_index-2]==0x0A && HOKUYO_datas[datas_index-1]==0x0A && datas_index>=2274)
				state=REMOVE_LF;
			else if(datas_index>2278)
				state=ASK_NEW_MEASUREMENT;
			else if(global.absolute_time - buffer_read_time_begin > HOKUYO_BUFFER_READ_TIMEOUT)
				state=ASK_NEW_MEASUREMENT;
#endif
		break;
		case REMOVE_LF:
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
void hokuyo_read_buffer(void)
{
	while(!UART_USB_isRxEmpty())
	{
		HOKUYO_datas[datas_index] = UART_USB_read();
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

#ifdef USE_COMMAND_ME
//Fonction qui renvoi les coordonnées des points detectés sur le terrain
void hokuyo_find_valid_points(void){
	Uint16 a,b,c,d,e,f;
	Uint16 i, power_intensity;
	Sint32 distance;
	Sint32 angle=0;		//[°*100] centièmes de degrés
	Sint16 teta_relative, teta_absolute;	//[rad4096]
	Sint16 offset_x, offset_y, offset_pos_x, offset_pos_y;
	Sint32 x_absolute;
	Sint32 y_absolute;
	Sint16 cos;
	Sint16 sin;
	Sint32 to_close_distance;
	bool_e point_filtered;
	nb_valid_points = 0;	//RAZ des points valides.
	//nb_valid_points_beacons=0;
	color_e color= ODOMETRY_get_color();

	if(QS_WHO_AM_I_get() == BIG_ROBOT){
		to_close_distance = GROS_ROBOT_HOKUYO_TOO_CLOSE_DISTANCE_IGNORE;
		offset_pos_x = HOKUYO_OFFSET_BIG_POS_X;
		offset_pos_y = HOKUYO_OFFSET_BIG_POS_Y;
	}else{
		to_close_distance = PETIT_ROBOT_HOKUYO_TOO_CLOSE_DISTANCE_IGNORE;
		offset_pos_x = HOKUYO_OFFSET_SMALL_POS_X;
		offset_pos_y = HOKUYO_OFFSET_SMALL_POS_Y;
	}

	//TODO mesurer la durée d'exécution de cet algo...
	for(i = 47; i<datas_index-3;)	//Les données commencent  l'octet 47...
	{
		if(HOKUYO_datas[i+1] == '\n') // Pour la checksum et LF en fin de chaque ligne
			i+=2;
		if(HOKUYO_datas[i] == '\n')	//FIN DES DONNEES !!
			break;
		a = (Uint16)HOKUYO_datas[i++];
		b = (Uint16)HOKUYO_datas[i++];
		if(HOKUYO_datas[i+1] == '\n')
			i+=2;
		c = (Uint16)HOKUYO_datas[i++];
		d = (Uint16)HOKUYO_datas[i++];
		if(HOKUYO_datas[i+1] == '\n')
			i+=2;
		e = (Uint16)HOKUYO_datas[i++];
		f = (Uint16)HOKUYO_datas[i++];

		distance = ((a-0x30)<<12) + (((b-0x30)&0x3f)<<6) +(((c-0x30)&0x3f));  //cf datasheet de l'hokuyo... pour comprendre comment les données sont codées.

		if(distance	> to_close_distance)	//On élimine est distances trop petites (ET LES CAS DE REFLEXIONS TORP GRANDE OU LE CAPTEUR RENVOIE 1 !)
		{
			teta_relative = ((((Sint32)(angle))*183)>>8) + HOKUYO_OFFSET_ANGLE_RAD4096;	//Angle relatif au robot, du point en cours, en rad4096
			teta_relative = CALCULATOR_modulo_angle(teta_relative);
			teta_absolute = CALCULATOR_modulo_angle(teta_relative + robot_position_during_measurement.teta);				//angle absolu par rapport au terrain, pour le pt en cours, en rad4096

			COS_SIN_4096_get(teta_absolute,&cos,&sin);
			offset_x = (Sint16)(offset_pos_x*cos/4096.);
			offset_y = (Sint16)(offset_pos_y*sin/4096.);

			x_absolute = (distance*(Sint32)(cos))/4096 + robot_position_during_measurement.x + offset_x;
			y_absolute = (distance*(Sint32)(sin))/4096 + robot_position_during_measurement.y + offset_y;


			if(		x_absolute 	< 	FIELD_SIZE_X - HOKUYO_MARGIN_FIELD_SIDE_IGNORE &&
					x_absolute	>	HOKUYO_MARGIN_FIELD_SIDE_IGNORE &&
					y_absolute 	< 	FIELD_SIZE_Y - HOKUYO_MARGIN_FIELD_SIDE_IGNORE &&
					y_absolute 	>	HOKUYO_MARGIN_FIELD_SIDE_IGNORE)
			{		//Un point est retenu s'il est sur le terrain

				point_filtered = FALSE;	//On suppose que le point n'est pas filtré


				if(x_absolute < MARCHE_RECT_X + MARCHE_RECT_WIDTH + MARGIN
						&& y_absolute > MARCHE_RECT_Y
						&& y_absolute < MARCHE_RECT_Y + MARCHE_RECT_HEIGHT + MARGIN) 	//zones des marches
						point_filtered = TRUE;	//on refuse les points

				if(x_absolute > BEGIN_ZONE_Y1_RECT_X
						&& x_absolute < BEGIN_ZONE_Y1_RECT_X + BEGIN_ZONE_Y1_WIDTH
						&& y_absolute > BEGIN_ZONE_Y1_RECT_Y
						&& y_absolute < BEGIN_ZONE_Y1_RECT_Y + BEGIN_ZONE_Y1_RECT_HEIGHT) 	//zones de départ jaune partie basse
						point_filtered = TRUE;	//on refuse les points

				if(x_absolute > BEGIN_ZONE_Y2_RECT_X
						&& x_absolute < BEGIN_ZONE_Y2_RECT_X + BEGIN_ZONE_Y2_WIDTH
						&& y_absolute > BEGIN_ZONE_Y2_RECT_Y
						&& y_absolute < BEGIN_ZONE_Y2_RECT_Y + BEGIN_ZONE_Y2_RECT_HEIGHT) 	//zones de départ jaune partie haute
						point_filtered = TRUE;	//on refuse les points

				if(x_absolute > BEGIN_ZONE_G1_RECT_X
						&& x_absolute < BEGIN_ZONE_G1_RECT_X + BEGIN_ZONE_G1_WIDTH
						&& y_absolute > BEGIN_ZONE_G1_RECT_Y
						&& y_absolute < BEGIN_ZONE_G1_RECT_Y + BEGIN_ZONE_G1_RECT_HEIGHT) 	//zones de départ verte partie basse
						point_filtered = TRUE;	//on refuse les points

				if(x_absolute > BEGIN_ZONE_G2_RECT_X
						&& x_absolute < BEGIN_ZONE_G2_RECT_X + BEGIN_ZONE_G2_WIDTH
						&& y_absolute > BEGIN_ZONE_G2_RECT_Y
						&& y_absolute < BEGIN_ZONE_G2_RECT_Y + BEGIN_ZONE_G2_RECT_HEIGHT) 	//zones de départ verte partie haute
						point_filtered = TRUE;	//on refuse les points

				if(angle < 100*5 || angle > 100*265)//on retire les 5 premiers degrés et les 5 derniers
					point_filtered = TRUE;

				if(point_filtered == FALSE)
				{
					detected_valid_points[nb_valid_points].teta = teta_relative;	//L'angle enregistré permet l'évitement, c'est l'angle relatif !!!!!
					detected_valid_points[nb_valid_points].coordX = x_absolute;
					detected_valid_points[nb_valid_points].coordY = y_absolute;

					power_intensity = ((((Sint32)(d)-0x30)<<12) + ((((Sint32)(e)-0x30)&0x3f)<<6) +((((Sint32)(f)-0x30)&0x3f))) >> 3; // Décale de 3, car 18 bits(16 bits ici) et bit de signe
					detected_valid_points[nb_valid_points].power_intensity = (power_intensity > 0)?power_intensity : -1;

					if(nb_valid_points < NB_DETECTED_VALID_POINTS)
						nb_valid_points++;
				}
			}

			//filtrage des points correspondant aux balises extérieures
			if( (color==BOT_COLOR && x_absolute<ECART_BALISE && x_absolute>-(62+ECART_BALISE) && y_absolute<ECART_BALISE && y_absolute>-(62+ECART_BALISE) )  //un point est retenu s'il fait partie d'une balise fixe de sa couleur sur le bord du terrain
							 || (color==BOT_COLOR && x_absolute<2062+ECART_BALISE && x_absolute>2000-ECART_BALISE && y_absolute<ECART_BALISE && y_absolute>-(62+ECART_BALISE) )
							 || (color==BOT_COLOR && x_absolute<1031+ECART_BALISE && x_absolute>979+ECART_BALISE && y_absolute<3062+ECART_BALISE && y_absolute>3000-ECART_BALISE )
							 || (color==TOP_COLOR && x_absolute<ECART_BALISE && x_absolute>-62+ECART_BALISE && y_absolute<3062+ECART_BALISE && y_absolute>3000-ECART_BALISE )  //un point est retenu s'il fait partie d'une balise fixe de sa couleur sur le bord du terrain
							 || (color==TOP_COLOR && x_absolute<2062+ECART_BALISE && x_absolute>2000-ECART_BALISE && y_absolute<3062+ECART_BALISE && y_absolute>3000-ECART_BALISE )
							 || (color==TOP_COLOR && x_absolute<1031+ECART_BALISE && x_absolute>979+ECART_BALISE && y_absolute<ECART_BALISE && y_absolute>-(62+ECART_BALISE) )
						 )
			{
				point_filtered = FALSE;	//On suppose que le point n'est pas filtré

				if(angle < 100*5 || angle > 100*265)//on retire les 5 premiers degrés et les 5 derniers
					point_filtered = TRUE;

				if(point_filtered == FALSE)
				{
					points_beacons_valid[ nb_valid_points_beacons].teta = teta_relative;	//L'angle enregistré permet l'évitement, c'est l'angle relatif !!!!!
					points_beacons_valid[ nb_valid_points_beacons].coordX = x_absolute;
					points_beacons_valid[ nb_valid_points_beacons].coordY = y_absolute;

					power_intensity = ((((Sint32)(d)-0x30)<<12) + ((((Sint32)(e)-0x30)&0x3f)<<6) +((((Sint32)(f)-0x30)&0x3f))) >> 3; // Décale de 3, car 18 bits(16 bits ici) et bit de signe
					points_beacons_valid[ nb_valid_points_beacons].power_intensity = (power_intensity > 0)?power_intensity : -1;

					if( nb_valid_points_beacons < NB_DETECTED_VALID_POINTS)
						 nb_valid_points_beacons++;
				}
			}
	 }
		angle+=25;	//Centième de degré
	}

//	static int time = 0;
//	if(time > 10){
//		CAN_msg_t msg, msg2;
//		msg.sid = DEBUG_HOKUYO_RESET;
//		msg.size = 0;
//		CANmsgToU1tx(&msg);
//		msg2.sid = DEBUG_HOKUYO_INTENSITY_RESET;
//		msg2.size = 0;
//		CANmsgToU1tx(&msg2);

//		msg.sid = DEBUG_HOKUYO_ADD_POINT;
//		msg.size = 8;
//		msg2.sid = DEBUG_HOKUYO_INTENSITY_ADD_POINT;
//		msg2.size = 8;
//		int j = 0, j2 = 0;
//		for(i = 0; i < nb_valid_points; i++) {
//			msg.data[j++] = detected_valid_points[i].coordX >> 4;
//			msg.data[j++] = detected_valid_points[i].coordY >> 4;

//			COS_SIN_4096_get(CALCULATOR_modulo_angle(detected_valid_points[i].teta + robot_position_during_measurement.teta),&cos,&sin);
//			Sint16 x = ((Sint32)(detected_valid_points[i].power_intensity)*(Sint32)(cos))/4096 + robot_position_during_measurement.x + offset_pos_x;
//			Sint16 y = ((Sint32)(detected_valid_points[i].power_intensity)*(Sint32)(sin))/4096 + robot_position_during_measurement.y + offset_pos_y;

//			if( x < 0){
//				float y2 = y - robot_position_during_measurement.y;
//				Sint32 c = y2*y + (x - robot_position_during_measurement.x)*x;
//				y = ((y2 != 0)? (float)(c)/y2 : 0);
//				x = 0;
//			}if(y < 0){
//				Sint32 c = (x - robot_position_during_measurement.x)*x - (y - robot_position_during_measurement.y)*y;
//				x = ((x - robot_position_during_measurement.x != 0)? (float)(c)/(float)(x - robot_position_during_measurement.x) : 0);
//				y = 0;
//			}

//			msg2.data[j2++] = x >> 4;
//			msg2.data[j2++] = y >> 4;

//			if(j > 7){
//				CANmsgToU1tx(&msg);
//				CANmsgToU1tx(&msg2);
//				j = j2 = 0;
//			}
//		}

//		msg.size = j;
//		CANmsgToU1tx(&msg);
//		msg2.size = j;
//		CANmsgToU1tx(&msg2);
//		time = 0;
//	}
//	time++;
}

#else
//Fonction qui renvoi les coordonnées des points detectés sur le terrain
void hokuyo_find_valid_points(void){
	Uint16 a,b;
	Uint16 i;
	Sint32 distance;
	Sint32 angle=0;		//[°*100] centièmes de degrés
	Sint16 teta_relative;	//[rad4096]
	Sint16 teta_absolute;
	Sint16 offset_x, offset_y, offset_pos_x, offset_pos_y;
	Sint32 x_absolute;
	Sint32 y_absolute;
	Sint16 cos;
	Sint16 sin;
	Sint32 to_close_distance;
	bool_e point_filtered;
	nb_valid_points = 0;	//RAZ des points valides.

	if(QS_WHO_AM_I_get() == BIG_ROBOT){
		to_close_distance = GROS_ROBOT_HOKUYO_TOO_CLOSE_DISTANCE_IGNORE;
		offset_pos_x = HOKUYO_OFFSET_BIG_POS_X;
		offset_pos_y = HOKUYO_OFFSET_BIG_POS_Y;
	}else{
		to_close_distance = PETIT_ROBOT_HOKUYO_TOO_CLOSE_DISTANCE_IGNORE;
		offset_pos_x = HOKUYO_OFFSET_SMALL_POS_X;
		offset_pos_y = HOKUYO_OFFSET_SMALL_POS_Y;
	}

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
			offset_x = (Sint16)(offset_pos_x*cos/4096.);
			offset_y = (Sint16)(offset_pos_y*sin/4096.);

			COS_SIN_4096_get(teta_absolute,&cos,&sin);
			x_absolute = (distance*(Sint32)(cos))/4096 + robot_position_during_measurement.x + offset_x;
			y_absolute = (distance*(Sint32)(sin))/4096 + robot_position_during_measurement.y + offset_y;

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

				if(x_absolute < MARCHE_RECT_X + MARCHE_RECT_WIDTH + MARGIN
						&& y_absolute > MARCHE_RECT_Y
						&& y_absolute < MARCHE_RECT_Y + MARCHE_RECT_HEIGHT + MARGIN) 	//zones des marches
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

//	static int time = 0;
//	if(time > 20){
//		CAN_msg_t msg;
//		msg.sid = DEBUG_HOKUYO_RESET;
//		msg.size = 0;
//		CANmsgToU1tx(&msg);

//		msg.sid = DEBUG_HOKUYO_ADD_POINT;
//		msg.size = 8;

//		int j = 0;
//		for(i = 0; i < nb_valid_points; i++) {
//			msg.data[j++] = detected_valid_points[i].coordX >> 4;
//			msg.data[j++] = detected_valid_points[i].coordY >> 4;

//			if(j > 7){
//				CANmsgToU1tx(&msg);
//				j = 0;
//			}
//		}

//		msg.size = j;
//		CANmsgToU1tx(&msg);
//		time = 0;
//	}
//	time++;
}
#endif

//fonction qui renvoie la plus petite distance
Sint32 hokuyo_dist_min(Uint16 compt)
{
	Uint16 i;
	Sint32 dist_min;
	dist_min = hokuyo_get_detected_valid_point_dist(0);
	for(i=1;i<compt;i++)
	{
		if(hokuyo_get_detected_valid_point_dist(i) < dist_min)
			dist_min = hokuyo_get_detected_valid_point_dist(i);
	}
	debug_printf("dist min [%ld mm] \n",dist_min);
	//if(dist_min < HOKUYO_EVITEMENT_MIN)
	//LCD_DisplayStringLineColon(10,LCD_LINE_27,"WARNING ENNEMY DETECTED");
	return dist_min;
}

Sint32 hokuyo_get_detected_valid_point_dist(Uint16 point){
	return (Sint32)(sqrt((detected_valid_points[point].coordX - robot_position_during_measurement.x)*(detected_valid_points[point].coordX - robot_position_during_measurement.x) +
						 (detected_valid_points[point].coordY - robot_position_during_measurement.y)*(detected_valid_points[point].coordY - robot_position_during_measurement.y)));
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

Sint32 x1, x2, y2;
Sint16 a1=0, a2=0, a3=0, a4=0, moyAngle12=0, moyAngle34=0, condition12=0, condition34=0; //abscisse, ordonnée et angle
void triangulation(){
	Sint32 y1;
	robot.teta = global.position.teta;
	robot.x = global.position.x - ECART_HOKUYO_A_DROITE*sin4096(robot.teta);
	robot.y = global.position.y + ECART_HOKUYO_A_DROITE*cos4096(robot.teta);
	//robot.x = global.position.x;
	//robot.y = global.position.y;
	/*robot.x = 700;  //test
	robot.y = 1500;
	robot.teta= -PI4096;*/
	debug_printf("Robot  x=%ld  y=%ld  teta=%ld \n\n",robot.x, robot.y, robot.teta);

	debug_printf("\n####TRIANGULATION####\n");
	debug_printf("B1_detected=%d \n", B1detected);
	debug_printf("B2_detected=%d \n", B2detected);
	debug_printf("B3_detected=%d \n\n", B3detected);
	debug_printf("Beacon1  dist=%ld  teta=%d \n", beacon1.dist, beacon1.teta);
	debug_printf("Beacon2  dist=%ld  teta=%d \n", beacon2.dist, beacon2.teta);
	debug_printf("Beacon3  dist=%ld  teta=%d \n", beacon3.dist, beacon3.teta);
	//debug_printf("Après correction hokuyo Robot  x=%ld  y=%ld  teta=%ld \n\n",robot.x, robot.y, robot.teta);*/

	//Repérage à une balise (nécessite d'avoir l'angle du robot)
		//balise 1
		if(B1detected==1){
			//debug_printf("\nTriangulation B1");
			x1 = -beacon1.dist*sin((beacon1.teta+robot.teta)/4096.)-62;
			y1 = beacon1.dist*cos((beacon1.teta+robot.teta)/4096.)-62;

			x2 = beacon1.dist*sin((beacon1.teta+robot.teta)/4096.)-62;
			y2 = -beacon1.dist*cos((beacon1.teta+robot.teta)/4096.)-62;

			if(x1>0 && x1<2000 && y1>0 && y1<3000){
				currentRobot[0].x = x1;
				currentRobot[0].y = y1;
			}else{
				currentRobot[0].x = x2;
				currentRobot[0].y = y2;
			}
			currentRobot[0].teta=robot.teta;
			currentRobot[0].weight=5;

			debug_printf("\n currentRobot[0]: x1=%ld y1=%ld ", x1, y1);
			debug_printf("\n currentRobot[0]: x2=%ld y2=%ld ", x2, y2);
			debug_printf("\n currentRobot[0]: x=%ld y=%ld ", currentRobot[0].x, currentRobot[0].y);
			//debug_printf("FIN\n");
		}

		//balise 2
		if(B2detected==1){

			//debug_printf("\nTriangulation B2");
			x1= - ( puissance_float(tan4096((Sint16)(robot.teta/2 + beacon2.teta/2 + PI4096/4)),2)*(beacon2.dist-1000)-beacon2.dist-1000) / (puissance_float(tan4096((Sint16)(robot.teta/2 + beacon2.teta/2 + PI4096/4)),2)+1) ;
			y1 = 2*( (beacon2.dist*tan4096((Sint16)(robot.teta/2 + beacon2.teta/2 + PI4096/4))+1531*puissance_float(tan4096((Sint16)(robot.teta/2 + beacon2.teta/2 + PI4096/4)),2)+1531) / (puissance_float(tan4096((Sint16)(robot.teta/2 + beacon2.teta/2 + PI4096/4)),2)+1) );

			x2= ( puissance_float(tan4096((Sint16)(robot.teta/2 + beacon2.teta/2 + PI4096/4)),2)*(beacon2.dist+1000)-beacon2.dist+1000) / (puissance_float(tan4096((Sint16)(robot.teta/2 + beacon2.teta/2 + PI4096/4)),2)+1) ;
			y2 = -2*( (beacon2.dist*tan4096((Sint16)(robot.teta/2 + beacon2.teta/2 + PI4096/4))-1531*puissance_float(tan4096((Sint16)(robot.teta/2 + beacon2.teta/2 + PI4096/4)),2)-1531) / (puissance_float(tan4096((Sint16)(robot.teta/2 + beacon2.teta/2 + PI4096/4)),2)+1) );
			//debug_printf("robot.teta/2=%d\n",robot.teta/2);
			//debug_printf("robot.teta/2+beacon2.teta/2+PI4096/4=%d\n",(Sint16)(robot.teta/2 + beacon2.teta/2 + PI4096/4));
			//display_float(tan4096(robot.teta/2+beacon2.teta/2+PI4096/4));
			//display_float(puissance_float(tan4096(robot.teta/2+beacon2.teta/2+PI4096/4),2));
			//debug_printf("puissance valeur a tester= %f\n",puissance_float(tan4096(1/2.0*robot.teta+1/2.0*beacon2.teta+1/4.*PI4096),2));
			debug_printf("\n currentRobot[1]: x1=%ld y1=%ld ", x1, y1);
			debug_printf("\n currentRobot[1]: x2=%ld y2=%ld ", x2, y2);
			if(x1>0 && x1<2000 && y1>0 && y1<3000){
				currentRobot[1].x = x1;
				currentRobot[1].y = y1;
			}else{
				currentRobot[1].x = x2;
				currentRobot[1].y = y2;
			}
			currentRobot[1].teta=robot.teta;
			currentRobot[1].weight=5;
			debug_printf("\n currentRobot[1]: x=%ld y=%ld ", currentRobot[1].x, currentRobot[1].y);
			//debug_printf("FIN\n");

		}

		//balise 3
		if(B3detected==1){
			//debug_printf("\nTriangulation B3");
			x1 = -beacon3.dist*sin4096(robot.teta+beacon3.teta)+2062;
			y1 = beacon3.dist*cos4096(robot.teta+beacon3.teta)-62;

			x2 = beacon3.dist*sin4096(robot.teta+beacon3.teta)+2062;
			y2 = -beacon3.dist*cos4096(robot.teta+beacon3.teta)-62;

			if(x1>0 && x1<2000 && y1>0 && y1<3000){
				currentRobot[2].x = x1;
				currentRobot[2].y = y1;
			}else{
				currentRobot[2].x = x2;
				currentRobot[2].y = y2;
			}
			currentRobot[2].teta=robot.teta;
			currentRobot[2].weight=5;
			debug_printf("\n currentRobot[2]: x=%ld y=%ld ", currentRobot[2].x, currentRobot[2].y);
			//debug_printf("FIN\n");
		}

		//Repérage à deux balises
		//balises 1 et 2
		if(B1detected==1 && B2detected==1){
			//debug_printf("\nTriangulation B1 & B2");
			x1=((531/10887220.0)*(puissance(beacon1.dist,2)-puissance(beacon2.dist,2))+469-(781/5443610.0)*sqrt(-puissance(beacon1.dist,4)+2*puissance(beacon1.dist,2)*puissance(beacon2.dist,2)-puissance(beacon2.dist,4)+21774440*(puissance(beacon1.dist,2)+puissance(beacon2.dist,2))-118531559328400ULL));
			y1=(781/5443610.0)*(puissance(beacon1.dist,2)-puissance(beacon2.dist,2))+1500+(531/10887220.0)*sqrt(-puissance(beacon1.dist,4)+2*puissance(beacon1.dist,2)*puissance(beacon2.dist,2)-puissance(beacon2.dist,4)+21774440*(puissance(beacon1.dist,2)+puissance(beacon2.dist,2))-118531559328400ULL);
			a1= (PI4096/2 - beacon1.teta +atan4096(y1/(1.*x1)));
			a2= atan2_4096(3000-y1,1000-x1)-PI4096/2 -beacon2.teta;

			x2=(531/10887220.0)*(puissance(beacon1.dist,2)-puissance(beacon2.dist,2))+469+(781/5443610.0)*sqrt(-puissance(beacon1.dist,4)+2*puissance(beacon1.dist,2)*puissance(beacon2.dist,2)-puissance(beacon2.dist,4)+21774440*(puissance(beacon1.dist,2)+puissance(beacon2.dist,2))-(118531559328400ULL));
			y2=(781/5443610.0)*(puissance(beacon1.dist,2)-puissance(beacon2.dist,2))+1500-(531/10887220.0)*sqrt(-puissance(beacon1.dist,4)+2*puissance(beacon1.dist,2)*puissance(beacon2.dist,2)-puissance(beacon2.dist,4)+21774440*(puissance(beacon1.dist,2)+puissance(beacon2.dist,2))-(118531559328400ULL));
			a3= PI4096/2 - beacon1.teta + atan4096(y2/(1.*x2));
			a4= atan2_4096(3000-y2,1000-x2)-PI4096/2 -beacon2.teta;

			if(a1>PI4096)  a1-=2*PI4096;
			if(a1<-PI4096) a1+=2*PI4096;
			if(a2>PI4096)  a2-=2*PI4096;
			if(a2<-PI4096) a2+=2*PI4096;
			if(a3>PI4096)  a3-=2*PI4096;
			if(a3<-PI4096) a3+=2*PI4096;
			if(a4>PI4096)  a4-=2*PI4096;
			if(a4<-PI4096) a4+=2*PI4096;


			//debug_printf("moyangle12=%d  moyangle34=%d\n", moyAngle12, moyAngle34);

			//if(moyAngle12<atan4096(y1/(1.*x1))+PI4096/2 && moyAngle12>PI4096/2){		//inutile
			//	condition12= PI4096/2 - moyAngle12 + atan4096(y1/(1.*x1));

			//}else{
				condition12= 2*PI4096+ PI4096/2 - a1 + atan4096(y1/(1.*x1));  //moyAngle12
			//}
			//if(moyAngle34<atan4096(y2/(1.*x2))+PI4096/2 && moyAngle34>PI4096/2){
			//	condition34= PI4096/2 - moyAngle34 + atan4096(y2/(1.*x2));
			//}else{
				condition34= PI4096/2 - a3 + atan4096(y2/(1.*x2));    //moyAngle34
				//debug_printf("atan4096(y2/(1.*x2))=%d\n", atan4096(y2/(1.*x2)));
				//debug_printf("PI4096/2 - moyAngle34 + atan4096(y2/(1.*x2))=%d\n", PI4096/2 - moyAngle34 + atan4096(y2/(1.*x2)));
			//}
			//debug_printf("cond12=%d   cond34=%d \n", condition12, condition34);

			if(condition12<0) condition12+=2*PI4096;
			if(condition34<0) condition34+=2*PI4096;
			if(condition12>2*PI4096) condition12-=2*PI4096;
			if(condition34>2*PI4096) condition34-=2*PI4096;

			//On gère la coupure en -PI
			if(a1>1000 && a2<-10000){
				a2+=2*PI4096;
			}else if(a1<-1000 && a2>10000){
				 a1+=2*PI4096;
			}
			moyAngle12=(a1+a2)/2;

			if(a3>1000 && a4<-10000){
				a4+=2*PI4096;
			}else if(a3<-1000 && a4>10000){
				 a3+=2*PI4096;
			}
			moyAngle34=(a3+a4)/2;

			if(moyAngle12>PI4096)  moyAngle12-=2*PI4096;
			if(moyAngle12<-PI4096) moyAngle12+=2*PI4096;
			if(moyAngle34>PI4096)  moyAngle34-=2*PI4096;
			if(moyAngle34<-PI4096) moyAngle34+=2*PI4096;

			//la mesure des angles permet de déterminer quelle position est correcte
			//if(	fabs(beacon1.teta-condition12) < fabs(beacon1.teta-condition34) ){
			if(puissance(robot.x-x1,2)+puissance(robot.y-y1,2) < puissance(robot.x-x2,2)+puissance(robot.y-y2,2)){
				currentRobot[3].x=x1;
				currentRobot[3].y=y1;
				currentRobot[3].teta=moyAngle12;
				currentRobot[3].weight=10;
			}else{
				currentRobot[3].x=x2;
				currentRobot[3].y=y2;
				currentRobot[3].teta=moyAngle34;
				currentRobot[3].weight=10;
			}
			//debug_printf("1) atan2=%d       2) atan2=%d \n", atan2_4096(3000-y1,1000-x1), atan2_4096(3000-y2,1000-x2) );
			debug_printf("\n currentRobot[3]: x1=%ld y1=%ld angle1=%ld angle2=%ld", x1, y1, a1, a2);
			debug_printf("\n currentRobot[3]: x2=%ld y2=%ld angle3=%ld angle4=%ld", x2, y2, a3, a4);
			debug_printf("\n currentRobot[3]: x=%ld y=%ld angle=%ld  ", currentRobot[3].x, currentRobot[3].y, currentRobot[3].teta);
			//debug_printf("FIN\n");
		}
		//balises 1 et 3
		if(B1detected==1 && B3detected==1){
			//debug_printf("\nTriangulation B1 & B3");
			x1=(1/4248.0)*(puissance(beacon1.dist,2)-puissance(beacon3.dist,2))+1000;
			y1=-62+(1/4248.)*sqrt((-puissance(beacon1.dist,4)+2*puissance(beacon1.dist,2)*puissance(beacon3.dist,2)-puissance(beacon3.dist,4)+9022752*puissance(beacon1.dist,2)+9022752*puissance(beacon3.dist,2)-20352513413376ULL));

			x2=x1;//l'abscisse est identique
			y2=-62-sqrt((-puissance(beacon1.dist,4)+2*puissance(beacon1.dist,2)*puissance(beacon3.dist,2)-puissance(beacon3.dist,4)+9022752*(puissance(beacon1.dist,2)+puissance(beacon3.dist,2))-20352513413376ULL)/4248.);

			a1= PI4096/2 - beacon1.teta + atan4096(y1/(1.*x1));
			a2= PI4096-beacon3.teta+atan4096((2000-x1)/(1.*y1));
			a3= PI4096/2 - beacon1.teta + atan4096(y2/(1.*x2));
			a4= PI4096-beacon3.teta+atan4096((2000-x2)/(1.*y2));
			if(a1>PI4096)  a1-=2*PI4096;
			if(a1<-PI4096) a1+=2*PI4096;
			if(a2>PI4096)  a2-=2*PI4096;
			if(a2<-PI4096) a2+=2*PI4096;

			//On gère la coupure en -PI
			if(a1>1000 && a2<-10000){
				a2+=2*PI4096;
			}else if(a1<-1000 && a2>10000){
				 a1+=2*PI4096;
			}
			moyAngle12=(a1+a2)/2;

			//la mesure des angles permet de déterminer quelle position est correcte
			currentRobot[4].x=x1;
			currentRobot[4].y=y1;
			currentRobot[4].teta=moyAngle12;
			currentRobot[4].weight=10;

			/*debug_printf("\n currentRobot[4]: x1=%ld y1=%ld angle1=%ld angle2=%ld", x1, y1, a1, a2);
			debug_printf("\n currentRobot[4]: x2=%ld y2=%ld angle3=%ld angle4=%ld", x2, y2, a3, a4);*/
			debug_printf("\n currentRobot[4]: x=%ld y=%ld angle=%ld  ", currentRobot[4].x, currentRobot[4].y,currentRobot[4].teta);
			//debug_printf("FIN\n");
		}
		//balises 2 et 3
		if(B2detected==1 && B3detected==1){
			//debug_printf("\nTriangulation B2 & B3");
			x1=(531/10887220.0)*(puissance(beacon2.dist,2)-puissance(beacon3.dist,2))+1531+(781/5443610.0)*sqrt(-puissance(beacon2.dist,4)+2*puissance(beacon2.dist,2)*puissance(beacon3.dist,2)-puissance(beacon3.dist,4)+21774440*(puissance(beacon2.dist,2)+puissance(beacon3.dist,2))-118531559328400ULL);
			y1=-(781/5443610.0)*(puissance(beacon2.dist,2)-puissance(beacon3.dist,2))+1500+(531/10887220.0)*sqrt(-puissance(beacon2.dist,4)+2*puissance(beacon2.dist,2)*puissance(beacon3.dist,2)-puissance(beacon3.dist,4)+21774440*(puissance(beacon2.dist,2)+puissance(beacon3.dist,2))-118531559328400ULL);
			a1= PI4096-beacon3.teta+atan4096((2000-x1)/(1.*y1));
			a2= atan2_4096(3000-y1,1000-x1)-PI4096/2 -beacon2.teta;

			x2=(531/10887220.0)*(puissance(beacon2.dist,2)-puissance(beacon3.dist,2))+1531-(781/5443610.0)*sqrt(-puissance(beacon2.dist,4)+2*puissance(beacon2.dist,2)*puissance(beacon3.dist,2)-puissance(beacon3.dist,4)+21774440*(puissance(beacon2.dist,2)+puissance(beacon3.dist,2))-118531559328400ULL);
			y2=-(781/5443610.0)*(puissance(beacon2.dist,2)-puissance(beacon3.dist,2))+1500-(531/10887220.0)*sqrt(-puissance(beacon2.dist,4)+2*puissance(beacon2.dist,2)*puissance(beacon3.dist,2)-puissance(beacon3.dist,4)+21774440*(puissance(beacon2.dist,2)+puissance(beacon3.dist,2))-118531559328400ULL);
			a3= PI4096-beacon3.teta+atan4096((2000-x2)/(1.*y2));
			a4= atan2_4096(3000-y2,1000-x2)-PI4096/2 -beacon2.teta;

			if(a1>PI4096)  a1-=2*PI4096;
			if(a1<-PI4096) a1+=2*PI4096;
			if(a2>PI4096)  a2-=2*PI4096;
			if(a2<-PI4096) a2+=2*PI4096;
			if(a3>PI4096)  a3-=2*PI4096;
			if(a3<-PI4096) a3+=2*PI4096;
			if(a4>PI4096)  a4-=2*PI4096;
			if(a4<-PI4096) a4+=2*PI4096;


			//condition12= (moyAngle12<-PI4096+atan4096((2000-x1)/(y1*1.)) && moyAngle12>-PI4096) ? -PI4096-moyAngle12 + atan4096((2000-x1)/(1.*y1)): PI4096-moyAngle12 + atan4096((2000-x1)/(1.*y1));  //inutile
			//condition34= (moyAngle34<-PI4096+atan4096((2000-x2)/(y2*1.)) && moyAngle34>-PI4096) ? -PI4096-moyAngle34 + atan4096((2000-x2)/(1.*y2)): PI4096-moyAngle34 + atan4096((2000-x2)/(1.*y2));
			condition12= -PI4096- a1 + atan4096((2000-x1)/(1.*y1));	//moyAngle12
			condition34=  -PI4096- a3 + atan4096((2000-x2)/(1.*y2));  //moyAngle34
			if(condition12<0) condition12+=2*PI4096;
			if(condition34<0) condition34+=2*PI4096;
			if(condition12>2*PI4096) condition12-=2*PI4096;
			if(condition34>2*PI4096) condition34-=2*PI4096;
			debug_printf("\ncond12=%ld   cond34=%ld \n", condition12, condition34);

			//On gère la coupure en -PI
			if(a1>1000 && a2<-10000){
				a2+=2*PI4096;
			}else if(a1<-1000 && a2>10000){
				 a1+=2*PI4096;
			}
			moyAngle12=(a1+a2)/2;

			if(a3>1000 && a4<-10000){
				a4+=2*PI4096;
			}else if(a3<-1000 && a4>10000){
				 a3+=2*PI4096;
			}
			moyAngle34=(a3+a4)/2;

			if(moyAngle12>PI4096)  moyAngle12-=2*PI4096;
			if(moyAngle12<-PI4096) moyAngle12+=2*PI4096;
			if(moyAngle34>PI4096)  moyAngle34-=2*PI4096;
			if(moyAngle34<-PI4096) moyAngle34+=2*PI4096;

			//la mesure des angles permet de déterminer quelle position est correcte
			//if(	fabs(beacon3.teta-condition12) < fabs(beacon3.teta-condition34) ){
			if(puissance(robot.x-x1,2)+puissance(robot.y-y1,2) < puissance(robot.x-x2,2)+puissance(robot.y-y2,2)){
				currentRobot[5].x=x1;
				currentRobot[5].y=y1;
				currentRobot[5].teta=moyAngle12;
				currentRobot[5].weight=10;
			}else{
				currentRobot[5].x=x2;
				currentRobot[5].y=y2;
				currentRobot[5].teta=moyAngle34;
				currentRobot[5].weight=10;
			}
			debug_printf("\n currentRobot[5]: x1=%ld y1=%ld angle1=%ld angle2=%ld", x1, y1, a1, a2);
			debug_printf("\n currentRobot[5]: x2=%ld y2=%ld angle3=%ld angle4=%ld", x2, y2, a3, a4);
			debug_printf("\n currentRobot[5]: x=%ld y=%ld angle=%ld \n ", currentRobot[5].x, currentRobot[5].y, currentRobot[5].teta);
		}

		//debug_printf("FIN\n");
	}

void findCorrectPosition(){
	Sint16 offset_x, offset_y, offset_pos_x, offset_pos_y;

	//Gestion des erreurs: un point trop éloigné ne doit pas être pris en compte lorsqu'on a pluseurs mesures disponibles (2 ou 3 balises)
	if(B1detected==1 && B2detected==1){
		if(sqrt((currentRobot[0].x-currentRobot[1].x)*(currentRobot[0].x-currentRobot[1].x)+(currentRobot[0].y-currentRobot[1].y)*(currentRobot[0].y-currentRobot[1].y))>ECART_POSITION
			&& sqrt((currentRobot[0].x-currentRobot[3].x)*(currentRobot[0].x-currentRobot[3].x)+(currentRobot[0].y-currentRobot[3].y)*(currentRobot[0].y-currentRobot[3].y))>ECART_POSITION ){
			currentRobot[0].weight=1;
		}
		if(sqrt((currentRobot[0].x-currentRobot[1].x)*(currentRobot[0].x-currentRobot[1].x)+(currentRobot[0].y-currentRobot[1].y)*(currentRobot[0].y-currentRobot[1].y))>ECART_POSITION
			&& sqrt((currentRobot[1].x-currentRobot[3].x)*(currentRobot[1].x-currentRobot[3].x)+(currentRobot[1].y-currentRobot[3].y)*(currentRobot[1].y-currentRobot[3].y))>ECART_POSITION ){
			currentRobot[1].weight=1;
		}
		if(sqrt((currentRobot[3].x-currentRobot[1].x)*(currentRobot[3].x-currentRobot[1].x)+(currentRobot[3].y-currentRobot[1].y)*(currentRobot[3].y-currentRobot[1].y))>ECART_POSITION
			&& sqrt((currentRobot[0].x-currentRobot[3].x)*(currentRobot[0].x-currentRobot[3].x)+(currentRobot[0].y-currentRobot[3].y)*(currentRobot[0].y-currentRobot[3].y))>ECART_POSITION ){
			currentRobot[3].weight=1;
		}
	}
	if(B1detected==1 && B3detected==1){
		if(sqrt((currentRobot[0].x-currentRobot[2].x)*(currentRobot[0].x-currentRobot[2].x)+(currentRobot[0].y-currentRobot[2].y)*(currentRobot[0].y-currentRobot[2].y))>ECART_POSITION
			&& sqrt((currentRobot[0].x-currentRobot[4].x)*(currentRobot[0].x-currentRobot[4].x)+(currentRobot[0].y-currentRobot[4].y)*(currentRobot[0].y-currentRobot[4].y))>ECART_POSITION ){
			currentRobot[0].weight=1;
		}
		if(sqrt((currentRobot[2].x-currentRobot[0].x)*(currentRobot[2].x-currentRobot[0].x)+(currentRobot[2].y-currentRobot[0].y)*(currentRobot[2].y-currentRobot[0].y))>ECART_POSITION
			&& sqrt((currentRobot[2].x-currentRobot[4].x)*(currentRobot[2].x-currentRobot[4].x)+(currentRobot[2].y-currentRobot[4].y)*(currentRobot[2].y-currentRobot[4].y))>ECART_POSITION ){
			currentRobot[2].weight=1;
		}
		if(sqrt((currentRobot[4].x-currentRobot[0].x)*(currentRobot[4].x-currentRobot[0].x)+(currentRobot[4].y-currentRobot[0].y)*(currentRobot[4].y-currentRobot[0].y))>ECART_POSITION
			&& sqrt((currentRobot[4].x-currentRobot[2].x)*(currentRobot[4].x-currentRobot[2].x)+(currentRobot[4].y-currentRobot[2].y)*(currentRobot[4].y-currentRobot[2].y))>ECART_POSITION ){
			currentRobot[4].weight=1;
		}
	}
	if(B2detected==1 && B3detected==1){
		if(sqrt((currentRobot[1].x-currentRobot[2].x)*(currentRobot[1].x-currentRobot[2].x)+(currentRobot[1].y-currentRobot[2].y)*(currentRobot[1].y-currentRobot[2].y))>ECART_POSITION
			&& sqrt((currentRobot[1].x-currentRobot[5].x)*(currentRobot[1].x-currentRobot[5].x)+(currentRobot[1].y-currentRobot[5].y)*(currentRobot[1].y-currentRobot[5].y))>ECART_POSITION ){
			currentRobot[1].weight=1;
		}
		if(sqrt((currentRobot[2].x-currentRobot[1].x)*(currentRobot[2].x-currentRobot[1].x)+(currentRobot[2].y-currentRobot[1].y)*(currentRobot[2].y-currentRobot[1].y))>ECART_POSITION
			&& sqrt((currentRobot[2].x-currentRobot[5].x)*(currentRobot[2].x-currentRobot[5].x)+(currentRobot[2].y-currentRobot[5].y)*(currentRobot[2].y-currentRobot[5].y))>ECART_POSITION ){
			currentRobot[2].weight=1;
		}
		if(sqrt((currentRobot[5].x-currentRobot[1].x)*(currentRobot[5].x-currentRobot[1].x)+(currentRobot[5].y-currentRobot[1].y)*(currentRobot[5].y-currentRobot[1].y))>ECART_POSITION
			&& sqrt((currentRobot[5].x-currentRobot[2].x)*(currentRobot[5].x-currentRobot[2].x)+(currentRobot[5].y-currentRobot[2].y)*(currentRobot[5].y-currentRobot[2].y))>ECART_POSITION ){
			currentRobot[5].weight=1;
		}
	}
	if(B1detected==1 && B2detected==1 && B3detected==1){
			if(sqrt((currentRobot[0].x-currentRobot[1].x)*(currentRobot[0].x-currentRobot[1].x)+(currentRobot[0].y-currentRobot[1].y)*(currentRobot[0].y-currentRobot[1].y))>ECART_POSITION
				&& sqrt((currentRobot[0].x-currentRobot[2].x)*(currentRobot[0].x-currentRobot[2].x)+(currentRobot[0].y-currentRobot[2].y)*(currentRobot[0].y-currentRobot[2].y))>ECART_POSITION ){
				currentRobot[0].weight=1; //la mesure avec la balise B1 a raté
				currentRobot[3].weight=1;
				currentRobot[4].weight=1;
			}
			if(sqrt((currentRobot[1].x-currentRobot[0].x)*(currentRobot[1].x-currentRobot[0].x)+(currentRobot[1].y-currentRobot[0].y)*(currentRobot[1].y-currentRobot[0].y))>ECART_POSITION
				&& sqrt((currentRobot[1].x-currentRobot[2].x)*(currentRobot[1].x-currentRobot[2].x)+(currentRobot[1].y-currentRobot[2].y)*(currentRobot[1].y-currentRobot[2].y))>ECART_POSITION ){
				currentRobot[1].weight=1; //la mesure avec la balise B2 a raté
				currentRobot[3].weight=1;
				currentRobot[5].weight=1;
			}
			if(sqrt((currentRobot[2].x-currentRobot[0].x)*(currentRobot[2].x-currentRobot[0].x)+(currentRobot[2].y-currentRobot[0].y)*(currentRobot[2].y-currentRobot[0].y))>ECART_POSITION
				&& sqrt((currentRobot[2].x-currentRobot[1].x)*(currentRobot[2].x-currentRobot[1].x)+(currentRobot[2].y-currentRobot[1].y)*(currentRobot[2].y-currentRobot[1].y))>ECART_POSITION ){
				currentRobot[2].weight=1; //la mesure avec la balise B3 a raté
				currentRobot[4].weight=1;
				currentRobot[5].weight=1;
			}
	}

	//Le poids des points mesurées dans les zones sensibles à chaque balises sont diminués
	if(B1detected==1 && (currentRobot[0].x<ECART_FIABILITE || currentRobot[0].y<ECART_FIABILITE )){
		currentRobot[0].weight=2;
	}
	if(B2detected==1 && (currentRobot[0].x>2000-ECART_FIABILITE || currentRobot[0].y<ECART_FIABILITE )){
		currentRobot[1].weight=2;
	}
	if(B3detected==1 && ((currentRobot[0].x>1000-ECART_FIABILITE && currentRobot[0].x<1000+ECART_FIABILITE) || currentRobot[0].y>3000-ECART_FIABILITE )){
		currentRobot[2].weight=2;
	}

	if(B1detected==1 && B2detected==0 && B3detected==0){
		robot.teta=currentRobot[0].teta;
	}else if(B1detected==0 && B2detected==1 && B3detected==0){
		robot.teta=currentRobot[1].teta;
	}else if(B1detected==0 && B2detected==0 && B3detected==1){
		robot.teta=currentRobot[2].teta;
	}else{
		/*debug_printf("Affectation finale angle\n");
		debug_printf("currentRobot[3].teta*currentRobot[3].weight=%d\n",currentRobot[3].teta*currentRobot[3].weight);
		debug_printf("currentRobot[4].teta*currentRobot[4].weight=%d\n",currentRobot[4].teta*currentRobot[4].weight);
		debug_printf("currentRobot[5].teta*currentRobot[5].weight=%d\n",currentRobot[5].teta*currentRobot[5].weight);
		debug_printf("robot.teta=%d\n",(Sint16)(((Sint32)(currentRobot[3].teta*currentRobot[3].weight+currentRobot[4].teta*currentRobot[4].weight+currentRobot[5].teta*currentRobot[5].weight)/(currentRobot[3].weight+currentRobot[4].weight+currentRobot[5].weight))));
		debug_printf("poids=%d\n",(currentRobot[3].weight+currentRobot[4].weight+currentRobot[5].weight));
		debug_printf("somme=%ld\n",(Sint32)(currentRobot[3].teta*currentRobot[3].weight+currentRobot[4].teta*currentRobot[4].weight+currentRobot[5].teta*currentRobot[5].weight));
		debug_printf("condition du A: %d %d %d = %d\n",(Sint16)currentRobot[3].teta>(Sint16)10000,currentRobot[4].teta>10000, currentRobot[5].teta<-10000,((currentRobot[3].teta>10000) && (currentRobot[4].teta>10000) && (currentRobot[5].teta<-10000)));
		debug_printf("currentRobot[3].teta=%d\n", currentRobot[3].teta);
		debug_printf("currentRobot[4].teta=%d\n", currentRobot[4].teta);
		debug_printf("currentRobot[5].teta=%d\n", currentRobot[5].teta);*/
		if(((Sint16)currentRobot[3].teta>(Sint16)10000) && ((Sint16)currentRobot[4].teta>(Sint16)10000) && ((Sint16)currentRobot[5].teta<(Sint16)-10000)){
			currentRobot[5].teta+=2*PI4096;
			debug_printf("A\n");
		}else if(((Sint16)currentRobot[3].teta) > (Sint16)10000 && ((Sint16)currentRobot[4].teta < (Sint16)-10000) && ((Sint16)currentRobot[5].teta >(Sint16)10000)){
			currentRobot[4].teta+=2*PI4096;
			debug_printf("B\n");
		}else if(((Sint16)currentRobot[3].teta) < (Sint16)-10000 && ((Sint16)currentRobot[4].teta > (Sint16)10000) && ((Sint16)currentRobot[5].teta > (Sint16)10000)){
			currentRobot[3].teta+=2*PI4096;
			debug_printf("C\n");
		}else if(((Sint16)currentRobot[3].teta) > (Sint16)10000 && ((Sint16)currentRobot[4].teta < (Sint16)-10000) && ((Sint16)currentRobot[5].teta < (Sint16)-10000)){
			currentRobot[3].teta-=2*PI4096;
			debug_printf("D\n");
		}else if(((Sint16)currentRobot[3].teta) < (Sint16)-10000 && ((Sint16)currentRobot[4].teta > (Sint16)10000) && ((Sint16)currentRobot[5].teta < (Sint16)-10000)){
			currentRobot[4].teta-=2*PI4096;
			debug_printf("E\n");
		}else if(((Sint16)currentRobot[3].teta) < (Sint16)-10000 && ((Sint16)currentRobot[4].teta < (Sint16)-10000) && ((Sint16)currentRobot[5].teta > (Sint16)10000)){
			currentRobot[5].teta-=2*PI4096;
			debug_printf("F\n");
		}

		robot.teta= (Sint16)((Sint32)(currentRobot[3].teta*currentRobot[3].weight+currentRobot[4].teta*currentRobot[4].weight+currentRobot[5].teta*currentRobot[5].weight)/((Sint32)(currentRobot[3].weight+currentRobot[4].weight+currentRobot[5].weight)));
	}

	robot.x=(currentRobot[0].x*currentRobot[0].weight+currentRobot[1].x*currentRobot[1].weight+currentRobot[2].x*currentRobot[2].weight+currentRobot[3].x*currentRobot[3].weight+currentRobot[4].x*currentRobot[4].weight+currentRobot[5].x*currentRobot[5].weight)/(1.0*(currentRobot[0].weight+currentRobot[1].weight+currentRobot[2].weight+currentRobot[3].weight+currentRobot[4].weight+currentRobot[5].weight));
	robot.y=(currentRobot[0].y*currentRobot[0].weight+currentRobot[1].y*currentRobot[1].weight+currentRobot[2].y*currentRobot[2].weight+currentRobot[3].y*currentRobot[3].weight+currentRobot[4].y*currentRobot[4].weight+currentRobot[5].y*currentRobot[5].weight)/(1.0*(currentRobot[0].weight+currentRobot[1].weight+currentRobot[2].weight+currentRobot[3].weight+currentRobot[4].weight+currentRobot[5].weight));

	//Correction du décalage de l'hokuyo
	//debug_printf("Avant correction décalage Hokuyo Robot  x=%ld  y=%ld  teta=%ld \n\n",robot.x, robot.y, robot.teta);

	if(QS_WHO_AM_I_get() == BIG_ROBOT){
		offset_pos_x = HOKUYO_OFFSET_BIG_POS_X;
		offset_pos_y = HOKUYO_OFFSET_BIG_POS_Y;
	}else{
		offset_pos_x = HOKUYO_OFFSET_SMALL_POS_X;
		offset_pos_y = HOKUYO_OFFSET_SMALL_POS_Y;
	}

	offset_x = (Sint16)(offset_pos_x*cos4096(robot.teta));
	offset_y = (Sint16)(offset_pos_y*sin4096(robot.teta));
	robot.x -= offset_x;
	robot.y -= offset_y;
	//debug_printf("POSITION ROBOT APRES TRIANGULATION: x=%ld   y=%ld teta=%d\n", robot.x, robot.y, robot.teta);
}


void tri_points(){
	Uint16 i;
	color_e color= ODOMETRY_get_color();

	if(color==BOT_COLOR){
		for(i=0;i<nb_valid_points_beacons;i++){
			if(points_beacons_valid[i].coordX>-62-ECART_BALISE && points_beacons_valid[i].coordX<-62+ECART_BALISE
			   && points_beacons_valid[i].coordY>-62-ECART_BALISE && points_beacons_valid[i].coordY<-62+ECART_BALISE){
				points_beacons[0][nb_points_B1] = points_beacons_valid[i];
			}else if(points_beacons_valid[i].coordX>1000-ECART_BALISE && points_beacons_valid[i].coordX<1000+ECART_BALISE
					 && points_beacons_valid[i].coordY>3062-ECART_BALISE && points_beacons_valid[i].coordY<3062+ECART_BALISE){
				points_beacons[1][nb_points_B2] = points_beacons_valid[i];
				nb_points_B2++;
			}else if(points_beacons_valid[i].coordX>2062-ECART_BALISE && points_beacons_valid[i].coordX<2062+ECART_BALISE
					 && points_beacons_valid[i].coordY>-62-ECART_BALISE && points_beacons_valid[i].coordY<-62+ECART_BALISE){
				points_beacons[2][nb_points_B3] = points_beacons_valid[i];
				nb_points_B3++;
			}
		}
	}else{
		for(i=0;i<nb_valid_points_beacons;i++){
			if(points_beacons_valid[i].coordX>2062-ECART_BALISE && points_beacons_valid[i].coordX<2062+ECART_BALISE
			   && points_beacons_valid[i].coordY>3062-ECART_BALISE && points_beacons_valid[i].coordY<3062+ECART_BALISE){
				points_beacons[0][nb_points_B1] = points_beacons_valid[i];
			}else if(points_beacons_valid[i].coordX>1000-ECART_BALISE && points_beacons_valid[i].coordX<1000+ECART_BALISE
					 && points_beacons_valid[i].coordY>-62-ECART_BALISE && points_beacons_valid[i].coordY<-62+ECART_BALISE){
				points_beacons[1][nb_points_B2] = points_beacons_valid[i];
				nb_points_B2++;
			}else if(points_beacons_valid[i].coordX>-62-ECART_BALISE && points_beacons_valid[i].coordX<-62+ECART_BALISE
					 && points_beacons_valid[i].coordY>3062-ECART_BALISE && points_beacons_valid[i].coordY<3062+ECART_BALISE){
				points_beacons[2][nb_points_B3] = points_beacons_valid[i];
				nb_points_B3++;
			}
		}

	}

	if(nb_points_B1 > 7) B1detected=1;
	if(nb_points_B2 > 7) B2detected=1;
	if(nb_points_B3 > 7) B3detected=1;
	//debug_printf("Tri points\n");
	//debug_printf("nb_points_B1=%d\n", nb_points_B1);
	//debug_printf("nb_points_B2=%d\n", nb_points_B2);
	//debug_printf("nb_points_B3=%d\n", nb_points_B3);
	/*debug_printf("Points balise 3 avant filtrage \n\nx= ");
		for(i=0;i<nb_points_B3;i++){
			debug_printf("%ld  ",points_beacons[2][i].coordX) ;
		}
		debug_printf("Points balise 3 \n\ny= ");
		for(i=0;i<nb_points_B3;i++){
			debug_printf("%ld  ",points_beacons[2][i].coordY) ;
		}*/

}



void tri_mesures(HOKUYO_point_position points_beacons[3][NB_MESURES_HOKUYO*20], Uint16 *nb_points, Uint8 numero_beacon){
	Uint16 i, n=0;
	debug_printf("TRI MESURES\n");
	//Elimination des points qui ne sont pas les points de la balise
	for(i=0;i<*nb_points;i++){
		if(points_beacons[numero_beacon-1][i].power_intensity>SEUIL_INTENSITE && points_beacons[numero_beacon-1][i].coordX>-500 && points_beacons[numero_beacon-1][i].coordX<2500 && points_beacons[numero_beacon-1][i].coordY>-500 && points_beacons[numero_beacon-1][i].coordY<3500 && i==n){
			//ne rien faire le pion est bien placé
			n++;
		}else if(points_beacons[numero_beacon-1][i].power_intensity>SEUIL_INTENSITE && points_beacons[numero_beacon-1][i].coordX>-500 && points_beacons[numero_beacon-1][i].coordX<2500 && points_beacons[numero_beacon-1][i].coordY>-500 && points_beacons[numero_beacon-1][i].coordY<3500){
			points_beacons[numero_beacon-1][n]=points_beacons[numero_beacon-1][i];
			n++;
		}
	}
	*nb_points=n;
	/*debug_printf("Points balise 3 après filtrage\n\nx= ");
		for(i=0;i<nb_points_B3;i++){
			debug_printf("%ld  ",points_beacons[2][i].coordX) ;
		}
		debug_printf("Points balise 3 \n\ny= ");
		for(i=0;i<nb_points_B3;i++){
			debug_printf("%ld  ",points_beacons[2][i].coordY) ;
		}*/
}
/*
//Fonction prenant la moyenne des points mesurés pour chaque angle donné (ie tout les 0,25°)
void moyenne_mesures(HOKUYO_point_position points_beacons[3][NB_MESURES_HOKUYO*20], Uint16 nb_points, Uint8 numero_beacon){
	Uint32 nb_points_moyenne=1, nb_points_new=0, i, moyenneX, moyenneY;
	moyenneX=points_beacons[numero_beacon][0].coordX;
	moyenneX=points_beacons[numero_beacon][0].coordY;

	for(i=1 ; i<nb_points ; i++){
		if(fabs(points_beacons[numero_beacon][i].teta-points_beacons[numero_beacon][i-1].teta)<0.01){ //on teste l'égalité des deux angles
			moyenneX =  (moyenneX*nb_points_moyenne + points_beacons[numero_beacon][i].coordX)/((nb_points_moyenne+1)*1.);
			moyenneY =  (moyenneY*nb_points_moyenne + points_beacons[numero_beacon][i].coordY)/((nb_points_moyenne+1)*1.);
			nb_points_moyenne++;
		}else{
			points_beacons[numero_beacon][nb_points_new].coordX = moyenneX;  //on stocke la moyenne effectuée pour un angle donné
			points_beacons[numero_beacon][nb_points_new].coordY = moyenneY;
			nb_points_new++;

			moyenneX=points_beacons[numero_beacon][i].coordX; //on initialise les variables pour le calcul de l'angle suivant
			moyenneY=points_beacons[numero_beacon][i].coordY;
			nb_points_moyenne=1;
		}
	}

	//on stocke le dernier point
	points_beacons[numero_beacon][nb_points_new].coordX = moyenneX;  //on stocke la moyenne effectuée pour un angle donné
	points_beacons[numero_beacon][nb_points_new].coordY = moyenneY;
	nb_points = nb_points_new+1;

	debug_printf("\nMoyenne Points \n\nx= ");
	for(i=0;i<nb_points;i++){
		debug_printf("%ld  ",points_beacons[numero_beacon-1][i].coordX) ;
	}
	debug_printf("\nMoyenne Points \n\ny= ");
	for(i=0;i<nb_points;i++){
		debug_printf("%ld  ",points_beacons[numero_beacon-1][i].coordY) ;
	}
}*/

//Fonction réalisant une régression circulaire d'un nuage de points
HOKUYO_adversary_position regression_circulaire(HOKUYO_point_position points_beacons[3][NB_MESURES_HOKUYO*20], Uint16 nb_points, Uint8 numero_beacon){
	HOKUYO_adversary_position beacon;

	//METHODE 3
	Sint64 sumX=0, sumY=0, sumX2=0, sumY2=0, sumX3=0, sumY3=0, sumXY=0, sumX2Y=0, sumXY2=0; //les sommes de 0 à nb_points-1 des coordonnées. ex: sumX=somme de toutes les abscisses
	Sint64 c11=0, c20=0, c30=0, c21=0, c02=0, c03=0, c12=0; //des coefficients intermédiaires de calculs
	Uint16 i;

	//Calcul des sommes
	//debug_printf("nb_points= %d \n", nb_points);
	//debug_printf("puissance=%lld \n", puissance(3,2));
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
	//debug_printf("c11=%lld \n", c11);
	c20=nb_points*sumX2-sumX*sumX;
	//debug_printf("c20=%lld \n", c20);
	c30=nb_points*sumX3-sumX2*sumX;
	//debug_printf("c30=%lld\n", c30);
	c21=nb_points*sumX2Y-sumX2*sumY;
	//debug_printf("c21=%lld \n", c21);
	c02=nb_points*sumY2-sumY*sumY;
	//debug_printf("c02=%lld \n", c02);
	c03=nb_points*sumY3-sumY2*sumY;
	//debug_printf("c03=%lld \n", c03);
	c12=nb_points*sumXY2-sumX*sumY2;
	//debug_printf("c12=%lld \n", c12);


	//calcul du centre de la balise
	beacon.coordX = ((c30+c12)*c02-(c03+c21)*c11)/(2.*(c20*c02-c11*c11));
	beacon.coordY = ((c03+c21)*c20-(c30+c12)*c11)/(2.*(c20*c02-c11*c11));

	/*//########################TEST TRIANGULATION#########################
	switch(numero_beacon){
		case 1:
			beacon.coordX=-62;
			beacon.coordY=-62;
			break;
		case 2:
			beacon.coordX=1000;
			beacon.coordY=3062;
			break;
		case 3:
			beacon.coordX=2062;
			beacon.coordY=-62;
			break;
	}
*/



	beacon.dist   = sqrt(puissance(robot.x-beacon.coordX,2) + puissance(robot.y-beacon.coordY,2));
	debug_printf("\n\nRobot x=%ld y=%ld teta=%ld\n", robot.x, robot.y, robot.teta);
	debug_printf("\n\nBeacon x=%ld y=%ld dist=%ld\n", beacon.coordX, beacon.coordY, beacon.dist);
	switch(numero_beacon){
		case 1:
			beacon.teta = (Sint16)((Sint16)atan4096((robot.y-beacon.coordY)/(1.0*(robot.x-beacon.coordX)))- robot.teta +PI4096/2);
			//display(atan4096((robot.y-beacon.coordY)/(1.0*(robot.x-beacon.coordX))));
			//debug_printf("ANGLE=%d\n", beacon.teta);
			break;
		case 2:
			beacon.teta = (Sint16)( atan2_4096(beacon.coordY-robot.y,beacon.coordX-robot.x)) - robot.teta -PI4096/2;
			//debug_printf("ATAN2=%d\n",(Sint16)( atan2_4096(beacon.coordY-robot.y,beacon.coordX-robot.x)));
			break;
		case 3:
			beacon.teta = (Sint16)(PI4096 - robot.teta + atan4096((beacon.coordX-robot.x)/(1.*(robot.y-beacon.coordY))));
			//debug_printf("Affectation angle atan=%d\n", atan4096((beacon.coordX-robot.x)/(1.*(robot.y-beacon.coordY))));
			//debug_printf("Affectation beacon.teta=%d\n", (Sint16)(PI4096 - robot.teta + atan4096((beacon.coordX-robot.x)/(1.*(robot.y-beacon.coordY)))));
			break;
	}


	if(beacon.teta < 0)  beacon.teta += 2*PI4096;
	if(beacon.teta > 2*PI4096) beacon.teta -= 2*PI4096;
	if(beacon.teta < 0)  beacon.teta += 2*PI4096;
	if(beacon.teta > 2*PI4096)  beacon.teta -= 2*PI4096;
	if(beacon.teta < 0)  beacon.teta += 2*PI4096;
	if(beacon.teta > 2*PI4096)  beacon.teta -= 2*PI4096;

	debug_printf("centre balise x=%ld y=%ld dist=%ld teta=%d",beacon.coordX, beacon.coordY, beacon.dist, beacon.teta);

	return beacon;
}

//Fonction trouvant le centre des balises
void find_beacons_centres(){
	robot.teta = global.position.teta;
	robot.x = global.position.x - ECART_HOKUYO_A_DROITE*sin4096(robot.teta);
	robot.y = global.position.y + ECART_HOKUYO_A_DROITE*cos4096(robot.teta);
	/*//TEST TRIANGULATION
	robot.x = 700;
	robot.y = 1500;
	robot.teta= -PI4096;
	B1detected=1;
	B2detected=1;
	B3detected=1;
	//FIN TEST*/
	robot.weight = 1;
	//Uint16 i;
	color_e color= ODOMETRY_get_color();


	if(B1detected==1){
		tri_mesures(points_beacons, &nb_points_B1, 1);
		debug_printf("\nCentre balise 1: ");
		beacon1 = regression_circulaire(points_beacons, nb_points_B1,1);
	}

	if(B2detected==1){
		tri_mesures(points_beacons, &nb_points_B2, 2);
		debug_printf("\nCentre balise 2: ");
		beacon2 = regression_circulaire(points_beacons, nb_points_B2,2);
	}

	if(B3detected==1){
		tri_mesures(points_beacons, &nb_points_B3, 3);
		//debug_printf("Tri mesures\n");
		debug_printf("nb_points_B3=%d\n", nb_points_B3);
		debug_printf("\nCentre balise 3: ");
		beacon3 = regression_circulaire(points_beacons, nb_points_B3,3);
	}
	//------------------Modification des paramètres pour le coté vert---------------------------
	if(color==TOP_COLOR){
		//position du robot
		robot.x=2000-robot.x;
		robot.y=3000-robot.y;
		robot.teta+=PI4096;
		if(robot.teta>PI4096)  robot.teta-=2*PI4096;
		if(robot.teta<-PI4096)  robot.teta+=2*PI4096;

		//positions des balises
		beacon1.coordX=2000-beacon1.coordX;
		beacon1.coordY=3000-beacon1.coordY;
		beacon2.coordX=2000-beacon2.coordX;
		beacon2.coordY=3000-beacon2.coordY;
		beacon3.coordX=2000-beacon3.coordX;
		beacon3.coordY=3000-beacon3.coordY;

	}
	/*
	debug_printf("\n\nPoints balise 1 \n\nx= ");
	for(i=0;i<nb_points_B1;i++){
		debug_printf("%ld  ",points_beacons[0][i].coordX) ;
	}
	debug_printf("\n\nPoints balise 1 \n\ny= ");
	for(i=0;i<nb_points_B1;i++){
		debug_printf("%ld  ", points_beacons[0][i].coordY) ;
	}*/
	/*debug_printf("\n\nPoints balise 2 \n\nx= ");
	for(i=0;i<nb_points_B2;i++){
		debug_printf("%ld  ", points_beacons[1][i].coordX) ;
	}
	debug_printf("\n\nPoints balise 2 \n\ny= ");
	for(i=0;i<nb_points_B2;i++){
		debug_printf("%ld  ", points_beacons[1][i].coordY) ;
	}*/
	/*debug_printf("Points balise 3 \n\nx= ");
	for(i=0;i<nb_points_B3;i++){
		debug_printf("%ld  ",points_beacons[2][i].coordX) ;
	}
	debug_printf("Points balise 3 \n\ny= ");
	for(i=0;i<nb_points_B3;i++){
		debug_printf("%ld  ",points_beacons[2][i].coordY) ;
	}*/

	triangulation();
	findCorrectPosition();
	//------------------Modification des paramètres pour le coté vert---------------------------
	if(color==TOP_COLOR){
		//position du robot
		robot.x=2000-robot.x;
		robot.y=3000-robot.y;
		robot.teta+=PI4096;
		if(robot.teta>PI4096)  robot.teta-=2*PI4096;
		if(robot.teta<-PI4096)  robot.teta+=2*PI4096;
	}

	debug_printf("FIN TRIANGULATION\n");


	nb_points_B1=0;
	nb_points_B2=0;
	nb_points_B3=0;
	B1detected=0;
	B2detected=0;
	B3detected=0;
}


#endif
