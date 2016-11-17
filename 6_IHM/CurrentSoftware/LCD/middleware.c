#include "middleware.h"
#include "low layer/xpt2046.h"
#include "low layer/ili9341.h"
#include "../QS/QS_outputlog.h"
#include "image/flecheDroite.h"

#define NB_OBJECT				30

#define OBJECT_TEXT_MAX_SIZE	50

typedef enum{
	BUTTON_STATE_ON,
	BUTTON_STATE_OFF
}buttonState_e;

typedef enum{
	BUTTON_STATE_TOUCH,
	BUTTON_STATE_NO_TOUCH
}buttonStateTouch_e;

typedef enum{
	TEXT,
	BUTTON_IMG,
	BUTTON_BASE,
	PROGRESS_BAR,
	SLIDER,
	IMAGE,
	ANIMATED_IMAGE,
	MULTI_TEXT
}objectType_e;

typedef struct{
	bool_e use;
	objectType_e type;
	bool_e toDisplay;
	bool_e toDestroy;

	union{

		struct{
			Sint16 x;
			Sint16 y;
			char text[OBJECT_TEXT_MAX_SIZE];
			Uint16 colorText;
			Uint32 colorBackground;
		}text;

		struct{
			buttonState_e state;
			buttonState_e lastState;
			buttonStateTouch_e lastStateTouch;
			Sint16 x;
			Sint16 y;
			Sint16 widthButton;
			Sint16 heightButton;
			Sint16 widthText;
			Sint16 heightText;
			char text[OBJECT_TEXT_MAX_SIZE];
			Uint16 colorText;
			Uint16 colorButton;
			Uint16 colorTouch;
			bool_e lockTouch;
			bool_e *touch;
		}buttonBase;

		struct{
			buttonState_e state;
			buttonState_e lastState;
			buttonStateTouch_e lastStateTouch;
			Sint16 x;
			Sint16 y;
			const imageInfo_s *imageNormal;
			const imageInfo_s *imageLock;
			bool_e lockTouch;
			bool_e *touch;
		}buttonImg;

		struct{
			Sint16 x;
			Sint16 y;
			Sint16 width;
			Sint16 height;
			objectOrientation_e orientation;
			bool_e refreshBack;
			Uint8 *value;
			Uint8 lastValue;
		}progressBar;

		struct{
			Sint16 x;
			Sint16 y;
			Sint16 width;
			Sint16 height;
			Sint32 minValue;
			Sint32 maxValue;
			Sint32 *value;
			objectOrientation_e orientation;
			Sint32 realValue;
			Sint32 lastRealValue;
		}slider;

		struct{
			Sint16 x;
			Sint16 y;
			const imageInfo_s *image;
		}image;

		struct{
			Sint16 x;
			Sint16 y;
			Uint8 actualFrame;
			Uint8 lastFrame;
			time32_t timeToRefresh;
			const animatedImageInfo_s *animatedImageInfo;
		}animatedImage;

	}objectData;
}object_s;

typedef struct{
	bool_e toDisplay;
	Uint16 color;
}background_s;

static volatile object_s objectTab[NB_OBJECT] = {0};
static volatile background_s background;

static bool_e MIDDLEWARE_objectTouch(Uint16 xT, Uint16 yT, Uint16 x, Uint16 y, Uint16 width, Uint16 height);
static void MIDDLEWARE_checkObjectTouch(bool_e touch, Sint16 x, Sint16 y);
static void MIDDLEWARE_checkRebuildObject();
static void MIDDLEWARE_rebuildObject();
static void MIDDLEWARE_checkDestroyObject();

void MIDDLEWARE_init(){
	XPT2046_init();
	ILI9341_init();

	MIDDLEWARE_resetScreen();
}

void MIDDLEWARE_processMain(){

	Sint16 x, y;

	XPT2046_setConfig();
	bool_e touch = XPT2046_getAverageCoordinates(&x, &y, 3, XPT2046_COORDINATE_SCREEN_RELATIVE);

	// Check des zones touchés
	MIDDLEWARE_checkObjectTouch(touch, x, y);

	ILI9341_setConfig();

	// Check destruction d'objets
	MIDDLEWARE_checkDestroyObject();

	// Check des objets à réafficher
	MIDDLEWARE_checkRebuildObject();

	// Refresh background
	if(background.toDisplay){
		ILI9341_fill(background.color);
		background.toDisplay = FALSE;
	}

	// Mise à jours graphique des objets
	MIDDLEWARE_rebuildObject();
}

//////////////////////////////////////////////////////////////////
//-------------------------Fonction Main------------------------//
//////////////////////////////////////////////////////////////////

