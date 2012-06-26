/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2010, Atmel Corporation
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
 * Implementation of HX8347 driver.
 *
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include "board.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>

/*----------------------------------------------------------------------------
 *        Local definitions
 *----------------------------------------------------------------------------*/
/** Pixel cache size */
#define LCD_DATA_CACHE_SIZE BOARD_LCD_WIDTH

/// HX8347 ID code
#define HX8347_HIMAXID_CODE    0x47

/*----------------------------------------------------------------------------
 *        Local variables
 *----------------------------------------------------------------------------*/


/* Pixel cache used to speed up SPI communication */
static LcdColor_t gLcdPixelCache[LCD_DATA_CACHE_SIZE];

/*----------------------------------------------------------------------------
 *        Local functions
 *----------------------------------------------------------------------------*/

/**
 * ----------------------------------------------------------------------------
 * \brief Send command to LCD controller.
 *
 * \param cmd   command.
 * ----------------------------------------------------------------------------
 */
static inline void WriteCmd(uint8_t cmd)
{
    LCD_IR = cmd;
}

/**
 * ----------------------------------------------------------------------------
 * \brief Send one data to LCD controller.
 *
 * \param data   data.
 * ----------------------------------------------------------------------------
 */
static inline void WriteData(uint16_t data)
{
    LCD_D  = data;
}

/**
 * ----------------------------------------------------------------------------
 * \brief Read one data from LCD controller.
 * ----------------------------------------------------------------------------
 */
static inline uint16_t ReadData( void )
{
    return LCD_D ;
}

/**
 * \brief Write mutiple data in buffer to LCD controller.
 *
 * \param pBuf  data buffer.
 * \param size  size in pixels.
 */
static void WriteBuffer( const LcdColor_t *pBuf, uint32_t size )
{
    uint32_t i ;

    for ( i = 0 ; i < size ; ++i )
    {
        WriteData( pBuf[i] ) ;
    }
}

/**
 * ----------------------------------------------------------------------------
 * \brief Write data to LCD Register.
 *
 * \param reg   Register address.
 * \param data  Data to be written.
 * ----------------------------------------------------------------------------
 */
static void WriteReg( uint8_t reg, uint16_t data )
{
    WriteCmd( reg ) ;
    WriteData( data ) ;
}

/**
 * \brief Write data to LCD Register.
 *
 * \param reg   Register address.
 * \return      Data read.
 */
static uint16_t ReadReg( uint8_t reg )
{
    WriteCmd( reg ) ;
    return ReadData() ;
}

/**
 * ----------------------------------------------------------------------------
 * \brief Simple delay, implemented as a loop.
 * Shall be used only during the initialization phase.
 *
 * \param ms    Delay specified in ms.
 * ----------------------------------------------------------------------------
 */
static void Delay( uint32_t ms )
{
    volatile uint32_t i ;

    i = ms * (BOARD_MCK / (1000 * 6)) ;
    while(i--) ;
}

/*----------------------------------------------------------------------------
 *        Basic HX8347 primitives
 *----------------------------------------------------------------------------*/

/**
 * ----------------------------------------------------------------------------
 * \brief Check Box coordinates. Return upper left and bottom right coordinates.
 *
 * \param pX1      X-coordinate of upper-left corner on LCD.
 * \param pY1      Y-coordinate of upper-left corner on LCD.
 * \param pX2      X-coordinate of lower-right corner on LCD.
 * \param pY2      Y-coordinate of lower-right corner on LCD.
 * ----------------------------------------------------------------------------
 */
