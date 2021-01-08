

#ifndef PWM_H_
#define PWM_H_


#include "F2837xbmsk.h"



/*----------------------------------------------------------------------------
Initialization constant for the F280X Time-Base Control Registers for PWM Generation.
Sets up the timer to run free upon emulation suspend, count up-down mode
prescaler 1.
----------------------------------------------------------------------------*/
#define PWM_INIT_STATE ( FREE_RUN_FLAG +          \
                         PRDLD_SHADOW  +       	  \
                         TIMER_CNT_UPDN +         \
                         HSPCLKDIV_PRESCALE_X_1 + \
                         CLKDIV_PRESCALE_X_1  +   \
                         PHSDIR_CNT_DN    +       \
                         CNTLD_DISABLE )
/* thu vien cua f2803: lam viec truc tiep voi thanh ghi(PRDLD), CTRMODE: up_down, HSPCLKDIV:/1,CLKDIV:/1,
 * 					PHSDIR_CNT: khi dong bo thi CNT tang, khong load du lieu tu TBPHS khi co xung dong bo (PHSEN) */

/*----------------------------------------------------------------------------
Initialization constant for the F280X Compare Control Register.
----------------------------------------------------------------------------*/
#define CMPCTL_INIT_STATE ( LOADAMODE_ZRO + \
                            LOADBMODE_ZRO + \
                            SHDWAMODE_SHADOW + \
                            SHDWBMODE_SHADOW )

/*----------------------------------------------------------------------------
Initialization constant for the F280X Action Qualifier Output A Register.
Out A CTR tang = cmpA, cho ra 1, xuong thi cho ra 0
----------------------------------------------------------------------------*/
 #define AQCTLA_INIT_STATE_MODE1 ( CAU_SET + CAD_CLEAR )

 #define AQCTLB_INIT_STATE_MODE1 ( CAU_SET + CAD_CLEAR)

 #define AQCTLA_INIT_STATE_MODE2 ( CAU_CLEAR + CAD_SET)

 #define AQCTLB_INIT_STATE_MODE2 ( CAU_CLEAR + CAD_SET)

/*----------------------------------------------------------------------------
Initialization constant for the F280X Dead-Band Generator registers for PWM Generation.
Sets up the dead band for PWM and sets up dead band values.
----------------------------------------------------------------------------*/
#define DBCTL_INIT_STATE  (BP_ENABLE + POLSEL_ACTIVE_HI_CMP  )

#define DBCNT_INIT_STATE   300  // 300 = 3us   // 100 counts = 1 usec (delay) * 100 count/usec (for TBCLK = SYSCLK/1)

/*----------------------------------------------------------------------------
Initialization constant for the F280X PWM Chopper Control register for PWM Generation.
----------------------------------------------------------------------------*/
#define  PCCTL_INIT_STATE  CHPEN_DISABLE

/*----------------------------------------------------------------------------
Initialization constant for the F280X Trip Zone Select Register
----------------------------------------------------------------------------*/
#define  TZSEL_INIT_STATE  DISABLE_TZSEL


/*----------------------------------------------------------------------------
 *
 */
#define HAL_AQ_SW_FORCE_PWM1A_LOW		EPwm1Regs.AQCSFRC.bit.CSFA = 0x01
#define HAL_AQ_SW_FORCE_PWM1A_HIGH 		EPwm1Regs.AQCSFRC.bit.CSFA = 0x02
#define HAL_AQ_SW_FORCE_PWM1B_LOW 		EPwm1Regs.AQCSFRC.bit.CSFB = 0x04
#define HAL_AQ_SW_FORCE_PWM1B_HIGH 		EPwm1Regs.AQCSFRC.bit.CSFB = 0x08
#define HAL_AQ_SW_FORCE_PWM1A_DISABLE 	EPwm1Regs.AQCSFRC.bit.CSFA = 0x03
#define HAL_AQ_SW_FORCE_PWM1B_DISABLE 	EPwm1Regs.AQCSFRC.bit.CSFA = 0x09

