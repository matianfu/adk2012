/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _ADK_H_
#define _ADK_H_

#include <stdint.h>
#include "btL2CAP.h"
#include "btSDP.h"
#include "btRFCOMM.h"

#define NUM_LEDS  64
#define NUM_DIGITS  6
#define NUM_ICONS  8

#define DMA_CHANNEL_LEDS	0	//we use channel 0. you want it? sucks to be you! get an IDE that supports a reasonable build system!

typedef void (*adkPutcharF)(char c);

typedef char (*adkBtConnectionRequestF)(const uint8_t* mac, uint32_t devClass, uint8_t linkType);	//return 1 to accept
typedef char (*adkBtLinkKeyRequestF)(const uint8_t* mac, uint8_t* buf);		//retrieve link key, fill buffer with it, return 1. if no key -> return 0
typedef void (*adkBtLinkKeyCreatedF)(const uint8_t* mac, const uint8_t* buf); 	//link key was just created, save it if you want it later
typedef char (*adkBtPinRequestF)(const uint8_t* mac, uint8_t* buf);		//fill buff with PIN code, return num bytes used (16 max) return 0 to decline
typedef char (*adkBtDiscoveryResultF)(const uint8_t* mac, uint8_t PSRM, uint8_t PSPM, uint8_t PSM, uint16_t CO, uint32_t devClass); //return 0 to stop scan immediately
typedef void (*adkBtSspDisplayF)(const uint8_t* mac, uint32_t val);

#define ADK_BT_SSP_DONE_VAL		0x0FF00000

#define BLUETOOTH_MAC_SIZE		6	//bytes
#define BLUETOOTH_LINK_KEY_SIZE		16	//bytes
#define BLUETOOTH_MAX_PIN_SIZE		16	//bytes
#define BLUETOOTH_MAX_NAME_LEN		248	//bytes
#define ADK_UNIQUE_ID_LEN		4	//4 32-bit values

/* keep in sync with Audio.h */
#define AUDIO_NULL 0
#define AUDIO_USB 1
#define AUDIO_BT  2
#define AUDIO_ALARM 3

#define AUDIO_MAX_SOURCE 3

/*	--- structure(s) and types reference ---

typedef struct{

    uint8_t flags;

    void* (*serviceInstanceAllocate)(uint16_t conn, uint16_t chan, uint16_t remChan);
    void (*serviceInstanceFree)(void* service);

    void (*serviceRx)(void* service, const uint8_t* data, uint16_t size);

}L2capService;


typedef void (*BtRfcommPortOpenF)(void* port, uint8_t dlci);
typedef void (*BtRfcommPortCloseF)(void* port, uint8_t dlci);
typedef void (*BtRfcommPortRxF)(void* port, uint8_t dlci, const uint8_t* buf, uint16_t sz);


*/

struct FatFile;
struct FatDir;
typedef struct FatFile* FatFileP;
typedef struct FatDir* FatDirP;
#define FATFS_READ	1
#define FATFS_WRITE	2
#define FATFS_CREATE	4
#define FATFS_TRUNCATE	8

#define FATFS_START	0
#define FATFS_CUR	1
#define FATFS_END	2