static void CheckBoundaries( uint32_t *pX1, uint32_t *pY1, uint32_t *pX2, uint32_t *pY2 )
{
    uint32_t dw;

    if ( *pX1 >= BOARD_LCD_WIDTH )
    {
        *pX1=BOARD_LCD_WIDTH-1 ;
    }

    if ( *pX2 >= BOARD_LCD_WIDTH )
    {
        *pX2=BOARD_LCD_WIDTH-1 ;
    }

    if ( *pY1 >= BOARD_LCD_HEIGHT )
    {
        *pY1=BOARD_LCD_HEIGHT-1 ;
    }

    if ( *pY2 >= BOARD_LCD_HEIGHT )
    {
        *pY2=BOARD_LCD_HEIGHT-1 ;
    }

    if ( *pX1 > *pX2 )
    {
        dw = *pX1;
        *pX1 = *pX2;
        *pX2 = dw;
    }

    if ( *pY1 > *pY2 )
    {
        dw = *pY1;
        *pY1 = *pY2;
        *pY2 = dw;
    }
}

/**
 * ----------------------------------------------------------------------------
 *  \brief Set the window size user can access.
 *  \param x_start    X-coordinate of upper-left corner on LCD.
 *  \param y_start    Y-coordinate of upper-left corner on LCD.
 *  \param x_end      X-coordinate of bottom-right corner on LCD.
 *  \param y_end      Y-coordinate of bottom-right corner on LCD.
 * ----------------------------------------------------------------------------
 */
extern void LCD_SetWindow( uint32_t dwX, uint32_t dwY, uint32_t dwWidth, uint32_t dwHeight )
{
    WriteReg( 0x02, (dwX & 0xff00) >> 8 ) ; // column high
    WriteReg( 0x03, dwX & 0xff ) ; // column low
    WriteReg( 0x06, (dwY & 0xff00) >>8 ) ; // row high
    WriteReg( 0x07, dwY & 0xff ) ; // row low

    WriteReg( 0x04, (dwWidth & 0xff00) >>8 ) ; // column high
    WriteReg( 0x05, dwWidth & 0xff ) ; // column low
    WriteReg( 0x08, (dwHeight & 0xff00) >>8 ) ; // row high
    WriteReg( 0x09, dwHeight & 0xff ) ; // row low
}

/**
 * ----------------------------------------------------------------------------
 * \brief Set cursor of LCD srceen.
 * \param x          X-coordinate of upper-left corner on LCD.
 * \param y          Y-coordinate of upper-left corner on LCD.
 * ----------------------------------------------------------------------------
 */
extern void LCD_SetCursor( uint32_t dwX, uint32_t dwY )
{
    WriteReg( HX8347_R02H, (dwX & 0xff00) >>8); // column high
    WriteReg( HX8347_R03H, dwX & 0xff); // column low
    WriteReg( HX8347_R06H, (dwY & 0xff00) >>8); // row high
    WriteReg( HX8347_R07H, dwY & 0xff); // row low
}

/**
 * ----------------------------------------------------------------------------
 * \brief Initialize the LCD controller.
 * ----------------------------------------------------------------------------
 */
