
#include "SD.h"
#include "../../QS/QS_ports.h"


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
 *  - R�glage de la vitesse du SPI... -> ?
 *  - Mettre en place un syst�me d'enregistrement des matchs... utilisant la date et un num�ro pour les cas o� la RTC ne fonctionne pas
 *  -
 *
 *
 */



int SD_process_main(void)
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


