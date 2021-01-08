/*
 * PV_DevInit_F28379D.c
 *
 *  Created on: 15 Feb 2017
 *      Author: dinhngock6
 */
//----------------------------------------------------------------------------------
//
//
//
// DESCRIPTION:	Device initialization for F28377x series
//
//----------------------------------------------------------------------------------
//  Copyright Texas Instruments © 2010
//----------------------------------------------------------------------------------

#include "F2837xD_Cla_defines.h"
#include "F2837xD_Examples.h"
#include "stdint.h"


//#include "F2837xS_GlobalPrototypes.h"
#include "stdio.h"

#include "../include/PV_Shared.h"
#include "PeripheralHeaderIncludes.h"


#pragma diag_suppress 70,770

__interrupt void cla1Isr1();
__interrupt void cla1Isr2();
__interrupt void cla1Isr3();
__interrupt void cla1Isr4();
__interrupt void cla1Isr5();
__interrupt void cla1Isr6();
__interrupt void cla1Isr7();
__interrupt void cla1Isr8();


// Functions that will be run from RAM need to be assigned to
// a different section.  This section will then be mapped to a load and
// run address using the linker cmd file.
#ifdef _FLASH
#pragma CODE_SECTION(InitFlash, "ramfuncs");
#endif


extern DelayUs(unsigned int);
extern DelayMs(unsigned long ms);

extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsLoadSize;
extern Uint16 RamfuncsRunStart;
extern Uint16 RamfuncsRunEnd;
extern Uint16 RamfuncsRunSize;


// used by CLA Initialaization function
//extern Uint16 Cla1funcsLoadStart, Cla1funcsLoadEnd, Cla1funcsRunStart,Cla1Prog_Start;
extern uint32_t Cla1funcsRunStart, Cla1funcsLoadStart, Cla1funcsLoadSize, Cla1Prog_Start;
extern uint32_t Cla1mathTablesRunStart, Cla1mathTablesLoadStart, Cla1mathTablesLoadSize;

void DeviceInit(void);
void PieCntlInit(void);
void PieVectTableInit(void);
void InitPeripheralClocks();
void DisablePeripheralClocks();
void WDogDisable(void);

void InitSysPll(Uint16 clock_source, Uint16 imult, Uint16 fmult, Uint16 divsel);
void InitAuxPll(Uint16 clock_source, Uint16 imult, Uint16 fmult, Uint16 divsel);

void ServiceDog(void);
void DisableDog(void);

void InitFlash(void);
void InitFlash_Bank0(void);
void InitFlash_Bank1(void);

void GPIO_EnableUnbondedIOPullups();

void ISR_ILLEGAL(void);
void CLA_Init(void);

void CLA_configClaMemory(void);
void CLA_initCpu1Cla1(void);


void MemCopy(Uint16 *SourceAddr, Uint16* SourceEndAddr, Uint16* DestAddr);

//--------------------------------------------------------------------
//  Configure Device for target Application Here
//--------------------------------------------------------------------
void DeviceInit(void)
{
	WDogDisable(); 	// Disable the watchdog initially

#ifdef _FLASH
// Copy time critical code and Flash setup code to RAM
// This includes the following functions:  InitFlash();
// The  RamfuncsLoadStart, RamfuncsLoadSize, and RamfuncsRunStart
// symbols are created by the linker. Refer to the device .cmd file.
    memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);

// Call Flash Initialization to setup flash waitstates
// This function must reside in RAM
    InitFlash();
#endif

	DINT;			// Global Disable all Interrupts
	IER = 0x0000;	// Disable CPU interrupts
	IFR = 0x0000;	// Clear all CPU interrupt flags=
	PieCntlInit();
	PieVectTableInit();


    EALLOW;

    //enable pull-ups on unbonded IOs as soon as possible to reduce power consumption.
    GPIO_EnableUnbondedIOPullups();

	CpuSysRegs.PCLKCR13.bit.ADC_A = 1;
	CpuSysRegs.PCLKCR13.bit.ADC_B = 1;
	CpuSysRegs.PCLKCR13.bit.ADC_C = 1;
	CpuSysRegs.PCLKCR13.bit.ADC_D = 1;

    //check if device is trimmed
    if(*((Uint16 *)0x5D1B6) == 0x0000){
        //device is not trimmed, apply static calibration values
        AnalogSubsysRegs.ANAREFTRIMA.all = 31709;
        AnalogSubsysRegs.ANAREFTRIMB.all = 31709;
        AnalogSubsysRegs.ANAREFTRIMC.all = 31709;
        AnalogSubsysRegs.ANAREFTRIMD.all = 31709;
    }

	CpuSysRegs.PCLKCR13.bit.ADC_A = 0;
	CpuSysRegs.PCLKCR13.bit.ADC_B = 0;
	CpuSysRegs.PCLKCR13.bit.ADC_C = 0;
	CpuSysRegs.PCLKCR13.bit.ADC_D = 0;
    EDIS;

// SYSTEM CLOCK speed based on internal oscillator = 10 MHz

   InitSysPll(XTAL_OSC,IMULT_40,FMULT_1,PLLCLK_BY_2); 		//PLLSYSCLK = 10Mhz(OSCCLK) * 40 (IMULT) * 1 (FMULT) /  2 (PLLCLK_BY_2)

   EALLOW; // below registers are "protected", allow access.

// LOW SPEED CLOCKS prescale register settings to SCI,SPI, McBSP
   ClkCfgRegs.LOSPCP.all = 0x0000;		// Sysclk / 1 ( MHz) --> 200MHZ

   EDIS;
/* NOTE: EPWMCLK : default  Sysclk/2. */
// PERIPHERAL CLOCK ENABLES
//---------------------------------------------------
	InitPeripheralClocks();


//--------------------------------------------------------------------------------------
// GPIO (GENERAL PURPOSE I/O) CONFIG
//--------------------------------------------------------------------------------------
//-----------------------
// QUICK NOTES on USAGE:
//-----------------------

//--------------------------------------------------------------------------------------
	EALLOW;

//  GPIO-00 - PIN FUNCTION = PWM1A
	GpioCtrlRegs.GPAGMUX1.bit.GPIO0 = 0;    // if GMUX = 0: 0=GPIO, 1= EPWM1A , 2=Resv , 3=Resv
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;		// 0=GPIO,  1=EPWM2A,  2=Resv,  3=Resv
	GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;		// 1=OUTput,  0=INput

//  GPIO-01 - PIN FUNCTION = PWM1B
	GpioCtrlRegs.GPAGMUX1.bit.GPIO1 = 0;    // if GMUX = 0: 0=GPIO, 1= EPWM1A , 2=Resv , 3=Resv
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;		// 0=GPIO,  1=EPWM1B,  2=Resv,  3=Resv
	GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;		// 1=OUTput,  0=INput

//  GPIO-02 - PIN FUNCTION = PWM2A
	GpioCtrlRegs.GPAGMUX1.bit.GPIO2 = 0;    // if GMUX = 0: 0=GPIO, 1= EPWM2A , 2=Resv , 3=Resv
	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;		// 0=GPIO,  1=EPWM2A,  2=Resv,  3=Resv
	GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;		// 1=OUTput,  0=INput
//--------------------------------------------------------------------------------------
//  GPIO-03 - PIN FUNCTION = PWM2B
	GpioCtrlRegs.GPAGMUX1.bit.GPIO3 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;		// 0=GPIO,  1=EPWM2B,  2=SPISOMI-A,  3=COMP2OUT
	GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;		// 1=OUTput,  0=INput

//--------------------------------------------------------------------------------------
//  GPIO-04 - PIN FUNCTION = PWM3A
	GpioCtrlRegs.GPAGMUX1.bit.GPIO4 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;		// 0=GPIO,  1=EPWM3A,  2=Resv,  3=Resv
	GpioCtrlRegs.GPADIR.bit.GPIO4 = 1;		// 1=OUTput,  0=INput

//--------------------------------------------------------------------------------------
//  GPIO-05 - PIN FUNCTION = PWM3B
	GpioCtrlRegs.GPAGMUX1.bit.GPIO5 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;		// 0=GPIO,  1=EPWM3A,  2=Resv,  3=Resv
	GpioCtrlRegs.GPADIR.bit.GPIO5 = 1;		// 1=OUTput,  0=INput

//--------------------------------------------------------------------------------------
//  GPIO-06 - PIN FUNCTION = PWM4A
	GpioCtrlRegs.GPAGMUX1.bit.GPIO6 = 0;   //
	GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 1;    // 0=GPIO,  1=Resv,  2=Resv,  3=COMP2OUT
	GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;     // 1=OUTput,  0=INput

