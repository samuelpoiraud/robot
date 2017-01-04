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

/** ----------------  Defines possibles  --------------------
 *	USE_I2C1					: Activation de l'I2C 1
 *	I2C1_CLOCK_SPEED			: Définit par défaut à 100000(100kHz)
 *
 *	USE_I2C2					: Activation de l'I2C 2
 *	I2C2_CLOCK_SPEED			: Définit par défaut à 100000(100kHz)
 *
 *	I2C_DISPLAY_ERROR			: Activation du verbose des erreurs I2C
 *
 *	I2C_ON_DMA					: Activation du DMA pour améliorer la vitesse de l'I2C
 */

 #ifndef QS_I2C_H
	#define QS_I2C_H
	#include "QS_all.h"

	#ifdef USE_I2C1
		#define I2C1_I2C_HANDLE I2C1
		#define I2C1_I2C_CLOCK  RCC_APB1Periph_I2C1
	#endif /* def USE_I2C1 */

	#ifdef USE_I2C2
		#define I2C2_I2C_HANDLE I2C2
		#define I2C2_I2C_CLOCK  RCC_APB1Periph_I2C2
	#endif /* def USE_I2C2 */

	#if defined(USE_I2C1) || defined(USE_I2C2)
		void I2C_init(void);
		void I2C_reset(void);

		bool_e I2C_Read(I2C_TypeDef* I2Cx, Uint8 address, Uint8 *reg, Uint8 nbReg, Uint8 *data, Uint8 nbData);
		bool_e I2C_Write(I2C_TypeDef* I2Cx, Uint8 address, Uint8 *reg, Uint8 nbReg, Uint8 *data, Uint8 nbData);
		bool_e I2C_IsDeviceConnected(I2C_TypeDef* I2Cx, Uint8 address);
	#endif



#endif /* ndef QS_I2C_H */
