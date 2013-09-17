/*
 *	Club Robot ESEO 2013
 *	???
 *
 *	Fichier : QS_i2c.h
 *	Package : Qualité Soft
 *	Description : Bus I2C
 *	Auteur : Nirgal
 *  Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 201308
 */
 
 #ifndef QS_I2C_H
	#define QS_I2C_H
	#include "QS_all.h"
	
	void I2C_init(void);
	#define I2C1_I2C_HANDLE I2C1
	#define I2C1_I2C_CLOCK  RCC_APB1Periph_I2C1

	#define I2C2_I2C_HANDLE I2C2
	#define I2C2_I2C_CLOCK  RCC_APB1Periph_I2C2
	
	#ifdef USE_I2C1
		
	#endif /* def USE_I2C1 */
	
	#ifdef USE_I2C2
		bool_e I2C2_read(Uint8 address, Uint8 * data, Uint8 size);
		bool_e I2C2_write(Uint8 address, Uint8 * data, Uint8 size, bool_e enable_stop_condition);
	#endif /* def USE_I2C2 */
	
#endif /* ndef QS_I2C_H */