#define HAL_AQ_SW_FORCE_PWM2A_LOW		EPwm2Regs.AQCSFRC.bit.CSFA = 0x01
#define HAL_AQ_SW_FORCE_PWM2A_HIGH 		EPwm2Regs.AQCSFRC.bit.CSFA = 0x02
#define HAL_AQ_SW_FORCE_PWM2B_LOW 		EPwm2Regs.AQCSFRC.bit.CSFB = 0x04
#define HAL_AQ_SW_FORCE_PWM2B_HIGH 		EPwm2Regs.AQCSFRC.bit.CSFB = 0x08
#define HAL_AQ_SW_FORCE_PWM2A_DISABLE 	EPwm2Regs.AQCSFRC.bit.CSFA = 0x03
#define HAL_AQ_SW_FORCE_PWM2B_DISABLE 	EPwm2Regs.AQCSFRC.bit.CSFA = 0x09

#define HAL_AQ_SW_FORCE_PWM3A_LOW		EPwm3Regs.AQCSFRC.bit.CSFA = 0x01
#define HAL_AQ_SW_FORCE_PWM3A_HIGH 		EPwm3Regs.AQCSFRC.bit.CSFA = 0x02
#define HAL_AQ_SW_FORCE_PWM3B_LOW 		EPwm3Regs.AQCSFRC.bit.CSFB = 0x04
#define HAL_AQ_SW_FORCE_PWM3B_HIGH 		EPwm3Regs.AQCSFRC.bit.CSFB = 0x08
#define HAL_AQ_SW_FORCE_PWM3A_DISABLE 	EPwm3Regs.AQCSFRC.bit.CSFA = 0x03
#define HAL_AQ_SW_FORCE_PWM3B_DISABLE 	EPwm3Regs.AQCSFRC.bit.CSFA = 0x09

#define HAL_AQ_SW_FORCE_PWM4A_LOW		EPwm4Regs.AQCSFRC.bit.CSFA = 0x01
#define HAL_AQ_SW_FORCE_PWM4A_HIGH 		EPwm4Regs.AQCSFRC.bit.CSFA = 0x02
#define HAL_AQ_SW_FORCE_PWM4B_LOW 		EPwm4Regs.AQCSFRC.bit.CSFB = 0x04
#define HAL_AQ_SW_FORCE_PWM4B_HIGH 		EPwm4Regs.AQCSFRC.bit.CSFB = 0x08
#define HAL_AQ_SW_FORCE_PWM4A_DISABLE 	EPwm4Regs.AQCSFRC.bit.CSFA = 0x03
#define HAL_AQ_SW_FORCE_PWM4B_DISABLE 	EPwm4Regs.AQCSFRC.bit.CSFA = 0x09

#define HAL_AQ_SW_FORCE_PWM5A_LOW		EPwm5Regs.AQCSFRC.bit.CSFA = 0x01
#define HAL_AQ_SW_FORCE_PWM5A_HIGH 		EPwm5Regs.AQCSFRC.bit.CSFA = 0x02
#define HAL_AQ_SW_FORCE_PWM5B_LOW 		EPwm5Regs.AQCSFRC.bit.CSFB = 0x04
#define HAL_AQ_SW_FORCE_PWM5B_HIGH 		EPwm5Regs.AQCSFRC.bit.CSFB = 0x08
#define HAL_AQ_SW_FORCE_PWM5A_DISABLE 	EPwm5Regs.AQCSFRC.bit.CSFA = 0x03
#define HAL_AQ_SW_FORCE_PWM5B_DISABLE 	EPwm5Regs.AQCSFRC.bit.CSFA = 0x09

