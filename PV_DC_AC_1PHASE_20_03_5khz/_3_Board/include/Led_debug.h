/*
 * Init_Buzzer.c
 *
 *  Created on: 10 Oct 2017
 *      Author: dinhngock6
 */

#ifndef _BOARD_INCLUDE_LED_DEBUG_C_
#define _BOARD_INCLUDE_LED_DEBUG_C_

#include "F2837xD_Device.h"

#define LED_BLUE_ON_MARCO   	{ EALLOW ; GpioDataRegs.GPASET.bit.GPIO31 =1 ; EDIS;	}
#define LED_BLUE_OFF_MARCO 		{ EALLOW ; GpioDataRegs.GPACLEAR.bit.GPIO31 = 1 ; EDIS;	}
#define LED_BLUE_TOGGLE_MARCO 	{ EALLOW ; GpioDataRegs.GPATOGGLE.bit.GPIO31= 1 ; EDIS;	}


#define LED_RED_ON_MARCO   		{ EALLOW ; GpioDataRegs.GPBSET.bit.GPIO34 =1 ; EDIS;	}
#define LED_RED_OFF_MARCO 		{ EALLOW ; GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1 ; EDIS;	}
#define LED_RED_TOGGLE_MARCO 	{ EALLOW ; GpioDataRegs.GPBTOGGLE.bit.GPIO34= 1 ; EDIS;	}


void Led_Init(void);


#endif /* 3_BOARD_INCLUDE_INIT_BUZZER_C_ */
