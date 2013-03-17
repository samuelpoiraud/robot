#include"levitation.h"
#include"mySDLlib.h"
#include"messages_CAN.h"
#include"message_liste.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

/* Prototypes */
	Uint32 send_position(Uint32 intervalle, void *parameter);
	void update_display(void);

/* Global vars */
	bool_e run,flag, must_update_view,must_refresh_all;
	CAN_msg_t msg_adv;
	CAN_msg_t msg;
	int port_serie;

SDL_Rect adv, us;	//Contains position and size of the adversary_rectangle and our robot rectangle !
SDL_Surface * screen; 	//Pointer to the "SDL screen"
SDL_Surface * map;
SDL_Surface * video_zone;	//This video zone will contain all moving elements.
SDL_Rect pos, previous_pos, goto_pos;
SDL_Rect background_position;

#define WINDOW_SIZE_X	1000		//px
#define WINDOW_SIZE_Y	666		//px
#define US_SIZE_X	100		//px
#define US_SIZE_Y	100		//px
#define ADV_SIZE_X	100		//px
#define ADV_SIZE_Y	100		//px

#define	ADVERSARY_VIEW_MIN_STEP	100	//mm

void init_screen(void)
{
	if( SDL_Init(SDL_INIT_VIDEO) < 0 ) 
		exit(1);
	SDL_WM_SetCaption("SDL Window", NULL);
	screen = init_window(WINDOW_SIZE_X, WINDOW_SIZE_Y, "Levitation V1.0", NULL);
	video_zone = SDL_SetVideoMode( WINDOW_SIZE_X , WINDOW_SIZE_Y , 32 , SDL_DOUBLEBUF|SDL_HWSURFACE|SDL_ANYFORMAT);
	map = load_image("terrain_BMP/terrain_996_663.bmp", video_zone, 0, 0, 0);
	//initialise position adverse
	background_position.x = 0;
	background_position.y = 0;
	pos.x = 1500;
	pos.y = 1500;
	goto_pos.x = 650;
	goto_pos.y = 120;

	update_display();
	SDL_Flip(video_zone);
	//SDL_EnableKeyRepeat(100,100);
}

void init_serial_port(void)
{
	struct termios options;	//options ouverture port série
	port_serie = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
	//lecture et ecriture | pas controlling terminal | ne pas attendre DCD
 
	//cas d'erreur d'ouverture
	if(port_serie < 0)
	{
		perror("Erreur d'ouverture du port serie");
		//exit(EXIT_FAILURE);
	}
	else
	{
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
	}
}

void update_display(void)
{
	adv.x = (3000-pos.y)/3-ADV_SIZE_X/2;
	adv.y = (2000-pos.x)/3-ADV_SIZE_Y/2;
	us.x = (3000-goto_pos.y)/3-US_SIZE_X/2;
	us.y = (2000-goto_pos.x)/3-US_SIZE_Y/2;
	us.h = US_SIZE_X; 
	us.w = US_SIZE_Y;
	adv.h = ADV_SIZE_X;
	adv.w = ADV_SIZE_Y;
	SDL_FillRect(video_zone , NULL , 0x221122);  
	SDL_BlitSurface(map, NULL, video_zone, &background_position);  		
	SDL_FillRect(video_zone , &adv , SDL_MapRGB(video_zone->format , 0, 150 , 0 ) );
	SDL_FillRect(video_zone , &us , SDL_MapRGB(video_zone->format , 100, 0 , 0 ) );
	if(must_refresh_all)
	{
		SDL_Flip(video_zone);
		must_refresh_all = FALSE;
	}	
	else
	{
		SDL_UpdateRect(video_zone, 	MIN(MAX(0,adv.x-ADVERSARY_VIEW_MIN_STEP),WINDOW_SIZE_X), 
						MIN(MAX(0,adv.y-ADVERSARY_VIEW_MIN_STEP),WINDOW_SIZE_Y),
						MIN(adv.w+2*ADVERSARY_VIEW_MIN_STEP, WINDOW_SIZE_X-adv.x+ADVERSARY_VIEW_MIN_STEP),
						MIN(adv.h+2*ADVERSARY_VIEW_MIN_STEP, WINDOW_SIZE_Y-adv.y+ADVERSARY_VIEW_MIN_STEP) );
	}
}
int main(void)
{
	//relatives à la SDL
	SDL_Event event;
	SDL_TimerID timer;
	int chase = 1;
	flag = FALSE;
	run = FALSE;
	
	init_screen();

	init_serial_port();
	
	timer = SDL_AddTimer(100, send_position, &pos); //Lancement du timer qui pointe sur la foncion send_position qui sera appelée toutes les 100ms

	
 /* Tache de fond*/
	while(chase){
		
		if(must_update_view || must_refresh_all)
		{
			update_display();	
			must_update_view = FALSE;
		}
		
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
					case SDLK_c:	//Calibration
						calibrage_CAN(&msg);
						can_send(&msg, port_serie);
						break;
					case SDLK_r:	//color red
						set_red_CAN(&msg);
						can_send(&msg, port_serie);
						break;
					case SDLK_b:	//color blue
						set_blue_CAN(&msg);
						can_send(&msg, port_serie);
						break;
					case SDLK_s:	//asser stop
						asser_stop_CAN(&msg);
						can_send(&msg, port_serie);
						break;
					default:
						break;
						
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if(event.button.button == SDL_BUTTON_RIGHT){
					position_adversaire_CAN(&msg_adv,&pos, &event);
					if(!run) run = ~run;
					flag = ~flag;
					must_refresh_all = TRUE;
				}
				if(event.button.button == SDL_BUTTON_LEFT){
					must_refresh_all = TRUE;
					gotopos_CAN(&msg, &goto_pos, &event);
					can_send(&msg, port_serie);
				}
				break;
			case SDL_MOUSEMOTION:
				if(flag){
					position_adversaire_CAN(&msg_adv,&pos, &event);
					if(	(ABS(pos.x - previous_pos.x) > ADVERSARY_VIEW_MIN_STEP) || 
						(ABS(pos.y - previous_pos.y) > ADVERSARY_VIEW_MIN_STEP)	)
					{
						must_update_view = TRUE;
						previous_pos.x = pos.x;
						previous_pos.y = pos.y;
					}
				}
				break;
			default:
				break;
		}
	}
	if(port_serie>0)
	{
		close(port_serie);	//fermeture du port serie
		printf("Port serie ferme. \n");
	}
	SDL_RemoveTimer(timer);
	printf("Arrêt du Timer. \n");
	quit_window();
	return EXIT_SUCCESS;
}


Uint32 send_position(Uint32 intervalle,void *parameter){
	if(run){
		//SDL_Rect *pos = parameter;
		// Envoi de donnees.
		can_send(&msg_adv, port_serie);
	}
	return intervalle;
}
