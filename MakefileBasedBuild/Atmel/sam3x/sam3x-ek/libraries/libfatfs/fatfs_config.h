/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
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

#ifndef FATFS_CONFIG_H
#define FATFS_CONFIG_H
#include "fatfs/src/integer.h"

/*-----------------------------------------------------------------------*/
/* Correspondence between physical drive number and physical drive.      */
/*-----------------------------------------------------------------------*/

#define DRV_NAND         0
#define DRV_MMC          1
#define DRV_ATA          2
#define DRV_USB          3
#define DRV_SDRAM        4


#define SECTOR_SIZE_DEFAULT 512
#define SECTOR_SIZE_SDRAM  512
#define SECTOR_SIZE_SDCARD 512

/*---------------------------------------------------------------------------/
/  FatFs - FAT file system module configuration file  R0.08  (C)ChaN, 2010
/----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------/
/ FatFs Configuration Options
/
/ CAUTION! Do not forget to make clean the project after any changes to
/ the configuration options.
/
/----------------------------------------------------------------------------*/
#define _FFCONF 8237    /* Revision ID */

/*---------------------------------------------------------------------------/
/ Function and Buffer Configurations
/----------------------------------------------------------------------------*/

#define    _FS_TINY    0        /* 0:Normal or 1:Tiny */
/* When _FS_TINY is set to 1, FatFs uses the sector buffer in the file system
/  object instead of the sector buffer in the individual file object for file
/  data transfer. This reduces memory consumption 512 bytes each file object. */

#if _FS_TINY != 1
#define _FS_READONLY    0    /* 0:Read/Write or 1:Read only */
/* Setting _FS_READONLY to 1 defines read only configuration. This removes
/  writing functions, f_write, f_sync, f_unlink, f_mkdir, f_chmod, f_rename,
/  f_truncate and useless f_getfree. */
#else
#define _FS_READONLY    1
/* Setting _FS_READONLY to 1 defines read only configuration. This removes
/  writing functions, f_write, f_sync, f_unlink, f_mkdir, f_chmod, f_rename,
/  f_truncate and useless f_getfree. */
#endif

#define _FS_MINIMIZE    0    /* 0, 1, 2 or 3 */
/* The _FS_MINIMIZE option defines minimization level to remove some functions.
/
/  0: Full function.
/   1: f_stat, f_getfree, f_unlink, f_mkdir, f_chmod, f_truncate and f_rename
/      are removed.
/  2: f_opendir and f_readdir are removed in addition to level 1.
/  3: f_lseek is removed in addition to level 2. */


#define    _USE_STRFUNC    0    /* 0:Disable or 1/2:Enable */
/* To enable string functions, set _USE_STRFUNC to 1 or 2. */


#define    _USE_MKFS    1        /* 0:Disable or 1:Enable */
/* To enable f_mkfs function, set _USE_MKFS to 1 and set _FS_READONLY to 0 */


#define    _USE_FORWARD    0    /* 0:Disable or 1:Enable */
/* To enable f_forward function, set _USE_FORWARD to 1 and set _FS_TINY to 1. */


#define    _USE_FASTSEEK    0    /* 0:Disable or 1:Enable */
/* To enable fast seek feature, set _USE_FASTSEEK to 1. */



/*---------------------------------------------------------------------------/
/ Locale and Namespace Configurations
/----------------------------------------------------------------------------*/

#define _CODE_PAGE    850
/* The _CODE_PAGE specifies the OEM code page to be used on the target system.
/  Incorrect setting of the code page can cause a file open failure.
/
/   932  - Japanese Shift-JIS (DBCS, OEM, Windows)
/   936  - Simplified Chinese GBK (DBCS, OEM, Windows)
/   949  - Korean (DBCS, OEM, Windows)
/   950  - Traditional Chinese Big5 (DBCS, OEM, Windows)
/   1250 - Central Europe (Windows)
/   1251 - Cyrillic (Windows)
/   1252 - Latin 1 (Windows)
/   1253 - Greek (Windows)
/   1254 - Turkish (Windows)
/   1255 - Hebrew (Windows)
/   1256 - Arabic (Windows)
/   1257 - Baltic (Windows)
/   1258 - Vietnam (OEM, Windows)
/   437  - U.S. (OEM)
/   720  - Arabic (OEM)
/   737  - Greek (OEM)
/   775  - Baltic (OEM)
/   850  - Multilingual Latin 1 (OEM)
/   858  - Multilingual Latin 1 + Euro (OEM)
/   852  - Latin 2 (OEM)
/   855  - Cyrillic (OEM)
/   866  - Russian (OEM)
/   857  - Turkish (OEM)
/   862  - Hebrew (OEM)
/   874  - Thai (OEM, Windows)
/    1    - ASCII only (Valid for non LFN cfg.)
*/


