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
void LCD_add_can(CAN_msg_t * msg);



/*
 * Ecrit une ligne d�finie par l'utilisateur � la position demand�e (et bascule le LCD en menu utilisateur !)
 * Line doit �tre entre 0 et 3 inclus.
 * La ligne 0 correspond � la derni�re ligne du menu principal (INFOS)
 * Les 3 autres lignes correspondent aux lignes du menu utilisateur.
 */
//__attribute__((format (printf, 4, 5)))  permet au compilateur de verifier l'argument chaine avec les suivants comme avec printf, et afficher des warning si les types ne correspondent pas (genre un %s avec un int)
void LCD_printf(Uint8 line, bool_e switch_on_menu, bool_e log_on_sd, char * chaine, ...) __attribute__((format (printf, 4, 5)));

/*
 * Redonne le control � l'�cran
 */
void LCD_free_control(void);

void sprintf_line(Uint8 line, const char * format, ...);

/*
 * Fonctions d'action des boutons
 */
void LCD_button_down(void);
void LCD_button_up(void);
void LCD_button_ok(void);
void LCD_button_set(void);

#endif	/* LCD_INTERFACE_H */

