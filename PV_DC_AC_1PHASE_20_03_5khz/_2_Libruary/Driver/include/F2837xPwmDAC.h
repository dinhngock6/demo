/*
 * F28377xPwmDAC.h
 *
 *  Created on: 22 Feb 2017
 *      Author: dinhngock6
 */

#ifndef _F2837XPWMDAC_H_
#define _F2837XPWMDAC_H_

#include "F2837xbmsk.h"
#include "F2837xD_epwm.h"
/*----------------------------------------------------------------------------
Initialization constant for the F2803X Time-Base Control Registers for PWM Generation.
Sets up the timer to run free upon emulation suspend, count up-down mode
prescaler 1.
----------------------------------------------------------------------------*/
#define PWMDAC_INIT_STATE ( FREE_RUN_FLAG +          \
                            PRDLD_IMMEDIATE  +       \
                            TIMER_CNT_UPDN +         \
                            HSPCLKDIV_PRESCALE_X_1 + \
                            CLKDIV_PRESCALE_X_1  +   \
                            PHSDIR_CNT_DN    +       \
                            CNTLD_DISABLE )

/*----------------------------------------------------------------------------
Initialization constant for the F2803X Compare Control Register.
----------------------------------------------------------------------------*/
#define PWMDAC_CMPCTL_INIT_STATE ( LOADAMODE_ZRO +    \
                                   LOADBMODE_ZRO +    \
                                   SHDWAMODE_SHADOW + \
                                   SHDWBMODE_SHADOW )

/*----------------------------------------------------------------------------
Initialization constant for the F2803X Action Qualifier Output A Register.
----------------------------------------------------------------------------*/
#define PWMDAC_AQCTLA_INIT_STATE ( CAU_SET + CAD_CLEAR )
#define PWMDAC_AQCTLB_INIT_STATE ( CBU_SET + CBD_CLEAR )

/*----------------------------------------------------------------------------
Initialization constant for the F2803X Dead-Band Generator registers for PWM Generation.
Sets up the dead band for PWMDAC and sets up dead band values.
----------------------------------------------------------------------------*/
#define PWMDAC_DBCTL_INIT_STATE   BP_DISABLE

/*----------------------------------------------------------------------------
Initialization constant for the F2803X PWM Chopper Control register for PWM Generation.
----------------------------------------------------------------------------*/
#define  PWMDAC_PCCTL_INIT_STATE  CHPEN_DISABLE

/*----------------------------------------------------------------------------
Initialization constant for the F2803X Trip Zone Select Register
----------------------------------------------------------------------------*/
#define  PWMDAC_TZSEL_INIT_STATE  DISABLE_TZSEL

/*----------------------------------------------------------------------------
Initialization constant for the F2803X Trip Zone Control Register
----------------------------------------------------------------------------*/
#define  PWMDAC_TZCTL_INIT_STATE ( TZA_HI_Z + TZB_HI_Z + 		 \
                                   DCAEVT1_HI_Z + DCAEVT2_HI_Z + \
                                   DCBEVT1_HI_Z + DCBEVT2_HI_Z )


/*------------------------------------------------------------------------------
	Default Initializers for the F2803X PWMGEN Object
------------------------------------------------------------------------------*/
#define F2803X_FC_PWM_DAC  {  1000,	    \
							  500,      \
                              0x4000, 	\
                              0x4000, 	\
                             }


#define PWMDAC_DEFAULTS 	F2803X_FC_PWM_DAC
/*------------------------------------------------------------------------------
	PWM Init & PWM Update Macro Definitions
------------------------------------------------------------------------------*/


#define PWM_DAC_CNF(ch,v)														    \
	     /* Setup Sync*/															\
         (*ePWM[ch]).TBCTL.bit.SYNCOSEL = 0;       /* Pass through*/				\
		 																			\
         /* Allow each timer to be sync'ed*/										\
         (*ePWM[ch]).TBCTL.bit.PHSEN = 1;											\
         																			\
																					\
         /* Init Timer-Base Phase Register for EPWM1-EPWM3*/						\
         (*ePWM[ch]).TBPHS.bit.TBPHS = 0;											\
																					\
         /* Init Timer-Base Control Register for EPWM1-EPWM3*/						\
         (*ePWM[ch]).TBCTL.all = PWMDAC_INIT_STATE;									\
																					\
         /* Init Compare Control Register for EPWM1-EPWM3*/							\
         (*ePWM[ch]).CMPCTL.all = PWMDAC_CMPCTL_INIT_STATE;							\
																					\
         /* Init Action Qualifier Output A Register for EPWM1-EPWM3*/				\
         (*ePWM[ch]).AQCTLA.all = PWMDAC_AQCTLA_INIT_STATE;							\
         (*ePWM[ch]).AQCTLB.all = PWMDAC_AQCTLB_INIT_STATE;							\
																					\
         /* Init Dead-Band Generator Control Register for EPWM1-EPWM3*/				\
         (*ePWM[ch]).DBCTL.all = PWMDAC_DBCTL_INIT_STATE ;							\
 																					\
         /* Init PWM Chopper Control Register for EPWM1-EPWM3*/						\
         (*ePWM[ch]).PCCTL.all = PWMDAC_PCCTL_INIT_STATE;							\
          																			\
		/* Init Timer-Base Period Register for EPWM1-EPWM3*/						\
         (*ePWM[ch]).TBPRD = v;														\
         EALLOW;                       /* Enable EALLOW */							\
																					\
         /* Init Trip Zone Select Register*/										\
         (*ePWM[ch]).TZSEL.all = PWMDAC_TZSEL_INIT_STATE;							\
 																					\
 	     /* Init Trip Zone Control Register*/										\
    	 (*ePWM[ch]).TZCTL.all = PWMDAC_TZCTL_INIT_STATE;    						\
        																			\
         EDIS;                         /* Disable EALLOW*/

#define PWMDAC_MACRO(d1,d2,d3,d4,v)			       		\
		EPwm8Regs.CMPA.bit.CMPA =v-(Uint16)(d1*v)  ;    \
		EPwm8Regs.CMPB.bit.CMPB =v-(Uint16)(d2*v) ;     \
		EPwm7Regs.CMPA.bit.CMPA =v-(Uint16)(d3*v)  ;    \
		EPwm7Regs.CMPB.bit.CMPB =v-(Uint16)(d4*v) ;


#endif