//--------------------------------------------------------------------------------------
//  GPIO-07 - PIN FUNCTION = PWM4B
	GpioCtrlRegs.GPAGMUX1.bit.GPIO7 = 0;   //
	GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 1;    // 0=GPIO,  1=Resv,  2=Resv,  3=COMP2OUT
	GpioCtrlRegs.GPADIR.bit.GPIO7 = 1;     // 1=OUTput,  0=INput

//--------------------------------------------------------------------------------------
//  GPIO-8 - PIN FUNCTION = PWM 5A
	GpioCtrlRegs.GPAGMUX1.bit.GPIO8 = 0;   //
	GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 1;    // 0=GPIO,  1=Resv,  2=Resv,  3=COMP2OUT
	GpioCtrlRegs.GPADIR.bit.GPIO8 = 1;     // 1=OUTput,  0=INput

//--------------------------------------------------------------------------------------
//  GPIO-9 - PIN FUNCTION = PWM 5B
	GpioCtrlRegs.GPAGMUX1.bit.GPIO9 = 0;   //
	GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 1;    // 0=GPIO,  1=Resv,  2=Resv,  3=COMP2OUT
	GpioCtrlRegs.GPADIR.bit.GPIO9 = 1;     // 1=OUTput,  0=INput
//--------------------------------------------------------------------------------------
//  GPIO-10 - PIN FUNCTION = PWM 6A
	GpioCtrlRegs.GPAPUD.bit.GPIO10 = 0;      //
	GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 1;    //
	GpioCtrlRegs.GPADIR.bit.GPIO10 = 1;     // 1=OUTput,  0=INput
//--------------------------------------------------------------------------------------
//  GPIO-11 - PIN FUNCTION = PWM 6B
	GpioCtrlRegs.GPAPUD.bit.GPIO11 = 0;      //
	GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO11 = 1;     // 1=OUTput,  0=INput
//--------------------------------------------------------------------------------------
//  GPIO-12 - PIN FUNCTION = GPIO
	GpioCtrlRegs.GPAGMUX1.bit.GPIO12 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0;	//
	GpioCtrlRegs.GPADIR.bit.GPIO12 = 1;		// 1=OUTput,  0=INput

//--------------------------------------------------------------------------------------
//  GPIO-14 - PIN FUNCTION = GPIO
	GpioCtrlRegs.GPAGMUX1.bit.GPIO14 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 2;	// 0=GPIO,
	GpioCtrlRegs.GPADIR.bit.GPIO14 = 1;

//--------------------------------------------------------------------------------------
//  GPIO-15 - PIN FUNCTION = GPIO
	GpioCtrlRegs.GPAGMUX1.bit.GPIO15 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 0;	// 0=GPIO,  1=TZ1,  2=LINRX-A,  3=SPISTE-B
	GpioCtrlRegs.GPADIR.bit.GPIO15 = 1;		// 1=OUTput,  0=INput

//--------------------------------------------------------------------------------------
//  GPIO-16 - PIN FUNCTION = FAULT
	GpioCtrlRegs.GPAGMUX2.bit.GPIO16 = 0;
	GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 0;	// 0=GPIO,
	GpioCtrlRegs.GPADIR.bit.GPIO16 = 0;

//--------------------------------------------------------------------------------------
//  GPIO-17 - PIN FUNCTION = GPIO
	GpioCtrlRegs.GPAGMUX2.bit.GPIO17 = 0;
	GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 0;	// 0=GPIO,
	GpioCtrlRegs.GPADIR.bit.GPIO17 = 1;		// 1=OUTput,  0=INput

//--------------------------------------------------------------------------------------
//  GPIO-18 - PIN FUNCTION = CANA_RX
	GpioCtrlRegs.GPAGMUX2.bit.GPIO18 = 0;
	GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 3;	// 0=GPIO, 3=CANA_RX
	GpioCtrlRegs.GPADIR.bit.GPIO18 = 0;		// 1=OUTput,  0=INput

//--------------------------------------------------------------------------------------
//  GPIO-19 - PIN FUNCTION = CANA_TX
	GpioCtrlRegs.GPAGMUX2.bit.GPIO19 = 0;
	GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 3;	// 0=GPIO,  3=CANA_TX
	GpioCtrlRegs.GPADIR.bit.GPIO19 = 1;		// 1=OUTput,  0=INput

//--------------------------------------------------------------------------------------
//  GPIO-20 - PIN FUNCTION = QEP1A
	GpioCtrlRegs.GPAGMUX2.bit.GPIO20 = 0;
	GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 1;	// 0=GPIO,  1=EQEPA-1,  2=Resv,  3=COMP1OUT
	GpioCtrlRegs.GPADIR.bit.GPIO20 = 0;		// 1=OUTput,  0=INput
	GpioCtrlRegs.GPAQSEL2.bit.GPIO20 = 3;    // Asynch input

//--------------------------------------------------------------------------------------
//  GPIO-21 - PIN FUNCTION = QEP1B
    GpioCtrlRegs.GPAGMUX2.bit.GPIO21 = 0;
    GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 1;	// 0=GPIO,  1=EQEPB-1,  2=Resv,  3=COMP2OUT
    GpioCtrlRegs.GPADIR.bit.GPIO21 = 0;		// 1=OUTput,  0=INput
	GpioCtrlRegs.GPAQSEL2.bit.GPIO21 = 3;    // Asynch input

//--------------------------------------------------------------------------------------

//  GPIO-22 - PIN FUNCTION = GPIO22
    GpioCtrlRegs.GPAGMUX2.bit.GPIO22 = 0;
    GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP1OUT
    GpioCtrlRegs.GPADIR.bit.GPIO21 = 0;     // 1=OUTput,  0=INput
	GpioCtrlRegs.GPAQSEL2.bit.GPIO22 = 3;    // Asynch input

//--------------------------------------------------------------------------------------

//  GPIO-23 - PIN FUNCTION = GPIO23
	GpioCtrlRegs.GPAGMUX2.bit.GPIO23 = 0;
	GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 0;	// 0=GPIO,
    GpioCtrlRegs.GPADIR.bit.GPIO21 = 0;     // 1=OUTput,  0=INput
	GpioCtrlRegs.GPAQSEL2.bit.GPIO23 = 3;    // Asynch input

//--------------------------------------------------------------------------------------
//  GPIO-24 - PIN FUNCTION = GPIO24
    GpioCtrlRegs.GPAGMUX2.bit.GPIO24 = 0;
    GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 0;    // 0=GPIO,
    GpioCtrlRegs.GPAQSEL2.bit.GPIO24 = 3;    // Asynch input

//--------------------------------------------------------------------------------------
//  GPIO-25 - PIN FUNCTION = GPIO
	GpioCtrlRegs.GPAGMUX2.bit.GPIO25 = 0;
	GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP1OUT
	GpioCtrlRegs.GPADIR.bit.GPIO25 = 1;		// 1=OUTput,  0=INput
	GpioCtrlRegs.GPAPUD.bit.GPIO25 = 1;     // disable pull-up
	GpioDataRegs.GPACLEAR.bit.GPIO25 = 1;	//
//--------------------------------------------------------------------------------------

//  GPIO-26 - PIN FUNCTION = DI2
	GpioCtrlRegs.GPAGMUX2.bit.GPIO26 = 0;
	GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP1OUT
	GpioCtrlRegs.GPADIR.bit.GPIO26 = 0;		// 1=OUTput,  0=INput
	GpioCtrlRegs.GPAQSEL2.bit.GPIO26 = 3;    // Asynch input
//--------------------------------------------------------------------------------------

//  GPIO-27 - PIN FUNCTION = GPIO
	GpioCtrlRegs.GPAGMUX2.bit.GPIO27 = 0;
	GpioCtrlRegs.GPAMUX2.bit.GPIO27 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP1OUT
	GpioCtrlRegs.GPADIR.bit.GPIO27 = 1;		// 1=OUTput,  0=INput
	GpioDataRegs.GPACLEAR.bit.GPIO27 = 1;	//
//--------------------------------------------------------------------------------------

//  GPIO-29 - PIN FUNCTION = GPIO
	GpioCtrlRegs.GPAGMUX2.bit.GPIO29 = 0;
	GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP1OUT
	GpioCtrlRegs.GPADIR.bit.GPIO29 = 1;		// 1=OUTput,  0=INput
	GpioDataRegs.GPACLEAR.bit.GPIO29 = 1;	//
//--------------------------------------------------------------------------------------

//  GPIO-31 - PIN FUNCTION = LED_ BLUE
	GpioCtrlRegs.GPAGMUX2.bit.GPIO31 = 0;
	GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP1OUT
	GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;		// 1=OUTput,  0=INput
	GpioDataRegs.GPACLEAR.bit.GPIO31 = 1;	//
//--------------------------------------------------------------------------------------

