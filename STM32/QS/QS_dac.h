/*
 * QS_dac.h
 *
 *  Created on: 23 mars 2016
 *      Author: a.guilmet
 */

#ifndef QS_DAC_H_
	#define QS_DAC_H_

	#include "QS_all.h"

	/**
	 * @brief Fonction d'initialisation des DACs
	 */
	void DAC_init(void);

	void DAC_setValueDAC1(Uint16 Data);

	void DAC_setValueDAC2(Uint16 Data);

	void DAC_setDualValueDAC(Uint16 Data1, Uint16 Data2);

#endif /* QS_DAC_H_ */
