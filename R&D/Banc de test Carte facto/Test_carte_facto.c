/*
 *  Club Robot ESEO 2010 - 2013
 *  Chomp
 *
 *  Fichier : QS_watchdog.h
 *  Package : Qualité Soft
 *  Description : Test des cartes génériques 2010-2011
 *  Auteur : Patrick, Erwan, Alexis, Nirgal
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 201302
 */
 
#include "Test_carte_facto.h"
#include "QS/QS_Timer.h"
#include "QS/QS_pwm.h"

void next_output_state(void);
void test_adc(void);
void test_can(void);
void process_pwm(void);

volatile bool_e flag_pwm[4] = {FALSE, FALSE, FALSE, FALSE};
volatile bool_e flag_pwm_time = TRUE;

void test_carte_facto_init(void)
{	
	flag_pwm[0] = FALSE;
	flag_pwm[1] = FALSE;
	flag_pwm[2] = FALSE;
	flag_pwm[3] = FALSE;


	PWM_stop(1);
	PWM_stop(2);
	PWM_stop(3);
	PWM_stop(4);
					
	TRISA = 0x3FFF;
	TRISE = 0xFCAA;
	TRISG = 0xFC33;
	TRISD = 0xC0FE;	//Ok

	LATAbits.LATA14 = 0;
	LATAbits.LATA15 = 0;

	LATDbits.LATD0 = 0;
	LATDbits.LATD8 = 0;
	LATDbits.LATD9 = 0;
	LATDbits.LATD10 = 0;
	LATDbits.LATD11 = 0;
	LATDbits.LATD12 = 0;
	LATDbits.LATD13 = 0;
	
	LATGbits.LATG2 = 0;
	LATGbits.LATG8 = 0;
	LATGbits.LATG6 = 0;
	LATGbits.LATG3 = 0;
	LATGbits.LATG7 = 0;
	LATGbits.LATG9 = 0;
	
	LATEbits.LATE2 = 0;
	LATEbits.LATE6 = 0;
	LATEbits.LATE0 = 0;
	LATEbits.LATE4 = 0;
	LATEbits.LATE8 = 0;
	LATEbits.LATE9 = 0;
}




void test_carte_facto_update(void)
{
	static bool_e previous_sw0 = BOUTON_OFF;
	static bool_e previous_sw1 = BOUTON_OFF;
	bool_e		  current_sw0;
	bool_e		  current_sw1;
		
	static bool_e previous_bp1 = BOUTON_OFF;
	static bool_e previous_bp2 = BOUTON_OFF;
	static bool_e previous_bp3 = BOUTON_OFF;
	static bool_e previous_bp4 = BOUTON_OFF;
	bool_e		  current_bp1;
	bool_e		  current_bp2;
	bool_e		  current_bp3;
	bool_e		  current_bp4;
	
	
	CAN_msg_t receivedCanMsg;
	
	//Lecture des boutons
	current_bp1 = BUTTON1_PORT;
	current_bp2 = BUTTON2_PORT;
	current_bp3 = BUTTON3_PORT;
	current_bp4 = BUTTON4_PORT;
	
	//Bouton 1: Test des sorties (affichage par LED)	
	if(current_bp1 && !previous_bp1)
		next_output_state();

	//Bouton 2: Test des entrées analogiques (RB2-RB15), affichage par l'uart 	
	if(current_bp2 && !previous_bp2)
		test_adc();
	
	//Bouton 3 Test CAN
	if(current_bp3 && !previous_bp3)
		test_can();
	
	//Bouton 4 : rien... ?	
	if(current_bp4 && !previous_bp4)
		debug_printf("  Bouton 4 : il vient d'être appuyé. Pensez donc à tester les UARTs...et les switchs\n");
	
	//On prépare le futur... en mémorisant les états actuels.
	previous_bp1 = current_bp1;
	previous_bp2 = current_bp2;
	previous_bp3 = current_bp3;
	previous_bp4 = current_bp4;	
	
	//Lecture des interrupteurs
	current_sw0 = PORTGbits.RG0;
	current_sw1 = PORTGbits.RG1;
	if(previous_sw0 != current_sw0)
		printf("Switch RG0 en position %d\n",current_sw0);
	if(previous_sw1 != current_sw1)
		printf("Switch RG1 en position %d\n",current_sw1);
	previous_sw0 = current_sw0;
	previous_sw1 = current_sw1;
	
	
	//Gestion des PWM.
	process_pwm();
		
	//Echo UART1
	if(UART1_data_ready())
		printf("1%c",UART1_get_next_msg());
	
	//Echo UART2
	if(UART2_data_ready())
	{
		UART2_putc('2');
		UART2_putc(UART2_get_next_msg());
	}
	
	//Lecture CAN
	if(CAN_data_ready()) 
	{
		receivedCanMsg = CAN_get_next_msg();
		debug_printf("  Msg CAN recu : SID=%x DATAS = %x %x %x %x %x %x %x %x | SIZE=%x\n", 
							receivedCanMsg.sid, 
							receivedCanMsg.data[0], 
							receivedCanMsg.data[1], 
							receivedCanMsg.data[2], 
							receivedCanMsg.data[3], 
							receivedCanMsg.data[4], 
							receivedCanMsg.data[5], 
							receivedCanMsg.data[6], 
							receivedCanMsg.data[7], 
							receivedCanMsg.size	);
	}				
}

