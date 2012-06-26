/* ----------------------------------------------------------------------------
 *     ATMEL Microcontroller Software Support
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
 * Implementation of draw font and string on LCD.
 *
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "board.h"

#include <stdint.h>
#include <assert.h>

/*----------------------------------------------------------------------------
 *        Local variables
 *----------------------------------------------------------------------------*/

/** Global variable describing the font being instancied. */
const Font gFont = {10, 14};

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Draws an ASCII character on LCD.
 *
 * \param dwX    X-coordinate of character upper-left corner.
 * \param dwY    Y-coordinate of character upper-left corner.
 * \param ucChar Character to output.
 */
extern void LCD_DrawChar( uint32_t dwX, uint32_t dwY, uint8_t ucChar )
{
    uint32_t dwRow, dwCol ;

    assert( (ucChar >= 0x20) && (ucChar <= 0x7F) ) ;

    for ( dwCol = 0 ; dwCol < 10 ; dwCol++ )
    {
        for ( dwRow = 0 ; dwRow < 8 ; dwRow++ )
        {
            if ( (pCharset10x14[((ucChar - 0x20) * 20) + dwCol * 2] >> (7 - dwRow)) & 0x1 )
            {
                LCD_DrawPixel( dwX+dwCol, dwY+dwRow ) ;
            }
        }

        for (dwRow = 0; dwRow < 6; dwRow++ )
        {
            if ((pCharset10x14[((ucChar - 0x20) * 20) + dwCol * 2 + 1] >> (7 - dwRow)) & 0x1)
            {
                LCD_DrawPixel( dwX+dwCol, dwY+dwRow+8 ) ;
            }
        }
    }
}

/**
 * \brief Draws a string inside a LCD buffer, at the given coordinates.
 * Line breaks will be honored.
 *
 * \param dwX      X-coordinate of string top-left corner.
 * \param dwY      Y-coordinate of string top-left corner.
 * \param pString  String to display.
 */
extern void LCD_DrawString( uint32_t dwX, uint32_t dwY, const uint8_t *pString )
{
    uint32_t dwXorg = dwX ;

    while ( *pString != 0 )
    {
        if ( *pString == '\n' )
        {
            dwY += gFont.ucHeight + 2 ;
            dwX = dwXorg ;
        }
        else
        {
            LCD_DrawChar( dwX, dwY, *pString ) ;
            dwX += gFont.ucWidth + 2 ;
        }

        pString++ ;
    }
}

