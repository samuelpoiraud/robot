/*
 *	Club Robot ESEO 2009 - 2010
 *	Chomp
 *
 *	Fichier : QS_sys.c
 *	Package : Qualite Soft
 *	Description : Configuration du STM32 - Horloges - Démarrage
 *	Auteur : Gwenn
 *	Version 20100421
 */

#include "QS_sys.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_flash.h"
#include "QS_outputlog.h"
#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>
#include <stdio.h>

#ifndef STDOUT_USART
	#define STDOUT_USART 1
#endif

#ifndef STDERR_USART
	#define STDERR_USART 1
#endif

#ifndef STDIN_USART
	#define STDIN_USART 1
#endif

#define HCLK_CHOOSEN_DIV  1
#define PCLK1_CHOOSEN_DIV 4
#define PCLK2_CHOOSEN_DIV 2

#include "QS_clocks_freq.h"

#if 1      //Pour pouvoir fold le code dessous (car long et utile que en cas de probl�me ...)

	//V�rification des valeurs, si elles sont bien celles voulu par l'utilisateur

	#define HCLK_DIV	FORCED_HCLK_DIV	//HCLK = SYSCLK_HZ / HCLK_DIV
	#define PCLK1_DIV	(HCLK_FREQUENCY_HZ/PCLK1_FREQUENCY_HZ)	//PCLK1 = HCLK_DIV / PCLK1_DIV
	#define PCLK2_DIV	(HCLK_FREQUENCY_HZ/PCLK2_FREQUENCY_HZ)	//PCLK2 = HCLK_DIV / PCLK2_DIV

	//VCO_INPUT_HZ = CPU_EXTERNAL_CLOCK_HZ / PLLM
	//VCO_OUTPUT_HZ = VCO_INPUT_HZ * PLLN
	//SYSCLK_HZ = VCO_OUTPUT_HZ / PLLP
	//USB_RNG_SDIO_CLK_HZ = SYSCLK_HZ / PLLQ
	//SYSCLK = CPU_FREQUENCY_HZ = CPU_EXTERNAL_CLOCK_HZ / PLLM * PLLN / PLLP


	#define PLLM (CPU_EXTERNAL_CLOCK_HZ / VCO_INPUT_HZ)
	#define PLLN (VCO_OUTPUT_HZ  / CPU_EXTERNAL_CLOCK_HZ * PLLM)	//On utilise pas directement VCO_INPUT_HZ car il peut ne pas pouvoir �tre exact
	#define PLLP FORCED_PLLP
	#define PLLQ (VCO_OUTPUT_HZ / USB_RNG_SDIO_CLK_HZ)

	#define FLASH_WAIT_CYCLES (HCLK_FREQUENCY_HZ / 30000000)	//Voir page 62 du manuel de r�ference RM0090

	#if HCLK_FREQUENCY_HZ != ((((CPU_EXTERNAL_CLOCK_HZ / PLLM) * PLLN) / PLLP) / HCLK_DIV)
		#warning "Computed HCLK frequency is not exactly HCLK_FREQUENCY_HZ"
	#endif

	#if PCLK1_FREQUENCY_HZ != ((((CPU_EXTERNAL_CLOCK_HZ / PLLM) * PLLN) / PLLP) / HCLK_DIV / PCLK1_DIV)
		#warning "Computed PCLK1 frequency is not exactly PCLK1_FREQUENCY_HZ"
	#endif

	#if PCLK2_FREQUENCY_HZ != ((((CPU_EXTERNAL_CLOCK_HZ / PLLM) * PLLN) / PLLP) / HCLK_DIV / PCLK2_DIV)
		#warning "Computed PCLK2 frequency is not exactly PCLK2_FREQUENCY_HZ"
	#endif

	#if USB_RNG_SDIO_CLK_HZ != (((CPU_EXTERNAL_CLOCK_HZ / PLLM) * PLLN) / PLLQ)
		#warning "USB Frequency is not exactly USB_RNG_SDIO_CLK_HZ"
	#endif

	#if VCO_INPUT_HZ < 1000000
		#error "VCO_INPUT_HZ must be >= 1Mhz"
	#elif VCO_INPUT_HZ > 2000000
		#error "VCO_INPUT_HZ must <= 2Mhz"
	#endif

	#if VCO_OUTPUT_HZ < 192000000
		#error "VCO_OUTPUT_HZ must be >= 192Mhz"
	#elif VCO_OUTPUT_HZ > 432000000
		#error "VCO_OUTPUT_HZ must be <= 432Mhz"
	#endif

	#if PLLM < 0 || PLLM > 63
		#error "PLLM be be a unsigned integer <= 63"
	#endif

	#if PLLN < 192 || PLLN > 432
		#error "PLLN must be >= 192 and <= 432"
	#endif

	#if PLLP != 2 && PLLP != 4 && PLLP != 6 && PLLP != 8
		#error "PLLP must be either 2, 4, 6 or 8"
	#endif

	#if PLLQ < 4 || PLLQ > 15
		#error "PLLQ must be >= 4 and <= 15"
	#endif

	#if HCLK_DIV != HCLK_CHOOSEN_DIV
		#warning "HCLK_DIV is not HCLK_CHOOSEN_DIV"
	#endif

	#if PCLK1_DIV != PCLK1_CHOOSEN_DIV
		#warning "PCLK1_DIV is not PCLK1_CHOOSEN_DIV"
	#endif

	#if PCLK2_DIV != PCLK2_CHOOSEN_DIV
		#warning "PCLK2_DIV is not PCLK2_CHOOSEN_DIV"
	#endif

	#if (SYSCLK_HZ / HCLK_DIV) > 168000000
		#error "HCLK Frequency is too high, increase HCLK_DIV"
		#error "Une frequence d'horloge HCLK < 168Mhz doit �tre selectionn�e"
	#endif

	#if (SYSCLK_HZ / HCLK_DIV / PCLK1_DIV) > 42000000
		#error "PCLK1 Frequency is too high, increase PCLK1_DIV"
		#error "Une frequence d'horloge PCLK1 < 42Mhz doit �tre selectionn�e"
	#endif

	#if (SYSCLK_HZ / HCLK_DIV / PCLK2_DIV) > 84000000
		#error "PCLK2 Frequency is too high, increase PCLK2_DIV"
		#error "Une frequence d'horloge PCLK2 < 84Mhz doit �tre selectionn�e"
	#endif

	#if FLASH_WAIT_CYCLES > 7
		#warning "FLASH_WAIT_CYCLES > 7, HCLK_FREQUENCY_HZ est plus grand que 168Mhz ?"
		#undef FLASH_WAIT_CYCLES
		#define FLASH_WAIT_CYCLES 7
	#endif