static void MIDDLEWARE_checkObjectTouch(bool_e touch, Sint16 x, Sint16 y){
	Uint8 i;
	for(i=0;i<NB_OBJECT;i++){
		if(objectTab[i].use){
			switch(objectTab[i].type){

				case BUTTON_BASE:
					if(objectTab[i].objectData.buttonBase.lockTouch){
						if(touch && MIDDLEWARE_objectTouch(x, y, objectTab[i].objectData.buttonBase.x, objectTab[i].objectData.buttonBase.y, objectTab[i].objectData.buttonBase.widthButton, objectTab[i].objectData.buttonBase.heightButton)){

							if(objectTab[i].objectData.buttonBase.lastStateTouch != BUTTON_STATE_TOUCH){

								if(objectTab[i].objectData.buttonBase.state == BUTTON_STATE_ON){
									objectTab[i].objectData.buttonBase.state = BUTTON_STATE_OFF;
									*(objectTab[i].objectData.buttonBase.touch) = FALSE;
								}else{
									objectTab[i].objectData.buttonBase.state = BUTTON_STATE_ON;
									*(objectTab[i].objectData.buttonBase.touch) = TRUE;
								}
								objectTab[i].objectData.buttonBase.lastStateTouch = BUTTON_STATE_TOUCH;
							}
						}else{
							objectTab[i].objectData.buttonBase.lastStateTouch = BUTTON_STATE_NO_TOUCH;
						}
					}else{
						if(touch && MIDDLEWARE_objectTouch(x, y, objectTab[i].objectData.buttonBase.x, objectTab[i].objectData.buttonBase.y, objectTab[i].objectData.buttonBase.widthButton, objectTab[i].objectData.buttonBase.heightButton)){

							objectTab[i].objectData.buttonBase.state = BUTTON_STATE_ON;
							*(objectTab[i].objectData.buttonBase.touch) = TRUE;

						}else{

							objectTab[i].objectData.buttonBase.state = BUTTON_STATE_OFF;
							*(objectTab[i].objectData.buttonBase.touch) = FALSE;

						}
					}
					break;

				case BUTTON_IMG:
					if(objectTab[i].objectData.buttonImg.lockTouch){
						if(touch && MIDDLEWARE_objectTouch(x, y, objectTab[i].objectData.buttonImg.x, objectTab[i].objectData.buttonImg.y, objectTab[i].objectData.buttonImg.imageNormal->width, objectTab[i].objectData.buttonImg.imageNormal->height)){

							if(objectTab[i].objectData.buttonImg.lastStateTouch != BUTTON_STATE_TOUCH){

								if(objectTab[i].objectData.buttonImg.state == BUTTON_STATE_ON){
									objectTab[i].objectData.buttonImg.state = BUTTON_STATE_OFF;
									*(objectTab[i].objectData.buttonImg.touch) = FALSE;
								}else{
									objectTab[i].objectData.buttonImg.state = BUTTON_STATE_ON;
									*(objectTab[i].objectData.buttonImg.touch) = TRUE;
								}
								objectTab[i].objectData.buttonImg.lastStateTouch = BUTTON_STATE_TOUCH;
							}
						}else{
							objectTab[i].objectData.buttonImg.lastStateTouch = BUTTON_STATE_NO_TOUCH;
						}
					}else{
						if(touch && MIDDLEWARE_objectTouch(x, y, objectTab[i].objectData.buttonImg.x, objectTab[i].objectData.buttonImg.y, objectTab[i].objectData.buttonImg.imageNormal->width, objectTab[i].objectData.buttonImg.imageNormal->height)){

							objectTab[i].objectData.buttonImg.state = BUTTON_STATE_ON;
							*(objectTab[i].objectData.buttonImg.touch) = TRUE;

						}else{

							objectTab[i].objectData.buttonImg.state = BUTTON_STATE_OFF;
							*(objectTab[i].objectData.buttonImg.touch) = FALSE;

						}
					}
					break;

				case SLIDER:
					if(touch && MIDDLEWARE_objectTouch(x, y, objectTab[i].objectData.slider.x, objectTab[i].objectData.slider.y, objectTab[i].objectData.slider.width, objectTab[i].objectData.slider.height)){

						if(objectTab[i].objectData.slider.orientation == OBJECT_HORIZONTAL_L_2_R || objectTab[i].objectData.slider.orientation == OBJECT_HORIZONTAL_R_2_L)
							objectTab[i].objectData.slider.realValue = (x - objectTab[i].objectData.slider.x) * (objectTab[i].objectData.slider.maxValue - objectTab[i].objectData.slider.minValue) / (objectTab[i].objectData.slider.width);
						else
							objectTab[i].objectData.slider.realValue = (y - objectTab[i].objectData.slider.y) * (objectTab[i].objectData.slider.maxValue - objectTab[i].objectData.slider.minValue) / (objectTab[i].objectData.slider.height);

						if(objectTab[i].objectData.slider.orientation == OBJECT_HORIZONTAL_R_2_L || objectTab[i].objectData.slider.orientation == OBJECT_VERTICAL_B_2_T)
							*(objectTab[i].objectData.slider.value) = objectTab[i].objectData.slider.maxValue - objectTab[i].objectData.slider.realValue;
						else
							*(objectTab[i].objectData.slider.value) = objectTab[i].objectData.slider.realValue;
					}
					break;

				default:
					break;
			}
		}
	}
}

