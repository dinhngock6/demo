/*
 * define_main.h
 *
 *  Created on: 09-09-2018
 *      Author: ngocnd
 */

#ifndef _DEFINE_MAIN_H_
#define _DEFINE_MAIN_H_


#ifndef YES
#define YES		1
#endif

#ifndef NO
#define NO			0
#endif

#ifndef HIGH
#define HIGH		1
#endif

#ifndef LOW
#define LOW			0
#endif

#ifndef TRUE
#define TRUE		1
#endif

#ifndef FALSE
#define FALSE		0
#endif

#ifndef ON
#define ON			1
#endif

#ifndef OFF
#define OFF			0
#endif

//=========================================
#define ERROR_NONE    		0
#define ERROR_VDC_HIGH		1
#define ERROR_VDC_LOW		2
#define ERROR_VGRID_HIGH    3
#define ERROR_VGRID_LOW     4
#define ERROR_VLOAD_HIGH    5
#define ERROR_VLOAD_LOW     6
#define ERROR_CURRENT_HIGH	7
#define ERROR_TEMP_HIGH		8
#define ERROR_FAULT_IGBT	9
#define ERROR_CONTROLLER    10
#define ERROR_OVER_CURRENT	11
#define ERROR_PEAK_CURRENT	12
//==========================================

#define ST_RUN		1
#define ST_STOP 	2
#define ST_JOG 		3
#define ST_CONFIG 	4
#define ST_CHECKING 5

#define FAULT_MES	0x01
#define FAULT_IGBT	0x02
#define FAULT_PHASE	0x03



#endif /* _DEFINE_MAIN_H_ */
