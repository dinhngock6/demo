/*
 * AVC_Main.c
 *
 *  Created on: 15 Feb 2017
 *      Author: dinhngock6
 */
#include "IQmathLib.h"
#include "F2837xD_Cla_defines.h"
#include "F2837xD_EPwm_defines.h"

#include "PV.h"
#include "PV_Shared.h"
#include "PeripheralHeaderIncludes.h"


interrupt void CANIntHandler(void);
__interrupt void epwm_isr(void);

//================================================================
// Khai bao cac ham
void DeviceInit(void);
void CLA_Init();
void PieCntlInit(void);
void PieVectTableInit(void);

void TripZone_Protection(void);
void CMPSS_Protection(void);

void SendMsgCan(uint16_t ID,uint16_t data1,uint16_t data2 ,uint16_t data3,uint16_t data4);
void ReciveCan(tCANMsgObject *pMsgObject, MsgDataCan *data);

//====================================================================

//====================================================================
// Alpha states
void A0(void);  //state A0
void B0(void);  //state B0
void C0(void);  //state C0

// A branch states    : A- TASKS
void STOP_STATE(void);
void RUN_STATE(void);
void CONFIG_STATE(void);
void CHECKING_STATE(void);

// B branch states    : B- TASKS
void SEND_CAN(void);

// C branch states
void C1(void);  //state C1

// Variable declarations
void (*Alpha_State_Ptr)(void);  // Base States pointer
void (*A_Task_Ptr)(void);       // State pointer A branch
void (*B_Task_Ptr)(void);       // State pointer B branch
void (*C_Task_Ptr)(void);       // State pointer C branch


int16   VTimer0[4];                 // Virtual Timers slaved off CPU Timer 0
int16   VTimer1[4];                 // Virtual Timers slaved off CPU Timer 1
int16   VTimer2[4];                 // Virtual Timers slaved off CPU Timer 2

float Datalog1[200],Datalog2[200];

PWMGEN pwm1 = PWMGEN_DEFAULTS;

//==========================================================================
// CMPSS parameters for Over Current Protection
Uint16  clkPrescale = 30,
		sampwin     = 30,
		thresh      = 18,
		LEM_curHi   = LEM(40.0),
		LEM_curLo   = LEM(40.0);

float curLimit = 55.0;
float curPeakLimit = 55.0;
//==========================================================================
// CAN
volatile unsigned long g_ulTxMsgCount = 0;
volatile unsigned long g_ulRxMsgCount = 0;
unsigned long u32CanAErrorStatus;
volatile unsigned long g_bErrFlag = 0;     // A flag to indicate that some
volatile unsigned long ulStatus;

// transmission error occurred.
tCANMsgObject sTXCANMessage;
tCANMsgObject sRXCANMessage;
tCANBitClkParms canbit = {5,4,1,40};

MsgDataCan TxMsgData,RxMsgData;
unsigned char ucTXMsgData[8] ={ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00 };
unsigned char ucRXMsgData[8];
unsigned int TxData[4];
unsigned int RxID, TxID = 0x00;

//===================================================================
SETTING_PV Setting_PV;
LIMIT_PV Limit;

Uint16 START, StartFlag = 0;

Uint16 error = 0;

int ChannelAdc = 0;

Uint16 Task1_Isr = 0;
Uint16 Task8_Isr = 0;
Uint16 Pwm_Isr = 0;

// adc static cal
int *adc_cal;


Uint16 RESET_IGBT = 0;
Uint16 ResetIGBTFlag = 0;
Uint16 TripFlagIGBT = 0;
Uint16 reset_charge = 0;
Uint16 OverCurrentMeasure = 0;
Uint16 clearTripFlagDMC = 0;

