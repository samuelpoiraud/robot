
#include "SD.h"
#include "../../QS/QS_ports.h"
#include "misc_test_term.h"
#include "ff_test_term.h"


void SD_init(void)
{
	PORTS_spi_init();

	#if STDIO_TEST
		stdio_tests();
	#endif

	#if DCC_TEST
		dcc_tests();
	#endif


}


//TODO :
/*
 *  - Remettre la gestion de la RTC (la notre...)
 *  - Faire fonctionner le DMA + mesurer le gain
 *  - Réglage de la vitesse du SPI... -> ?
 *  - Mettre en place un système d'enregistrement des matchs... utilisant la date et un numéro pour les cas où la RTC ne fonctionne pas
 *  -
 *
 *
 */

/*	//Exemple pour s'inspirer...
static void ini_test_create()
{
	int res;

	if ( f_mount(0, &fatfs) != FR_OK ) {
		xprintf("f_mount failed\n");
	} else {
		if ( f_open(&file, ini_filename, FA_CREATE_ALWAYS | FA_WRITE ) != FR_OK ) {
			xprintf("f_open for %s failed\n", ini_filename );
		} else {
			res = f_puts("[INFO]\n", &file );
			if ( res != EOF ) { res = f_puts("company = Martin Thomas Software Engineering\n", &file ); }
			if ( res != EOF ) { res = f_puts("author = Martin Thomas\n", &file ); }
			if ( res != EOF ) { res = f_puts("number = 12345678\n", &file ); }
			f_close( &file );
			xprintf("%s created\n", ini_filename);
		}
	}
}
*/


void SD_process_main(void)
{
	typedef enum { APPSTATE_FF_TERM, APPSTATE_TESTMENU } AppState;

	static AppState appState = APPSTATE_FF_TERM;

	switch ( appState ) 
	{
		case APPSTATE_FF_TERM:
			/* ff_test_term is not reentrant, blocks until exit */
			if ( !ff_test_term() ) {
				appState = APPSTATE_TESTMENU;
			}
			break;
		case APPSTATE_TESTMENU:
			/* misc_test_term is a state-machine, main-loop keeps running
			 * but may be throttled by time-consuming routines */
			if ( !misc_test_term() ) {
				appState = APPSTATE_FF_TERM;
			}
			break;
		default:
			appState = APPSTATE_TESTMENU;
			break;
	}
}

void SD_process_1ms(void)
{
	static uint16_t cnt=0;
		static uint8_t flip=0, cntdiskio=0;

		cnt++;
		if( cnt >= 500 ) {
			cnt = 0;
			/* alive sign */
			if ( flip ) {
				// GPIO_SetBits(GPIO_LED, GPIO_Pin_LED2 );
				//GPIO_LED->BSRR = GPIO_Pin_LED2;
			} else {
				// GPIO_ResetBits(GPIO_LED, GPIO_Pin_LED2 );
				//GPIO_LED->BRR = GPIO_Pin_LED2;
			}
			flip = !flip;
		}

		cntdiskio++;
		if ( cntdiskio >= 10 ) {
			cntdiskio = 0;
			disk_timerproc(); /* to be called every 10ms */
		}

		ff_test_term_timerproc(); /* to be called every ms */
}