static void MIDDLEWARE_checkDestroyObject(){
	Uint8 i;
	for(i=0;i<NB_OBJECT;i++){
		if(objectTab[i].use && objectTab[i].toDestroy){
			switch(objectTab[i].type){

				case TEXT:{
					Uint16 width, height;
					ILI9341_getStringSize((char *)objectTab[i].objectData.buttonBase.text, &Font_7x10, &width, &height);
					ILI9341_drawFilledRectangle(objectTab[i].objectData.buttonBase.x,
												objectTab[i].objectData.buttonBase.y,
												objectTab[i].objectData.buttonBase.x + width,
												objectTab[i].objectData.buttonBase.y + height,
												background.color);
					}break;

				case BUTTON_BASE:
					ILI9341_drawFilledRectangle(objectTab[i].objectData.buttonBase.x,
												objectTab[i].objectData.buttonBase.y,
												objectTab[i].objectData.buttonBase.x + objectTab[i].objectData.buttonBase.widthButton,
												objectTab[i].objectData.buttonBase.y + objectTab[i].objectData.buttonBase.heightButton,
												background.color);
					break;

				case BUTTON_IMG:
					ILI9341_drawFilledRectangle(objectTab[i].objectData.buttonImg.x,
												objectTab[i].objectData.buttonImg.y,
												objectTab[i].objectData.buttonImg.x + objectTab[i].objectData.buttonImg.imageNormal->width,
												objectTab[i].objectData.buttonImg.y + objectTab[i].objectData.buttonImg.imageNormal->height,
												background.color);
					break;

				case PROGRESS_BAR:
					ILI9341_drawFilledRectangle(objectTab[i].objectData.progressBar.x,
												objectTab[i].objectData.progressBar.y,
												objectTab[i].objectData.progressBar.x + objectTab[i].objectData.progressBar.width,
												objectTab[i].objectData.progressBar.y + objectTab[i].objectData.progressBar.height,
												background.color);
					break;

				case SLIDER:
					ILI9341_drawFilledRectangle(objectTab[i].objectData.slider.x,
												objectTab[i].objectData.slider.y,
												objectTab[i].objectData.slider.x + objectTab[i].objectData.slider.width,
												objectTab[i].objectData.slider.y + objectTab[i].objectData.slider.height,
												background.color);
					break;

				case IMAGE:
					ILI9341_drawFilledRectangle(objectTab[i].objectData.image.x,
												objectTab[i].objectData.image.y,
												objectTab[i].objectData.image.x + objectTab[i].objectData.image.image->width,
												objectTab[i].objectData.image.y + objectTab[i].objectData.image.image->height,
												background.color);
					break;

				case ANIMATED_IMAGE:
					ILI9341_drawFilledRectangle(objectTab[i].objectData.animatedImage.x,
												objectTab[i].objectData.animatedImage.y,
												objectTab[i].objectData.animatedImage.x + objectTab[i].objectData.animatedImage.animatedImageInfo->frame[objectTab[i].objectData.animatedImage.actualFrame].width,
												objectTab[i].objectData.animatedImage.y + objectTab[i].objectData.animatedImage.animatedImageInfo->frame[objectTab[i].objectData.animatedImage.actualFrame].height,
												background.color);
					break;

				default:
					break;
			}
			objectTab[i].use = FALSE;
		}
	}
}

static void MIDDLEWARE_checkRebuildObject(){
	Uint8 i;
	for(i=0;i<NB_OBJECT;i++){
		if(objectTab[i].use){
			if(background.toDisplay){
				switch(objectTab[i].type){
					case PROGRESS_BAR:
						objectTab[i].objectData.progressBar.refreshBack = TRUE;
						break;

					default:
						break;
				}
				objectTab[i].toDisplay = TRUE;
			}else{
				switch(objectTab[i].type){

					case BUTTON_BASE:
						if(objectTab[i].objectData.buttonBase.lastState != objectTab[i].objectData.buttonBase.state){
							objectTab[i].toDisplay = TRUE;
						}
						objectTab[i].objectData.buttonBase.lastState = objectTab[i].objectData.buttonBase.state;
						break;

					case BUTTON_IMG:
						if(objectTab[i].objectData.buttonImg.lastState != objectTab[i].objectData.buttonImg.state){
							objectTab[i].toDisplay = TRUE;
						}
						objectTab[i].objectData.buttonImg.lastState = objectTab[i].objectData.buttonImg.state;
						break;

					case PROGRESS_BAR:
						if(objectTab[i].objectData.progressBar.lastValue != *(objectTab[i].objectData.progressBar.value)){
							if(objectTab[i].objectData.progressBar.lastValue > *(objectTab[i].objectData.progressBar.value))
									objectTab[i].objectData.progressBar.refreshBack = TRUE;
							objectTab[i].objectData.progressBar.lastValue = *(objectTab[i].objectData.progressBar.value);
							objectTab[i].toDisplay = TRUE;
						}
						break;

					case SLIDER:
						if(objectTab[i].objectData.slider.realValue != objectTab[i].objectData.slider.lastRealValue){
							objectTab[i].objectData.slider.lastRealValue = objectTab[i].objectData.slider.realValue;
							objectTab[i].toDisplay = TRUE;
						}
						break;

					case ANIMATED_IMAGE:
						if(global.absolute_time >= objectTab[i].objectData.animatedImage.timeToRefresh)
							objectTab[i].toDisplay = TRUE;
						break;

					default:
						break;
				}
			}
		}
	}
}

