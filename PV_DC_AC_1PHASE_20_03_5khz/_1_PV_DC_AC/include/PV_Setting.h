/*
 * PV_Setting.h
 *
 *  Created on: 15 Feb 2017
 *      Author: dinhngock6
 */

#ifndef _PV_SETTING_H_
#define _PV_SETTING_H_

/*------------------------------------------------------------------------------
Following is the list of the Build Level choices.
------------------------------------------------------------------------------*/
#define LEVEL1  1           //   current controler
#define LEVEL2  2           //   speed controler
#define LEVEL3  3           //   position controler
#define LEVEL4  4           //
#define LEVEL5  5           //
#define LEVEL6  6           //
#define LEVEL7  7           //
#define LEVELALL 8          //
/*------------------------------------------------------------------------------
This line sets the BUILDLEVEL to one of the available choices.
------------------------------------------------------------------------------*/
#define   BUILDLEVEL LEVEL6

#define   TYPHOON_HIL 0

#define PI 3.14159265358979
#define can2 1.414213562
#define can3 1.732050808

// Define the base quantites for PU system conversion
#define NORMAL_FREQ     50.0
#define BASE_FREQ       100.0           // Base electrical frequency (Hz)
#define Udc_max         800.0           // Udc_max = 800

#define Us_max          400.0           // U dien ap pha lon nhat = 350
#define Is_max          75.0              // Base peak phase current (amp)
#define Iload_max       50.0              // Base peak phase current (amp)

#define Idm             25

#define Fr              50.0
#define Wref            (2.0*PI*Fr)
#define Wmax            (2.0*PI*BASE_FREQ)
#define GRID_FREQUENCY  50.0

/*************khoi tao he thong ***************/

#define SYSTEM_FREQUENCY 200

#define T_udc            0.002          // time sample voltage Udc
#define T_us             0.0002          // time sample voltage Us
#define Ti               0.0002            // time sample current
#define T                0.0002            // time sample
#define SVM_SYSTEM       10000
#define ISR_FREQUENCY    5000
#define FREQUENCY_CLOCK_PWM 100000000

#define Time_interrupt   100.0
#define NUMBER_SAMPLE    100.0

//------------------------------------------------------------------
// ADC
#define ADC_VREFHI 3.0

#define ADC_SCALE_MEASURE_VOLTAGE_DC (600.0/3.0)
#define ADC_SCALE_MEASURE_VOLTAGE_AC (800.0/3.0)
#define ADC_SCALE_MEASURE_CURRENT_AC (150.0/3.0)    // 75A max


#define ADC_PU_SCALE_FACTOR_16BIT        0.0000152587890625         //1/2^12=0.000244140625
#define ADC_PU_SCALE_FACTOR_12BIT        0.000244140625             //1/2^12=0.000244140625
#define ADC_PU_PPB_SCALE_FACTOR          0.000488281250             //1/2^11
#define SD_PU_SCALE_FACTOR               0.000030517578125          // 1/2^15

/*------------------------------------------------------------------------------
Current sensors scaling
------------------------------------------------------------------------------*/
// LEM    1.0pu current ==> 50.0A -> 2048 counts
#define LEM(A)     (2048.0*A/Is_max)

//------------------------------------------------------------------
// ADC Related defines
#if (TYPHOON_HIL == 1)

#define UDC_HCPL  AdcaResultRegs.ADCRESULT0
#define UDC1_HCPL AdcbResultRegs.ADCRESULT0
#define UDC2_HCPL AdccResultRegs.ADCRESULT0
#define UDC1_HCPL_PPB ((signed int)AdcaResultRegs.ADCPPB1RESULT.all)
#define UDC2_HCPL_PPB ((signed int)AdcbResultRegs.ADCPPB1RESULT.all)
#define UDC_HCPL_PPB ((signed int)AdccResultRegs.ADCPPB2RESULT.all)

#define VL_HCPL     AdcaResultRegs.ADCRESULT1
#define I_INV_LEM   AdcaResultRegs.ADCRESULT2

#define TEMP_IGBT   AdcaResultRegs.ADCRESULT2

#define VG_HCPL   AdcaResultRegs.ADCRESULT1
#define IL_LEM    AdcaResultRegs.ADCRESULT2

