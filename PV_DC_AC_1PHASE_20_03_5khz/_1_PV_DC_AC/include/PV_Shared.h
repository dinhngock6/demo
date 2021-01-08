
/*
 * AVC_Share.h
 *
 *  Created on: 15 Feb 2017
 *      Author: dinhngock6
 */

#ifndef _PV_SHARE_H_
#define _PV_SHARE_H_

#ifndef DSP28_DATA_TYPES
#define DSP28_DATA_TYPES
typedef short           int16;
typedef long            int32;
typedef unsigned short  Uint16;
typedef unsigned long   Uint32;
typedef float           float32;
typedef long double     float64;
#endif

// inclue

// end inclue  cac thu vien
#include "PV_Setting.h"
#include "define_main.h"

#include "CLAmath.h"
#include "RAMP_CTL_CLA.h"
#include "RAMP_GEN_CLA.h"
#include "iPARK_CLA.h"
#include "CLARKE_CLA.h"
#include "PARK_CLA.h"
#include "PARK_1PHASE_CLA_PE.h"

#include "PWM_3T_CLA.h"
#include "SinPWM_T_TYPE_1Pha_CLA.h"

#include "PLL_1PHASE_CLA_PE.h"
#include "Calculator_RMS.h"


#include "UFtoUab.h"
#include "Ramp.h"
#include "LowFilter.h"
#include "BandStop_Filter.h"
#include "PI_NORMAL_CLA.h"
#include "PR_Controller.h"
#include "Speed_Calculating.h"
#include "Observer_Controller.h"

#include "PV_Variables.h"
#include "Var.h"

#include "F2837xD_eqep.h"
#include "F2837xD_adc.h"
#include "F2837xD_dac.h"
#include "F2837xD_epwm.h"
#include "F2837xPwmDAC.h"
#include "F2837xPwm.h"
#include "F2837xD_EPwm_defines.h"


//==================================================
// Khai bao cac bien share CPU --> CLA

extern volatile CPU_TO_CLA CpuToCLA;
//========================================================
// Khai bao cac bien share CPU --> CLA
extern volatile CLA_TO_CPU ClaToCPU;

interrupt void Cla1Task1();
interrupt void Cla1Task2();
interrupt void Cla1Task3();
interrupt void Cla1Task4();
interrupt void Cla1Task5();
interrupt void Cla1Task6();
interrupt void Cla1Task7();
interrupt void Cla1Task8();


#endif /*  */
