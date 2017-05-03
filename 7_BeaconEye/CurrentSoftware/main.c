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
#include "LCD/middleware.h"
#include "environment.h"
#include "IHM/buzzer.h"
#include "LCD/lcd.h"
#include "QS/QS_hokuyo/hokuyo.h"
#include "IHM/terminal.h"
#include "IHM/MusicPlayer/musicPlayer.h"
#include "IHM/MusicPlayer/song/songs.h"
#include "zone.h"
#include "Secretary.h"

static void initialisation(void);
static void processMain(void);

int main(void) {

	initialisation();

	debug_printf("------- Hello, I'm BEACON EYE -------\n");

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
	PORTS_init();
	ENVIRONMENT_init();
	UART_init();
	IT_init();
	MIDDLEWARE_init();
	MUSIC_PLAYER_init();
#ifdef USE_HOKUYO
	HOKUYO_init();
#endif
	ZONE_init();
	TERMINAL_init();
	SECRETARY_init();

	MUSIC_PLAYER_addSongToPlaylist(smurfs);
	MUSIC_PLAYER_addSongToPlaylist(missionImpossible);
	MUSIC_PLAYER_addSongToPlaylist(superMarioBros);
	MUSIC_PLAYER_addSongToPlaylist(letItGo);
	MUSIC_PLAYER_addSongToPlaylist(imperialMarch);
}

static void processMain(void) {

	LCD_processMain();
	ENVIRONMENT_processMain();
#ifdef USE_HOKUYO
	HOKUYO_processMain();
#endif
	MUSIC_PLAYER_processMain();
	ZONE_processMain();
	SECRETARY_processMain();
	TERMINAL_processMain();
}
