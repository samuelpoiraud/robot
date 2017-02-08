/*
 *	Club Robot ESEO 2008 - 2012
 *	Archi'Tech, CHOMP, CheckNorris, Shark & Fish
 *
 *	Fichier : main.h
 *	Package : Carte Principale
 *	Description : Ordonnancement de la carte Principale
 *	Auteur : Jacen, modifi� par Gonsevi
 *	Version 2012/01/14
 */

#include "QS/QS_all.h"


#ifndef MAIN_H
	#define MAIN_H
	#include "QS_hokuyo/STM32_USB_HOST_Library/Core/usbh_core.h"



	//Fonction appel�e en boucle par le module USB lorsqu'un p�riph�rique mass storage est connect�.
	int user_callback_MSC_Application(void);
	void user_callback_MSC_Application_Deinit(void);


	extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
	extern USBH_HOST                    USB_Host;

#endif /* ndef MAIN_H */