typedef struct{

    uint32_t fsize;
    uint8_t attrib;
    char name[13];	//short name
    char* longName;	//you make this point somewhere
    uint32_t nameSz;

}FatFileInfo;



    //generic
        void ADK_adkInit(void);
	void ADK_adkSetPutchar(adkPutcharF f);
        void ADK_adkEventProcess(void); //call this often
	void ADK_getUniqueId(uint32_t* id);
	uint64_t ADK_getUptime(void);	//in ms

    //LEDS
        //raw LED write
        void ADK_ledWrite(uint8_t led_id, uint8_t r, uint8_t g, uint8_t b);
        //draw an icon
        void ADK_ledDrawIcon(uint8_t icon, uint8_t r, uint8_t g, uint8_t b);
        //draw a letter
	void ADK_ledDrawLetter(char letter, uint8_t val, uint8_t r, uint8_t g, uint8_t b);
        //flush the backbuffer to the display (call often)
        void ADK_ledUpdate(void);
        //on-board debug led
        void ADK_ledDbgState(char on);

    //RAW Audio
	void ADK_audioOn(int source, uint32_t samplerate);
	void ADK_audioOff(int source);
	void ADK_audioAddBuffer(int source, const uint16_t* samples, uint32_t numSamples);	//if buffers full, will block until they arent...
	int ADK_audioTryAddBuffer(int source, const uint16_t* samples, uint32_t numSamples);	//0 if failed

    //OGG Audio	
	void ADK_playOgg(const char* path);
	void ADK_playOggBackground(const char* path, char *complete, char *abort);
	void ADK_setVolume(uint8_t vol);
	uint8_t ADK_getVolume(void);

    //BT
        void ADK_btEnable(adkBtConnectionRequestF crF, adkBtLinkKeyRequestF krF, adkBtLinkKeyCreatedF kcF, adkBtPinRequestF prF, adkBtDiscoveryResultF drF);
	char ADK_btSetLocalName(const char* name);
	char ADK_btGetRemoteName(const uint8_t* mac, uint8_t PSRM, uint8_t PSM, uint16_t co, char* nameBuf);
	void ADK_btScan(void);
	char ADK_btDiscoverable(char on);
	char ADK_btConnectable(char on);
	char ADK_btSetDeviceClass(uint32_t cls);

    //advanced BT
	//ACL
	void ADK_l2capServiceTx(uint16_t conn, uint16_t remChan, const uint8_t* data, uint32_t size); //send data over L2CAP
	void ADK_l2capServiceCloseConn(uint16_t conn, uint16_t chan);
	char ADK_l2capServiceRegister(uint16_t PSM, const L2capService* svcData);
	char ADK_l2capServiceUnregister(uint16_t PSM);

	//SDP
	void ADK_btSdpServiceDescriptorAdd(const uint8_t* descriptor, uint16_t descrLen); //a copy will NOT be made do not include handle
	void ADK_btSdpServiceDescriptorDel(const uint8_t* descriptor);
    
	//RFCOMM
	void ADK_btRfcommRegisterPort(uint8_t dlci, BtRfcommPortOpenF oF, BtRfcommPortCloseF cF, BtRfcommPortRxF rF);
	void ADK_btRfcommPortTx(void* port, uint8_t dlci, const uint8_t* data, uint16_t size); //makes a copy of your buffer
	uint8_t ADK_btRfcommReserveDlci(uint8_t preference);	//return dlci if success, zero if fail
	void ADK_btRfcommReleaseDlci(uint8_t dlci);

        //SSP
        void ADK_btSetSspCallback(adkBtSspDisplayF pdF);


    //Sensors
	char ADK_hygroRead(int32_t *temp, int32_t *humidity);	//return 0 on failure
	void ADK_baroRead(uint8_t oss, long* kPa, long* decicelcius);
	uint8_t ADK_capSenseSlider(void);
	uint16_t ADK_capSenseButtons(void);
	uint16_t ADK_capSenseIcons(void);
	void ADK_capSenseDump(void);
	void ADK_alsRead(uint16_t* prox, uint16_t* clear, uint16_t* R, uint16_t* G, uint16_t* B, uint16_t* IR, uint16_t* temp);
	void ADK_accelRead(int16_t* x, int16_t* y, int16_t* z);
	void ADK_magRead(int16_t* x, int16_t* y, int16_t* z);

    //RTC
	void ADK_rtcGet(uint16_t* yearP, uint8_t* monthP, uint8_t* dayP, uint8_t* hourP, uint8_t* minuteP, uint8_t* secondP);
	void ADK_rtcSet(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

    //FATFS (retrn 0 on success)
	char ADK_fatfsMount(void);								// Mount/Unmount a logical drive
	char ADK_fatfsOpen(struct FatFile**, const char* path, uint8_t mode);			// Open or create a file
	char ADK_fatfsRead(struct FatFile*, void* buf, uint32_t num, uint32_t* numDone);	// Read data from a file
	char ADK_fatfsWrite(struct FatFile*, void* buf, uint32_t num, uint32_t* numDone);	// Write data to a file
	char ADK_fatfsSeek(struct FatFile*, uint8_t whence, int32_t pos);			// Move file pointer of a file object
	char ADK_fatfsClose(struct FatFile*);							// Close an open file object
	char ADK_fatfsTruncate(struct FatFile*);						// Truncate file
	char ADK_fatfsSync(struct FatFile*);							// Flush cached data of a writing file
	
	char ADK_fatfsOpenDir(struct FatDir**, const char* path);				// Open an existing directory
	char ADK_fatfsReadDir(struct FatDir*, FatFileInfo*);					// Read a directory item
	char ADK_fatfsCloseDir(struct FatDir*);							// Close a directory

	char ADK_fatfsStat(const char* path, FatFileInfo*);					// Get file status
	char ADK_fatfsGetFree(const char* path, uint64_t* freeSize);				// Get number of free space on the drive
	char ADK_fatfsUnlink(const char* path);							// Delete an existing file or directory
	char ADK_fatfsMkdir(const char* path);							// Create a new directory
	char ADK_fatfsChmod(const char* path, uint8_t val, uint8_t mask);			// Change attributes of the file/dir
	char ADK_fatfsRename(const char* path, const char* newPath);				// Rename/Move a file or directory
	char ADK_fatfsMkfs(void);								// Create a file system on the drive

    //USB
	void ADK_usbStart(void);
	void ADK_usbSetAccessoryStringVendor(const char *str);
	void ADK_usbSetAccessoryStringName(const char *str);
	void ADK_usbSetAccessoryStringLongname(const char *str);
	void ADK_usbSetAccessoryStringVersion(const char *str);
	void ADK_usbSetAccessoryStringUrl(const char *str);
	void ADK_usbSetAccessoryStringSerial(const char *str);

    //USB accessory
	int ADK_accessoryConnected(void);
	int ADK_accessorySend(const void *buf, unsigned int len);
	int ADK_accessoryReceive(void *buf, unsigned int len);



#endif


