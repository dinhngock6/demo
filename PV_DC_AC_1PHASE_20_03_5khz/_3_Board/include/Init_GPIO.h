/*
 * Init_GPIO.h
 *
 *  Created on: 27 Nov 2019
 *      Author: Dinh Ngoc
 */

#ifndef _BOARD_INCLUDE_INIT_GPIO_H_
#define _BOARD_INCLUDE_INIT_GPIO_H_


#include "F2837xD_Device.h"

#define ENABLE_PWM_ON_MARCO       { EALLOW ; GpioDataRegs.GPCSET.bit.GPIO67 =1 ; EDIS;    }
#define ENABLE_PWM_OFF_MARCO      { EALLOW ; GpioDataRegs.GPCCLEAR.bit.GPIO67 = 1 ; EDIS; }
#define ENABLE_PWM_TOGGLE_MARCO   { EALLOW ; GpioDataRegs.GPCTOGGLE.bit.GPIO67= 1 ; EDIS; }

#define ENABLE_GRID_ON_MARCO       { EALLOW ; GpioDataRegs.GPESET.bit.GPIO139 =1 ; EDIS;    }
#define ENABLE_GRID_OFF_MARCO      { EALLOW ; GpioDataRegs.GPECLEAR.bit.GPIO139 = 1 ; EDIS; }
#define ENABLE_GRID_TOGGLE_MARCO   { EALLOW ; GpioDataRegs.GPETOGGLE.bit.GPIO139= 1 ; EDIS; }

#define ENABLE_LOAD_ON_MARCO       { EALLOW ; GpioDataRegs.GPBSET.bit.GPIO56 =1 ; EDIS;    }
#define ENABLE_LOAD_OFF_MARCO      { EALLOW ; GpioDataRegs.GPBCLEAR.bit.GPIO56 = 1 ; EDIS; }
#define ENABLE_LOAD_TOGGLE_MARCO   { EALLOW ; GpioDataRegs.GPBTOGGLE.bit.GPIO56= 1 ; EDIS; }

#define CHARGE_CAP_ON_MARCO       { EALLOW ; GpioDataRegs.GPCSET.bit.GPIO95 =1 ; EDIS;    }
#define CHARGE_CAP_OFF_MARCO      { EALLOW ; GpioDataRegs.GPCCLEAR.bit.GPIO95 = 1 ; EDIS; }

#define RESET_U_ON_MARCO       { EALLOW ; GpioDataRegs.GPDSET.bit.GPIO105 =1 ; EDIS;    }
#define RESET_U_OFF_MARCO      { EALLOW ; GpioDataRegs.GPDCLEAR.bit.GPIO105 = 1 ; EDIS; }
#define RESET_U_TOGGLE_MARCO   { EALLOW ; GpioDataRegs.GPDTOGGLE.bit.GPIO105= 1 ; EDIS; }

#define RESET_V_ON_MARCO       { EALLOW ; GpioDataRegs.GPDSET.bit.GPIO104 =1 ; EDIS;    }
#define RESET_V_OFF_MARCO      { EALLOW ; GpioDataRegs.GPDCLEAR.bit.GPIO104 = 1 ; EDIS; }
#define RESET_V_TOGGLE_MARCO   { EALLOW ; GpioDataRegs.GPDTOGGLE.bit.GPIO104 = 1 ; EDIS; }

#define ENABLE_PWM_ON_MARCO       { EALLOW ; GpioDataRegs.GPCSET.bit.GPIO67 =1 ; EDIS;    }
#define ENABLE_PWM_OFF_MARCO      { EALLOW ; GpioDataRegs.GPCCLEAR.bit.GPIO67 = 1 ; EDIS; }
#define ENABLE_PWM_TOGGLE_MARCO   { EALLOW ; GpioDataRegs.GPCTOGGLE.bit.GPIO67= 1 ; EDIS; }

#define LED_BLUE_ON_MARCO       { EALLOW ; GpioDataRegs.GPASET.bit.GPIO31 =1 ; EDIS;    }
#define LED_BLUE_OFF_MARCO      { EALLOW ; GpioDataRegs.GPACLEAR.bit.GPIO31 = 1 ; EDIS; }
#define LED_BLUE_TOGGLE_MARCO   { EALLOW ; GpioDataRegs.GPATOGGLE.bit.GPIO31= 1 ; EDIS; }

#define LED_RED_ON_MARCO        { EALLOW ; GpioDataRegs.GPBSET.bit.GPIO34 =1 ; EDIS;    }
#define LED_RED_OFF_MARCO       { EALLOW ; GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1 ; EDIS; }
#define LED_RED_TOGGLE_MARCO    { EALLOW ; GpioDataRegs.GPBTOGGLE.bit.GPIO34= 1 ; EDIS; }


void GPIO_Init(void);
void ResetIGBT(void);

#endif /* 3_BOARD_INCLUDE_INIT_GPIO_H_ */
