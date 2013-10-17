/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  Fichier : QS_trap_handler.c
 *  Package : Qualité Soft
 *  Description : Gestion des trap en cas d'erreur fatales
 *  Auteur : amurzeau
 *  Version 20130603
 */


#include "QS_trap_handler.h"
#include "QS_outputlog.h"

void dump_trap_info(Uint32 stack_ptr[], Uint32 lr) {
}

void _ISR _MathError()
{
  _MATHERR = 0;
  LED_ERROR = 1;
  debug_printf("Trap Math\r\n");
  while(1) Nop();
}

void _ISR _StackError()
{
  _STKERR = 0;
  LED_ERROR = 1;
  debug_printf("Trap Stack\r\n");
  while(1) Nop();
}

void _ISR _AddressError()
{
  _ADDRERR = 0;
  LED_ERROR = 1;
  debug_printf("Trap Address\r\n");
  while(1) Nop();
}

void _ISR _OscillatorFail()
{
  _OSCFAIL = 0;
  LED_ERROR = 1;
  debug_printf("Trap OscFail\r\n");
  while(1) Nop();
}
