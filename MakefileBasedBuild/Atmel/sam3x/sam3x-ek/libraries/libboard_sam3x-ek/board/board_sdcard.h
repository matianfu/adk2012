#ifndef _BOARD_SDCARD_
#define _BOARD_SDCARD_

/**
 * \file
 * SD Card
 * - \ref BOARD_SD_PINS
 * - \ref BOARD_SD_PIN_CD
 *
 */

/** MCI pins that shall be configured to access the SD card. */
#define BOARD_SD_PINS               PINS_MCI
/** MCI Card Detect pin. */
#define BOARD_SD_PIN_CD             PIN_MCI_CD
/** Total number of MCI interface */
#define BOARD_NUM_MCI           1

#endif /* _BOARD_SDCARD_ */