Uint16 OverCurrentChargeCap = 0;
Uint16 ChargeCapFlag = 0;
float data = 0;
//==========================================================================
void main(){

    DeviceInit();       // init clock system, clock ngoai vi , set RAM or ROM

    //---------------------------------------------------------------------------
    CpuTimer0Regs.PRD.all =  mSec5;     // A tasks
    CpuTimer1Regs.PRD.all =  mSec500;   // B tasks
    CpuTimer2Regs.PRD.all =  mSec1000;  // C tasks

    // Tasks State-machine init
    Alpha_State_Ptr = &A0;
    A_Task_Ptr = &STOP_STATE;
    B_Task_Ptr = &SEND_CAN;
    C_Task_Ptr = &C1;

    /*************************************************************************/
    CLA_Init();

    Cla1ForceTask8();
    Init_ADC_A();
    Init_ADC_B();
    Init_ADC_C();

    // TRIP ZONE PWM
    EALLOW;
    InputXbarRegs.INPUT1SELECT = 16;
    EDIS;

    // Initialize the PWM module

    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    EDIS;

    pwm1.PeriodMax = (FREQUENCY_CLOCK_PWM/(2*SVM_SYSTEM)); /* Initializa ePWM */
    PWM_INIT_MACRO(pwm1);
    PWM_DAC_CNF(7,125); // 20Khz :500; 40kHz: 250; 80 kHz: 125
    PWM_DAC_CNF(8,125); // 20Khz :500; 40kHz: 250; 80 kHz: 125
    TripZone_Protection();
    CMPSS_Protection();

    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;

    // ******************************************************
    // static analog trim for all ADCs (A, B, C and D)
    // *******************************************************
//  adc_cal=(int*)0x0000743F;
//  *adc_cal=0x7000;
//  adc_cal=(int*)0x000074BF;
//  *adc_cal=0x7000;
//  adc_cal=(int*)0x0000753F;
//  *adc_cal=0x7000;
//  adc_cal=(int*)0x000075BF;
//  *adc_cal=0x7000;

    /**************************************************************************/

    // Step 7. Enable global Interrupts and higher priority real-time debug events:
    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global realtime interrupt DBGM

    //---------------------------------------------------------
    CANInit(CANA_BASE);                             // Initialize the CAN controller
    CANClkSourceSelect(CANA_BASE, 0);               // Setup CAN to be clocked off the PLL output clock (500kHz CAN-Clock)
    CANBitRateSet(CANA_BASE, 200000000, 500000);    //the CAN bus is set to 500 kHz
    CANBitTimingSet(CANA_BASE, &canbit);
    CANIntEnable(CANA_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);  // Enable interrupts on the CAN peripheral.

    // ***************************************************************
    EALLOW;
    PieVectTable.CANA0_INT = &CANIntHandler;
    PieVectTable.EPWM1_INT = &epwm_isr;

    PieCtrlRegs.PIEIER11.bit.INTx1 = 1;  // CLA 1
    IER |= M_INT11;
    PieCtrlRegs.PIEIER9.bit.INTx5 = 1;   // CAN A
    IER |= M_INT9;  // M_INT9
    PieCtrlRegs.PIEIER3.bit.INTx1 = 1;   // PWM 1
    IER |= M_INT3;
    EINT;
    EDIS;


    //-----------------------------------------------------------------------------------
    CANEnable(CANA_BASE);                                 // Enable the CAN for operation.
    CANGlobalIntEnable(CANA_BASE, CAN_GLB_INT_CANINT0);   // Enable CAN Global Interrupt line0

    sTXCANMessage.ui32MsgID = 1;                      // CAN message ID - use 1
    sTXCANMessage.ui32MsgIDMask = 1;                  // no mask needed for TX
    sTXCANMessage.ui32Flags = MSG_OBJ_TX_INT_ENABLE;  // enable interrupt on TX
    sTXCANMessage.ui32MsgLen = sizeof(ucTXMsgData);   // size of message is 8
    sTXCANMessage.pucMsgData = ucTXMsgData;           // ptr to message content

    *(unsigned long *)ucRXMsgData = 0;
    sRXCANMessage.ui32MsgID = 0;                      // CAN message ID - use 1
    sRXCANMessage.ui32MsgIDMask = 0x00;                 // no mask needed for TX
    sRXCANMessage.ui32Flags = MSG_OBJ_USE_ID_FILTER|MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    sRXCANMessage.ui32MsgLen = sizeof(ucRXMsgData);   // size of message is 8
    sRXCANMessage.pucMsgData = ucRXMsgData;           // ptr to message content

    // Setup the message object being used to receive messages
    CANMessageSet(CANA_BASE, 2, &sRXCANMessage, MSG_OBJ_TYPE_RX);

    //==================================================================================
    // Khoi tao ngoai vi

    GPIO_Init();

    VTimer0[0] = 0;
    VTimer1[0] = 0;
    VTimer2[0] = 0;

    /*****************************************************************************/
    // Cpu To CLA
    CpuToCLA.EnableADC = 0;
    DelayMs(1000);

    CpuToCLA.EnableADC = 1;
    CpuToCLA.EnableFlag = 0;
    CpuToCLA.UdcTesting = 400.0/Udc_max;
    CpuToCLA.UsTesting = 220.0/Us_max;
    CpuToCLA.VdTesting = 220.0/Us_max;
    CpuToCLA.VqTesting = 0;

    CpuToCLA.Fref = 50.00/BASE_FREQ;

    //------------------------------------------------------------------------------
    // SETTING PV
    Setting_PV.Voltage = PV_VOLTAGE;
    Setting_PV.Frequency = PV_FREQUENCY;
    Setting_PV.Power = PV_POWER;
    Setting_PV.Current = PV_CURRENT;
    Setting_PV.HeSo = PV_COSPHI;
    Setting_PV.Udc = PV_UDC;

    Setting_PV.FrequencyMax = PV_FREQUENCY_MAX;
    Setting_PV.FrequencyMin = PV_FREQUENCY_MIN;

    Setting_PV.VgridMax = PV_VGRID_MAX;
    Setting_PV.VgridMin = PV_VGRID_MIN;
    Setting_PV.IgridMax = PV_IGRID_MAX;

    Setting_PV.VLoadMax = PV_VLOAD_MAX;
    Setting_PV.VLoadMin = PV_VLOAD_MIN;
    Setting_PV.ILoadMax = PV_ILOAD_MAX;

    Setting_PV.UdcMax = PV_UDC_MAX;
    Setting_PV.UdcMin = PV_UDC_MIN;
    Setting_PV.TemperatureMax = PV_TEMP_MAX;


    //---------------------------------------------------------------------------------
    EALLOW;
    GpioCtrlRegs.GPBGMUX1.bit.GPIO32 = 0;   //
    GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 0;    // 0=GPIO,  1=Resv,  2=Resv,  3=COMP2OUT
    GpioCtrlRegs.GPBDIR.bit.GPIO32 = 1;     // 1=OUTput,  0=INput
    GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;      // Enable pull-up
    DO_LOW
    EDIS;
    ENABLE_PWM_OFF_MARCO;

    CpuToCLA.UdcTesting = 400.0/Udc_max;
    CpuToCLA.UsTesting = 235.0/Us_max;
    Setting_PV.Voltage = 235.0;



    #if(BUILDLEVEL == LEVEL1||BUILDLEVEL == LEVEL2||BUILDLEVEL == LEVEL3||BUILDLEVEL == LEVEL4||BUILDLEVEL == LEVEL7||BUILDLEVEL == LEVEL8)
    ENABLE_LOAD_ON_MARCO;
    ENABLE_GRID_OFF_MARCO;
    CHARGE_CAP_ON_MARCO;
    Setting_PV.VgridMin = 0;
    #endif

    #if(BUILDLEVEL == LEVEL5||BUILDLEVEL == LEVEL6)
    ENABLE_LOAD_OFF_MARCO;
    ENABLE_GRID_ON_MARCO;
    CHARGE_CAP_OFF_MARCO;
    Setting_PV.VLoadMin = 0;
    #endif

    ChargeCapFlag = 0;
    reset_charge = 0;

    while(1)
     {
        //====================================
    	// Reset IGBT after fault HCPL
        if(RESET_IGBT == 1)
        {
            if(ResetIGBTFlag == 0)
            {
               ResetIGBT();
               if(EPwm1Regs.TZFLG.bit.OST == 0x1)          // TripZ for PWMs is low (fault trip)
               {
                  EALLOW; // Clear flag TripZone (One Shot)
                  EPwm1Regs.TZCLR.bit.OST = 1;
                  EPwm2Regs.TZCLR.bit.OST = 1;
                  EPwm3Regs.TZCLR.bit.OST = 1;
                  EPwm4Regs.TZCLR.bit.OST = 1;
                  EPwm5Regs.TZCLR.bit.OST = 1;
                  EPwm6Regs.TZCLR.bit.OST = 1;
                  EDIS;
              }
            }
            ResetIGBTFlag = 1;
        }
        else
        {
            ResetIGBTFlag = 0;
        }
        if(clearTripFlagDMC == 1)
        {
        	TripFlagIGBT = 0;
        	OverCurrentMeasure = 0;
            if(EPwm1Regs.TZFLG.bit.OST == 0x1)          // TripZ for PWMs is low (fault trip)
            {
               EALLOW; // Clear flag TripZone (One Shot )
               EPwm1Regs.TZCLR.bit.OST = 1;
               EPwm2Regs.TZCLR.bit.OST = 1;
               EPwm3Regs.TZCLR.bit.OST = 1;
               EPwm4Regs.TZCLR.bit.OST = 1;
               EPwm5Regs.TZCLR.bit.OST = 1;
               EPwm6Regs.TZCLR.bit.OST = 1;
               EDIS;
           }

           if(EPwm1Regs.TZFLG.bit.DCAEVT1 == 0x01 ||EPwm1Regs.TZFLG.bit.DCBEVT1 == 0x01)
           {
        	  EALLOW;
       		  // clear DCAEVT1 flags
       		  EPwm1Regs.TZCLR.bit.DCAEVT1 = 1;
       		  EPwm2Regs.TZCLR.bit.DCAEVT1 = 1;
       		  EPwm3Regs.TZCLR.bit.DCAEVT1 = 1;
       		  EPwm4Regs.TZCLR.bit.DCAEVT1 = 1;
       		  EPwm5Regs.TZCLR.bit.DCAEVT1 = 1;
       		  EPwm6Regs.TZCLR.bit.DCAEVT1 = 1;

       		  // clear DCBEVT1 flags
       		  EPwm1Regs.TZCLR.bit.DCBEVT1 = 1;
       		  EPwm2Regs.TZCLR.bit.DCBEVT1 = 1;
       		  EPwm3Regs.TZCLR.bit.DCBEVT1 = 1;
       		  EPwm4Regs.TZCLR.bit.DCBEVT1 = 1;
       		  EPwm5Regs.TZCLR.bit.DCBEVT1 = 1;
       		  EPwm6Regs.TZCLR.bit.DCBEVT1 = 1;

       		  // clear HLATCH - (not in TRIP gen path)
       		  Cmpss3Regs.COMPSTSCLR.bit.HLATCHCLR = 1;

       		  // clear LLATCH - (not in TRIP gen path)
       		  Cmpss3Regs.COMPSTSCLR.bit.LLATCHCLR = 1;

       		  EDIS;
           }

        }
        //==========================================
         if(START == 1)
         {
            // LEVEL1
            #if(BUILDLEVEL == LEVEL1)
            CpuToCLA.UsTesting = ((float)Setting_PV.Voltage)/Us_max;
            CpuToCLA.EnableFlag = 1;
            #endif

            // LEVEL2
            #if(BUILDLEVEL == LEVEL2)
            if(Setting_PV.Voltage > PV_VOLTAGE) Setting_PV.Voltage = PV_VOLTAGE;
            CpuToCLA.UsTesting = ((float)Setting_PV.Voltage)/Us_max;
            CpuToCLA.EnableFlag = 1;
            #endif

            // LEVEL3
            #if(BUILDLEVEL == LEVEL3)
            if(Setting_PV.Current > PV_CURRENT) Setting_PV.Current = PV_CURRENT;
            CpuToCLA.IsRef = ((float)Setting_PV.Current)/Is_max;
            CpuToCLA.EnableFlag = 1;
            #endif

            // LEVEL4
            #if(BUILDLEVEL == LEVEL4)
            if(Setting_PV.Voltage > PV_VOLTAGE) Setting_PV.Voltage = PV_VOLTAGE;
            CpuToCLA.UsRef = ((float)Setting_PV.Voltage)/Us_max;
            CpuToCLA.EnableFlag = 1;
            #endif

            // LEVEL5
            #if(BUILDLEVEL == LEVEL5)
            if(Setting_PV.Current > PV_CURRENT) Setting_PV.Current = PV_CURRENT;
            CpuToCLA.IsRef = ((float)Setting_PV.Current)/Is_max;
            CpuToCLA.EnableFlag = 1;
            #endif

            // LEVEL6
            #if(BUILDLEVEL == LEVEL6)
            if(Setting_PV.Udc > PV_UDC) Setting_PV.Udc = PV_UDC;
            CpuToCLA.UdcRef = ((float)Setting_PV.Udc)/Udc_max;
			if(reset_charge == 0 )
			 {
				 CHARGE_CAP_ON_MARCO;
				 DelayMs(1000);
				 reset_charge = 1;
				 ChargeCapFlag = 1;
			 }
			StartFlag = 1;
            CpuToCLA.EnableFlag = 1;
            #endif
         }
         else
         {
        	 reset_charge = 0;
        	 StartFlag = 0;
             CpuToCLA.EnableFlag = 0;

         }

        // State machine entry & exit point
        //===========================================================
        (*Alpha_State_Ptr)();   // jump to an Alpha state (A0,B0,...)
        //===========================================================
     }



} //END MAIN CODE

