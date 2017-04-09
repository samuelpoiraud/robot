/*
 *	Club Robot ESEO 2013
 *
 *	Fichier : LCD_interface.c
 *	Package : Supervision
 *	Description : impl�mentation de l'interface de l'�cran LCD MCCOG42005A6W-BNMLWI
 *	Licence : CeCILL-C
 *	Auteur : HERZAEONE
 *	Version 201308
 */

#ifndef LCD_INTERFACE_H
	#define	LCD_INTERFACE_H

	#include "../QS/QS_all.h"

	#ifdef USE_LCD

		void LCD_init(void);
		void LCD_processMain(void);

	#endif

		/*
		 * Ecrit une ligne d�finie par l'utilisateur � la position demand�e (et bascule le LCD en menu utilisateur !)
		 * Line doit �tre entre 0 et 3 inclus.
		 * La ligne 0 correspond � la derni�re ligne du menu principal (INFOS)
		 * Les 3 autres lignes correspondent aux lignes du menu utilisateur.
		 */
		void LCD_printf(Uint8 line, bool_e switch_on_menu, bool_e log_on_sd, char * chaine, ...) __attribute__((format (printf, 4, 5)));

#endif	/* LCD_INTERFACE_H */