extern uint32_t LCD_Initialize( void )
{
    uint16_t chipid ;

    const Pin pPins[] = {BOARD_LCD_PINS};
    SmcCs_number *pSmcCs = &(SMC->SMC_CS_NUMBER[BOARD_LCD_NCS]);

    // Enable pins
    PIO_PinConfigure( pPins, PIO_LISTSIZE( pPins ) ) ;

    // Enable peripheral clock
    PMC_EnablePeripheral( ID_SMC ) ;

    // EBI SMC Configuration
    pSmcCs->SMC_SETUP = BOARD_LCD_SETUP ;
    pSmcCs->SMC_PULSE = BOARD_LCD_PULSE ;
    pSmcCs->SMC_CYCLE = BOARD_LCD_CYCLE ;
    pSmcCs->SMC_MODE  = BOARD_LCD_MODE ;

    /* Turn off LCD */
    LCD_Off() ;

    /*======== LCD module initial code ========*/

    // Check HX8347 chipid
    chipid = ReadReg( HX8347_R67H ) ;
    if ( chipid != HX8347_HIMAXID_CODE )
    {
        printf( "Read HX8347 chip ID error, skip initialization.\r\n" ) ;

        return 1 ;
    }

    // Start internal OSC
    WriteReg(HX8347_R19H, 0x49); // OSCADJ=10 0000, OSD_EN=1 //60Hz
    WriteReg(HX8347_R93H, 0x0C); // RADJ=1100

    // Power on flow
    WriteReg(HX8347_R44H, 0x4D); // VCM=100 1101
    WriteReg(HX8347_R45H, 0x11); // VDV=1 0001
    WriteReg(HX8347_R20H, 0x40); // BT=0100
    WriteReg(HX8347_R1DH, 0x07); // VC1=111
    WriteReg(HX8347_R1EH, 0x00); // VC3=000
    WriteReg(HX8347_R1FH, 0x04); // VRH=0100

    WriteReg(HX8347_R1CH, 0x04); // AP=100
    WriteReg(HX8347_R1BH, 0x10); // GASENB=0, PON=1, DK=0, XDK=0, DDVDH_TRI=0, STB=0
    Delay(50);

    WriteReg(HX8347_R43H, 0x80); // Set VCOMG=1
    Delay(50);
/*
    // Gamma for CMO 2.8
    WriteReg(HX8347_R46H, 0x95);
    WriteReg(HX8347_R47H, 0x51);
    WriteReg(HX8347_R48H, 0x00);
    WriteReg(HX8347_R49H, 0x36);
    WriteReg(HX8347_R4AH, 0x11);
    WriteReg(HX8347_R4BH, 0x66);
    WriteReg(HX8347_R4CH, 0x14);
    WriteReg(HX8347_R4DH, 0x77);
    WriteReg(HX8347_R4EH, 0x13);
    WriteReg(HX8347_R4FH, 0x4C);
    WriteReg(HX8347_R50H, 0x46);
    WriteReg(HX8347_R51H, 0x46);
*/
    //240x320 window setting
    WriteReg(HX8347_R02H, 0x00); // Column address start2
    WriteReg(HX8347_R03H, 0x00); // Column address start1
    WriteReg(HX8347_R04H, 0x00); // Column address end2
    WriteReg(HX8347_R05H, 0xEF); // Column address end1
    WriteReg(HX8347_R06H, 0x00); // Row address start2
    WriteReg(HX8347_R07H, 0x00); // Row address start1
    WriteReg(HX8347_R08H, 0x01); // Row address end2
    WriteReg(HX8347_R09H, 0x3F); // Row address end1

    // Display Setting
    WriteReg(HX8347_R01H, 0x06); // IDMON=0, INVON=1, NORON=1, PTLON=0
    //WriteReg(HX8347_R16H, 0xC8); // MY=1, MX=1, MV=0, BGR=1
    WriteReg(HX8347_R16H, 0x68); // MY=0, MX=1, MV=1, RGB XY exchange X mirror
    WriteReg(HX8347_R23H, 0x95); // N_DC=1001 0101
    WriteReg(HX8347_R24H, 0x95); // P_DC=1001 0101
    WriteReg(HX8347_R25H, 0xFF); // I_DC=1111 1111
    WriteReg(HX8347_R27H, 0x06); // N_BP=0000 0110
    WriteReg(HX8347_R28H, 0x06); // N_FP=0000 0110
    WriteReg(HX8347_R29H, 0x06); // P_BP=0000 0110
    WriteReg(HX8347_R2AH, 0x06); // P_FP=0000 0110
    WriteReg(HX8347_R2CH, 0x06); // I_BP=0000 0110
    WriteReg(HX8347_R2DH, 0x06); // I_FP=0000 0110
    WriteReg(HX8347_R3AH, 0x01); // N_RTN=0000, N_NW=001
    WriteReg(HX8347_R3BH, 0x01); // P_RTN=0000, P_NW=001
    WriteReg(HX8347_R3CH, 0xF0); // I_RTN=1111, I_NW=000
    WriteReg(HX8347_R3DH, 0x00); // DIV=00
    WriteReg(HX8347_R3EH, 0x38); // SON=38h
    WriteReg(HX8347_R40H, 0x0F); // GDON=0Fh
    WriteReg(HX8347_R41H, 0xF0); // GDOF=F0h

    return 0;
}

/**
 * ----------------------------------------------------------------------------
 * \brief Turn on the LCD.
 * ----------------------------------------------------------------------------
 */