//*****************************************************************************
// ISR  CLA
//*****************************************************************************
__interrupt void cla1Isr1 ()
{
    Task1_Isr++;
    static Uint16 i = 0;

    //------------------------------------------------------------------------
    // Protection when START
    if(START == 1)
    {
        if(EPwm1Regs.TZFLG.bit.OST == 0x1)         // TripZ for PWMs is low (fault trip)
        {
            TripFlagIGBT=1;
            START = 0;
            error = ERROR_FAULT_IGBT;
        }
        else TripFlagIGBT = 0;
        if(EPwm1Regs.TZFLG.bit.DCAEVT1 == 0x01 || EPwm1Regs.TZFLG.bit.DCBEVT1 == 0x01)
        {
        	OverCurrentMeasure = 1;
        	START = 0;
        	error = ERROR_OVER_CURRENT;
        	data = ClaToCPU.ADC_CPU.Iinv;
        }
        else OverCurrentMeasure = 0;

        // Igrid_MAX
        if(ClaToCPU.MEASUARE_CPU.IinvRms > Limit.IgridMax  || ClaToCPU.MEASUARE_CPU.ILRms > Limit.ILoadMax)          // OverCurrent
        {
            error = ERROR_CURRENT_HIGH;
        }

        // Vgrid_MAX
        if(ClaToCPU.MEASUARE_CPU.VgRms > Limit.VgridMax)         // OverVoltage
        {
            error = ERROR_VGRID_HIGH;
        }
        // Vgrid_Min
        if(ClaToCPU.MEASUARE_CPU.VgRms < Limit.VgridMin)
        {
            error = ERROR_VGRID_LOW;
        }

        // VLoad_MAX
        if(ClaToCPU.MEASUARE_CPU.VLRms > Limit.VLoadMax)         // OverVoltage
        {
            error = ERROR_VLOAD_HIGH;
        }
        // VLoad_Min
        if(ClaToCPU.MEASUARE_CPU.VLRms < Limit.VLoadMin)
        {
            error = ERROR_VLOAD_LOW;
        }

        // Udc_max
        if(ClaToCPU.ADC_CPU.Udc > Limit.UdcMax)           // LowDC
        {
            error = ERROR_VDC_HIGH;
        }
        // Udc_min
        if(ClaToCPU.ADC_CPU.Udc < Limit.UdcMin)           // OverUdc
        {
           error = ERROR_VDC_LOW;
        }

        // Udc_max
        if(ClaToCPU.ADC_CPU.Udc1 > Limit.UcMax)      // LowDC
        {
            error = ERROR_VDC_HIGH;
        }
        // Udc_min
        if(ClaToCPU.ADC_CPU.Udc1 < Limit.UcMin)       // OverUdc
        {
            error = ERROR_VDC_LOW;
        }

        // Udc_max
        if(ClaToCPU.ADC_CPU.Udc2 > Limit.UcMax)      // LowDC
        {
            error = ERROR_VDC_HIGH;
        }
        // Udc_min
        if(ClaToCPU.ADC_CPU.Udc2 < Limit.UcMin)       // OverUdc
        {
            error = ERROR_VDC_LOW;

        }


        if(ClaToCPU.ADC_CPU.Iinv > Limit.Ipeak || ClaToCPU.ADC_CPU.Iinv < -Limit.Ipeak)
        {
        	error = ERROR_PEAK_CURRENT;
        	data = ClaToCPU.ADC_CPU.Iinv;
        }

        if(ClaToCPU.MEASUARE_CPU.flag == 1)error = ERROR_CONTROLLER;

    }

    if(error != ERROR_NONE)
    {
        START = 0;
    }
    //------------------------------------------------------------------------
    // Protection when charge cap
    if(ChargeCapFlag == 0)
    {
        if(ClaToCPU.ADC_CPU.Iinv > 10.0/Is_max || ClaToCPU.ADC_CPU.Iinv < -10.0/Is_max)
        {
        	OverCurrentChargeCap = 1;
        	data = 2.0;
        }

		if(EPwm1Regs.TZFLG.bit.DCAEVT1 == 0x01 || EPwm1Regs.TZFLG.bit.DCBEVT1 == 0x01)
		{
			OverCurrentChargeCap = 1;
			data = 1.0;
		}
		if(OverCurrentChargeCap == 1)
		{
			ENABLE_GRID_OFF_MARCO;
		}
    }
    //----------------------------------------------------------------------
    // Datalog
    if(i == 200) i =0;
    switch(ChannelAdc)
    {
    	case 0:
    		Datalog1[i] = ClaToCPU.ADC_CPU.Udc;
    		Datalog2[i] = ClaToCPU.ADC_CPU.Tmp;
    		break;
        case 1:
            Datalog1[i] = ClaToCPU.ADC_CPU.Udc1;
            Datalog2[i] = ClaToCPU.ADC_CPU.Udc2;
            break;
        case 2:
            Datalog1[i] = ClaToCPU.ADC_CPU.VL;
            Datalog2[i] = ClaToCPU.ADC_CPU.Iinv;
            break;
        case 3:
            Datalog1[i] = ClaToCPU.ADC_CPU.VL;
            Datalog2[i] = ClaToCPU.ADC_CPU.IL;
            break;
        case 4:
            Datalog1[i] = ClaToCPU.ADC_CPU.Vg;
            Datalog2[i] = ClaToCPU.ADC_CPU.Iinv;
            break;
        case 5:
            Datalog1[i] = ClaToCPU.ADC_CPU.Vg;
            Datalog2[i] = ClaToCPU.MEASUARE_CPU.Theta;
            break;
        case 6:
            Datalog1[i] = ClaToCPU.MEASUARE_CPU.Ta1;
            Datalog2[i] = ClaToCPU.MEASUARE_CPU.Ta2;
            break;
        case 7:
            Datalog1[i] = ClaToCPU.MEASUARE_CPU.Tb1;
            Datalog2[i] = ClaToCPU.MEASUARE_CPU.Tb2;
            break;
        case 8:
            Datalog2[i] = ClaToCPU.ADC_CPU.Temp;
            break;
    }
    i++;
    if(i == 200) i =0;


    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared
    PieCtrlRegs.PIEACK.all = (PIEACK_GROUP1 | PIEACK_GROUP11);

    // Acknowledge the end-of-task interrupt for task 1
    PieCtrlRegs.PIEACK.all = M_INT11;//0400=0000 0100 0000 0000

}

