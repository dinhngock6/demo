//----------------------------------------------------------------------------------
//	FILE:			encoder-integration.h
//
//	Description:	Encoder declarations
//
//	Version: 		1.0
//
//  Target:  		TMS320F28377D,
//
//----------------------------------------------------------------------------------
//  Copyright Texas Instruments © 2004-2015
//----------------------------------------------------------------------------------
//  Revision History:
//----------------------------------------------------------------------------------
//  Date	  | Description / Status
//----------------------------------------------------------------------------------
// 4 Nov 2015  - Encoder declarations
//----------------------------------------------------------------------------------


#include "endat.h"


/*-----------------------------------------------------------------------------
Define the structure of the Encoder Driver Object
-----------------------------------------------------------------------------*/
typedef struct {
                    float ElecTheta;       // Output: Motor Electrical angle (float)
                    float MechTheta;       // Output: Motor Mechanical Angle (float)
                    float RawTheta;        // Variable: Raw position data from resolver (float)
                    float Speed;           // Variable: Speed data from resolver (float)
                    float InitTheta;       // Parameter: Raw angular offset between encoder index and phase a (float)
                    float MechScaler;      // Parameter: 0.9999/total count (float)
                    float StepsPerTurn;    // Parameter: Number of discrete positions (float)
                    Uint16 PolePairs;       // Parameter: Number of pole pairs (Q0)

                }  ABS_ENCODER;

#define  ABS_ENCODER_INT(v) 		\
        v.ElecTheta = 0;      	\
        v.MechTheta = 0;     	\
        v.RawTheta = 0;     	\
        v.Speed = 0;         	\
        v.InitTheta = 0;       	\
        v.MechScaler = 0;     	\
        v.StepsPerTurn = 0;   	\
        v.PolePairs = 0;