void test_adc(void)
{
	ADC_init();
	debug_printf("  Bouton 2 : Test des entrées analogiques.\n");
	debug_printf("  valeur entree analogique B2 : %.2fV\n",(COEFF*ADC_getValue(0)));
	debug_printf("  valeur entree analogique B3 : %.2fV\n",(COEFF*ADC_getValue(1)));
	debug_printf("  valeur entree analogique B4/QEA : %.2fV\n",(COEFF*ADC_getValue(2)));
	debug_printf("  valeur entree analogique B5/QEB : %.2fV\n",(COEFF*ADC_getValue(3)));
	debug_printf("  valeur entree analogique B8 : %.2fV\n",(COEFF*ADC_getValue(4)));
	debug_printf("  valeur entree analogique B9 : %.2fV\n",(COEFF*ADC_getValue(5)));
	debug_printf("  valeur entree analogique B10 : %.2fV\n",(COEFF*ADC_getValue(6)));
	debug_printf("  valeur entree analogique B11 : %.2fV\n",(COEFF*ADC_getValue(7)));
	debug_printf("  valeur entree analogique B12 : %.2fV\n",(COEFF*ADC_getValue(8)));
	debug_printf("  valeur entree analogique B13 : %.2fV\n",(COEFF*ADC_getValue(9)));
	debug_printf("  valeur entree analogique B14 : %.2fV\n",(COEFF*ADC_getValue(10)));
	debug_printf("  valeur entree analogique B15 : %.2fV\n\n",(COEFF*ADC_getValue(11)));	
}	

void CAN_reinit(void);
void test_can(void)
{
	Uint16 i;
	CAN_msg_t canMsg;
	canMsg.sid = 0b10101010101;
	canMsg.data[0] = 1;
	canMsg.data[1] = 2;
	canMsg.data[2] = 3;
	canMsg.data[3] = 4;
	canMsg.data[4] = 5;
	canMsg.data[5] = 6;
	canMsg.data[6] = 7;
	canMsg.data[7] = 8;
	canMsg.size = 8;
	debug_printf("Bouton 3 : Test CAN: Sid: 0b10101010101, data[8]={1,2,3,4,5,6,7,8}, size = 8\n");
	for(i=0;i<10;i++)
	{
		CAN_reinit();
		CAN_send(&canMsg);
	}	
}	

