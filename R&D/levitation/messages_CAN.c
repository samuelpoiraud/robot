#include"levitation.h"

void can_send(CAN_msg_t * msg, int port_serie)						
{																			
	 Uint8 buffer[13];
	 																	
	Uint8 i;																
	/* Envoi de l'octet SOH cf : protocole de communication QS */				
	buffer[0]=SOH;
	buffer[1]=(Uint8)(msg->sid >>8);
	buffer[2]=(Uint8)msg->sid;	
	for (i=0; i<msg->size; i++)												
		buffer[i+3]=msg->data[i];		

	for (i=msg->size; i<8; i++)																			
		buffer[i+3]=0xFF;	//On rempli les autres datas par des 0xFF.										
	
	buffer[11]=msg->size;
	/* Envoi de l'octet EOT cf : protocole de communication QS */			

	buffer[12]=EOT;
	
	printf("Le message suivant va etre envoyé:\t");
	for(i=0;i<13;i++){
		printf(" %x",buffer[i]);
	}
	putchar('\n');
	
	if (write(port_serie, buffer, 13) < 0)
		perror("ERROR\n");
}