__interrupt void cla1Isr2 ()
{
    asm(" ESTOP0");
}

__interrupt void cla1Isr3 ()
{
    asm(" ESTOP0");
}

__interrupt void cla1Isr4 ()
{
    asm(" ESTOP0");
}

__interrupt void cla1Isr5 ()
{
    asm(" ESTOP0");
}

__interrupt void cla1Isr6 ()
{
    asm(" ESTOP0");
}

__interrupt void cla1Isr7 ()
{
    asm(" ESTOP0");
}

__interrupt void cla1Isr8 ()
{
    Task8_Isr++;
    // Acknowledge the end-of-task interrupt for task 8
    PieCtrlRegs.PIEACK.all = M_INT11;

}


interrupt void CANIntHandler(void)
{
    // Read the CAN interrupt status to find the cause of the interrupt
    ulStatus = CANIntStatus(CANA_BASE, CAN_INT_STS_CAUSE);

    // If the cause is a controller status interrupt, then get the status
    if(ulStatus == CAN_INT_INT0ID_STATUS)
    {

        ulStatus = CANStatusGet(CANA_BASE, CAN_STS_CONTROL);

        if(((ulStatus  & ~(CAN_ES_TXOK | CAN_ES_RXOK)) != 7) &&
           ((ulStatus  & ~(CAN_ES_TXOK | CAN_ES_RXOK)) != 0))
        {
            g_bErrFlag = 1;
        }
    }

    // Ngat truyen CAN
    else if(ulStatus == 1)
    {

        CANIntClear(CANA_BASE, 1);
        g_ulTxMsgCount++;
        g_bErrFlag = 0;
    }

    // Xu ly du lieu CAN nhan duoc
    else if(ulStatus == 2)
    {
        CANMessageGet(CANA_BASE, 2, &sRXCANMessage, true);
        CANIntClear(CANA_BASE, 2);
        g_ulRxMsgCount++;
        g_bErrFlag = 0;
        ReciveCan(&sRXCANMessage, &RxMsgData);

        switch (RxMsgData.ID)
        {

        }
    }

    else
    {
        //
        // Spurious interrupt handling can go here.
        //
    }

    CANGlobalIntClear(CANA_BASE, CAN_GLB_INT_CANINT0);
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}

