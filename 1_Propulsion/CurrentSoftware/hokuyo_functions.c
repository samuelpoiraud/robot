/*
 * hokuyo_functions.c
 *
 *  Created on: 8 d�c. 2013
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
	#include "usb_host/Class/MSC/fat_fs/ff.h"
	#include "term_io.h"
	#include "ff_test_term.h"
	#include "uart_via_usb.h"
	#include "cos_sin.h"
	#include "hokuyo_vars.h"



void hokuyo_init(void);



//Process main
void HOKUYO_process_main(void){
	USBH_Process(&USB_OTG_Core, &USB_Host);
	HOKUYO_kikujiro();
}


//Machine a �tat de l'hokuyo
void HOKUYO_kikujiro(void)
{
	typedef enum
	{
		INIT=0,
		HOKUYO_WAIT,
		ASK_NEW_MEASUREMENT,
		BUFFER_READ,
		REMOVE_LF,
		TREATMENT_DATA,
		DETECTION_ENNEMIS,
		DONE
	}state_e;
	static state_e state = INIT;
	static char tab[5000];
	static Uint16 nb_pts_terrain=0;
	static Uint32 index=0;
	static int nb_ennemis=1;


	switch(state){
		case INIT:
			hokuyo_init();
			state=HOKUYO_WAIT;
		break;
		case HOKUYO_WAIT:
			if(USBH_CDC_is_ready_to_run())
				state=ASK_NEW_MEASUREMENT;

		break;
		case ASK_NEW_MEASUREMENT:
			hokuyo_write_command((Uint8*)"MS0000108001001");
			index=0;
			nb_pts_terrain=0;
			nb_ennemis=1;
			state=BUFFER_READ;
		break;

		case BUFFER_READ:
			hokuyo_read_buffer(tab,&index);
			if(tab[index-2]==0x0A && tab[index-1]==0x0A && index>=2274) state=REMOVE_LF;
			else if(index>2278) state=DONE;
		break;
		case REMOVE_LF:
			hokuyo_format_data(tab);
			state=TREATMENT_DATA;
		break;
		case TREATMENT_DATA:
			hokuyo_traitement_data(tab,&nb_pts_terrain);
			state=DETECTION_ENNEMIS;
		break;
		case DETECTION_ENNEMIS:

			hokuyo_dist_min(nb_pts_terrain);
			//hokuyo_detection_ennemis(nb_pts_terrain,&nb_ennemis);
			DetectRobots(nb_pts_terrain,&nb_ennemis);
			//ReconObjet(nb_pts_terrain);
			state=ASK_NEW_MEASUREMENT;
		break;

		case DONE:
			debug_printf("5");
		break;
		default:
		break;
	}

}



//Fonction d'initialisation du protocole USB
void hokuyo_init(void){

	debug_printf("\nInit usb\n");
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
void hokuyo_read_buffer(char *tab,Uint32 *i)
{
	while(!UART_USB_isRxEmpty())
	{
				tab[*i] = UART_USB_read();
				//debug_printf("%c",tab[*i]);
				*i+=1;
				//UART1_putc(tab[*i]);
	}
}

//Fonction qui formate les donn�es afin d'etre trait�es.
void hokuyo_format_data(char *tab)
{
	int j=47;
	int i=0;
	while(j<=2274)
	{
		if(tab[j+1]=='\n') j+=2;
		tab[i]=tab[j];
		//debug_printf("%c",tab[i]);
		i++;
		j++;
	}
}

//Fonction qui renvoi les coordonn�es des points detect�s sur le terrain
void hokuyo_traitement_data(char * tabvaleurs, Uint16 * compt_sushis){
		Uint16 nb_val_bizarres=0;
		Uint16 a,b;
		Uint16 comp=0;
		Sint32 dist[1500];
		Uint16 nb_pts=0;
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

		//Sint32 coordTerrainX = 160;
		//Sint32 coordTerrainY = 130;

		while(nb_pts<=1080)
		{
				a = tabvaleurs[comp];
				b = tabvaleurs[comp+1];

				//calcul distance
				dist[nb_pts]=((a-0x30)<<6)+((b-0x30)&0x3f);	//cf datasheet de l'hokuyo... pour comprendre comment les donn�es sont cod�es.

				//Sint32(a) * PI4096 /18000
				anglerad = (((Sint32)(angle))*183)>>8;
				angleoffsetrad = (((Sint32)(HOKUYO_OFFSET))*183)>>8;

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

				if(coordCibleX <= 2000-HOKUYO_BORDS_TERRAIN && coordCibleX>=0+HOKUYO_BORDS_TERRAIN && coordCibleY <= 3000-HOKUYO_BORDS_TERRAIN && coordCibleY >=0+HOKUYO_BORDS_TERRAIN && dist[nb_pts]>30){
					//debug_printf("d = [%ld mm]",dist[nb_pts]);
					hokuyo_sushi[*compt_sushis].dist=dist[nb_pts];

					//debug_printf(" teta = [%d] \n",angle-HOKUYO_OFFSET);
					hokuyo_sushi[*compt_sushis].teta=(angle-HOKUYO_OFFSET);

					//debug_printf(" X = [%ld mm]",coordCibleX);
					hokuyo_sushi[*compt_sushis].coordX=coordCibleX;

					//debug_printf(" Y = [%ld mm]\n",coordCibleY);
					hokuyo_sushi[*compt_sushis].coordY=coordCibleY;

					*compt_sushis+=1;
				}
				angle+=25;
				nb_pts++;
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
	dist_min = hokuyo_sushi[0].dist;
	for(i=1;i<compt;i++)
	{
		if(hokuyo_sushi[i].dist < dist_min)
			dist_min = hokuyo_sushi[i].dist;
	}
	debug_printf("dist min [%ld mm] \n",dist_min);
	//if(dist_min < HOKUYO_EVITEMENT_MIN)
		//LCD_DisplayStringLineColon(10,LCD_LINE_27,"WARNING ENNEMY DETECTED");
	return dist_min;
}

//fonctions de detection des ennemis
void hokuyo_detection_ennemis(Uint16 compt_sushis,int *nb_ennemi){
	Uint16 i;
	Sint32 x_comp;
	Sint32 y_comp;
	Sint32 moyenne_x;
	Sint32 moyenne_y;

	x_comp=hokuyo_sushi[0].coordX;
	y_comp=hokuyo_sushi[0].coordY;

	for(i=0;i<compt_sushis;i++)
	{
		if(	!(	hokuyo_sushi[i].coordX <= (x_comp+HOKUYO_DETECTION_MARGE)  	&&
				hokuyo_sushi[i].coordX >= (x_comp-HOKUYO_DETECTION_MARGE)	) &&
			!(hokuyo_sushi[i].coordY <= (y_comp+HOKUYO_DETECTION_MARGE)  	&&
				hokuyo_sushi[i].coordY >= (y_comp-HOKUYO_DETECTION_MARGE)	))
		{
				moyenne_x=(x_comp+hokuyo_sushi[i-1].coordX)/2;
				moyenne_y=(y_comp+hokuyo_sushi[i-1].coordY)/2;
				hokuyo_ennemi[*nb_ennemi].coordX=moyenne_x;
				hokuyo_ennemi[*nb_ennemi].coordY=moyenne_y;
				*nb_ennemi+=1;
				x_comp=hokuyo_sushi[i].coordX;
				y_comp=hokuyo_sushi[i].coordY;
		}
	}
	debug_printf("il y a  %d mechant(s)\n",*nb_ennemi);

	for(i=1;i<=*nb_ennemi;i++)
	{
		debug_printf(" ennemi numero %d x=[%ld mm]",i,hokuyo_ennemi[i].coordX);
		debug_printf(" et y=[%ld mm]\n",hokuyo_ennemi[i].coordY);
	}
}

void ReconObjet(Uint16 compt_sushis,int *nb_ennemi)
{
	Uint16 i;
	Sint32 nb_close_points;
	Sint32 sumX;
	Sint32 sumY;

	sumX=hokuyo_sushi[0].coordX;
	sumY=hokuyo_sushi[0].coordY;
	nb_close_points=1;
	for(i=1;i<compt_sushis;i++)
	{
		if((hokuyo_sushi[i].coordX > hokuyo_sushi[i-1].coordX+HOKUYO_DETECTION_MARGE && hokuyo_sushi[i].coordX < hokuyo_sushi[i-1].coordX-HOKUYO_DETECTION_MARGE) && (hokuyo_sushi[i].coordY > hokuyo_sushi[i-1].coordY+HOKUYO_DETECTION_MARGE && hokuyo_sushi[i].coordY < hokuyo_sushi[i-1].coordY-HOKUYO_DETECTION_MARGE) )
		{
			hokuyo_ennemi[*nb_ennemi].coordX=sumX/nb_close_points;
			hokuyo_ennemi[*nb_ennemi].coordY=sumY/nb_close_points;
			nb_close_points=1;
			*nb_ennemi+=1;
			sumX=hokuyo_sushi[i].coordX;
			sumY=hokuyo_sushi[i].coordY;
		}
		else
		{
			sumX+=hokuyo_sushi[i].coordX;
			sumY+=hokuyo_sushi[i].coordY;
			nb_close_points++;
			hokuyo_ennemi[*nb_ennemi].coordX=sumX/nb_close_points;
			hokuyo_ennemi[*nb_ennemi].coordY=sumY/nb_close_points;
		}
	}
	debug_printf("il y a  %d mechant(s)\n",*nb_ennemi);

	for(i=1;i<=*nb_ennemi;i++)
	{
		debug_printf(" ennemi numero %d x=[%ld mm]",i,hokuyo_ennemi[i].coordX);
		debug_printf(" et y=[%ld mm]\n",hokuyo_ennemi[i].coordY);
	}
}


void DetectRobots(Uint16 compt_sushis,int *nb_ennemi)
{
	Uint16 i;
	Uint16 nb_pts;
	Sint32 Distmax;
	Sint32 Xcarre, Ycarre;
	Sint32 Distcarre;
	Sint32 x_comp, y_comp, sumX, sumY;

	x_comp=hokuyo_sushi[0].coordX;
	y_comp=hokuyo_sushi[0].coordY;
	sumX=hokuyo_sushi[0].coordX;
	sumY=hokuyo_sushi[0].coordY;
	Distmax=10000;
	nb_pts=1;

	for(i=1;i<compt_sushis;i++)
	{
		Xcarre=(hokuyo_sushi[i].coordX-x_comp)^2;
		Ycarre=(hokuyo_sushi[i].coordY-y_comp)^2;
		Distcarre=Xcarre+Ycarre;
		if(Distcarre<=Distmax)
		{
			sumX+=hokuyo_sushi[i].coordX;
			sumY+=hokuyo_sushi[i].coordY;
			nb_pts++;
		}
		else
		{
			hokuyo_ennemi[*nb_ennemi].coordX=sumX/nb_pts;
			hokuyo_ennemi[*nb_ennemi].coordY=sumY/nb_pts;
			nb_ennemi++;
			nb_pts=1;
			x_comp=hokuyo_sushi[i].coordX;
			y_comp=hokuyo_sushi[i].coordY;
			sumX=hokuyo_sushi[i].coordX;
			sumY=hokuyo_sushi[i].coordY;
		}
	}
}
