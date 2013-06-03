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

void dump_trap_info(Uint32 stack_ptr[], Uint32 lr) {
	extern char _estack;	//Defined by the linker, end of stack
#ifdef VERBOSE_MODE
	debug_printf("FATAL Error ! Exception %ld\n", __get_IPSR() & 0xFF);
	if(lr & 0x00000008)
		debug_printf("CPU was in thread mode\n");
	else debug_printf("CPU was in handler mode\n");

	int offset, i;
	offset = 0;

	debug_printf("CPU status was:\n");
	debug_printf("-  R0: 0x%08lX  R1: 0x%08lX\n", stack_ptr[offset], stack_ptr[offset + 1]); offset += 2;
	debug_printf("-  R2: 0x%08lX  R3: 0x%08lX\n", stack_ptr[offset], stack_ptr[offset + 1]); offset += 2;
	debug_printf("- R12: 0x%08lX\n", stack_ptr[offset++]);
	debug_printf("- LR:   0x%08lX\n", stack_ptr[offset++]);
	debug_printf("- PC:   0x%08lX\n", stack_ptr[offset++]);
	debug_printf("- xPSR: 0x%08lX\n\n", stack_ptr[offset++]);
	if(lr & 0x00000010) {
		debug_printf("FPU status was:\n");
		debug_printf("-  S0: 0x%08lX   S1: 0x%08lX   S2: 0x%08lX   S3: 0x%08lX\n", stack_ptr[offset], stack_ptr[offset + 1], stack_ptr[offset + 2], stack_ptr[offset + 3]); offset += 4;
		debug_printf("-  S4: 0x%08lX   S5: 0x%08lX   S6: 0x%08lX   S7: 0x%08lX\n", stack_ptr[offset], stack_ptr[offset + 1], stack_ptr[offset + 2], stack_ptr[offset + 3]); offset += 4;
		debug_printf("-  S8: 0x%08lX   S9: 0x%08lX  S10: 0x%08lX  S11: 0x%08lX\n", stack_ptr[offset], stack_ptr[offset + 1], stack_ptr[offset + 2], stack_ptr[offset + 3]); offset += 4;
		debug_printf("- S12: 0x%08lX  S13: 0x%08lX  S14: 0x%08lX  S15: 0x%08lX\n", stack_ptr[offset], stack_ptr[offset + 1], stack_ptr[offset + 2], stack_ptr[offset + 3]); offset += 4;
		debug_printf("- FPSCR: 0x%08lX\n", stack_ptr[offset++]);
		offset++; //empty value at end
	}

	debug_printf("Stack was: \n");
	for(; i < 32 && (char*)&(stack_ptr[offset]) < &_estack; i++) {
		if(!((i + 1) % 4) && i)
			debug_printf("\n");
		debug_printf("0x%08lX ", stack_ptr[offset++]);
	}
	debug_printf("\n");
#endif

	//See http://infocenter.arm.com/help/topic/com.arm.doc.dui0553a/Cihdjcfc.html

	if((__get_IPSR() & 0xFF) == 3) {
		debug_printf("HardFault reason:\n");
		if(SCB->HFSR & SCB_HFSR_DEBUGEVT_Msk)
			debug_printf("- DEBUGEVT\n");
		if(SCB->HFSR & SCB_HFSR_FORCED_Msk)
			debug_printf("- Fault escalated to a hard fault\n");
		if(SCB->HFSR & SCB_HFSR_VECTTBL_Msk)
			debug_printf("- Bus error on a vector read\n");
	}


	//See http://infocenter.arm.com/help/topic/com.arm.doc.dui0553a/Cihcfefj.html#Cihgghei

	if((__get_IPSR() & 0xFF) == 4) {
		if((SCB->CFSR >> SCB_CFSR_MEMFAULTSR_Pos) & 0x80)
			debug_printf("MemManage fault at address 0x%08lX\n", SCB->MMFAR);
		else
			debug_printf("MemManage fault\n");

		if((SCB->CFSR >> SCB_CFSR_MEMFAULTSR_Pos) & 0x01)
			debug_printf("- Memory is not executable\n");
		if((SCB->CFSR >> SCB_CFSR_MEMFAULTSR_Pos) & 0x02)
			debug_printf("- Memory is not readable/writable\n");
		if((SCB->CFSR >> SCB_CFSR_MEMFAULTSR_Pos) & 0x08)
			debug_printf("- Exception when unstacking from exception\n");
		if((SCB->CFSR >> SCB_CFSR_MEMFAULTSR_Pos) & 0x10)
			debug_printf("- Exception when stacking for an exception\n");
		if((SCB->CFSR >> SCB_CFSR_MEMFAULTSR_Pos) & 0x20)
			debug_printf("- Fault when floating-point lazy state preservation\n");
	}


	//See http://infocenter.arm.com/help/topic/com.arm.doc.dui0553a/Cihcfefj.html#Cihbeigb

	if((__get_IPSR() & 0xFF) == 5) {
		if((SCB->CFSR >> SCB_CFSR_BUSFAULTSR_Pos) & 0x80)
			debug_printf("BusFault fault at address 0x%08lX\n", SCB->BFAR);
		else
			debug_printf("BusFault fault\n");

		if((SCB->CFSR >> SCB_CFSR_BUSFAULTSR_Pos) & 0x01)
			debug_printf("- Instruction bus error\n");
		if((SCB->CFSR >> SCB_CFSR_BUSFAULTSR_Pos) & 0x02)
			debug_printf("- Precise Data bus error\n");
		if((SCB->CFSR >> SCB_CFSR_BUSFAULTSR_Pos) & 0x04)
			debug_printf("- Imprecise Data bus error\n");
		if((SCB->CFSR >> SCB_CFSR_BUSFAULTSR_Pos) & 0x08)
			debug_printf("- Exception when unstacking from exception\n");
		if((SCB->CFSR >> SCB_CFSR_BUSFAULTSR_Pos) & 0x10)
			debug_printf("- Exception when stacking for an exception\n");
		if((SCB->CFSR >> SCB_CFSR_BUSFAULTSR_Pos) & 0x20)
			debug_printf("- Fault when floating-point lazy state preservation\n");
	}

	//See http://infocenter.arm.com/help/topic/com.arm.doc.dui0553a/Cihcfefj.html#Cihgbdbi

	if((__get_IPSR() & 0xFF) == 6) {
		debug_printf("UsageFault fault, return address: 0x%08lX\n", stack_ptr[6]);

		if((SCB->CFSR >> SCB_CFSR_USGFAULTSR_Pos) & 0x001)
			debug_printf("- Undefined instruction\n");
		if((SCB->CFSR >> SCB_CFSR_USGFAULTSR_Pos) & 0x002)
			debug_printf("- Illegal use of the EPSR\n");
		if((SCB->CFSR >> SCB_CFSR_USGFAULTSR_Pos) & 0x004)
			debug_printf("- Illegal load of the PC\n");
		if((SCB->CFSR >> SCB_CFSR_USGFAULTSR_Pos) & 0x008)
			debug_printf("- Attempt to access a coprocessor but not present\n");
		if((SCB->CFSR >> SCB_CFSR_USGFAULTSR_Pos) & 0x100)
			debug_printf("- Unaligned memory access\n");
		if((SCB->CFSR >> SCB_CFSR_USGFAULTSR_Pos) & 0x200)
			debug_printf("- Divide by zero\n");
	}
}

