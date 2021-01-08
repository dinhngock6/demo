/*
 * Init_buzzer.c
 *
 *  Created on: 10 Oct 2017
 *      Author: dinhngock6
 */


#include "Led_debug.h"

void Led_Init(void)
{

	EALLOW;
	//--------------------------------------------------------------------------------------

	//  GPIO-31 - PIN FUNCTION = LED_ BLUE
	GpioCtrlRegs.GPAGMUX2.bit.GPIO31 = 0;
	GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP1OUT
	GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;		// 1=OUTput,  0=INput
	GpioDataRegs.GPACLEAR.bit.GPIO31 = 1;	//


	//  GPIO-34 - PIN FUNCTION = LED RED
	GpioCtrlRegs.GPBGMUX1.bit.GPIO34 = 0;
	GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP1OUT
	GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;		// 1=OUTput,  0=INput
	GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;	//
	EDIS;

	LED_BLUE_ON_MARCO;
	LED_RED_ON_MARCO;
}

