/******************************************************************************
* TIDA-00796 Resolver Frontend based on PGA411-Q1
* Module: PGA411 driver
* Author: Jiri Panacek, j-panacek@ti.com
*
* Copyright 2016 Texas Instruments Incorporated. All rights reserved.
******************************************************************************/

#include "pga411.h"
#include "hal.h"
#include "F2837xD_device.h"

/******************************************************************************
 * MACROS
 *****************************************************************************/

#define    CRC_BITCOUNT     24 /* top 24 bits of Uint32 for CRC6 */
#define    CRC_BYTECOUNT    3 /* top 3 B of Uint32 for CRC6 */
#define    CRC_INITSEED     0x3F /* initial seed for CRC6 */
#define    CRC_POLYNOM      0x5B /* CRC = X6+X4+X3+X+1 */

#define    SPI_DUMMY        0xF0 /* dummy SPI data for read operation */

#define    EMPTY_LOOP

/******************************************************************************
 * ENUMS
 *****************************************************************************/

/******************************************************************************
 * TYPEDEFS
 *****************************************************************************/

/******************************************************************************
 * VARIABLES, CONSTANTS
 *****************************************************************************/

volatile pga411_spi_frame_t pga411_spi_frame = {.addr = 0x00,
                                                .dataout = 0x0000,
                                                .reserved = 0x0,
                                                .mcrc = 0x00
                                               };

pga411_reg_t pga411_regs[PGA411_REG_COUNT + 1] =
{
    /* memory constants init */
    [DEV_OVUV1]     = {.read_add = 0x53, .write_add = 0x87, .def_val = 0x8B40},  // 0x8B40
    [DEV_OVUV2]     = {.read_add = 0x6B, .write_add = 0x26, .def_val = 0x00ED},
    [DEV_OVUV3]     = {.read_add = 0x65, .write_add = 0x17, .def_val = 0xFFFF},  // FCFF
    [DEV_OVUV5]     = {.read_add = 0x52, .write_add = 0x75, .def_val = 0x1C00},
    [DEV_OVUV6]     = {.read_add = 0xE9, .write_add = 0x83, .def_val = 0x038F},
    [DEV_TLOOP_CFG] = {.read_add = 0xA6, .write_add = 0x42, .def_val = 0x0514},
    [DEV_AFE_CFG]   = {.read_add = 0xC2, .write_add = 0x91, .def_val = 0x0005},
    [DEV_PHASE_CFG] = {.read_add = 0x57, .write_add = 0x85, .def_val = 0x1400},   //1400 ( 4-Vrms) // 1800 (7-Vrms)
    [DEV_CONFIG1]   = {.read_add = 0xBE, .write_add = 0xEB, .def_val = 0x0002}, // 12V; 0x0002
    [DEV_CONTROL1]  = {.read_add = 0x90, .write_add = 0x0D, .def_val = 0x0000},
    [DEV_CONTROL2]  = {.read_add = 0x63, .write_add = 0x38, .def_val = 0x0000},
    [DEV_CONTROL3]  = {.read_add = 0xDD, .write_add = 0xAE, .def_val = 0x0003},
    [DEV_STAT1]     = {.read_add = 0x81, .write_add = 0x00, .def_val = 0x0000},
    [DEV_STAT2]     = {.read_add = 0x4D, .write_add = 0x00, .def_val = 0x0000},
    [DEV_STAT3]     = {.read_add = 0x84, .write_add = 0x00, .def_val = 0x0000},
    [DEV_STAT4]     = {.read_add = 0x1F, .write_add = 0x00, .def_val = 0x0000},
    [DEV_STAT5]     = {.read_add = 0x41, .write_add = 0x00, .def_val = 0x0000},
    [DEV_STAT6]     = {.read_add = 0x6F, .write_add = 0x00, .def_val = 0x0000},
    [DEV_STAT7]     = {.read_add = 0xE1, .write_add = 0x00, .def_val = 0x0000},
    [DEV_CLCRC]     = {.read_add = 0x4F, .write_add = 0xFC, .def_val = 0x0000},
    [DEV_CRC]       = {.read_add = 0x0F, .write_add = 0xE7, .def_val = 0x0000},
    [CRCCALC]       = {.read_add = 0xD9, .write_add = 0xFF, .def_val = 0x0000},
    [DEV_EE_CTRL1]  = {.read_add = 0xE3, .write_add = 0x6E, .def_val = 0x0000},
    [DEV_CRC_CTRL1] = {.read_add = 0x7A, .write_add = 0xB6, .def_val = 0x0000},
    [DEV_EE_CTRL4]  = {.read_add = 0xBA, .write_add = 0x56, .def_val = 0x0000},
    [DEV_UNLK_CTRL1] = {.read_add = 0x64, .write_add = 0x95, .def_val = 0x0000},
    /* for continuous read we need to read 28 times. This significantly helps */
    [DUMMY_REG]     = {.read_add = 0x53, .write_add = 0x87, .def_val = 0x0000},
};