#endif

void SYS_init(void)
{
	RCC_DeInit();

	/* Oscillateur externe */
	ErrorStatus HSEStartUpStatus;
	RCC_HSEConfig(RCC_HSE_ON);
	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	if(HSEStartUpStatus != ERROR)
	{
		//Voir page 60 du manuel de reference
		FLASH_SetLatency(FLASH_WAIT_CYCLES);

		//D�fini la clock HSE pour avoir des valeurs correcte pour RCC_GetClocksFreq()
		RCC_SetHSEFreq(CPU_EXTERNAL_CLOCK_HZ);

		/* PCLK1 = HCLK/2, PCLK2 = HCLK | HCLK = SYSCLK */
		//Pour savoir si les valeurs sont correctes, veuillez changer HCLK_CHOOSEN_DIV, PCLK1_CHOOSEN_DIV et PCLK2_CHOOSEN_DIV. Une erreur de pr�compilation indiquera s'il y a un probl�me
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div4);
		RCC_PCLK2Config(RCC_HCLK_Div2);

		RCC_PLLConfig(RCC_PLLSource_HSE, PLLM, PLLN, PLLP, PLLQ);

		/* Enable PLL1 */
		RCC_PLLCmd(ENABLE);
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET){}

		/* Select PLL as system clock source */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		while (RCC_GetSYSCLKSource() != 0x08){}
	}

	SystemCoreClockUpdate();

	//Pas de subpriority sur les interruptions
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	//Activation de l'exception Division par 0
	SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk;	//

	//Config LibC: no buffering
	setvbuf(stdout, NULL, _IONBF, 0 );
	setvbuf(stderr, NULL, _IONBF, 0 );
	setvbuf(stdin, NULL, _IONBF, 0 );

}

