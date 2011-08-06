/*
	i2c handler
	
	*/

#include "FreeRTOS.h"
#include "task.h"
#include "i2c.h"

extern volatile BYTE I2CMasterBuffer[BUFSIZE];
extern volatile BYTE I2CSlaveBuffer[BUFSIZE];
extern volatile DWORD I2CMasterState;
extern volatile DWORD I2CSlaveState;
extern volatile DWORD I2CCmd;
extern volatile DWORD I2CMode;
extern volatile DWORD I2CReadLength;
extern volatile DWORD I2CWriteLength;

extern volatile DWORD RdIndex;
extern volatile DWORD WrIndex;

void i2c_Wrapper( void ) __attribute__ ((naked));

void i2c_Wrapper( void )
{
	/* Save the context of the interrupted task. */
	portSAVE_CONTEXT();

	/* Call the handler.  This must be a separate function from the wrapper
	to ensure the correct stack frame is set up. */
	I2C0MasterHandler();

	/* Restore the context of whichever task is going to run next. */
	portRESTORE_CONTEXT();
}

/*****************************************************************************
** Function name:		I2C0MasterHandler
**
** Descriptions:		I2C0 interrupt handler, deal with master mode
**				only.
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void I2C0MasterHandler(void) 
{
  BYTE StatValue;

	#if 	1
  /* this handler deals with master read and master write only */
  StatValue = I20STAT;
  //IENABLE;				/* handles nested interrupt */	
  switch ( StatValue )
  {
	case 0x08:			/* A Start condition is issued. */
	I20DAT = I2CMasterBuffer[0];
	I20CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	I2CMasterState = I2C_STARTED;
	break;
	
	case 0x10:			/* A repeated started is issued */
	if ( I2CCmd == LM75_TEMP )
	{
	  I20DAT = I2CMasterBuffer[2];
	}
	I20CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	I2CMasterState = I2C_RESTARTED;
	break;
	
	case 0x18:			/* Regardless, it's a ACK */
	if ( I2CMasterState == I2C_STARTED )
	{
	  I20DAT = I2CMasterBuffer[1+WrIndex];
	  WrIndex++;
	  I2CMasterState = DATA_ACK;
	}
	I20CONCLR = I2CONCLR_SIC;
	break;
	
	case 0x28:	/* Data byte has been transmitted, regardless ACK or NACK */
	case 0x30:
	if ( WrIndex != I2CWriteLength )
	{   
	  I20DAT = I2CMasterBuffer[1+WrIndex]; /* this should be the last one */
	  WrIndex++;
	  if ( WrIndex != I2CWriteLength )
	  {   
		I2CMasterState = DATA_ACK;
	  }
	  else
	  {
		I2CMasterState = DATA_NACK;
		if ( I2CReadLength != 0 )
		{
		  I20CONSET = I2CONSET_STA;	/* Set Repeated-start flag */
		  I2CMasterState = I2C_REPEATED_START;
		}
	  }
	}
	else
	{
	  if ( I2CReadLength != 0 )
	  {
		I20CONSET = I2CONSET_STA;	/* Set Repeated-start flag */
		I2CMasterState = I2C_REPEATED_START;
	  }
	  else
	  {
		I2CMasterState = DATA_NACK;
		I20CONSET = I2CONSET_STO;      /* Set Stop flag */
	  }
	}
	I20CONCLR = I2CONCLR_SIC;
	break;
	
	case 0x40:	/* Master Receive, SLA_R has been sent */
	I20CONSET = I2CONSET_AA;	/* assert ACK after data is received */
	I20CONCLR = I2CONCLR_SIC;
	break;
	
	case 0x50:	/* Data byte has been received, regardless following ACK or NACK */
	case 0x58:
	I2CMasterBuffer[3+RdIndex] = I20DAT;
	RdIndex++;
	if ( RdIndex != I2CReadLength )
	{   
	  I2CMasterState = DATA_ACK;
	}
	else
	{
	  RdIndex = 0;
	  I2CMasterState = DATA_NACK;
	}
	I20CONSET = I2CONSET_AA;	/* assert ACK after data is received */
	I20CONCLR = I2CONCLR_SIC;
	break;
	
	case 0x20:		/* regardless, it's a NACK */
	case 0x48:
	I20CONCLR = I2CONCLR_SIC;
	I2CMasterState = DATA_NACK;
	break;
	
	case 0x38:		/* Arbitration lost, in this example, we don't
					deal with multiple master situation */
	default:
	I20CONCLR = I2CONCLR_SIC;	
	break;
  }
  #endif
  
  //IDISABLE;
  VICVectAddr = 0;		/* Acknowledge Interrupt */
}
