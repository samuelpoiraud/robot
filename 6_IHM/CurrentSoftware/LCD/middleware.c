#include "middleware.h"
#include "low layer/xpt2046.h"
#include "low layer/ili9341.h"
#include "../QS/QS_outputlog.h"
#include "image/flecheDroite.h"

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
	MULTI_TEXT,
	RECTANGLE,
	CIRCLE,
	LINE
}objectType_e;

typedef struct{
	bool_e use;
	objectType_e type;
	bool_e toDisplay;
	bool_e toErase;
	bool_e toDestroy;

	struct{
		Sint16 x;
		Sint16 y;
		Uint16 width;
		Uint16 height;
	}surfaceToErase;

	union{

		struct{
			Sint16 x;
			Sint16 y;
			Uint16 width;
			Uint16 height;
			char text[OBJECT_TEXT_MAX_SIZE];
			textFonts_e fonts;
			objectColor_e colorText;
			objectColor_e colorBackground;
		}text;

		struct{
			buttonState_e state;
			buttonState_e lastState;
			buttonStateTouch_e lastStateTouch;
			Sint16 x;
			Sint16 y;
			Uint16 widthButton;
			Uint16 heightButton;
			Uint16 widthText;
			Uint16 heightText;
			char text[OBJECT_TEXT_MAX_SIZE];
			textFonts_e fonts;
			Uint16 colorText;
			objectColor_e colorButton;
			objectColor_e colorTouch;
			objectColor_e colorBorder;
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
			Uint16 width;
			Uint16 height;
			objectOrientation_e orientation;
			bool_e refreshBack;
			Uint8 *value;
			Uint8 lastValue;
		}progressBar;

		struct{
			Sint16 x;
			Sint16 y;
			Uint16 width;
			Uint16 height;
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

		struct{
			Sint16 x;
			Sint16 y;
			Uint16 width;
			Uint16 height;
			objectColor_e colorBorder;
			objectColor_e colorCenter;
		}rectangle;

		struct{
			Sint16 x;
			Sint16 y;
			Uint16 r;
			objectColor_e colorBorder;
			objectColor_e colorCenter;
		}circle;

		struct{
			Sint16 x0;
			Sint16 y0;
			Sint16 x1;
			Sint16 y1;
			objectColor_e color;
		}line;

	}objectData;
}object_s;

typedef struct{
	bool_e toDisplay;
	Uint16 color;
}background_s;

static volatile object_s objectTab[LCD_NB_MAX_OBJECT] = {0};
static volatile background_s background;

static bool_e MIDDLEWARE_objectTouch(Uint16 xT, Uint16 yT, Uint16 x, Uint16 y, Uint16 width, Uint16 height);
static void MIDDLEWARE_checkObjectTouch(bool_e touch, Sint16 x, Sint16 y);
static void MIDDLEWARE_checkRebuildObject();
static void MIDDLEWARE_rebuildObject();
static void MIDDLEWARE_checkEraseObject();
static void MIDDLEWARE_checkDestroyObject();
static objectId_t MIDDLEWARE_newObject();
static FontDef_t* MIDDLEWARE_getFont(textFonts_e fonts);

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

	// Check effacement d'objets
	MIDDLEWARE_checkEraseObject();

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
	for(i=0;i<LCD_NB_MAX_OBJECT;i++){
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

static void MIDDLEWARE_checkEraseObject(){
	Uint8 i;
	for(i=0;i<LCD_NB_MAX_OBJECT;i++){
		if(objectTab[i].use && objectTab[i].toErase){
			switch(objectTab[i].type){

				case TEXT:
				case BUTTON_BASE:
					ILI9341_drawFilledRectangle(objectTab[i].surfaceToErase.x,
												objectTab[i].surfaceToErase.y,
												objectTab[i].surfaceToErase.x + objectTab[i].surfaceToErase.width,
												objectTab[i].surfaceToErase.y + objectTab[i].surfaceToErase.height,
												background.color);
					break;

				default:
					break;
			}
			objectTab[i].toErase = FALSE;
		}
	}
}

static void MIDDLEWARE_checkDestroyObject(){
	Uint8 i;
	for(i=0;i<LCD_NB_MAX_OBJECT;i++){
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

				case RECTANGLE:
					ILI9341_drawFilledRectangle(objectTab[i].objectData.rectangle.x,
																	objectTab[i].objectData.rectangle.y,
																	objectTab[i].objectData.rectangle.x + objectTab[i].objectData.rectangle.width,
																	objectTab[i].objectData.rectangle.y + objectTab[i].objectData.rectangle.height,
																	background.color);
					break;

				case CIRCLE:
					ILI9341_drawCircle(objectTab[i].objectData.circle.x,
										objectTab[i].objectData.circle.y,
										objectTab[i].objectData.circle.r,
										background.color);
					break;

				case LINE:
					ILI9341_drawLine(objectTab[i].objectData.line.x0,
									objectTab[i].objectData.line.y0,
									objectTab[i].objectData.line.x1,
									objectTab[i].objectData.line.y1,
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
	for(i=0;i<LCD_NB_MAX_OBJECT;i++){
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
	for(i=0;i<LCD_NB_MAX_OBJECT;i++){
		if(objectTab[i].use && objectTab[i].toDisplay){
			switch(objectTab[i].type){
				case TEXT:{
					ILI9341_printf(objectTab[i].objectData.text.x,
									objectTab[i].objectData.text.y,
									MIDDLEWARE_getFont(objectTab[i].objectData.text.fonts),
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
									MIDDLEWARE_getFont(objectTab[i].objectData.buttonBase.fonts),
									objectTab[i].objectData.buttonBase.colorText,
									ILI9341_TRANSPARENT,
									"%s", objectTab[i].objectData.buttonBase.text);

					if(objectTab[i].objectData.buttonBase.colorBorder != ILI9341_TRANSPARENT){
						ILI9341_drawRectangle(objectTab[i].objectData.buttonBase.x,
													objectTab[i].objectData.buttonBase.y,
													objectTab[i].objectData.buttonBase.x + objectTab[i].objectData.buttonBase.widthButton,
													objectTab[i].objectData.buttonBase.y + objectTab[i].objectData.buttonBase.heightButton,
													objectTab[i].objectData.buttonBase.colorBorder
													);
					}

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
					Uint8 lastFrame = objectTab[i].objectData.animatedImage.lastFrame;

					if(animatedImageInfo->frame[actualFrame].transparence){

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

				case RECTANGLE:
					if(objectTab[i].objectData.rectangle.colorCenter != ILI9341_TRANSPARENT){
						ILI9341_drawFilledRectangle(objectTab[i].objectData.rectangle.x,
													objectTab[i].objectData.rectangle.y,
													objectTab[i].objectData.rectangle.x + objectTab[i].objectData.rectangle.width,
													objectTab[i].objectData.rectangle.y + objectTab[i].objectData.rectangle.height,
													objectTab[i].objectData.rectangle.colorCenter);
					}

					if(objectTab[i].objectData.rectangle.colorBorder != ILI9341_TRANSPARENT){
						ILI9341_drawRectangle(objectTab[i].objectData.rectangle.x,
													objectTab[i].objectData.rectangle.y,
													objectTab[i].objectData.rectangle.x + objectTab[i].objectData.rectangle.width,
													objectTab[i].objectData.rectangle.y + objectTab[i].objectData.rectangle.height,
													objectTab[i].objectData.rectangle.colorBorder);
					}
					break;

				case CIRCLE:
					if(objectTab[i].objectData.circle.colorCenter != ILI9341_TRANSPARENT){
						ILI9341_drawFilledCircle(objectTab[i].objectData.circle.x,
													objectTab[i].objectData.circle.y,
													objectTab[i].objectData.circle.r,
													objectTab[i].objectData.circle.colorCenter);
					}

					if(objectTab[i].objectData.circle.colorBorder != ILI9341_TRANSPARENT){
						ILI9341_drawCircle(objectTab[i].objectData.circle.x,
											objectTab[i].objectData.circle.y,
											objectTab[i].objectData.circle.r,
											objectTab[i].objectData.circle.colorBorder);
					}
					break;

				case LINE:
					ILI9341_drawLine(objectTab[i].objectData.line.x0,
									objectTab[i].objectData.line.y0,
									objectTab[i].objectData.line.x1,
									objectTab[i].objectData.line.y1,
									objectTab[i].objectData.line.color);
					break;

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

void MIDDLEWARE_setBackground(objectColor_e color){
	assert(IS_OBJECT_COLOR_REAL(color));

	background.color = color;
	background.toDisplay = TRUE;
}

void MIDDLEWARE_setText(objectId_t id, const char *text, ...){
	assert(id < LCD_NB_MAX_OBJECT);
	assert(objectTab[id].use);

	switch(objectTab[id].type){
		case TEXT :{

			objectTab[id].surfaceToErase.x = objectTab[id].objectData.text.x;
			objectTab[id].surfaceToErase.y = objectTab[id].objectData.text.x;
			objectTab[id].surfaceToErase.width = objectTab[id].objectData.text.width;
			objectTab[id].surfaceToErase.height = objectTab[id].objectData.text.height;
			objectTab[id].toErase = TRUE;

			va_list args_list;
			va_start(args_list, text);
			vsnprintf((char *)objectTab[id].objectData.text.text, OBJECT_TEXT_MAX_SIZE, text, args_list);
			va_end(args_list);

			Uint16 widthText, heightText;
			ILI9341_getStringSize((char *)objectTab[id].objectData.text.text, MIDDLEWARE_getFont(objectTab[id].objectData.text.fonts), &widthText, &heightText);
			objectTab[id].objectData.text.width = widthText;
			objectTab[id].objectData.text.height = heightText;

			}break;

		case BUTTON_IMG :
			break;

		case BUTTON_BASE :{

				objectTab[id].surfaceToErase.x = objectTab[id].objectData.buttonBase.x;
				objectTab[id].surfaceToErase.y = objectTab[id].objectData.buttonBase.x;
				objectTab[id].surfaceToErase.width = objectTab[id].objectData.buttonBase.widthButton;
				objectTab[id].surfaceToErase.height = objectTab[id].objectData.buttonBase.heightButton;
				objectTab[id].toErase = TRUE;

				va_list args_list;
				va_start(args_list, text);
				vsnprintf((char *)objectTab[id].objectData.buttonBase.text, OBJECT_TEXT_MAX_SIZE, text, args_list);
				va_end(args_list);

				Uint16 widthText, heightText;
				ILI9341_getStringSize((char *)objectTab[id].objectData.buttonBase.text, MIDDLEWARE_getFont(objectTab[id].objectData.buttonBase.fonts), &widthText, &heightText);
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

objectId_t MIDDLEWARE_addText(Sint16 x, Sint16 y, objectColor_e colorText, objectColor_e colorBackground,  textFonts_e fonts, const char *text, ...){
	assert(IS_OBJECT_COLOR_REAL(colorText));
	assert(IS_TEXT_FONTS(fonts));
	assert(text != NULL);

	objectId_t idFound = MIDDLEWARE_newObject();

	if(idFound == OBJECT_ID_ERROR_FULL)
		return OBJECT_ID_ERROR_FULL;

	objectTab[idFound].type = TEXT;
	objectTab[idFound].objectData.text.x = x;
	objectTab[idFound].objectData.text.y = y;
	objectTab[idFound].objectData.text.fonts = fonts;
	objectTab[idFound].objectData.text.colorText = colorText;
	objectTab[idFound].objectData.text.colorBackground = colorBackground;

	va_list args_list;
	va_start(args_list, text);
	vsnprintf((char *)objectTab[idFound].objectData.text.text, OBJECT_TEXT_MAX_SIZE, text, args_list);
	va_end(args_list);

	return idFound;
}

objectId_t MIDDLEWARE_addButtonImg(Sint16 x, Sint16 y, const imageInfo_s *imageNormal, const imageInfo_s *imageLock, bool_e lockTouch, bool_e * touch){
	assert(imageNormal != NULL);
	assert(imageLock != NULL);
	assert(touch != NULL);

	objectId_t idFound = MIDDLEWARE_newObject();

	if(idFound == OBJECT_ID_ERROR_FULL)
		return OBJECT_ID_ERROR_FULL;

	*touch = FALSE;

	objectTab[idFound].type = BUTTON_IMG;
	objectTab[idFound].objectData.buttonImg.state = BUTTON_STATE_OFF;
	objectTab[idFound].objectData.buttonImg.lastState = BUTTON_STATE_OFF;
	objectTab[idFound].objectData.buttonImg.lastStateTouch = BUTTON_STATE_NO_TOUCH;
	objectTab[idFound].objectData.buttonImg.x = x;
	objectTab[idFound].objectData.buttonImg.y = y;
	objectTab[idFound].objectData.buttonImg.imageNormal = imageNormal;
	objectTab[idFound].objectData.buttonImg.imageLock = imageLock;
	objectTab[idFound].objectData.buttonImg.lockTouch = lockTouch;
	objectTab[idFound].objectData.buttonImg.touch = touch;

	return idFound;
}

objectId_t MIDDLEWARE_addButton(Sint16 x, Sint16 y, Uint16 width, Uint16 height, bool_e lockTouch, bool_e *touch, objectColor_e colorText, objectColor_e colorButton, objectColor_e colorButtonTouch, objectColor_e colorBorder, textFonts_e fonts, const char *text, ...){
	assert(touch != NULL);
	assert(IS_OBJECT_COLOR_REAL(colorText));
	assert(IS_OBJECT_COLOR_REAL(colorButton));
	assert(IS_OBJECT_COLOR_REAL(colorButtonTouch));
	assert(IS_TEXT_FONTS(fonts));
	assert(text != NULL);

	objectId_t idFound = MIDDLEWARE_newObject();

	if(idFound == OBJECT_ID_ERROR_FULL)
		return OBJECT_ID_ERROR_FULL;

	*touch = FALSE;

	objectTab[idFound].type = BUTTON_BASE;
	objectTab[idFound].objectData.buttonBase.state = BUTTON_STATE_OFF;
	objectTab[idFound].objectData.buttonBase.lastState = BUTTON_STATE_OFF;
	objectTab[idFound].objectData.buttonBase.lastStateTouch = BUTTON_STATE_NO_TOUCH;
	objectTab[idFound].objectData.buttonBase.x = x;
	objectTab[idFound].objectData.buttonBase.y = y;
	objectTab[idFound].objectData.buttonBase.fonts = fonts;
	objectTab[idFound].objectData.buttonBase.lockTouch = lockTouch;
	objectTab[idFound].objectData.buttonBase.colorText = colorText;
	objectTab[idFound].objectData.buttonBase.colorButton = colorButton;
	objectTab[idFound].objectData.buttonBase.colorTouch = colorButtonTouch;
	objectTab[idFound].objectData.buttonBase.colorBorder = colorBorder;
	objectTab[idFound].objectData.buttonBase.touch = touch;

	va_list args_list;
	va_start(args_list, text);
	vsnprintf((char *)objectTab[idFound].objectData.buttonBase.text, OBJECT_TEXT_MAX_SIZE, text, args_list);
	va_end(args_list);

	Uint16 widthText, heightText;
	ILI9341_getStringSize((char *)objectTab[idFound].objectData.buttonBase.text, MIDDLEWARE_getFont(fonts), &widthText, &heightText);
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

	return idFound;
}

objectId_t MIDDLEWARE_addProgressBar(Sint16 x, Sint16 y, Uint16 width, Uint16 height, objectOrientation_e orientation, Uint8 *value){
	assert(IS_OBJECT_ORIENTATION(orientation));
	assert(value != NULL);

	objectId_t idFound = MIDDLEWARE_newObject();

	if(idFound == OBJECT_ID_ERROR_FULL)
		return OBJECT_ID_ERROR_FULL;

	objectTab[idFound].type = PROGRESS_BAR;
	objectTab[idFound].objectData.progressBar.x = x;
	objectTab[idFound].objectData.progressBar.y = y;
	objectTab[idFound].objectData.progressBar.width = width;
	objectTab[idFound].objectData.progressBar.height = height;
	objectTab[idFound].objectData.progressBar.orientation = orientation;
	objectTab[idFound].objectData.progressBar.refreshBack = TRUE;
	objectTab[idFound].objectData.progressBar.value = value;
	objectTab[idFound].objectData.progressBar.lastValue = *value;

	return idFound;
}

objectId_t MIDDLEWARE_addSlider(Sint16 x, Sint16 y, Uint16 width, Uint16 height, Sint32 minValue, Sint32 maxValue, objectOrientation_e orientation, Sint32 *value){
	assert(IS_OBJECT_ORIENTATION(orientation));
	assert(value != NULL);
	assert(minValue != maxValue);

	objectId_t idFound = MIDDLEWARE_newObject();

	if(idFound == OBJECT_ID_ERROR_FULL)
		return OBJECT_ID_ERROR_FULL;

	objectTab[idFound].type = SLIDER;
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

	return idFound;
}

objectId_t MIDDLEWARE_addImage(Sint16 x, Sint16 y, const imageInfo_s *image){
	assert(image != NULL);

	objectId_t idFound = MIDDLEWARE_newObject();

	if(idFound == OBJECT_ID_ERROR_FULL)
		return OBJECT_ID_ERROR_FULL;

	objectTab[idFound].type = IMAGE;
	objectTab[idFound].objectData.image.x = x;
	objectTab[idFound].objectData.image.y = y;
	objectTab[idFound].objectData.image.image = image;

	return idFound;
}

objectId_t MIDDLEWARE_addAnimatedImage(Sint16 x, Sint16 y, const animatedImageInfo_s *animatedImageInfo){
	assert(animatedImageInfo != NULL);

	objectId_t idFound = MIDDLEWARE_newObject();

	if(idFound == OBJECT_ID_ERROR_FULL)
		return OBJECT_ID_ERROR_FULL;

	objectTab[idFound].type = ANIMATED_IMAGE;
	objectTab[idFound].objectData.animatedImage.x = x;
	objectTab[idFound].objectData.animatedImage.y = y;
	objectTab[idFound].objectData.animatedImage.animatedImageInfo = animatedImageInfo;
	objectTab[idFound].objectData.animatedImage.actualFrame = 0;
	objectTab[idFound].objectData.animatedImage.lastFrame = 255;
	objectTab[idFound].objectData.animatedImage.timeToRefresh = 0;

	return idFound;
}

objectId_t MIDDLEWARE_addRectangle(Sint16 x, Sint16 y, Uint16 width, Uint16 height, objectColor_e colorBorder, objectColor_e colorCenter){
	assert(IS_OBJECT_COLOR_REAL(colorBorder) || IS_OBJECT_COLOR_REAL(colorCenter));

	objectId_t idFound = MIDDLEWARE_newObject();

	if(idFound == OBJECT_ID_ERROR_FULL)
		return OBJECT_ID_ERROR_FULL;

	objectTab[idFound].type = RECTANGLE;
	objectTab[idFound].objectData.rectangle.x = x;
	objectTab[idFound].objectData.rectangle.y = y;
	objectTab[idFound].objectData.rectangle.width = width;
	objectTab[idFound].objectData.rectangle.height = height;
	objectTab[idFound].objectData.rectangle.colorBorder = colorBorder;
	objectTab[idFound].objectData.rectangle.colorCenter = colorCenter;

	return idFound;
}

objectId_t MIDDLEWARE_addCircle(Sint16 x, Sint16 y, Uint16 r, objectColor_e colorBorder, objectColor_e colorCenter){
	assert(IS_OBJECT_COLOR_REAL(colorBorder) || IS_OBJECT_COLOR_REAL(colorCenter));

	objectId_t idFound = MIDDLEWARE_newObject();

	if(idFound == OBJECT_ID_ERROR_FULL)
		return OBJECT_ID_ERROR_FULL;

	objectTab[idFound].type = CIRCLE;
	objectTab[idFound].objectData.circle.x = x;
	objectTab[idFound].objectData.circle.y = y;
	objectTab[idFound].objectData.circle.r = r;
	objectTab[idFound].objectData.circle.colorBorder = colorBorder;
	objectTab[idFound].objectData.circle.colorCenter = colorCenter;

	return idFound;
}

objectId_t MIDDLEWARE_addLine(Sint16 x0, Sint16 y0, Sint16 x1, Sint16 y1, objectColor_e color){
	assert(IS_OBJECT_COLOR_REAL(color));

	objectId_t idFound = MIDDLEWARE_newObject();

	if(idFound == OBJECT_ID_ERROR_FULL)
		return OBJECT_ID_ERROR_FULL;

	objectTab[idFound].type = LINE;
	objectTab[idFound].objectData.line.x0 = x0;
	objectTab[idFound].objectData.line.y0 = y0;
	objectTab[idFound].objectData.line.x1 = x1;
	objectTab[idFound].objectData.line.y1 = y1;
	objectTab[idFound].objectData.line.color = color;

	return idFound;
}

static objectId_t MIDDLEWARE_newObject(){
	Uint8 i;
	for(i=0; i<LCD_NB_MAX_OBJECT; i++){
		if(objectTab[i].use == FALSE){
			objectTab[i].toDisplay = TRUE;
			objectTab[i].toErase = FALSE;
			objectTab[i].toDestroy = FALSE;
			objectTab[i].use = TRUE;
			return i;
		}
	}
	return OBJECT_ID_ERROR_FULL;
}

static FontDef_t* MIDDLEWARE_getFont(textFonts_e fonts){
	assert(IS_TEXT_FONTS(fonts));

	switch(fonts){
		case TEXT_FONTS_7x10 :
			return &Font_7x10;

		case TEXT_FONTS_11x18 :
			return &Font_11x18;

		case TEXT_FONTS_16x26 :
			return &Font_16x26;

		default:
			return &Font_7x10;
	}
}

//////////////////////////////////////////////////////////////////
//------------------Fonction Destruction------------------------//
//////////////////////////////////////////////////////////////////

void MIDDLEWARE_deleteObject(objectId_t id){
	assert(id < LCD_NB_MAX_OBJECT);
	assert(objectTab[id].use);

	objectTab[id].toDestroy = TRUE;
}

void MIDDLEWARE_resetScreen(){
	Uint8 i;
	for(i=0;i<LCD_NB_MAX_OBJECT;i++){
		objectTab[i].use = FALSE;
	}
	background.color = 0xFFFF;
	background.toDisplay = TRUE;
}


