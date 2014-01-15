/**
  ******************************************************************************
  * <h2><center>&copy; COPYRIGHT 2012 Embest Tech. Co., Ltd.</center></h2>
  * @file    LCD_Touch_Calibration.c
  * @author  CMP Team + Amaury.Gaudin && Edouard.Thyebaut
  * @version V1.0.0
  * @date    28-December-2012
  * @update 10-Decembre-2013
  * @brief   LCD touch screen calibration      
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, Embest SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
  * OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
  * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
  * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  ******************************************************************************
  */

#include "LCD_Touch_Calibration.h"

#if defined (SIMULATION_VIRTUAL_PERFECT_ROBOT)

#define LCD_TOUCH_CALIBRATION_MODULE

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "stm32f4_discovery_lcd.h"
#include "stmpe811qtr.h"
#include "LCD.h"

Adjust_Struct adjust_Para = {-0.0897,0.0631,-350,13};
/** @defgroup STM32F4xx_StdPeriph_Examples
  * @{
  */
/* Private define ------------------------------------------------------------*/
/*Calibration accuracy, + /-x pixels*/
#define CALIBRATION_RANGE      (10)

/*1st calibration point  position*/
#define CAL_POINT1_X           (20)
#define CAL_POINT1_Y           (20)

/*2nd calibration point position*/
#define CAL_POINT2_X          (300)
#define CAL_POINT2_Y           (20)

/*3rd calibration point position*/
#define CAL_POINT3_X           (20)
#define CAL_POINT3_Y          (220)

/*4th calibration point position*/
#define CAL_POINT4_X          (300)
#define CAL_POINT4_Y          (220)

/*calibration test point position*/
#define TST_POINT_X           (160)
#define TST_POINT_Y           (120)

#define TOUCH_AD_VALUE_MAX    (4000)
#define TOUCH_AD_VALUE_MIN    (100)

#define ABS(X)  ((X) > 0 ? (X) : -(X))

/* External function prototypes -----------------------------------------------*/
void delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0;
  for (index = (100000 * nCount); index != 0; index--);
}