/******************************************************************************
 * INTERNAL FUNCTIONS PROTOTYPES
 *****************************************************************************/

/* calculate CRC6 for output data, function takes first 3B left aligned data */
Uint16 pga411_crc2(Uint32 datin);

/* Xmit data to PGA over SPI, for reg use defined macros, return incoming frame */
pga411_spi_frame_t pga411_XmitSPI(Uint16 dir, Uint16 reg, Uint16 wdata);

/******************************************************************************
 * INTERNAL FUNCTIONS
 *****************************************************************************/

/* CRC6 calculation - optimized for PGA411 */
Uint16 pga411_crc2(Uint32 datin)
{
    Uint16 byte_idx, bit_idx, crc = (CRC_INITSEED << 2);

    /* byte by byte starting from most significant (3-2-1) */
    for (byte_idx = CRC_BYTECOUNT; byte_idx >= 1; byte_idx--)
    {
        /* XOR-in new byte from left to right */
        crc ^= ((datin >> (byte_idx << 3)) & 0x000000FF);

        /* bit by bit for each byte */
        for (bit_idx = 0; bit_idx < 8; bit_idx++)
        {
            crc = crc << 1 ^ (crc & 0x80 ? (CRC_POLYNOM << 2) : 0);
        }
    }

    return (crc >> 2 & CRC_INITSEED); /*restore two bit offset */
}

/******************************************************************************
 * SYSTEM INTERFACE FUNCTIONS
 *****************************************************************************/

/* Initialization PGA411-Q1 related hardware */
void pga411_Init(void)
{
    hal_InitSPI(); /* init SPI periphery */
    pga411_Reset(); /* perform PGA411 reset */
}

/* Perform PGA411-Q1 reset using GPIO */
void pga411_Reset(void)
{
    HAL_PGA_RESET_N_LOW; /* RESET_N low */
    HAL_DELAY_US(500); /* hold in reset for 500us */
    HAL_PGA_RESET_N_HIGH; /* release from reset */
    HAL_DELAY_US(10000); /* wait at least 10 ms*/
}

/* Perform PGA411-Q1 FAULT reset using GPIO */
void pga411_FaultReset(void)
{
    HAL_PGA_FAULTRES_LOW; /* assert FAULTRES low - open collector */
    HAL_DELAY_US(1000); /* hold in the state for 1ms */
    HAL_PGA_FAULTRES_HIZ; /* return back to high-z*/
}
/* Xmit data to PGA over SPI, for reg use defined macros */
pga411_spi_frame_t pga411_XmitSPI(Uint16 dir, Uint16 reg, Uint16 wdata)
{
    pga411_spi_frame_t out, in;

    /* do we read data ? */
    if (dir == READ) { out.addr = pga411_regs[reg].read_add; } /* read address */
    /* or write data ? */
    else { out.addr = pga411_regs[reg].write_add; } /* write address */

    /* compose the rest of the frame */
    out.dataout = wdata; /* real data (W) or dummy data (R) */
    out.reserved = 0x00; /* always zero */
    out.mcrc = pga411_crc2(out.frame); /* calculate TX CRC */
    in.frame = hal_Xmit4BSPI(out.frame);

    /* check RX CRC */
    if (pga411_crc2(in.frame) != in.scrc)
    {
        hal_assert(); /* if error -> terminate */
    }

    return (in);
}