//===============================================================================
//=================================================================================
//  STATE-MACHINE SEQUENCING AND SYNCRONIZATION
//=================================================================================

//--------------------------------- FRAMEWORK -------------------------------------
void A0(void)
{
    // loop rate synchronizer for A-tasks
    if(CpuTimer0Regs.TCR.bit.TIF == 1)
    {
        CpuTimer0Regs.TCR.bit.TIF = 1;  // clear flag

        //-----------------------------------------------------------
        (*A_Task_Ptr)();        // jump to an A Task (A1,A2,A3,...)
        //-----------------------------------------------------------

        VTimer0[0]++;           // virtual timer 0, instance 0 (spare)
    }

    Alpha_State_Ptr = &B0;      // Comment out to allow only A tasks
}

void B0(void)
{
    // loop rate synchronizer for B-tasks
    if(CpuTimer1Regs.TCR.bit.TIF == 1)
    {
        CpuTimer1Regs.TCR.bit.TIF = 1;              // clear flag

        //-----------------------------------------------------------
        (*B_Task_Ptr)();        // jump to a B Task (B1,B2,B3,...)
        //-----------------------------------------------------------
        VTimer1[0]++;           // virtual timer 1, instance 0 (spare)
    }

    Alpha_State_Ptr = &C0;      // Allow C state tasks
}

void C0(void)
{
    // loop rate synchronizer for C-tasks
    if(CpuTimer2Regs.TCR.bit.TIF == 1)
    {
        CpuTimer2Regs.TCR.bit.TIF = 1;              // clear flag

        //-----------------------------------------------------------
        (*C_Task_Ptr)();        // jump to a C Task (C1,C2,C3,...)
        //-----------------------------------------------------------
        VTimer2[0]++;           //virtual timer 2, instance 0 (spare)
    }

    Alpha_State_Ptr = &A0;  // Back to State A0
}
//=================================================================================
//  A - TASKS
//=================================================================================
//--------------------------------------------------------
void STOP_STATE(void)  // Dash Board Measurements
//--------------------------------------------------------
{

}

//--------------------------------------------------------
void RUN_STATE(void)  // Panel Connect Disconnect
//-----------------------------------------------------------------
{


}

//--------------------------------------------------------
void CONFIG_STATE(void)  // Talk to the Panel Emulator
//-----------------------------------------
{


}

//--------------------------------------------------------
void CHECKING_STATE(void)  // Talk to the Panel Emulator
//-----------------------------------------
{

}

//=================================================================================
//  B - TASKS
//=================================================================================
//----------------------------------------
void SEND_CAN(void)  // MPPT Execution
//----------------------------------------
{

    //-----------------
    //the next time CpuTimer1 'counter' reaches Period value go to B2
    B_Task_Ptr = &SEND_CAN;
    //-----------------
}

// Setup OCP limits and digital filter parameters of CMPSS
void  CMPSS_DIG_FILTER(volatile struct CMPSS_REGS *v, Uint16 curHi, Uint16 curLo)
{
	// comparator references
	v->DACHVALS.bit.DACVAL = curHi;   // positive max current limit
	v->DACLVALS.bit.DACVAL = curLo;   // negative max current limit

	// digital filter settings - HIGH side
	v->CTRIPHFILCLKCTL.bit.CLKPRESCALE = clkPrescale;    // set time between samples, max : 1023
	v->CTRIPHFILCTL.bit.SAMPWIN        = sampwin;        // # of samples in window, max : 31
	v->CTRIPHFILCTL.bit.THRESH         = thresh;         // recommended : thresh > sampwin/2

	// digital filter settings - LOW side
	v->CTRIPLFILCLKCTL.bit.CLKPRESCALE = clkPrescale;    // Max count of 1023 */
	v->CTRIPLFILCTL.bit.SAMPWIN        = sampwin;        // # of samples in window, max : 31
	v->CTRIPLFILCTL.bit.THRESH         = thresh;         // recommended : thresh > sampwin/2

	return;
}

//=================================================================================
//  C - TASKS
//=================================================================================
//------------------------------------------------------
void C1(void)    // Spare
//------------------------------------------------------
{

	// *******************************************************
	// Current limit setting / tuning in Debug environment
	// *******************************************************
	EALLOW;
	LEM_curHi = 2048 + LEM(curLimit);
	LEM_curLo = 2048 - LEM(curLimit);

	CMPSS_DIG_FILTER(&Cmpss3Regs, LEM_curHi, LEM_curLo);      // LEM - W
	EDIS;

	Limit.VLoadMax = (float)Setting_PV.VLoadMax/Us_max;
	Limit.VLoadMin = (float)Setting_PV.VLoadMin/Us_max;

	Limit.VgridMax = (float)Setting_PV.VgridMax/Us_max;
	Limit.VgridMin = (float)Setting_PV.VgridMin/Us_max;

	Limit.UdcMax = (float)Setting_PV.UdcMax/Udc_max;
	Limit.UdcMin = (float)Setting_PV.UdcMin/Udc_max;

	Limit.UcMax = Limit.UdcMax/2.0;
	Limit.UcMin = Limit.UdcMin/2.0;

	Limit.ILoadMax = (float)Setting_PV.ILoadMax/Iload_max;

	Limit.IgridMax = (float)Setting_PV.IgridMax/Is_max;

	Limit.Ipeak = curPeakLimit/Is_max;

    LED_BLUE_TOGGLE_MARCO
    LED_RED_TOGGLE_MARCO
    //----------------
    //the next time CpuTimer2 'counter' reaches Period value go to C2
    C_Task_Ptr = &C1;
    //-----------------

}


void SendMsgCan(uint16_t ID, uint16_t data1,uint16_t data2 ,uint16_t data3,uint16_t data4)
{
     ucTXMsgData[0] =(uint8_t) ((data1 >> 8) & 0x00FF);
     ucTXMsgData[1] =(uint8_t) ( data1 & 0x00FF);
     ucTXMsgData[2] =(uint8_t) ((data2 >> 8) & 0x00FF);
     ucTXMsgData[3] =(uint8_t) (data2 & 0x00FF);
     ucTXMsgData[4] =(uint8_t) ((data3 >> 8) & 0x00FF);
     ucTXMsgData[5] =(uint8_t) (data3 & 0x00FF);
     ucTXMsgData[6] =(uint8_t) ((data4 >> 8) & 0x00FF);
     ucTXMsgData[7] =(uint8_t) (data4 & 0x00FF);

     sTXCANMessage.ui32MsgID = ID;                      // CAN message ID
     sTXCANMessage.pucMsgData = ucTXMsgData;
     CANMessageSet(CANA_BASE, 1, &sTXCANMessage, MSG_OBJ_TYPE_TX);
}

