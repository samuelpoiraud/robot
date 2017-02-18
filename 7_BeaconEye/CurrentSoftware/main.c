/*
 *	Club Robot ESEO 2008 - 2012
 *	Archi'Tech, CHOMP, CheckNorris, Shark & Fish
 *
 *	Fichier : main.c
 *	Package : Carte Principale
 *	Description : Ordonnancement de la carte Principale
 *	Auteur : Jacen, modifié par Gonsevi
 *	Version 2012/01/14
 */

#define MAIN_C
#include "main.h"
#include "QS/QS_ports.h"
#include "QS/QS_uart.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_timer.h"
#include "QS/QS_systick.h"
#ifdef STM32F40XX
#include "QS/QS_sys.h"
#endif
#include "it.h"
#include "zone.h"
#include "Secretary.h"
#include "LCD/middleware.h"
#include "LCD/low layer/ssd2119.h"
#include "environment.h"
#include "LCD/low layer/ssd2119.h"
#include "IHM/buzzer.h"
#include "LCD/lcd.h"
#include "QS_hokuyo/hokuyo.h"
#include "IHM/terminal.h"
#include "IHM/led.h"
#include "IHM/MusicPlayer/musicPlayer.h"
#include "IHM/MusicPlayer/song/songs.h"

static void initialisation(void);
static void processMain(void);

int main(void) {

	initialisation();

	debug_printf("------- Hello, I'm BEACON EYE -------\n");
	TERMINAL_puts("Initialization completed");

	while(1){
		processMain();
	}

	return 0;
}

static void initialisation(void) {

	/* Sans ce delai l'hokuyo n'est pas reconnu */
	#ifdef USE_HOKUYO
		Uint32 i;
		for(i=0; i<100000;i++){}
	#endif

	SYS_init();											// Initialisation du système
	SYSTICK_init((time32_t*)&(global.absolute_time));	// Init du compteur de temps
	TERMINAL_init();
	PORTS_init();
	ENVIRONMENT_init();
	UART_init();
	IT_init();
	MIDDLEWARE_init();
	MUSIC_PLAYER_init();
	#ifdef USE_HOKUYO
		HOKUYO_init();
	#endif

	MUSIC_PLAYER_addSongToPlaylist(missionImpossible);
	MUSIC_PLAYER_addSongToPlaylist(flinstones);
	MUSIC_PLAYER_addSongToPlaylist(imperialMarch);
	MUSIC_PLAYER_addSongToPlaylist(letItGo);


	MUSIC_PLAYER_play();

	//BUZZER_play(50, NOTE_SOL, 2);

}

static void processMain(void) {

	MUSIC_PLAYER_processMain();
	LCD_processMain();
	ENVIRONMENT_processMain();
#ifdef USE_HOKUYO
	HOKUYO_processMain();
#endif
}
