/*
 * Speed_Calculating.h
 *
 *  Created on: Apr 14, 2018
 *      Author: Nguyen Hien
 */

#ifndef SPEED_CALCULATING_H_
#define SPEED_CALCULATING_H_

typedef struct {
       float ElecTheta;       	// Input: Electrical angle (pu)
       Uint32 DirectionQep; 	// Variable: Direction of rotation (Q0) - independently with global Q
       float OldElecTheta;    	// History: Electrical angle at previous step (pu)
       float Speed;           	// Output: Speed in per-unit  (pu)
       Uint32 BaseRpm;      	// Parameter: Base speed in rpm (Q0) - independently with global Q
       float K1;            	// Parameter: Constant for differentiator (Q21) - independently with global Q
       float SpeedRpm;      	// Output : Speed in rpm  (Q0) - independently with global Q
       float Tmp;             	// Variable: Temp variable
       } SPEED_MEAS_QEP;    	// Data type created

#define SE_DIFF_MAX_LIMIT  	0.9
#define SE_DIFF_MIN_LIMIT  	0.1

#define SPEED_MEAS_QEP_INIT(v)  \
        v.ElecTheta     =0;     \
        v.OldElecTheta  =0;     \
        v.Speed         =0;     \
        v.SpeedRpm      =0;     \
        v.Tmp           =0;
/*------------------------------------------------------------------------------
 SPEED_FR Macro Definition
------------------------------------------------------------------------------*/


#define SPEED_FR_MACRO(v)                                           	\
/* Differentiator*/                                                 	\
   v.Tmp = v.ElecTheta - v.OldElecTheta;                           		\
   if (v.Tmp < -0.5)                                           			\
     v.Tmp = v.Tmp + 1.0;                                      			\
   else if (v.Tmp > 0.5)                                       			\
     v.Tmp = v.Tmp - 1.0;                                      			\
/*   we=tmp*(2pi/(Ts.Wemax)) */                               			\
    if (v.Tmp > 1) v.Tmp = 1;                                       	\
    else if (v.Tmp < -1) v.Tmp = -1;                                	\
    v.Speed = v.K1*v.Tmp;                                        		\
/* Update the electrical angle */                                   	\
    v.OldElecTheta = v.ElecTheta;                                  		\
/* Change motor speed from pu value to rpm value (GLOBAL_Q -> Q0)*/ 	\
    v.SpeedRpm = v.BaseRpm*v.Speed;


#define SPEED_FR_LIMIT_MACRO(v)                                         		\
/* Differentiator*/                                                 			\
   if((v.ElecTheta < SE_DIFF_MAX_LIMIT) & (v.ElecTheta > SE_DIFF_MIN_LIMIT))	\
   {																			\
	   v.Tmp = v.ElecTheta - v.OldElecTheta;                           			\
	   v.Tmp = v.K1*v.Tmp; 														\
   }																			\
   else v.Tmp = v.Speed;														\
   	v.Tmp=(((v.Tmp>1.0)?1.0:v.Tmp)<-1)?-1:v.Tmp;								\
   	v.Speed  = v.Tmp;															\
/* Update the electrical angle */                                   			\
    v.OldElecTheta = v.ElecTheta;                                  				\
/* Change motor speed from pu value to rpm value (GLOBAL_Q -> Q0)*/ 			\
    v.SpeedRpm = v.BaseRpm*v.Speed;

#endif /* 2_LIBRUARY_DRIVER_INCLUDE_SPEED_CALCULATING_H_ */
