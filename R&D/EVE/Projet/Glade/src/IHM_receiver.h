/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : IHM_receiver.h
 *	Package : EVE
 *	Description : Gestion de la réception et de l'affichage des uart et du CAN 
 *	Auteur : Julien Franchineau & François Even
 *	Version 20111228
 */

#ifndef IHM_RECEIVER_H
	#define MAIN_H

	void* IHM_thread_receiver(void);
	//void IHM_update_can(can_msg_t can_message);
	//void IHM_change_uart_display(int uart_id,uart_list_e new_uart_display);
	void IHM_update_uart(void);
	void IHM_update_data(void);
#endif
