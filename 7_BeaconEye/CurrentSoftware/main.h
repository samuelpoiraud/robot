/*
 *	Club Robot ESEO 2008 - 2012
 *	Archi'Tech, CHOMP, CheckNorris, Shark & Fish
 *
 *	Fichier : main.h
 *	Package : Carte Principale
 *	Description : Ordonnancement de la carte Principale
 *	Auteur : Jacen, modifié par Gonsevi
 *	Version 2012/01/14
 */

#include "QS/QS_all.h"


#ifndef MAIN_H
	#define MAIN_H
	#include "usb_host/Core/usbh_core.h"



	//Fonction appelée en boucle par le module USB lorsqu'un périphérique mass storage est connecté.
	int user_callback_MSC_Application(void);
	void user_callback_MSC_Application_Deinit(void);


	extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
	extern USBH_HOST                    USB_Host;

#endif /* ndef MAIN_H */
