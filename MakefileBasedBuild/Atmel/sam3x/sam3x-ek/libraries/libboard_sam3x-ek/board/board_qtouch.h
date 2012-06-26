#ifndef _BOARD_QTOUCH_
#define _BOARD_QTOUCH_

/**
 * \file
 * QTouch
 * PIO definitions for Slider
 * \ref SLIDER_IOMASK_SNS
 * \ref SLIDER_IOMASK_SNSK
 * \ref PINS_SLIDER_SNS
 * \ref PINS_SLIDER_SNSK
 *
 * PIO definitions for keys
 * \ref KEY_IOMASK_SNS
 * \ref KEY_IOMASK_SNSK
 * \ref PINS_KEY_SNS
 * \ref PINS_KEY_SNSK
 *
 * PIOS for QTouch
 * \ref PINS_QTOUCH
 *
 */

// ----------------------------------------------------------------------------------------------------------
// QTOUCH
// checked - odi
// ----------------------------------------------------------------------------------------------------------
/** PIO definitions for Slider */
#define SLIDER_IOMASK_SNS   (uint32_t)(PIO_PF0 | PIO_PF2 | PIO_PF4)
#define SLIDER_IOMASK_SNSK  (uint32_t)(PIO_PF1 | PIO_PF3 | PIO_PF5)
#define PINS_SLIDER_SNS     {SLIDER_IOMASK_SNS,  PIOF, ID_PIOF, PIO_INPUT, PIO_DEFAULT}
#define PINS_SLIDER_SNSK    {SLIDER_IOMASK_SNSK, PIOF, ID_PIOF, PIO_INPUT, PIO_DEFAULT}

/** PIO definitions for keys */
#define KEY_IOMASK_SNS   (uint32_t)(PIO_PE21 | PIO_PE27 | PIO_PE25 | PIO_PE19 | PIO_PE23)
#define KEY_IOMASK_SNSK  (uint32_t)(PIO_PE20 | PIO_PE26 | PIO_PE24 | PIO_PE18 | PIO_PE22)
#define PINS_KEY_SNS     {KEY_IOMASK_SNS,  PIOE, ID_PIOE, PIO_INPUT, PIO_DEFAULT}
#define PINS_KEY_SNSK    {KEY_IOMASK_SNSK, PIOE, ID_PIOE, PIO_INPUT, PIO_DEFAULT}

/** PIOS for QTouch */
#define PINS_QTOUCH     PINS_SLIDER_SNS, PINS_SLIDER_SNSK, PINS_KEY_SNS, PINS_KEY_SNSK


#endif /* _BOARD_QTOUCH_ */
