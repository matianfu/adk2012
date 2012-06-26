/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2011, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */


/**
  * \file
  *
  * Implementation WM8731 driver.
  *
  */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "board.h"

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Read data from WM8731 Register.
 *
 * \param pTwid   Pointer to twi driver structure
 * \param dwDevice  Twi slave address.
 * \param dwRegAddr Register address to read.
 * \return value in the given register.
 */
uint16_t WM8731_Read( Twid *pTwid, uint32_t dwDevice, uint32_t dwRegAddr )
{
    uint16_t uwBitsDataRegister;
    uint8_t pTdata[2]={0,0};

    pTwid->address = dwDevice;
    pTwid->iaddress = dwRegAddr;
    pTwid->isize = 0;
    
    TWID_Read(pTwid, pTdata, 2, 0);
    uwBitsDataRegister = (pTdata[0] << 8) | pTdata[1];
    return uwBitsDataRegister;
}

/**
 * \brief  Write data to WM8731 Register.
 *
 * \param pTwid   Pointer to twi driver structure
 * \param dwDevice  Twi slave address.
 * \param dwRegAddr Register address to read.
 * \param uwData    Data to write
 */
void WM8731_Write( Twid *pTwid, uint32_t dwDevice, uint32_t dwRegAddr, uint16_t uwData )
{
    uint8_t pTmpData[2];
    uint16_t uwTmp;
    uwTmp = ((dwRegAddr & 0x7f) << 9) | (uwData & 0x1ff);

    pTwid->address = dwDevice;
    pTwid->iaddress = dwRegAddr;
    pTwid->isize = 0;

    pTmpData[0] = (uwTmp & 0xff00) >> 8;
    pTmpData[1] = uwTmp & 0xff;
    TWID_Write(pTwid, pTmpData, 2, 0);
}

/**
 * \brief  Init WM8731 to DAC mode.
 *
 * \param pTwid   Pointer to twi driver structure
 * \param dwDevice  Twi slave address.
 * \return 0.
 */
uint8_t WM8731_DAC_Init( Twid *pTwid, uint32_t dwDevice )
{
    /* reset */
    WM8731_Write(pTwid, dwDevice, WM8731_REG_RESET, 0);

    /* analogue audio path control */
    WM8731_Write(pTwid, dwDevice, WM8731_REG_ANALOGUE_PATH_CTRL, 0x14);

    /* digital audio path control*/
    WM8731_Write(pTwid, dwDevice, WM8731_REG_DIGITAL_PATH_CTRL, 0x00);

    /* power down control */
    WM8731_Write(pTwid, dwDevice, WM8731_REG_PWDOWN_CTRL, 0x60);

    /* Active control*/
    WM8731_Write(pTwid, dwDevice, WM8731_REG_ACTIVE_CTRL, 0x01);

    return 0;
}

/**
 * \brief  Set WM8731 volume
 *
 * \param pTwid   Pointer to twi driver structure
 * \param dwDevice  Twi slave address.
 * \param uwValue Register value, valid value is between 0x30 to 0x7f
 * \return 0.
 */
uint8_t WM8731_VolumeSet( Twid *pTwid, uint32_t dwDevice, uint16_t uwValue )
{
    uint16_t uwRegValue;

    uwValue &= WM8731_LHPVOL_BITS;
    uwRegValue = WM8731_LRHPBOTH_BIT | WM8731_LZCEN_BIT | uwValue;
    WM8731_Write(pTwid, dwDevice, WM8731_REG_LEFT_HPOUT, uwRegValue);
    uwRegValue = WM8731_RZCEN_BIT | WM8731_RLHPBOTH_BIT | uwValue;
    WM8731_Write(pTwid, dwDevice, WM8731_REG_RIGHT_HPOUT, uwValue);
    return 0;
}

