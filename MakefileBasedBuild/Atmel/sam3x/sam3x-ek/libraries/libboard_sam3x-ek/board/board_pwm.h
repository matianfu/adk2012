#ifndef _BOARD_PWM_
#define _BOARD_PWM_

/**
 * \file
 * PWMC
 * - \ref PIN_PWMC_PWMH0
 * - \ref PIN_PWMC_PWML0
 * - \ref PIN_PWMC_PWMH1
 * - \ref PIN_PWMC_PWML1
 * - \ref PIN_PWMC_PWMH2
 * - \ref PIN_PWMC_PWML2
 * - \ref PIN_PWMC_PWMH3
 * - \ref PIN_PWMC_PWML3
 * - \ref PIN_PWM_LED0
 * - \ref PIN_PWM_LED1
 * - \ref PIN_PWM_LED2
 * - \ref CHANNEL_PWM_LED0
 * - \ref CHANNEL_PWM_LED1
 * - \ref CHANNEL_PWM_LED2
 *
 */

// ----------------------------------------------------------------------------------------------------------
// PWM
// ----------------------------------------------------------------------------------------------------------
/** PWMC PWM0 pin definition: Output High. */
#define PIN_PWMC_PWMH0  {PIO_PB12B_PWMH0, PIOB, ID_PIOB, PIO_PERIPH_B, PIO_DEFAULT}
/** PWMC PWM0 pin definition: Output Low. */
#define PIN_PWMC_PWML0  {PIO_PA21B_PWML0, PIOA, ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT}
/** PWMC PWM1 pin definition: Output High. */
#define PIN_PWMC_PWMH1  {PIO_PB13B_PWMH1, PIOB, ID_PIOB, PIO_PERIPH_B, PIO_DEFAULT}
/** PWMC PWM1 pin definition: Output Low. */
#define PIN_PWMC_PWML1  {PIO_PB17B_PWML1, PIOB, ID_PIOB, PIO_PERIPH_B, PIO_DEFAULT}
/** PWMC PWM2 pin definition: Output High. */
#define PIN_PWMC_PWMH2  {PIO_PA13B_PWMH2, PIOA, ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT}
/** PWMC PWM2 pin definition: Output Low. */
#define PIN_PWMC_PWML2  {PIO_PA20B_PWML2, PIOA, ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT}
/** PWMC PWM3 pin definition: Output High. */
#define PIN_PWMC_PWMH3  {PIO_PA9B_PWMH3, PIOA, ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT}
/** PWMC PWM3 pin definition: Output Low. */
#define PIN_PWMC_PWML3  {PIO_PC8B_PWML3, PIOC, ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT}
/** PWM pins definition for LED0 */
#define PIN_PWM_LED0 PIN_PWMC_PWMH0, PIN_PWMC_PWML0
/** PWM pins definition for LED1 */
#define PIN_PWM_LED1 PIN_PWMC_PWMH1, PIN_PWMC_PWML1
/** PWM pins definition for LED2 */
#define PIN_PWM_LED2 PIN_PWMC_PWMH2, PIN_PWMC_PWML2
/** PWM channel for LED0 */
#define CHANNEL_PWM_LED0 0
/** PWM channel for LED1 */
#define CHANNEL_PWM_LED1 1
/** PWM channel for LED2 */
#define CHANNEL_PWM_LED2 2

#endif /* _BOARD_PWM_ */
