#ifndef _BOARD_USB_
#define _BOARD_USB_

/**
 * \file
 * USB
 * - \ref PIN_USBOTG_VBOF
 * - \ref PIN_USB_FAULT
 *
 */
 
// ----------------------------------------------------------------------------------------------------------
// USB
// checked - tvd jcb
// ----------------------------------------------------------------------------------------------------------
/** USB OTG VBus On/Off: Bus Power Control Port. */
#define PIN_UOTGHS_VBOF  { PIO_PB10, PIOB, ID_PIOB, PIO_PERIPH_A, PIO_PULLUP }
/** USB OTG Identification: Mini Connector Identification Port. */
#define PIN_UOTGHS_ID    { PIO_PB11, PIOB, ID_PIOB, PIO_PERIPH_A, PIO_PULLUP }
/** USB Fault monitoring pin definition. */
#define PIN_UOTGHS_FAULT { PIO_PE5, PIOE, ID_PIOE, PIO_INPUT, PIO_PULLUP }

/*----------------------------------------------------------------------------*/
/**
 * \page sam3x_ek_usb "SAM3X-EK - USB device"
 *
 * \section Definitions
 * - \ref BOARD_USB_BMATTRIBUTES
 * - \ref CHIP_USB_UDP
 * - \ref CHIP_USB_PULLUP_INTERNAL
 * - \ref CHIP_USB_NUMENDPOINTS
 * - \ref CHIP_USB_ENDPOINTS_MAXPACKETSIZE
 * - \ref CHIP_USB_ENDPOINTS_BANKS
 */

/** USB attributes configuration descriptor (bus or self powered, remote wakeup) */
#define BOARD_USB_BMATTRIBUTES              USBConfigurationDescriptor_SELFPOWERED_RWAKEUP

/** Indicates chip has an internal pull-up. */
#define CHIP_USB_PULLUP_INTERNAL

/** Number of USB endpoints */
#define CHIP_USB_NUMENDPOINTS 10

/** Endpoints max paxcket size */
#define CHIP_USB_ENDPOINTS_MAXPACKETSIZE(i) \
   ((i == 0) ? 64 : 1024)

/** Endpoints Number of Bank */
#define CHIP_USB_ENDPOINTS_BANKS(i) \
   ((i == 0) ? 1 : \
   ((i == 1) ? 3 : \
   ((i == 2) ? 3 : 2)))


#endif /* _BOARD_USB_ */
