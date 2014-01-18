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

#ifdef USE_HOKUYO

	#define HOKUYO_ANGLE_OFFSET 4500 //45 degrés
	#define HOKUYO_ANGLE_ROBOT_TERRAIN 0
	#define HOKUYO_DETECTION_MARGE 130
	#define HOKUYO_EVITEMENT_MIN 150
	#define HOKUYO_BORDS_TERRAIN 80
	#define ROBOT_COORDX 150
	#define ROBOT_COORDY 150
	#define ECARTEMENT_PTS_MAX_CARRE 22500
	#define NB_MAX_ADVERSARIES	16
	#define NB_BYTES_FROM_HOKUYO	5000

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

	static HOKUYO_adversary_position detected_valid_points[1080];	//Points valides détectés par le capteur (X, Y, teta, distance)
	static Uint16 nb_valid_points=0;								//Nombre de points valides détectés

	static HOKUYO_adversary_position hokuyo_adversaries[NB_MAX_ADVERSARIES];	//Positions des adversaires détectés
	static Uint8 adversaries_number=0;											//Nombre d'adversaires détectés

	static bool_e hokuyo_initialized = FALSE;						//Module initialisé - sécurité.
	volatile bool_e flag_device_disconnected = FALSE;				//Flag levé en callback lorsque le capteur vient d'être débranché


	void hokuyo_write_command(Uint8 tab[]);
	int hokuyo_write_uart_manually(void);
	void hokuyo_read_buffer(void);
	void hokuyo_format_data(void);
	void hokuyo_find_valid_points(void);
	Sint32 hokuyo_dist_min(Uint16 compt);

	//Deux fonctions pour detecter des regroupements de points
	void hokuyo_detection_ennemis(void);
	//void ReconObjet(void);
	void DetectRobots(void);





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
			//ReconObjet(nb_pts_terrain);
			state=SEND_ADVERSARIES_DATAS;
		break;
		case SEND_ADVERSARIES_DATAS:
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
	int j=47;
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
}