//  GPIO-32 - PIN FUNCTION = GPIO
	GpioCtrlRegs.GPBGMUX1.bit.GPIO32 = 0;
	GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP1OUT
	GpioCtrlRegs.GPBDIR.bit.GPIO32 = 0;		// 1=OUTput,  0=INput
	GpioCtrlRegs.GPBPUD.bit.GPIO32 = 1;     // disable pull-up
	GpioCtrlRegs.GPBQSEL1.bit.GPIO32 = 3;

//--------------------------------------------------------------------------------------
//  GPIO-34 - PIN FUNCTION = LED RED
	GpioCtrlRegs.GPBGMUX1.bit.GPIO34 = 0;
	GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP1OUT
	GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;		// 1=OUTput,  0=INput
	GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;	//

//--------------------------------------------------------------------------------------
 //  GPIO-40 - PIN FUNCTION = GPIO
   	GpioCtrlRegs.GPBGMUX1.bit.GPIO41 = 0;	// 0=GPIO,  1=EPWM7B,  2=Resv,  3=Resv
   	GpioCtrlRegs.GPBMUX1.bit.GPIO41 = 0;
   	GpioCtrlRegs.GPBDIR.bit.GPIO41 = 0;		/* input by default */
   	GpioCtrlRegs.GPBPUD.bit.GPIO41 = 1;     /* pull up disabled - driven by PGA */
    GpioDataRegs.GPBSET.bit.GPIO41 = 1;	 /* default value 1, ignored anyway */

//--------------------------------------------------------------------------------------
//  GPIO-41 - PIN FUNCTION = GPIO
   	GpioCtrlRegs.GPBGMUX1.bit.GPIO41 = 0;	// 0=GPIO,  1=EPWM7B,  2=Resv,  3=Resv
   	GpioCtrlRegs.GPBMUX1.bit.GPIO41 = 0;
   	GpioCtrlRegs.GPBDIR.bit.GPIO41 = 0;		/* input by default */
   	GpioCtrlRegs.GPBPUD.bit.GPIO41 = 1;     /* input by default - output collector */
    GpioDataRegs.GPBSET.bit.GPIO41 = 1;	    /* default value 1, ignored anyway */

//--------------------------------------------------------------------------------------
//  GPIO-42 - PIN FUNCTION = SCITX - A
   	GpioCtrlRegs.GPBGMUX1.bit.GPIO42 = 3;
   	GpioCtrlRegs.GPBMUX1.bit.GPIO42 = 3;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP1OUT
   	GpioCtrlRegs.GPBDIR.bit.GPIO42 = 1;		// 1=OUTput,  0=INput

//--------------------------------------------------------------------------------------
//  GPIO-43 - PIN FUNCTION = SCIRX _A

   	GpioCtrlRegs.GPBGMUX1.bit.GPIO43 = 3;
   	GpioCtrlRegs.GPBMUX1.bit.GPIO43 = 3;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP2OUT
   	GpioCtrlRegs.GPBDIR.bit.GPIO43 = 0;		// 1=OUTput,  0=INput
	GpioCtrlRegs.GPBQSEL1.bit.GPIO43 = 0;   /* no qualification */

//--------------------------------------------------------------------------------------
//  GPIO-52 - PIN FUNCTION = GPIO
	GpioCtrlRegs.GPBGMUX2.bit.GPIO52 = 0;
	GpioCtrlRegs.GPBMUX2.bit.GPIO52 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP2OUT
   	GpioCtrlRegs.GPBDIR.bit.GPIO52 = 1;		// 1=OUTput
   	GpioDataRegs.GPBCLEAR.bit.GPIO52 = 1;

//--------------------------------------------------------------------------------------
//  GPIO-54 - PIN FUNCTION = QEP2A

	GpioCtrlRegs.GPBGMUX2.bit.GPIO54 = 1;
	GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 1;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP2OUT
   	GpioCtrlRegs.GPBDIR.bit.GPIO54 = 0;		//
	GpioCtrlRegs.GPBQSEL2.bit.GPIO54 = 0;
//--------------------------------------------------------------------------------------
 //  GPIO-55 - PIN FUNCTION = QEP2B

	GpioCtrlRegs.GPBGMUX2.bit.GPIO55 = 1;
	GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 1;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP2OUT
   	GpioCtrlRegs.GPBDIR.bit.GPIO55 = 0;		//
	GpioCtrlRegs.GPBQSEL2.bit.GPIO55 = 0;

//--------------------------------------------------------------------------------------
//  GPIO-56 - PIN FUNCTION = ENABLE_LOAD
	GpioCtrlRegs.GPBGMUX2.bit.GPIO56 = 0;
	GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 0;	// 0=GPIO,
	GpioCtrlRegs.GPBDIR.bit.GPIO56 = 1;     // 1=OUTput

//--------------------------------------------------------------------------------------
//  GPIO-57 - PIN FUNCTION = QEP2I
	GpioCtrlRegs.GPBGMUX2.bit.GPIO55 = 1;
	GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 1;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP2OUT
	GpioCtrlRegs.GPBQSEL2.bit.GPIO55 = 0;

//--------------------------------------------------------------------------------------
//  GPIO-60 - PIN FUNCTION = GPIO
   	GpioCtrlRegs.GPBGMUX2.bit.GPIO60 = 3;
   	GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 3;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP2OUT
   	GpioCtrlRegs.GPBPUD.bit.GPIO60 = 0;		 // Enable pull-up

//--------------------------------------------------------------------------------------
//  GPIO-61 - PIN FUNCTION = GPIO
   	GpioCtrlRegs.GPBGMUX2.bit.GPIO61 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP2OUT
   	GpioCtrlRegs.GPBMUX2.bit.GPIO61 = 0;
   	GpioCtrlRegs.GPBPUD.bit.GPIO61 = 0;		 // Enable pull-up

//--------------------------------------------------------------------------------------
//  GPIO-63 - PIN FUNCTION = GPIO
    GpioCtrlRegs.GPBGMUX2.bit.GPIO63 = 0;   //
    GpioCtrlRegs.GPBMUX2.bit.GPIO63 = 0;    // 0=GPIO,  1=Resv,  2=Resv,  3=COMP2OUT
    GpioCtrlRegs.GPBQSEL2.bit.GPIO63 = 0;

//--------------------------------------------------------------------------------------
//  GPIO-64 - PIN FUNCTION = GPIO
   	GpioCtrlRegs.GPCGMUX1.bit.GPIO64 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP2OUT
   	GpioCtrlRegs.GPCMUX1.bit.GPIO64 = 0;
    GpioCtrlRegs.GPCQSEL1.bit.GPIO64 = 0;

//--------------------------------------------------------------------------------------
//  GPIO-65 - PIN FUNCTION = GPIO
    GpioCtrlRegs.GPCGMUX1.bit.GPIO65 = 0;   //
    GpioCtrlRegs.GPCMUX1.bit.GPIO65 = 0;    // 0=GPIO,  1=Resv,  2=Resv,  3=COMP2OUT
    GpioCtrlRegs.GPCQSEL1.bit.GPIO65 = 0;

//--------------------------------------------------------------------------------------
//  GPIO-67 - PIN FUNCTION = ENABLE_PWM
   	GpioCtrlRegs.GPCGMUX1.bit.GPIO67 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP2OUT
   	GpioCtrlRegs.GPCMUX1.bit.GPIO67 = 0;
   	GpioCtrlRegs.GPCDIR.bit.GPIO67 = 1;		 /*output  */
   	GpioDataRegs.GPCCLEAR.bit.GPIO67 = 1;

//--------------------------------------------------------------------------------------
//  GPIO-94 - PIN FUNCTION = GPIO
    GpioCtrlRegs.GPCGMUX2.bit.GPIO94 = 0;   //
    GpioCtrlRegs.GPCMUX2.bit.GPIO94 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP2OUT
    GpioCtrlRegs.GPCDIR.bit.GPIO94 = 1;		/* output by default */
    GpioCtrlRegs.GPCPUD.bit.GPIO94 = 1;		 /* pull up disabled */
    GpioDataRegs.GPCSET.bit.GPIO94 = 1;    	/* default 12b resolution */

//--------------------------------------------------------------------------------------
//  GPIO-95 - PIN FUNCTION = CHARGE_CAP
   	GpioCtrlRegs.GPCGMUX2.bit.GPIO95 = 0;   //
   	GpioCtrlRegs.GPCMUX2.bit.GPIO95 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP2OUT
   	GpioCtrlRegs.GPCDIR.bit.GPIO95 = 1;		// 1=OUTput,  0=INput
   	GpioDataRegs.GPCSET.bit.GPIO95 = 0;		// Enable pull-up
   	GpioDataRegs.GPCCLEAR.bit.GPIO95 = 1 ;  // logic 0

