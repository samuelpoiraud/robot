/*
 *	Club Robot ESEO 2010 - 2011
 *	CheckNorris
 *
 *	Fichier : PGA.c
 *	Package : Projet Balise Récepteur US
 *	Description : Gestion du PGA (Programmable Gain Amplifier : Ampli à Gain Variable)
 *	Auteur : Nirgal
 *	Version 201012
 */
#define PGA_C
#include "PGA.h"
#include "QS/QS_spi.h"


volatile Uint8 ampli = 0;
volatile Uint8 current_ampli = 8;	//8 = valeur impossible pour forcer un premier envoi vers le PGA.
#define TRIS_CS	TRISGbits.TRISG9
#define LAT_CS	LATGbits.LATG9

//Ampli variable.
// L'objectif de l'ampli variable est de rendre le signal qui "vient de loin" une même amplitude que le signal qui "vient de près".
// L'idée d'origine qui consiste à augmenter l'ampli au fur et à mesure des steps (car un signal qui vient de loin vient plus tard) est une bonne idée, mais qui ne marche pas :
// La cause est la taille du motif, beaucoup trop grande... (10ms de motif, le motif est bien trop atteint par l'ampli variable)
// La seule possibilité restante est d'utiliser la distance pour modifier le gain de l'ampli...
// Encore faut-il avoir la distance, et la retrouver lorsqu'on l'a perdue ! (cela suppose aussi de se rendre compte qu'on l'a perdue !)


void PGA_init(void)
{
	SPI_init();
	LAT_CS = 1;
	TRIS_CS = 0;
	TRISGbits.TRISG8 = 0;
	TRISGbits.TRISG7 = 0;
	TRISGbits.TRISG6 = 0;
	PGA_set_ampli(2);
}	


//On met à jour l'ampli du PGA en fonction de la distance à laquelle on pense que l'autre balise se situe...
//A priori, il s'agit de la distance actuellement mesurée...
void PGA_maj_ampli(Uint16 distance_estimee)
{
	//MAJ de l'ampli en fonction de la distance...
	ampli = 0;								//Ampli par 1.
	if(distance_estimee > 1000)	ampli = 1;	//Ampli par 2
	if(distance_estimee > 2000)	ampli = 2;	//Ampli par 5
	if(distance_estimee > 3000)	ampli = 3;	//Ampli par 10
	//if(distance_estimee > 1000)	ampli = 3;
	//if(distance_estimee > 1400)	ampli = 3;
	//if(distance_estimee > 2000)	ampli = 3;
	//if(distance_estimee > 2800)	ampli = 3;
//	PGA_set_ampli(ampli);
}
	
	#define CHANNEL 0b00000000	//Channel 0
//	#define CHANNEL 0b00000001	//Channel 1	
	
//Attention : ampli compris entre 0 et 7 !
void PGA_set_ampli(Uint8 ampli)
{
	if(ampli == current_ampli)
		return;		//Rien à faire si on est déjà bon !
	
	if(ampli > 7) 
		return;	//Rien à faire si on nous demande n'importe quoi !
	//debug_printf("ampli = %d", ampli);
	current_ampli = ampli;

	LAT_CS = 0;
	SPI2_put_byte(0x2A);
	SPI2_put_byte((current_ampli << 4) | CHANNEL);
	LAT_CS = 1;	
	
}	

