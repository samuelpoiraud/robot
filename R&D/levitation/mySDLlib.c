/******************************************************************/
/*	Auteur: Herzaeone                                             */
/*	Date: Friday October 26, 2012                                 */
/*	File: mySDLlib.c                                             */
/******************************************************************/ 

/******************************************************************/
/*	Ce fichier nécessite l'instalation des			*/
/*	bibliothèques SDL					*/
/*	Installation:						*/
/*	Pour Ubuntu: sudo apt-get  				*/
/* Pour Fedora: sudo yum install 				*/
/******************************************************************/

/******************************************************************/
/* Lors de la compilation ajouter le parametre -lSDL -lSDL_image  */
/******************************************************************/


#include"mySDLlib.h"

//Fonctions relatives au fonctionnement de la fenetre SDL

//Initialisation et ouverture d'une fenetre et renvoi une variable de type SDL_Surface
//Prend en charge le nom de la fenetre et son icone: si pas d'icone, écrire NULL dans le champ
SDL_Surface* init_window(int Xsize, int Ysize, char* window_name, char* window_icon){
	if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER)==-1){
		printf("SDL_INIT: ERROR\n");
		exit(EXIT_FAILURE);
	}
	if(window_icon)
		SDL_WM_SetIcon(IMG_Load(window_icon), NULL);
	surface screen = NULL;
	screen = SDL_SetVideoMode(Xsize, Ysize , NCOLOR, SDL_HWSURFACE | SDL_RESIZABLE | SDL_DOUBLEBUF);
	if(!screen){
		printf("SDL_WINDOW_LOAD: ERROR\n");
		exit(EXIT_FAILURE);
	}	
	SDL_WM_SetCaption(window_name, NULL);
	return screen;
}

//Fermeture de la fenetre
void quit_window(){
	SDL_Quit();
}



//Fonctions de dessin

//Nettoie l'écran en blanc
void clear_scr(SDL_Surface* screen){
	SDL_FillRect(screen, NULL, WHITE);
}

//Nettoie l'écran avec une couleur spécifiée en RGB 0->255
void clear_scrColor(SDL_Surface* screen, int Red, int Green, int Blue){
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, Red, Green, Blue));
}

//Créé une surface rectangle de la couleur,à la position et aux dimensions spécifiées
void fill_rect(SDL_Rect * rect, SDL_Surface* screen, int Xsize, int Ysize, int Xpos, int Ypos, int Red, int Green, int Blue){
	rect->x = Xpos;
	rect->y = Ypos;
	rect->w = Xsize;
	rect->h = Ysize;
	SDL_FillRect(screen, rect, SDL_MapRGB(screen->format, Red, Green, Blue));
	//SDL_Rect position;
	//position.x = Xpos;
	//position.y = Ypos;
	//SDL_BlitSurface(rect, NULL, screen, &position);
	return;
}



//Fonctions relatives à l'affichage d'images

//Affiche une image à la position spécifiée
SDL_Surface * load_image(char* img_name, SDL_Surface* screen, int Xpos, int Ypos, int transparency){
	SDL_Surface * image;
	image = IMG_Load(img_name);
	SDL_Rect position;
	position.x = Xpos;
	position.y = Ypos;
	SDL_SetAlpha(image, SDL_SRCALPHA, (255 - transparency));	//transparency est le taux de transparence: 0 = opaque
	SDL_BlitSurface(image, NULL, screen, &position);
	return image;
}

//Affiche une image à la position spécifiée avec ablation de la couleur demandée
SDL_Surface * load_abl_image(char* img_name, SDL_Surface* screen, int Xpos, int Ypos, int Red, int Green, int Blue, int transparency){
	SDL_Surface * image;
	image = IMG_Load(img_name);
	SDL_Rect position;
	position.x = Xpos;
	position.y = Ypos;
	SDL_SetColorKey(image, SDL_SRCCOLORKEY, SDL_MapRGB(image->format, Red, Green, Blue));
	SDL_SetAlpha(image, SDL_SRCALPHA, (255 - transparency));	//transparency est le taux de transparence: 0 = opaque
	SDL_BlitSurface(image, NULL, screen, &position);
	return image;
}



//Fonctions autres

//Change la position d'une surface
void change_pos(SDL_Surface* layer, SDL_Surface* screen, int Xpos, int Ypos){
	SDL_Rect position;
	position.x = Xpos;
	position.y = Ypos;
	SDL_BlitSurface(layer, NULL, screen, &position);
}

//Test si le programme est en focus ou non
int focus_stop(SDL_Event event){
	if((event.active.state & SDL_APPACTIVE) == SDL_APPACTIVE){
		if(event.active.gain == 0)
			return 1;
		else
			return 0;
	}
	return 0;
}

//Vérifie si le pointeur de la souris se situe une surface rectangulaire spécifiée
int button_pos(SDL_Event event, int Xpos, int Ypos, int Xtaille, int Ytaille){
	if((event.motion.x > Xpos) && (event.motion.x < (Xpos+Xtaille))){
		if((event.motion.y > Ypos) && (event.motion.y < (Ypos+Ytaille)))
			return 1;
	}
	return 0;
}


//Mise en pause du programme
void pause(){
    int chase = 1;
    SDL_Event event;
 
    while (chase)
    {
        SDL_WaitEvent(&event);
        switch(event.type){
            case SDL_QUIT:
                chase = 0;
            case SDL_KEYUP:
            	switch(event.key.keysym.sym){
            		case SDLK_ESCAPE:
           				chase = 0;
           				break;
           			default:
           				break;
           			}
           	default:
           		break;
        }
    }
}



	


