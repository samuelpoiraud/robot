/*
 *	Club Robot ESEO 2016 - 2017
 *
 *	Fichier : xpt2046.h
 *	Package : IHM
 *	Description : Driver du XPT2046
 *	Auteur : Arnaud Guilmet
 *  Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100929
 */

/** ----------------  Defines possibles  --------------------
 *	IRQ_TOUCH					: GPIO de lecture de l'interruption
 *	USE_IRQ_TOUCH_VALIDATION	: Utiliser la broche d'interruption de tactile pour valider les données
 *	LCD_CS_TOUCH				: GPIO d'écriture du chip select
 *
 *	TODO : Gestion de IRQPen en interruption
 */

#ifndef MIDDLEWARE_H_
	#define MIDDLEWARE_H_
	#include "../QS/QS_all.h"
	#include "../QS/QS_lcd_over_uart.h"

	typedef Uint8 objectId_t;
	#define OBJECT_ID_ERROR_FULL		255

	#define ANIMATED_IMAGE_MAX_FRAME	10

	typedef struct{
		bool_e transparence;
		Uint16 colorTransparence;
		Uint32 size;
		Uint32 width;
		Uint32 height;
		const Uint16 *image;
	}imageInfo_s;

	typedef struct{
		Uint8 nbFrame;
		Uint16 speedFrame;			// 1 frame = x millisecondes
		imageInfo_s frame[ANIMATED_IMAGE_MAX_FRAME];
	}animatedImageInfo_s;

	typedef enum{
		OBJECT_HORIZONTAL_L_2_R = LCD_OBJECT_HORIZONTAL_L_2_R,
		OBJECT_HORIZONTAL_R_2_L = LCD_OBJECT_HORIZONTAL_R_2_L,
		OBJECT_VERTICAL_T_2_B = LCD_OBJECT_VERTICAL_T_2_B,
		OBJECT_VERTICAL_B_2_T = LCD_OBJECT_VERTICAL_B_2_T
	}objectOrientation_e;

	void MIDDLEWARE_init();
	void MIDDLEWARE_processMain();



	//////////////////////////////////////////////////////////////////
	//---------------------Fonction Création------------------------//
	//////////////////////////////////////////////////////////////////

	objectId_t MIDDLEWARE_addText(Sint16 x, Sint16 y, Uint16 colorText, Uint32 colorBackground, const char *text, ...)  __attribute__((format (printf, 5, 6)));

	objectId_t MIDDLEWARE_addButtonImg(Sint16 x, Sint16 y, const imageInfo_s *imageNormal, const imageInfo_s *imageLock, bool_e lockTouch, bool_e * touch);

	objectId_t MIDDLEWARE_addButton(Sint16 x, Sint16 y, Uint16 width, Uint16 height, char * text, bool_e lockTouch, bool_e *touch, Uint16 colorText, Uint16 colorButton, Uint16 colorButtonTouch, Uint32 colorBorder);

	objectId_t MIDDLEWARE_addProgressBar(Sint16 x, Sint16 y, Uint16 width, Uint16 height, objectOrientation_e orientation, Uint8 *value);

	objectId_t MIDDLEWARE_addSlider(Sint16 x, Sint16 y, Uint16 width, Uint16 height, Sint32 minValue, Sint32 maxValue, objectOrientation_e orientation, Sint32 *value);

	objectId_t MIDDLEWARE_addImage(Sint16 x, Sint16 y, const imageInfo_s *image);

	objectId_t MIDDLEWARE_addAnimatedImage(Sint16 x, Sint16 y, const animatedImageInfo_s *animatedImageInfo);

	objectId_t MIDDLEWARE_addRectangle(Sint16 x, Sint16 y, Uint16 width, Uint16 height, Uint32 colorBorder, Uint32 colorCenter);

	objectId_t MIDDLEWARE_addCircle(Sint16 x, Sint16 y, Uint16 r, Uint32 colorBorder, Uint32 colorCenter);

	objectId_t MIDDLEWARE_addLine(Sint16 x0, Sint16 y0, Sint16 x1, Sint16 y1, Uint16 color);


	//////////////////////////////////////////////////////////////////
	//---------------------Fonction Mutation------------------------//
	//////////////////////////////////////////////////////////////////

	void MIDDLEWARE_setBackground(Uint16 color);

	void MIDDLEWARE_setText(objectId_t id, const char *text, ...)   __attribute__((format (printf, 2, 3)));



	//////////////////////////////////////////////////////////////////
	//------------------Fonction Destruction------------------------//
	//////////////////////////////////////////////////////////////////

	void MIDDLEWARE_deleteObject(objectId_t id);

	void MIDDLEWARE_resetScreen();

#endif //MIDDLEWARE_H_