#if 0

#define MPU_BASE_ADDR (0xE000ED90)

typedef struct
{
	struct
	{
		int SEPARATE :1;
		int :7; // reserved (warning : mistake in datasheet)
		int DREGION :8;
		int IREGION :8;
		int :8; // reserved
	} MPU_TYPER;
	struct
	{
		int ENABLE :1;
		int HFNMIENA :1;
		int PRIVDEFENA :1;
		int :29; // reserved
	} MPU_CTRL;
	struct
	{
		int REGION :8;
		int :24; // reserved
	} MPU_RNR;
	union
	{
		unsigned int value;
		struct
		{
			int REGION :4;
			int VALID :1;
			int ADDR :27;
		};
	}	MPU_RBAR;
	struct
	{
		int ENABLE :1;
		int SIZE :5;
		int :2; // reserved
		int SRD :8;
		int B :1;
		int C :1;
		int S :1;
		int TEX :3;
		int :2; // reserved
		int AP :3;
		int :1; // reserved
		int XN :1;
		int :3; // reserved
	} MPU_RASR;
	struct
	{
		int REGION :4;
		int VALID :1;
		int ADDR :27;
	} MPU_RBAR_A1;
	struct
	{
		int ENABLE :1;
		int SIZE :5;
		int :2; // reserved
		int SRD :8;
		int B :1;
		int C :1;
		int S :1;
		int TEX :3;
		int :2; // reserved
		int AP :3;
		int :1; // reserved
		int XN :1;
		int :3; // reserved
	} MPU_RASR_A1;
	struct
	{
		int REGION :4;
		int VALID :1;
		int ADDR :27;
	} MPU_RBAR_A2;
	struct
	{
		int ENABLE :1;
		int SIZE :5;
		int :2; // reserved
		int SRD :8;
		int B :1;
		int C :1;
		int S :1;
		int TEX :3;
		int :2; // reserved
		int AP :3;
		int :1; // reserved
		int XN :1;
		int :3; // reserved
	} MPU_RASR_A2;
	struct
	{
		int REGION :4;
		int VALID :1;
		int ADDR :27;
	} MPU_RBAR_A3;
	struct
	{
		int ENABLE :1;
		int SIZE :5;
		int :2; // reserved
		int SRD :8;
		int B :1;
		int C :1;
		int S :1;
		int TEX :3;
		int :2; // reserved
		int AP :3;
		int :1; // reserved
		int XN :1;
		int :3; // reserved
	} MPU_RASR_A3;
} MpuRegs_t;

MpuRegs_t *MpuRegs = ((MpuRegs_t *) MPU_BASE_ADDR);

