/*
	FreeRTOS.org V5.1.1 - Copyright (C) 2003-2008 Richard Barry.

	This file is part of the FreeRTOS.org distribution.

	FreeRTOS.org is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	FreeRTOS.org is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FreeRTOS.org; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	A special exception to the GPL can be applied should you wish to distribute
	a combined work that includes FreeRTOS.org, without being obliged to provide
	the source code for any proprietary components.  See the licensing section
	of http://www.FreeRTOS.org for full details of how and when the exception
	can be applied.

    ***************************************************************************
    ***************************************************************************
    *                                                                         *
    * SAVE TIME AND MONEY!  We can port FreeRTOS.org to your own hardware,    *
    * and even write all or part of your application on your behalf.          *
    * See http://www.OpenRTOS.com for details of the services we provide to   *
    * expedite your project.                                                  *
    *                                                                         *
    ***************************************************************************
    ***************************************************************************

	Please ensure to read the configuration and relevant port sections of the
	online documentation.

	http://www.FreeRTOS.org - Documentation, latest information, license and
	contact details.

	http://www.SafeRTOS.com - A version that is certified for use in safety
	critical systems.

	http://www.OpenRTOS.com - Commercial support, development, porting,
	licensing and training services.
*/

/*
	Changes from V2.4.0

		+ Made serial ISR handling more complete and robust.

	Changes from V2.4.1

		+ Split serial.c into serial.c and serialISR.c.  serial.c can be
		  compiled using ARM or THUMB modes.  serialISR.c must always be
		  compiled in ARM mode.
		+ Another small change to cSerialPutChar().

	Changed from V2.5.1

		+ In cSerialPutChar() an extra check is made to ensure the post to
		  the queue was successful if then attempting to retrieve the posted
		  character.

*/

/*
	BASIC INTERRUPT DRIVEN SERIAL PORT DRIVER FOR UART0.

	This file contains all the serial port components that can be compiled to
	either ARM or THUMB mode.  Components that must be compiled to ARM mode are
	contained in serialISR.c.
*/

/* Standard includes. */
#include <stdlib.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

/* Demo application includes. */
#include "serial.h"

/*-----------------------------------------------------------*/

/* Constants to setup and access the UART. */
#define serDLAB							( ( unsigned portCHAR ) 0x80 )
#define serENABLE_INTERRUPTS			( ( unsigned portCHAR ) 0x03 )
#define serNO_PARITY					( ( unsigned portCHAR ) 0x00 )
#define ser1_STOP_BIT					( ( unsigned portCHAR ) 0x00 )
#define ser8_BIT_CHARS					( ( unsigned portCHAR ) 0x03 )
#define serFIFO_ON						( ( unsigned portCHAR ) 0x01 )
#define serCLEAR_FIFO					( ( unsigned portCHAR ) 0x06 )
#define serWANTED_CLOCK_SCALING			( ( unsigned portLONG ) 16 )

/* Constants to setup and access the VIC. */
#define serUART0_VIC_CHANNEL			( ( unsigned portLONG ) 0x0006 )
#define serUART0_VIC_CHANNEL_BIT		( ( unsigned portLONG ) 0x0040 )
#define serUART0_VIC_ENABLE				( ( unsigned portLONG ) 0x0020 )
#define serCLEAR_VIC_INTERRUPT			( ( unsigned portLONG ) 0 )

#define serINVALID_QUEUE				( ( xQueueHandle ) 0 )
#define serHANDLE						( ( xComPortHandle ) 1 )
#define serNO_BLOCK						( ( portTickType ) 0 )

/*-----------------------------------------------------------*/

/* Queues used to hold received characters, and characters waiting to be
transmitted. */
static xQueueHandle xRxedChars;
static xQueueHandle xCharsForTx;

/* Communication flag between the interrupt service routine and serial API. */
static volatile portLONG *plTHREEmpty;

/*-----------------------------------------------------------*/

