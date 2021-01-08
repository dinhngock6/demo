/* =================================================================================
File name:        F2803XQEP.H                     
Target   : 		  F2803x family        
===================================================================================*/

#ifndef __F2837X_QEP_H__
#define __F2837X_QEP_H__

#include "f2837xbmsk.h"
#include "F2837xD_eqep.h"
#include "CLAmath.h"

/*-----------------------------------------------------------------------------
    Initialization states for EQEP Decode Control Register
------------------------------------------------------------------------------*/
#define QDECCTL_INIT_STATE     ( XCR_X2 + QSRC_QUAD_MODE )

/*-----------------------------------------------------------------------------
    Initialization states for EQEP Control Register
------------------------------------------------------------------------------*/
#define QEPCTL_INIT_STATE      ( QEP_EMULATION_FREE + 	\
                                 PCRM_POSMAX + 			\
                                 IEI_RISING + 			\
                                 IEL_RISING + 			\
                                 QPEN_ENABLE + 			\
                                 QCLM_TIME_OUT + 		\
                                 UTE_ENABLE )  

/*-----------------------------------------------------------------------------
    Initialization states for EQEP Position-Compare Control Register
------------------------------------------------------------------------------*/
#define QPOSCTL_INIT_STATE      PCE_DISABLE 

/*-----------------------------------------------------------------------------
    Initialization states for EQEP Capture Control Register
------------------------------------------------------------------------------*/
#define QCAPCTL_INIT_STATE     ( UPPS_X32 + 		\
                                 CCPS_X128 + 		\
                                 CEN_ENABLE )

/*-----------------------------------------------------------------------------
Define the structure of the QEP (Quadrature Encoder) Driver Object 
-----------------------------------------------------------------------------*/
typedef struct {float32 ElecTheta;        // Output: Motor Electrical angle (Q24)
                float32 MechTheta;        // Output: Motor Mechanical Angle (Q24)
                Uint16  DirectionQep;    // Output: Motor rotation direction (Q0)
                Uint16  QepPeriod;       // Output: Capture period of QEP signal in number of EQEP capture timer (QCTMR) period  (Q0)
                Uint32  QepCountIndex;   // Variable: Encoder counter index (Q0)
                int32   RawTheta;        // Variable: Raw angle from EQEP Postiion counter (Q0)
                float32 MechScaler;      // Parameter: 0.9999/total count (Q30)
                Uint16  NumberEncoderPulse;     // Parameter: Number of line encoder (Q0)
                Uint32  LineEncoder;     // Parameter: Number of line encoder (Q0)
                Uint16  PolePairs;       // Parameter: Number of pole pairs (Q0)
                int32   CalibratedAngle; // Parameter: Raw angular offset between encoder index and phase a (Q0)
                int32   Pre_CalibratedAngle;
                Uint16  IndexSyncFlag;   // Output: Index sync status (Q0)
                Uint16  Pre_MechTheta;
                float32 Theta_fbk;
                Uint16  circle;
                } QEP;


#define QEP_INIT(v)					\
    v.ElecTheta=0;					\
    v.MechTheta=0;					\
    v.DirectionQep=0;				\
    v.QepPeriod=0;					\
    v.QepCountIndex=0;				\
    v.RawTheta=0;					\
    v.MechScaler=0;					\
    v.LineEncoder=0;				\
    v.PolePairs=0;					\
    v.CalibratedAngle=0;			\
    v.Pre_CalibratedAngle=0;		\
    v.IndexSyncFlag=0;				\
    v.Theta_fbk = 0;				\
    v.circle = 0;
/*-----------------------------------------------------------------------------
	QEP Init and QEP Update Macro Definitions                                 
-----------------------------------------------------------------------------*/
#define QEP1_INIT_MACRO(v)																		\
																								\
	EQep1Regs.QDECCTL.all = QDECCTL_INIT_STATE;													\
	EQep1Regs.QEPCTL.all = QEPCTL_INIT_STATE;													\
	EQep1Regs.QPOSCTL.all = QPOSCTL_INIT_STATE;													\
	EQep1Regs.QUPRD = 600000;		        	    /* Unit Timer for 100Hz*/					\
	EQep1Regs.QCAPCTL.all = QCAPCTL_INIT_STATE;													\
	EQep1Regs.QPOSMAX = 4*v.LineEncoder;  //4*Number_encoder