void ReciveCan(tCANMsgObject *pMsgObject, MsgDataCan *data)
{
    data->ID = pMsgObject->ui32MsgID;

    data->WORD_ONE = *(pMsgObject->pucMsgData);
    data->WORD_ONE <<= 8;
    data->WORD_ONE += *(pMsgObject->pucMsgData + 1);

    data->WORD_SEC = *(pMsgObject->pucMsgData + 2);
    data->WORD_SEC <<= 8;
    data->WORD_SEC += *(pMsgObject->pucMsgData + 3);

    data->WORD_THREE = *(pMsgObject->pucMsgData + 4);
    data->WORD_THREE <<= 8;
    data->WORD_THREE += *(pMsgObject->pucMsgData + 5);

    data->WORD_FOUR = *(pMsgObject->pucMsgData + 6);
    data->WORD_FOUR <<= 8;
    data->WORD_FOUR += *(pMsgObject->pucMsgData + 7);

}


void TripZone_Protection(void)
{
    // Configure Trip Mechanism for the Motor control software
    // -Cycle by cycle trip on CPU halt
    // -One shot IPM trip zone trip
    // These trips need to be repeated for EPWM1 ,2 & 3

    //===========================================================================
    //Motor Control Trip Config, EPwm1,2,3
    //===========================================================================
     EALLOW;
     // CPU Halt Trip

      EPwm1Regs.TZSEL.bit.OSHT1   = 1;  //enable TZ1 for OSHT
      EPwm2Regs.TZSEL.bit.OSHT1   = 1;  //enable TZ1 for OSHT
      EPwm3Regs.TZSEL.bit.OSHT1   = 1;  //enable TZ1 for OSHT
      EPwm4Regs.TZSEL.bit.OSHT1   = 1;  //enable TZ1 for OSHT
      EPwm5Regs.TZSEL.bit.OSHT1   = 1;  //enable TZ1 for OSHT
      EPwm6Regs.TZSEL.bit.OSHT1   = 1;  //enable TZ1 for OSHT


    // What do we want the OST/CBC events to do?
    // TZA events can force EPWMxA
    // TZB events can force EPWMxB

      //
      EPwm1Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // EPWMxA will go low
      EPwm1Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // EPWMxB will go low

      EPwm2Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // EPWMxA will go low
      EPwm2Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // EPWMxB will go low

      EPwm3Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // EPWMxA will go low
      EPwm3Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // EPWMxB will go low

      EPwm3Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // EPWMxA will go low
      EPwm3Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // EPWMxB will go low

      EPwm4Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // EPWMxA will go low
      EPwm4Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // EPWMxB will go low

      EPwm5Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // EPWMxA will go low
      EPwm5Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // EPWMxB will go low

      EPwm6Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // EPWMxA will go low
      EPwm6Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // EPWMxB will go low

      // Enable TZ interrupt
      // EPwm1Regs.TZEINT.bit.OST = 1;

      EDIS;

      // Clear any spurious OV trip
      EPwm1Regs.TZCLR.bit.OST = 1;
      EPwm2Regs.TZCLR.bit.OST = 1;
      EPwm3Regs.TZCLR.bit.OST = 1;
      EPwm4Regs.TZCLR.bit.OST = 1;
      EPwm5Regs.TZCLR.bit.OST = 1;
      EPwm6Regs.TZCLR.bit.OST = 1;
}

__interrupt void epwm_isr(void)
{
    Pwm_Isr++;
    // Clear INT flag for this timer
    EPwm1Regs.ETCLR.bit.INT = 1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}


void cmpssConfig(volatile struct CMPSS_REGS *v, int16 Hi, int16 Lo)
{
    /*
    //------------------------------------------------------
     CMPIN1P -> ADCINA2
     CMPIN1N -> ADCINA3
     CMPIN2P -> ADCINA4
     CMPIN2N -> ADCINA5
     CMPIN4P -> ADCIN14
     CMPIN4N -> ADCIN15

     CMPIN3P -> ADCINB2
     CMPIN3N -> ADCINB3

     CMPIN6P -> ADCINC2
     CMPIN6N -> ADCINC3
     CMPIN5P -> ADCINC4
     CMPIN5N -> ADCINC5

     CMPIN7P -> ADCIND0
     CMPIN7N -> ADCIND1
     CMPIN8P -> ADCIND2
     CMPIN8N -> ADCIND3

    */
	//------------------------------------------------------
	// Set up COMPCTL register
	EALLOW;
	v->COMPCTL.bit.COMPDACE    = 1;             // Enable CMPSS
	v->COMPCTL.bit.COMPLSOURCE = NEGIN_DAC;     // NEG signal from DAC for COMP-L
	v->COMPCTL.bit.COMPHSOURCE = NEGIN_DAC;     // NEG signal from DAC for COMP-H
	v->COMPCTL.bit.COMPHINV    = 0;             // COMP-H output is NOT inverted
	v->COMPCTL.bit.COMPLINV    = 1;             // COMP-L output is inverted
	v->COMPCTL.bit.ASYNCHEN    = 0;             // Disable aynch COMP-H ouput
	v->COMPCTL.bit.ASYNCLEN    = 0;             // Disable aynch COMP-L ouput
	v->COMPCTL.bit.CTRIPHSEL    = CTRIP_FILTER; // Dig filter output ==> CTRIPH
	v->COMPCTL.bit.CTRIPOUTHSEL = CTRIP_FILTER; // Dig filter output ==> CTRIPOUTH
	v->COMPCTL.bit.CTRIPLSEL    = CTRIP_FILTER; // Dig filter output ==> CTRIPL
	v->COMPCTL.bit.CTRIPOUTLSEL = CTRIP_FILTER; // Dig filter output ==> CTRIPOUTL

	// Set up COMPHYSCTL register
	v->COMPHYSCTL.bit.COMPHYS   = 2; // COMP hysteresis set to 2x typical value

	// set up COMPDACCTL register
	v->COMPDACCTL.bit.SELREF    = REFERENCE_VDDA_CMPSS; // VDDA is REF for CMPSS DACs
	v->COMPDACCTL.bit.SWLOADSEL = 0; // DAC updated on sysclock
	v->COMPDACCTL.bit.DACSOURCE = 0; // Ramp bypassed

	// Load DACs - High and Low
	v->DACHVALS.bit.DACVAL = Hi;     // Set DAC-H to allowed MAX +ve current
	v->DACLVALS.bit.DACVAL = Lo;     // Set DAC-L to allowed MAX -ve current

	// digital filter settings - HIGH side
	v->CTRIPHFILCLKCTL.bit.CLKPRESCALE = clkPrescale; // set time between samples, max : 1023
	v->CTRIPHFILCTL.bit.SAMPWIN        = sampwin;     // # of samples in window, max : 31
	v->CTRIPHFILCTL.bit.THRESH         = thresh;      // recommended : thresh > sampwin/2
	v->CTRIPHFILCTL.bit.FILINIT        = 1;           // Init samples to filter input value

	// digital filter settings - LOW side
	v->CTRIPLFILCLKCTL.bit.CLKPRESCALE = clkPrescale; // set time between samples, max : 1023
	v->CTRIPLFILCTL.bit.SAMPWIN        = sampwin;     // # of samples in window, max : 31
	v->CTRIPLFILCTL.bit.THRESH         = thresh;      // recommended : thresh > sampwin/2
	v->CTRIPLFILCTL.bit.FILINIT        = 1;           // Init samples to filter input value

	// Clear the status register for latched comparator events
	v->COMPSTSCLR.bit.HLATCHCLR = 1;
	v->COMPSTSCLR.bit.LLATCHCLR = 1;
	EDIS;
	return;
}

