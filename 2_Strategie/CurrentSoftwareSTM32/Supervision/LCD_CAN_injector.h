/*
 *	Club Robot ESEO 2013
 *
 *	Fichier : LCD__CAN_injector.c
 *	Package : Supervision
 *	Description : Affiche les messages can sur l'�cran selon leur SID
 *	Licence : CeCILL-C
 *	Auteur : HERZAEONE
 *	Version 201308
 */

#ifndef LCD_CAN_INJECTOR_H
#define	LCD_CAN_INJECTOR_H



/*
 * Affiche sur l'�cran le message can en param�tre � la position donn�e
 */
void display_can(CAN_msg_t msg, Uint8 pos);

#endif	/* LCD_CAN_INJECTOR_H */