#define QEP2_INIT_MACRO(v)																	    \
																								\
	EQep2Regs.QDECCTL.all = QDECCTL_INIT_STATE;													\
	EQep2Regs.QEPCTL.all = QEPCTL_INIT_STATE;													\
	EQep2Regs.QPOSCTL.all = QPOSCTL_INIT_STATE;													\
	EQep2Regs.QUPRD = 600000;		        	    /* Unit Timer for 100Hz*/					\
	EQep2Regs.QCAPCTL.all = QCAPCTL_INIT_STATE;													\
	EQep2Regs.QPOSMAX = 4*v.LineEncoder;  //4*Number_encoder
																								
//---------------------------------------------------------------------------------------------------------

#define QEP1_MACRO_CLA(v)																											\
																																	\
v.DirectionQep = EQep1Regs.QEPSTS.bit.QDF;																							\
																																	\
if (BUILDLEVEL == LEVEL4){ if(EQep1Regs.QPOSCNT > 0.995*EQep1Regs.QPOSMAX) EQep1Regs.QPOSCNT = 0;}         							\
	v.Theta_fbk = 1.0*EQep1Regs.QPOSCNT/EQep1Regs.QPOSMAX;                                             								\
	v.RawTheta = EQep1Regs.QPOSCNT - (v.NumberEncoderPulse*4.0)*((int)(EQep1Regs.QPOSCNT/(v.NumberEncoderPulse*4.0)));				\
	v.circle   = (int)(EQep1Regs.QPOSCNT/(v.NumberEncoderPulse*4.0));																\
/* Compute the mechanical angle */																									\
	v.MechTheta = v.MechScaler*v.RawTheta;																							\
/* Compute the electrical angle  */																									\
	v.ElecTheta = v.MechTheta*v.PolePairs - (int)(v.MechTheta*v.PolePairs); 																			\
/* Check an index occurrence*/																										\
if (EQep1Regs.QFLG.bit.IEL == 1)    																								\
{  																																	\
	v.IndexSyncFlag = 0x00F0;																										\
	v.QepCountIndex = EQep1Regs.QPOSILAT;																							\
	EQep1Regs.QCLR.bit.IEL = 1;	/* Clear interrupt flag */																			\
}																																	\
																																	\
/* Check unit Time out-event for speed calculation: */																				\
/* Unit Timer is configured for 100Hz in INIT function*/																			\
if(EQep1Regs.QFLG.bit.UTO == 1)															    										\
{																																	\
	/***** Low Speed Calculation   ****/																							\
if((EQep1Regs.QEPSTS.bit.COEF || EQep1Regs.QEPSTS.bit.CDEF))																		\
{	/* Capture Counter overflowed, hence do no compute speed*/																		\
		EQep1Regs.QEPSTS.all = 0x000C;																								\
}																																	\
else if(EQep1Regs.QCPRDLAT!=0xffff)														    										\
	/* Compute lowspeed using capture counter value*/																				\
	v.QepPeriod = EQep1Regs.QCPRDLAT;																								\
}

//============================================================================================

#define MotorPosCalc(v,start)                                                                     \
        if (start == 0)                                                                           \
        {                                                                                         \
            /* during alignment, assign the current shaft position as initial position*/          \
			EQep1Regs.QPOSCNT = 0;                                                                \
			EQep1Regs.QCLR.bit.IEL = 1;  /*Reset position cnt for QEP*/                           \
			EQep1Regs.QCLR.bit.PCU = 1;                                                           \
			EQep1Regs.QCLR.bit.PCO = 1;                                                           \
        }                                                                                         \
    /******************************************************************************/              \
                        																		  \
        /* for once the QEP index pulse is found, go to lsw=2*/                                   \
        if(start == 1)    /* doan if nay khong can thiet*/                                        \
        {     																					  \
        	/* Check the index occurrence*/                                                       \
            v.CalibratedAngle = EQep1Regs.QPOSCNT;                                                \
            /*if (EQep2Regs.QFLG.bit.IEL == 1) lsw=2;*/                                           \
            if(start==1) v.Pre_CalibratedAngle = v.CalibratedAngle;                               \
            /* Keep the latched pos. at the first index*/                                         \
        }                                                                                         \
        if (start !=0){                                                                           \
        	QEP1_MACRO_CLA(v)                                                                     \
        }