/* Private variables ---------------------------------------------------------*/
Point_Struct point_new, point_old;
/*Variables definition for calibration point*/
static Point_Struct point_Base[5] = {
  {CAL_POINT1_X,CAL_POINT1_Y},
  {CAL_POINT2_X,CAL_POINT2_Y},
  {CAL_POINT3_X,CAL_POINT3_Y},
  {CAL_POINT4_X,CAL_POINT4_Y},
  {TST_POINT_X,TST_POINT_Y}
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Lcd_Touch Calibration  test
  * @param  None
  * @retval None
  */
void Lcd_Touch_Calibration(void)
{
#define CURSOR_LEN    (10)
  uint8_t k,i;
  float ratio1,ratio2;
  Point_Struct left_upper_point,right_upper_point,left_down_point,right_down_point,tst_point;
  int tpx_sum = 0,tpy_sum = 0;

  /*Indicates whether Calibration is OK*/
  uint8_t adjust_OK_Falg = 0;
  TS_STATE *pstate = NULL;

  /* Clear the LCD */ 
  LCD_Clear(White);
  LCD_SetTextColor(Red); 
  delay(100);
  while (!adjust_OK_Falg) {
    tpx_sum = 0;
    tpy_sum = 0;
    /*wait for Calibration */
    for (k = 0;k < 4;k++) {
      LCD_DrawUniLine( point_Base[k].x - CURSOR_LEN,
                       point_Base[k].y,
                       point_Base[k].x + CURSOR_LEN,
                       point_Base[k].y);

      LCD_DrawUniLine( point_Base[k].x,
                       point_Base[k].y - CURSOR_LEN,
                       point_Base[k].x,
                       point_Base[k].y + CURSOR_LEN);
      do {
        pstate = IOE_TS_GetState();
      } while(!pstate->TouchDetected);
			delay(10);
      /*Read AD convert result*/
      for(i = 0; i < 16; i++) {
        tpx_sum += IOE_TS_Read_X();
        tpy_sum += IOE_TS_Read_Y();	
        delay(2);
      }
      tpx_sum >>= 4;
      tpy_sum >>= 4;

      switch (k) {
        case 0:
          left_upper_point.x = tpx_sum;
          left_upper_point.y = tpy_sum;
          break;

        case 1:
          right_upper_point.x = tpx_sum;
          right_upper_point.y = tpy_sum;
          break;

        case 2:
          left_down_point.x = tpx_sum;
          left_down_point.y = tpy_sum;
          break;

        case 3:
          right_down_point.x = tpx_sum;
          right_down_point.y = tpy_sum;
          break;

        default:
          break;
      }
      delay(200);
    }

    ratio1 = (float)((point_Base[1].x - point_Base[0].x) + (point_Base[3].x - point_Base[2].x)) / 2.0;
    ratio2 = (float)((right_upper_point.x - left_upper_point.x) + (right_down_point.x - left_down_point.x)) / 2.0;
    adjust_Para.xScale = ratio1 / ratio2;

    ratio1 = (float)((point_Base[2].y - point_Base[0].y) + 
             (point_Base[3].y - point_Base[1].y)) / 2.0;
    ratio2 = (float)((left_down_point.y - left_upper_point.y) + 
             (right_down_point.y - right_upper_point.y)) / 2.0;
    adjust_Para.yScale = ratio1 / ratio2;

    ratio1 = (((float)right_upper_point.x * adjust_Para.xScale - (float)point_Base[1].x)
             + ((float)left_upper_point.x * adjust_Para.xScale - (float)point_Base[0].x)) / 2.0;
    adjust_Para.xOffset = (int)ratio1;

    ratio1 = (((float)right_upper_point.y * adjust_Para.yScale - (float)point_Base[1].y)
             + ((float)left_upper_point.y * adjust_Para.yScale - (float)point_Base[0].y)) / 2.0;

    adjust_Para.yOffset = (int)ratio1; 

    /*Draw cross sign for calibration points*/
    LCD_DrawUniLine(point_Base[4].x - CURSOR_LEN,
                    point_Base[4].y,
                    point_Base[4].x + CURSOR_LEN,
                    point_Base[4].y);

    LCD_DrawUniLine(point_Base[4].x,
                    point_Base[4].y - CURSOR_LEN,
                    point_Base[4].x,
                    point_Base[4].y + CURSOR_LEN);

    do {
      pstate = IOE_TS_GetState();
    } while(!pstate->TouchDetected);
    delay(10);
    tpx_sum = 0;
    tpy_sum = 0;
    /*Read AD convert result*/
    for(i = 0; i < 16; i++) {
      tpx_sum += IOE_TS_Read_X();
      tpy_sum += IOE_TS_Read_Y();	
      delay(2);
    }
    tpx_sum >>= 4;
    tpy_sum >>= 4;

    tst_point.x = tpx_sum;
    tst_point.y = tpy_sum;
         
    tst_point.x = (int)(tst_point.x * adjust_Para.xScale - adjust_Para.xOffset); 
    tst_point.y = (int)(tst_point.y * adjust_Para.yScale - adjust_Para.yOffset); 

    if (tst_point.x > (point_Base[4].x + CALIBRATION_RANGE)
        || tst_point.x < (point_Base[4].x - CALIBRATION_RANGE)
        || tst_point.y > (point_Base[4].y + CALIBRATION_RANGE)
        || tst_point.y < (point_Base[4].y - CALIBRATION_RANGE)) {
      adjust_OK_Falg = 0;
      LCD_DisplayStringLine(LINE(8),"   Calibration Fail!    ");
      delay(200);
      LCD_Clear(White);
      delay(300);
    } else {
      adjust_OK_Falg = 1;
    }
  }
  LCD_DisplayStringLine(LINE(8),"   Calibration OK!    ");
  delay(200);
}

#define TOUCH_BUTTON_SIZE	(BUTTON_SIZE+3)
#define entrance (previous_touch == FALSE && pstate->TouchDetected!=FALSE)
/**
  * @brief  Display the value of calibration point
  * @param  *x Positions en x
  * @param  *y Positions en y
  * @param  *couleur_robot Couleur du Robot (numéro)
  * @param  *flag_update (flage d'un nouveau toucher à l'interrieur d'une zone
  *
  * @retval None
  */

bool_e Calibration_Test_Dispose(display_robot_t *pos, robots_e *id_robot)
{
  TS_STATE *pstate = NULL;
  bool_e ret = FALSE;
  static bool_e previous_touch = FALSE;

  // Variables qui récuperent la nouvelle et l'ancienne valeur de la position appuyée
  point_new.x = 0;
  point_new.y = 0;
  point_old.x = 0;
  point_old.y = 0;

    pstate = IOE_TS_GetState();	// Récupération des données d'appuye sur l'écran tactile

    if(pstate->TouchDetected){	// Dans le cas où on a détecté un appuye
		point_new.x = pstate->X;
		point_new.y = pstate->Y;

		// La valeur obtenue est-elle bien sur l'écran ?
		if (!((point_new.x > TOUCH_AD_VALUE_MAX)
			 || (point_new.x < TOUCH_AD_VALUE_MIN)
			 || (point_new.y > TOUCH_AD_VALUE_MAX)
			 || (point_new.y < TOUCH_AD_VALUE_MIN))){

			// On calcul et ajuste les coordonnées en fonction du calibrage
			point_new.x = ((int)(point_new.x * adjust_Para.xScale - adjust_Para.xOffset));
			point_new.y = ((int)(point_new.y * adjust_Para.yScale - adjust_Para.yOffset));
			point_new.x = point_new.x -6; //Calibrage manuel
			point_new.y =point_new.y +4; //Calibrage manuel

		// A-t-on appyé sur un boutton?
			if(point_new.x>=310-TOUCH_BUTTON_SIZE && point_new.x<=310+TOUCH_BUTTON_SIZE)
			{ //test d'appui sur les boutons
				// Permet de bouger notre propre robot: Désactivé car géré par la propulsion
 				if (point_new.y>=20-TOUCH_BUTTON_SIZE && point_new.y<=20+TOUCH_BUTTON_SIZE)
				{//boutton 1
 					if(entrance){
 						if(*id_robot == FRIEND_1){
 							*id_robot= NONE;
 						}else{
 						*	id_robot= FRIEND_1;
 						}
						ret = TRUE;
 					}
				}
				else if (point_new.y>=40-TOUCH_BUTTON_SIZE && point_new.y<=40+TOUCH_BUTTON_SIZE)
				{//boutton 2
					if(entrance){
						if(*id_robot == FRIEND_2){
							*id_robot= NONE;
							set_robot_disable(FRIEND_2);
						}else{
							 *id_robot= FRIEND_2;
							 set_robot_enable(FRIEND_2);
						}
						ret = TRUE;
					}
				}
				else if (point_new.y>=80-TOUCH_BUTTON_SIZE && point_new.y<=80+TOUCH_BUTTON_SIZE)
				{// Boutton3
					if(entrance){
						if(*id_robot == ADVERSARY_1){
							*id_robot= NONE;
							set_robot_disable(ADVERSARY_1);
						}else{
							*id_robot= ADVERSARY_1;
							set_robot_enable(ADVERSARY_1);
						}
						ret = TRUE;
					}
				}
				else if (point_new.y>=100-TOUCH_BUTTON_SIZE && point_new.y<=100+TOUCH_BUTTON_SIZE)
				{// Boutton4
					if(entrance){
						if(*id_robot == ADVERSARY_2){
							*id_robot= NONE;
							set_robot_disable(ADVERSARY_2);
						}else{
							*id_robot= ADVERSARY_2;
							set_robot_enable(ADVERSARY_2);
						}
						ret = TRUE;
					}
				}
			}

			// A-t-on appuyé sur le terrain
			if(point_new.x < LCD_PIXEL_WIDTH-25 && point_new.y < LCD_PIXEL_HEIGHT-45 && (pos->enable==TRUE)){ //fenetre du plateau de jeu
				if (point_new.x >= LCD_PIXEL_WIDTH-30) {//pour ne pas que le carré d'affichage ne sorte du tableau (plateau de jeu)
					point_new.x = LCD_PIXEL_WIDTH-30 -1;
				}
				if (point_new.y >= LCD_PIXEL_HEIGHT-50) {	//pour ne pas que le carré d'affichage ne sorte du tableau (plateau de jeu)
					point_new.y = LCD_PIXEL_HEIGHT-50 -1;
				}
				if (point_new.x <= 9) {//pour ne pas que le carré d'affichage ne sorte du tableau (plateau de jeu)
					point_new.x = 10;
				}
				if (point_new.y <= 9) {	//pour ne pas que le carré d'affichage ne sorte du tableau (plateau de jeu)
					point_new.y = 10;
				}

				if(*id_robot == US){ //Si c'est nous on force la nouvelle coordonnée de la proulsion
					Uint8 data[8];
					//go pos
					data[0] = 0x20;	//Multipoint, MAINTENANT.
					data[1] = HIGHINT(robots[FRIEND_1].x*10);
					data[2] = LOWINT(robots[FRIEND_1].x*10);
					data[3] = HIGHINT(robots[FRIEND_1].y*10);
					data[4] = LOWINT(robots[FRIEND_1].y*10);
					data[5] = 0x01; 	//slow
					data[6] = 0x00;		//Avant ou arrière
					data[7] = 0;
					SECRETARY_send_canmsg(ASSER_GO_POSITION, data, 8);
				}

				pos->y=point_new.x;	//ATTENTION INVERSION X et Y (les repères terrain et LCD sont différents)
				pos->x=point_new.y; //ATTENTION INVERSION X et Y (les repères terrain et LCD sont différents)
				pos->updated=TRUE;
				ret = TRUE;
			}
		}
    }

    previous_touch = pstate->TouchDetected!=FALSE; //Je ne sais pas si c'est un booléen donc je fais une opération logique dessus => je sais seulement qu'il est a 0 lorsque rien nest détecté
    return ret;
}

/******************** COPYRIGHT 2012 Embest Tech. Co., Ltd.*****END OF FILE****/

#endif
