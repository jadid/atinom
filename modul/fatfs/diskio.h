/*-----------------------------------------------------------------------
/  Low level disk interface modlue include file  R0.07   (C)ChaN, 2009
/-----------------------------------------------------------------------*/

#ifndef _DISKIO

#define _READONLY	0	/* 1: Read-only mode */
#define _USE_IOCTL	1

#include "integer.h"

typedef unsigned int BOOL;
#define FALSE 0
#define TRUE 1

/* Status of Disk Functions */
typedef BYTE	DSTATUS;

/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,		/* 0: Successful */
	RES_ERROR,		/* 1: R/W Error */
	RES_WRPRT,		/* 2: Write Protected */
	RES_NOTRDY,		/* 3: Not Ready */
	RES_PARERR		/* 4: Invalid Parameter */
} DRESULT22;

//
//  Results of Disk Functions 
//
typedef enum 
{
	DRESULT_OK = 0,		/* 0: Successful */
	DRESULT_ERROR,		/* 1: R/W Error */
	DRESULT_WRPRT,		/* 2: Write Protected */
	DRESULT_NOTRDY,		/* 3: Not Ready */
	DRESULT_PARERR		/* 4: Invalid Parameter */
} 
DRESULT;

typedef enum
{
  DSC_COMMANDPASS = 0, 
  DSC_NOTREADY, 
  DSC_NOTPRESENT,
  DSC_PARAMETERSERROR, 
  DSC_MISCOMPAREERROR, 
  DSC_DISKCHANGED,
  DSC_UNKNOWNERROR,
} 
diskStatusCode_e;

//
//  Disk Status Bits (DSTATUS)
//
#define DSTATUS_NOINIT		0x01	/* Drive not initialized */
#define DSTATUS_NODISK		0x02	/* No media in the drive */
#define DSTATUS_PROTECT		0x04	/* Write protected */


/*---------------------------------------*/
/* Prototypes for disk control functions */

BOOL assign_drives (int argc, char *argv[]);
DSTATUS disk_initialize (BYTE);
DSTATUS disk_status (BYTE);
DRESULT disk_read (BYTE, BYTE*, DWORD, BYTE);
#if	_READONLY == 0
DRESULT disk_write (BYTE, const BYTE*, DWORD, BYTE);
#endif
DRESULT disk_ioctl (BYTE, BYTE, void*);



/* Disk Status Bits (DSTATUS) */

#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */
#define STA_PROTECT		0x04	/* Write protected */


//
//  Command code for disk_ioctl() 
//
#define IOCTL_GET_SECTOR_COUNT	1
#define IOCTL_GET_SECTOR_SIZE		2
#define IOCTL_CTRL_SYNC			    3
#define IOCTL_CTRL_POWER			  4
#define IOCTL_CTRL_LOCK			    5
#define IOCTL_CTRL_EJECT			  6
#define IOCTL_MMC_GET_CSD			 10
#define IOCTL_MMC_GET_CID			 11
#define IOCTL_MMC_GET_OCR			 12
#define IOCTL_ATA_GET_REV			 20
#define IOCTL_ATA_GET_MODEL		 21
#define IOCTL_ATA_GET_SN			 22

//
//
//
typedef struct
{
  diskStatusCode_e statusCode;
  BOOL writeProtect;
  BOOL mediaChanged;
} 
mediaStatus_t;

/* Command code for disk_ioctrl() */

/* Generic command */
#define CTRL_SYNC			0	/* Mandatory for write functions */
#define GET_SECTOR_COUNT	1	/* Mandatory for only f_mkfs() */
#define GET_SECTOR_SIZE		2
#define GET_BLOCK_SIZE		3	/* Mandatory for only f_mkfs() */
#define CTRL_POWER			4
#define CTRL_LOCK			5
#define CTRL_EJECT			6
/* MMC/SDC command */
#define MMC_GET_TYPE		10
#define MMC_GET_CSD			11
#define MMC_GET_CID			12
#define MMC_GET_OCR			13
#define MMC_GET_SDSTAT		14
/* ATA/CF command */
#define ATA_GET_REV			20
#define ATA_GET_MODEL		21
#define ATA_GET_SN			22


#define _DISKIO
#endif
