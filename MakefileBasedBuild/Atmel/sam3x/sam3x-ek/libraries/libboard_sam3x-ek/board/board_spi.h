#ifndef _BOARD_SPI_
#define _BOARD_SPI_

/**
 * \file
 * SPI
 * - \ref PIN_SPI_MISO
 * - \ref PIN_SPI_MOSI
 * - \ref PIN_SPI_SPCK
 * - \ref PINS_SPI
 * - \ref PIN_SPI_NPCS0
 * - \ref PIN_SPI1_MISO
 * - \ref PIN_SPI1_MOSI
 * - \ref PIN_SPI1_SPCK
 * - \ref PINS_SPI1
 * - \ref PIN_SPI1_NPCS0
 *
 */
 
// ----------------------------------------------------------------------------------------------------------
// SPI
// checked - tvd
// ----------------------------------------------------------------------------------------------------------
/** SPI MISO pin definition. */
#define PIN_SPI0_MISO    {PIO_PA25A_SPI0_MISO, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/** SPI MOSI pin definition. */
#define PIN_SPI0_MOSI    {PIO_PA26A_SPI0_MOSI, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/** SPI SPCK pin definition. */
#define PIN_SPI0_SPCK    {PIO_PA27A_SPI0_SPCK, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/** SPI chip select pin definition. */
#define PIN_SPI0_NPCS0  {PIO_PA28A_SPI0_NPCS0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/** List of SPI pin definitions (MISO, MOSI & SPCK). */
#define PINS_SPI0        PIN_SPI0_MISO, PIN_SPI0_MOSI, PIN_SPI0_SPCK

/** SPI1 MISO pin definition. */
#define PIN_SPI1_MISO    {PIO_PE28A_SPI1_MISO, PIOE, ID_PIOE, PIO_PERIPH_A, PIO_DEFAULT}
/** SPI1 MOSI pin definition. */
#define PIN_SPI1_MOSI    {PIO_PE29A_SPI1_MOSI, PIOE, ID_PIOE, PIO_PERIPH_A, PIO_DEFAULT}
/** SPI1 SPCK pin definition. */
#define PIN_SPI1_SPCK    {PIO_PE30A_SPI1_SPCK, PIOE, ID_PIOE, PIO_PERIPH_A, PIO_DEFAULT}
/** SPI1 chip select pin definition. */
#define PIN_SPI1_NPCS0  {PIO_PE31A_SPI1_NPCS0, PIOE, ID_PIOE, PIO_PERIPH_A, PIO_DEFAULT}
/** List of SPI1 pin definitions (MISO, MOSI & SPCK). */
#define PINS_SPI1        PIN_SPI1_MISO, PIN_SPI1_MOSI, PIN_SPI1_SPCK


#endif /* _BOARD_SPI_ */