//--------------------------------------------------------------------------------------
//  GPIO-97 - PIN FUNCTION = GPIO
   	GpioCtrlRegs.GPDGMUX1.bit.GPIO97 = 0;   //
   	GpioCtrlRegs.GPDMUX1.bit.GPIO97 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP2OUT
   	GpioCtrlRegs.GPDDIR.bit.GPIO97 = 1;		/* output by default */
   	GpioCtrlRegs.GPDPUD.bit.GPIO97 = 1;		/* pull up disabled */
   	GpioDataRegs.GPDSET.bit.GPIO97 = 1;

//--------------------------------------------------------------------------------------
//  GPIO-99 - PIN FUNCTION = EQEP1I
    GpioCtrlRegs.GPDGMUX1.bit.GPIO99 = 1;   //
    GpioCtrlRegs.GPDMUX1.bit.GPIO99 = 1;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP2OUT
    GpioCtrlRegs.GPDQSEL1.bit.GPIO99 = 3;

//--------------------------------------------------------------------------------------
//  GPIO-104 - PIN FUNCTION = RESET_V
    GpioCtrlRegs.GPDGMUX1.bit.GPIO104 = 0;
    GpioCtrlRegs.GPDMUX1.bit.GPIO104 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP1OUT
    GpioCtrlRegs.GPDDIR.bit.GPIO104 = 1;	// 1=OUTput,  0=INput
    GpioCtrlRegs.GPDPUD.bit.GPIO104 = 1;    // disable pull-up
    GpioDataRegs.GPDCLEAR.bit.GPIO104 = 1;	//

//--------------------------------------------------------------------------------------
//  GPIO-105 - PIN FUNCTION = RESET_U
    GpioCtrlRegs.GPDGMUX1.bit.GPIO105 = 0;
    GpioCtrlRegs.GPDMUX1.bit.GPIO105 = 0;	// 0=GPIO,
    GpioCtrlRegs.GPDDIR.bit.GPIO105 = 1;	// 1=OUTput,  0=INput
    GpioCtrlRegs.GPDPUD.bit.GPIO105 = 1;    // disable pull-up
    GpioDataRegs.GPDSET.bit.GPIO105 = 1;	//

//--------------------------------------------------------------------------------------
//  GPIO-111 - PIN FUNCTION = GPIO
	GpioCtrlRegs.GPDGMUX1.bit.GPIO111 = 0;   //
	GpioCtrlRegs.GPDMUX1.bit.GPIO111 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP2OUT
	GpioCtrlRegs.GPDDIR.bit.GPIO111 = 1;		// 1=OUTput,  0=INput
	GpioCtrlRegs.GPDPUD.bit.GPIO111 = 0;		 // Enable pull-up
	GpioDataRegs.GPDSET.bit.GPIO111 = 1;

//--------------------------------------------------------------------------------------
//  GPIO-122 - PIN FUNCTION = GPIO
	GpioCtrlRegs.GPDGMUX2.bit.GPIO122 = 0;   //
	GpioCtrlRegs.GPDMUX2.bit.GPIO122 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP2OUT
	GpioCtrlRegs.GPDDIR.bit.GPIO122 = 0;		// 1=OUTput,  0=INput
	GpioCtrlRegs.GPDPUD.bit.GPIO122 = 0;		 // Enable pull-up
	GpioDataRegs.GPDSET.bit.GPIO122 = 1;

//--------------------------------------------------------------------------------------
//  GPIO-123 - PIN FUNCTION = GPIO
	GpioCtrlRegs.GPDGMUX2.bit.GPIO123 = 0;   //
	GpioCtrlRegs.GPDMUX2.bit.GPIO123 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP2OUT
	GpioCtrlRegs.GPDDIR.bit.GPIO123 = 0;		// 1=OUTput,  0=INput
	GpioCtrlRegs.GPDPUD.bit.GPIO123 = 0;		 // Enable pull-up
	GpioDataRegs.GPDSET.bit.GPIO123 = 1;

//--------------------------------------------------------------------------------------
//  GPIO-124 - PIN FUNCTION = GPIO
	GpioCtrlRegs.GPDGMUX2.bit.GPIO124 = 0;   //
	GpioCtrlRegs.GPDMUX2.bit.GPIO124 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP2OUT
	GpioCtrlRegs.GPDDIR.bit.GPIO124 = 0;		// 1=OUTput,  0=INput
	GpioCtrlRegs.GPDPUD.bit.GPIO124 = 0;		 // Enable pull-up
	GpioDataRegs.GPDSET.bit.GPIO124 = 1;

//--------------------------------------------------------------------------------------
//  GPIO-125 - PIN FUNCTION = GPIO
	GpioCtrlRegs.GPDGMUX2.bit.GPIO125 = 0;   //
	GpioCtrlRegs.GPDMUX2.bit.GPIO125 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP2OUT
	GpioCtrlRegs.GPDDIR.bit.GPIO125 = 0;		// 1=OUTput,  0=INput
	GpioCtrlRegs.GPDPUD.bit.GPIO125 = 0;		 // Enable pull-up
	GpioDataRegs.GPDSET.bit.GPIO125 = 1;

//--------------------------------------------------------------------------------------
//  GPIO-130 - PIN FUNCTION = GPIO

	GpioCtrlRegs.GPEGMUX1.bit.GPIO130 = 0;   //
	GpioCtrlRegs.GPEMUX1.bit.GPIO130 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP2OUT
	GpioCtrlRegs.GPEDIR.bit.GPIO130 = 0;		// 1=OUTput,  0=INput
	GpioCtrlRegs.GPEPUD.bit.GPIO130 = 0;		 // Enable pull-up
	GpioCtrlRegs.GPEQSEL1.bit.GPIO130 = 3;
//--------------------------------------------------------------------------------------
//  GPIO-131 - PIN FUNCTION = GPIO
	GpioCtrlRegs.GPEGMUX1.bit.GPIO131 = 0;   //
	GpioCtrlRegs.GPEMUX1.bit.GPIO131 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP2OUT
	GpioCtrlRegs.GPEDIR.bit.GPIO131 = 1;		// 1=OUTput,  0=INput
	GpioCtrlRegs.GPEPUD.bit.GPIO131 = 0;		 // Enable pull-up
	GpioDataRegs.GPESET.bit.GPIO131 = 1;

//--------------------------------------------------------------------------------------
//  GPIO-139 - PIN FUNCTION = ENABLE_GRID
	GpioCtrlRegs.GPEGMUX1.bit.GPIO139 = 0;   //
	GpioCtrlRegs.GPEMUX1.bit.GPIO139 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=COMP2OUT
	GpioCtrlRegs.GPEDIR.bit.GPIO139 = 1;		// 1=OUTput,  0=INput
	GpioCtrlRegs.GPEPUD.bit.GPIO139 = 0;		 // Enable pull-up
	GpioDataRegs.GPESET.bit.GPIO139 = 1;

//--------------------------------------------------------------------------------------
//  GPIO-157 - PIN FUNCTION = DAC3 - PWM7A
	GpioCtrlRegs.GPEGMUX2.bit.GPIO157 = 0;   //
	GpioCtrlRegs.GPEMUX2.bit.GPIO157 = 1;	// 1=PWM

//--------------------------------------------------------------------------------------
//  GPIO-158 - PIN FUNCTION = DAC4 - PWM7B
	GpioCtrlRegs.GPEGMUX2.bit.GPIO158 = 0;   //
	GpioCtrlRegs.GPEMUX2.bit.GPIO158 = 1;	// 1=PWM

//--------------------------------------------------------------------------------------
//  GPIO-159 - PIN FUNCTION = DAC1 - PWM8A
	GpioCtrlRegs.GPEGMUX2.bit.GPIO159 = 0;   //
	GpioCtrlRegs.GPEMUX2.bit.GPIO159 = 1;	// 1=PWM

//--------------------------------------------------------------------------------------
//  GPIO-160 - PIN FUNCTION = DAC2 - PWM8B
	GpioCtrlRegs.GPFGMUX1.bit.GPIO160 = 0;   //
	GpioCtrlRegs.GPFMUX1.bit.GPIO160 = 1;	// 1 = PWM


	EDIS;	// Disable register access
}


