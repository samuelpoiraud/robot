/*
 *  Club Robot ESEO 2010 - 2011
 *  Chomp
 *
 *  Fichier : QS_watchdog.h
 *  Package : Qualité Soft
 *  Description : Test des cartes génériques 2010-2011
 *  Auteur : Patrick, Erwan
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20110215
 */
 
#include "Test_carte_facto.h"
 
	void WATCHDOG_init(void);
 
 void test_carte_facto_init(){
	WATCHDOG_init();
	
	global.flag_pwm1 = 0;
	global.flag_pwm2 = 0;
	global.flag_pwm3 = 0;
	global.flag_pwm4 = 0;
	global.flag_toutes_pwm = 0;

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

void test_carte_facto_update(){
	
	static bool_e etat_precedent_bp1 = BOUTON_OFF;
	static bool_e etat_precedent_bp2 = BOUTON_OFF;
	static bool_e etat_precedent_bp3 = BOUTON_OFF;
	static bool_e etat_precedent_bp4 = BOUTON_OFF;
	
	static bool_e test_multiple_sorties = 0;
	static bool_e dec;
	
	static Uint8 i;
	Uint8 msg;
	CAN_msg_t canMsg;
	
	static watchdog_id_t id;
	
	enum TEST_etat_test_e {
		AUCUN_TEST,
		TEST_SORTIES,
		TEST_UART,
		TEST_CAN
	};
	static enum TEST_etat_test_e etat_test_e = 0;
	
	enum TEST_etat_test_sorties_e {
		TEST_SORTIE1,
		TEST_SORTIE2,
		TEST_SORTIE3,
		TEST_SORTIE4,
		TEST_SORTIE5,
		TEST_SORTIE6,
		TEST_SORTIE7,
		TEST_SORTIE8,
		TEST_SORTIE9,
		TEST_SORTIE10,
		TEST_SORTIE11,
		TEST_SORTIE12,
		TEST_SORTIE13,
		TEST_SORTIE14,
		TEST_SORTIE15,
		TEST_SORTIE16,
		TEST_SORTIE17,
		TEST_SORTIE18,
		TEST_SORTIE19,
		TEST_SORTIE20,
		TEST_SORTIE21,
		TEST_SORTIE_PWM1,
		TEST_SORTIE_PWM2,
		TEST_SORTIE_PWM3,
		TEST_SORTIE_PWM4,
		TEST_TOUTES_SORTIES
	};
	static enum TEST_etat_test_sorties_e etat_test_sorties_e = 0;

	enum TEST_test_uart_e {
		AUCUN_UART,
		TRANSMISSION_UART1,
		RECEPTION_UART1,
		TRANSMISSION_UART2,
		RECEPTION_UART2
	};
	static enum TEST_test_uart_e test_uart_e = 0;

	//Bouton 1: Test des sorties (affichage par LED)
	if(BUTTON1_PORT == BOUTON_ON && etat_precedent_bp1 == BOUTON_OFF && etat_test_e != TEST_SORTIES){
		etat_precedent_bp1 = BOUTON_ON;
		etat_test_e = TEST_SORTIES;
		debug_printf("  Test des ports:\nTest port D8/LED_RUN\n");
	}
	else if(BUTTON1_PORT == BOUTON_OFF && etat_precedent_bp1 == BOUTON_ON){
		etat_precedent_bp1 = BOUTON_OFF;
	}

	//Bouton 2: Test des entrées analogiques (RB2-RB15), affichage par l'uart 1
	if(BUTTON2_PORT == BOUTON_ON && etat_precedent_bp2 == BOUTON_OFF){
		etat_precedent_bp2 = BOUTON_ON;
		ADC_init();
		debug_printf("  valeur entree analogique B2 : %fV\n",(COEFF*ADC_getValue(0)));
		debug_printf("  valeur entree analogique B3 : %fV\n",(COEFF*ADC_getValue(1)));
		debug_printf("  valeur entree analogique B4/QEA : %fV\n",(COEFF*ADC_getValue(2)));
		debug_printf("  valeur entree analogique B5/QEB : %fV\n",(COEFF*ADC_getValue(3)));
		debug_printf("  valeur entree analogique B8 : %fV\n",(COEFF*ADC_getValue(4)));
		debug_printf("  valeur entree analogique B9 : %fV\n",(COEFF*ADC_getValue(5)));
		debug_printf("  valeur entree analogique B10 : %fV\n",(COEFF*ADC_getValue(6)));
		debug_printf("  valeur entree analogique B11 : %fV\n",(COEFF*ADC_getValue(7)));
		debug_printf("  valeur entree analogique B12 : %fV\n",(COEFF*ADC_getValue(8)));
		debug_printf("  valeur entree analogique B13 : %fV\n",(COEFF*ADC_getValue(9)));
		debug_printf("  valeur entree analogique B14 : %fV\n",(COEFF*ADC_getValue(10)));
		debug_printf("  valeur entree analogique B15 : %fV\n\n",(COEFF*ADC_getValue(11)));
	}
	else if(BUTTON2_PORT == BOUTON_OFF && etat_precedent_bp2 == BOUTON_ON){
		etat_precedent_bp2 = BOUTON_OFF;
	}


	//Bouton 3 et 4 simultané: Test CAN
	if(BUTTON3_PORT == BOUTON_ON && BUTTON4_PORT == BOUTON_ON) {
		if(etat_test_e != TEST_CAN) {
			CAN_init();
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
			debug_printf("  Test CAN: Sid: 0b10101010101, data[8]={1,2,3,4,5,6,7,8}, size = 8\n");
		}

		etat_test_e = TEST_CAN;

		//Impeche la detection du front descendant comme un appui de bouton pour lancer un test UART
		etat_precedent_bp3 = BOUTON_OFF;
		etat_precedent_bp4 = BOUTON_OFF;
	} else if(etat_test_e == TEST_CAN && (BUTTON3_PORT == BOUTON_OFF && BUTTON4_PORT == BOUTON_OFF)) {
		etat_test_e = AUCUN_TEST;
	} else if(etat_test_e != TEST_CAN) {
		//Bouton 3: Test uart 1: echo des données reçues
		if(BUTTON3_PORT == BOUTON_ON && etat_precedent_bp3 == BOUTON_OFF){
			etat_precedent_bp3 = BOUTON_ON;
		}
		else if(BUTTON3_PORT == BOUTON_OFF && etat_precedent_bp3 == BOUTON_ON){
			etat_precedent_bp3 = BOUTON_OFF;
			etat_test_e = TEST_UART;
			test_uart_e = TRANSMISSION_UART1;
			debug_printf("  Test UART1: echo\n");
		}

		//Bouton 4: Test uart 2: echo des données reçues
		if(BUTTON4_PORT == BOUTON_ON && etat_precedent_bp4 == BOUTON_OFF){
			etat_precedent_bp4 = BOUTON_ON;
		}
		else if(BUTTON4_PORT == BOUTON_OFF && etat_precedent_bp4 == BOUTON_ON){
			etat_precedent_bp4 = BOUTON_OFF;
			etat_test_e = TEST_UART;
			test_uart_e = TRANSMISSION_UART2;
			debug_printf("  Test UART2: echo\n");
		}
	}

	switch(etat_test_e){
		case TEST_SORTIES :
			switch(etat_test_sorties_e){
				case TEST_SORTIE1 :
					if(test_multiple_sorties) {
						test_carte_facto_init();
						PWM_stop(1);
						PWM_stop(2);
						PWM_stop(3);
						PWM_stop(4);
					}
					if(LATDbits.LATD8 != LED_ON);
						test_multiple_sorties = 0;
						LATDbits.LATD8 = LED_ON;
					if(BUTTON1_PORT == BOUTON_ON && etat_precedent_bp1 == BOUTON_OFF){
						etat_precedent_bp1 = BOUTON_ON;
						etat_test_sorties_e = TEST_SORTIE2;
						debug_printf("Test port D9/LED_CAN\n");
					}
					else if(BUTTON1_PORT == BOUTON_OFF && etat_precedent_bp1 == BOUTON_ON){
						etat_precedent_bp1 = BOUTON_OFF;
					}
					break;
				case TEST_SORTIE2 :
					TRISDbits.TRISD9 = 0;
					if(LATDbits.LATD9 != LED_ON)
						LATDbits.LATD9 = LED_ON;
					if(LATDbits.LATD8 != LED_OFF)
						LATDbits.LATD8 = LED_OFF;
					if(BUTTON1_PORT == BOUTON_ON && etat_precedent_bp1 == BOUTON_OFF){
						etat_precedent_bp1 = BOUTON_ON;
						etat_test_sorties_e = TEST_SORTIE3;
						debug_printf("Test port D10/LED_UART\n");
					}	
					else if(BUTTON1_PORT == BOUTON_OFF && etat_precedent_bp1 == BOUTON_ON)
						etat_precedent_bp1 = BOUTON_OFF;
					break;
				case TEST_SORTIE3 :
					if(LATDbits.LATD10 != LED_ON)
						LATDbits.LATD10 = LED_ON;
					if(LATDbits.LATD9 != LED_OFF)	
						LATDbits.LATD9 = LED_OFF;		
					if(BUTTON1_PORT == BOUTON_ON && etat_precedent_bp1 == BOUTON_OFF){
						etat_precedent_bp1 = BOUTON_ON;
						etat_test_sorties_e = TEST_SORTIE4;
						debug_printf("Test port D11/LED_USER\n");
					}		
					else if(BUTTON1_PORT == BOUTON_OFF && etat_precedent_bp1 == BOUTON_ON)
						etat_precedent_bp1 = BOUTON_OFF;
					break;
				case TEST_SORTIE4 :
					if(LATDbits.LATD11 != LED_ON)
						LATDbits.LATD11 = LED_ON;
					if(LATDbits.LATD10 != LED_OFF)
						LATDbits.LATD10 = LED_OFF;
					if(BUTTON1_PORT == BOUTON_ON && etat_precedent_bp1 == BOUTON_OFF){
						etat_precedent_bp1 = BOUTON_ON;
						etat_test_sorties_e = TEST_SORTIE5;
						debug_printf("Test port D12/LED_USER\n");
					}	
					else if(BUTTON1_PORT == BOUTON_OFF && etat_precedent_bp1 == BOUTON_ON)
						etat_precedent_bp1 = BOUTON_OFF;
					break;
				case TEST_SORTIE5 :
					if(LATDbits.LATD12 != LED_ON)
						LATDbits.LATD12 = LED_ON;
					if(LATDbits.LATD11 != LED_OFF)
						LATDbits.LATD11 = LED_OFF;
					if(BUTTON1_PORT == BOUTON_ON && etat_precedent_bp1 == BOUTON_OFF){
						etat_precedent_bp1 = BOUTON_ON;
						etat_test_sorties_e = TEST_SORTIE6;
						debug_printf("Test port D13/LED_ERROR\n");
					}	
					else if(BUTTON1_PORT == BOUTON_OFF && etat_precedent_bp1 == BOUTON_ON)
						etat_precedent_bp1 = BOUTON_OFF;
					break;
				case TEST_SORTIE6 :
					if(LATDbits.LATD13 != LED_ON)
						LATDbits.LATD13 = LED_ON;
					if(LATDbits.LATD12 != LED_OFF)
						LATDbits.LATD12 = LED_OFF;			
					if(BUTTON1_PORT == BOUTON_ON && etat_precedent_bp1 == BOUTON_OFF){
						etat_precedent_bp1 = BOUTON_ON;
						etat_test_sorties_e = TEST_SORTIE7;
						debug_printf("Test port D0\n");
					}	
					else if(BUTTON1_PORT == BOUTON_OFF && etat_precedent_bp1 == BOUTON_ON)
						etat_precedent_bp1 = BOUTON_OFF;
					break;

				case TEST_SORTIE7 :
					if(LATDbits.LATD0 != LED_ON)
						LATDbits.LATD0 = LED_ON;
					if(LATDbits.LATD13 != LED_OFF)
						LATDbits.LATD13 = LED_OFF;			
					if(BUTTON1_PORT == BOUTON_ON && etat_precedent_bp1 == BOUTON_OFF){
						etat_precedent_bp1 = BOUTON_ON;
						etat_test_sorties_e = TEST_SORTIE8;
						debug_printf("Test port A15/INT4\n");
					}	
					else if(BUTTON1_PORT == BOUTON_OFF && etat_precedent_bp1 == BOUTON_ON)
						etat_precedent_bp1 = BOUTON_OFF;
					break;

				case TEST_SORTIE8 :
					if(LATAbits.LATA15 != LED_ON)
						LATAbits.LATA15 = LED_ON;
					if(LATDbits.LATD0 != LED_OFF)
						LATDbits.LATD0 = LED_OFF;			
					if(BUTTON1_PORT == BOUTON_ON && etat_precedent_bp1 == BOUTON_OFF){
						etat_precedent_bp1 = BOUTON_ON;
						etat_test_sorties_e = TEST_SORTIE9;
						debug_printf("Test port A14/INT3\n");
					}	
					else if(BUTTON1_PORT == BOUTON_OFF && etat_precedent_bp1 == BOUTON_ON)
						etat_precedent_bp1 = BOUTON_OFF;
					break;

				case TEST_SORTIE9 :
					if(LATAbits.LATA14 != LED_ON)
						LATAbits.LATA14 = LED_ON;
					if(LATAbits.LATA15 != LED_OFF)
						LATAbits.LATA15 = LED_OFF;
					if(BUTTON1_PORT == BOUTON_ON && etat_precedent_bp1 == BOUTON_OFF){
						etat_precedent_bp1 = BOUTON_ON;
						etat_test_sorties_e = TEST_SORTIE10;
						debug_printf("Test port E8/INT1\n");
					}	
					else if(BUTTON1_PORT == BOUTON_OFF && etat_precedent_bp1 == BOUTON_ON)
						etat_precedent_bp1 = BOUTON_OFF;
					break;

				case TEST_SORTIE10 :
					if(LATEbits.LATE8 != LED_ON)
						LATEbits.LATE8 = LED_ON;
					if(LATAbits.LATA14 != LED_OFF)
						LATAbits.LATA14 = LED_OFF;
					if(BUTTON1_PORT == BOUTON_ON && etat_precedent_bp1 == BOUTON_OFF){
						etat_precedent_bp1 = BOUTON_ON;
						etat_test_sorties_e = TEST_SORTIE11;
						debug_printf("Test port E9/INT2\n");
					}	
					else if(BUTTON1_PORT == BOUTON_OFF && etat_precedent_bp1 == BOUTON_ON)
						etat_precedent_bp1 = BOUTON_OFF;
					break;

				case TEST_SORTIE11 :
					if(LATEbits.LATE9 != LED_ON)
						LATEbits.LATE9 = LED_ON;
					if(LATEbits.LATE8 != LED_OFF)
						LATEbits.LATE8 = LED_OFF;
					if(BUTTON1_PORT == BOUTON_ON && etat_precedent_bp1 == BOUTON_OFF){
						etat_precedent_bp1 = BOUTON_ON;
						etat_test_sorties_e = TEST_SORTIE12;
						debug_printf("Test port G2/SCL\n");
					}	
					else if(BUTTON1_PORT == BOUTON_OFF && etat_precedent_bp1 == BOUTON_ON)
						etat_precedent_bp1 = BOUTON_OFF;
					break;

				case TEST_SORTIE12 :
					if(LATGbits.LATG2 != LED_ON)
						LATGbits.LATG2 = LED_ON;
					if(LATEbits.LATE9 != LED_OFF)
						LATEbits.LATE9 = LED_OFF;
					if(BUTTON1_PORT == BOUTON_ON && etat_precedent_bp1 == BOUTON_OFF){
						etat_precedent_bp1 = BOUTON_ON;
						etat_test_sorties_e = TEST_SORTIE13;
						debug_printf("Test port E2/sens2\n");
					}	
					else if(BUTTON1_PORT == BOUTON_OFF && etat_precedent_bp1 == BOUTON_ON)
						etat_precedent_bp1 = BOUTON_OFF;
					break;
				case TEST_SORTIE13 :
					if(LATEbits.LATE2 != LED_ON)
						LATEbits.LATE2 = LED_ON;
					if(LATGbits.LATG2 != LED_OFF)
						LATGbits.LATG2 = LED_OFF;			
					if(BUTTON1_PORT == BOUTON_ON && etat_precedent_bp1 == BOUTON_OFF){
						etat_precedent_bp1 = BOUTON_ON;
						etat_test_sorties_e = TEST_SORTIE14;
						debug_printf("Test port E6/sens4\n");
					}	
					else if(BUTTON1_PORT == BOUTON_OFF && etat_precedent_bp1 == BOUTON_ON)
						etat_precedent_bp1 = BOUTON_OFF;
					break;
				case TEST_SORTIE14 :
					if(LATEbits.LATE6 != LED_ON)
						LATEbits.LATE6 = LED_ON;
					if(LATEbits.LATE2 != LED_OFF)
						LATEbits.LATE2 = LED_OFF;			
					if(BUTTON1_PORT == BOUTON_ON && etat_precedent_bp1 == BOUTON_OFF){
						etat_precedent_bp1 = BOUTON_ON;
						etat_test_sorties_e = TEST_SORTIE15;
						debug_printf("Test port G8/SDO2\n");
					}	
					else if(BUTTON1_PORT == BOUTON_OFF && etat_precedent_bp1 == BOUTON_ON)
						etat_precedent_bp1 = BOUTON_OFF;
					break;
				case TEST_SORTIE15 :
					if(LATGbits.LATG8 != LED_ON)
						LATGbits.LATG8 = LED_ON;
					if(LATEbits.LATE6 != LED_OFF)
						LATEbits.LATE6 = LED_OFF;			
					if(BUTTON1_PORT == BOUTON_ON && etat_precedent_bp1 == BOUTON_OFF){
						etat_precedent_bp1 = BOUTON_ON;
						etat_test_sorties_e = TEST_SORTIE16;
						debug_printf("Test port E0/sens1\n");
					}	
					else if(BUTTON1_PORT == BOUTON_OFF && etat_precedent_bp1 == BOUTON_ON)
						etat_precedent_bp1 = BOUTON_OFF;
					break;
				case TEST_SORTIE16 :
					if(LATEbits.LATE0 != LED_ON)
						LATEbits.LATE0 = LED_ON;
					if(LATGbits.LATG8 != LED_OFF)
						LATGbits.LATG8 = LED_OFF;			
					if(BUTTON1_PORT == BOUTON_ON && etat_precedent_bp1 == BOUTON_OFF){
						etat_precedent_bp1 = BOUTON_ON;
						etat_test_sorties_e = TEST_SORTIE17;
						debug_printf("Test port E4/sens3\n");
					}	
					else if(BUTTON1_PORT == BOUTON_OFF && etat_precedent_bp1 == BOUTON_ON)
						etat_precedent_bp1 = BOUTON_OFF;
					break;
				case TEST_SORTIE17 :
					if(LATEbits.LATE4 != LED_ON)
						LATEbits.LATE4 = LED_ON;
					if(LATEbits.LATE0 != LED_OFF)
						LATEbits.LATE0 = LED_OFF;			
					if(BUTTON1_PORT == BOUTON_ON && etat_precedent_bp1 == BOUTON_OFF){
						etat_precedent_bp1 = BOUTON_ON;
						etat_test_sorties_e = TEST_SORTIE18;
						debug_printf("Test port G6/SCK2\n");
					}	
					else if(BUTTON1_PORT == BOUTON_OFF && etat_precedent_bp1 == BOUTON_ON)
						etat_precedent_bp1 = BOUTON_OFF;
					break;
				case TEST_SORTIE18 :
					if(LATGbits.LATG6 != LED_ON)
						LATGbits.LATG6 = LED_ON;
					if(LATEbits.LATE4 != LED_OFF)
						LATEbits.LATE4 = LED_OFF; 			
					if(BUTTON1_PORT == BOUTON_ON && etat_precedent_bp1 == BOUTON_OFF){
						etat_precedent_bp1 = BOUTON_ON;
						etat_test_sorties_e = TEST_SORTIE19;
						debug_printf("Test port G3/SDA\n");
					}	
					else if(BUTTON1_PORT == BOUTON_OFF && etat_precedent_bp1 == BOUTON_ON)
						etat_precedent_bp1 = BOUTON_OFF;
					break;
				case TEST_SORTIE19 :
					if(LATGbits.LATG3 != LED_ON)
						LATGbits.LATG3 = LED_ON;
					if(LATGbits.LATG6 != LED_OFF)
						LATGbits.LATG6 = LED_OFF;			
					if(BUTTON1_PORT == BOUTON_ON && etat_precedent_bp1 == BOUTON_OFF){
						etat_precedent_bp1 = BOUTON_ON;
						etat_test_sorties_e = TEST_SORTIE20;
						debug_printf("Test port G9/SS2\n");
					}	
					else if(BUTTON1_PORT == BOUTON_OFF && etat_precedent_bp1 == BOUTON_ON)
						etat_precedent_bp1 = BOUTON_OFF;
					break;
				case TEST_SORTIE20 :
					if(LATGbits.LATG9 != LED_ON)
						LATGbits.LATG9 = LED_ON;
					if(LATGbits.LATG3 != LED_OFF)
						LATGbits.LATG3 = LED_OFF;			
					if(BUTTON1_PORT == BOUTON_ON && etat_precedent_bp1 == BOUTON_OFF){
						etat_precedent_bp1 = BOUTON_ON;
						etat_test_sorties_e = TEST_SORTIE21;
						debug_printf("Test port G7/SDI2\n");
					}	
					else if(BUTTON1_PORT == BOUTON_OFF && etat_precedent_bp1 == BOUTON_ON)
						etat_precedent_bp1 = BOUTON_OFF;
					break;
				case TEST_SORTIE21 :
					if(LATGbits.LATG7 != LED_ON)
						LATGbits.LATG7 = LED_ON;
					if(LATGbits.LATG9 != LED_OFF)
						LATGbits.LATG9 = LED_OFF;			
					if(BUTTON1_PORT == BOUTON_ON && etat_precedent_bp1 == BOUTON_OFF){
						etat_precedent_bp1 = BOUTON_ON;
						etat_test_sorties_e = TEST_SORTIE_PWM1;
						debug_printf("Test pwm1 port E1/PWM1\n");
						id = WATCHDOG_create_flag(INC_PWM, (bool_e*) &(global.flag_pwm1));
						i = 1;
					}	
					else if(BUTTON1_PORT == BOUTON_OFF && etat_precedent_bp1 == BOUTON_ON)
						etat_precedent_bp1 = BOUTON_OFF;
					break;
				case TEST_SORTIE_PWM1 :
					if(LATGbits.LATG7 != LED_OFF)
						LATGbits.LATG7 = LED_OFF;
					if(i == 100)
						dec = 1;
					else if(i == 1)
						dec = 0;
					if(global.flag_pwm1 == 1 && i<101 && i>0 && dec == 0){
						WATCHDOG_stop(id);/////
						id = WATCHDOG_create_flag(INC_PWM, (bool_e*) &(global.flag_pwm1));
						global.flag_pwm1 = 0;
						i++;
					}else if(global.flag_pwm1 == 1 && i<101 && i>0 && dec == 1){
						id = WATCHDOG_create_flag(INC_PWM, (bool_e*) &(global.flag_pwm1));
						global.flag_pwm1 = 0;
						i--;
					}
					PWM_run(i,1);
					if(BUTTON1_PORT == BOUTON_ON && etat_precedent_bp1 == BOUTON_OFF){
						etat_precedent_bp1 = BOUTON_ON;
						etat_test_sorties_e = TEST_SORTIE_PWM2;
						debug_printf("Test pwm2 port E3/PWM2\n");
						WATCHDOG_stop(id);
						id = WATCHDOG_create_flag(INC_PWM, (bool_e*) &(global.flag_pwm2));
						i = 1;
					}	
					else if(BUTTON1_PORT == BOUTON_OFF && etat_precedent_bp1 == BOUTON_ON)
						etat_precedent_bp1 = BOUTON_OFF;
					break;
				case TEST_SORTIE_PWM2 :
					PWM_stop(1);
					if(i == 100)
						dec = 1;
					else if(i == 1){
						dec = 0;
					}
					if(global.flag_pwm2 == 1 && i<101 && i>0 && dec == 0){
						WATCHDOG_create_flag(INC_PWM, (bool_e*) &(global.flag_pwm2));
						global.flag_pwm2 = 0;
						i++;
					}else if(global.flag_pwm2 == 1 && i<101 && i>0 && dec == 1){
						WATCHDOG_create_flag(INC_PWM, (bool_e*) &(global.flag_pwm2));
						global.flag_pwm2 = 0;
						i--;
					}
					PWM_run(i,2);		
					if(BUTTON1_PORT == BOUTON_ON && etat_precedent_bp1 == BOUTON_OFF){
						etat_precedent_bp1 = BOUTON_ON;
						etat_test_sorties_e = TEST_SORTIE_PWM3;
						debug_printf("Test pwm3 port E5/PWM3\n");
						WATCHDOG_stop(id);
						id = WATCHDOG_create_flag(INC_PWM, (bool_e*) &(global.flag_pwm3));
						i = 1;
					}	
					else if(BUTTON1_PORT == BOUTON_OFF && etat_precedent_bp1 == BOUTON_ON)
						etat_precedent_bp1 = BOUTON_OFF;
					break;
				case TEST_SORTIE_PWM3 :
					PWM_stop(2);
					if(i == 100)
						dec = 1;
					else if(i == 1)
						dec = 0;
					if(global.flag_pwm3 == 1 && i<101 && i>0 && dec == 0){
						WATCHDOG_create_flag(INC_PWM, (bool_e*) &(global.flag_pwm3));
						global.flag_pwm3 = 0;
						i++;
					}else if(global.flag_pwm3 == 1 && i<101 && i>0 && dec == 1){
						WATCHDOG_create_flag(INC_PWM, (bool_e*) &(global.flag_pwm3));
						global.flag_pwm3 = 0;
						i--;
					}
					PWM_run(i,3);			
					if(BUTTON1_PORT == BOUTON_ON && etat_precedent_bp1 == BOUTON_OFF){
						etat_precedent_bp1 = BOUTON_ON;
						etat_test_sorties_e = TEST_SORTIE_PWM4;
						debug_printf("Test pwm4 port E7/PWM4\n");
						WATCHDOG_stop(id);
						id = WATCHDOG_create_flag(INC_PWM, (bool_e*) &(global.flag_pwm4));
						i = 1;
					}	
					else if(BUTTON1_PORT == BOUTON_OFF && etat_precedent_bp1 == BOUTON_ON)
						etat_precedent_bp1 = BOUTON_OFF;
					break;
				case TEST_SORTIE_PWM4 :
					PWM_stop(3);
					if(i == 100)
						dec = 1;
					else if(i == 1)
						dec = 0;
					if(global.flag_pwm4 == 1 && i<101 && i>0 && dec == 0){
						WATCHDOG_create_flag(INC_PWM, (bool_e*) &(global.flag_pwm4));
						global.flag_pwm4 = 0;
						i++;
					}else if(global.flag_pwm4 == 1 && i<101 && i>0 && dec == 1){
						WATCHDOG_create_flag(INC_PWM, (bool_e*) &(global.flag_pwm4));
						global.flag_pwm4 = 0;
						i--;
					}
					PWM_run(i,4);
					if(BUTTON1_PORT == BOUTON_ON && etat_precedent_bp1 == BOUTON_OFF){
						etat_precedent_bp1 = BOUTON_ON;
						etat_test_sorties_e = TEST_TOUTES_SORTIES;
						debug_printf("Test de tous les ports et PWM\n");
						WATCHDOG_stop(id);
						id = WATCHDOG_create_flag(INC_PWM, (bool_e*) &(global.flag_toutes_pwm));
						i = 1;
					}	
					else if(BUTTON1_PORT == BOUTON_OFF && etat_precedent_bp1 == BOUTON_ON)
						etat_precedent_bp1 = BOUTON_OFF;
					break;
				case TEST_TOUTES_SORTIES :
					LATAbits.LATA14 = LED_ON;
					LATAbits.LATA15 = LED_ON;

					LATDbits.LATD0 = LED_ON;
					LATDbits.LATD8 = LED_ON;
					LATDbits.LATD9 = LED_ON;
					LATDbits.LATD10 = LED_ON;
					LATDbits.LATD11 = LED_ON;
					LATDbits.LATD12 = LED_ON;
					LATDbits.LATD13 = LED_ON;

					LATGbits.LATG2 = LED_ON;
					LATGbits.LATG8 = LED_ON;
					LATGbits.LATG6 = LED_ON;
					LATGbits.LATG3 = LED_ON;
					LATGbits.LATG7 = LED_ON;
					LATGbits.LATG9 = LED_ON;

					LATEbits.LATE2 = LED_ON;
					LATEbits.LATE6 = LED_ON;
					LATEbits.LATE0 = LED_ON;
					LATEbits.LATE4 = LED_ON;
					LATEbits.LATE8 = LED_ON;
					LATEbits.LATE9 = LED_ON;


					if(i == 100)
						dec = 1;
					else if(i == 1)
						dec = 0;
					if(global.flag_toutes_pwm == 1 && i<101 && i>0 && dec == 0){
						WATCHDOG_create_flag(INC_PWM, (bool_e*) &(global.flag_toutes_pwm));
						global.flag_toutes_pwm = 0;
						i++;
					}else if(global.flag_toutes_pwm == 1 && i<101 && i>0 && dec == 1){
						WATCHDOG_create_flag(INC_PWM, (bool_e*) &(global.flag_toutes_pwm));
						global.flag_toutes_pwm = 0;
						i--;
					}
					PWM_run(i,1);
					PWM_run(i,2);
					PWM_run(i,3);
					PWM_run(i,4);
					if(BUTTON1_PORT == BOUTON_ON && etat_precedent_bp1 == BOUTON_OFF){
						WATCHDOG_stop(id);
						etat_precedent_bp1 = BOUTON_ON;
						etat_test_sorties_e = TEST_SORTIE1;
						//debug_printf("Test pwm1 port D8\n");
						debug_printf("Test fini, rebouclage: Test port D8/LED_RUN\n");
						test_multiple_sorties = 1;
					}	
					else if(BUTTON1_PORT == BOUTON_OFF && etat_precedent_bp1 == BOUTON_ON)
						etat_precedent_bp1 = BOUTON_OFF;
					break;
				default :
					break;
			}
			break;

		case TEST_UART :
			switch(test_uart_e)
			{
				case AUCUN_UART : etat_test_e = AUCUN_TEST;
					break;
				case TRANSMISSION_UART1 :
					UART1_putc('1');
					UART1_putc('\r');
					UART1_putc('\n');
					test_uart_e = RECEPTION_UART1;
					break;
				case RECEPTION_UART1 :
					if(UART1_data_ready()){
						msg = UART1_get_next_msg();
						UART1_putc('1');
						UART1_putc(msg);
						UART1_putc('\r');
						UART1_putc('\n');
						etat_test_e = AUCUN_TEST;
						test_uart_e = AUCUN_UART;
					}
					break;			
				case TRANSMISSION_UART2 :
					UART2_putc('2');
					UART2_putc('\r');
					UART2_putc('\n');
					test_uart_e = RECEPTION_UART2;
					break;
				case RECEPTION_UART2 :
					if(UART2_data_ready())
					{						
						msg = UART2_get_next_msg();
						UART2_putc('2');
						UART2_putc(msg);
						UART2_putc('\r');
						UART2_putc('\n');
						etat_test_e = AUCUN_TEST;
						test_uart_e = AUCUN_UART;
					}
					break;
				default :
					break;
			}
			break;

		case TEST_CAN :
			CAN_send(&canMsg);
			if(CAN_data_ready()) {
				canMsg = CAN_get_next_msg();
			}
			break;

		case AUCUN_TEST :
			break;

		default :
			break;
	}		
}