#define QEP2_MACRO_POSITION_CLA(v)																									\
																																	\
v.DirectionQep = EQep2Regs.QEPSTS.bit.QDF;																							\
																																	\
	v.Theta_fbk = 1.0*EQep2Regs.QPOSCNT/EQep2Regs.QPOSMAX;                                             								\
	v.RawTheta = EQep2Regs.QPOSCNT - (v.NumberEncoderPulse*4.0)*((int)(EQep2Regs.QPOSCNT/(v.NumberEncoderPulse*4.0)));				\
	v.circle   = (int)(EQep1Regs.QPOSCNT/(v.NumberEncoderPulse*4.0));																\
/* Compute the mechanical angle */																									\
	v.MechTheta = v.MechScaler*v.RawTheta;																							\
/* Compute the electrical angle  */																									\
	v.ElecTheta = v.MechTheta*v.PolePairs - (int)(v.MechTheta*v.PolePairs); 														\
/* Check an index occurrence*/																										\
if (EQep2Regs.QFLG.bit.IEL == 1)    																								\
{  																																	\
	v.IndexSyncFlag = 0x00F0;																										\
	v.QepCountIndex = EQep2Regs.QPOSILAT;																							\
	EQep2Regs.QCLR.bit.IEL = 0;	/* Clear interrupt flag */																			\
}																																	\
																																	\
/* Check unit Time out-event for speed calculation: */																				\
/* Unit Timer is configured for 100Hz in INIT function*/																			\
if(EQep2Regs.QFLG.bit.UTO == 1)															    										\
{																																	\
	/***** Low Speed Calculation   ****/																							\
if((EQep2Regs.QEPSTS.bit.COEF || EQep2Regs.QEPSTS.bit.CDEF))																		\
{	/* Capture Counter overflowed, hence do no compute speed*/																		\
		EQep2Regs.QEPSTS.all = 0x000C;																								\
}																																	\
else if(EQep2Regs.QCPRDLAT!=0xffff)														    										\
	/* Compute lowspeed using capture counter value*/																				\
	v.QepPeriod = EQep2Regs.QCPRDLAT;																								\
}



#define QEP2_MACRO_CLA(v)																		\
																								\
/* Check the rotational direction */															\
     v.DirectionQep = EQep2Regs.QEPSTS.bit.QDF;													\
																								\
/* Check the position counter for EQEP1 */														\
     v.RawTheta = EQep2Regs.QPOSCNT + v.CalibratedAngle;										\
     																							\
     if (v.RawTheta < 0)																		\
       v.RawTheta = v.RawTheta + EQep2Regs.QPOSMAX;												\
     else if (v.RawTheta > EQep2Regs.QPOSMAX)													\
       v.RawTheta = v.RawTheta - EQep2Regs.QPOSMAX;												\
       																							\
     v.MechTheta = v.MechScaler*v.RawTheta;  													\
																								\
     v.ElecTheta = v.PolePairs*v.MechTheta - (int)(v.PolePairs*v.MechTheta);            		\
																								\
/* Check an index occurrence*/																	\
     if (EQep2Regs.QFLG.bit.IEL == 1)    														\
     {  																						\
     	v.IndexSyncFlag = 0x00F0;																\
        v.QepCountIndex = EQep2Regs.QPOSILAT;													\
    	EQep2Regs.QCLR.bit.IEL = 1;	/* Clear interrupt flag */									\
     }																							\

// ================== Use the code sbippet below for low speed operation =======================\
																								\


#define QEP1_RESET()																	  \
	/* during alignment, assign the current shaft position as initial position*/          \
	EQep1Regs.QPOSCNT = 0;                                                                \
	EQep1Regs.QCLR.bit.IEL = 1;  /*Reset position cnt for QEP*/                           \
	EQep1Regs.QCLR.bit.PCU = 1;                                                           \
	EQep1Regs.QCLR.bit.PCO = 1;                                                           \

#define QEP2_RESET()																	  \
	/* during alignment, assign the current shaft position as initial position*/          \
	EQep2Regs.QPOSCNT = 0;                                                                \
	EQep2Regs.QCLR.bit.IEL = 1;  /*Reset position cnt for QEP*/                           \
	EQep2Regs.QCLR.bit.PCU = 1;                                                           \
	EQep2Regs.QCLR.bit.PCO = 1;                                                           \

#endif // __F2837X_QEP_H__




