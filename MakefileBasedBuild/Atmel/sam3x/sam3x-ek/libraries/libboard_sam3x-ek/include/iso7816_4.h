/**
  *  \file iso7816_4.h
  *
  *  Include Defines & function prototype for the iso7816 functions.
  */

#ifndef _ISO7816_4_
#define _ISO7816_4_

/*------------------------------------------------------------------------------
 * Constants Definition
 *----------------------------------------------------------------------------*/

/** Size max of Answer To Reset */
#define ATR_SIZE_MAX            55

/** NULL byte to restart byte procedure */
#define ISO_NULL_VAL            0x60

/*------------------------------------------------------------------------------
 *         Exported functions
 *----------------------------------------------------------------------------*/
extern void ISO7816_Init( const Pin pPinIso7816RstMC ) ;
extern void ISO7816_IccPowerOff( void ) ;
extern uint16_t ISO7816_XfrBlockTPDU_T0( const uint8_t *pAPDU, uint8_t *pMessage, uint16_t wLength ) ;
extern void ISO7816_Escape( void ) ;
extern void ISO7816_RestartClock( void ) ;
extern void ISO7816_StopClock( void ) ;
extern void ISO7816_toAPDU( void ) ;
extern void ISO7816_Datablock_ATR( uint8_t* pAtr, uint8_t* pLength ) ;
extern void ISO7816_SetDataRateandClockFrequency( uint32_t dwClockFrequency, uint32_t dwDataRate ) ;
extern uint8_t ISO7816_StatusReset( void ) ;
extern void ISO7816_cold_reset( void ) ;
extern void ISO7816_warm_reset( void ) ;
extern void ISO7816_Decode_ATR( uint8_t* pAtr ) ;

#endif /* _ISO7816_4_ */

