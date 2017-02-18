/*
 * musicPlayer.c
 *
 *  Created on: 18 févr. 2017
 *      Author: guill
 */

#include "musicPlayer.h"
#include "../QS/QS_pwm.h"
#include "../QS/QS_types.h"
#include "../QS/QS_outputlog.h"

#define NB_MAX_PLAYLIST_SONG	10
#define TRANSITION_DURATION		600 	// [ms]
#define SONG_NAME_SIZE_MAX		100

static MUSIC_PLAYER_songInfo_s playlist[NB_MAX_PLAYLIST_SONG];
static Uint8 nbSongInPlaylist = 0;
static bool_e isLaunch = FALSE;

static bool_e MUSIC_PLAYER_playSong(MUSIC_PLAYER_songInfo_s song);
static bool_e MUSIC_PLAYER_tone(MUSIC_PLAYER_note_t note);

void MUSIC_PLAYER_init(void) {
	PWM_init();
}

void MUSIC_PLAYER_processMain(void) {

	typedef enum {
		PLAY_SONG,
		PLAYING,
		TRANSITION,
		PAUSE
	} state_e;

	static state_e state = PAUSE;
	static time32_t begin_song_time = 0;

	switch(state) {

		case PLAY_SONG:
			if(nbSongInPlaylist <= 0) {
				state = PAUSE;
				isLaunch = FALSE;
			} else {
				state = PLAYING;
			}
			break;

		case PLAYING:
			if(MUSIC_PLAYER_playSong(playlist[0])) {
				MUSIC_PLAYER_removeSongFromPlaylist(0);
				state = TRANSITION;
				begin_song_time = global.absolute_time;
			}
			break;

		case TRANSITION:
			if(global.absolute_time > (begin_song_time + TRANSITION_DURATION)) {
				state = PLAY_SONG;
			}
			break;

		case PAUSE:
			if(isLaunch && nbSongInPlaylist > 0) {
				state = PLAY_SONG;
			}
			break;

	}

}

void MUSIC_PLAYER_play(void) {
	isLaunch = TRUE;
}

void MUSIC_PLAYER_removeSongFromPlaylist(Uint8 index) {
	if(index < nbSongInPlaylist) {
		Uint8 i;
		for(i = index; i < nbSongInPlaylist - 1; i++) {
			playlist[i] = playlist[i + 1];
		}
		nbSongInPlaylist--;
	}
}

void MUSIC_PLAYER_addSongToPlaylist(MUSIC_PLAYER_songInfo_s song) {
	if(nbSongInPlaylist < NB_MAX_PLAYLIST_SONG) {
		playlist[nbSongInPlaylist++] = song;
	}
}

static bool_e MUSIC_PLAYER_playSong(MUSIC_PLAYER_songInfo_s song) {

	static Uint16 currentNote = 0;

	if(currentNote < song.nbNotes) {
		if(MUSIC_PLAYER_tone(song.music[currentNote])) {
			currentNote++;
		}
	} else {
		/* C'est la fin de la musique */
		PWM_stop(4);
		currentNote = 0;
		return TRUE;
	}

	return FALSE;
}

static bool_e MUSIC_PLAYER_tone(MUSIC_PLAYER_note_t note) {

	typedef enum {
		PLAY_NOTE,
		WAIT_NOTE
	} state_e;

	static state_e state = PLAY_NOTE;
	static time32_t begin_note_time = 0;

	switch (state) {

		case PLAY_NOTE:
			if(note.freq > 0) {
				PWM_set_frequency(note.freq);
				PWM_run(50, 4);
			} else {
				PWM_stop(4);
			}

			begin_note_time = global.absolute_time;
			state = WAIT_NOTE;
		break;

		case WAIT_NOTE:
			if(global.absolute_time > (begin_note_time + note.dur)) {
				state = PLAY_NOTE;
				return TRUE;
			}
		break;

	}

	return FALSE;
}