#include <stdarg.h>

#ifdef PAKAI_MMC
char printbuffer[512];
#else
char printbuffer[256];
#endif

#ifdef PAKAI_SERIAL_3 
void ser3_putstring(const signed portCHAR * const pcString);
signed portBASE_TYPE ser3_getchar( xComPortHandle pxPort, signed portCHAR *pcRxedChar, portTickType xBlockTime );
signed portBASE_TYPE xSerialPutChar3( xComPortHandle pxPort, signed portCHAR cOutChar, portTickType xBlockTime );  
#endif

#ifdef PAKAI_SERIAL_1
signed portBASE_TYPE xSerialPutChar1( xComPortHandle pxPort, signed portCHAR cOutChar, portTickType xBlockTime );
#endif

int printf2 (const char *fmt, ...)
{
   va_list args;
   uint i;

   va_start (args, fmt);

   /* For this to work, printbuffer must be larger than
    * anything we ever want to print.
    */
   i = vsprintf (printbuffer, fmt, args);
   va_end (args);

   /* Print the string */
   vSerialPutString(1, printbuffer);
   return 0;
} 



/*
 * The queues are created in serialISR.c as they are used from the ISR.
 * Obtain references to the queues and THRE Empty flag.
 */
extern void vSerialISRCreateQueues(	unsigned portBASE_TYPE uxQueueLength, xQueueHandle *pxRxedChars, xQueueHandle *pxCharsForTx, portLONG volatile **pplTHREEmptyFlag );

/*-----------------------------------------------------------*/