static void MIDDLEWARE_rebuildObject(){
	Uint8 i;
	for(i=0;i<NB_OBJECT;i++){
		if(objectTab[i].use && objectTab[i].toDisplay){
			switch(objectTab[i].type){
				case TEXT:{
					ILI9341_printf(objectTab[i].objectData.text.x,
									objectTab[i].objectData.text.y,
									&Font_7x10,
									objectTab[i].objectData.text.colorText,
									((objectTab[i].objectData.text.colorBackground == ILI9341_TRANSPARENT)? background.color : objectTab[i].objectData.text.colorBackground),
									"%s", objectTab[i].objectData.text.text);
				}break;

				case BUTTON_BASE:{

					ILI9341_drawFilledRectangle(objectTab[i].objectData.buttonBase.x,
												objectTab[i].objectData.buttonBase.y,
												objectTab[i].objectData.buttonBase.x + objectTab[i].objectData.buttonBase.widthButton,
												objectTab[i].objectData.buttonBase.y + objectTab[i].objectData.buttonBase.heightButton,
												(objectTab[i].objectData.buttonBase.state == BUTTON_STATE_ON)? objectTab[i].objectData.buttonBase.colorTouch : objectTab[i].objectData.buttonBase.colorButton
												);

					ILI9341_printf(objectTab[i].objectData.buttonBase.x + objectTab[i].objectData.buttonBase.widthButton/2 - objectTab[i].objectData.buttonBase.widthText/2,
									objectTab[i].objectData.buttonBase.y + objectTab[i].objectData.buttonBase.heightButton/2 - objectTab[i].objectData.buttonBase.heightText/2,
									&Font_7x10,
									objectTab[i].objectData.buttonBase.colorText,
									ILI9341_TRANSPARENT,
									"%s", objectTab[i].objectData.buttonBase.text);
					}break;

				case BUTTON_IMG:{
					const imageInfo_s *imageInfo = (objectTab[i].objectData.buttonImg.state == BUTTON_STATE_ON)? objectTab[i].objectData.buttonImg.imageLock : objectTab[i].objectData.buttonImg.imageNormal;

					if(objectTab[i].objectData.buttonImg.imageNormal->transparence){
						ILI9341_putImageWithTransparence(objectTab[i].objectData.buttonImg.x,
														objectTab[i].objectData.buttonImg.y,
														imageInfo->width,
														imageInfo->height,
														imageInfo->image,
														imageInfo->colorTransparence,
														imageInfo->size);
					}else{
						ILI9341_putImage(objectTab[i].objectData.buttonImg.x,
														objectTab[i].objectData.buttonImg.y,
														imageInfo->width,
														imageInfo->height,
														imageInfo->image,
														imageInfo->size);
					}

					}break;

				case PROGRESS_BAR:{
					if(objectTab[i].objectData.progressBar.refreshBack){
						ILI9341_drawFilledRectangle(objectTab[i].objectData.progressBar.x,
													objectTab[i].objectData.progressBar.y,
													objectTab[i].objectData.progressBar.x + objectTab[i].objectData.progressBar.width,
													objectTab[i].objectData.progressBar.y + objectTab[i].objectData.progressBar.height,
													ILI9341_COLOR_GRAY);
						objectTab[i].objectData.progressBar.refreshBack = FALSE;
					}

					if(objectTab[i].objectData.progressBar.orientation == OBJECT_HORIZONTAL_L_2_R){

						Uint16 width = objectTab[i].objectData.progressBar.lastValue * (objectTab[i].objectData.progressBar.width - 4) / 100;

						ILI9341_drawFilledRectangle(objectTab[i].objectData.progressBar.x + 2,
													objectTab[i].objectData.progressBar.y + 2,
													objectTab[i].objectData.progressBar.x + 2 + width,
													objectTab[i].objectData.progressBar.y + objectTab[i].objectData.progressBar.height - 2,
													ILI9341_COLOR_GREEN);

					}else if(objectTab[i].objectData.progressBar.orientation == OBJECT_HORIZONTAL_R_2_L){

						Uint16 width = objectTab[i].objectData.progressBar.lastValue * (objectTab[i].objectData.progressBar.width - 4) / 100;

						ILI9341_drawFilledRectangle(objectTab[i].objectData.progressBar.x + objectTab[i].objectData.progressBar.width - 2 - width,
													objectTab[i].objectData.progressBar.y + 2,
													objectTab[i].objectData.progressBar.x + objectTab[i].objectData.progressBar.width - 2,
													objectTab[i].objectData.progressBar.y + objectTab[i].objectData.progressBar.height - 2,
													ILI9341_COLOR_GREEN);

					}else if(objectTab[i].objectData.progressBar.orientation == OBJECT_VERTICAL_T_2_B){

						Uint16 height = objectTab[i].objectData.progressBar.lastValue * (objectTab[i].objectData.progressBar.height - 4) / 100;

						ILI9341_drawFilledRectangle(objectTab[i].objectData.progressBar.x + 2,
													objectTab[i].objectData.progressBar.y + 2,
													objectTab[i].objectData.progressBar.x + objectTab[i].objectData.progressBar.width - 2,
													objectTab[i].objectData.progressBar.y + 2 + height,
													ILI9341_COLOR_GREEN);
					}else if(objectTab[i].objectData.progressBar.orientation == OBJECT_VERTICAL_B_2_T){

						Uint16 height = objectTab[i].objectData.progressBar.lastValue * (objectTab[i].objectData.progressBar.height - 4) / 100;

						ILI9341_drawFilledRectangle(objectTab[i].objectData.progressBar.x + 2,
													objectTab[i].objectData.progressBar.y + objectTab[i].objectData.progressBar.height - 2 - height,
													objectTab[i].objectData.progressBar.x + objectTab[i].objectData.progressBar.width - 2,
													objectTab[i].objectData.progressBar.y + objectTab[i].objectData.progressBar.height - 2,
													ILI9341_COLOR_GREEN);
					}
					}break;

				case SLIDER:{

					Uint16 positionX, positionY;
					ILI9341_drawFilledRectangle(objectTab[i].objectData.slider.x,
												objectTab[i].objectData.slider.y,
												objectTab[i].objectData.slider.x + objectTab[i].objectData.slider.width,
												objectTab[i].objectData.slider.y + objectTab[i].objectData.slider.height,
												ILI9341_COLOR_GRAY);

					if(objectTab[i].objectData.slider.orientation == OBJECT_HORIZONTAL_L_2_R || objectTab[i].objectData.slider.orientation == OBJECT_HORIZONTAL_R_2_L){

						positionX = objectTab[i].objectData.slider.x + (objectTab[i].objectData.slider.realValue * objectTab[i].objectData.slider.width) / (objectTab[i].objectData.slider.maxValue - objectTab[i].objectData.slider.minValue);
						positionY = objectTab[i].objectData.slider.y + objectTab[i].objectData.slider.height / 2;

					}else if(objectTab[i].objectData.progressBar.orientation == OBJECT_VERTICAL_T_2_B || objectTab[i].objectData.slider.orientation == OBJECT_VERTICAL_B_2_T){

						positionX = objectTab[i].objectData.slider.x + objectTab[i].objectData.slider.width / 2;
						positionY = objectTab[i].objectData.slider.y + (objectTab[i].objectData.slider.realValue * objectTab[i].objectData.slider.height) / (objectTab[i].objectData.slider.maxValue - objectTab[i].objectData.slider.minValue);
					}

					ILI9341_drawFilledCircle(positionX-2, positionY, 5, ILI9341_COLOR_RED);
					}break;

				case IMAGE:
					if(objectTab[i].objectData.image.image->transparence){
						ILI9341_putImageWithTransparence(objectTab[i].objectData.image.x,
														objectTab[i].objectData.image.y,
														objectTab[i].objectData.image.image->width,
														objectTab[i].objectData.image.image->height,
														objectTab[i].objectData.image.image->image,
														objectTab[i].objectData.image.image->colorTransparence,
														objectTab[i].objectData.image.image->size);
					}else{
						ILI9341_putImage(objectTab[i].objectData.image.x,
														objectTab[i].objectData.image.y,
														objectTab[i].objectData.image.image->width,
														objectTab[i].objectData.image.image->height,
														objectTab[i].objectData.image.image->image,
														objectTab[i].objectData.image.image->size);
					}

					break;

				case ANIMATED_IMAGE:{
					const animatedImageInfo_s *animatedImageInfo = objectTab[i].objectData.animatedImage.animatedImageInfo;
					Uint8 actualFrame = objectTab[i].objectData.animatedImage.actualFrame;
					Sint8 lastFrame = objectTab[i].objectData.animatedImage.lastFrame;

					if(lastFrame != 255){
						ILI9341_putColorInvertedImage(objectTab[i].objectData.animatedImage.x,
														objectTab[i].objectData.animatedImage.y,
														animatedImageInfo->frame[lastFrame].width,
														animatedImageInfo->frame[lastFrame].height,
														background.color,
														animatedImageInfo->frame[lastFrame].image,
														animatedImageInfo->frame[lastFrame].colorTransparence,
														animatedImageInfo->frame[lastFrame].size);
					}

					if(animatedImageInfo->frame[actualFrame].transparence){
						ILI9341_putImageWithTransparence(objectTab[i].objectData.animatedImage.x,
														objectTab[i].objectData.animatedImage.y,
														animatedImageInfo->frame[actualFrame].width,
														animatedImageInfo->frame[actualFrame].height,
														animatedImageInfo->frame[actualFrame].image,
														animatedImageInfo->frame[actualFrame].colorTransparence,
														animatedImageInfo->frame[actualFrame].size);
					}else{
						ILI9341_putImage(objectTab[i].objectData.animatedImage.x,
														objectTab[i].objectData.animatedImage.y,
														animatedImageInfo->frame[actualFrame].width,
														animatedImageInfo->frame[actualFrame].height,
														animatedImageInfo->frame[actualFrame].image,
														animatedImageInfo->frame[actualFrame].size);
					}

					objectTab[i].objectData.animatedImage.lastFrame = objectTab[i].objectData.animatedImage.actualFrame;

					if(actualFrame >= animatedImageInfo->nbFrame - 1  || actualFrame == ANIMATED_IMAGE_MAX_FRAME)
						objectTab[i].objectData.animatedImage.actualFrame = 0;
					else
						(objectTab[i].objectData.animatedImage.actualFrame)++;

					objectTab[i].objectData.animatedImage.timeToRefresh = global.absolute_time + objectTab[i].objectData.animatedImage.animatedImageInfo->speedFrame;

					}break;

				default:
					break;
			}

			objectTab[i].toDisplay = FALSE;
		}
	}
}