//Fonction qui renvoi les coordonnées des points detectés sur le terrain
void hokuyo_find_valid_points(void)
{
		Uint16 nb_val_bizarres=0;
		Uint16 a,b;
		Uint16 comp=0;
		Sint32 dist[1500];
		Uint16 nb_pts;
		Sint32 angle=0;

		Sint32 anglerad;
		Sint32 angleoffsetrad;
		Sint32 anglerad_decale;
		Sint32 angleRobotTerrainRad;
		Sint32 angleFinal;

		Sint16 coco;
		Sint16 sisi;
		Sint32 coordX;
		Sint32 coordY;

		Sint32 coordCibleX;
		Sint32 coordCibleY;

		Sint16 cosoffsetTerrain;
		Sint16 sinoffsetTerrain;

		nb_valid_points=0;

		for(nb_pts = 0; nb_pts<=1080; nb_pts++)
		{
			a = HOKUYO_datas[comp];
			b = HOKUYO_datas[comp+1];

			//calcul distance
			dist[nb_pts]=((a-0x30)<<6)+((b-0x30)&0x3f);	//cf datasheet de l'hokuyo... pour comprendre comment les données sont codées.

			//Sint32(a) * PI4096 /18000
			anglerad = (((Sint32)(angle))*183)>>8;
			angleoffsetrad = (((Sint32)(HOKUYO_ANGLE_OFFSET))*183)>>8;

			angleRobotTerrainRad = (((Sint32)(HOKUYO_ANGLE_ROBOT_TERRAIN))*183)>>8;

			anglerad_decale = anglerad-angleoffsetrad;
			angleFinal = anglerad_decale + angleRobotTerrainRad;

			COS_SIN_4096_get(anglerad,&coco,&sisi);
			COS_SIN_4096_get(angleFinal,&cosoffsetTerrain,&sinoffsetTerrain);

			coordX=(dist[nb_pts]*cosoffsetTerrain)/4096;
			coordY=(dist[nb_pts]*sinoffsetTerrain)/4096;

			coordCibleX=coordX+ROBOT_COORDX;
			coordCibleY=coordY+ROBOT_COORDY;

			//debug_printf("distance = [%ld]",data[lala]);
			//debug_printf(" angle degre = [%d]",angle);
			//debug_printf(" angle radian = [%ld]",anglerad);
			//debug_printf(" angle radian = [%ld]",angleFinal);
			//debug_printf(" CX = [%ld]",coordX);
			//debug_printf(" CY = [%ld]\n",coordY);

			if(dist[nb_pts] == 1)
				nb_val_bizarres++;

			if(		coordCibleX <= 1000-HOKUYO_BORDS_TERRAIN &&
					coordCibleX>=0+HOKUYO_BORDS_TERRAIN &&
					coordCibleY <= 2000-HOKUYO_BORDS_TERRAIN &&
					coordCibleY >=0+HOKUYO_BORDS_TERRAIN &&
					dist[nb_pts]>100)
			{
								//debug_printf("d = [%ld mm]",dist[nb_pts]);
				detected_valid_points[nb_valid_points].dist=dist[nb_pts];

				//debug_printf(" teta = [%d] \n",angle-HOKUYO_ANGLE_OFFSET);
				detected_valid_points[nb_valid_points].teta=(angle-HOKUYO_ANGLE_OFFSET);

				//debug_printf(" X = [%ld mm]",coordCibleX);
				detected_valid_points[nb_valid_points].coordX=coordCibleX;

				//debug_printf(" Y = [%ld mm]\n",coordCibleY);
				detected_valid_points[nb_valid_points].coordY=coordCibleY;

				nb_valid_points++;
			}
			angle+=25;
			comp+=2;
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
	debug_printf("il y a  %d mechant(s)\n",adversaries_number);

	for(i=1;i<=adversaries_number;i++)
	{
		debug_printf(" ennemi numero %d x=[%ld mm]",i,hokuyo_adversaries[i].coordX);
		debug_printf(" et y=[%ld mm]\n",hokuyo_adversaries[i].coordY);
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
	Sint32 deltaXcarre, deltaYcarre;
	Sint32 Distcarre;
	Sint32 x_comp, y_comp, sumX, sumY;

	adversaries_number = 0;

	if(nb_valid_points < 1)
		return;

	x_comp=detected_valid_points[0].coordX;
	y_comp=detected_valid_points[0].coordY;
	sumX=detected_valid_points[0].coordX;
	sumY=detected_valid_points[0].coordY;

	nb_pts=1;

	for(i=1;i<nb_valid_points;i++)
	{
		deltaXcarre=(Sint32)(detected_valid_points[i].coordX-x_comp)*(detected_valid_points[i].coordX-x_comp);
		deltaYcarre=(Sint32)(detected_valid_points[i].coordY-y_comp)*(detected_valid_points[i].coordY-y_comp);
		Distcarre=deltaXcarre+deltaYcarre;

		if(Distcarre<=ECARTEMENT_PTS_MAX_CARRE && i<nb_valid_points-1)
		{
			sumX+=detected_valid_points[i].coordX;
			sumY+=detected_valid_points[i].coordY;
			nb_pts++;
		}else if(i<nb_valid_points-1){

			hokuyo_adversaries[adversaries_number].coordX=sumX/nb_pts;
			hokuyo_adversaries[adversaries_number].coordY=sumY/nb_pts;
			if(adversaries_number < NB_MAX_ADVERSARIES - 1)
				adversaries_number++;

			nb_pts=1;

			x_comp=detected_valid_points[i].coordX;
			y_comp=detected_valid_points[i].coordY;

			sumX=detected_valid_points[i].coordX;
			sumY=detected_valid_points[i].coordY;
		}else{                                            //cas du tout dernier point qui est en effet récalcitrant.
			if(Distcarre<=ECARTEMENT_PTS_MAX_CARRE){
				sumX+=detected_valid_points[i].coordX;
				sumY+=detected_valid_points[i].coordY;
				nb_pts++;
				hokuyo_adversaries[adversaries_number].coordX=sumX/nb_pts;
				hokuyo_adversaries[adversaries_number].coordY=sumY/nb_pts;
			}else{
				hokuyo_adversaries[adversaries_number].coordX=sumX/nb_pts;
				hokuyo_adversaries[adversaries_number].coordY=sumY/nb_pts;
				if(adversaries_number < NB_MAX_ADVERSARIES - 1)
					adversaries_number++;
				hokuyo_adversaries[adversaries_number].coordX=detected_valid_points[i].coordX;
				hokuyo_adversaries[adversaries_number].coordY=detected_valid_points[i].coordY;
			}
		}
	}

	debug_printf("il y a  %d intru(s)\n",adversaries_number);

	for(i=0;i<=adversaries_number;i++)
		{
			debug_printf(" ennemi numero %d x=[%ld mm]",i,hokuyo_adversaries[i].coordX);
			debug_printf(" et y=[%ld mm]\n",hokuyo_adversaries[i].coordY);
		}

}
#endif	//def USE_HOKUYO

void user_callback_DeviceDisconnected(void)
{
#ifdef USE_HOKUYO
	flag_device_disconnected = TRUE;
#endif
}



