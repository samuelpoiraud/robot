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
void LCD_change_pos(Uint16 x,Uint16 y,Uint16 t);

/*
 * Fonction qui prend une chaine de caractere pour l'ecrire sur l'�cran
 * Seuls les 20 premiers caracteres sont ecrits
 */
void LCD_printf(char* format);

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
void LCD_free_line(char buf[20], Uint8 pos);

/*
 * Prend le control de l'�cran pour afficher les messages utilisateurs
 */
void LCD_take_control(void);

/*
 * Redonne le control � l'�cran
 */
void LCD_free_control(void);

#endif	/* LCD_INTERFACE_H */