#else
#define UDC1_HCPL   AdcaResultRegs.ADCRESULT0
#define VL_HCPL     AdcaResultRegs.ADCRESULT1
#define TEMP_IGBT   AdcaResultRegs.ADCRESULT2

#define UDC2_HCPL   AdcbResultRegs.ADCRESULT0
#define I_INV_LEM   AdcbResultRegs.ADCRESULT1
#define I_INV_LEM_TB   ((AdcbResultRegs.ADCRESULT1 + AdcbResultRegs.ADCRESULT2)/2.0)

#define VG_HCPL   AdccResultRegs.ADCRESULT0
#define IL_LEM    AdccResultRegs.ADCRESULT1

#define UDC1_HCPL_PPB   ((signed int)AdcaResultRegs.ADCPPB1RESULT.all)
#define VL_HCPL_PPB     ((signed int)AdcaResultRegs.ADCPPB2RESULT.all)
#define TEMP_ADC_PPB    ((signed int)AdcaResultRegs.ADCPPB3RESULT.all)

#define UDC2_HCPL_PPB   ((signed int)AdcbResultRegs.ADCPPB1RESULT.all)
#define I_INV_LEM_PPB   ((signed int)AdcbResultRegs.ADCPPB2RESULT.all)

#define VG_HCPL_PPB     ((signed int)AdccResultRegs.ADCPPB1RESULT.all)
#define IL_LEM_PPB      ((signed int)AdccResultRegs.ADCPPB2RESULT.all)

#endif

//--------------------------------------------------------
// Tham so bo dieu khien

#define Kp_pll          0.5
#define Ki_pll          18.0

//--------------------------------------------------------
#if (BUILDLEVEL == LEVEL2)

#define KP_CURR_LOOP            5.1285
#define KI_CURR_LOOP            1000

#endif

//--------------------------------------------------------
// Current loop stand alone
#if (BUILDLEVEL == LEVEL3)

#define KP_CURR_LOOP            2.0
#define KI_CURR_LOOP            5000

#endif

//--------------------------------------------------------
// Voltage loop stand alone
#if (BUILDLEVEL == LEVEL4)

#define KP_CURR_LOOP            2.0
#define KI_CURR_LOOP            5000

#define KP_VOLT_US_LOOP         0.05
#define KI_VOLT_US_LOOP         100

#endif

//--------------------------------------------------------
// Current loop grid mode
#if (BUILDLEVEL == LEVEL5)

#define KP_CURR_LOOP            4.0
#define KI_CURR_LOOP            5000.0

#endif

//--------------------------------------------------------
// voltage loop grid mode
#if (BUILDLEVEL == LEVEL6)

#define KP_CURR_LOOP         4.0
#define KI_CURR_LOOP         5000.0

#define KP_VOLT_UDC_LOOP     0.1941
#define KI_VOLT_UDC_LOOP     1.0

#endif

//-----------------------------------------------------------------------------
// SETTING PV DC AC 1 PHASE

#define PV_POWER                  5
#define PV_VOLTAGE                250
#define PV_CURRENT                25
#define PV_FREQUENCY              50
#define PV_COSPHI                 0.9
#define PV_UDC                    400

#define PV_FREQUENCY_MAX          60
#define PV_FREQUENCY_MIN          40

#define PV_UDC_MAX                600
#define PV_UDC_MIN                20

#define PV_VGRID_MAX              300
#define PV_VGRID_MIN              20.0
#define PV_IGRID_MAX              40

#define PV_VLOAD_MAX              300
#define PV_VLOAD_MIN              0
#define PV_ILOAD_MAX              40

#define PV_TEMP_MAX               100

//------------------------------------------------------------------------------

#define DO_HIGH     { EALLOW ; GpioDataRegs.GPBSET.bit.GPIO32 =1 ; EDIS;    }
#define DO_LOW      { EALLOW ; GpioDataRegs.GPBCLEAR.bit.GPIO32 = 1 ; EDIS; }
#define DO_TOGGLE   { EALLOW ; GpioDataRegs.GPBTOGGLE.bit.GPIO32= 1 ; EDIS; }


#endif /* _AVC_SETTING_H_ */
