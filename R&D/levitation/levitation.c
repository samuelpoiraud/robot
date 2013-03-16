#include"levitation.h"

int main(void){
	//Ouverture de la fenetre SDL
	surface screen = init_window(1000, 666, "Levitation V1.0", "map.png");
	
 /* Déclarations et initialisations */
	
	//relatives à l'envioi de messages
	struct termios options;
	int reception;
	char buffer[16] = "";
	
	port_serie = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
	//lecture et ecriture | pas controlling terminal | ne pas attendre DCD
 
	//cas d'erreur d'ouverture
	if(port_serie < 0)
	{
		perror("Erreur d'ouverture du port serie");
		exit(EXIT_FAILURE);
	}
	
	printf("Port serie numero %d bien ouvert. \n", port_serie);
 
	//chargement des données
	tcgetattr(port_serie, &options);
	//B115200 bauds
	cfsetospeed(&options, B9600);		//Au robot, le standard est 9600 bits/secondes
	options.c_cflag |= (CLOCAL | CREAD);//programme propriétaire du port
	//structure en 8N1 !!
	options.c_cflag &= ~PARENB; //pas de parité
	options.c_cflag &= ~CSTOPB; // 1 bit de stop
	options.c_cflag &= ~CSIZE; //option a 0
	options.c_cflag |= CS8; //8 bits
	tcsetattr(port_serie, TCSANOW, &options); //enregistrement des valeurs de configuration
	printf("Configuration OK structure en 8N1 !. \n");
	
	
	//relatives à la SDL
	SDL_Event event;
	SDL_Rect pos;
	pos.x = 10;
	pos.y = 10;
	SDL_TimerID timer;
	timer = SDL_AddTimer(100, send_position, &pos); //Lancement du timer qui pointe sur la foncion send_position qui sera appellée toutes les 100ms
		
	
	//relatives au fonctionnement du programme
	int x,y,chase = 1;
	flag = 0b0;
	run = 0b0;
	
 /* Créaton de l'image de fond */
	surface map = load_image("terrain_BMP/terrain_996_663.bmp", screen, 0, 0, 0);
	SDL_Flip(screen);
	//SDL_EnableKeyRepeat(100,100);
	
 /* Lancement de la boucle d'event */
	while(chase){
		SDL_WaitEvent(&event);
		switch(event.type){
			case SDL_QUIT:
				chase = 0;
				break;
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym){
					case SDLK_p:
						run = ~run;
						break;
					case SDLK_c:
						calibrage_CAN();
						can_send(&msg_to_send, port_serie);
						break;
					case SDLK_r:
						set_red_CAN();
						can_send(&msg_to_send, port_serie);
						break;
					case SDLK_b:
						set_blue_CAN();
						can_send(&msg_to_send, port_serie);
						break;
					case SDLK_s:
						asser_stop_CAN();
						can_send(&msg_to_send, port_serie);
						break;
						
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if(event.button.button == SDL_BUTTON_RIGHT){
					if(!run) run = ~run;
					flag = ~flag;
				}
				if(event.button.button == SDL_BUTTON_LEFT){
					gotopos_CAN(pos, &event);
					can_send(&msg_to_send, port_serie);
				}
				break;
			case SDL_MOUSEMOTION:
				if(flag){
					position_adversaire_CAN(pos, &event);
				}
				break;
			default:
				break;
		}
	}
	close(port_serie);//fermeture du port serie
	printf("Port serie ferme. \n");
	SDL_RemoveTimer(timer);
	printf("Arrêt du Timer. \n");
	quit_window();
	return EXIT_SUCCESS;
}


Uint32 send_position(Uint32 intervalle,void *parameter){
	if(run){
		//SDL_Rect *pos = parameter;
		// Envoie de donnees.
		printf("Envoi des donnees en cours ... \n");
		can_send(&msg_to_send, port_serie);
		printf("Envoi Termine. \n");
	}
	return intervalle;
}