extern void LCD_On( void )
{
    // Display ON Setting
    WriteReg( HX8347_R90H, 0x7F ) ; // SAP=0111 1111
    WriteReg( HX8347_R26H, 0x04 ) ; // GON=0, DTE=0, D=01
    Delay( 100 ) ;
    WriteReg( HX8347_R26H, 0x24 ) ; // GON=1, DTE=0, D=01
    WriteReg( HX8347_R26H, 0x2C ) ; // GON=1, DTE=0, D=11
    Delay( 100 ) ;
    WriteReg( HX8347_R26H, 0x3C ) ; // GON=1, DTE=1, D=11
}

/**
 * ----------------------------------------------------------------------------
 * \brief Turn off the LCD.
 * ----------------------------------------------------------------------------
 */
extern void LCD_Off( void )
{
    WriteReg( HX8347_R90H, 0x00 ) ; // SAP=0000 0000
    WriteReg( HX8347_R26H, 0x00 ) ; // GON=0, DTE=0, D=00
}

/**
 * ----------------------------------------------------------------------------
 * \brief Convert 24 bit RGB color into 5-6-5 rgb color space.
 *
 * Initialize the LcdColor_t cache with the color pattern.
 * \param x  24-bits RGB color.
 * \return 0 for successfull operation.
 * ----------------------------------------------------------------------------
 */
extern uint32_t LCD_SetColor( uint32_t dwRgb24Bits )
{
    uint16_t i ;
    LcdColor_t wColor ;

    wColor = (dwRgb24Bits & 0xF80000) >> 8 |
             (dwRgb24Bits & 0x00FC00) >> 5 |
             (dwRgb24Bits & 0x0000F8) >> 3;

    /* Fill the cache with selected color */
    for ( i = 0 ; i < LCD_DATA_CACHE_SIZE ; ++i )
    {
        gLcdPixelCache[i] = wColor ;
    }

    return 0 ;
}

/**
 * ----------------------------------------------------------------------------
 * \brief Draw a LcdColor_t on LCD of given color.
 *
 * \param x  X-coordinate of pixel.
 * \param y  Y-coordinate of pixel.
 * ----------------------------------------------------------------------------
 */
extern uint32_t LCD_DrawPixel( uint32_t x, uint32_t y )
{
    if ( (x >= BOARD_LCD_WIDTH) || (y >= BOARD_LCD_HEIGHT) )
    {
        return 1 ;
    }

    /* Set cursor */
    LCD_SetCursor( x, y ) ;

    /* Prepare to write in GRAM */
    WriteCmd( HX8347_R22H ) ;

    WriteData( *gLcdPixelCache ) ;

    return 0 ;
}

/**
 * ----------------------------------------------------------------------------
 * \brief Write several pixels with the same color to LCD GRAM.
 *
 * LcdColor_t color is set by the LCD_SetColor() function.
 * This function is optimized using an sram buffer to transfer block instead of
 * individual pixels in order to limit the number of SPI interrupts.
 * \param dwX1      X-coordinate of upper-left corner on LCD.
 * \param dwY1      Y-coordinate of upper-left corner on LCD.
 * \param dwX2      X-coordinate of lower-right corner on LCD.
 * \param dwY2      Y-coordinate of lower-right corner on LCD.
 * ----------------------------------------------------------------------------
 */
