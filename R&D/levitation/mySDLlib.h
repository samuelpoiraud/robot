/******************************************************************/
/*	Auteur: Herzaeone                                         */
/*	Date: Friday October 26, 2012                             */
/*	File: mySDLlib.h                                          */
/******************************************************************/

/****************************************************************/
/*	Ce fichier nécessite l'installation des			*/
/*	bibliothèques SDL					*/
/*	Installation:				 		*/
/*	Pour Ubuntu: sudo apt-get libsdl-image1.2-dev libsdl-image1.2 libsdl-1.2-dev	*/
/* Pour Fedora: sudo yum install 				*/
/****************************************************************/

/******************************************************************/
/* Lors de la compilation ajouter le parametre -lSDL -lSDL_image  */
/******************************************************************/


#ifndef MYSDLLIB_H
#define MYSDLLIB_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdlib.h>

#define surface SDL_Surface*

#define NCOLOR 32
#define WHITE 16777215


//Fonctions relatives au fonctionnement de la fenetre SDL
SDL_Surface* init_window(int Xsize, int Ysize, char* window_name, char* window_icon);	//l.18
void quit_window();		//l.34


//Fonctions de dessin
void clear_scr(SDL_Surface* screen);
void clear_scrColor(SDL_Surface* screen, int Red, int Green, int Blue);
void fill_rect(SDL_Rect * rect, SDL_Surface* screen, int Xsize, int Ysize, int Xpos, int Ypos, int Red, int Green, int Blue);

//Fonctions relatives à l'affichage d'images
SDL_Surface * load_image(char* img_name, SDL_Surface* screen, int Xpos, int Ypos, int transparency);
SDL_Surface * load_abl_image(char* img_name, SDL_Surface* screen, int Xpos, int Ypos, int Red, int Green, int Blue, int transparency);


//Fonctions Autres
void change_pos(SDL_Surface* layer, SDL_Surface* screen, int Xpos, int Ypos);
int focus_stop(SDL_Event event);
int button_pos(SDL_Event event, int Xpos, int Ypos, int Xtaille, int Ytaille);


#endif
