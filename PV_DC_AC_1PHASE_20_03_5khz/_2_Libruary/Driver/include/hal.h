#ifndef TIDA_00796_CS_WORK_HAL_H_
#define TIDA_00796_CS_WORK_HAL_H_
/******************************************************************************
* TIDA-00796 Resolver Frontend based on PGA411-Q1
* Module: Hardware Abstraction Layer
* Author: Jiri Panacek, j-panacek@ti.com
*
* Copyright 2016 Texas Instruments Incorporated. All rights reserved.
******************************************************************************/

#include "F2837xD_device.h" /* registers definition */

/******************************************************************************
 * MACROS
 *****************************************************************************/

/* PGA411 GPIO macros */
#define    HAL_PGA_RESET_N_LOW      asm(" EALLOW") ;GpioDataRegs.GPDCLEAR.bit.GPIO105 = 1; asm(" EDIS")
#define    HAL_PGA_RESET_N_HIGH     asm(" EALLOW") ; GpioDataRegs.GPDSET.bit.GPIO105 = 1 ;asm(" EDIS")
#define    HAL_PGA_FAULTRES_LOW     asm(" EALLOW"); \
                                    GpioCtrlRegs.GPBDIR.bit.GPIO41 = 1; \
                                    GpioDataRegs.GPBCLEAR.bit.GPIO41 = 1; \
                                    asm(" EDIS")
#define    HAL_PGA_FAULTRES_HIZ     asm(" EALLOW"); \
                                    GpioCtrlRegs.GPBDIR.bit.GPIO41 = 0; \
                                    asm(" EDIS")

#define    HAL_OUTZ_STATE           GpioDataRegs.GPADAT.bit.GPIO1


#define HAL_DELAY_US(A)  DelayUs(A)


/******************************************************************************
 * ENUMS
 *****************************************************************************/

/******************************************************************************
 * TYPEDEFS
 *****************************************************************************/

/******************************************************************************
 * VARIABLES, CONSTANTS
 *****************************************************************************/

/******************************************************************************
 * SYSTEM INTERFACE FUNCTIONS PROTOTYPES
 *****************************************************************************/

/* SPI init */
void hal_InitSPI(void);

/* Exchanges 4 Bytes via SPI, blocks until finished, 32b architecture = no pointer */
Uint32 hal_Xmit4BSPI(Uint32 frame);

/* asm delay routine in DelayUs.asm */
extern void DelayUs(unsigned int);
extern void DelayMs(unsigned long ms);
/* pseudo assert */
void hal_assert(void);

#endif /* TIDA_00796_CS_WORK_HAL_H_ */
