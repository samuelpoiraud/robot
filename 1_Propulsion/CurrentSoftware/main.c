/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi-Tech'
 *
 *	Fichier : main.c
 *	Package : Projet Propulsion
 *	Description : fonction principale d'exemple pour le projet
 *				standard construit par la QS pour exemple, pour
 *				utilisation en Match
 *	Auteur : Jacen
 *	Version 20080924
 */


#include "main.h"
#include "odometry.h"
#include "copilot.h"
#include "pilot.h"
#include "supervisor.h"
#include "warner.h"
#include "it.h"
#include "roadmap.h"
#include "secretary.h"
#include "sequences.h"
#include "debug.h"
#include "joystick.h"
#include "QS/QS_ports.h"
#include "QS/QS_uart.h"
#include "QS/QS_buttons.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_outputlog.h"


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

#if defined (STM32F40XX)
	#include "QS/QS_sys.h"
#endif
#if defined (SIMULATION_VIRTUAL_PERFECT_ROBOT)
	#include "LCDTouch/stm32f4_discovery_lcd.h"
	#include "LCDTouch/LCDTouch_Display.h"
	#include "LCDTouch/LCD.h"
	#include "LCDTouch/zone.h"
#endif
#ifdef	SCAN_TRIANGLE
	#include "scan_triangle.h"
#endif

#ifdef MODE_SAVE_STRUCTURE_GLOBAL_A_CHAQUE_IT
	extern volatile global_data_storage_t SAVE;
#endif


#if defined(VERBOSE_MSG_SEND_OVER_UART) ||				\
	 defined(SIMULATION_VIRTUAL_PERFECT_ROBOT) ||		\
	 defined(MODE_PRINTF_TABLEAU) ||					\
	 defined(MODE_SAVE_STRUCTURE_GLOBAL_A_CHAQUE_IT) ||	\
	 defined(SUPERVISOR_DISABLE_ERROR_DETECTION) ||		\
	 defined(MODE_REGLAGE_KV) ||						\
	 !defined(ENABLE_CAN)
	#warning "SOYEZ CONSCIENT QUE VOUS NE COMPILEZ PAS EN MODE MATCH..."
#endif


#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
	#pragma data_alignment=4
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE      USB_OTG_Core __ALIGN_END;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
	#pragma data_alignment=4
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USBH_HOST                USB_Host __ALIGN_END;

/* Private functions ---------------------------------------------------------*/
static Uint8 Explore_Disk (char* path , Uint8 recu_level);

void RCON_read(void);
void button_autocalage_avant(void);
void button_autocalage_arriere(void);

void initialisation(void){

	#ifdef USE_QSx86
		// Initialisation pour EVE
		EVE_manager_card_init();
	#endif	/* USE_QSx86 */
	#if defined(STM32F40XX)
		SYS_init();
	#endif
	// Config des ports
	PORTS_init();

	LED_RUN = 1;

#ifdef __dsPIC30F6010A__
	// Pour l'utilisation de la fenetre psv
	//> ceci est utile pour le stockage d'un tableau de boeuf dans la mémoire programme
	CORCONbits.PSV=1;
#endif

	UART_init(); //Si les résistances de tirages uart ne sont pas reliées, le code bloque ici si aucun cable n'y est relié.
	Uint16 delay;
	for(delay = 1;delay;delay++);	//attente pour que l'UART soit bien prete...
	RCON_read();

	//Sur quel robot est-on ?
	QS_WHO_AM_I_find();	//Détermine le robot sur lequel est branchée la carte.
	//Doit se faire AVANT ODOMETRY_init() !!!
	debug_printf("I am %s\n",(QS_WHO_AM_I_get()==TINY)?"TINY":"KRUSTY");

	ODOMETRY_init();
	SUPERVISOR_init();
	COPILOT_init();
	PILOT_init();
	SECRETARY_init();
	ROADMAP_init();
	WARNER_init();

	JOYSTICK_init();
	DEBUG_init();

	BUTTONS_init();
//	BUTTONS_define_actions(BUTTON3,button_autocalage_avant, NULL, 0);
//	BUTTONS_define_actions(BUTTON1,button_autocalage_arriere, NULL, 0);


	IT_init();
	/*
	Récapitulatif des priorités des ITs :
	-> 7 : Codeurs QEI_on_it
	-> 6 : CAN
	-> 5 : timer1 (5ms)
	-> 4 : UART
	-> 3 : timer2 (100ms)
	-> 0 : tâche de fond
	*/


#if defined (SIMULATION_VIRTUAL_PERFECT_ROBOT)
	LCD_init();
#endif

	#ifdef SCAN_TRIANGLE
		SCAN_TRIANGLE_init();
		// Initialisation des ADC pour les DT10s des scans des triangles
	#endif

}

