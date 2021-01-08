/*
 * ADC_Conf.h
 *
 *  Created on: 18 Feb 2017
 *      Author: dinhngock6
 */

#ifndef _ADC_CONF_H_
#define _ADC_CONF_H_

#include "PeripheralHeaderIncludes.h"
#include "F2837xD_Examples.h"

void AdcSetMode(Uint16 adc, Uint16 resolution, Uint16 signalmode);
void CalAdcINL(Uint16 adc);

void Init_ADC_A();
void Init_ADC_B();
void Init_ADC_C();
void Init_ADC_D();
#endif /* 2_LIBRUARY_DRIVER_INCLUDE_ADC_CONF_H_ */
