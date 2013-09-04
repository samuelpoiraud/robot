/* Copyright (c) 2010, Martin Thomas, 3BSD-license */

#define STDIO_TEST      0

#include <stdint.h>

#include <stdbool.h>
#include <stdlib.h>
#include "../../QS/QS_all.h"
#include "../../stm32f4xx/stm32f4xx_conf.h"
#include "../../stm32f4xx/stm32f4xx_flash.h"
#include "../../stm32f4xx/stm32f4xx_gpio.h"


#include "term_io.h"
#include "misc_test_term.h"

#include "Libraries/fat_sd/ff.h"


//Samuel pr dcompiler :
#define NumbOfVar 3


const uint16_t VirtAddVarTab[NumbOfVar] = { 1000, 1001, 1002 };


// command-line input-buffer
#define INBUF_MAX 20
static char inbuf[INBUF_MAX];
static int inbuf_idx;


// sampling-buffers
#define BUFFER_SIZE (1024)
#define NBUFFERS    3

typedef struct {
	size_t  cnt;
	uint8_t data[BUFFER_SIZE];
} Log_buffer_t;

Log_buffer_t log_buffers[NBUFFERS];
volatile size_t sample_buffer_idx;
size_t store_buffer_idx;
volatile bool overrun;
volatile bool sampling;
#define RECORD_SIZE (2*sizeof(uint32_t))
#define DWORD_TO_BYTEARRAY(ptr,val) \
	*(uint8_t*)(ptr)=(uint8_t)(val);\
	*(uint8_t*)((ptr)+1)=(uint8_t)((uint16_t)(val)>>8);\
	*(uint8_t*)((ptr)+2)=(uint8_t)((uint32_t)(val)>>16);\
	*(uint8_t*)((ptr)+3)=(uint8_t)((uint32_t)(val)>>24)
#define BYTEARRAY_TO_DWORD(ptr) \
	(uint32_t)(((uint32_t)*(uint8_t*)((ptr)+3)<<24) | \
	((uint32_t)*(uint8_t*)((ptr)+2)<<16) | \
	((uint16_t)*(uint8_t*)((ptr)+1)<<8) | \
	*(uint8_t*)(ptr))


// FFAT file-handling
FATFS fatfs;
FIL file;
#define SAMPLE_FILENAME "sampled.bin"
uint32_t pre_allocation_size = 1024*1024*200; /* 200 MBytes */

static const char ini_filename[] = "0:/TEST.INI";


/*
 * reminders: timer2 @ APB1, PCLK1=36MHz, TIM2 prescaler=4 -> TIM2 clock 7.2MHz
 * TIM2-CCR1 increment=144 -> TIM2_IT_CC1 hit frequency=0.1MHz
 * xprintf("t=%lu p=%lu\n", BYTEARRAY_TO_DWORD(&(record[0])), BYTEARRAY_TO_DWORD(&(record[4])) );
*/

#define CCR1_VAL      ((uint16_t)144)
#define BASEFREQUENCY ((uint32_t)100000) /* Hz */
#define FREQUENCY_TO_SOFTPRESCALE( freq ) ((BASEFREQUENCY/freq)-1)
#define SOFTPRESCALE_TO_FREQUENCY( sps )  (BASEFREQUENCY/(sps+1))
static volatile uint32_t soft_prescale = FREQUENCY_TO_SOFTPRESCALE( 1 );