void init_mpu (void)
{
	extern uint8_t _susrstack;
	unsigned int rodata;
	rodata = (unsigned int)&_susrstack;
	// general parameters
	MpuRegs->MPU_TYPER.SEPARATE = 0; // unified MPU (data and instruction are the same accesses)
	MpuRegs->MPU_TYPER.IREGION = 0;  // no regions for instruction accesses
	MpuRegs->MPU_TYPER.DREGION = 8;  // 8 data regions

	// region 0: default code region
	MpuRegs->MPU_RBAR.value = 1 << 4 | 0 | 0x08000000; // code area
	MpuRegs->MPU_RASR.XN = 0;        // can fetch instructions
	MpuRegs->MPU_RASR.AP = 7;				 // permission = read/write
	MpuRegs->MPU_RASR.TEX = 0;       // for bits TEX, C, B et S, see table #42 at page 186 of PM0214
	MpuRegs->MPU_RASR.C = 1;
	MpuRegs->MPU_RASR.B = 0;
	MpuRegs->MPU_RASR.S = 0;
	MpuRegs->MPU_RASR.SRD = 0;
	MpuRegs->MPU_RASR.SIZE = 19;     // region size = 1MiB
	MpuRegs->MPU_RASR.ENABLE = 1;    // enable region

	// region 1: default variable area
	MpuRegs->MPU_RBAR.value = 1 << 4 | 1 | 0x20000000; // variable area
	MpuRegs->MPU_RASR.XN = 0;        // no instruction fetch
	MpuRegs->MPU_RASR.AP = 3;				 // permission = read/write
	MpuRegs->MPU_RASR.TEX = 0;       // for bits TEX, C, B et S, see table #42 at page 186 of PM0214
	MpuRegs->MPU_RASR.C = 1;
	MpuRegs->MPU_RASR.B = 0;
	MpuRegs->MPU_RASR.S = 1;
	MpuRegs->MPU_RASR.SRD = 0;
	MpuRegs->MPU_RASR.SIZE = 19;     // region size = 1MiB
	MpuRegs->MPU_RASR.ENABLE = 1;    // enable region

	// region 2: periph area
	MpuRegs->MPU_RBAR.value = 1 << 4 | 2 | 0x40000000; // adresse de base de la r�gion + n� de la r�gion
	MpuRegs->MPU_RASR.XN = 1;        // interdiction de lire des instructions
	MpuRegs->MPU_RASR.AP = 3;				 // permission = lecture/�criture
	MpuRegs->MPU_RASR.TEX = 0;       // for bits TEX, C, B et S, see table #42 at page 186 of PM0214
	MpuRegs->MPU_RASR.C = 1;
	MpuRegs->MPU_RASR.B = 0;
	MpuRegs->MPU_RASR.S = 1;
	MpuRegs->MPU_RASR.SRD = 0;
	MpuRegs->MPU_RASR.SIZE = 30;			 // taille de la r�gion
	MpuRegs->MPU_RASR.ENABLE = 1;    // activer la r�gion

	// region 3: read only variable
	MpuRegs->MPU_RBAR.value = 1 << 4 | 3 | (rodata); // adresse de base de la r�gion + n� de la r�gion
	MpuRegs->MPU_RASR.XN = 1;        //  interdiction de lire des instructions
	MpuRegs->MPU_RASR.AP = 7;				 // permission = lecture seule
	MpuRegs->MPU_RASR.TEX = 0;       // for bits TEX, C, B et S, see table #42 at page 186 of PM0214
	MpuRegs->MPU_RASR.C = 1;
	MpuRegs->MPU_RASR.B = 0;
	MpuRegs->MPU_RASR.S = 1;
	MpuRegs->MPU_RASR.SRD = 0;
	MpuRegs->MPU_RASR.SIZE = 1;			 // taille de la r�gion = 4 octets
	MpuRegs->MPU_RASR.ENABLE = 1;    //  activer la r�gion

/*	// region 4: forbidden variable
	MpuRegs->MPU_RBAR.value = 1 << 4 | 4 | forbidden_address; // adresse de base de la r�gion + n� de la r�gion
	MpuRegs->MPU_RASR.XN = 1;        // interdiction de lire des instructions
	MpuRegs->MPU_RASR.AP = 0;				 // permission = rien
	MpuRegs->MPU_RASR.TEX = 0;       // for bits TEX, C, B et S, see table #42 at page 186 of PM0214
	MpuRegs->MPU_RASR.C = 1;
	MpuRegs->MPU_RASR.B = 0;
	MpuRegs->MPU_RASR.S = 1;
	MpuRegs->MPU_RASR.SRD = 0;
	MpuRegs->MPU_RASR.SIZE = 4;			 // taille de la r�gion = 32 octets
	MpuRegs->MPU_RASR.ENABLE = 1;    // activer la r�gion
*/
	MpuRegs->MPU_CTRL.ENABLE = 1;    // activer la MPU
}





void stack_overflow_detection(void)
{
	init_mpu();
//	extern uint8_t _susrstack;
	//_susrstack = 0x55;

}

#endif


void SYS_check_stack_level(void)
{
	extern uint8_t __Stack_Init;
	extern uint8_t _estack;
	uint32_t * a;
	for(a = (uint32_t *)(&__Stack_Init+4); a<(uint32_t *)(&_estack); a++)
	{
		if(*a != 0x55555555)
			break;
	}
	a = (uint32_t *)((uint32_t *)&_estack - a);
	info_printf("stack max occupation : %ld\n",(uint32_t)(a));
}



