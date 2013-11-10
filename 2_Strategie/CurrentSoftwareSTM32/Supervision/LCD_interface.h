/*
 *	Club Robot ESEO 2013
 *
 *	Fichier : LCD_interface.c
 *	Package : Supervision
 *	Description : implémentation de l'interface de l'écran LCD MCCOG42005A6W-BNMLWI
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
 * Met à jour l'affichage selon les evenements
 */
void LCD_Update(void);

/*
 * Permet de changer de mode d'affichage mode info<->can
 * Le mode info donne la positiondu robot, les infos balises, les strategies et des messages de lutilisateur
 */
void LCD_switch_mode(void);

/*
 * Permet de mettre à jour la position du robot : fonction à appeler à chaque broadcast position
 * Déclanche le rafraichissement de l'affichage
 */
void LCD_change_pos(Uint16 x,Uint16 y,Uint16 t);

/*
 * Fonction qui prend une chaine de caractere pour l'ecrire sur l'écran
 * Seuls les 20 premiers caracteres sont ecrits
 */
void LCD_printf(char* format);

/*
 * Ajoute un message can à la liste de can à afficher en écrasant le plus ancien
 */
void LCD_add_can(CAN_msg_t msg);

/*
 * Se base sur les switchs pour mettre à jour les infos stratégies
 */
void LCD_strat_number_update(void);


/*
 * Ecrit une ligne définie par l'utilisateur à la position demandée
 */
void LCD_free_line(char buf[20], Uint8 pos);

/*
 * Prend le control de l'écran pour afficher les messages utilisateurs
 */
void LCD_take_control(void);

/*
 * Redonne le control à l'écran
 */
void LCD_free_control(void);

#endif	/* LCD_INTERFACE_H */