static bool_e MIDDLEWARE_objectTouch(Uint16 xT, Uint16 yT, Uint16 x, Uint16 y, Uint16 width, Uint16 height){
	return 	(xT >= x && xT <= (x + width) && yT >= y && yT <= (y + height));
}

//////////////////////////////////////////////////////////////////
//---------------------Fonction Mutation------------------------//
//////////////////////////////////////////////////////////////////

void MIDDLEWARE_setBackground(Uint16 color){
	background.color = color;
	background.toDisplay = TRUE;
}

void MIDDLEWARE_setText(objectId_t id, char * text){
	assert(id < NB_OBJECT);
	assert(objectTab[id].use);

	switch(objectTab[id].type){
		case TEXT :
			strncpy((char *)objectTab[id].objectData.text.text, text, OBJECT_TEXT_MAX_SIZE);
			break;

		case BUTTON_IMG :
			break;

		case BUTTON_BASE :{
				strncpy((char *)objectTab[id].objectData.buttonBase.text, text, OBJECT_TEXT_MAX_SIZE);
				Uint16 widthText, heightText;
				ILI9341_getStringSize((char *)objectTab[id].objectData.buttonBase.text, &Font_7x10, &widthText, &heightText);
				objectTab[id].objectData.buttonBase.widthText = widthText;
				objectTab[id].objectData.buttonBase.heightText = heightText;
				objectTab[id].objectData.buttonBase.widthButton = widthText + 6;
				objectTab[id].objectData.buttonBase.heightButton = heightText + 6;
			}break;

		case PROGRESS_BAR :
			break;

		case SLIDER :
			break;

		case IMAGE :
			break;

		case ANIMATED_IMAGE :
			break;

		default :
			break;
	}
	objectTab[id].toDisplay = TRUE;
}