int main (void)
{
	initialisation();


	//Routines de tests UART et CAN
	debug_printf("\nAsser Ready !\n");
/*		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1793,894	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1734,918	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1678,947	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1624,982	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1575,1022	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1529,1066	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1489,1115	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1453,1168	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1423,1223	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1398,1282	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1380,1343	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1367,1405	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1361,1468	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1361,1532	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1367,1595	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1380,1657	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1398,1718	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1423,1777	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1453,1832	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1489,1885	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1529,1934	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1575,1978	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1624,2018	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1678,2053	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1734,2082	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1793,2106	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);
		ROADMAP_add_order(TRAJECTORY_AUTOMATIC_CURVE, 	1830,2115	, 0, NOT_RELATIVE, NOT_NOW, ANY_WAY, NOT_BORDER_MODE, NO_MULTIPOINT, FAST, ACKNOWLEDGE_ASKED, CORRECTOR_ENABLE);


*/

	while(1)
	{
		#ifdef USE_QSx86
			// Update pour EVE
			EVE_manager_card();
		#endif	/* USE_QSx86 */

		DEBUG_process_main();

		BUTTONS_update();			//Gestion des boutons

		SECRETARY_process_main();	//Communication avec l'extérieur. (Réception des messages)

		WARNER_process_main();		//Communication avec l'extérieur. (Envois des messages)
		#ifdef SCAN_TRIANGLE
			SCAN_TRIANGLE_calculate();
		#endif
#if defined (SIMULATION_VIRTUAL_PERFECT_ROBOT)
		LCD_process_main();
#endif

	}
	return 0;
}


/*
void button_autocalage_avant(void)
{
	SEQUENCES_calibrate(FORWARD);
}

void button_autocalage_arriere(void)
{
	SEQUENCES_calibrate(BACKWARD);
}
*/
void RCON_read(void)
{
#if defined(__dsPIC30F6010A__)
	debug_printf("dsPIC30F reset source :\r\n");
	if(RCON & 0x8000)
		debug_printf("- Trap conflict event\r\n");
	if(RCON & 0x4000)
		debug_printf("- Illegal opcode or uninitialized W register access\r\n");
	if(RCON & 0x80)
		debug_printf("- MCLR Reset\r\n");
	if(RCON & 0x40)
		debug_printf("- RESET instruction\r\n");
	if(RCON & 0x10)
		debug_printf("- WDT time-out\r\n");
	if(RCON & 0x8)
		debug_printf("- PWRSAV #SLEEP instruction\r\n");
	if(RCON & 0x4)
		debug_printf("- PWRSAV #IDLE instruction\r\n");
	if(RCON & 0x2)
		debug_printf("- POR, BOR\r\n");
	if(RCON & 0x1)
		debug_printf("- POR\r\n");
	RCON=0;
#elif defined(STM32F40XX)
	debug_printf("STM32F4xx reset source :\n");
	if(RCC_GetFlagStatus(RCC_FLAG_LPWRRST))
		debug_printf("- Low power management\n");
	if(RCC_GetFlagStatus(RCC_FLAG_WWDGRST))
		debug_printf("- Window watchdog time-out\n");
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST))
		debug_printf("- Independent watchdog time-out\n");
	if(RCC_GetFlagStatus(RCC_FLAG_SFTRST))
		debug_printf("- Software reset\n");
	if(RCC_GetFlagStatus(RCC_FLAG_PORRST))
		debug_printf("- POR\n");
	if(RCC_GetFlagStatus(RCC_FLAG_PINRST))
		debug_printf("- Pin NRST\n");
	if(RCC_GetFlagStatus(RCC_FLAG_BORRST))
		debug_printf("- POR or BOR\n");
	RCC_ClearFlag();
