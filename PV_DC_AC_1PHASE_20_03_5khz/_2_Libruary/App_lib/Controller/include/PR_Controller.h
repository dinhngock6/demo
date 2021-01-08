/*
 * PR_Controller.h
 *
 *  Created on: 8 May 2017
 *      Author: dinhngock6
 */

#ifndef _PR_CONTROLLER_H_
#define _PR_CONTROLLER_H_


#include "CLAmath.h"

typedef struct
{
	float Kp;
	float Ki;
	float IN_MAX;
	float OUT_MAX;
	float w1;
    float Ts;
    float phi;
    float b0;
    float b1;
    float b2;
    float a0;
    float a1;
    float a2;
    float Ki_dsp;
    float Kp_dsp;
}PR_Parameter;

typedef struct
{
	float Up     ;
	float Ui_k     ;
	float Ui_k_1   ;
	float Ui_k_2   ;
	float Error_k  ;
	float Error_k_1;
	float Error_k_2;
	float OUT ;

}PR_Calculator;

//---------------------------------------------------------
// Khoi tao
//---------------------------------------------------------
#define PR_PARAMETER_INT(v)             \
		v.Kp = 0;						\
		v.Ki = 0;						\
		v.IN_MAX = 0;					\
		v.OUT_MAX = 0;					\
		v.w1 = 0;						\
		v.Ts = 0;						\
		v.phi = 0;						\
		v.b0 = 0;						\
		v.b1 = 0;						\
		v.b2 = 0;						\
		v.a0 = 0;						\
		v.a1 = 0;						\
		v.a2 = 0;						\
		v.Ki_dsp = 0;					\
		v.Kp_dsp = 0;

#define PR_CACULATOR_INT(v)                     \
		v.Up = 0    ;							\
		v.Ui_k = 0  ;							\
		v.Ui_k_1 = 0  ;							\
		v.Ui_k_2  = 0 ;							\
		v.Error_k  = 0;							\
		v.Error_k_1 = 0;						\
		v.Error_k_2 = 0;						\
		v.OUT = 0;

//----------------------------------------------------------------------------------
// MARCO
#define PR_CALCULATOR_PARAMETER_V1(v,h)                                                    	\
		v.Kp_dsp      = 2*v.Kp*v.IN_MAX/v.OUT_MAX;											\
		v.Ki_dsp      = v.Ki*v.Ts*v.IN_MAX/v.OUT_MAX;										\
		v.phi =  h*v.w1*2.0*v.Ts;                                                       	\
		v.b2  = -v.Ki_dsp*CLAcos(v.phi);                                           	        \
	    v.b1  = v.Ki_dsp*(CLAcos(v.phi) - h*v.w1*v.Ts*CLAsin(v.phi));						\
	    v.b0 = 0;																			\
		v.a2  = 1.0;                                                                    	\
		v.a1  = h*h*v.w1*v.w1*v.Ts*v.Ts - 2.0;                                          	\
		v.a0  = 1.0;


#define PR_CALCULATOR_PARAMETER_V2(v,h)                                                    	\
		v.Kp_dsp      = 2*v.Kp*v.IN_MAX/v.OUT_MAX;											\
		v.Ki_dsp      = v.Ki*v.Ts*v.IN_MAX/v.OUT_MAX;										\
		v.phi =  h*v.w1*2.0*v.Ts;                                                       	\
		v.b2  = 0;                                           	        					\
	    v.b1  = v.Ki_dsp*(CLAcos(v.phi) - h*v.w1*v.Ts*CLAsin(v.phi));						\
	    v.b0  = -v.Ki_dsp*CLAcos(v.phi);													\
		v.a2  = 1.0;                                                                    	\
		v.a1  = h*h*v.w1*v.w1*v.Ts*v.Ts - 2.0;                                          	\
		v.a0  = 1.0;


//-----------------------------------------------------------------------------------------------
#define PR_Calcalator_MACRO(v1,v2)                              							\
v1.Up = (v2.Kp_dsp*v1.Error_k);																\
v1.Ui_k = (v2.b2*v1.Error_k_2) + (v2.b1*v1.Error_k_1) + (v2.b0*v1.Error_k)- (v2.a2*v1.Ui_k_2) - (v2.a1*v1.Ui_k_1) ;		\
v1.OUT = v1.Up + v1.Ui_k;																	\
v1.Error_k_2 = v1.Error_k_1;																\
v1.Error_k_1 = v1.Error_k;																	\
v1.Ui_k_2 = v1.Ui_k_1;																		\
v1.Ui_k_1 = v1.Ui_k;																		\
if(v1.OUT > (0.95)) v1.OUT = (0.95);				    									\
if(v1.OUT < (-0.95)) v1.OUT = (-0.95);


#endif /* 2_LIBRUARY_INCLUDE_CONTROLLER_PR_CONTROLLER_H_ */