void CLA_Init(void)
{
	EALLOW;

#ifdef _FLASH
	// Copy over code from FLASH to RAM
	memcpy((uint32_t *)&Cla1funcsRunStart, (uint32_t *)&Cla1funcsLoadStart,
			(uint32_t)&Cla1funcsLoadSize);

#ifndef CLA_MATH_TABLES_IN_ROM

	memcpy((uint32_t *)&Cla1mathTablesRunStart,(uint32_t *) &Cla1mathTablesLoadStart, (uint32_t)&Cla1mathTablesLoadSize);

#endif //CLA_MATH_TABLES_IN_ROM

#endif //_FLASH


	// Initialize and wait for CLA1ToCPUMsgRAM
	MemCfgRegs.MSGxINIT.bit.INIT_CLA1TOCPU = 1;
	while(MemCfgRegs.MSGxINITDONE.bit.INITDONE_CLA1TOCPU != 1){};

	// Initialize and wait for CPUToCLA1MsgRAM
	MemCfgRegs.MSGxINIT.bit.INIT_CPUTOCLA1 = 1;
	while(MemCfgRegs.MSGxINITDONE.bit.INITDONE_CPUTOCLA1 != 1){};



	// Select LS5RAM to be the programming space for the CLA
	// First configure the CLA to be the master for LS5 and then
	// set the space to be a program block
	MemCfgRegs.LSxMSEL.bit.MSEL_LS5 = 1;
	MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS5 = 1;
	MemCfgRegs.LSxMSEL.bit.MSEL_LS4 = 1;
	MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS4 = 1;
	MemCfgRegs.LSxMSEL.bit.MSEL_LS3 = 1;
	MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS3 = 1;
    MemCfgRegs.LSxMSEL.bit.MSEL_LS2 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS2 = 1;
    MemCfgRegs.LSxMSEL.bit.MSEL_LS1 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS1 = 1;

   	MemCfgRegs.LSxMSEL.bit.MSEL_LS0 = 1;        // 0=CPU    1=CPU and CLA
   	MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS0 = 0;


	EDIS;

	//****************************************************************************************/
	// Compute all CLA task vectors
	// On Type-1 CLAs the MVECT registers accept full 16-bit task addresses as
	// opposed to offsets used on older Type-0 CLAs
	EALLOW;
	Cla1Regs.MVECT1 = (uint16_t)(&Cla1Task1);
	Cla1Regs.MVECT2 = (uint16_t)(&Cla1Task2);
	Cla1Regs.MVECT3 = (uint16_t)(&Cla1Task3);
	Cla1Regs.MVECT4 = (uint16_t)(&Cla1Task4);
	Cla1Regs.MVECT5 = (uint16_t)(&Cla1Task5);
	Cla1Regs.MVECT6 = (uint16_t)(&Cla1Task6);
	Cla1Regs.MVECT7 = (uint16_t)(&Cla1Task7);
	Cla1Regs.MVECT8 = (uint16_t)(&Cla1Task8);

	// Enable the IACK instruction to start a task on CLA in software
	// for all  8 CLA tasks. Also, globally enable all 8 tasks (or a
	// subset of tasks) by writing to their respective bits in the
	// MIER register
	Cla1Regs.MCTL.bit.IACKE = 1;
	Cla1Regs.MIER.all 	= (M_INT8 | M_INT1);

	// Configure the vectors for the end-of-task interrupt for all
	// 8 tasks
	PieVectTable.CLA1_1_INT   = &cla1Isr1;
	PieVectTable.CLA1_2_INT   = &cla1Isr2;
	PieVectTable.CLA1_3_INT   = &cla1Isr3;
	PieVectTable.CLA1_4_INT   = &cla1Isr4;
	PieVectTable.CLA1_5_INT   = &cla1Isr5;
	PieVectTable.CLA1_6_INT   = &cla1Isr6;
	PieVectTable.CLA1_7_INT   = &cla1Isr7;
	PieVectTable.CLA1_8_INT   = &cla1Isr8;

	// Set the adca.1 as the trigger for task 1
	DmaClaSrcSelRegs.CLA1TASKSRCSEL1.bit.TASK1 = 1;

	// Enable CLA interrupts at the group and subgroup levels
	PieCtrlRegs.PIEIER11.all  = 0xFFFF;
	IER |= (M_INT11 );
	EDIS;
}
//
// CLA_configClaMemory - Configure CLA memory sections
//
void CLA_configClaMemory(void)
{

	EALLOW;

#ifdef _FLASH
	// Copy over code from FLASH to RAM
	memcpy((uint32_t *)&Cla1funcsRunStart, (uint32_t *)&Cla1funcsLoadStart,
			(uint32_t)&Cla1funcsLoadSize);

#ifndef CLA_MATH_TABLES_IN_ROM

	memcpy((uint32_t *)&Cla1mathTablesRunStart,(uint32_t *) &Cla1mathTablesLoadStart, (uint32_t)&Cla1mathTablesLoadSize);

#endif //CLA_MATH_TABLES_IN_ROM

#endif //_FLASH


	// Initialize and wait for CLA1ToCPUMsgRAM
	MemCfgRegs.MSGxINIT.bit.INIT_CLA1TOCPU = 1;
	while(MemCfgRegs.MSGxINITDONE.bit.INITDONE_CLA1TOCPU != 1){};

	// Initialize and wait for CPUToCLA1MsgRAM
	MemCfgRegs.MSGxINIT.bit.INIT_CPUTOCLA1 = 1;
	while(MemCfgRegs.MSGxINITDONE.bit.INITDONE_CPUTOCLA1 != 1){};



	// Select LS5RAM to be the programming space for the CLA
	// First configure the CLA to be the master for LS5 and then
	// set the space to be a program block
	MemCfgRegs.LSxMSEL.bit.MSEL_LS5 = 1;
	MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS5 = 1;
	MemCfgRegs.LSxMSEL.bit.MSEL_LS4 = 1;
	MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS4 = 1;
	MemCfgRegs.LSxMSEL.bit.MSEL_LS3 = 1;
	MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS3 = 1;


   	MemCfgRegs.LSxMSEL.bit.MSEL_LS0 = 1;        // 0=CPU    1=CPU and CLA
   	MemCfgRegs.LSxMSEL.bit.MSEL_LS1 = 1;        // 0=CPU    1=CPU and CLA
   	MemCfgRegs.LSxMSEL.bit.MSEL_LS2 = 1;        // 0=CPU    1=CPU and CLA
   	MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS0 = 0;
   	MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS1 = 0;
 	MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS2 = 0;
	EDIS;
}
//
// CLA_initCpu1Cla1 - Initialize CLA1 task vectors and end of task interrupts
//
void CLA_initCpu1Cla1(void)
{
	// Compute all CLA task vectors
	// On Type-1 CLAs the MVECT registers accept full 16-bit task addresses as
	// opposed to offsets used on older Type-0 CLAs
	EALLOW;
	Cla1Regs.MVECT1 = (uint16_t)(&Cla1Task1);
	Cla1Regs.MVECT2 = (uint16_t)(&Cla1Task2);
	Cla1Regs.MVECT3 = (uint16_t)(&Cla1Task3);
	Cla1Regs.MVECT4 = (uint16_t)(&Cla1Task4);
	Cla1Regs.MVECT5 = (uint16_t)(&Cla1Task5);
	Cla1Regs.MVECT6 = (uint16_t)(&Cla1Task6);
	Cla1Regs.MVECT7 = (uint16_t)(&Cla1Task7);
	Cla1Regs.MVECT8 = (uint16_t)(&Cla1Task8);

	// Enable the IACK instruction to start a task on CLA in software
	// for all  8 CLA tasks. Also, globally enable all 8 tasks (or a
	// subset of tasks) by writing to their respective bits in the
	// MIER register
	Cla1Regs.MCTL.bit.IACKE = 1;
    Cla1Regs.MIER.all 	= (M_INT8 | M_INT1);

    // Configure the vectors for the end-of-task interrupt for all
    // 8 tasks
	PieVectTable.CLA1_1_INT   = &cla1Isr1;
	PieVectTable.CLA1_2_INT   = &cla1Isr2;
	PieVectTable.CLA1_3_INT   = &cla1Isr3;
	PieVectTable.CLA1_4_INT   = &cla1Isr4;
	PieVectTable.CLA1_5_INT   = &cla1Isr5;
	PieVectTable.CLA1_6_INT   = &cla1Isr6;
	PieVectTable.CLA1_7_INT   = &cla1Isr7;
	PieVectTable.CLA1_8_INT   = &cla1Isr8;

	// Set the adca.1 as the trigger for task 1
	DmaClaSrcSelRegs.CLA1TASKSRCSEL1.bit.TASK1 = 1;

	// Enable CLA interrupts at the group and subgroup levels
	PieCtrlRegs.PIEIER11.all  = 0xFFFF;
	IER |= (M_INT11 );
	EDIS;
}

//============================================================================
// NOTE:
// IN MOST APPLICATIONS THE FUNCTIONS AFTER THIS POINT CAN BE LEFT UNCHANGED
// THE USER NEED NOT REALLY UNDERSTAND THE BELOW CODE TO SUCCESSFULLY RUN THIS
// APPLICATION.
//============================================================================