extern uint32_t LCD_DrawFilledRectangle( uint32_t dwX1, uint32_t dwY1, uint32_t dwX2, uint32_t dwY2 )
{
    uint32_t size, blocks ;

    /* Swap coordinates if necessary */
    CheckBoundaries( &dwX1, &dwY1, &dwX2, &dwY2 ) ;

    /* Determine the refresh window area */
    LCD_SetWindow( dwX1, dwY1, dwX2, dwY2 ) ;

    /* Set cursor */
    LCD_SetCursor( dwX1, dwY1 ) ;

    /* Prepare to write in GRAM */
    WriteCmd( HX8347_R22H ) ;

    size = (dwX2 - dwX1 + 1) * (dwY2 - dwY1 + 1) ;
    /* Send pixels blocks => one SPI IT / block */
    blocks = size / LCD_DATA_CACHE_SIZE ;
    while ( blocks-- )
    {
        WriteBuffer( gLcdPixelCache, LCD_DATA_CACHE_SIZE ) ;
    }
    /* Send remaining pixels */
    WriteBuffer( gLcdPixelCache, size % LCD_DATA_CACHE_SIZE ) ;

    /* Reset the refresh window area */
    LCD_SetWindow( 0, 0, BOARD_LCD_WIDTH - 1, BOARD_LCD_HEIGHT - 1 ) ;

    return 0 ;
}

/**
 * \brief Write several pixels pre-formatted in a bufer to LCD GRAM.
 *
 * \param dwX1      X-coordinate of upper-left corner on LCD.
 * \param dwY1      Y-coordinate of upper-left corner on LCD.
 * \param dwX2      X-coordinate of lower-right corner on LCD.
 * \param dwY2      Y-coordinate of lower-right corner on LCD.
 * \param pBuffer   LcdColor_t buffer area.
 */
extern uint32_t LCD_DrawPicture( uint32_t dwX1, uint32_t dwY1, uint32_t dwX2, uint32_t dwY2, const LcdColor_t *pBuffer )
{
    uint32_t size, blocks ;
    LcdColor_t currentColor ;

    /* Swap coordinates if necessary */
    CheckBoundaries( &dwX1, &dwY1, &dwX2, &dwY2 ) ;

    /* Determine the refresh window area */
    LCD_SetWindow( dwX1, dwY1, dwX2, dwY2 ) ;

    /* Set cursor */
    LCD_SetCursor( dwX1, dwY1 ) ;

    /* Prepare to write in GRAM */
    WriteCmd( HX8347_R22H ) ;

    size = (dwX2 - dwX1 + 1) * (dwY2 - dwY1 + 1) ;
    /* Check if the buffer is within the SRAM */
    if ( (IRAM0_ADDR <= (uint32_t)pBuffer) && ((uint32_t)pBuffer < NFC_RAM_ADDR) )
    {
        WriteBuffer( pBuffer, size ) ;
    }
    /* If the buffer is not in SRAM, transfer it in SRAM first before transfer */
    else
    {
        /* Use color buffer as a cache */
        currentColor = gLcdPixelCache[0];
        /* Send pixels blocks => one SPI IT / block */
        blocks = size / LCD_DATA_CACHE_SIZE;
        while ( blocks-- )
        {
            memcpy( gLcdPixelCache, pBuffer, LCD_DATA_CACHE_SIZE * sizeof( LcdColor_t ) ) ;
            WriteBuffer( gLcdPixelCache, LCD_DATA_CACHE_SIZE ) ;
            pBuffer += LCD_DATA_CACHE_SIZE ;
        }

        /* Send remaining pixels */
        memcpy( gLcdPixelCache, pBuffer, (size % LCD_DATA_CACHE_SIZE) * sizeof( LcdColor_t ) ) ;
        WriteBuffer( gLcdPixelCache, size % LCD_DATA_CACHE_SIZE ) ;

        /* Restore the color cache */
        LCD_SetColor( currentColor ) ;
    }

    /* Reset the refresh window area */
    LCD_SetWindow( 0, 0, BOARD_LCD_WIDTH - 1, BOARD_LCD_HEIGHT - 1 ) ;

    return 0 ;
}

/*
 * \brief Draw a line on LCD, which is not horizontal or vertical.
 *
 * \param x         X-coordinate of line start.
 * \param y         Y-coordinate of line start.
 * \param length    line length.
 * \param direction line direction: 0 - horizontal, 1 - vertical.
 * \param color     LcdColor_t color.
 */