__attribute__((naked)) void Fault_Handler(void)
{
	//On ne veux pas perdre l'état des registres, donc pas de C
	//l'attribut naked indique qu'on ne veux pas de prologue / epilogue générés par GCC
	__ASM volatile
	(
		"TST LR, #4\n"		// Test for MSP or PSP
		"ITE EQ\n"			//If equal
		"MRSEQ R0, MSP\n"	//r0 = msp
		"MRSNE R0, PSP\n"	//else r0 = psp
		"MOV R1, LR\n"
		"B dump_trap_info\n"
	);
}
void HardFault_Handler(void) __attribute__((alias("Fault_Handler")));
void MemManage_Handler(void) __attribute__((alias("Fault_Handler")));
void BusFault_Handler(void) __attribute__((alias("Fault_Handler")));
void UsageFault_Handler(void) __attribute__((alias("Fault_Handler")));

void NMI_Handler(void)
{
}

void SVC_Handler(void)
{
	debug_printf("SVC interrupt: unimplemented\n");
}

void DebugMon_Handler(void)
{
	debug_printf("DebugMon: unimplemented\n");
}

void PendSV_Handler(void)
{
	debug_printf("Pending SVC interrupt: unimplemented\n");
}

__attribute__((weak))
void SysTick_Handler(void)
{
	//Unused but not an error
}