#define HAL_AQ_SW_FORCE_PWM6A_LOW		EPwm6Regs.AQCSFRC.bit.CSFA = 0x01
#define HAL_AQ_SW_FORCE_PWM6A_HIGH 		EPwm6Regs.AQCSFRC.bit.CSFA = 0x02
#define HAL_AQ_SW_FORCE_PWM6B_LOW 		EPwm6Regs.AQCSFRC.bit.CSFB = 0x04
#define HAL_AQ_SW_FORCE_PWM6B_HIGH 		EPwm6Regs.AQCSFRC.bit.CSFB = 0x08
#define HAL_AQ_SW_FORCE_PWM6A_DISABLE 	EPwm6Regs.AQCSFRC.bit.CSFA = 0x03
#define HAL_AQ_SW_FORCE_PWM6B_DISABLE 	EPwm6Regs.AQCSFRC.bit.CSFA = 0x09
/*-----------------------------------------------------------------------------
	Define the structure of the PWM Driver Object
-----------------------------------------------------------------------------*/
typedef struct {
		unsigned int PeriodMax;     // Parameter: PWM Half-Period in CPU clock cycles (Q0)
        float MfuncA1;        // Input: EPWM1 A&B Duty cycle ratio (Q15)
        float MfuncA2;        // Input: EPWM2 A&B Duty cycle ratio (Q15)
        float MfuncB1;        // Input: EPWM3 A&B Duty cycle ratio (Q15)
        float MfuncB2;        // Input: EPWM1 A&B Duty cycle ratio (Q15)
        float MfuncC1;        // Input: EPWM2 A&B Duty cycle ratio (Q15)
        float MfuncC2;        // Input: EPWM3 A&B Duty cycle ratio (Q15)
        } PWMGEN ;

/*-----------------------------------------------------------------------------
	Define a PWMGEN_handle
-----------------------------------------------------------------------------*/
typedef PWMGEN *PWMGEN_handle;

/*------------------------------------------------------------------------------
	Default Initializers for the F280X PWMGEN Object
	bo di 1 cai
------------------------------------------------------------------------------*/
#define F2837X_FC_PWM_GEN    { 10000,  \
                              0.0, \
                              0.0, \
                              0.0, \
                              0.0, \
                              0.0, \
                              0.0, \
                             }


#define PWMGEN_DEFAULTS 	F2837X_FC_PWM_GEN
/*------------------------------------------------------------------------------
	PWM Init & PWM Update Macro Definitions
------------------------------------------------------------------------------*/