//////////////////////////////////////////////////////////////////
//---------------------Fonction Création------------------------//
//////////////////////////////////////////////////////////////////

objectId_t MIDDLEWARE_addText(Sint16 x, Sint16 y, char * text, Uint16 colorText, Uint32 colorBackground){
	assert(text != NULL);

	Uint8 i, idFound = OBJECT_ID_ERROR_FULL;
	for(i=0;i<NB_OBJECT && idFound == OBJECT_ID_ERROR_FULL;i++){
		if(objectTab[i].use == FALSE){
			idFound = i;
		}
	}

	if(idFound == OBJECT_ID_ERROR_FULL)
		return OBJECT_ID_ERROR_FULL;

	objectTab[idFound].type = TEXT;
	objectTab[idFound].toDisplay = TRUE;
	objectTab[idFound].toDestroy = FALSE;
	objectTab[idFound].objectData.text.x = x;
	objectTab[idFound].objectData.text.y = y;
	objectTab[idFound].objectData.text.colorText = colorText;
	objectTab[idFound].objectData.text.colorBackground = colorBackground;
	strncpy((char *)objectTab[idFound].objectData.text.text, text, OBJECT_TEXT_MAX_SIZE);
	objectTab[idFound].use = TRUE;

	return idFound;
}

objectId_t MIDDLEWARE_addButtonImg(Sint16 x, Sint16 y, const imageInfo_s *imageNormal, const imageInfo_s *imageLock, bool_e lockTouch, bool_e * touch){
	assert(touch != NULL);

	Uint8 i, idFound = OBJECT_ID_ERROR_FULL;
	for(i=0;i<NB_OBJECT && idFound == OBJECT_ID_ERROR_FULL;i++){
		if(objectTab[i].use == FALSE){
			idFound = i;
		}
	}

	if(idFound == OBJECT_ID_ERROR_FULL)
		return OBJECT_ID_ERROR_FULL;

	*touch = FALSE;

	objectTab[idFound].type = BUTTON_IMG;
	objectTab[idFound].toDisplay = TRUE;
	objectTab[idFound].toDestroy = FALSE;
	objectTab[idFound].objectData.buttonImg.state = BUTTON_STATE_OFF;
	objectTab[idFound].objectData.buttonImg.lastState = BUTTON_STATE_OFF;
	objectTab[idFound].objectData.buttonImg.lastStateTouch = BUTTON_STATE_NO_TOUCH;
	objectTab[idFound].objectData.buttonImg.x = x;
	objectTab[idFound].objectData.buttonImg.y = y;
	objectTab[idFound].objectData.buttonImg.imageNormal = imageNormal;
	objectTab[idFound].objectData.buttonImg.imageLock = imageLock;
	objectTab[idFound].objectData.buttonImg.lockTouch = lockTouch;
	objectTab[idFound].objectData.buttonImg.touch = touch;
	objectTab[idFound].use = TRUE;

	return idFound;
}