void WDogDisable(void)
{
    EALLOW;
    WdRegs.WDCR.all = 0x0068;
    EDIS;
}


// This function initializes the PIE control registers to a known state.
//
void PieCntlInit(void)
{
    // Disable Interrupts at the CPU level:
    DINT;

    // Disable the PIE
    PieCtrlRegs.PIECTRL.bit.ENPIE = 0;

	// Clear all PIEIER registers:
	PieCtrlRegs.PIEIER1.all = 0;
	PieCtrlRegs.PIEIER2.all = 0;
	PieCtrlRegs.PIEIER3.all = 0;
	PieCtrlRegs.PIEIER4.all = 0;
	PieCtrlRegs.PIEIER5.all = 0;
	PieCtrlRegs.PIEIER6.all = 0;
	PieCtrlRegs.PIEIER7.all = 0;
	PieCtrlRegs.PIEIER8.all = 0;
	PieCtrlRegs.PIEIER9.all = 0;
	PieCtrlRegs.PIEIER10.all = 0;
	PieCtrlRegs.PIEIER11.all = 0;
	PieCtrlRegs.PIEIER12.all = 0;

	// Clear all PIEIFR registers:
	PieCtrlRegs.PIEIFR1.all = 0;
	PieCtrlRegs.PIEIFR2.all = 0;
	PieCtrlRegs.PIEIFR3.all = 0;
	PieCtrlRegs.PIEIFR4.all = 0;
	PieCtrlRegs.PIEIFR5.all = 0;
	PieCtrlRegs.PIEIFR6.all = 0;
	PieCtrlRegs.PIEIFR7.all = 0;
	PieCtrlRegs.PIEIFR8.all = 0;
	PieCtrlRegs.PIEIFR9.all = 0;
	PieCtrlRegs.PIEIFR10.all = 0;
	PieCtrlRegs.PIEIFR11.all = 0;
	PieCtrlRegs.PIEIFR12.all = 0;
}


void PieVectTableInit(void)
{
    Uint16  i;
    Uint32  *Source  =  (void  *)  &PieVectTableInit;
    Uint32  *Dest  =  (void  *)  &PieVectTable;

// Do not write over first 3 32-bit locations (these locations are
// initialized by Boot ROM with boot variables)

    Source  =  Source  +  3;
    Dest  =  Dest  +  3;

    EALLOW;
    for(i  =  0;  i  <  221;  i++)
    {
    *Dest++  =  *Source++;
    }
    EDIS;

// Enable the PIE Vector Table
    PieCtrlRegs.PIECTRL.bit.ENPIE  =  1;
}

void InitPeripheralClocks()
{
	EALLOW;

	CpuSysRegs.PCLKCR0.bit.CLA1 = 1;
	CpuSysRegs.PCLKCR0.bit.DMA = 1;
	CpuSysRegs.PCLKCR0.bit.CPUTIMER0 = 1;
	CpuSysRegs.PCLKCR0.bit.CPUTIMER1 = 1;
	CpuSysRegs.PCLKCR0.bit.CPUTIMER2 = 1;
	CpuSysRegs.PCLKCR0.bit.HRPWM = 1;
	CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;

	CpuSysRegs.PCLKCR1.bit.EMIF1 = 1;
	CpuSysRegs.PCLKCR1.bit.EMIF2 = 1;

	CpuSysRegs.PCLKCR2.bit.EPWM1 = 1;
	CpuSysRegs.PCLKCR2.bit.EPWM2 = 1;
	CpuSysRegs.PCLKCR2.bit.EPWM3 = 1;
	CpuSysRegs.PCLKCR2.bit.EPWM4 = 1;
	CpuSysRegs.PCLKCR2.bit.EPWM5 = 1;
	CpuSysRegs.PCLKCR2.bit.EPWM6 = 1;
	CpuSysRegs.PCLKCR2.bit.EPWM7 = 1;
	CpuSysRegs.PCLKCR2.bit.EPWM8 = 1;
	CpuSysRegs.PCLKCR2.bit.EPWM9 = 1;
	CpuSysRegs.PCLKCR2.bit.EPWM10 = 1;
	CpuSysRegs.PCLKCR2.bit.EPWM11 = 1;
	CpuSysRegs.PCLKCR2.bit.EPWM12 = 1;

	CpuSysRegs.PCLKCR3.bit.ECAP1 = 1;
	CpuSysRegs.PCLKCR3.bit.ECAP2 = 1;
	CpuSysRegs.PCLKCR3.bit.ECAP3 = 1;
	CpuSysRegs.PCLKCR3.bit.ECAP4 = 1;
	CpuSysRegs.PCLKCR3.bit.ECAP5 = 1;
	CpuSysRegs.PCLKCR3.bit.ECAP6 = 1;

	CpuSysRegs.PCLKCR4.bit.EQEP1 = 1;
	CpuSysRegs.PCLKCR4.bit.EQEP2 = 1;
	CpuSysRegs.PCLKCR4.bit.EQEP3 = 1;

	CpuSysRegs.PCLKCR6.bit.SD1 = 1;
	CpuSysRegs.PCLKCR6.bit.SD2 = 1;

	CpuSysRegs.PCLKCR7.bit.SCI_A = 1;
	CpuSysRegs.PCLKCR7.bit.SCI_B = 1;
	CpuSysRegs.PCLKCR7.bit.SCI_C = 1;
	CpuSysRegs.PCLKCR7.bit.SCI_D = 1;

	CpuSysRegs.PCLKCR8.bit.SPI_A = 1;
	CpuSysRegs.PCLKCR8.bit.SPI_B = 1;
	CpuSysRegs.PCLKCR8.bit.SPI_C = 1;

	CpuSysRegs.PCLKCR9.bit.I2C_A = 1;
	CpuSysRegs.PCLKCR9.bit.I2C_B = 1;

	CpuSysRegs.PCLKCR10.bit.CAN_A = 1;
	CpuSysRegs.PCLKCR10.bit.CAN_B = 1;

	CpuSysRegs.PCLKCR11.bit.McBSP_A = 1;
	CpuSysRegs.PCLKCR11.bit.McBSP_B = 1;
	CpuSysRegs.PCLKCR11.bit.USB_A = 1;

	CpuSysRegs.PCLKCR12.bit.uPP_A = 1;

	CpuSysRegs.PCLKCR13.bit.ADC_A = 1;
	CpuSysRegs.PCLKCR13.bit.ADC_B = 1;
	CpuSysRegs.PCLKCR13.bit.ADC_C = 1;
	CpuSysRegs.PCLKCR13.bit.ADC_D = 1;

	CpuSysRegs.PCLKCR14.bit.CMPSS1 = 1;
	CpuSysRegs.PCLKCR14.bit.CMPSS2 = 1;
	CpuSysRegs.PCLKCR14.bit.CMPSS3 = 1;
	CpuSysRegs.PCLKCR14.bit.CMPSS4 = 1;
	CpuSysRegs.PCLKCR14.bit.CMPSS5 = 1;
	CpuSysRegs.PCLKCR14.bit.CMPSS6 = 1;
	CpuSysRegs.PCLKCR14.bit.CMPSS7 = 1;
	CpuSysRegs.PCLKCR14.bit.CMPSS8 = 1;

	CpuSysRegs.PCLKCR16.bit.DAC_A = 1;
	CpuSysRegs.PCLKCR16.bit.DAC_B = 1;
	CpuSysRegs.PCLKCR16.bit.DAC_C = 1;

	//------------------------------------------------
	CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;   // Enable TBCLK
	//------------------------------------------------

	EDIS;
}

void DisablePeripheralClocks()
{
	EALLOW;

	CpuSysRegs.PCLKCR0.all = 0;
	CpuSysRegs.PCLKCR1.all = 0;
	CpuSysRegs.PCLKCR2.all = 0;
	CpuSysRegs.PCLKCR3.all = 0;
	CpuSysRegs.PCLKCR4.all = 0;
	CpuSysRegs.PCLKCR6.all = 0;
	CpuSysRegs.PCLKCR7.all = 0;
	CpuSysRegs.PCLKCR8.all = 0;
	CpuSysRegs.PCLKCR9.all = 0;
	CpuSysRegs.PCLKCR10.all = 0;
	CpuSysRegs.PCLKCR11.all = 0;
	CpuSysRegs.PCLKCR12.all = 0;
	CpuSysRegs.PCLKCR13.all = 0;
	CpuSysRegs.PCLKCR14.all = 0;
	CpuSysRegs.PCLKCR16.all = 0;

	EDIS;
}