void CMPSS_Protection(void)
{

	// LEM Current (ADC B2, COMP3), High Low Compare event trips
	LEM_curHi = 2048 + LEM(curLimit);
	LEM_curLo = 2048 - LEM(curLimit);
	cmpssConfig(&Cmpss3Regs, LEM_curHi, LEM_curLo);  //Enable CMPSS3 - LEM CURRENT

	EALLOW;
	// Configure TRIP 4 to OR the High and Low trips from both comparator 1 & 3
	// Clear everything first
	EPwmXbarRegs.TRIP4MUX0TO15CFG.all  = 0x0000;
	EPwmXbarRegs.TRIP4MUX16TO31CFG.all = 0x0000;
	// Enable Muxes for ored input of CMPSS1H and 1L, i.e. .1 mux for Mux0
	EPwmXbarRegs.TRIP4MUX0TO15CFG.bit.MUX4  = 1;  //cmpss3 - tripH or tripL

	// Disable all the muxes first
	EPwmXbarRegs.TRIP4MUXENABLE.all = 0x0000;
	// Enable Mux 4 to generate TRIP4
	EPwmXbarRegs.TRIP4MUXENABLE.bit.MUX4  = 1;


    //
    // Configure CTRIPOUTH output pin
    //Configure OUTPUTXBAR3 to be CTRIPOUT1H
    //
    OutputXbarRegs.OUTPUT3MUX0TO15CFG.bit.MUX4 = 0;

    //
    //Enable OUTPUTXBAR3 Mux for Output
    //
    OutputXbarRegs.OUTPUT3MUXENABLE.bit.MUX4 = 1;


	EPwm1Regs.DCTRIPSEL.bit.DCAHCOMPSEL = 3; //Trip 4 is the input to the DCAHCOMPSEL
	EPwm1Regs.TZDCSEL.bit.DCAEVT1       = TZ_DCAH_HI;
	EPwm1Regs.DCACTL.bit.EVT1SRCSEL     = DC_EVT1;
	EPwm1Regs.DCACTL.bit.EVT1FRCSYNCSEL = DC_EVT_ASYNC;
	EPwm1Regs.TZSEL.bit.DCAEVT1         = 1;           // 1/0 - Enable/Disable One Shot Mode

	EPwm1Regs.DCTRIPSEL.bit.DCBHCOMPSEL = 3; //Trip 4 is the input to the DCBHCOMPSEL
	EPwm1Regs.TZDCSEL.bit.DCBEVT1       = TZ_DCBH_HI;
	EPwm1Regs.DCBCTL.bit.EVT1SRCSEL     = DC_EVT1;
	EPwm1Regs.DCBCTL.bit.EVT1FRCSYNCSEL = DC_EVT_ASYNC;
	EPwm1Regs.TZSEL.bit.DCBEVT1         = 1;           // 1/0 - Enable/Disable One Shot Mode

	EPwm2Regs.DCTRIPSEL.bit.DCAHCOMPSEL = 3; //Trip 4 is the input to the DCAHCOMPSEL
	EPwm2Regs.TZDCSEL.bit.DCAEVT1       = TZ_DCAH_HI;
	EPwm2Regs.DCACTL.bit.EVT1SRCSEL     = DC_EVT1;
	EPwm2Regs.DCACTL.bit.EVT1FRCSYNCSEL = DC_EVT_ASYNC;
	EPwm2Regs.TZSEL.bit.DCAEVT1         = 1;

	EPwm2Regs.DCTRIPSEL.bit.DCBHCOMPSEL = 3; //Trip 4 is the input to the DCBHCOMPSEL
	EPwm2Regs.TZDCSEL.bit.DCBEVT1       = TZ_DCBH_HI;
	EPwm2Regs.DCBCTL.bit.EVT1SRCSEL     = DC_EVT1;
	EPwm2Regs.DCBCTL.bit.EVT1FRCSYNCSEL = DC_EVT_ASYNC;
	EPwm2Regs.TZSEL.bit.DCBEVT1         = 1;           // 1/0 - Enable/Disable One Shot Mode

	EPwm3Regs.DCTRIPSEL.bit.DCAHCOMPSEL = 3; //Trip 4 is the input to the DCAHCOMPSEL
	EPwm3Regs.TZDCSEL.bit.DCAEVT1       = TZ_DCAH_HI;
	EPwm3Regs.DCACTL.bit.EVT1SRCSEL     = DC_EVT1;
	EPwm3Regs.DCACTL.bit.EVT1FRCSYNCSEL = DC_EVT_ASYNC;
	EPwm3Regs.TZSEL.bit.DCAEVT1         = 1;

	EPwm3Regs.DCTRIPSEL.bit.DCBHCOMPSEL = 3; //Trip 4 is the input to the DCBHCOMPSEL
	EPwm3Regs.TZDCSEL.bit.DCBEVT1       = TZ_DCBH_HI;
	EPwm3Regs.DCBCTL.bit.EVT1SRCSEL     = DC_EVT1;
	EPwm3Regs.DCBCTL.bit.EVT1FRCSYNCSEL = DC_EVT_ASYNC;
	EPwm3Regs.TZSEL.bit.DCBEVT1         = 1;           // 1/0 - Enable/Disable One Shot Mode

	EPwm4Regs.DCTRIPSEL.bit.DCAHCOMPSEL = 3; //Trip 4 is the input to the DCAHCOMPSEL
	EPwm4Regs.TZDCSEL.bit.DCAEVT1       = TZ_DCAH_HI;
	EPwm4Regs.DCACTL.bit.EVT1SRCSEL     = DC_EVT1;
	EPwm4Regs.DCACTL.bit.EVT1FRCSYNCSEL = DC_EVT_ASYNC;
	EPwm4Regs.TZSEL.bit.DCAEVT1         = 1;

	EPwm4Regs.DCTRIPSEL.bit.DCBHCOMPSEL = 3; //Trip 4 is the input to the DCBHCOMPSEL
	EPwm4Regs.TZDCSEL.bit.DCBEVT1       = TZ_DCBH_HI;
	EPwm4Regs.DCBCTL.bit.EVT1SRCSEL     = DC_EVT1;
	EPwm4Regs.DCBCTL.bit.EVT1FRCSYNCSEL = DC_EVT_ASYNC;
	EPwm4Regs.TZSEL.bit.DCBEVT1         = 1;           // 1/0 - Enable/Disable One Shot Mode

	EPwm5Regs.DCTRIPSEL.bit.DCAHCOMPSEL = 3; //Trip 4 is the input to the DCAHCOMPSEL
	EPwm5Regs.TZDCSEL.bit.DCAEVT1       = TZ_DCAH_HI;
	EPwm5Regs.DCACTL.bit.EVT1SRCSEL     = DC_EVT1;
	EPwm5Regs.DCACTL.bit.EVT1FRCSYNCSEL = DC_EVT_ASYNC;
	EPwm5Regs.TZSEL.bit.DCAEVT1         = 1;

	EPwm5Regs.DCTRIPSEL.bit.DCBHCOMPSEL = 3; //Trip 4 is the input to the DCBHCOMPSEL
	EPwm5Regs.TZDCSEL.bit.DCBEVT1       = TZ_DCBH_HI;
	EPwm5Regs.DCBCTL.bit.EVT1SRCSEL     = DC_EVT1;
	EPwm5Regs.DCBCTL.bit.EVT1FRCSYNCSEL = DC_EVT_ASYNC;
	EPwm5Regs.TZSEL.bit.DCBEVT1         = 1;           // 1/0 - Enable/Disable One Shot Mode

	EPwm6Regs.DCTRIPSEL.bit.DCAHCOMPSEL = 3; //Trip 4 is the input to the DCAHCOMPSEL
	EPwm6Regs.TZDCSEL.bit.DCAEVT1       = TZ_DCAH_HI;
	EPwm6Regs.DCACTL.bit.EVT1SRCSEL     = DC_EVT1;
	EPwm6Regs.DCACTL.bit.EVT1FRCSYNCSEL = DC_EVT_ASYNC;
	EPwm6Regs.TZSEL.bit.DCAEVT1         = 1;

	EPwm6Regs.DCTRIPSEL.bit.DCBHCOMPSEL = 3; //Trip 4 is the input to the DCBHCOMPSEL
	EPwm6Regs.TZDCSEL.bit.DCBEVT1       = TZ_DCBH_HI;
	EPwm6Regs.DCBCTL.bit.EVT1SRCSEL     = DC_EVT1;
	EPwm6Regs.DCBCTL.bit.EVT1FRCSYNCSEL = DC_EVT_ASYNC;
	EPwm6Regs.TZSEL.bit.DCBEVT1         = 1;           // 1/0 - Enable/Disable One Shot Mode

	// What do we want the DCAEVT1 events to do?
	// TZA events can force EPWMxA
	// TZB events can force EPWMxB

    EPwm1Regs.TZCTL.bit.DCAEVT1 = TZ_FORCE_LO; // EPWMxA will go low
    EPwm1Regs.TZCTL.bit.DCBEVT1 = TZ_FORCE_LO; // EPWMxB will go low

    EPwm2Regs.TZCTL.bit.DCAEVT1 = TZ_FORCE_LO; // EPWMxA will go low
    EPwm2Regs.TZCTL.bit.DCBEVT1 = TZ_FORCE_LO; // EPWMxB will go low

    EPwm3Regs.TZCTL.bit.DCAEVT1 = TZ_FORCE_LO; // EPWMxA will go low
    EPwm3Regs.TZCTL.bit.DCBEVT1 = TZ_FORCE_LO; // EPWMxB will go low

    EPwm3Regs.TZCTL.bit.DCAEVT1 = TZ_FORCE_LO; // EPWMxA will go low
    EPwm3Regs.TZCTL.bit.DCBEVT1 = TZ_FORCE_LO; // EPWMxB will go low

    EPwm4Regs.TZCTL.bit.DCAEVT1 = TZ_FORCE_LO; // EPWMxA will go low
    EPwm4Regs.TZCTL.bit.DCBEVT1 = TZ_FORCE_LO; // EPWMxB will go low

    EPwm5Regs.TZCTL.bit.DCAEVT1 = TZ_FORCE_LO; // EPWMxA will go low
    EPwm5Regs.TZCTL.bit.DCBEVT1 = TZ_FORCE_LO; // EPWMxB will go low

    EPwm6Regs.TZCTL.bit.DCAEVT1 = TZ_FORCE_LO; // EPWMxA will go low
    EPwm6Regs.TZCTL.bit.DCBEVT1 = TZ_FORCE_LO; // EPWMxB will go low


	// Clear any spurious OV trip
	EPwm1Regs.TZCLR.bit.DCAEVT1 = 1;
	EPwm2Regs.TZCLR.bit.DCAEVT1 = 1;
	EPwm3Regs.TZCLR.bit.DCAEVT1 = 1;
	EPwm4Regs.TZCLR.bit.DCAEVT1 = 1;
	EPwm5Regs.TZCLR.bit.DCAEVT1 = 1;
	EPwm6Regs.TZCLR.bit.DCAEVT1 = 1;

	// Clear any spurious OV trip
	EPwm1Regs.TZCLR.bit.DCBEVT1 = 1;
	EPwm2Regs.TZCLR.bit.DCBEVT1 = 1;
	EPwm3Regs.TZCLR.bit.DCBEVT1 = 1;
	EPwm4Regs.TZCLR.bit.DCBEVT1 = 1;
	EPwm5Regs.TZCLR.bit.DCBEVT1 = 1;
	EPwm6Regs.TZCLR.bit.DCBEVT1 = 1;

	EDIS;

}