static uint32_t DrawLineBresenham( uint32_t dwX1, uint32_t dwY1, uint32_t dwX2, uint32_t dwY2 )
{
    int dx, dy ;
    int i ;
    int xinc, yinc, cumul ;
    int x, y ;

    x = dwX1 ;
    y = dwY1 ;
    dx = dwX2 - dwX1 ;
    dy = dwY2 - dwY1 ;

    xinc = ( dx > 0 ) ? 1 : -1 ;
    yinc = ( dy > 0 ) ? 1 : -1 ;
    dx = ( dx > 0 ) ? dx : -dx ;
    dy = ( dy > 0 ) ? dy : -dy ;

    LCD_DrawPixel( x, y ) ;

    if ( dx > dy )
    {
      cumul = dx / 2 ;
      for ( i = 1 ; i <= dx ; i++ )
      {
        x += xinc ;
        cumul += dy ;

        if ( cumul >= dx )
        {
          cumul -= dx ;
          y += yinc ;
        }
        LCD_DrawPixel( x, y ) ;
      }
    }
    else
    {
        cumul = dy / 2 ;
        for ( i = 1 ; i <= dy ; i++ )
        {
            y += yinc ;
            cumul += dx ;

            if ( cumul >= dy )
            {
                cumul -= dy ;
                x += xinc ;
            }

            LCD_DrawPixel( x, y ) ;
        }
    }

    return 0 ;
}


/*
 * \brief Draw a line on LCD, horizontal and vertical line are supported.
 *
 * \param dwX1      X-coordinate of line start.
 * \param dwY1      Y-coordinate of line start.
 * \param dwX2      X-coordinate of line end.
 * \param dwY2      Y-coordinate of line end.
  */
extern uint32_t LCD_DrawLine ( uint32_t dwX1, uint32_t dwY1, uint32_t dwX2, uint32_t dwY2 )
{
    /* Optimize horizontal or vertical line drawing */
    if ( ( dwY1 == dwY2 ) || (dwX1 == dwX2) )
    {
        LCD_DrawFilledRectangle( dwX1, dwY1, dwX2, dwY2 ) ;
    }
    else
    {
        DrawLineBresenham( dwX1, dwY1, dwX2, dwY2 ) ;
    }

    return 0 ;
}

