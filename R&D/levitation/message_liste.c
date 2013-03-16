#include "levitation.h"


void position_adversaire_CAN(SDL_Rect pos, SDL_Event *event){
	char data[] = ":0103070D005098\r\n"; //message que tu veux envoyer
	
	msg_to_send.sid = BEACON_ADVERSARY_POSITION_IR_ARROUND_AERA;
	msg_to_send.size = 8;
	
	//les octets 4 et 5 servent à la position X en mm, les octets 6 et 7 à la position en Y.
	
 /* ATTENTION: La position en x du terrain est la position en y de la fenetre SDL! */
	pos.x = 3*event->button.y; //position à envoyer sur le terrain
	pos.y = 3*event->button.x;
					
	//Construcion du message
	msg_to_send.data[5] = pos.y & 0x00FF;
	
	//Uint16 temp = e.y & 0xFF00;
	msg_to_send.data[4] = pos.y >> 8;
	
	msg_to_send.data[3] = pos.x & 0x00FF;
	
	//temp = pos.x &0xFF00;
	msg_to_send.data[2] = pos.x >> 8;
	/*printf("0x%x\n0x%x\n", pos.x, pos.y);
	printf("\n\n%x\t\t%x\n\n",msg_to_send.data[4],msg_to_send.data[5]);
	printf("\n\n%x\t\t%x\n\n",msg_to_send.data[6],msg_to_send.data[7]);
	printf("\n\n%x\t\t%x\t\t%x\t\t%x\n\n",msg_to_send.data[0],msg_to_send.data[1],msg_to_send.data[2],msg_to_send.data[3]);
	*/
}

void set_red_CAN(void){
	int i;
	//char data[] = ":0103070D005098\r\n"; //message que tu veux envoyer
	
	msg_to_send.sid = SET_COLOR;
	msg_to_send.size = 1;
	msg_to_send.data[0] = 0;	
}

void set_blue_CAN(void){
	//char data[] = ":0103070D005098\r\n"; //message que tu veux envoyer
	
	msg_to_send.sid = SET_COLOR;
	msg_to_send.size = 1;
	msg_to_send.data[0] = 1;	
}

void calibrage_CAN(void){

	msg_to_send.sid = CALIBRAGE;
	msg_to_send.size = 8;
	int i;
	for(i=0;i<8;i++)
		msg_to_send.data[i] = 0;
}

void gotopos_CAN(SDL_Rect pos, SDL_Event *event){
	pos.x = 2000-3*event->button.y; //position à envoyer sur le terrain
	pos.y = 3000-3*event->button.x;
	msg_to_send.sid = ASSER_PUSH;
	msg_to_send.size = 8;
	msg_to_send.data[0] = 0;
	
	msg_to_send.data[1] = pos.x >> 8;
	msg_to_send.data[2] = pos.x & 0x00FF;
	msg_to_send.data[3] = pos.y >> 8;
	msg_to_send.data[4] = pos.x & 0x00FF;
	msg_to_send.data[5] = 0;
	msg_to_send.data[6] = 1;
	msg_to_send.data[7] = 0;
}

void asser_stop_CAN(void){
	msg_to_send.sid = 0x101;
	msg_to_send.size = 6;
	msg_to_send.data[0] = 1;	
	msg_to_send.data[1] = 1;
	msg_to_send.data[2] = 0;
	msg_to_send.data[3] = 0;
	msg_to_send.data[4] = 0;
	msg_to_send.data[5] = 0;
}
	