xComPortHandle xSerialPortInitMinimal( unsigned portLONG ulWantedBaud, unsigned portBASE_TYPE uxQueueLength )	{
unsigned portLONG ulDivisor, ulWantedClock;
xComPortHandle xReturn = serHANDLE;
extern void ( vUART_ISR_Wrapper )( void );

	/* The queues are used in the serial ISR routine, so are created from
	serialISR.c (which is always compiled to ARM mode. */
	vSerialISRCreateQueues( uxQueueLength, &xRxedChars, &xCharsForTx, &plTHREEmpty );

	if(
		( xRxedChars != serINVALID_QUEUE ) &&
		( xCharsForTx != serINVALID_QUEUE ) &&
		( ulWantedBaud != ( unsigned portLONG ) 0 )
	  )
	{
		portENTER_CRITICAL();
		{
			/* Setup the baud rate:  Calculate the divisor value. */
			ulWantedClock = ulWantedBaud * serWANTED_CLOCK_SCALING;
			ulDivisor = configCPU_CLOCK_HZ / ulWantedClock;

			/* Set the DLAB bit so we can access the divisor. */
			UART0_LCR |= serDLAB;

			/* Setup the divisor. */
			UART0_DLL = ( unsigned portCHAR ) ( ulDivisor & ( unsigned portLONG ) 0xff );
			ulDivisor >>= 8;
			UART0_DLM = ( unsigned portCHAR ) ( ulDivisor & ( unsigned portLONG ) 0xff );

			/* Turn on the FIFO's and clear the buffers. */
			UART0_FCR = ( serFIFO_ON | serCLEAR_FIFO );

			/* Setup transmission format. */
			UART0_LCR = serNO_PARITY | ser1_STOP_BIT | ser8_BIT_CHARS;

			/* Setup the VIC for the UART. */
			VICIntSelect &= ~( serUART0_VIC_CHANNEL_BIT );
			VICIntEnable |= serUART0_VIC_CHANNEL_BIT;
			VICVectAddr6 = ( portLONG ) vUART_ISR_Wrapper;
			VICVectCntl6 = serUART0_VIC_CHANNEL | serUART0_VIC_ENABLE;

			/* Enable UART0 interrupts. */
			UART0_IER |= serENABLE_INTERRUPTS;
		}
		portEXIT_CRITICAL();
	}
	else
	{
		xReturn = ( xComPortHandle ) 0;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialGetChar( xComPortHandle pxPort, signed portCHAR *pcRxedChar, portTickType xBlockTime )
{
	/* The port handle is not required as this driver only supports UART0. */
	( void ) pxPort;

	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
	if( xQueueReceive( xRxedChars, pcRxedChar, xBlockTime ) )
	{
		return pdTRUE;
	}
	else
	{
		return pdFALSE;
	}
}
/*-----------------------------------------------------------*/

void vSerialPutString( xComPortHandle pxPort, const signed portCHAR * const pcString)
{
signed portCHAR *pxNext;

	/* NOTE: This implementation does not handle the queue being full as no
	block time is used! */

	/* The port handle is not required as this driver only supports UART0. */
	( void ) pxPort;
	//( void ) usStringLength;

	/* Send each character in the string, one at a time. */
	pxNext = ( signed portCHAR * ) pcString;
	while( *pxNext )
	{		
		//xSerialPutChar( pxPort, *pxNext, serNO_BLOCK );
		
		xSerialPutChar( pxPort, *pxNext, 10000 );	// 100 OK
		pxNext++;
	}
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialPutChar( xComPortHandle pxPort, signed portCHAR cOutChar, portTickType xBlockTime )
{
signed portBASE_TYPE xReturn;

	/* This demo driver only supports one port so the parameter is not used. */
	( void ) pxPort;
	
	#ifdef PAKE_TELNETD
	telnetdPutChar(cOutChar);
	#endif
	
	portENTER_CRITICAL();
	{
		/* Is there space to write directly to the UART? */
		if( *plTHREEmpty == ( portLONG ) pdTRUE )
		{
			//* We wrote the character directly to the UART, so was
			//successful. 
			*plTHREEmpty = pdFALSE;
			UART0_THR = cOutChar;
			xReturn = pdPASS;
		}
		else
		{
			/* We cannot write directly to the UART, so queue the character.
			Block for a maximum of xBlockTime if there is no space in the
			queue. */
			xReturn = xQueueSend( xCharsForTx, &cOutChar, xBlockTime );

			/* Depending on queue sizing and task prioritisation:  While we
			were blocked waiting to post interrupts were not disabled.  It is
			possible that the serial ISR has emptied the Tx queue, in which
			case we need to start the Tx off again. */
			if( ( *plTHREEmpty == ( portLONG ) pdTRUE ) && ( xReturn == pdPASS ) )
			{
				xQueueReceive( xCharsForTx, &cOutChar, serNO_BLOCK );
				*plTHREEmpty = pdFALSE;
				UART0_THR = cOutChar;
			}
		}
	}
	portEXIT_CRITICAL();
	
	return xReturn;
}
/*-----------------------------------------------------------*/

void vSerialClose( xComPortHandle xPort )
{
	/* Not supported as not required by the demo application. */
	( void ) xPort;
}
/*-----------------------------------------------------------*/


void uart0GetRxQueue (xQueueHandle *qh)	{
  	*qh = xRxedChars;
}

//*

void serX_putstring(int no, const signed portCHAR * const pcString) {
	#ifdef PAKAI_SERIAL_1
	if (no==1) {
		ser1_putstring(pcString);
	}
	#endif
	
	#ifdef PAKAI_SERIAL_2
	if (no==2) {
		ser2_putstring(pcString);
	}
	#endif
	
	#ifdef PAKAI_SERIAL_3
	if (no==3) {
		//printf("masuk serial.c 3 !\r\n");
		ser3_putstring(pcString);
	}
	#endif
}

void serX_putchar(int no, signed portCHAR * pxNext, portTickType xBlockTime) {
	//*
	#ifdef PAKAI_SERIAL_1
	if (no==1) {
		//ser1_putstring(pcString);
		xSerialPutChar1( 1, *pxNext, xBlockTime );
	}
	#endif
	//*/
	
	#ifdef PAKAI_SERIAL_2
	if (no==2) {
		//ser2_putstring(pcString);
		xSerialPutChar2( 1, *pxNext, xBlockTime );
	}
	#endif
	
	#ifdef PAKAI_SERIAL_3
	if (no==3) {
		//printf("masuk serial.c 3 !\r\n");
		//ser3_putstring(pcString);
		xSerialPutChar3( 1, *pxNext, xBlockTime );
	}
	#endif
}


void serX_getchar(int no, signed portCHAR *pcRxedChar, portTickType xBlockTime ) {
//	ser2_getchar( xComPortHandle pxPort, signed portCHAR *pcRxedChar, portTickType xBlockTime )	
	#ifdef PAKAI_SERIAL_2
	//printf("%d\r\n", PAKAI_PM);
	if (no==2) {
		ser2_getchar(1, *pcRxedChar, xBlockTime );
		//ser2_getchar(1, &buf_rx[i], 100 )
	}
	#endif
	
	#ifdef PAKAI_SERIAL_3
	if (no==3) {
		ser3_getchar(1, *pcRxedChar, xBlockTime );
	}
	#endif
}


#ifdef PAKAI_SERIAL_3
/* UART 3 */
static xQueueHandle Qrx3;
static xQueueHandle Qtx3;
static volatile portLONG *plTHREEmpty3;

#define serUART3_VIC_CHANNEL			( ( unsigned portLONG ) 0x0006 )
#define serUART3_VIC_CHANNEL_BIT		( ( unsigned portLONG ) 0x0040 )
#define serUART3_VIC_ENABLE				( ( unsigned portLONG ) 0x0020 )
//#define serCLEAR_VIC_INTERRUPT			( ( unsigned portLONG ) 0 )

xComPortHandle serial3_init( unsigned portLONG ulWantedBaud, unsigned portBASE_TYPE uxQueueLength )
{
	unsigned portLONG ulDivisor, ulWantedClock;
	xComPortHandle xReturn = serHANDLE;
	extern void ( vUART3_ISR_Wrapper )( void );

	/* The queues are used in the serial ISR routine, so are created from
	serialISR.c (which is always compiled to ARM mode. */
	vSerialISRCreateQueues3( uxQueueLength, &Qrx3, &Qtx3, &plTHREEmpty3 );

	if(
		( Qrx3 != serINVALID_QUEUE ) &&
		( Qtx3 != serINVALID_QUEUE ) &&
		( ulWantedBaud != ( unsigned portLONG ) 0 )
	  )
	{
		portENTER_CRITICAL();
		{
			/* Setup the baud rate:  Calculate the divisor value. */
			ulWantedClock = ulWantedBaud * serWANTED_CLOCK_SCALING;
			ulDivisor = configCPU_CLOCK_HZ / ulWantedClock;

			/* Set the DLAB bit so we can access the divisor. */
			U3LCR |= serDLAB;

			/* Setup the divisor. */
			U3DLL = ( unsigned portCHAR ) ( ulDivisor & ( unsigned portLONG ) 0xff );
			ulDivisor >>= 8;
			U3DLM = ( unsigned portCHAR ) ( ulDivisor & ( unsigned portLONG ) 0xff );

			/* Turn on the FIFO's and clear the buffers. */
			U3FCR = ( serFIFO_ON | serCLEAR_FIFO );

			/* Setup transmission format. */
			U3LCR = serNO_PARITY | ser1_STOP_BIT | ser8_BIT_CHARS;

//#define serUART0_VIC_CHANNEL			( ( unsigned portLONG ) 0x0006 )
//#define serUART0_VIC_CHANNEL_BIT		( ( unsigned portLONG ) 0x0040 )
//#define serUART0_VIC_ENABLE				( ( unsigned portLONG ) 0x0020 )

			/* Setup the VIC for the UART. */
			//VICIntSelect &= ~( serUART0_VIC_CHANNEL_BIT );
			
			#if 1
			VICIntSelect &= ~BIT(29);
			VICVectAddr29 = ( portLONG ) vUART3_ISR_Wrapper;
			//VICVectCntl29 = serUART3_VIC_CHANNEL | serUART3_VIC_ENABLE;
			//VICVectCntl29 = (29 | 10);	// priority
			VICVectCntl29 = (serUART3_VIC_ENABLE | 29);
			
			//VICVectPriority29 = 10;
			//VICSWPrioMask =	10; 
			/* Enable UART0 interrupts. */
			U3IER |= serENABLE_INTERRUPTS;
			
			VICIntEnable |= BIT(29);
			#endif
			
		}
		portEXIT_CRITICAL();
	}
	else
	{
		printf(" Tidak bisa init !!");
		xReturn = ( xComPortHandle ) 0;
	}

	return xReturn;
}

signed portBASE_TYPE ser3_getchar( xComPortHandle pxPort, signed portCHAR *pcRxedChar, portTickType xBlockTime )
{
	/* The port handle is not required as this driver only supports UART0. */
	( void ) pxPort;

	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
	
	if( xQueueReceive( Qrx3, pcRxedChar, xBlockTime ) )
	{
		return pdTRUE;
	}
	else
	{
		return pdFALSE;
	}
}

signed portBASE_TYPE xSerialPutChar3( xComPortHandle pxPort, signed portCHAR cOutChar, portTickType xBlockTime );

void ser3_putstring(const signed portCHAR * const pcString)
{
	signed portCHAR *pxNext;

	/* Send each character in the string, one at a time. */
	pxNext = ( signed portCHAR * ) pcString;
	while( *pxNext )
	{		
		xSerialPutChar3( 1, *pxNext, 100 );	// 100 OK
		pxNext++;
	}
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialPutChar3( xComPortHandle pxPort, signed portCHAR cOutChar, portTickType xBlockTime )
{
signed portBASE_TYPE xReturn;

	/* This demo driver only supports one port so the parameter is not used. */
	( void ) pxPort;
	
	portENTER_CRITICAL();
	{
		/* Is there space to write directly to the UART? */
		if( *plTHREEmpty3 == ( portLONG ) pdTRUE )
		{
			//* We wrote the character directly to the UART, so was
			//successful. 
			*plTHREEmpty3 = pdFALSE;
			U3THR = cOutChar;
			xReturn = pdPASS;
		}
		else
		{
			/* We cannot write directly to the UART, so queue the character.
			Block for a maximum of xBlockTime if there is no space in the
			queue. */
			xReturn = xQueueSend( Qtx3, &cOutChar, xBlockTime );

			/* Depending on queue sizing and task prioritisation:  While we
			were blocked waiting to post interrupts were not disabled.  It is
			possible that the serial ISR has emptied the Tx queue, in which
			case we need to start the Tx off again. */
			if( ( *plTHREEmpty3 == ( portLONG ) pdTRUE ) && ( xReturn == pdPASS ) )
			{
				xQueueReceive( Qtx3, &cOutChar, serNO_BLOCK );
				*plTHREEmpty3 = pdFALSE;
				U3THR = cOutChar;
			}
		}
	}
	portEXIT_CRITICAL();	
	return xReturn;
}#endif

#ifdef PAKAI_SERIAL_2
/* UART 2, untuk PM server */
static xQueueHandle Qrx2;
static xQueueHandle Qtx2;
static volatile portLONG *plTHREEmpty2;

#define serUART2_VIC_CHANNEL			( ( unsigned portLONG ) 0x0006 )
#define serUART2_VIC_CHANNEL_BIT		( ( unsigned portLONG ) 0x0040 )
#define serUART2_VIC_ENABLE				( ( unsigned portLONG ) 0x0020 )
//#define serCLEAR_VIC_INTERRUPT			( ( unsigned portLONG ) 0 )

xComPortHandle serial2_init( unsigned portLONG ulWantedBaud, unsigned portBASE_TYPE uxQueueLength )	{
	unsigned portLONG ulDivisor, ulWantedClock;
	xComPortHandle xReturn = serHANDLE;
	extern void ( vUART2_ISR_Wrapper )( void );
	
	//printf("%s(): ", __FUNCTION__);
	
	/* The queues are used in the serial ISR routine, so are created from
	serialISR.c (which is always compiled to ARM mode. */
	vSerialISRCreateQueues2( uxQueueLength, &Qrx2, &Qtx2, &plTHREEmpty2 );

	if(
		( Qrx2 != serINVALID_QUEUE ) &&
		( Qtx2 != serINVALID_QUEUE ) &&
		( ulWantedBaud != ( unsigned portLONG ) 0 )
	  )
	{
		//printf(" buf OK\r\n");
		portENTER_CRITICAL();
		{
			/* Setup the baud rate:  Calculate the divisor value. */
			ulWantedClock = ulWantedBaud * serWANTED_CLOCK_SCALING;
			ulDivisor = configCPU_CLOCK_HZ / ulWantedClock;
			/* Set the DLAB bit so we can access the divisor. */
			U2LCR |= serDLAB;
			/* Setup the divisor. */
			U2DLL = ( unsigned portCHAR ) ( ulDivisor & ( unsigned portLONG ) 0xff );
			ulDivisor >>= 8;
			U2DLM = ( unsigned portCHAR ) ( ulDivisor & ( unsigned portLONG ) 0xff );
			/* Turn on the FIFO's and clear the buffers. */
			U2FCR = ( serFIFO_ON | serCLEAR_FIFO );
			/* Setup transmission format. */
			U2LCR = serNO_PARITY | ser1_STOP_BIT | ser8_BIT_CHARS;
			//#define serUART0_VIC_CHANNEL			( ( unsigned portLONG ) 0x0006 )
			//#define serUART0_VIC_CHANNEL_BIT		( ( unsigned portLONG ) 0x0040 )
			//#define serUART0_VIC_ENABLE				( ( unsigned portLONG ) 0x0020 )

			/* Setup the VIC for the UART. */
			//VICIntSelect &= ~( serUART0_VIC_CHANNEL_BIT );
			
			#if 1
			VICIntSelect &= ~BIT(28);
			VICVectAddr28 = ( portLONG ) vUART2_ISR_Wrapper;
			VICVectCntl28 = (serUART0_VIC_ENABLE | 28);
		
			/* Enable UART2 interrupts. */
			U2IER |= serENABLE_INTERRUPTS;
			
			VICIntEnable |= BIT(28);
			#endif
			
		}
		portEXIT_CRITICAL();
	}
	else
	{
		//printf(" Tidak bisa init !!\r\n");
		xReturn = ( xComPortHandle ) 0;
	}
	return xReturn;
}

signed portBASE_TYPE ser2_getchar( xComPortHandle pxPort, signed portCHAR *pcRxedChar, portTickType xBlockTime )
{
	/* The port handle is not required as this driver only supports UART0. */
	( void ) pxPort;
	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
	if( xQueueReceive( Qrx2, pcRxedChar, xBlockTime ) )
	{
		return pdTRUE;
	}
	else
	{
		return pdFALSE;
	}
}

signed portBASE_TYPE xSerialPutChar2( xComPortHandle pxPort, signed portCHAR cOutChar, portTickType xBlockTime );





void ser2_putstring(const signed portCHAR * const pcString)
{
	signed portCHAR *pxNext;
	/* Send each character in the string, one at a time. */
	pxNext = ( signed portCHAR * ) pcString;
	while( *pxNext )
	{		
		xSerialPutChar2( 1, *pxNext, 1000 );	// 100 OK
		pxNext++;
	}
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialPutChar2( xComPortHandle pxPort, signed portCHAR cOutChar, portTickType xBlockTime )
{
	signed portBASE_TYPE xReturn;
	/* This demo driver only supports one port so the parameter is not used. */
	( void ) pxPort;
	
	portENTER_CRITICAL();
	{
		/* Is there space to write directly to the UART? */
		if( *plTHREEmpty2 == ( portLONG ) pdTRUE )
		{
			//printf2(": kosong\r\n");
			
			//* We wrote the character directly to the UART, so was
			//successful. 
			*plTHREEmpty2 = pdFALSE;
			U2THR = cOutChar;
			xReturn = pdPASS;
		}
		else
		{
			//printf2("%s(): aktif\r\n", __FUNCTION__);
			/* We cannot write directly to the UART, so queue the character.
			Block for a maximum of xBlockTime if there is no space in the
			queue. */
			xReturn = xQueueSend( Qtx2, &cOutChar, xBlockTime );
			
			/*
			if (xReturn == pdPASS)
				printf2("%s(): bs masuk\r\n", __FUNCTION__);
			else
				printf2("%s(): masih penuh\r\n", __FUNCTION__);
			*/
			
			#if 0
			/* Depending on queue sizing and task prioritisation:  While we
			were blocked waiting to post interrupts were not disabled.  It is
			possible that the serial ISR has emptied the Tx queue, in which
			case we need to start the Tx off again. */
			if( ( *plTHREEmpty2 == ( portLONG ) pdTRUE ) && ( xReturn == pdPASS ) )
			{
				xQueueReceive( Qtx2, &cOutChar, serNO_BLOCK );
				*plTHREEmpty2 = pdFALSE;
				U2THR = cOutChar;
			}
			#endif
		}
	}
	portEXIT_CRITICAL();	
	return xReturn;
}#endif

#ifdef PAKAI_SERIAL_1
/* UART 1 */
static xQueueHandle Qrx1;
static xQueueHandle Qtx1;
static volatile portLONG *plTHREEmpty1;

#define serUART1_VIC_CHANNEL			( ( unsigned portLONG ) 0x0006 )
#define serUART1_VIC_CHANNEL_BIT		( ( unsigned portLONG ) 0x0040 )
#define serUART1_VIC_ENABLE				( ( unsigned portLONG ) 0x0020 )
//#define serCLEAR_VIC_INTERRUPT			( ( unsigned portLONG ) 0 )

xComPortHandle serial1_init( unsigned portLONG ulWantedBaud, unsigned portBASE_TYPE uxQueueLength )
{
	unsigned portLONG ulDivisor, ulWantedClock;
	xComPortHandle xReturn = serHANDLE;
	extern void ( vUART1_ISR_Wrapper )( void );
	
	//printf("%s(): ", __FUNCTION__);
	
	/* The queues are used in the serial ISR routine, so are created from
	serialISR.c (which is always compiled to ARM mode. */
	vSerialISRCreateQueues1( uxQueueLength, &Qrx1, &Qtx1, &plTHREEmpty1 );

	if(
		( Qrx1 != serINVALID_QUEUE ) &&
		( Qtx1 != serINVALID_QUEUE ) &&
		( ulWantedBaud != ( unsigned portLONG ) 0 )
	  )
	{
		printf(" serial1 .................................OK\r\n");
		portENTER_CRITICAL();
		{
			/* Setup the baud rate:  Calculate the divisor value. */
			ulWantedClock = ulWantedBaud * serWANTED_CLOCK_SCALING;
			ulDivisor = configCPU_CLOCK_HZ / ulWantedClock;
			/* Set the DLAB bit so we can access the divisor. */
			U1LCR |= serDLAB;
			/* Setup the divisor. */
			U1DLL = ( unsigned portCHAR ) ( ulDivisor & ( unsigned portLONG ) 0xff );
			ulDivisor >>= 8;
			U1DLM = ( unsigned portCHAR ) ( ulDivisor & ( unsigned portLONG ) 0xff );
			/* Turn on the FIFO's and clear the buffers. */
			U1FCR = ( serFIFO_ON | serCLEAR_FIFO );
			/* Setup transmission format. */
			U1LCR = serNO_PARITY | ser1_STOP_BIT | ser8_BIT_CHARS;
			//#define serUART0_VIC_CHANNEL			( ( unsigned portLONG ) 0x0006 )
			//#define serUART0_VIC_CHANNEL_BIT		( ( unsigned portLONG ) 0x0040 )
			//#define serUART0_VIC_ENABLE				( ( unsigned portLONG ) 0x0020 )

			/* Setup the VIC for the UART. */
			//VICIntSelect &= ~( serUART0_VIC_CHANNEL_BIT );
			
			#if 1
			VICIntSelect &= ~BIT(7);
			VICVectAddr7 = ( portLONG ) vUART1_ISR_Wrapper;
			VICVectCntl7 = (serUART1_VIC_ENABLE | 7);
		
			/* Enable UART1 interrupts. */
			U1IER |= serENABLE_INTERRUPTS;
			
			VICIntEnable |= BIT(7);
			#endif
			
		}
		portEXIT_CRITICAL();
	}
	else
	{
		printf(" Tidak bisa init !!\r\n");
		xReturn = ( xComPortHandle ) 0;
	}
	return xReturn;
}

signed portBASE_TYPE ser1_getchar( xComPortHandle pxPort, signed portCHAR *pcRxedChar, portTickType xBlockTime )
{
	/* The port handle is not required as this driver only supports UART0. */
	( void ) pxPort;
	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
	if( xQueueReceive( Qrx1, pcRxedChar, xBlockTime ) )
	{
		return pdTRUE;
	}
	else
	{
		return pdFALSE;
	}
}




void ser1_putstring(const signed portCHAR * const pcString)
{
	signed portCHAR *pxNext;
	/* Send each character in the string, one at a time. */
	pxNext = ( signed portCHAR * ) pcString;
	while( *pxNext )
	{		
		xSerialPutChar1( 1, *pxNext, 1000 );	// 100 OK
		pxNext++;
	}
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialPutChar1( xComPortHandle pxPort, signed portCHAR cOutChar, portTickType xBlockTime )
{
	signed portBASE_TYPE xReturn;
	/* This demo driver only supports one port so the parameter is not used. */
	( void ) pxPort;
	
	portENTER_CRITICAL();
	{
		/* Is there space to write directly to the UART? */
		if( *plTHREEmpty1 == ( portLONG ) pdTRUE )
		{
			//printf(":...................................... kosong\r\n");
			
			//* We wrote the character directly to the UART, so was
			//successful. 
			*plTHREEmpty1 = pdFALSE;
			U1THR = cOutChar;
			xReturn = pdPASS;
		}
		else
		{
			//printf("%s(): ....................................aktif\r\n", __FUNCTION__);
			/* We cannot write directly to the UART, so queue the character.
			Block for a maximum of xBlockTime if there is no space in the
			queue. */
			xReturn = xQueueSend( Qtx1, &cOutChar, xBlockTime );
			
			/*
			if (xReturn == pdPASS)
				printf("%s(): .................................bs masuk\r\n", __FUNCTION__);
			else
				printf("%s(): ...................masih penuh\r\n", __FUNCTION__);
			//*/
			
			#if 0
			/* Depending on queue sizing and task prioritisation:  While we
			were blocked waiting to post interrupts were not disabled.  It is
			possible that the serial ISR has emptied the Tx queue, in which
			case we need to start the Tx off again. */
			if( ( *plTHREEmpty1== ( portLONG ) pdTRUE ) && ( xReturn == pdPASS ) )
			{
				xQueueReceive( Qtx1, &cOutChar, serNO_BLOCK );
				*plTHREEmpty1 = pdFALSE;
				U1THR = cOutChar;
			}
			#endif
		}
	}
	portEXIT_CRITICAL();	
	return xReturn;
}
#endif
