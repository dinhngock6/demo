/*
 * F28377xPwm_1ph.h
 *
 *  Created on: 6 Dec 2017
 *      Author: dinhngock6
 */

#ifndef _F2837XPWM_1PH_H_
#define _F2837XPWM_1PH_H_

#include "F2837xD_EPwm_defines.h"

#define PWM_1ch_UpDwnCnt_CNF(n, period, db)								\
	EALLOW;																\
																		\
	(*ePWM[n]).TBCTL.bit.PRDLD = TB_IMMEDIATE; /* set Immediate load*/  \
	(*ePWM[n]).TBPRD = period ; /* PWM frequency = 1 / period */		\
	(*ePWM[n]).TBPHS.bit.TBPHS = 0;										\
	(*ePWM[n]).TBCTR = 0;												\
	(*ePWM[n]).TBCTL.bit.CTRMODE   = TB_COUNT_UPDOWN;					\
	(*ePWM[n]).TBCTL.bit.HSPCLKDIV = TB_DIV1;							\
	(*ePWM[n]).TBCTL.bit.CLKDIV    = TB_DIV1;							\
																		\
	(*ePWM[n]).TBCTL.bit.PHSEN    = TB_DISABLE;							\
	(*ePWM[n]).TBCTL.bit.SYNCOSEL = TB_CTR_ZERO; /* sync "down-stream"*/	\
																			\
	/* Counter Compare Submodule Registers */								\
	(*ePWM[n]).CMPA.bit.CMPA = 0; /* set duty 0% initially	*/				\
	(*ePWM[n]).CMPCTL.bit.SHDWAMODE = CC_SHADOW;							\
	(*ePWM[n]).CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;							\
																			\
	/* Action Qualifier SubModule Registers */								\
	(*ePWM[n]).AQCTLA.bit.CAU = AQ_CLEAR;									\
	(*ePWM[n]).AQCTLA.bit.CAD = AQ_SET;										\
																			\
	/* Active high complementary PWMs - Set up the deadband */ 				\
	(*ePWM[n]).DBCTL.bit.IN_MODE  = DBA_ALL;								\
	(*ePWM[n]).DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;							\
	(*ePWM[n]).DBCTL.bit.POLSEL   = DB_ACTV_HIC;							\
	(*ePWM[n]).DBRED.all = db;												\
	(*ePWM[n]).DBFED.all = db;												\
	EDIS;

#define PWM_1ch_AB_UpDwnCnt_CNF(n, period )								\
	EALLOW;																\
																		\
	(*ePWM[n]).TBCTL.bit.PRDLD = TB_IMMEDIATE; /* set Immediate load*/  \
	(*ePWM[n]).TBPRD = period ; /* PWM frequency = 1 / period */		\
	(*ePWM[n]).TBPHS.bit.TBPHS = 0;										\
	(*ePWM[n]).TBCTR = 0;												\
	(*ePWM[n]).TBCTL.bit.CTRMODE   = TB_COUNT_UPDOWN;					\
	(*ePWM[n]).TBCTL.bit.HSPCLKDIV = TB_DIV1;							\
	(*ePWM[n]).TBCTL.bit.CLKDIV    = TB_DIV1;							\
																		\
	(*ePWM[n]).TBCTL.bit.PHSEN    = TB_DISABLE;							\
	(*ePWM[n]).TBCTL.bit.SYNCOSEL = TB_CTR_ZERO; /* sync "down-stream"*/	\
																			\
	/* Counter Compare Submodule Registers */								\
	(*ePWM[n]).CMPA.bit.CMPA = 0; /* set duty 0% initially	*/				\
	(*ePWM[n]).CMPB.bit.CMPB = 0; /* set duty 0% initially	*/				\
	(*ePWM[n]).CMPCTL.bit.SHDWAMODE = CC_SHADOW;							\
	(*ePWM[n]).CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;							\
																			\
	/* Action Qualifier SubModule Registers */								\
	(*ePWM[n]).AQCTLA.bit.CAU = AQ_CLEAR;									\
	(*ePWM[n]).AQCTLA.bit.CAD = AQ_SET;										\
	(*ePWM[n]).AQCTLB.bit.CBU = AQ_CLEAR;									\
	(*ePWM[n]).AQCTLB.bit.CBD = AQ_SET;										\
																			\
	/* Active high complementary PWMs - Set up the deadband */ 				\
	(*ePWM[n]).DBCTL.bit.IN_MODE  = DBA_ALL;								\
	(*ePWM[n]).DBCTL.bit.OUT_MODE = DB_DISABLE;								\
	EDIS;


#define PWM_1ch_UpCnt_CNF(n,period)											\
	EALLOW;																	\
	(*ePWM[n]).TBCTL.bit.PRDLD = TB_IMMEDIATE; /* set Immediate load*/		\
	(*ePWM[n]).TBPRD = period-1; /* PWM frequency = 1 / period */			\
	(*ePWM[n]).TBPHS.bit.TBPHS = 0;											\
	(*ePWM[n]).TBCTR = 0;													\
	(*ePWM[n]).TBCTL.bit.CTRMODE   = TB_COUNT_UP;							\
	(*ePWM[n]).TBCTL.bit.HSPCLKDIV = TB_DIV1;								\
	(*ePWM[n]).TBCTL.bit.CLKDIV    = TB_DIV1;								\
																			\
	(*ePWM[n]).TBCTL.bit.PHSEN    = TB_DISABLE;								\
	(*ePWM[n]).TBCTL.bit.SYNCOSEL = TB_CTR_ZERO; /* sync "down-stream"*/	\
																			\
	/* Counter Compare Submodule Registers*/								\
	(*ePWM[n]).CMPA.bit.CMPA        = 0; /* set duty 0% initially*/			\
	(*ePWM[n]).CMPCTL.bit.SHDWAMODE = CC_SHADOW;							\
	(*ePWM[n]).CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;							\
																			\
	/* Action Qualifier SubModule Registers */								\
	(*ePWM[n]).AQCTLA.bit.CAU = AQ_CLEAR;									\
	(*ePWM[n]).AQCTLA.bit.ZRO = AQ_SET;										\
																			\
	/* Active high complementary PWMs - Set up the deadband*/				\
	(*ePWM[n]).DBCTL.bit.IN_MODE  = DBA_ALL;								\
	(*ePWM[n]).DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;							\
	(*ePWM[n]).DBCTL.bit.POLSEL   = DB_ACTV_HIC;							\
	(*ePWM[n]).DBRED = 0;													\
	(*ePWM[n]).DBFED = 0;													\
	EDIS;



#endif /* 2_LIBRUARY_DRIVER_INCLUDE_F28377XPWM_1PH_H_ */
