/*
	interface ke SPI MMC blackfin
	dengan fatfs
	
	furkan jadid, daun biru engineering
	27 April 2009
	
	21 Juli 2009
	copy ke atinom
	
	11 Maret 2010
	diberi efek blink saat nulis ke flash
	
	*/
//#include "../penting.h"
#define SIMULATOR	0

#if (SIMULATOR == 0)
#include "FreeRTOS.h"
#endif

#include <stdio.h> 
//#include "../mmc/mmc.h"
#include "diskio.h"
#include <time.h>
#include "sysdefs.h"

#define no_printf(...)	do {} while(0)

//#define printf_fs	printf
#define printf_fs	no_printf
//#define printf_fs	debug_out

#define S_MAX_SIZ 512
static volatile DSTATUS gDiskStatus = DSTATUS_NOINIT; 
static mediaStatus_t mediaStatus;

static int ledh=0;

#define BLINK_LED	if ( ledh == 1 ) 		\
					{						\
						FIO1CLR = BIT(14);	\
						ledh = 0;			\
					}						\
					else 					\
					{						\
						FIO1SET = BIT(14);	\
						ledh = 1;			\
					}						\


static int init_mmc_awal(void)
{
	int loop;
	
	//init_gpio_mmc();
	//uncs_mmc();
	
	printf("Init SPI/MMC .....");
	//init_spi(300);
	init_spi_mmc(1); // low speed
	printf("... spi .. OK\r\n");
	vTaskDelay(10);
	loop = tes_mmc_awal();
	if (loop != 0)
	{
		printf("...");
	   //	init_spi(20000);
		init_spi_mmc(2);	// high speed
		loop = init_mmc();
		if (loop > 100 || loop == 0) 
		{	
			printf("MMC .. failed");
			
			return -1;
		}
		else
		{
			printf("MMC .. OK");
			get_cid_csd();
			
			return 0;
		}
	}
	else
	{
		printf("... failed init MMC");
		
		return -1;
	}
}

DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{
	int ret;
	ret = init_mmc_awal();
	
	if (ret == 0)
	{
		mediaStatus.statusCode = DSC_COMMANDPASS;
        mediaStatus.mediaChanged = TRUE;
        gDiskStatus = 0;
        
        //printf("DISK OK\r\n");
        #if 0
        if (mmcWriteProtect ())
        {
          mediaStatus.writeProtect = TRUE;
          gDiskStatus |= DSTATUS_PROTECT;
        }
        #endif
	}
	else if (ret < 0)
	{
		 mediaStatus.statusCode = DSC_NOTREADY;
        gDiskStatus = DSTATUS_NOINIT;
	}
	else
	{
		mediaStatus.statusCode = DSC_NOTPRESENT;
        gDiskStatus = DSTATUS_NODISK;
	}
	
	return gDiskStatus;
}

DSTATUS diskShutdown (void)
{
  gDiskStatus |= DSTATUS_NOINIT;

  return gDiskStatus;
}

DSTATUS disk_status (U8 drv __attribute__ ((unused)))
{
  return gDiskStatus;
}

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
	U32 res;
	int i;
	 
	//if (count > 1) printf("%s(): read %d sektor !\r\n", __FUNCTION__, count);
	
	sector = sector * 512;
	
	for (i=0; i<count; i++)
	{
		portENTER_CRITICAL();
		baca_mmc_blok(sector + (i * 512), &buff[ i * 512] );
		portEXIT_CRITICAL();
	}
	BLINK_LED;
	
	return DRESULT_OK;
}

DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
  	U32 res;
  	int i;
  	
  	//if (count > 1) printf(" write lebih dari satu !\r\n");
  	//if (count > 1) debug_out_h("write %d sektor", count);
  	
  	sector = sector * 512;  	
  	
  	for (i=0; i<count; i++)
	{
  		portENTER_CRITICAL();
		tulis_mmc_blok( sector + (i*512) , (unsigned char *) &buff[i * 512]);
		portEXIT_CRITICAL();
	}
	BLINK_LED;
	
	return DRESULT_OK;
}

//
//  Return 1 if MMC card present, 0 if not
//
BOOL diskPresent (void)
{
  //return mmcCardPresent ();
  return 1;
}

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	
	res = DRESULT_ERROR;
	printf_fs(" diskIOCTL()");
	 
	switch (ctrl) 
  	{
    	case IOCTL_GET_SECTOR_COUNT :
      	{
      		printf_fs(" IOCTL_GET_SECTOR_COUNT\r\n");
      		
      		*(U32 *) buff = get_sector_count();
      		
      		res = DRESULT_OK;	
      	}
      	break;
      	
      	case IOCTL_GET_SECTOR_SIZE :
      	{
      		printf_fs(" IOCTL_GET_SECTOR_SIZE\r\n");
      		
      		*(U16 *) buff = 512;
      		res = DRESULT_OK;
      	}
      	break;
      	
      	case IOCTL_CTRL_SYNC :
      	{
      		printf_fs(" IOCTL_CTRL_SYNC\r\n");
      		
      		res = DRESULT_OK;
      	}
      	break;
      	
      	case IOCTL_MMC_GET_CSD :
      	{
        	printf_fs(" IOCTL_MMC_GET_CSD\r\n");
        	copy_csd(buff);
        	
        	res = DRESULT_OK;
        }
      	break;
		
		case IOCTL_MMC_GET_CID :
  	    {
			printf_fs(" IOCTL_MMC_GET_CID\r\n");
			copy_cid(buff);
			
			res = DRESULT_OK;
	    }
      	break;

# if 0
    case IOCTL_CTRL_POWER:
    case IOCTL_CTRL_LOCK:
    case IOCTL_CTRL_EJECT:
    case IOCTL_MMC_GET_OCR:     /* Receive OCR as an R3 resp (4 bytes) */
    case IOCTL_ATA_GET_REV:
    case IOCTL_ATA_GET_MODEL:
    case IOCTL_ATA_GET_SN:
#endif                        

    	default:    	
      		res = DRESULT_PARERR;
    } 
    
    printf_fs(" default\r\n");
    return res;
}


