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
 * Interface for draw font and string on LCD.
 *
 */

/**
 * \section Purpose
 *
 * The lcd_font.h file declares a font structure and a LCD_DrawChar function
 * that must be implemented by a font definition file to be used with the
 * LCD_DrawString method.
 *
 * The lcd_font10x14.c implements the necessary variable and function for a 10x14
 * font.
 *
 * \section Usage
 *
 * -# Declare a gFont global variable with the necessary Font information.
 * -# Implement an LCD_DrawChar function which displays the specified
 *    character on the LCD.
 * -# Use the LCD_DrawString method to display a complete string.
 */

#ifndef _LCD_FONT_
#define _LCD_FONT_

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include <stdint.h>

/*----------------------------------------------------------------------------
 *        Types
 *----------------------------------------------------------------------------*/


/** \brief Describes the font (width, height, supported characters, etc.) used by
 * the LCD driver draw API.
 */
typedef struct _Font {
    /* Font width in pixels. */
    uint8_t ucWidth;
    /* Font height in pixels. */
    uint8_t ucHeight;
} Font;

/*----------------------------------------------------------------------------
 *        Variables
 *----------------------------------------------------------------------------*/

/** Global variable describing the font being instancied. */
extern const Font gFont;

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/
extern void LCD_DrawChar( uint32_t dwX, uint32_t dwY, uint8_t ucChar ) ;
extern void LCD_DrawString( uint32_t dwX, uint32_t dwY, const uint8_t *pString ) ;

#endif /* #ifndef LCD_FONT_ */