#define PWM_INIT_MACRO(v)		        									\
	     /* Setup Sync*/													\
		 EALLOW;															\
         EPwm1Regs.TBCTL.bit.SYNCOSEL = 1;       /* Pass through*/			\
         EPwm2Regs.TBCTL.bit.SYNCOSEL = 0;       /* Pass through*/			\
         EPwm3Regs.TBCTL.bit.SYNCOSEL = 0;       /* Pass through*/			\
         EPwm4Regs.TBCTL.bit.SYNCOSEL = 0;       /* Pass through*/			\
         EPwm5Regs.TBCTL.bit.SYNCOSEL = 0;       /* Pass through*/			\
         EPwm6Regs.TBCTL.bit.SYNCOSEL = 0;       /* Pass through*/			\
        																	\
		 EPwm1Regs.TBPRD = v.PeriodMax;										\
         EPwm2Regs.TBPRD = v.PeriodMax;										\
         	 	 	 	 	 	 	 	 	 	 	 	 	 	 			\
         EPwm3Regs.TBPRD = v.PeriodMax;										\
         EPwm4Regs.TBPRD = v.PeriodMax;										\
         	 	 	 	 	 	 	 	 	 	 	 	 	 	 			\
         EPwm5Regs.TBPRD = v.PeriodMax;										\
         EPwm6Regs.TBPRD = v.PeriodMax;										\
         	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	\
		 /* Init Timer-Base Phase Register for EPWM1-EPWM3*/				\
         EPwm1Regs.TBPHS.bit.TBPHS = 0;									    \
         EPwm2Regs.TBPHS.bit.TBPHS = 0;										\
         EPwm3Regs.TBPHS.bit.TBPHS = 0;									    \
         EPwm4Regs.TBPHS.bit.TBPHS = 0;										\
         EPwm5Regs.TBPHS.bit.TBPHS = 0;									    \
         EPwm6Regs.TBPHS.bit.TBPHS = 0;										\
																			\
		 EPwm1Regs.TBCTR = 0;												\
		 EPwm2Regs.TBCTR = 0;												\
		 EPwm3Regs.TBCTR = 0;												\
		 EPwm4Regs.TBCTR = 0;												\
		 EPwm5Regs.TBCTR = 0;												\
		 EPwm6Regs.TBCTR = 0;												\
		 																	\
         /* Allow each timer to be sync'ed*/								\
         EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;							\
         EPwm2Regs.TBCTL.bit.PHSEN = TB_ENABLE;								\
         EPwm3Regs.TBCTL.bit.PHSEN = TB_ENABLE;								\
         EPwm4Regs.TBCTL.bit.PHSEN = TB_ENABLE;								\
         EPwm5Regs.TBCTL.bit.PHSEN = TB_ENABLE;								\
         EPwm6Regs.TBCTL.bit.PHSEN = TB_ENABLE;								\
         	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	\
																			\
																			\
         /* Init Timer-Base Control Register for EPWM1-EPWM3*/				\
         EPwm1Regs.TBCTL.all = PWM_INIT_STATE;								\
         EPwm2Regs.TBCTL.all = PWM_INIT_STATE;								\
         EPwm3Regs.TBCTL.all = PWM_INIT_STATE;								\
         EPwm4Regs.TBCTL.all = PWM_INIT_STATE;								\
         EPwm5Regs.TBCTL.all = PWM_INIT_STATE;								\
         EPwm6Regs.TBCTL.all = PWM_INIT_STATE;								\
																			\
         /* Init Compare Control Register for EPWM1-EPWM3*/					\
         EPwm1Regs.CMPCTL.all = CMPCTL_INIT_STATE;							\
         EPwm2Regs.CMPCTL.all = CMPCTL_INIT_STATE;							\
         EPwm3Regs.CMPCTL.all = CMPCTL_INIT_STATE;							\
         EPwm4Regs.CMPCTL.all = CMPCTL_INIT_STATE;							\
         EPwm5Regs.CMPCTL.all = CMPCTL_INIT_STATE;							\
         EPwm6Regs.CMPCTL.all = CMPCTL_INIT_STATE;							\
         	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	\
	     EPwm1Regs.CMPA.bit.CMPA = v.PeriodMax;  							\
	     EPwm2Regs.CMPA.bit.CMPA = v.PeriodMax;  							\
		 EPwm3Regs.CMPA.bit.CMPA = v.PeriodMax;  							\
		 EPwm4Regs.CMPA.bit.CMPA = v.PeriodMax;  							\
		 EPwm5Regs.CMPA.bit.CMPA = v.PeriodMax;  							\
		 EPwm6Regs.CMPA.bit.CMPA = v.PeriodMax;  							\
																			\
		 EPwm1Regs.AQCTL.all = CMPCTL_INIT_STATE;							\
		 EPwm2Regs.AQCTL.all = CMPCTL_INIT_STATE;							\
		 EPwm3Regs.AQCTL.all = CMPCTL_INIT_STATE;							\
		 EPwm4Regs.AQCTL.all = CMPCTL_INIT_STATE;							\
		 EPwm5Regs.AQCTL.all = CMPCTL_INIT_STATE;							\
		 EPwm6Regs.AQCTL.all = CMPCTL_INIT_STATE; 							\
																			\
         /* Init Action Qualifier Output A Register for EPWM1-EPWM3*/		\
         EPwm1Regs.AQCTLA.all = AQCTLA_INIT_STATE_MODE1;					\
         EPwm2Regs.AQCTLA.all = AQCTLA_INIT_STATE_MODE1;					\
         EPwm3Regs.AQCTLA.all = AQCTLA_INIT_STATE_MODE1;					\
         EPwm4Regs.AQCTLA.all = AQCTLA_INIT_STATE_MODE1;					\
         EPwm5Regs.AQCTLA.all = AQCTLA_INIT_STATE_MODE1;					\
         EPwm6Regs.AQCTLA.all = AQCTLA_INIT_STATE_MODE1;					\
         	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	\
         EPwm1Regs.AQCTLB.all = AQCTLB_INIT_STATE_MODE1;					\
         EPwm2Regs.AQCTLB.all = AQCTLB_INIT_STATE_MODE1;					\
         EPwm3Regs.AQCTLB.all = AQCTLB_INIT_STATE_MODE1;					\
         EPwm4Regs.AQCTLB.all = AQCTLB_INIT_STATE_MODE1;					\
         EPwm5Regs.AQCTLB.all = AQCTLB_INIT_STATE_MODE1;					\
         EPwm6Regs.AQCTLB.all = AQCTLB_INIT_STATE_MODE1;					\
         	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	\
         	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	\
         /* Init Dead-Band Generator Control Register for EPWM1-EPWM3*/		\
         EPwm1Regs.DBCTL.all = DBCTL_INIT_STATE;							\
         EPwm2Regs.DBCTL.all = DBCTL_INIT_STATE;							\
         EPwm3Regs.DBCTL.all = DBCTL_INIT_STATE;							\
         EPwm4Regs.DBCTL.all = DBCTL_INIT_STATE;							\
         EPwm5Regs.DBCTL.all = DBCTL_INIT_STATE;							\
         EPwm6Regs.DBCTL.all = DBCTL_INIT_STATE;							\
																			\
         /* Init Dead-Band Generator for EPWM1-EPWM3*/						\
         EPwm1Regs.DBFED.all = DBCNT_INIT_STATE;							\
         EPwm1Regs.DBRED.all = DBCNT_INIT_STATE;							\
         	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 		\
         EPwm2Regs.DBFED.all = DBCNT_INIT_STATE;							\
         EPwm2Regs.DBRED.all = DBCNT_INIT_STATE;							\
         	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 		\
         EPwm3Regs.DBFED.all = DBCNT_INIT_STATE;							\
         EPwm3Regs.DBRED.all = DBCNT_INIT_STATE;							\
         	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 		\
         EPwm4Regs.DBFED.all = DBCNT_INIT_STATE;							\
         EPwm4Regs.DBRED.all = DBCNT_INIT_STATE;							\
         	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 		\
         EPwm5Regs.DBFED.all = DBCNT_INIT_STATE;							\
         EPwm5Regs.DBRED.all = DBCNT_INIT_STATE;							\
         	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 		\
         EPwm6Regs.DBFED.all = DBCNT_INIT_STATE;							\
         EPwm6Regs.DBRED.all = DBCNT_INIT_STATE;							\
         	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 		\
		  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  		\
		 EPwm1Regs.ETSEL.bit.SOCAEN   = 1;  											\
         EPwm1Regs.ETPS.bit.SOCAPRD = 2;             /* Generate pulse on 2 event*/		\
         EPwm1Regs.ETSEL.bit.SOCASEL = 2;            /* CTR = PRD*/					    \
		 EPwm1Regs.ETCLR.bit.SOCA = 1;  												\
		 EPwm1Regs.ETPS.bit.SOCACNT = 0x02 ;         /* Generate INT on 2 event */ 		\
         EDIS;


#define PWM_MACRO(v)								          	\
 EPwm1Regs.CMPA.bit.CMPA =(Uint16)(v.MfuncA1*v.PeriodMax);  	\
 EPwm2Regs.CMPA.bit.CMPA =(Uint16)(v.MfuncA2*v.PeriodMax);   	\
 EPwm3Regs.CMPA.bit.CMPA =(Uint16)(v.MfuncB1*v.PeriodMax);	  	\
 EPwm4Regs.CMPA.bit.CMPA =(Uint16)(v.MfuncB2*v.PeriodMax);  	\
 EPwm5Regs.CMPA.bit.CMPA =(Uint16)(v.MfuncC1*v.PeriodMax);   	\
 EPwm6Regs.CMPA.bit.CMPA =(Uint16)(v.MfuncC2*v.PeriodMax);



#endif /* PWM_H_ */
