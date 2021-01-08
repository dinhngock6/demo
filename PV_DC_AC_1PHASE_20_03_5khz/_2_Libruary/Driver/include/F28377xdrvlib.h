/*
 * f280xdrvlib.h
 *
 *  Created on: 21 Sep 2017
 *      Author: dinhngock6
 */

#ifndef _DRIVER__F2808_DRIVER_INCLUDE_F280XDRVLIB_H_
#define _DRIVER__F2808_DRIVER_INCLUDE_F280XDRVLIB_H_


//---------------------------------------------------------------------------
// Used to indirectly access all EPWM modules
volatile struct EPWM_REGS *ePWM[] =
 				  { &EPwm1Regs,			//intentional: (ePWM[0] not used)
				  	&EPwm1Regs,
					&EPwm2Regs,
					&EPwm3Regs,
					&EPwm4Regs,
					&EPwm5Regs,
					&EPwm6Regs,
					&EPwm7Regs,
					&EPwm8Regs,
					&EPwm9Regs,
					&EPwm10Regs,
					&EPwm11Regs,
					&EPwm12Regs,
				  };



#endif /* 4_DRIVER__F2808_DRIVER_INCLUDE_F280XDRVLIB_H_ */