void next_output_state(void)
{
	typedef enum 
	{
		TEST_INTRO=0,
		TEST_RD8,
		TEST_RD9,
		TEST_RD10,
		TEST_RD11,
		TEST_RD12,
		TEST_RD13,
		TEST_RD0,
		TEST_INT3,
		TEST_INT4,
		TEST_RG3,
		TEST_RG2,
		TEST_PWM1,
		TEST_SENS1,
		TEST_PWM2,
		TEST_SENS2,
		TEST_PWM3,
		TEST_SENS3,
		TEST_PWM4,
		TEST_SENS4,
		TEST_RG7,
		TEST_RG6,
		TEST_RG9,
		TEST_RG8,
		TEST_INT2,
		TEST_INT1,
		TEST_ALL,
		TEST_LAST
	}TEST_output_state_e ;
	static TEST_output_state_e output_state_e = TEST_INTRO;
	
		switch(output_state_e){
			case TEST_INTRO:
				debug_printf("  Test des ports...\n Appuyez encore plein de fois sur le bouton 1...\n");
				debug_printf(" Test port D8/LED_RUN\n");
				test_carte_facto_init();
				LATDbits.LATD8 = LED_ON;
			break;
			case TEST_RD8:
				LATDbits.LATD8 = LED_OFF;
				LATDbits.LATD9 = LED_ON;
			break;
			case TEST_RD9:
				LATDbits.LATD9 = LED_OFF;
				LATDbits.LATD10 = LED_ON;
			break;
			case TEST_RD10:
				LATDbits.LATD10 = LED_OFF;
				LATDbits.LATD11 = LED_ON;
			break;
			case TEST_RD11:
				LATDbits.LATD11 = LED_OFF;
				LATDbits.LATD12 = LED_ON;
			break;
			case TEST_RD12:
				LATDbits.LATD12 = LED_OFF;
				LATDbits.LATD13 = LED_ON;
			break;
			case TEST_RD13:
				LATDbits.LATD13 = LED_OFF;
				LATDbits.LATD0 = LED_ON;
			break;
			case TEST_RD0:
				LATDbits.LATD0 = LED_OFF;
				LATAbits.LATA14 = LED_ON;
			break;
			case TEST_INT3:
				LATAbits.LATA14 = LED_OFF;
				LATAbits.LATA15 = LED_ON;
			break;
			case TEST_INT4:
				LATAbits.LATA15 = LED_OFF;
				LATGbits.LATG3 = LED_ON;
			break;
			case TEST_RG3:
				LATGbits.LATG3 = LED_OFF;
				LATGbits.LATG2 = LED_ON;
			break;
			case TEST_RG2:
				LATGbits.LATG2 = LED_OFF;
				flag_pwm[0] = 1;
			break;
			case TEST_PWM1:
				flag_pwm[0] = 0;
				LATEbits.LATE0 = LED_ON;
			break;
			case TEST_SENS1:
				LATEbits.LATE0 = LED_OFF;
				flag_pwm[1] = 1;
			break;
			case TEST_PWM2:
				flag_pwm[1] = 0;
				LATEbits.LATE2 = LED_ON;
			break;
			case TEST_SENS2:
				LATEbits.LATE2 = LED_OFF;
				flag_pwm[2] = 1;
			break;
			case TEST_PWM3:
				flag_pwm[2] = 0;
				LATEbits.LATE4 = LED_ON;
			break;
			case TEST_SENS3:
				LATEbits.LATE4 = LED_OFF;
				flag_pwm[3] = 1;
			break;
			case TEST_PWM4:
				flag_pwm[3] = 0;
				LATEbits.LATE6 = LED_ON;
			break;
			case TEST_SENS4:
				LATEbits.LATE6 = LED_OFF;
				LATGbits.LATG7 = LED_ON;
			break;
			case TEST_RG7:
				LATGbits.LATG7 = LED_OFF;
				LATGbits.LATG6 = LED_ON;
			break;
			case TEST_RG6:
				LATGbits.LATG6 = LED_OFF;
				LATGbits.LATG9 = LED_ON;
			break;
			case TEST_RG9:
				LATGbits.LATG9 = LED_OFF;
				LATGbits.LATG8 = LED_ON;
			break;
			case TEST_RG8:
				LATGbits.LATG8 = LED_OFF;
				LATEbits.LATE9 = LED_ON;
			break;
			case TEST_INT2:
				LATEbits.LATE9 = LED_OFF;
				LATEbits.LATE8 = LED_ON;
			break;
			case TEST_INT1:
				LATEbits.LATE8 = LED_OFF;
				
				//ALL ON...
				LATDbits.LATD8 = LED_ON;
				LATDbits.LATD9 = LED_ON;
				LATDbits.LATD10 = LED_ON;
				LATDbits.LATD11 = LED_ON;
				LATDbits.LATD12 = LED_ON;
				LATDbits.LATD13 = LED_ON;
				LATDbits.LATD0 = LED_ON;
				LATAbits.LATA14 = LED_ON;
				LATAbits.LATA15 = LED_ON;
				LATGbits.LATG3 = LED_ON;
				LATGbits.LATG2 = LED_ON;
				flag_pwm[0] = 1;
				LATEbits.LATE0 = LED_ON;
				flag_pwm[1] = 1;
				LATEbits.LATE2 = LED_ON;
				flag_pwm[2] = 1;
				LATEbits.LATE4 = LED_ON;
				flag_pwm[3] = 1;
				LATEbits.LATE6 = LED_ON;
				LATGbits.LATG7 = LED_ON;
				LATGbits.LATG6 = LED_ON;
				LATGbits.LATG9 = LED_ON;
				LATGbits.LATG8 = LED_ON;
				LATEbits.LATE9 = LED_ON;
				LATEbits.LATE8 = LED_ON;
			break;
			case TEST_ALL:
				test_carte_facto_init();
			break;
			case TEST_LAST:
				debug_printf(" Test des ports en sortie terminé\n");		
			default :
				break;
		}
		//Prochain état...	
		output_state_e = (output_state_e<TEST_LAST)?output_state_e+1:TEST_INTRO;
	}
	
/*
	Les flags global.flag_pwm[X] permettent d'activer les PWMs en sortie.
*/
void process_pwm(void)
{
	Uint8 i;
	typedef enum
	{
		PWM_INIT,
		PWM_INC,
		PWM_DEC
	}state_pwm_e;
	static state_pwm_e state_pwm = PWM_INIT;	
	static Uint8 alpha = 0;
		
	switch(state_pwm)
	{
		case PWM_INIT:
			TIMER2_run(INC_PWM_PERIOD);
			state_pwm = PWM_INC;
		break;
		case PWM_INC:
			if(flag_pwm_time)
			{
				alpha++;
				if(alpha == 100)
					state_pwm = PWM_DEC;
			}	
		break;
		case PWM_DEC:
			if(flag_pwm_time)
			{
				alpha--;
				if(alpha == 0)
					state_pwm = PWM_INC;
			}
		break;
		default:
		break;
	}
	if(flag_pwm_time)
	{
		flag_pwm_time = FALSE;
		for(i=0;i<4;i++)	//Pour les 4 PWM.
		{
			if(flag_pwm[i])			//i va de 0 à 3
				PWM_run(alpha,i+1);	//Le canal PWM va de 1 à 4.
			else
				PWM_stop(i+1);
		}		
	}	
}

 
void _ISR _T2Interrupt(void)
{
	flag_pwm_time = TRUE;
	IFS0bits.T2IF = 0;
}