void InitFlash(void)
{
    EALLOW;

    //At reset bank and pump are in sleep
    //A Flash access will power up the bank and pump automatically
    //After a Flash access, bank and pump go to low power mode (configurable in FBFALLBACK/FPAC1 registers)-
    //if there is no further access to flash
    //Power up Flash bank and pump and this also sets the fall back mode of flash and pump as active
    Flash0CtrlRegs.FPAC1.bit.PMPPWR = 0x1;
    Flash0CtrlRegs.FBFALLBACK.bit.BNKPWR0 = 0x3;

    //Disable Cache and prefetch mechanism before changing wait states
    Flash0CtrlRegs.FRD_INTF_CTRL.bit.DATA_CACHE_EN = 0;
    Flash0CtrlRegs.FRD_INTF_CTRL.bit.PREFETCH_EN = 0;

    //Set waitstates according to frequency
    //                CAUTION
    //Minimum waitstates required for the flash operating
    //at a given CPU rate must be characterized by TI.
    //Refer to the datasheet for the latest information.
    #if CPU_FRQ_200MHZ
    Flash0CtrlRegs.FRDCNTL.bit.RWAIT = 0x3;
    #endif

    #if CPU_FRQ_150MHZ
    Flash0CtrlRegs.FRDCNTL.bit.RWAIT = 0x2;
    #endif

    #if CPU_FRQ_120MHZ
    Flash0CtrlRegs.FRDCNTL.bit.RWAIT = 0x2;
    #endif

    //Enable Cache and prefetch mechanism to improve performance
    //of code executed from Flash.
    Flash0CtrlRegs.FRD_INTF_CTRL.bit.DATA_CACHE_EN = 1;
    Flash0CtrlRegs.FRD_INTF_CTRL.bit.PREFETCH_EN = 1;

    //At reset, ECC is enabled
    //If it is disabled by application software and if application again wants to enable ECC
    Flash0EccRegs.ECC_ENABLE.bit.ENABLE = 0xA;

    EDIS;

    //Force a pipeline flush to ensure that the write to
    //the last register configured occurs before returning.

    __asm(" RPT #7 || NOP");

}
void InitFlash_Bank0(void)
{
    EALLOW;

    //At reset bank and pump are in sleep
    //A Flash access will power up the bank and pump automatically
    //After a Flash access, bank and pump go to low power mode (configurable in FBFALLBACK/FPAC1 registers)-
    //if there is no further access to flash
    //Power up Flash bank and pump and this also sets the fall back mode of flash and pump as active
    Flash0CtrlRegs.FPAC1.bit.PMPPWR = 0x1;
    Flash0CtrlRegs.FBFALLBACK.bit.BNKPWR0 = 0x3;

    //Disable Cache and prefetch mechanism before changing wait states
    Flash0CtrlRegs.FRD_INTF_CTRL.bit.DATA_CACHE_EN = 0;
    Flash0CtrlRegs.FRD_INTF_CTRL.bit.PREFETCH_EN = 0;

    //Set waitstates according to frequency
    //                CAUTION
    //Minimum waitstates required for the flash operating
    //at a given CPU rate must be characterized by TI.
    //Refer to the datasheet for the latest information.
    #if CPU_FRQ_200MHZ
    Flash0CtrlRegs.FRDCNTL.bit.RWAIT = 0x3;
    #endif

    #if CPU_FRQ_150MHZ
    Flash0CtrlRegs.FRDCNTL.bit.RWAIT = 0x2;
    #endif

    #if CPU_FRQ_120MHZ
    Flash0CtrlRegs.FRDCNTL.bit.RWAIT = 0x2;
    #endif

    //Enable Cache and prefetch mechanism to improve performance
    //of code executed from Flash.
    Flash0CtrlRegs.FRD_INTF_CTRL.bit.DATA_CACHE_EN = 1;
    Flash0CtrlRegs.FRD_INTF_CTRL.bit.PREFETCH_EN = 1;

    //At reset, ECC is enabled
    //If it is disabled by application software and if application again wants to enable ECC
    Flash0EccRegs.ECC_ENABLE.bit.ENABLE = 0xA;

    EDIS;

    //Force a pipeline flush to ensure that the write to
    //the last register configured occurs before returning.

    __asm(" RPT #7 || NOP");

}

//---------------------------------------------------------------------------
// Example: InitFlash_Bank1
//---------------------------------------------------------------------------
// This function initializes the Flash Control registers for Bank1

/*
void InitFlash_Bank1(void)
{
    EALLOW;

    //At reset bank and pump are in sleep
    //A Flash access will power up the bank and pump automatically
    //After a Flash access, bank and pump go to low power mode (configurable in FBFALLBACK/FPAC1 registers)-
    //if there is no further access to flash
    //Power up Flash bank and pump and this also sets the fall back mode of flash and pump as active
    Flash1CtrlRegs.FPAC1.bit.PMPPWR = 0x1;
    Flash1CtrlRegs.FBFALLBACK.bit.BNKPWR0 = 0x3;

    //Disable Cache and prefetch mechanism before changing wait states
    Flash1CtrlRegs.FRD_INTF_CTRL.bit.DATA_CACHE_EN = 0;
    Flash1CtrlRegs.FRD_INTF_CTRL.bit.PREFETCH_EN = 0;

    //Set waitstates according to frequency
    //                CAUTION
    //Minimum waitstates required for the flash operating
    //at a given CPU rate must be characterized by TI.
    //Refer to the datasheet for the latest information.
    #if CPU_FRQ_200MHZ
    Flash1CtrlRegs.FRDCNTL.bit.RWAIT = 0x3;
    #endif

    #if CPU_FRQ_150MHZ
    Flash1CtrlRegs.FRDCNTL.bit.RWAIT = 0x2;
    #endif

    #if CPU_FRQ_120MHZ
    Flash1CtrlRegs.FRDCNTL.bit.RWAIT = 0x2;
    #endif

    //Enable Cache and prefetch mechanism to improve performance
    //of code executed from Flash.
    Flash1CtrlRegs.FRD_INTF_CTRL.bit.DATA_CACHE_EN = 1;
    Flash1CtrlRegs.FRD_INTF_CTRL.bit.PREFETCH_EN = 1;

    //At reset, ECC is enabled
    //If it is disabled by application software and if application again wants to enable ECC
    Flash1EccRegs.ECC_ENABLE.bit.ENABLE = 0xA;

    EDIS;

    //Force a pipeline flush to ensure that the write to
    //the last register configured occurs before returning.

    __asm(" RPT #7 || NOP");

}

*/
//---------------------------------------------------------------------------
// Example: ServiceDog:
//---------------------------------------------------------------------------
// This function resets the watchdog timer.
// Enable this function for using ServiceDog in the application

void ServiceDog(void)
{
    EALLOW;
    WdRegs.WDKEY.bit.WDKEY = 0x0055;
    WdRegs.WDKEY.bit.WDKEY = 0x00AA;
    EDIS;
}

//---------------------------------------------------------------------------
// Example: DisableDog:
//---------------------------------------------------------------------------
// This function disables the watchdog timer.

void DisableDog(void)
{
	volatile Uint16 temp;
    EALLOW;
    //Grab the clock config so we don't clobber it
    temp = WdRegs.WDCR.all & 0x0007;
    WdRegs.WDCR.all = 0x0068 | temp;
    EDIS;
}

//---------------------------------------------------------------------------
// Example: InitPll:
//---------------------------------------------------------------------------
// This function initializes the PLL registers.
//
// Note: The internal oscillator CANNOT be used as the PLL source if the
// PLLSYSCLK is configured to frequencies above 194 MHz.

