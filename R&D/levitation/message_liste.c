#include "message_liste.h"


void position_adversaire_CAN(CAN_msg_t * msg, SDL_Rect * pos, SDL_Event *event){
	//char data[] = ":0103070D005098\r\n"; //message que tu veux envoyer
	
	msg->sid = BEACON_ADVERSARY_POSITION_IR_ARROUND_AERA;
	msg->size = 8;
	
	//les octets 4 et 5 servent à la position X en mm, les octets 6 et 7 à la position en Y.
	
	 /* ATTENTION: La position en x du terrain est la position en y de la fenetre SDL! */
	pos->x = 2000-3*event->button.y; //position à envoyer sur le terrain
	pos->y = 3000-3*event->button.x;
				
	//Construcion du message
	msg->data[5] = pos->y & 0x00FF;
	
	//Uint16 temp = e.y & 0xFF00;
	msg->data[4] = pos->y >> 8;
	
	msg->data[3] = pos->x & 0x00FF;
	
	//temp = pos.x &0xFF00;
	msg->data[2] = pos->x >> 8;
	/*printf("0x%x\n0x%x\n", pos.x, pos.y);
	printf("\n\n%x\t\t%x\n\n",msg->data[4],msg->data[5]);
	printf("\n\n%x\t\t%x\n\n",msg->data[6],msg->data[7]);
	printf("\n\n%x\t\t%x\t\t%x\t\t%x\n\n",msg->data[0],msg->data[1],msg->data[2],msg->data[3]);
	*/
}

void set_red_CAN(CAN_msg_t * msg){
	//int i;
	//char data[] = ":0103070D005098\r\n"; //message que tu veux envoyer
	
	msg->sid = SET_COLOR;
	msg->size = 1;
	msg->data[0] = 0;	
}

void set_blue_CAN(CAN_msg_t * msg){
	//char data[] = ":0103070D005098\r\n"; //message que tu veux envoyer
	
	msg->sid = SET_COLOR;
	msg->size = 1;
	msg->data[0] = 1;	
}

void calibrage_CAN(CAN_msg_t * msg){

	msg->sid = CALIBRAGE;
	msg->size = 8;
	int i;
	for(i=0;i<8;i++)
		msg->data[i] = 0;
}

void gotopos_CAN(CAN_msg_t * msg, SDL_Rect * pos, SDL_Event *event){
	pos->x = 2000-3*event->button.y; //position à envoyer sur le terrain
	pos->y = 3000-3*event->button.x;
	msg->sid = ASSER_PUSH;
	msg->size = 8;
	msg->data[0] = 0;
	
	msg->data[1] = pos->x >> 8;
	msg->data[2] = pos->x & 0x00FF;
	msg->data[3] = pos->y >> 8;
	msg->data[4] = pos->x & 0x00FF;
	msg->data[5] = 0;
	msg->data[6] = 1;
	msg->data[7] = 0;
}

void asser_stop_CAN(CAN_msg_t * msg){
	msg->sid = 0x101;
	msg->size = 6;
	msg->data[0] = 1;	
	msg->data[1] = 1;
	msg->data[2] = 0;
	msg->data[3] = 0;
	msg->data[4] = 0;
	msg->data[5] = 0;
}
	
