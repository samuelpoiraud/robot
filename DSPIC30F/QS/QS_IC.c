
#include "QS_IC.h"
//#define


#ifdef USE_IC4

static Uint16 IC4_bufferICVal1;
static Uint16 IC4_bufferICVal2;
static Uint16 IC4_bufferICVal3;

static IC_CaptureMode_e IC4_start_Mode;
static IC_CaptureInterrupt_e IC4_NbCapture;
static bool_e IC4_Interrupt_done;

IC_valu_t IC4_get_puls(Uint16 TimeOut){
    
    IC_valu_t valOfbuff;

    Uint16 Time=0;
    IC4_start ();
    while(IC4_interrupt_is_done() == FALSE && Time < TimeOut ){
        Time = Time+1;

    }
    IC4_stop();

    valOfbuff.valOfBuf1 = IC4_bufferICVal1;
    valOfbuff.valOfBuf2 =  IC4_bufferICVal2;
    valOfbuff.valOfBuf3 =  IC4_bufferICVal3; 
           
    return valOfbuff;
    
}





void IC4_init (IC_config_t config){
    
    IC4_Interrupt_done = FALSE;
    
    IC4_start_Mode = config.InputCaptureMode;
    IC4_NbCapture = config.InputCaptureInterrupt;

    IC4CONbits.ICM= (IC_CaptureMode_e)IC_CM_TurnedOff;
    IC4CONbits.ICBNE=0;		/*Input capture buffer is empty*/
    IC4CONbits.ICOV=0;		/*No input capture overflow occured*/
    IC4CONbits.ICI=config.InputCaptureInterrupt;
    IC4CONbits.ICTMR=config.InputCaptureTimer;		/*contents are captured on captured event*/
    IC4CONbits.ICSIDL=config.InputCaptureIdle;    /*Input capture module will continue to operate in CPU Idle mode*/

    IFS1bits.IC4IF = 0;/*Interrupt bit is cleared*/
    IPC7bits.IC4IP = config.InterruptPriority;     // Set Interrupt priority
    IEC1bits.IC4IE = 1;     /*Set the IC4 interrupt enable bit */
}
void IC4_stop (void){
    IC4CONbits.ICM = (IC_CaptureMode_e)IC_CM_TurnedOff;
}
void IC4_start (void){
    IC4CONbits.ICM =  IC4_start_Mode;
}

bool_e IC4_interrupt_is_done(void){
    
    if(IC4_Interrupt_done ==FALSE){
        return FALSE;
    }else{
        IC4_Interrupt_done = FALSE;
        return TRUE;
    }
}
/****INTERRUPT FOR IC4****/
// Capture Interrupt Service Routine
//unsigned int timePeriod= 0;
void __attribute__((interrupt, auto_psv))  _IC4Interrupt() {
    
    switch(IC4_NbCapture){
        case IC_CI_EveryCapture :
            IC4_bufferICVal1=IC4BUF;
            IC4_bufferICVal2=0;
            IC4_bufferICVal3=0;
            break;
        case IC_CI_Every2Capture :
            IC4_bufferICVal1=IC4BUF;
            IC4_bufferICVal2=IC4BUF;
            IC4_bufferICVal3=0;
            break;
        case IC_CI_Every4Capture :
        case IC_CI_Every3Capture :
            IC4_bufferICVal1=IC4BUF;
            IC4_bufferICVal2=IC4BUF;
            IC4_bufferICVal3=IC4BUF;
            break;
      
        default:

            break;
            
    }
    //LED_USER2=!LED_USER2;
    IC4_Interrupt_done = TRUE;

    IC4_stop();
    IFS1bits.IC4IF=0;}

#endif
