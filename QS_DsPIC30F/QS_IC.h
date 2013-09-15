/* 
 * File:   sensor_color.h
 * Author: ino
 *
 * Created on November 4, 2012, 8:39 AM
 */
#include "QS_all.h"
#include "QS_timer.h"

#ifndef QS_IC_H
#define	QS_IC_H


// paramètre de comptage de front ICM
typedef enum{
    IC_CM_onlyWhenSleep =0b111,
    IC_CM_CaptureEvery16Rising =0b101,
    IC_CM_CaptureEvery4Rising =0b100,
    IC_CM_CaptureEveryRising =0b011,
    IC_CM_CaptureEveryFalling =0b010,
    M_CaptureEveryFallingAndRising =0b001,
    IC_CM_TurnedOfIC_CM_CaptureEveryFallingAndRising =0b001,
    IC_CM_TurnedOff =0b000   
}IC_CaptureMode_e;

//paramètre d'interuption ICI
typedef enum{
    IC_CI_Every4Capture = 0b11,
    IC_CI_Every3Capture = 0b10,
    IC_CI_Every2Capture = 0b01, 
    IC_CI_EveryCapture = 0b00
}IC_CaptureInterrupt_e;

//paramètre de Timer ICMR
typedef enum{
    IC_CIS_Timer2 = 0xb1,
    IC_CIS_Timer3 = 0xb0
}IC_CaptureTimerSelect_e;

//paramètrage toujours continu
typedef enum{
    IC_CSI_halt = 0xb1,
    IC_CSI_continue = 0xb0
}IC_CaptureStopIdle_e;

//paramètrage
typedef struct {
    IC_CaptureMode_e InputCaptureMode;
    IC_CaptureInterrupt_e InputCaptureInterrupt;
    IC_CaptureTimerSelect_e InputCaptureTimer;
    IC_CaptureStopIdle_e InputCaptureIdle;
    Uint8 InterruptPriority;
}IC_config_t;

typedef struct {
    Uint16 valOfBuf1;
    Uint16 valOfBuf2;
    Uint16 valOfBuf3;
}IC_valu_t;

#ifdef USE_IC4

IC_valu_t IC4_get_puls(Uint16 TimeOut);
void IC4_init (IC_config_t config);
void IC4_stop(void);
void IC4_start(void);
bool_e IC4_interrupt_is_done(void);
#endif


#endif	/* QS_IC_H */