/**
 * \brief Draws a circle on LCD, at the given coordinates.
 *
 * \param dwX      X-coordinate of circle center.
 * \param dwY      Y-coordinate of circle center.
 * \param dwR      circle radius.
*/
extern uint32_t LCD_DrawCircle( uint32_t dwX, uint32_t dwY, uint32_t dwR )
{
    int32_t   d ;    /* Decision Variable */
    uint32_t  curX ; /* Current X Value */
    uint32_t  curY ; /* Current Y Value */

    if ( dwR == 0 )
    {
        return 0 ;
    }

    d = 3 - (dwR << 1) ;
    curX = 0 ;
    curY = dwR ;

    while ( curX <= curY )
    {
        LCD_DrawPixel(dwX + curX, dwY + curY);
        LCD_DrawPixel(dwX + curX, dwY - curY);
        LCD_DrawPixel(dwX - curX, dwY + curY);
        LCD_DrawPixel(dwX - curX, dwY - curY);
        LCD_DrawPixel(dwX + curY, dwY + curX);
        LCD_DrawPixel(dwX + curY, dwY - curX);
        LCD_DrawPixel(dwX - curY, dwY + curX);
        LCD_DrawPixel(dwX - curY, dwY - curX);

        if (d < 0) {
            d += (curX << 2) + 6;
        }
        else {
            d += ((curX - curY) << 2) + 10;
            curY--;
        }
        curX++;
    }
    return 0;
}
/**
 * \brief Draws a filled circle on LCD, at the given coordinates.
 *
 * \param dwX              X-coordinate of circle center.
 * \param dwY              Y-coordinate of circle center.
 * \param dwRadius      circle radius.
*/
extern uint32_t LCD_DrawFilledCircle( uint32_t dwX, uint32_t dwY, uint32_t dwRadius )
{
    signed int d ; // Decision Variable
    uint32_t dwCurX ; // Current X Value
    uint32_t dwCurY ; // Current Y Value
    uint32_t dwXmin, dwYmin;

    if ( dwRadius == 0 )
    {
        return 0 ;
    }
    d = 3 - (dwRadius << 1) ;
    dwCurX = 0 ;
    dwCurY = dwRadius ;

    while ( dwCurX <= dwCurY )
    {
        dwXmin = (dwCurX > dwX) ? 0 : dwX-dwCurX;
        dwYmin = (dwCurY > dwY) ? 0 : dwY-dwCurY;
        LCD_DrawFilledRectangle( dwXmin, dwYmin, dwX+dwCurX, dwYmin ) ;
        LCD_DrawFilledRectangle( dwXmin, dwY+dwCurY, dwX+dwCurX, dwY+dwCurY ) ;
        dwXmin = (dwCurY > dwX) ? 0 : dwX-dwCurY;
        dwYmin = (dwCurX > dwY) ? 0 : dwY-dwCurX;
        LCD_DrawFilledRectangle( dwXmin, dwYmin, dwX+dwCurY, dwYmin ) ;
        LCD_DrawFilledRectangle( dwXmin, dwY+dwCurX, dwX+dwCurY, dwY+dwCurX ) ;

        if ( d < 0 )
        {
            d += (dwCurX << 2) + 6 ;
        }
        else
        {
            d += ((dwCurX - dwCurY) << 2) + 10;
            dwCurY-- ;
        }

        dwCurX++ ;
    }

    return 0 ;
}
/**
 * \brief Draws a rectangle on LCD, at the given coordinates.
 *
 * \param dwX 1     X-coordinate of one angle.
 * \param dwY1      Y-coordinate of one angle.
 * \param dwX2      X-coordinate of another angle.
 * \param dwY2      Y-coordinate of another angle.
*/
extern uint32_t LCD_DrawRectangle( uint32_t dwX1, uint32_t dwY1, uint32_t dwX2, uint32_t dwY2 )
{
    CheckBoundaries( &dwX1, &dwY1, &dwX2, &dwY2 ) ;

    LCD_DrawFilledRectangle( dwX1, dwY1, dwX2, dwY1 ) ;
    LCD_DrawFilledRectangle( dwX1, dwY2, dwX2, dwY2 ) ;

    LCD_DrawFilledRectangle( dwX1, dwY1, dwX1, dwY2 ) ;
    LCD_DrawFilledRectangle( dwX2, dwY1, dwX2, dwY2 ) ;

    return 0 ;
}


/**
 * \brief Set the backlight of the LCD (AAT3193).
 *
 * \param level   Backlight brightness level [1..16], 1 means maximum brightness.
 */
extern void LCD_SetBacklight( uint32_t level )
{
    uint32_t i ;
    const Pin pPins[] = { BOARD_BACKLIGHT_PIN } ;

    /* Ensure valid level */
    level = (level < 1) ? 1 : level ;
    level = (level > 32) ? 32 : level ;

    /* Enable pins */
    PIO_PinConfigure( pPins, PIO_LISTSIZE( pPins ) ) ;

    /* Switch off backlight */
    PIO_PinClear( pPins ) ;
    i = 600 * (BOARD_MCK / 1000000) ;    /* wait for at least 500us */
    while ( i-- ) ;

    /* Set new backlight level */
    for ( i = 0 ; i < level ; i++ )
    {
        PIO_PinClear( pPins ) ;
        PIO_PinClear( pPins ) ;
        PIO_PinClear( pPins ) ;

        PIO_PinSet( pPins ) ;
        PIO_PinSet( pPins ) ;
        PIO_PinSet( pPins ) ;
    }
}
/**
 * \brief Set the display fomat of the LCD to landscape(AAT3193).
 *
 * \param dwRGB  The color value   .
 */
extern void LCD_SetDisplayLandscape( uint32_t dwRGB )
{
    dwRGB = dwRGB;
}
/**
 * \brief Set the display fomat of the LCD to portrait(AAT3193).
 *
 * \param level   The color value.
 */
extern void LCD_SetDisplayPortrait( uint32_t dwRGB )
{
    dwRGB = dwRGB;
}