#ifdef RAMFUNC
/* avoid long-calls to library-functions by direct register-access */
RAMFUNC void TIM2_IRQHandler(void)
{
	static uint32_t soft_prescale_cnt;

	static uint8_t  record[RECORD_SIZE];
	static uint32_t rec_count;
	uint32_t val1, val2;

	if (TIM2->SR & TIM_IT_CC1) {
		TIM2->SR = (uint16_t)~TIM_IT_CC1;
		TIM2->CCR1 += CCR1_VAL;

		if ( soft_prescale_cnt++ >= soft_prescale ) {
			soft_prescale_cnt = 0;
			if ( GPIO_LED->ODR & GPIO_Pin_LED4 ) {
				GPIO_LED->BRR = GPIO_Pin_LED4;
			} else {
				GPIO_LED->BSRR = GPIO_Pin_LED4;
			}

			if (sampling) {
				val1 = TIM2->CNT;
				val2 = rec_count++;
				DWORD_TO_BYTEARRAY(&record[0], val1);
				DWORD_TO_BYTEARRAY(&record[4], val2);

				Log_buffer_t* plb = &log_buffers[sample_buffer_idx];
				for ( size_t i = 0; i < RECORD_SIZE; i++ ) {
					if (plb->cnt < BUFFER_SIZE) {
						plb->data[plb->cnt++] = record[i];
					}
					if ( plb->cnt >= BUFFER_SIZE ) {
						if (sample_buffer_idx < ( NBUFFERS - 1 ) ) {
							sample_buffer_idx++;
						} else {
							sample_buffer_idx = 0;
						}
						if ( log_buffers[sample_buffer_idx].cnt != 0 ) {
							overrun = true;
						}
					}
				}
			}
		}
	}
}
#else
/*
void TIM2_IRQHandler(void)
{
	static uint32_t soft_prescale;

	if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET) {
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);

		if ( soft_prescale++ >= 100000 ) {
			soft_prescale = 0;
			GPIO_WriteBit( GPIO_LED, GPIO_Pin_LED3,
			         !( GPIO_ReadOutputDataBit( GPIO_LED, GPIO_Pin_LED3 ) ) );
		}
		TIM_SetCompare1(TIM2, TIM_GetCapture1(TIM2) + CCR1_Val);
	}
}*/
#endif /* RAMFUNC */


static void my_timer2_start(void)
{
	/*

	 NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	//* TIM2 clock enable
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	//* Enable the TIM2 global Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);

	//* Time base configuration
	TIM_TimeBaseStructure.TIM_Period = 65535;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	//* Prescaler configuration
	TIM_PrescalerConfig(TIM2, 4, TIM_PSCReloadMode_Immediate);

	//* Output Compare Timing Mode configuration: Channel1
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR1_VAL;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM2, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);

	 //* TIM IT enable
	TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);

	//* TIM2 enable counter
	TIM_Cmd(TIM2, ENABLE);
	*/
}


static void my_timer2_stop(void)
{
	//TIM_Cmd(TIM2, DISABLE);
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);
}


static void help(void)
{
	debug_printf("l              LED toggle\n");
	debug_printf("l 0|1          LED off/on\n");
	debug_printf("t              Show RTC time\n");
	debug_printf("t y m d h m s  Set RTC time\n");
	debug_printf("x              Exit\n");
	debug_printf("b 0|1          LED blink with TIM2/on\n");
	debug_printf("a              USE_FULL_ASSERT test\n");
	debug_printf("sf [<freq>]    sampling frequency\n");
	debug_printf("sp [<bytes>]   file preallocation size\n");
	debug_printf("sb             begin sampling\n");
	debug_printf("se             end sampling\n");
	debug_printf("so             open sample-file for read\n");
	debug_printf("sd <num>       dump num records\n");
	debug_printf("sc             close sample-file\n");
	debug_printf("ic             create test INI-file\n");
	debug_printf("ir             read test INI-file\n");
	debug_printf("v              view system information\n");
	debug_printf("ef             format emulated EEPROM\n");
	debug_printf("ew <idx> <v>   write into emulated EEEPROM\n");
	debug_printf("er [<idx>]     read from emulated EEEPROM\n");
	debug_printf("d              DCC test\n");
}




void full_assert_test(void)
{
#ifdef USE_FULL_ASSERT
	/* Simulate wrong parameter passed to library function -----------------*/
	/* To enable SPI1 clock, RCC_APB2PeriphClockCmd function must be used and
	 not RCC_APB1PeriphClockCmd */
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
#else
	debug_printf("USE_FULL_ASSERT not defined - test disabled\n");
#endif
}


static void init_buffers(void)
{
	size_t i;
	for (i = 0; i < NBUFFERS; i++ ) {
		log_buffers[i].cnt = 0;
	}
	sample_buffer_idx = 0;
	store_buffer_idx  = 0;
	overrun = FALSE;
	sampling = FALSE;
}

