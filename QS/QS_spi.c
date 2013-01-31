/*
 *	Club Robot ESEO 2010 - 2011
 *	???
 *
 *	Fichier : QS_spi.c
 *	Package : Qualité Soft
 *	Description : Fonction SPI
 *	Auteur : Ronan
 *  Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100929
 */
 
#define QS_SPI_C
#include "QS_spi.h"

#include <spi.h>

#ifdef USE_SPI1
static Uint8 m_s1rbuf[SPI_R_BUF_SIZE];
static volatile Uint8 m_s1rnum;
#endif /* def USE_SPI1 */

#ifdef USE_SPI2
static Uint8 m_s2rbuf[SPI_R_BUF_SIZE];
static volatile Uint8 m_s2rnum;
#endif /* def USE_SPI2 */


void SPI_init(void) 
{
	static bool_e initialized = FALSE;
	if(initialized)
		return;
	initialized = TRUE;
	
	Sint16 SPICONValue;
	Sint16 SPISTATValue;
	
	CloseSPI1();
	DisableIntSPI1;
	CloseSPI2();
	DisableIntSPI2;

	SPICONValue = FRAME_ENABLE_OFF & FRAME_SYNC_OUTPUT &
    		ENABLE_SDO_PIN & SPI_MODE16_OFF &
            SPI_SMP_ON & SPI_CKE_OFF &
           	SLAVE_ENABLE_OFF &
          	CLK_POL_ACTIVE_LOW &
           	MASTER_ENABLE_ON &
            SEC_PRESCAL_1_1 &
            PRI_PRESCAL_64_1;
                    	
    SPISTATValue = SPI_ENABLE & 
    		SPI_IDLE_CON &
     		SPI_RX_OVFLOW_CLR;


	#ifdef USE_SPI1
		OpenSPI1(SPICONValue,SPISTATValue);
		ConfigIntSPI1(SPI_INT_DIS & SPI_INT_PRI_6);
		m_s1rnum=0;	
	#endif /* def USE_SPI1 */
	
	#ifdef USE_SPI2		
		OpenSPI2(SPICONValue,SPISTATValue);
		ConfigIntSPI2(SPI_INT_DIS & SPI_INT_PRI_6);
		m_s2rnum=0;		
	#endif /* def USE_SPI2 */

}

#ifdef USE_SPI1

void SPI1_put_byte(Uint8 msg) 
{
	while(SPI1STATbits.SPITBF);
	putcSPI1(msg);	
	while(SPI1STATbits.SPITBF);
}

bool_e SPI1_data_ready() 
{
	return (bool_e) m_s1rnum>0;
}

Uint8 SPI1_get_next_msg() 
{
	if(SPI1_data_ready()) /* Si des messges dans le buffer de réception */		
		return m_s1rbuf[m_s1rnum--];
	else
		return 0;
}

void _ISR _SPI1Interrupt(void) 
{   
	m_s1rbuf[m_s1rnum++]=getcSPI1();
		
    IFS0bits.SPI1IF = 0;
		IEC1bits.SPI1IE = 0;
    /* SPI1STATbits.SPIROV = 0; */  /* Clear SPI1 receive overflow flag if set */
}

#endif /* def USE_SPI1 */

#ifdef USE_SPI2


Uint8 SPI2_exchange(Uint8 c)
{
	Uint8 ret;
	if(SPI2STATbits.SPIRBF)
	{
		debug_printf("buffer non vide \n");
		ret = SPI2BUF;
	}	
	SPI2BUF = c;
	while(SPI2STATbits.SPITBF);		//Attente transmission
	while(!SPI2STATbits.SPIRBF);	//Attente réception
	nop();	//Ca marche pas sans, et je sais pas pourquoi...
	
	if(SPI2STATbits.SPIROV)
	{
		debug_printf("collision\n");
		SPI2STATbits.SPIROV = 0;	//collision
	}
	
	ret = SPI2BUF;
	IFS1bits.SPI2IF = 0;
	return ret;
}


void SPI2_put_byte(Uint8 msg) 
{

	while(SPI2STATbits.SPITBF);
	//putcSPI2(msg);	
	SPI2_exchange(msg);
	while(SPI2STATbits.SPITBF);
}

void SPI2_write(Uint8 msg) 
{
	SPI2_exchange(msg);
}

Uint8 SPI2_read() 
{
	return SPI2_exchange(0x00);
}

bool_e SPI2_data_ready() 
{
	return (bool_e) m_s2rnum>0;
}

Uint8 SPI2_get_next_msg()
{
	if(SPI2_data_ready() && (SPI2STATbits.SPIROV == 1)) /* Si des messges dans le buffer de réception */		
		return m_s2rbuf[m_s2rnum--];
	else
		return 0;
}

void _ISR _SPI2Interrupt(void) 
{   
	debug_printf("IT SPI \n");
	if(SPI2STATbits.SPIRBF)
		m_s2rbuf[m_s2rnum++]=getcSPI2();
	if(SPI2STATbits.SPIRBF)
		printf("pzs repasse a zero");
    IFS1bits.SPI2IF = 0;
//	IEC1bits.SPI2IE = 0;
    SPI2STATbits.SPIROV = 0;   /* Clear SPI2 receive overflow flag if set */
}

#endif /* def USE_SPI2 */  

