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
 *	USE_I2C2					: Activation de l'I2C 2
 */

 #ifndef QS_I2C_H
	#define QS_I2C_H
	#include "QS_all.h"

	void I2C_init(void);
	void I2C_reset(void);

	#ifdef USE_I2C1
		bool_e I2C1_read(Uint8 address, Uint8 * data, Uint8 size);
		bool_e I2C1_write(Uint8 address, Uint8 * data, Uint8 size, bool_e enable_stop_condition);
	#endif /* def USE_I2C1 */

	#ifdef USE_I2C2
		bool_e I2C2_read(Uint8 address, Uint8 * data, Uint8 size);
		bool_e I2C2_write(Uint8 address, Uint8 * data, Uint8 size, bool_e enable_stop_condition);
	#endif /* def USE_I2C2 */

#endif /* ndef QS_I2C_H */