objectId_t MIDDLEWARE_addButton(Sint16 x, Sint16 y, Sint16 width, Sint16 height, char * text, bool_e lockTouch, bool_e *touch, Uint16 colorText, Uint16 colorButton, Uint16 colorButtonTouch){
	assert(touch != NULL);
	assert(text != NULL);

	Uint8 i, idFound = OBJECT_ID_ERROR_FULL;
	for(i=0;i<NB_OBJECT && idFound == OBJECT_ID_ERROR_FULL;i++){
		if(objectTab[i].use == FALSE){
			idFound = i;
		}
	}

	if(idFound == OBJECT_ID_ERROR_FULL)
		return OBJECT_ID_ERROR_FULL;

	*touch = FALSE;

	objectTab[idFound].type = BUTTON_BASE;
	objectTab[idFound].toDisplay = TRUE;
	objectTab[idFound].toDestroy = FALSE;
	objectTab[idFound].objectData.buttonBase.state = BUTTON_STATE_OFF;
	objectTab[idFound].objectData.buttonBase.lastState = BUTTON_STATE_OFF;
	objectTab[idFound].objectData.buttonBase.lastStateTouch = BUTTON_STATE_NO_TOUCH;
	objectTab[idFound].objectData.buttonBase.x = x;
	objectTab[idFound].objectData.buttonBase.y = y;
	strncpy((char *)objectTab[idFound].objectData.buttonBase.text, text, OBJECT_TEXT_MAX_SIZE);
	objectTab[idFound].objectData.buttonBase.lockTouch = lockTouch;
	objectTab[idFound].objectData.buttonBase.colorText = colorText;
	objectTab[idFound].objectData.buttonBase.colorButton = colorButton;
	objectTab[idFound].objectData.buttonBase.colorTouch = colorButtonTouch;
	objectTab[idFound].objectData.buttonBase.touch = touch;

	Uint16 widthText, heightText;
	ILI9341_getStringSize((char *)objectTab[idFound].objectData.buttonBase.text, &Font_7x10, &widthText, &heightText);
	objectTab[idFound].objectData.buttonBase.widthText = widthText;
	objectTab[idFound].objectData.buttonBase.heightText = heightText;

	if(width > 0)
		objectTab[idFound].objectData.buttonBase.widthButton = width;
	else
		objectTab[idFound].objectData.buttonBase.widthButton = widthText + 6;

	if(height > 0)
		objectTab[idFound].objectData.buttonBase.heightButton = height;
	else
		objectTab[idFound].objectData.buttonBase.heightButton = heightText + 6;


	objectTab[idFound].use = TRUE;

	return idFound;
}

objectId_t MIDDLEWARE_addProgressBar(Sint16 x, Sint16 y, Sint16 width, Sint16 height, objectOrientation_e orientation, Uint8 *value){
	assert(value != NULL);

	Uint8 i, idFound = OBJECT_ID_ERROR_FULL;
	for(i=0;i<NB_OBJECT && idFound == OBJECT_ID_ERROR_FULL;i++){
		if(objectTab[i].use == FALSE){
			idFound = i;
		}
	}

	if(idFound == OBJECT_ID_ERROR_FULL)
		return OBJECT_ID_ERROR_FULL;

	objectTab[idFound].type = PROGRESS_BAR;
	objectTab[idFound].toDisplay = TRUE;
	objectTab[idFound].toDestroy = FALSE;
	objectTab[idFound].objectData.progressBar.x = x;
	objectTab[idFound].objectData.progressBar.y = y;
	objectTab[idFound].objectData.progressBar.width = width;
	objectTab[idFound].objectData.progressBar.height = height;
	objectTab[idFound].objectData.progressBar.orientation = orientation;
	objectTab[idFound].objectData.progressBar.refreshBack = TRUE;
	objectTab[idFound].objectData.progressBar.value = value;
	objectTab[idFound].objectData.progressBar.lastValue = *value;
	objectTab[idFound].use = TRUE;

	return idFound;
}