/* Read data from defined register */
Uint16 pga411_ReadReg(Uint16 reg)
{
    /* first read returns whatever */
    pga411_XmitSPI(READ, reg, SPI_DUMMY);
    /* second read returns requested data */
    return (pga411_XmitSPI(READ, reg, SPI_DUMMY).datain);
}

/* Write data to defined register */
void pga411_WriteReg(Uint16 reg, Uint16 data)
{
    /* here we just making it nice, can be macro too */
    pga411_XmitSPI(WRITE, reg, data);
}

/* Read all registers from PGA */
void pga411_ReadAll(void)
{
    int i;
    Uint16 rx_data;

    /* register by register (27+1) */
    /* PGA411 always returns previous read register value */
    /* thats why 28 reads are needed (first read returns whatever data) */
    for (i = 0; i < PGA411_REG_COUNT + 1; i++)
    {
        rx_data = pga411_XmitSPI(READ, i, SPI_DUMMY).datain;

        /*boolean trick - save data if not first cycle */
        if (i)
        {
            pga411_regs[i - 1].real_val = rx_data;
        }
    }
}

/* Change state diagnostic/normal */
void pga411_State(Uint16 state)
{
    Uint16 temp;

    /* Enter Diagnostic state */
    if (state == DIAG)
    {
        /* read content of  DEV_CONTROL3 register */
        temp = pga411_ReadReg(DEV_CONTROL3);
        temp |= 0x0004; /* set bit SPIDIAG */
        pga411_WriteReg(DEV_CONTROL3, temp); /* finish R-M-W */
    }
    /* Go back to normal state */
    else
    {
        /* read content of  DEV_CONTROL1 register */
        temp = pga411_ReadReg(DEV_CONTROL1);
        temp |= 0x0001; /* set bit DIAGEXIT */
        pga411_WriteReg(DEV_CONTROL1, temp); /* finish R-M-W */
    }
}

/* Device unlock (must be in diagnostic state) */
void pga411_DeviceUnlock(void)
{
    pga411_WriteReg(DEV_UNLK_CTRL1, 0x000F);
    pga411_WriteReg(DEV_UNLK_CTRL1, 0x0055);
    pga411_WriteReg(DEV_UNLK_CTRL1, 0x00AA);
    pga411_WriteReg(DEV_UNLK_CTRL1, 0x00F0);
}

/* EEPROM unlock (must be in diagnostic state) */
void pga411_EEPROMUnlock(void)
{
    pga411_WriteReg(DEV_EE_CTRL4, 0x000F);
    pga411_WriteReg(DEV_EE_CTRL4, 0x0055);
    pga411_WriteReg(DEV_EE_CTRL4, 0x00AA);
    pga411_WriteReg(DEV_EE_CTRL4, 0x00F0);
}

/* Write default configuration to PGA */
void pga411_DefaultCfg(void)
{
    int i;
    /* go to diagnostic state */
    pga411_State(DIAG);
    /* unlock registers */
    pga411_DeviceUnlock();
 //   pga411_regs[DEV_CONFIG1].def_val = 0x0006;
    /* first 12 registers to default */
    /* note: for some u need to unlocked them first ! */
    for (i = 0; i < 12; i++)
    {
        pga411_XmitSPI(WRITE, i, pga411_regs[i].def_val);
    }

    /* go to normal state */
    pga411_State(NORMAL);
}
