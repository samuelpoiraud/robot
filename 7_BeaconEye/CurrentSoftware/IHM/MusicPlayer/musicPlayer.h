/*
 * musicPlayer.h
 *
 *  Created on: 18 févr. 2017
 *      Author: guill
 */

#ifndef IHM_MUSICPLAYER_MUSICPLAYER_H_
#define IHM_MUSICPLAYER_MUSICPLAYER_H_

#include "../../QS/QS_all.h"


typedef enum {
	P = 0,
 	C0 = 16,
 	Db0 = 17,
 	D0 = 18,
 	Eb0 = 19,
 	E0 = 20,
 	F0 = 21,
 	Gb0 = 23,
 	G0 = 24,
 	Ab0 = 25,
 	LA0 = 27,
 	Bb0 = 29,
 	B0 = 30,

 	C1 = 32,
 	Db1 = 34,
 	D1 = 36,
 	Eb1 = 38,
 	E1 = 41,
 	F1 = 43,
 	Gb1 = 46,
 	G1 = 49,
 	Ab1 = 51,
 	LA1 = 55,
 	Bb1 = 58,
 	B1 = 61,

 	C2 = 65,
 	Db2 = 69,
 	D2 = 73,
 	Eb2 = 77,
 	E2 = 82,
 	F2 = 87,
 	Gb2 = 92,
 	G2 = 98,
 	Ab2 = 103,
 	LA2 = 110,
 	Bb2 = 116,
 	B2 = 123,

 	C3 = 130,
 	Db3 = 138,
 	D3 = 146,
 	Eb3 = 155,
 	E3 = 164,
 	F3 = 174,
 	Gb3 = 185,
 	G3 = 196,
 	Ab3 = 207,
 	LA3 = 220,
 	Bb3 = 233,
 	B3 = 246,

 	C4 = 261,
 	Db4 = 277,
 	D4 = 293,
 	Eb4 = 311,
 	E4 = 329,
 	F4 = 349,
 	Gb4 = 369,
 	G4 = 392,
 	Ab4 = 415,
 	LA4 = 440,
 	Bb4 = 466,
 	B4 = 493,

 	C5 = 523,
 	Db5 = 554,
 	D5 = 587,
 	Eb5 = 622,
 	E5 = 659,
 	F5 = 698,
 	Gb5 = 739,
 	G5 = 783,
 	Ab5 = 830,
 	LA5 = 880,
 	Bb5 = 932,
 	B5 = 987,

 	C6 = 1046,
 	Db6 = 1108,
 	D6 = 1174,
 	Eb6 = 1244,
 	E6 = 1318,
 	F6 = 1396,
 	Gb6 = 1479,
 	G6 = 1567,
 	Ab6 = 1661,
 	LA6 = 1760,
 	Bb6 = 1864,
 	B6 = 1975,

 	C7 = 2093,
 	Db7 = 2217,
 	D7 = 2349,
 	Eb7 = 2489,
 	E7 = 2637,
 	F7 = 2793,
 	Gb7 = 2959,
 	G7 = 3135,
 	Ab7 = 3322,
 	LA7 = 3520,
 	Bb7 = 3729,
 	B7 = 3951,

 	C8 = 4186,
 	Db8 = 4434,
 	D8 = 4698,
 	Eb8 = 4978
} MUSIC_PLAYER_frequency_e;
typedef struct {
	MUSIC_PLAYER_frequency_e freq;
	Uint16 dur;
} MUSIC_PLAYER_note_t;

typedef struct{
	Uint16 nbNotes;
	const MUSIC_PLAYER_note_t *music;
} MUSIC_PLAYER_songInfo_s;

void MUSIC_PLAYER_init(void);
void MUSIC_PLAYER_processMain(void);
void MUSIC_PLAYER_play(void);
void MUSIC_PLAYER_removeSongFromPlaylist(Uint8 index);
void MUSIC_PLAYER_addSongToPlaylist(MUSIC_PLAYER_songInfo_s song);

#endif /* IHM_MUSICPLAYER_MUSICPLAYER_H_ */