static void system_information(void)
{
	uint32_t v;
	volatile uint32_t* pw;
	volatile uint16_t* ph;
	debug_printf("unimplemented\n");
/*
	v = SCB->CPUID;
	xprintf("Implementer %s, ", ((v & SCB_CPUID_IMPLEMENTER) == (0x41 << 24)) ? "ARM" : "unknown" );
	xprintf("PartNo %s, ", ((v & SCB_CPUID_PARTNO) == (0xC23 << 4)) ? "Cortex-M3" : "unknown" );
	xprintf("Prod.-Vers.-ID r%ldp%ld\n", ((v & SCB_CPUID_VARIANT)) >> 20, v & SCB_CPUID_REVISION);
	v = SCB->VTOR;
	xprintf("Vector table at 0x%08lX in %s (TBLBASE)\n", v & SCB_VTOR_TBLOFF,
			(v & SCB_VTOR_TBLBASE) ? "SRAM" : "Code");
	v = *((volatile uint32_t*)0x1FFFF7E0);
	xprintf("Flash size %ld kBytes\n", v & 0xFFFF);
	ph = (volatile uint16_t*)0x1FFFF7E8;
	pw = (volatile uint32_t*)(0x1FFFF7E8+4);
	xprintf("Unique Device ID 0x%08lX:0x%08lX:0x%04X:0x%04X\n", *(pw+1), *pw, *(ph+1), *ph);
	xprintf("__STM32F10X_STDPERIPH_VERSION %d.%d.%d\n",
			__STM32F10X_STDPERIPH_VERSION_MAIN,
			__STM32F10X_STDPERIPH_VERSION_SUB1,
			__STM32F10X_STDPERIPH_VERSION_SUB2);
			*/
#if _FATFS
	debug_printf("ChaN FatFS Version %04X\n", _FATFS);
#ifdef STM32_SD_USE_DMA
	debug_printf("STM32 SD/MMC/SDHC Interface uses SPI DMA\n");
#else
	debug_printf("STM32 SD/MMC/SDHC Interface without DMA\n");
#endif
#endif /* _FATFS */
}

static bool flash_is_locked(void)
{
	return (FLASH->CR & (1<<7) /* Bit 7 LOCK */) ? true : false;
}

static bool my_FLASH_Unlock(void)
{
	if ( flash_is_locked() ) {
		FLASH_Unlock();
		FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGPERR | FLASH_FLAG_WRPERR);
	}
	return flash_is_locked() ? false : true;
}

static bool my_FLASH_Lock(void)
{
	if ( !flash_is_locked() ) {
		FLASH_Lock();
	}
	return flash_is_locked() ? true : false;
}






