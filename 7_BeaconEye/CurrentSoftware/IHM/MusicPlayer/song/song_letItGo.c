/*
 * song_letItGo.c
 *
 *  Created on: 18 févr. 2017
 *      Author: guill
 */

#include "song_letItGo.h"

extern const MUSIC_PLAYER_note_t letItGoData[51];

const MUSIC_PLAYER_songInfo_s letItGo = {
		.nbNotes = 51,
		.music = letItGoData
};

const MUSIC_PLAYER_note_t letItGoData[51] = {
	(MUSIC_PLAYER_note_t){F6,  250},
	(MUSIC_PLAYER_note_t){G6,  250},
	(MUSIC_PLAYER_note_t){Ab6, 1000},
	(MUSIC_PLAYER_note_t){Eb6, 250},
	(MUSIC_PLAYER_note_t){Eb6, 250},
	(MUSIC_PLAYER_note_t){Bb6, 1000},

	(MUSIC_PLAYER_note_t){Ab6, 500},
	(MUSIC_PLAYER_note_t){F6,  250},
	(MUSIC_PLAYER_note_t){F6,  250},
	(MUSIC_PLAYER_note_t){F6,  500},
	(MUSIC_PLAYER_note_t){F6,  250},
	(MUSIC_PLAYER_note_t){G6,  500},
	(MUSIC_PLAYER_note_t){Ab6, 1000},

	(MUSIC_PLAYER_note_t){F6,  250},
	(MUSIC_PLAYER_note_t){G6,  250},
	(MUSIC_PLAYER_note_t){Ab6, 1000},
	(MUSIC_PLAYER_note_t){Eb6, 250},
	(MUSIC_PLAYER_note_t){Eb6, 250},
	(MUSIC_PLAYER_note_t){C7,  500},

	(MUSIC_PLAYER_note_t){Bb6, 1000},
	(MUSIC_PLAYER_note_t){Ab6, 250},
	(MUSIC_PLAYER_note_t){Bb6, 250},
	(MUSIC_PLAYER_note_t){C7,  500},
	(MUSIC_PLAYER_note_t){C7,  250},
	(MUSIC_PLAYER_note_t){Db7, 500},
	(MUSIC_PLAYER_note_t){C7,  500},
	(MUSIC_PLAYER_note_t){Bb6, 500},
	(MUSIC_PLAYER_note_t){Ab6, 1000},

	(MUSIC_PLAYER_note_t){Eb7, 667},
	(MUSIC_PLAYER_note_t){C7,  667},
	(MUSIC_PLAYER_note_t){Bb6, 1000},
	(MUSIC_PLAYER_note_t){Ab6, 500},
	(MUSIC_PLAYER_note_t){Ab6, 500},
	(MUSIC_PLAYER_note_t){Eb7, 625},
	(MUSIC_PLAYER_note_t){C7,  750},
	(MUSIC_PLAYER_note_t){Ab6, 500},

	(MUSIC_PLAYER_note_t){Ab6, 500},
	(MUSIC_PLAYER_note_t){Ab6, 500},
	(MUSIC_PLAYER_note_t){G6,  667},
	(MUSIC_PLAYER_note_t){Eb6, 667},
	(MUSIC_PLAYER_note_t){Eb6, 1000},

	(MUSIC_PLAYER_note_t){C6,  250},
	(MUSIC_PLAYER_note_t){Db6, 500},
	(MUSIC_PLAYER_note_t){Db6, 250},
	(MUSIC_PLAYER_note_t){C6,  250},
	(MUSIC_PLAYER_note_t){Db6, 250},
	(MUSIC_PLAYER_note_t){C6,  250},
	(MUSIC_PLAYER_note_t){Db6, 250},
	(MUSIC_PLAYER_note_t){Db6, 250},
	(MUSIC_PLAYER_note_t){C6,  250},
	(MUSIC_PLAYER_note_t){Ab5, 1000},
};