void InitSysPll(Uint16 clock_source, Uint16 imult, Uint16 fmult, Uint16 divsel)
{
    if((clock_source == ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL)    &&
      (imult         == ClkCfgRegs.SYSPLLMULT.bit.IMULT)           &&
      (fmult         == ClkCfgRegs.SYSPLLMULT.bit.FMULT)           &&
      (divsel        == ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV))
    {
        //everything is set as required, so just return
        return;
    }

    if(clock_source != ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL)
    {
        switch (clock_source)
        {
            case INT_OSC1:
            {
                EALLOW;
                ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL = 2; // Clk Src = INTOSC1
                EDIS;
            }
                break;

            case INT_OSC2:
            {
                EALLOW;
                ClkCfgRegs.CLKSRCCTL1.bit.INTOSC2OFF=0;     // Turn on INTOSC2
                ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL = 0; // Clk Src = INTOSC2
                EDIS;
            }
                break;

            case XTAL_OSC:
            {
            	 EALLOW;
            	 ClkCfgRegs.CLKSRCCTL1.bit.XTALOFF=0;        // Turn on XTALOSC
            	 ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL = 1; // Clk Src = XTAL
            }
            	    EDIS;
                break;
        }
    }

    EALLOW;
     // first modify the PLL multipliers
    if(imult != ClkCfgRegs.SYSPLLMULT.bit.IMULT || fmult != ClkCfgRegs.SYSPLLMULT.bit.FMULT)
    {
        // Bypass PLL and set dividers to /1
        ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 0;
        ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV = 0;

        // Program PLL multipliers
        Uint32 temp_syspllmult = ClkCfgRegs.SYSPLLMULT.all;
        ClkCfgRegs.SYSPLLMULT.all = ((temp_syspllmult & ~(0x37FU)) |
                                     ((fmult << 8U) | imult));

        ClkCfgRegs.SYSPLLCTL1.bit.PLLEN = 1;            // Enable SYSPLL

        // Wait for the SYSPLL lock
        while(ClkCfgRegs.SYSPLLSTS.bit.LOCKS != 1)
        {
            // Uncomment to service the watchdog
            // ServiceDog();
        }

        // Write a multiplier again to ensure proper PLL initialization
        // This will force the PLL to lock a second time
        ClkCfgRegs.SYSPLLMULT.bit.IMULT = imult;        // Setting integer multiplier

        // Wait for the SYSPLL re-lock
        while(ClkCfgRegs.SYSPLLSTS.bit.LOCKS != 1)
        {
            // Uncomment to service the watchdog
            // ServiceDog();
        }
    }

    // Set divider to produce slower output frequency to limit current increase
    if(divsel != PLLCLK_BY_126)
    {
         ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV = divsel + 1;
    }else
    {
         ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV = divsel;
    }

    // Enable PLLSYSCLK is fed from system PLL clock
    ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 1;

    // Small 100 cycle delay
    asm(" RPT #100 || NOP");

    // Set the divider to user value
    ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV = divsel;
    EDIS;
}


//---------------------------------------------------------------------------
// Example: InitPll2:
//---------------------------------------------------------------------------
// This function initializes the PLL2 registers.
//
// Note: The internal oscillator CANNOT be used as the PLL source if the
// PLLSYSCLK is configured to frequencies above 194 MHz.

void InitAuxPll(Uint16 clock_source, Uint16 imult, Uint16 fmult, Uint16 divsel)
{
	Uint16 temp_divsel;

	if((clock_source == ClkCfgRegs.CLKSRCCTL2.bit.AUXOSCCLKSRCSEL)   &&
	  (imult		 == ClkCfgRegs.AUXPLLMULT.bit.IMULT) 	      &&
	  (fmult 	     == ClkCfgRegs.AUXPLLMULT.bit.FMULT)          &&
	  (divsel        == ClkCfgRegs.AUXCLKDIVSEL.bit.AUXPLLDIV))
    {
	    //everything is set as required, so just return
	    return;
	}

	switch (clock_source)
	{

		case INT_OSC2:
			  EALLOW;
			  ClkCfgRegs.CLKSRCCTL1.bit.INTOSC2OFF=0;     // Turn on INTOSC2
			  ClkCfgRegs.CLKSRCCTL2.bit.AUXOSCCLKSRCSEL = 0; // Clk Src = INTOSC2
			  EDIS;
			break;

		case XTAL_OSC:
			 EALLOW;
			 ClkCfgRegs.CLKSRCCTL1.bit.XTALOFF=0;        // Turn on XTALOSC
			 ClkCfgRegs.CLKSRCCTL2.bit.AUXOSCCLKSRCSEL = 1; // Clk Src = XTAL
			 EDIS;
			break;

		case AUXCLKIN:
			 EALLOW;
			 ClkCfgRegs.CLKSRCCTL2.bit.AUXOSCCLKSRCSEL = 2; // Clk Src = XTAL
			 EDIS;
			break;

	}

   // Change the SYSPLL Integer Multiplier (or) SYSPLL Fractional Multiplier
   if(ClkCfgRegs.AUXPLLMULT.bit.IMULT != imult || ClkCfgRegs.AUXPLLMULT.bit.FMULT !=fmult)
   {
	   EALLOW;
	   ClkCfgRegs.AUXCLKDIVSEL.bit.AUXPLLDIV = AUXPLLRAWCLK_BY_8;

       //Set integer and fractional multiplier
       Uint32 temp_auxpllmult = ClkCfgRegs.AUXPLLMULT.all;
       ClkCfgRegs.AUXPLLMULT.all = ((temp_auxpllmult & ~(0x37FU)) |
                                    ((fmult << 8U) | imult));

	   ClkCfgRegs.AUXPLLCTL1.bit.PLLEN = 1;			//Enable AUXPLL
	   EDIS;

	   //Wait for the AUXPLL lock
	   while(ClkCfgRegs.AUXPLLSTS.bit.LOCKS != 1)
	   {
	        // Uncomment to service the watchdog
	        // ServiceDog();
	   }

       // Write a multiplier again to ensure proper PLL initialization
       // This will force the PLL to lock a second time
       EALLOW;
       ClkCfgRegs.AUXPLLMULT.bit.IMULT = imult;        // Setting integer multiplier
       EDIS;

       //Wait for the AUXPLL lock
	   while(ClkCfgRegs.AUXPLLSTS.bit.LOCKS != 1)
	   {
	        // Uncomment to service the watchdog
	        // ServiceDog();
	   }
   }

	 //increase the freq. of operation in steps to avoid any VDD fluctuations
	 temp_divsel = AUXPLLRAWCLK_BY_8;
	 while(ClkCfgRegs.AUXCLKDIVSEL.bit.AUXPLLDIV != divsel)
	 {
		 EALLOW;
		 ClkCfgRegs.AUXCLKDIVSEL.bit.AUXPLLDIV = temp_divsel - 1;
		 EDIS;

		 temp_divsel = temp_divsel - 1;
		 if(ClkCfgRegs.AUXCLKDIVSEL.bit.AUXPLLDIV != divsel)
		 {
			 DelayUs(15);
		 }
	 }

   EALLOW;
   ClkCfgRegs.AUXPLLCTL1.bit.PLLCLKEN = 1;			//Enable AUXPLLCLK is fed from AUX PLL
   EDIS;
}


void GPIO_EnableUnbondedIOPullups()
{
    unsigned char pin_count = ((DevCfgRegs.PARTIDL.all & 0x00000700) >> 8) ; //bits 8-10 have pin count*/
/*
 * 5 = 100 pin
 * 6 = 176 pin
 * 7 = 337 pin
 */

    if(pin_count == 5)
    {
        //GPIO_EnableUnbondedIOPullupsFor100Pin();
		EALLOW;
		GpioCtrlRegs.GPAPUD.all = ~0xFFC003E3;  //GPIOs 0-1, 5-9, 22-31
		GpioCtrlRegs.GPBPUD.all = ~0x03FFF1FF;  //GPIOs 32-40, 44-57
		GpioCtrlRegs.GPCPUD.all = ~0xE10FBC18;  //GPIOs 67-68, 74-77, 79-83, 93-95
		GpioCtrlRegs.GPDPUD.all = ~0xFFFFFFF7;  //GPIOs 96-127
		GpioCtrlRegs.GPEPUD.all = ~0xFFFFFFFF;  //GPIOs 128-159
		GpioCtrlRegs.GPFPUD.all = ~0x000001FF;  //GPIOs 160-168
		EDIS;
    }
    else if (pin_count == 6)
    {
        //GPIO_EnableUnbondedIOPullupsFor176Pin();
    	EALLOW;
		GpioCtrlRegs.GPCPUD.all = ~0x80000000;  //GPIO 95
		GpioCtrlRegs.GPDPUD.all = ~0xFFFFFFF7;  //GPIOs 96-127
		GpioCtrlRegs.GPEPUD.all = ~0xFFFFFFDF;  //GPIOs 128-159 except for 133
		GpioCtrlRegs.GPFPUD.all = ~0x000001FF;  //GPIOs 160-168
		EDIS;
    }
    else
    {
        //do nothing - this is 337 pin package
    }

}
interrupt void ISR_ILLEGAL(void)   // Illegal operation TRAP
{
  // Insert ISR Code here

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm("          ESTOP0");
  for(;;);

}

// This function initializes the Flash Control registers

//                   CAUTION
// This function MUST be executed out of RAM. Executing it
// out of OTP/Flash will yield unpredictable results



// This function will copy the specified memory contents from
// one location to another.
//
//	Uint16 *SourceAddr        Pointer to the first word to be moved
//                          SourceAddr < SourceEndAddr
//	Uint16* SourceEndAddr     Pointer to the last word to be moved
//	Uint16* DestAddr          Pointer to the first destination word
//
// No checks are made for invalid memory locations or that the
// end address is > then the first start address.

void MemCopy(Uint16 *SourceAddr, Uint16* SourceEndAddr, Uint16* DestAddr)
{
    while(SourceAddr < SourceEndAddr)
    {
       *DestAddr++ = *SourceAddr++;
    }
    return;
}


//===========================================================================
// End of file.
//===========================================================================