__attribute__((naked)) int SYS_Read_Interrupt_state(void)
{
	//l'attribut naked indique qu'on ne veux pas de prologue / epilogue g�n�r�s par GCC
	__asm volatile
	(
		"MRS R0, IPSR\n"
		"BX LR\n"
	);
	return 0;	//Jamais atteint. Uniquement pour satisfaire le compilo C.
}


/*  Fonctions appel�es par la libc (comme printf)  */
// https://sites.google.com/site/stm32discovery/open-source-development-with-the-stm32-discovery/getting-newlib-to-work-with-stm32-and-code-sourcery-lite-eabi
/*
 environ
 A pointer to a list of environment variables and their values.
 For a minimal environment, this empty list is adequate:
 */
char *__env[1] = { 0 };
char **environ = __env;

__attribute__((weak))
int _write(int file, char *ptr, int len);

__attribute__((weak))
void _exit(int status) {
	_write(1, "exit", 4);
	while (1) {
		;
	}
}

__attribute__((weak))
int _close(int file) {
	return -1;
}

/*
 execve
 Transfer control to a new process. Minimal implementation (for a system without processes):
 */
__attribute__((weak))
int _execve(char *name, char **argv, char **env) {
	errno = ENOMEM;
	return -1;
}

/*
 fork
 Create a new process. Minimal implementation (for a system without processes):
 */
__attribute__((weak))
int _fork() {
	errno = EAGAIN;
	return -1;
}

/*
 fstat
 Status of an open file. For consistency with other minimal implementations in these examples,
 all files are regarded as character special devices.
 The `sys/stat.h' header file required is distributed in the `include' subdirectory for this C library.
 */
__attribute__((weak))
int _fstat(int file, struct stat *st) {
	st->st_mode = S_IFCHR;
	return 0;
}

/*
 getpid
 Process-ID; this is sometimes used to generate strings unlikely to conflict with other processes. Minimal implementation, for a system without processes:
 */
__attribute__((weak))
int _getpid() {
	return 1;
}

/*
 isatty
 Query whether output stream is a terminal. For consistency with the other minimal implementations,
 */
__attribute__((weak))
int _isatty(int file) {
	switch (file){
		case STDOUT_FILENO:
		case STDERR_FILENO:
		case STDIN_FILENO:
			return 1;
		default:
			//errno = ENOTTY;
			errno = EBADF;
			return 0;
	}
}

/*
 kill
 Send a signal. Minimal implementation:
 */
__attribute__((weak))
int _kill(int pid, int sig) {
	errno = EINVAL;
	return (-1);
}

/*
 link
 Establish a new name for an existing file. Minimal implementation:
 */
__attribute__((weak))
int _link(char *old, char *new) {
	errno = EMLINK;
	return -1;
}

/*
 lseek
 Set position in a file. Minimal implementation:
 */
__attribute__((weak))
int _lseek(int file, int ptr, int dir) {
	return 0;
}

/*
 sbrk
 Increase program data space.
 Malloc and related functions depend on this
 */
char toHex(char v) {
	v &= 0x0F;

	if(v > 0x9)
		return v - 0xA + 'A';
	else
		return v + '0';
}

__attribute__((weak))
caddr_t _sbrk(int incr) {
	extern char _ebss; // Defined by the linker
	static char *heap_end;
	char *prev_heap_end;

	if (heap_end == 0) {
		heap_end = &_ebss;
	}
	prev_heap_end = heap_end;

	char * stack = (char*) __get_MSP();
	if (heap_end + incr >  stack)
	{
		int i;
		char integer[8];
		_write (STDERR_FILENO, "Heap and stack collision: ", 26);

		for(i = 0; i < 8; i++)
			integer[i] = toHex((int)heap_end >> ((7-i)*4));

		_write (STDERR_FILENO, integer, 8);
		_write (STDERR_FILENO, " + ", 3);

		for(i = 0; i < 8; i++)
			integer[i] = toHex(incr >> ((7-i)*4));

		_write (STDERR_FILENO, integer, 8);

		_write (STDERR_FILENO, " > ", 3);

		for(i = 0; i < 8; i++)
			integer[i] = toHex((int)stack >> ((7-i)*4));

		_write (STDERR_FILENO, integer, 8);
		_write (STDERR_FILENO, "\n", 1);
		errno = ENOMEM;
		return  (caddr_t) -1;
		//abort ();
	}

	heap_end += incr;
	return (caddr_t) prev_heap_end;
}

