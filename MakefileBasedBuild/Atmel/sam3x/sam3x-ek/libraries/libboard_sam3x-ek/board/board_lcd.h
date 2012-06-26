#ifndef _BOARD_LCD_
#define _BOARD_LCD_

/**
 * \file 
 * LCD
 *
 * - \ref BOARD_LCD_PINS
 * - \ref BOARD_BACKLIGHT_PIN
 * - \ref BOARD_LCD_BASE
 * - \ref BOARD_LCD_RS
 * - \ref BOARD_LCD_WIDTH
 * - \ref BOARD_LCD_HEIGHT
 *
 */

/** LCD Base Address */
#define BOARD_LCD_BASE              0x62000000
/** LCD pins definition. */
#define BOARD_LCD_PINS              PIN_EBI_DATA_BUS, PIN_EBI_NRD, PIN_EBI_NWE, PIN_EBI_NCS2, PIN_EBI_LCD_RS
/** Board chip select */
#define BOARD_LCD_NCS 2
/** Board LCD SMC setup timings */
#define BOARD_TIMING_NS(ns) \
             (((uint32_t) ((ns*(BOARD_MCK/1000000))/1000)) + 1)

#define BOARD_LCD_SETUP \
             SMC_SETUP_NWE_SETUP(BOARD_TIMING_NS(10)) | \
             SMC_SETUP_NCS_WR_SETUP(BOARD_TIMING_NS(10)) | \
             SMC_SETUP_NRD_SETUP(BOARD_TIMING_NS(90)) | \
             SMC_SETUP_NCS_RD_SETUP(BOARD_TIMING_NS(90))
/** Board LCD SMC pulse timings */
#define BOARD_LCD_PULSE \
             SMC_PULSE_NWE_PULSE(BOARD_TIMING_NS(35)) | \
             SMC_PULSE_NCS_WR_PULSE(BOARD_TIMING_NS(35)) | \
             SMC_PULSE_NRD_PULSE(BOARD_TIMING_NS(355)) | \
             SMC_PULSE_NCS_RD_PULSE(BOARD_TIMING_NS(355))
/** Board LCD SMC cycle timings */
#define BOARD_LCD_CYCLE \
            SMC_CYCLE_NWE_CYCLE(BOARD_TIMING_NS(100)) | \
            SMC_CYCLE_NRD_CYCLE(BOARD_TIMING_NS(460))
/** Board LCD SMC mode */
#define BOARD_LCD_MODE \
        SMC_MODE_WRITE_MODE | \
        SMC_MODE_READ_MODE | \
        SMC_MODE_DBW
/** Display width in pixels. */
#define BOARD_LCD_WIDTH             320
/** Display height in pixels. */
#define BOARD_LCD_HEIGHT            240

/** Backlight pin definition. */
#define BOARD_BACKLIGHT_PIN         {PIO_PB27, PIOB, ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}

#endif /* _BOARD_LCD_ */
