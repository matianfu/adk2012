#ifndef _BOARD_TOUCHSCREEN_
#define _BOARD_TOUCHSCREEN_

/**
 * \file
 * TouchScreen
 *
 * - \ref PIN_TSC_IRQ
 * - \ref PIN_TSC_BUSY
 * - \ref BOARD_TSC_SPI_BASE
 * - \ref BOARD_TSC_SPI_ID
 * - \ref BOARD_TSC_SPI_PINS
 * - \ref BOARD_TSC_NPCS
 * - \ref BOARD_TSC_NPCS_PIN
 *
 */

// ----------------------------------------------------------------------------------------------------------
// Touchscreen
// checked - tvd
// ----------------------------------------------------------------------------------------------------------
/** Touchscreen controller IRQ pin definition. */
#define PIN_TSC_IRQ  {PIO_PA31, PIOA, ID_PIOA, PIO_INPUT, PIO_PULLUP}
/** Touchscreen controller Busy pin definition. */
#define PIN_TSC_BUSY {PIO_PA30, PIOA, ID_PIOA, PIO_INPUT, PIO_PULLUP}

/** Base address of SPI peripheral connected to the touchscreen controller. */
#define BOARD_TSC_SPI_BASE         SPI0
/** Identifier of SPI peripheral connected to the touchscreen controller. */
#define BOARD_TSC_SPI_ID           ID_SPI0
/** Pins of the SPI peripheral connected to the touchscreen controller. */
#define BOARD_TSC_SPI_PINS         PINS_SPI0
/** Chip select connected to the touchscreen controller. */
#define BOARD_TSC_NPCS             0
/** Chip select pin connected to the touchscreen controller. */
/* We use PIO mode for chip select to meet ADS7843's timing specification */
#define BOARD_TSC_NPCS_PIN  {PIO_PA28A_SPI0_NPCS0, PIOA, ID_PIOA, PIO_OUTPUT_1, PIO_PULLUP}

#endif /* _BOARD_TOUCHSCREEN_ */
