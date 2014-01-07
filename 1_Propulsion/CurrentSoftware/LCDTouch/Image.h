/**
 * \file Image.h
 * \brief gestion de l'image en RVB_8bits.
 * \author Amaury.Gaudin Edouard.Thyebaut
 * \version 1
 * \date  26 nov. 2013
 */

#ifndef IMAGE_H_
#define IMAGE_H_

#include "../QS/QS_all.h"

#ifndef IMAGE_C_
extern uint8_t ram_tableauImage[320*200];
#endif
const uint8_t * get_tableauImage(void);

void image_Init(void);

void Display_Tableau(uint32_t index);

void LCD_Affiche_Image(void);

#endif /* IMAGE_H_ */
