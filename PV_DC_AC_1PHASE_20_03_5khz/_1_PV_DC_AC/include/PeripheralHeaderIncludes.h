//============================================================================
//============================================================================
//
// FILE:   			PeripheralHeaderIncludes.h (2833x version)
// 
// DESCRIPTION:		Contains F283xx device specific definitions and includes
//
// VERSION:			04 Apr 2008 - (BRL)
//============================================================================
//============================================================================
#ifndef DSP2833x_DEVICE_H
#define DSP2833x_DEVICE_H 1

#ifdef __cplusplus
extern "C" {
#endif

#define CPU_200MHZ 1
#define CPU_150MHZ 0

#if CPU_200MHZ
#define		mSec0_1		20000		// 0.1 mS
#define		mSec0_2		40000		// 0.2 mS
#define		mSec0_5		100000		// 0.5 mS
#define		mSec1		200000		// 1.0 mS
#define		mSec2		400000		// 2.0 mS
#define		mSec5		1000000		// 5.0 mS
#define		mSec7_5		1500000		// 7.5 mS
#define		mSec10		2000000		// 10 mS
#define		mSec20		4000000		// 20 mS
#define		mSec50		10000000	// 50 mS
#define		mSec100		20000000	// 100 mS
#define		mSec500		100000000	// 500 mS
#define		mSec1000	200000000	// 1000 mS
#define		mSec5000	1000000000	// 5000 mS
#endif

#if CPU_150MHZ
//---------------------------------------------------------------------------
// CPU Timer Definitions:
// Timer definitions are based on 150MHz System Clock
// 	if not using a 150MHz clock define a different set of constants elsewhere
#define		mSec0_5		75000		// 0.5 mS
#define		mSec1		150000		// 1.0 mS
#define		mSec2		300000		// 2.0 mS
#define		mSec5		750000		// 5.0 mS
#define		mSec7_5		1125000		// 7.5 mS
#define		mSec10		1500000		// 10 mS
#define		mSec20		3000000		// 20 mS
#define		mSec50		7500000		// 50 mS
#define		mSec100		15000000	// 100 mS
#define		mSec500		75000000	// 500 mS
#define		mSec1000	150000000	// 1000 mS
#define		mSec5000	750000000	// 5000 mS
#endif
//---------------------------------------------------------------------------
// Common CPU Definitions:
//
extern cregister volatile unsigned int IFR;
extern cregister volatile unsigned int IER;

//---------------------------------------------------------------------------
// For Portability, User Is Recommended To Use Following Data Type Size
// Definitions For 16-bit and 32-Bit Signed/Unsigned Integers:
//
#ifndef DSP28_DATA_TYPES
#define DSP28_DATA_TYPES
typedef int             int16;
typedef long            int32;
typedef unsigned int    Uint16;
typedef unsigned long   Uint32;
typedef float           float32;
typedef long double     float64;
#endif

//---------------------------------------------------------------------------
// Include All Peripheral Header Files:
//
#include "F2837xD_device.h"

#include "F2837xD_adc.h"					// ADC Registers
#include "F2837xD_analogsubsys.h"
#include "F2837xD_cla.h"					// CLA Registers
#include "F2837xD_cmpss.h"					//
#include "F2837xD_cputimer.h"				// Timer Registers
#include "F2837xD_dac.h"					// DAC   Registers
#include "F2837xD_dcsm.h"
#include "F2837xD_dma.h"					// DMA Registers
#include "F2837xD_defaultisr.h"
#include "F2837xD_ecap.h"					// Ecap Registers
#include "F2837xD_emif.h"
#include "F2837xD_epwm.h"                	// Enhanced PWM
#include "F2837xD_epwm_xbar.h"				// Eqep Xbar Registers
#include "F2837xD_eqep.h"					// Eqep Registers
#include "F2837xD_flash.h"
#include "F2837xD_gpio.h"                	// General Purpose I/O Registers
#include "F2837xD_i2c.h"					// I2C Registers
#include "F2837xD_input_xbar.h"				// Input Xbar Registers
#include "F2837xD_mcbsp.h"
#include "F2837xD_memconfig.h"
#include "F2837xD_nmiintrupt.h"          // NMI Interrupt Registers
#include "F2837xD_output_xbar.h"
#include "F2837xD_piectrl.h"             // PIE Control Registers
#include "F2837xD_pievect.h"
#include "F2837xD_sci.h"
#include "F2837xD_sdfm.h"
#include "F2837xD_spi.h"				 // Spi Registers
#include "F2837xD_sysctrl.h"             // System Control/Power Modes
#include "F2837xD_upp.h"
#include "F2837xD_xbar.h"
#include "F2837xD_xint.h"                // External Interrupts
#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  // end of DSP2833x_DEVICE_H definition

//===========================================================================
// End of file.
//===========================================================================
