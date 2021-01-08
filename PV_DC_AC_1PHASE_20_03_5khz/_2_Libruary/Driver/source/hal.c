/******************************************************************************
* TIDA-00796 Resolver Frontend based on PGA411-Q1
* Module: Hardware Abstraction Layer
* Author: Jiri Panacek, j-panacek@ti.com
*
* Copyright 2016 Texas Instruments Incorporated. All rights reserved.
******************************************************************************/

#include "hal.h"
#include "F2837xD_device.h"

/******************************************************************************
 * MACROS
 *****************************************************************************/

#define    EMPTY_LOOP
#define    ENDLESS    1

/* pointer to ADC and CLK calibration values */
#define Device_cal (void   (*)(void))0x3D7C80

/* PUSH data to SPI buffer - left adjusted */
#define SPIA_PUSH(TX) SpiaRegs.SPITXBUF = (TX << 8)
/* POP data from SPI buffer - right adjusted */
#define SPIA_POP SpiaRegs.SPIRXBUF


/* Watchdog timer disable function */
void hal_DisableWDT(void);

/******************************************************************************
 * INTERNAL FUNCTIONS
 *****************************************************************************/

/******************************************************************************
 * SYSTEM INTERFACE FUNCTIONS
 *****************************************************************************/

/* SPI init */
void hal_InitSPI(void)
{
    asm(" EALLOW"); /* Enable EALLOW protected register access */
    /* enable peripheral clock */
    CpuSysRegs.PCLKCR8.bit.SPI_A = 1; 			/* SPI-A */
    /* init GPIOs */
    GpioCtrlRegs.GPBPUD.bit.GPIO58 = 0; /* Enable pull-up on GPIO58 (SPISIMOA) */
    GpioCtrlRegs.GPBPUD.bit.GPIO59 = 0; /* Enable pull-up on GPIO59 (SPISOMIA) */
    GpioCtrlRegs.GPBPUD.bit.GPIO60 = 0; /* Enable pull-up on GPIO60 (SPICLKA) */
    GpioCtrlRegs.GPBPUD.bit.GPIO61 = 0; /* Enable pull-up on GPIO61 (SPISTEA) */
    GpioCtrlRegs.GPBQSEL2.bit.GPIO58 = 3; /* Asynch input GPIO58 (SPISIMOA) */
    GpioCtrlRegs.GPBQSEL2.bit.GPIO59 = 3; /* Asynch input GPIO59 (SPISOMIA) */
    GpioCtrlRegs.GPBQSEL2.bit.GPIO60 = 3; /* Asynch input GPIO60 (SPICLKA) */
    GpioCtrlRegs.GPBQSEL2.bit.GPIO61 = 3; /* Asynch input GPIO61 (SPISTEA) */

    GpioCtrlRegs.GPBGMUX2.bit.GPIO58 = 3;
    GpioCtrlRegs.GPBGMUX2.bit.GPIO59 = 3;
    GpioCtrlRegs.GPBGMUX2.bit.GPIO60 = 3;
    GpioCtrlRegs.GPBGMUX2.bit.GPIO61 = 3;

    GpioCtrlRegs.GPBMUX2.bit.GPIO58 = 3;
    GpioCtrlRegs.GPBMUX2.bit.GPIO59 = 3;
    GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 3;
    GpioCtrlRegs.GPBMUX2.bit.GPIO61 = 3;

    /* Initialize SPI periphery */
    SpiaRegs.SPICCR.bit.SPISWRESET = 0; 	/* Reset SPI and hold */
    SpiaRegs.SPICCR.all = 0x0007; 			/* Loopback off  8-bit character, Clock Polarity 0 */
    SpiaRegs.SPICTL.all = 0x0006; 			/* master, xmit enable */
    SpiaRegs.SPIBRR.all = 24; 				/* SYSCLK = 200Mhz = LSPCLK, baud rate = LSPCLK/(SPIBRR+1) = 8 MHz */
    SpiaRegs.SPISTS.all = 0x0000;  			/* reset all status information */
    SpiaRegs.SPIPRI.bit.FREE = 1; 			/* priority control settings - free run */
    SpiaRegs.SPICCR.bit.SPISWRESET = 1; 	/* Release from SPI reset */
    /* reset both fifos */
    SpiaRegs.SPIFFTX.bit.TXFIFO = 0;
    SpiaRegs.SPIFFRX.bit.RXFIFORESET = 0; 	/*and keep them in reset during configuration */
    /* configure fifos  */
    SpiaRegs.SPIFFTX.all = 0xC004; 			/* Enable FIFO's, do not use interrupts */
    SpiaRegs.SPIFFRX.all = 0x0004; 			/* Set RX FIFO, do not use interrupts  */
    SpiaRegs.SPIFFCT.all = 0x0000; 			/* no delay between FIFO and TXBUF */
    /* releasing fifos from reset */
    SpiaRegs.SPIFFTX.bit.TXFIFO = 1;
    SpiaRegs.SPIFFRX.bit.RXFIFORESET = 1; /* OK */

    /* error checking, fifo pointers should be zero */
    if ((SpiaRegs.SPIFFTX.bit.TXFFST != 0) || (SpiaRegs.SPIFFRX.bit.RXFFST != 0)) { hal_assert(); }

    asm(" EDIS"); /* Disable EALLOW protected register access */
}

/* Exchanges 4 Bytes via SPI, blocks until finished, 32b architecture = no pointer */
Uint32 hal_Xmit4BSPI(Uint32 frame)
{
    Uint32 rxval = 0x00000000; /* incoming data */
    /* empty RX and TX 4B FIFOs */
    SpiaRegs.SPIFFTX.bit.TXFIFO = 0; /* reset FIFOs */
    SpiaRegs.SPIFFRX.bit.RXFIFORESET = 0;
    asm(" NOP"); /* probably not needed */
    asm(" NOP"); /* probably not needed */
    SpiaRegs.SPIFFTX.bit.TXFIFO = 1;
    SpiaRegs.SPIFFRX.bit.RXFIFORESET = 1; /* release from reset */
    /* quick & dirty ! */
    SPIA_PUSH(frame >> 24); /* first byte (from most significant) */
    SPIA_PUSH(frame >> 16);
    SPIA_PUSH(frame >> 8);
    SPIA_PUSH(frame); /* last byte */

    /* wait until the transmission is finished */
    while (SpiaRegs.SPIFFRX.bit.RXFFST < 4) {EMPTY_LOOP};

    /* get data from RX FIFO */
    rxval = SPIA_POP; /* first byte (from the most significant) */

    rxval = ((rxval << 8) | SPIA_POP);

    rxval = ((rxval << 8) | SPIA_POP);

    rxval = ((rxval << 8) | SPIA_POP); /* last byte */

    /* NCS must remain high at least 200ns */
    HAL_DELAY_US(1);

    return (rxval);
}

/* pseudo assert */
void hal_assert(void)
{
    __asm("        ESTOP0"); /* stops emulation at this point */

    while (ENDLESS); /* just in case */
}