/*
 read
 Read a character to a file. `libc' subroutines will use this system routine for input from all files, including stdin
 Returns -1 on error or blocks until the number of characters have been read.
 */
__attribute__((weak))
int _read(int file, char *ptr, int len) {
	int n;
	int num = 0;
	switch (file) {
		case STDIN_FILENO:
			for (n = 0; n < len; n++) {
#if   STDIN_USART == 1
				while ((USART1->SR & USART_FLAG_RXNE) == (uint16_t)RESET) {}
				char c = (char)(USART1->DR & (uint16_t)0x01FF);
#elif STDIN_USART == 2
				while ((USART2->SR & USART_FLAG_RXNE) == (uint16_t) RESET) {}
				char c = (char) (USART2->DR & (uint16_t) 0x01FF);
#elif STDIN_USART == 3
				while ((USART3->SR & USART_FLAG_RXNE) == (uint16_t)RESET) {}
				char c = (char)(USART3->DR & (uint16_t)0x01FF);
#endif
				*ptr++ = c;
				num++;
			}
			break;
		default:
			errno = EBADF;
			return -1;
	}
	return num;
}

/*
 stat
 Status of a file (by name). Minimal implementation:
 int    _EXFUN(stat,( const char *__path, struct stat *__sbuf ));
 */
__attribute__((weak))
int _stat(const char *filepath, struct stat *st) {
	st->st_mode = S_IFCHR;
	return 0;
}

/*
 times
 Timing information for current process. Minimal implementation:
 */
__attribute__((weak))
clock_t _times(struct tms *buf) {
	return -1;
}

/*
 unlink
 Remove a file's directory entry. Minimal implementation:
 */
__attribute__((weak))
int _unlink(char *name) {
	errno = ENOENT;
	return -1;
}

/*
 wait
 Wait for a child process. Minimal implementation:
 */
__attribute__((weak))
int _wait(int *status) {
	errno = ECHILD;
	return -1;
}

/*
 write
 Write a character to a file. `libc' subroutines will use this system routine for output to all files, including stdout
 Returns -1 on error or number of bytes sent
 */
__attribute__((weak))
int _write(int file, char *ptr, int len) {
	int n;
	switch (file) {
		case STDOUT_FILENO: /*stdout*/
			for (n = 0; n < len; n++) {
#if STDOUT_USART == 1
				while ((USART1->SR & USART_FLAG_TC) == (uint16_t)RESET) {}
				USART1->DR = (*ptr++ & (uint16_t)0x01FF);
#elif  STDOUT_USART == 2
				while ((USART2->SR & USART_FLAG_TC) == (uint16_t) RESET) {
				}
				USART2->DR = (*ptr++ & (uint16_t) 0x01FF);
#elif  STDOUT_USART == 3
				while ((USART3->SR & USART_FLAG_TC) == (uint16_t)RESET) {}
				USART3->DR = (*ptr++ & (uint16_t)0x01FF);
#endif
			}
			break;
		case STDERR_FILENO: /* stderr */
			for (n = 0; n < len; n++) {
#if STDERR_USART == 1
				while ((USART1->SR & USART_FLAG_TC) == (uint16_t)RESET) {}
				USART1->DR = (*ptr++ & (uint16_t)0x01FF);
#elif  STDERR_USART == 2
				while ((USART2->SR & USART_FLAG_TC) == (uint16_t) RESET) {
				}
				USART2->DR = (*ptr++ & (uint16_t) 0x01FF);
#elif  STDERR_USART == 3
				while ((USART3->SR & USART_FLAG_TC) == (uint16_t)RESET) {}
				USART3->DR = (*ptr++ & (uint16_t)0x01FF);
#endif
			}
			break;
		default:
			errno = EBADF;
			return -1;
	}
	return len;
}