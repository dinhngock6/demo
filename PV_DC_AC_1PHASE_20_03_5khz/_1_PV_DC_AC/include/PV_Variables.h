/*
 * AVC_Variables.h
 *
 *  Created on: 5 Mar 2017
 *      Author: dinhngock6
 */

#ifndef _PV_VARIABLES_H_
#define _PV_VARIABLES_H_

#include "Var.h"
//----------------------------------------------------------------------------------------
// CLA ---> CPU
typedef struct
{
    float Udc;
	float Udc1;
	float Udc2;
	float Vg;
	float Iinv;
	float VL;
	float IL;
	float Temp;
	float Tmp;
}ADC_VALUE;


typedef struct
{
    float VgRms;
    float IinvRms;
    float VLRms;
    float ILRms;
	float Ta1;
	float Ta2;
	float Tb1;
	float Tb2;
	float Theta;
	int32 flag;
}MEASUREMENT_VAULE;

typedef struct
{
	ADC_VALUE ADC_CPU;
	MEASUREMENT_VAULE MEASUARE_CPU;
}CLA_TO_CPU;

//----------------------------------------------------------------------------------------

typedef struct
{
	float UdcTesting;
	float UsTesting;

	float VdTesting;
	float VqTesting;

	float IdRef;
	float IqRef;
	float IsRef;

	float UdRef;
	float UqRef;
	float UsRef;

	float UdcRef;

	float Fref;
	short EnableFlag;
	short EnableADC;

}CPU_TO_CLA;

#endif /* _PV_VARIABLES_H_ */