bool_e misc_execute_command(char * ptr)
{
	static bool_e overrunalerted;
	bool_e retval = TRUE;
	long p1, p2;
	//RTC_t rtc;
	unsigned int i, s2;


	switch (*ptr++)
	{
		case 'x':
			retval = FALSE;
			break;
		case 'h':
			help();
			break;
		case 'v':
			system_information();
			break;
		case 'd':
			//dcc_tests();
			break;
		case 'l':
			if (xatoi(&ptr, &p1)) {
				if (p1) {
				//	GPIO_SetBits(GPIO_LED, GPIO_Pin_LED3 );
				} else {
					//GPIO_ResetBits(GPIO_LED, GPIO_Pin_LED3 );
				}
			} else {
				//GPIO_WriteBit(GPIO_LED, GPIO_Pin_LED3,
				//!(GPIO_ReadOutputDataBit(GPIO_LED, GPIO_Pin_LED3 )));
			}
			break;
		case 't':
			/*if (xatoi(&ptr, &p1)) {
				rtc.year = (uint16_t) p1;
				xatoi(&ptr, &p1);
				rtc.month = (uint8_t) p1;
				xatoi(&ptr, &p1);
				rtc.mday = (uint8_t) p1;
				xatoi(&ptr, &p1);
				rtc.hour = (uint8_t) p1;
				xatoi(&ptr, &p1);
				rtc.min = (uint8_t) p1;
				if (!xatoi(&ptr, &p1))
					break;
				rtc.sec = (uint8_t) p1;
				rtc_settime(&rtc);
			}
			rtc_gettime(&rtc);
			xprintf("%u/%u/%u %02u:%02u:%02u\n", rtc.year, rtc.month, rtc.mday,
					rtc.hour, rtc.min, rtc.sec);
					*/
			debug_printf("Rtc unimplemented\n");
			break;
		case 'b':
			if (xatoi(&ptr, &p1)) {
				if (p1) {
					//my_timer2_start();
				} else {
					my_timer2_stop();
				}
			}
			break;
		case 'a':
			full_assert_test();
			break;
		case 's':
			switch (*ptr++) {
			case 'f': // sf [<freq>] - set sampling frequency [Hz]
				if (xatoi(&ptr, &p1)) {
					soft_prescale = FREQUENCY_TO_SOFTPRESCALE(p1);
					if ( !soft_prescale ) soft_prescale = 1;
				}
				debug_printf("Sampling Frequency %lu Hz (soft_prescale %lu)\n",
						SOFTPRESCALE_TO_FREQUENCY(soft_prescale),
						soft_prescale);
				break;
			case 'p': // sp [<bytes>] - pre-allocation size
				if (xatoi(&ptr, &p1)) {
					pre_allocation_size = p1;
				}
				debug_printf("Preallocation size %lu Bytes\n", pre_allocation_size);
				break;
			case 'b': // sb - begin sampling
				init_buffers();
				if ( f_mount( 0, &fatfs ) != FR_OK ) {
					debug_printf("f_mount failed\n");
				} else {
					if ( f_open( &file, SAMPLE_FILENAME, FA_CREATE_ALWAYS | FA_WRITE ) != FR_OK ) {
						debug_printf("f_open failed\n");
					} else {
						debug_printf("Preallocation of %lu Bytes ... ", pre_allocation_size);
						if ( f_lseek( &file, pre_allocation_size ) == FR_OK ) {
							if ( file.fptr != pre_allocation_size ) {
								debug_printf("failed (size)\n");
							}
							else {
								f_lseek( &file, 0L );
								overrunalerted = FALSE;
								sampling = true;
								//my_timer2_start();
								debug_printf("done.\nSampling into file %s started\n", SAMPLE_FILENAME);
							}
						} else {
							debug_printf("failed (seek)\n");
						}
					}
				}
				break;
			case 'e': // se - end sampling
				sampling = FALSE;
				for ( i = 0; i < NBUFFERS; i++ ) {
					Log_buffer_t* plb = &log_buffers[store_buffer_idx];
					if ( plb->cnt ) {
						unsigned int written;
						FRESULT res;
						debug_printf("Flushing Log-Buffer %u\n", store_buffer_idx);
						res = f_write(&file, plb->data, plb->cnt, &written);
						if ((res != FR_OK) || (written != plb->cnt)) {
							sampling = FALSE;
							debug_printf("f_write error during shutdown\n");
						}
						plb->cnt = 0;
						if (store_buffer_idx < (NBUFFERS - 1)) {
							store_buffer_idx++;
						} else {
							store_buffer_idx = 0;
						}
					}
				}
				f_truncate(&file);
				f_close(&file);
				debug_printf("Sampling ended\n");
				break;
			case 'i': // si - sampling-buffers state
				if (sampling) {
					for (i = 0; i < NBUFFERS; i++) {
						debug_printf("Buffer#%u count=%lu\n", i, (volatile uint32_t) log_buffers[i].cnt);
					}
					debug_printf("overrun-state: %s\n", (overrun) ? "OVERRUN"
							: "none");
					debug_printf("SampBufIdx %u   StoreBufIdx %u\n", sample_buffer_idx, store_buffer_idx);
				} else {
					debug_printf("Sampling not active\n");
				}
				break;
			case 'o' : // so - open file for read-access
				if ( !sampling ) {
					if ( f_mount( 0, &fatfs ) != FR_OK ) {
						debug_printf("f_mount failed\n");
					} else {
						if ( f_open( &file, SAMPLE_FILENAME, FA_READ ) != FR_OK ) {
							debug_printf("f_open failed\n");
						}
					}
				}
				break;
			case 'd' : // sd <n> - dump n records
				if ( !xatoi( &ptr, &p1 ) ) {
					p1 = 1;
				}
				uint32_t prev=0;
				bool first = true;
				for ( i = 0; i < (unsigned)p1; i++ ) {
					if ( f_read( &file, log_buffers[0].data, RECORD_SIZE, &s2 ) != FR_OK ) {
						debug_printf("f_read error\n");
						break;
					}
					if ( s2 != RECORD_SIZE ) {
						debug_printf("EOF\n");
						break;
					} else {
						uint32_t ts = BYTEARRAY_TO_DWORD( log_buffers[0].data );
						uint32_t pl = BYTEARRAY_TO_DWORD( log_buffers[0].data+4 );
						if ( first ) {
							debug_printf("t:%6lu  l:%6lu\n", ts, pl);
							first = FALSE;
						} else {
							uint32_t delta = (uint16_t)((uint16_t)ts-(uint16_t)prev);
							uint32_t delta_us = delta * (1000000UL/BASEFREQUENCY) / CCR1_VAL;
							debug_printf("t:%6lu  l:%6lu (delta %lu, %lu nsec)\n", ts, pl, delta, delta_us);
						}
						prev = ts;
					}

				}
				break;
			case 'c' : // sc - close file
				if ( !sampling ) {
					f_close( &file );
				}
				break;
			}
			break;
		case 'e' :
			switch (*ptr++) {
				case 'f': // ef - format virtual EEPROM
					if ( my_FLASH_Unlock() ) {
						//if (EE_Format() != FLASH_COMPLETE) { xprintf("Format failed\n"); }
						//else { xprintf("Format o.k.\n"); }
						my_FLASH_Lock();
					} else {
						debug_printf("FLASH_Unlock failed\n");
					}
					break;
				case 'w': // ew <idx> <val> - write to virtual EEPROM
					// here: p1 = index in virtual address array
					if (xatoi(&ptr, &p1) && xatoi(&ptr, &p2)) {
						if ( p1 < NumbOfVar ) {
							p1 = VirtAddVarTab[p1];
							debug_printf("write EEMUL[%d] := %d ", (uint16_t)p1, (uint16_t)p2);
							if ( my_FLASH_Unlock() ) {
								/*if ( EE_WriteVariable(p1, p2) == FLASH_COMPLETE ) {
									xprintf("o.k.\n");
								} else {
									xprintf("failed\n");
								}*/
								debug_printf("EEPROM not implemented\n");
								my_FLASH_Lock();
							} else {
								debug_printf("FLASH_Unlock failed\n");
							}
						} else {
							debug_printf("Index out of range\n");
						}
					} else {
						debug_printf("Parameter error\n");
					}
					break;
				case 'r': { // er [<idx>] - read from virtual EEPROM
					uint16_t r;
					if (xatoi(&ptr, &p1)) {
						if ( p1 < NumbOfVar ) {
							p1 = VirtAddVarTab[p1];
							/*if (EE_ReadVariable((uint16_t)p1, &r) == 0) {
								xprintf("read EEMUL[%d] = %d\n", (uint16_t)p1,
										(uint16_t) r);
							} else {
								xprintf("read EEMUL[%d] failed!\n", (uint16_t)p1);
							}*/
							debug_printf("EEPROM not implemented\n");
						} else {
							debug_printf("Index out of range\n");
						}
					} else {
						for (i = 0; i < NumbOfVar; i++) {
							p1 = VirtAddVarTab[i];
							/*if (EE_ReadVariable(p1, &r) == 0) {
								xprintf("read EEMUL[%d] = %d\n", (uint16_t)p1,
										(uint16_t) r);
							} else {
								xprintf("read EEMUL[%d] failed!\n", (uint16_t)p1);
							}*/
							debug_printf("EEPROM not implemented\n");
						}
					}
				}
			}
			break;
		}



	if ( overrun && !overrunalerted ) {
		debug_printf("\n*** OVERRUN ***\n");
		overrunalerted = true;
	}

	if ( sampling ) {
		Log_buffer_t* plb = &log_buffers[store_buffer_idx];
		if ( plb->cnt >= BUFFER_SIZE ) {
			//xprintf("Storing Buffer %u - ", store_buffer_idx);
			unsigned int written;
			FRESULT res;
			res = f_write(&file, plb->data, plb->cnt, &written);
			if ( ( res != FR_OK ) || ( written != plb->cnt ) ) {
				sampling = FALSE;
				debug_printf("f_write error - sampling stopped\n");
				f_close(&file);
			}
			//xprintf("done\n");
			plb->cnt = 0;
			if ( store_buffer_idx < ( NBUFFERS - 1 ) ) {
				store_buffer_idx++;
			} else {
				store_buffer_idx = 0;
			}
		}
	}

	return retval;
}


bool_e misc_test_term (void)
{
	static char *ptr;
	bool_e ret;
	//RTC_t rtc;

	typedef enum
	{
		INIT = 0,
		SEND_PROMPT,
		WAIT_COMMAND,
		EXECUTE_COMMAND
	}state_e;
	static state_e state = INIT;

	ret = TRUE;	//On garde la main

	switch (state)
	{
		case INIT:
			debug_printf("\nMisc test terminal (h for help)\n");
			state = SEND_PROMPT;
			break;
		case SEND_PROMPT:
			UART1_putc('>');
			state = WAIT_COMMAND;
			break;
		case WAIT_COMMAND:
			ptr = get_command();
			if(ptr)
				state = EXECUTE_COMMAND;
			break;
		case EXECUTE_COMMAND:
			ret = misc_execute_command(ptr);
			if(ret)
				state = SEND_PROMPT;
			else
				state = INIT;
			break;
	}
	return ret;
}