#define    _USE_LFN    2        /* 0 to 3 */
#define    _MAX_LFN    255        /* Maximum LFN length to handle (12 to 255) */
/* The _USE_LFN option switches the LFN support.
/
/   0: Disable LFN. _MAX_LFN and _LFN_UNICODE have no effect.
/   1: Enable LFN with static working buffer on the bss. NOT REENTRANT.
/   2: Enable LFN with dynamic working buffer on the STACK.
/   3: Enable LFN with dynamic working buffer on the HEAP.
/
/  The LFN working buffer occupies (_MAX_LFN + 1) * 2 bytes. When enable LFN,
/  Unicode handling functions ff_convert() and ff_wtoupper() must be added
/  to the project. When enable to use heap, memory control functions
/  ff_memalloc() and ff_memfree() must be added to the project. */


#define    _LFN_UNICODE    0    /* 0:ANSI/OEM or 1:Unicode */
/* To switch the character code set on FatFs API to Unicode,
/  enable LFN feature and set _LFN_UNICODE to 1. */


#define _FS_RPATH    0        /* 0:Disable or 1:Enable */
/* When _FS_RPATH is set to 1, relative path feature is enabled and f_chdir,
/  f_chdrive function are available.
/  Note that output of the f_readdir fnction is affected by this option. */



/*---------------------------------------------------------------------------/
/ Physical Drive Configurations
/----------------------------------------------------------------------------*/

#define _VOLUMES    2
/* Number of volumes (logical drives) to be used. */


#define    _MAX_SS        512        /* 512, 1024, 2048 or 4096 */
/* Maximum sector size to be handled.
/  Always set 512 for memory card and hard disk but a larger value may be
/  required for floppy disk (512/1024) and optical disk (512/2048).
/  When _MAX_SS is larger than 512, GET_SECTOR_SIZE command must be implememted
/  to the disk_ioctl function. */


#define    _MULTI_PARTITION    0    /* 0:Single parition or 1:Multiple partition */
/* When _MULTI_PARTITION is set to 0, each volume is bound to the same physical
/ drive number and can mount only first primaly partition. When it is set to 1,
/ each volume is tied to the partitions listed in Drives[]. */

#define    _USE_ERASE    0    /* 0:Disable or 1:Enable */
/* To enable sector erase feature, set _USE_ERASE to 1. CTRL_ERASE_SECTOR command
/  should be added to the disk_ioctl functio. */


/*---------------------------------------------------------------------------/
/ System Configurations
/----------------------------------------------------------------------------*/

#define _WORD_ACCESS    0    /* 0 or 1 */
/* Set 0 first and it is always compatible with all platforms. The _WORD_ACCESS
/  option defines which access method is used to the word data on the FAT volume.
/
/   0: Byte-by-byte access.
/   1: Word access. Do not choose this unless following condition is met.
/
/  When the byte order on the memory is big-endian or address miss-aligned word
/  access results incorrect behavior, the _WORD_ACCESS must be set to 0.
/  If it is not the case, the value can also be set to 1 to improve the
/  performance and code size. */


#define _FS_REENTRANT    0        /* 0:Disable or 1:Enable */
#define _FS_TIMEOUT        1000    /* Timeout period in unit of time ticks */
#define    _SYNC_t            HANDLE    /* O/S dependent type of sync object. e.g. HANDLE, OS_EVENT*, ID and etc.. */
/* Include a header file here to define O/S system calls */
/* #include <windows.h>, <ucos_ii.h.h>, <semphr.h> or ohters. */

/* The _FS_REENTRANT option switches the reentrancy of the FatFs module.
/
/   0: Disable reentrancy. _SYNC_t and _FS_TIMEOUT have no effect.
/   1: Enable reentrancy. Also user provided synchronization handlers,
/      ff_req_grant, ff_rel_grant, ff_del_syncobj and ff_cre_syncobj
/      function must be added to the project. */


#define    _FS_SHARE    0    /* 0:Disable or >=1:Enable */
/* To enable file shareing feature, set _FS_SHARE to >= 1 and also user
   provided memory handlers, ff_memalloc and ff_memfree function must be
   added to the project. The value defines number of files can be opened
   per volume. */


#include "fatfs/src/diskio.h"
#include "fatfs/src/ff.h"

#endif /* FATFS_CONFIG_H */