objectId_t MIDDLEWARE_addSlider(Sint16 x, Sint16 y, Sint16 width, Sint16 height, Sint32 minValue, Sint32 maxValue, objectOrientation_e orientation, Sint32 *value){
	assert(value != NULL);
	assert(minValue != maxValue);

	Uint8 i, idFound = OBJECT_ID_ERROR_FULL;
	for(i=0;i<NB_OBJECT && idFound == OBJECT_ID_ERROR_FULL;i++){
		if(objectTab[i].use == FALSE){
			idFound = i;
		}
	}

	if(idFound == OBJECT_ID_ERROR_FULL)
		return OBJECT_ID_ERROR_FULL;

	objectTab[idFound].type = SLIDER;
	objectTab[idFound].toDisplay = TRUE;
	objectTab[idFound].toDestroy = FALSE;
	objectTab[idFound].objectData.slider.x = x;
	objectTab[idFound].objectData.slider.y = y;
	objectTab[idFound].objectData.slider.width = width;
	objectTab[idFound].objectData.slider.height = height;
	objectTab[idFound].objectData.slider.minValue = minValue;
	objectTab[idFound].objectData.slider.maxValue = maxValue;
	objectTab[idFound].objectData.slider.orientation = orientation;
	objectTab[idFound].objectData.slider.value = value;
	if(*value < minValue || *value > maxValue)
		objectTab[idFound].objectData.slider.realValue = (maxValue - minValue)/2;
	else
		objectTab[idFound].objectData.slider.realValue = *value;
	objectTab[idFound].objectData.slider.lastRealValue = objectTab[idFound].objectData.slider.realValue;
	objectTab[idFound].use = TRUE;

	return idFound;
}

objectId_t MIDDLEWARE_addImage(Sint16 x, Sint16 y, const imageInfo_s *image){
	assert(image != NULL);

	Uint8 i, idFound = OBJECT_ID_ERROR_FULL;
	for(i=0;i<NB_OBJECT && idFound == OBJECT_ID_ERROR_FULL;i++){
		if(objectTab[i].use == FALSE){
			idFound = i;
		}
	}

	if(idFound == OBJECT_ID_ERROR_FULL)
		return OBJECT_ID_ERROR_FULL;

	objectTab[idFound].type = ANIMATED_IMAGE;
	objectTab[idFound].toDisplay = TRUE;
	objectTab[idFound].toDestroy = FALSE;
	objectTab[idFound].objectData.image.x = x;
	objectTab[idFound].objectData.image.y = y;
	objectTab[idFound].objectData.image.image = image;
	objectTab[idFound].use = TRUE;

	return idFound;
}

objectId_t MIDDLEWARE_addAnimatedImage(Sint16 x, Sint16 y, const animatedImageInfo_s *animatedImageInfo){
	assert(animatedImageInfo != NULL);

	Uint8 i, idFound = OBJECT_ID_ERROR_FULL;
	for(i=0;i<NB_OBJECT && idFound == OBJECT_ID_ERROR_FULL;i++){
		if(objectTab[i].use == FALSE){
			idFound = i;
		}
	}

	if(idFound == OBJECT_ID_ERROR_FULL)
		return OBJECT_ID_ERROR_FULL;

	objectTab[idFound].type = ANIMATED_IMAGE;
	objectTab[idFound].toDisplay = TRUE;
	objectTab[idFound].toDestroy = FALSE;
	objectTab[idFound].objectData.animatedImage.x = x;
	objectTab[idFound].objectData.animatedImage.y = y;
	objectTab[idFound].objectData.animatedImage.animatedImageInfo = animatedImageInfo;
	objectTab[idFound].objectData.animatedImage.actualFrame = 0;
	objectTab[idFound].objectData.animatedImage.lastFrame = 255;
	objectTab[idFound].objectData.animatedImage.timeToRefresh = 0;
	objectTab[idFound].use = TRUE;

	return idFound;
}

//////////////////////////////////////////////////////////////////
//------------------Fonction Destruction------------------------//
//////////////////////////////////////////////////////////////////

void MIDDLEWARE_deleteObject(objectId_t id){
	assert(id < NB_OBJECT);
	assert(objectTab[id].use);

	objectTab[id].toDestroy = TRUE;
}

void MIDDLEWARE_resetScreen(){
	Uint8 i;
	for(i=0;i<NB_OBJECT;i++){
		objectTab[i].use = FALSE;
	}
	background.color = 0xFFFF;
	background.toDisplay = TRUE;
}