#endif
}



const Uint8 MSG_DISK_SIZE[]      = "> Size of the disk in MBytes: \n";
const Uint8 MSG_LUN[]            = "> LUN Available in the device:\n";
const Uint8 MSG_ROOT_CONT[]      = "> Exploring disk flash ...\n";
const Uint8 MSG_WR_PROTECT[]      = "> The disk is write protected\n";
const Uint8 MSG_UNREC_ERROR[]     = "> UNRECOVERED ERROR STATE\n";


Uint8 line_idx = 0;

volatile bool_e deinit_asked = FALSE;
void user_callback_MSC_Application_Deinit(void)
{
	deinit_asked = TRUE;
}

int user_callback_MSC_Application(void)
{
	static FATFS fatfs;
	static FIL file;
	FRESULT res;
/*
 * 	static bool_e wrote_done = 0;
	UINT bytesWritten;
	UINT bytesToWrite;

	if(!wrote_done)
	{
		if(f_mount(0, &fatfs) == FR_OK)
		{
			if(f_open(&file, "0:Host_Write_Demo.TXT",FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
			{
				res= f_write (&file, "ceci est une chaine\n", 20, (void *)&bytesWritten);

				if((bytesWritten == 0) || (res != FR_OK)) //EOF or Error
				{
					debug_printf("> Host_Write_Demo.TXT CANNOT be writen.\n");
				}
				else
				{
					debug_printf("> 'Host_Write_Demo.TXT' file created\n");
				}

				//close file and filesystem
				f_close(&file);
				wrote_done = 1;
			}
		}
	}
*/
	  Uint8 writeTextBuff[] = "STM32 Connectivity line Host Demo application using FAT_FS   ";
	  Uint16 bytesWritten, bytesToWrite;
	  /* State Machine for the USBH_USR_ApplicationState */
	  typedef enum
	  {
		  USH_USR_FS_INIT  = 0,
		  USH_USR_FS_READLIST,
		  USH_USR_FS_WRITEFILE,
		  IDLE
	  }state_e;
	  static state_e USBH_USR_ApplicationState = USH_USR_FS_INIT;
	  if(deinit_asked)
	  {
		  USBH_USR_ApplicationState = USH_USR_FS_INIT;
		  deinit_asked = FALSE;
	  }
	  switch(USBH_USR_ApplicationState)
	  {
		  case USH_USR_FS_INIT:

			/* Initialises the File System*/
			if ( f_mount( 0, &fatfs ) != FR_OK )
			{
			  /* efs initialisation fails*/
			  debug_printf("> Cannot initialize File System.\n");
			  return(-1);
			}
			debug_printf("> File System initialized.\n");
			debug_printf("> Disk capacity : %lu Bytes\n", USBH_MSC_Param.MSCapacity * \
			  USBH_MSC_Param.MSPageLength);

			if(USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED)
			{
				debug_printf("%s", MSG_WR_PROTECT);
			}

			USBH_USR_ApplicationState = USH_USR_FS_READLIST;
			break;

		  case USH_USR_FS_READLIST:
			debug_printf("%s", MSG_ROOT_CONT);
			Explore_Disk("0:/", 1);
			line_idx = 0;
			USBH_USR_ApplicationState = USH_USR_FS_WRITEFILE;

			break;

		  case USH_USR_FS_WRITEFILE:
			USB_OTG_BSP_mDelay(100);

			debug_printf("> Writing File to disk flash ...\n");
			if(USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED)
			{
				debug_printf ( "> Disk flash is write protected \n");
			  USBH_USR_ApplicationState = IDLE;
			  break;
			}

			/* Register work area for logical drives */
			f_mount(0, &fatfs);

			if(f_open(&file, "0:Host_Write_Demo.TXT",FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
			{
			  /* Write buffer to file */
			  bytesToWrite = sizeof(writeTextBuff);
			  res= f_write (&file, writeTextBuff, bytesToWrite, (void *)&bytesWritten);

			  if((bytesWritten == 0) || (res != FR_OK)) /*EOF or Error*/
			  {
				  debug_printf("> Host_Write_Demo.TXT CANNOT be writen.\n");
			  }
			  else
			  {
				  debug_printf("> 'Host_Write_Demo.TXT' file created\n");
			  }

			  /*close file and filesystem*/
			  f_close(&file);
			  f_mount(0, NULL);
			}

			else
			{
				debug_printf ("> Host_Write_Demo.TXT created in the disk\n");
			}

			USBH_USR_ApplicationState = IDLE; //ToDo : wait here (continueous PB pressed issue)
			break;

		  case IDLE:

			if(HCD_IsDeviceConnected(&USB_OTG_Core))
			{
			  if ( f_mount( 0, &fatfs ) != FR_OK )
			  {
				/* fat_fs initialisation fails*/
				return(-1);
			  }
			}
			else
				USBH_USR_ApplicationState = USH_USR_FS_INIT;
			break;
		  default:
			  break;
	  }
	  return(0);

}

/**
* @brief  Explore_Disk
*         Displays disk content
* @param  path: pointer to root path
* @retval None
*/

static Uint8 Explore_Disk (char* path , Uint8 recu_level)
{

  FRESULT res;
  FILINFO fno;
  DIR dir;
  char *fn;

#if _USE_LFN
  static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);
#endif


  res = f_opendir(&dir, path);
  if (res == FR_OK) {
	while(HCD_IsDeviceConnected(&USB_OTG_Core))
	{
	  res = f_readdir(&dir, &fno);
	  if (res != FR_OK || fno.fname[0] == 0)
	  {
		break;
	  }
	  if (fno.fname[0] == '.')
	  {
		continue;
	  }
#if _USE_LFN
	  fn = *fno.lfname ? fno.lfname : fno.fname;
#else
	  fn = fno.fname;
#endif


	  line_idx++;
	  if(line_idx > 12)
	  {
		line_idx = 0;

	  }


	  if(recu_level == 1)
	  {
		debug_printf("   |__");
	  }
	  else if(recu_level == 2)
	  {
		debug_printf("   |   |__");
	  }
	  if((fno.fattrib & AM_MASK) == AM_DIR)
	  {
		debug_printf("%s", fn);
	  }
	  else
	  {
		debug_printf("%s", fn);
	  }
	  debug_printf("\n");
	  if(((fno.fattrib & AM_MASK) == AM_DIR)&&(recu_level == 1))
	  {
		Explore_Disk(fn, 2);
	  }
	}
  }
  return res;
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

#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED
/**
  * @brief  This function handles EP1_IN Handler.
  * @param  None
  * @retval None
  */
void OTG_HS_EP1_IN_IRQHandler(void)
{
  USBD_OTG_EP1IN_ISR_Handler (&USB_OTG_Core);
}

/**
  * @brief  This function handles EP1_OUT Handler.
  * @param  None
  * @retval None
  */
void OTG_HS_EP1_OUT_IRQHandler(void)
{
  USBD_OTG_EP1OUT_ISR_Handler (&USB_OTG_Core);
}
#endif
