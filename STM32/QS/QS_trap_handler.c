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
#include <stdio.h>

void dump_trap_info(Uint32 stack_ptr[], Uint32 lr) {
	extern char _estack;	//Defined by the linker, end of stack

	fprintf(stderr, "FATAL Error ! Exception %ld\n", __get_IPSR() & 0xFF);
	//See http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/CHDBIBGJ.html
	/*  ISR_NUMBER
		This is the number of the current exception:
		0 = Thread mode
		1 = Reserved
		2 = NMI
		3 = HardFault
		4 = MemManage
		5 = BusFault
		6 = UsageFault
		7-10 = Reserved
		11 = SVCall
		12 = Reserved for Debug
		13 = Reserved
		14 = PendSV
		15 = SysTick
		16 = IRQ0.
	*/
	if(lr & 0x00000008)
		fprintf(stderr, "CPU was in thread mode\n");
	else fprintf(stderr, "CPU was in handler mode\n");

	int offset, i;
	offset = 0;

	fprintf(stderr, "CPU status was:\n");
	fprintf(stderr, "-  R0: 0x%08lX  R1: 0x%08lX\n", stack_ptr[offset], stack_ptr[offset + 1]); offset += 2;
	fprintf(stderr, "-  R2: 0x%08lX  R3: 0x%08lX\n", stack_ptr[offset], stack_ptr[offset + 1]); offset += 2;
	fprintf(stderr, "- R12: 0x%08lX\n", stack_ptr[offset++]);
	fprintf(stderr, "- LR:   0x%08lX\n", stack_ptr[offset++]);
	fprintf(stderr, "- PC:   0x%08lX\n", stack_ptr[offset++]);
	fprintf(stderr, "- xPSR: 0x%08lX\n\n", stack_ptr[offset++]);
	if(lr & 0x00000010) {
		fprintf(stderr, "FPU status was:\n");
		fprintf(stderr, "-  S0: 0x%08lX   S1: 0x%08lX   S2: 0x%08lX   S3: 0x%08lX\n", stack_ptr[offset], stack_ptr[offset + 1], stack_ptr[offset + 2], stack_ptr[offset + 3]); offset += 4;
		fprintf(stderr, "-  S4: 0x%08lX   S5: 0x%08lX   S6: 0x%08lX   S7: 0x%08lX\n", stack_ptr[offset], stack_ptr[offset + 1], stack_ptr[offset + 2], stack_ptr[offset + 3]); offset += 4;
		fprintf(stderr, "-  S8: 0x%08lX   S9: 0x%08lX  S10: 0x%08lX  S11: 0x%08lX\n", stack_ptr[offset], stack_ptr[offset + 1], stack_ptr[offset + 2], stack_ptr[offset + 3]); offset += 4;
		fprintf(stderr, "- S12: 0x%08lX  S13: 0x%08lX  S14: 0x%08lX  S15: 0x%08lX\n", stack_ptr[offset], stack_ptr[offset + 1], stack_ptr[offset + 2], stack_ptr[offset + 3]); offset += 4;
		fprintf(stderr, "- FPSCR: 0x%08lX\n", stack_ptr[offset++]);
		offset++; //empty value at end
	}

	fprintf(stderr, "Stack was: \n");
	for(i=0; i < 32 && (char*)&(stack_ptr[offset]) < &_estack; i++) {
		if(!((i + 1) % 4) && i)
			fprintf(stderr, "\n");
		fprintf(stderr, "0x%08lX ", stack_ptr[offset++]);
	}
	fprintf(stderr, "\n");

	//See http://infocenter.arm.com/help/topic/com.arm.doc.dui0553a/Cihdjcfc.html

	if((__get_IPSR() & 0xFF) == 3) {
		fprintf(stderr, "HardFault reason:\n");
		if(SCB->HFSR & SCB_HFSR_DEBUGEVT_Msk)
			fprintf(stderr, "- DEBUGEVT\n");
		if(SCB->HFSR & SCB_HFSR_FORCED_Msk)
			fprintf(stderr, "- Fault escalated to a hard fault\n");
		if(SCB->HFSR & SCB_HFSR_VECTTBL_Msk)
			fprintf(stderr, "- Bus error on a vector read\n");
	}


	//See http://infocenter.arm.com/help/topic/com.arm.doc.dui0553a/Cihcfefj.html#Cihgghei

	if((__get_IPSR() & 0xFF) == 4) {
		if((SCB->CFSR >> SCB_CFSR_MEMFAULTSR_Pos) & 0x80)
			fprintf(stderr, "MemManage fault at address 0x%08lX\n", SCB->MMFAR);
		else
			fprintf(stderr, "MemManage fault\n");

		if((SCB->CFSR >> SCB_CFSR_MEMFAULTSR_Pos) & 0x01)
			fprintf(stderr, "- Memory is not executable\n");
		if((SCB->CFSR >> SCB_CFSR_MEMFAULTSR_Pos) & 0x02)
			fprintf(stderr, "- Memory is not readable/writable\n");
		if((SCB->CFSR >> SCB_CFSR_MEMFAULTSR_Pos) & 0x08)
			fprintf(stderr, "- Exception when unstacking from exception\n");
		if((SCB->CFSR >> SCB_CFSR_MEMFAULTSR_Pos) & 0x10)
			fprintf(stderr, "- Exception when stacking for an exception\n");
		if((SCB->CFSR >> SCB_CFSR_MEMFAULTSR_Pos) & 0x20)
			fprintf(stderr, "- Fault when floating-point lazy state preservation\n");
	}


	//See http://infocenter.arm.com/help/topic/com.arm.doc.dui0553a/Cihcfefj.html#Cihbeigb

	if((__get_IPSR() & 0xFF) == 5) {
		if((SCB->CFSR >> SCB_CFSR_BUSFAULTSR_Pos) & 0x80)
			fprintf(stderr, "BusFault fault at address 0x%08lX\n", SCB->BFAR);
		else
			fprintf(stderr, "BusFault fault\n");

		if((SCB->CFSR >> SCB_CFSR_BUSFAULTSR_Pos) & 0x01)
			fprintf(stderr, "- Instruction bus error\n");
		if((SCB->CFSR >> SCB_CFSR_BUSFAULTSR_Pos) & 0x02)
			fprintf(stderr, "- Precise Data bus error\n");
		if((SCB->CFSR >> SCB_CFSR_BUSFAULTSR_Pos) & 0x04)
			fprintf(stderr, "- Imprecise Data bus error\n");
		if((SCB->CFSR >> SCB_CFSR_BUSFAULTSR_Pos) & 0x08)
			fprintf(stderr, "- Exception when unstacking from exception\n");
		if((SCB->CFSR >> SCB_CFSR_BUSFAULTSR_Pos) & 0x10)
			fprintf(stderr, "- Exception when stacking for an exception\n");
		if((SCB->CFSR >> SCB_CFSR_BUSFAULTSR_Pos) & 0x20)
			fprintf(stderr, "- Fault when floating-point lazy state preservation\n");
	}

	//See http://infocenter.arm.com/help/topic/com.arm.doc.dui0553a/Cihcfefj.html#Cihgbdbi

	if((__get_IPSR() & 0xFF) == 6) {
		fprintf(stderr, "UsageFault fault, return address: 0x%08lX\n", stack_ptr[6]);

		if((SCB->CFSR >> SCB_CFSR_USGFAULTSR_Pos) & 0x001)
			fprintf(stderr, "- Undefined instruction\n");
		if((SCB->CFSR >> SCB_CFSR_USGFAULTSR_Pos) & 0x002)
			fprintf(stderr, "- Illegal use of the EPSR\n");
		if((SCB->CFSR >> SCB_CFSR_USGFAULTSR_Pos) & 0x004)
			fprintf(stderr, "- Illegal load of the PC\n");
		if((SCB->CFSR >> SCB_CFSR_USGFAULTSR_Pos) & 0x008)
			fprintf(stderr, "- Attempt to access a coprocessor but not present\n");
		if((SCB->CFSR >> SCB_CFSR_USGFAULTSR_Pos) & 0x100)
			fprintf(stderr, "- Unaligned memory access\n");
		if((SCB->CFSR >> SCB_CFSR_USGFAULTSR_Pos) & 0x200)
			fprintf(stderr, "- Divide by zero\n");
	}
	fprintf(stderr, "END of Fault Handler\n");
}

