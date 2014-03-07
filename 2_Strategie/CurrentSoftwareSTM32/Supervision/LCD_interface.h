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

#include "selftest.h"


/*
 * Initialise l'ecran et son affichage
 */
void init_LCD_interface(void);

/*
 * Met � jour l'affichage selon les evenements
 */
void LCD_Update(void);

/*
 * Permet de changer de mode d'affichage mode info<->can
 * Le mode info donne la positiondu robot, les infos balises, les strategies et des messages de lutilisateur
 */
void LCD_switch_mode(void);

/*
 * Permet de mettre � jour la position du robot : fonction � appeler � chaque broadcast position
 * D�clanche le rafraichissement de l'affichage
 */
void LCD_change_pos(void);

/*
 * Ajoute un message can � la liste de can � afficher en �crasant le plus ancien
 */
void LCD_add_can(CAN_msg_t msg);

/*
 * Se base sur les switchs pour mettre � jour les infos strat�gies
 */
void LCD_strat_number_update(void);


/*
 * Ecrit une ligne d�finie par l'utilisateur � la position demand�e
 */
//__attribute__((format (printf, 2, 3)))  permet au compilateur de verifier l'argument chaine avec les suivants comme avec printf, et afficher des warning si les types ne correspondent pas (genre un %s avec un int)
void LCD_printf(Uint8 pos, char * chaine, ...) __attribute__((format (printf, 2, 3)));

/*
 * Prend le control de l'�cran pour afficher les messages utilisateurs
 */
void LCD_take_control(void);

/*
 * Redonne le control � l'�cran
 */
void LCD_free_control(void);

/*
 * Lance l'affichage des erreurs de selftest
 */
void LCD_write_selftest_errors(Uint8 size);


/*
 * Fonctions d'action des boutons
 */
void LCD_button_minus(void);
void LCD_button_plus(void);
void LCD_button_ok(void);
void LCD_button_set(void);

#endif	/* LCD_INTERFACE_H */

