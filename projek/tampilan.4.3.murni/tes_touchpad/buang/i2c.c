/*****************************************************************************
 *   i2c.c:  I2C C file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.19  ver 1.00    Prelimnary version, first Release
 *
*****************************************************************************/
//#include "LPC23xx.h"                        /* LPC23xx/24xx definitions */
//#include "type.h"
//#include "irq.h"
/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "i2c.h"

volatile DWORD I2CMasterState = I2C_IDLE;
volatile DWORD I2CSlaveState = I2C_IDLE;

volatile DWORD I2CCmd;
volatile DWORD I2CMode;

volatile BYTE I2CMasterBuffer[BUFSIZE];
volatile BYTE I2CSlaveBuffer[BUFSIZE];
volatile DWORD I2CCount = 0;
volatile DWORD I2CReadLength;
volatile DWORD I2CWriteLength;

volatile DWORD RdIndex = 0;
volatile DWORD WrIndex = 0;

#define delay(c) vTaskDelay(c)
#define out(s) vSerialPutString(0, s)

/* 
From device to device, the I2C communication protocol may vary, 
in the example below, the protocol uses repeated start to read data from or 
write to the device:
For master read: the sequence is: STA,Addr(W),offset,RE-STA,Addr(r),data...STO 
for master write: the sequence is: STA,Addr(W),length,RE-STA,Addr(w),data...STO
Thus, in state 8, the address is always WRITE. in state 10, the address could 
be READ or WRITE depending on the I2CCmd.
*/   



/*****************************************************************************
** Function name:		I2CStart
**
** Descriptions:		Create I2C start condition, a timeout
**				value is set if the I2C never gets started,
**				and timed out. It's a fatal error. 
**
** parameters:			None
** Returned value:		true or false, return false if timed out
** 
*****************************************************************************/
DWORD I2CStart( void )
{
  DWORD timeout = 0;
  DWORD retVal = FALSE;
 
  /*--- Issue a start condition ---*/
  I20CONSET = I2CONSET_STA;	/* Set Start flag */
    
  /*--- Wait until START transmitted ---*/
  while( 1 )
  {
	if ( I2CMasterState == I2C_STARTED )
	{
	  retVal = TRUE;
	  break;	
	}
	if ( timeout >= 20 )	// 2 detik
	{
	  retVal = FALSE;
	  break;
	}
	out("..");
	delay(100);
	timeout++;
  }
  return( retVal );
}

/*****************************************************************************
** Function name:		I2CStop
**
** Descriptions:		Set the I2C stop condition, if the routine
**				never exit, it's a fatal bus error.
**
** parameters:			None
** Returned value:		true or never return
** 
*****************************************************************************/
DWORD I2CStop( void )
{
  I20CONSET = I2CONSET_STO;      /* Set Stop flag */ 
  I20CONCLR = I2CONCLR_SIC;  /* Clear SI flag */ 
            
  /*--- Wait for STOP detected ---*/
  while( I20CONSET & I2CONSET_STO );
  return TRUE;
}


/*****************************************************************************
** Function name:		I2CInit
**
** Descriptions:		Initialize I2C controller
**
** parameters:			I2c mode is either MASTER or SLAVE
** Returned value:		true or false, return false if the I2C
**				interrupt handler was not installed correctly
** 
*****************************************************************************/
void i2c_Wrapper( void );

DWORD I2CInit( DWORD I2cMode ) 
{
  PCONP |= (1 << 19);
  PINSEL1 &= ~0x03C00000;
  PINSEL1 |= 0x01400000;	/* set PIO0.27 and PIO0.28 to I2C0 SDA and SCK */
							/* function to 01 on both SDA and SCK. */
  /*--- Clear flags ---*/
  I20CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC;    

  /*--- Reset registers ---*/
  I20SCLL   = I2SCLL_SCLL;
  I20SCLH   = I2SCLH_SCLH;
  if ( I2cMode == I2CSLAVE )
  {
	I20ADR = LM75_ADDR;
  }    
	
	#if 0
  /* Install interrupt handler */	
  if ( install_irq( I2C0_INT, (void *)I2C0MasterHandler, HIGHEST_PRIORITY ) == FALSE )
  {
	return( FALSE );
  }
  #endif
  
	/*
			VICIntSelect &= ~( serUART0_VIC_CHANNEL_BIT );
			VICIntEnable |= serUART0_VIC_CHANNEL_BIT;
			VICVectAddr6 = ( portLONG ) vUART_ISR_Wrapper;
			VICVectCntl6 = serUART0_VIC_CHANNEL | serUART0_VIC_ENABLE;
	*/
	
	/* Setup the VIC for the I2C. BIT(9) */
	VICIntSelect &= ~(0x200);
	VICIntEnable |= 0x200;
	VICVectAddr9 = ( portLONG ) i2c_Wrapper;
	VICVectCntl9 = I2C0_INT | 0x20;

	/* Enable UART0 interrupts. */
	//UART0_IER |= serENABLE_INTERRUPTS;
  
  
  I20CONSET = I2CONSET_I2EN;
  return( TRUE );
}

/*****************************************************************************
** Function name:		I2CEngine
**
** Descriptions:		The routine to complete a I2C transaction
**				from start to stop. All the intermitten
**				steps are handled in the interrupt handler.
**				Before this routine is called, the read
**				length, write length, I2C master buffer,
**				and I2C command fields need to be filled.
**				see i2cmst.c for more details. 
**
** parameters:			None
** Returned value:		true or false, return false only if the
**				start condition can never be generated and
**				timed out. 
** 
*****************************************************************************/
DWORD I2CEngine( void ) 
{
  I2CMasterState = I2C_IDLE;
  RdIndex = 0;
  WrIndex = 0;
  out("a.");
  if ( I2CStart() != TRUE )
  {
	out("b.");
	I2CStop();
	out("c.");
	return ( FALSE );
  }
  out("d.");
  while ( 1 )
  {
	out("e.");
	if ( I2CMasterState == DATA_NACK )
	{
	  I2CStop();
	  break;
	}
  }    
  return ( TRUE );      
}

/******************************************************************************
**                            End Of File
******************************************************************************/

int test_ping(void)
{
	I2CWriteLength = 2;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = 0x68;
	I2CMasterBuffer[1] = 0x23;
	I2CMasterBuffer[2] = 0x00;	/* configuration value, no change from 
								default */
	I2CCmd = 0x23;
	return (I2CEngine()); 	
}