__attribute__((naked)) void Fault_Handler(void)
{
	//On ne veux pas perdre l'état des registres, donc pas de C
	//l'attribut naked indique qu'on ne veux pas de prologue / epilogue générés par GCC
	__asm volatile
	(
		"TST LR, #4\n"		// Test for MSP or PSP
		"ITE EQ\n"			//If equal
		"MRSEQ R0, MSP\n"	//r0 = msp
		"MRSNE R0, PSP\n"	//else r0 = psp
		"MOV R1, LR\n"
		"B dump_trap_info\n"
	);
}
void HardFault_Handler(void) __attribute__((alias("Fault_Handler"))) 	__attribute__((unused));
void MemManage_Handler(void) __attribute__((alias("Fault_Handler"))) 	__attribute__((unused));
void BusFault_Handler(void) __attribute__((alias("Fault_Handler"))) 	__attribute__((unused));
void UsageFault_Handler(void) __attribute__((alias("Fault_Handler"))) 	__attribute__((unused));

void NMI_Handler(void)
{
}

void SVC_Handler(void)
{
	fprintf(stderr, "SVC interrupt: unimplemented\n");
}

void DebugMon_Handler(void)
{
	fprintf(stderr, "DebugMon: unimplemented\n");
}

void PendSV_Handler(void)
{
	fprintf(stderr, "Pending SVC interrupt: unimplemented\n");
}

__attribute__((weak))
void SysTick_Handler(void)
{
	//Unused but not an error
}
