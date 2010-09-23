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
	BASIC INTERRUPT DRIVEN SERIAL PORT DRIVER FOR UART0. 

	This file contains all the serial port components that must be compiled
	to ARM mode.  The components that can be compiled to either ARM or THUMB
	mode are contained in serial.c.

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

/* Constant to access the VIC. */
#define serCLEAR_VIC_INTERRUPT			( ( unsigned portLONG ) 0 )

/* Constants to determine the ISR source. */
#define serSOURCE_THRE					( ( unsigned portCHAR ) 0x02 )
#define serSOURCE_RX_TIMEOUT			( ( unsigned portCHAR ) 0x0c )
#define serSOURCE_ERROR					( ( unsigned portCHAR ) 0x06 )
#define serSOURCE_RX					( ( unsigned portCHAR ) 0x04 )
#define serINTERRUPT_SOURCE_MASK		( ( unsigned portCHAR ) 0x0f )

/* Queues used to hold received characters, and characters waiting to be
transmitted. */
static xQueueHandle xRxedChars; 
static xQueueHandle xCharsForTx; 
static volatile portLONG lTHREEmpty;

/*-----------------------------------------------------------*/




/* 
 * The queues are created in serialISR.c as they are used from the ISR.
 * Obtain references to the queues and THRE Empty flag. 
 */
void vSerialISRCreateQueues( unsigned portBASE_TYPE uxQueueLength, xQueueHandle *pxRxedChars, xQueueHandle *pxCharsForTx, portLONG volatile **pplTHREEmptyFlag );

/* UART0 interrupt service routine entry point. */
void vUART_ISR_Wrapper( void ) __attribute__ ((naked));

/* UART0 interrupt service routine handler. */
void vUART_ISR_Handler( void );

/*-----------------------------------------------------------*/
void vSerialISRCreateQueues(	unsigned portBASE_TYPE uxQueueLength, xQueueHandle *pxRxedChars, 
								xQueueHandle *pxCharsForTx, portLONG volatile **pplTHREEmptyFlag )
{
	/* Create the queues used to hold Rx and Tx characters. */
	//xRxedChars = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	xRxedChars = xQueueCreate( 8, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	xCharsForTx = xQueueCreate( uxQueueLength + 1, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );

	/* Pass back a reference to the queues so the serial API file can 
	post/receive characters. */
	*pxRxedChars = xRxedChars;
	*pxCharsForTx = xCharsForTx;

	/* Initialise the THRE empty flag - and pass back a reference. */
	lTHREEmpty = ( portLONG ) pdTRUE;
	*pplTHREEmptyFlag = &lTHREEmpty;
}
/*-----------------------------------------------------------*/

void vUART_ISR_Wrapper( void )
{
	/* Save the context of the interrupted task. */
	portSAVE_CONTEXT();

	/* Call the handler.  This must be a separate function from the wrapper
	to ensure the correct stack frame is set up. */
	vUART_ISR_Handler();

	/* Restore the context of whichever task is going to run next. */
	portRESTORE_CONTEXT();
}
/*-----------------------------------------------------------*/

void vUART_ISR_Handler( void )
{
signed portCHAR cChar;
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* What caused the interrupt? */
	switch( UART0_IIR & serINTERRUPT_SOURCE_MASK )
	{
		case serSOURCE_ERROR :	/* Not handling this, but clear the interrupt. */
								cChar = UART0_LSR;
								break;

		case serSOURCE_THRE	:	/* The THRE is empty.  If there is another
								character in the Tx queue, send it now. */
								if( xQueueReceiveFromISR( xCharsForTx, &cChar, &xHigherPriorityTaskWoken ) == pdTRUE )
								{
									UART0_THR = cChar;
								}
								else
								{
									/* There are no further characters 
									queued to send so we can indicate 
									that the THRE is available. */
									lTHREEmpty = pdTRUE;
								}
								break;

		case serSOURCE_RX_TIMEOUT :
		case serSOURCE_RX	:	/* A character was received.  Place it in 
								the queue of received characters. */
								cChar = UART0_RBR;
								xQueueSendFromISR( xRxedChars, &cChar, &xHigherPriorityTaskWoken );
								break;

		default				:	/* There is nothing to do, leave the ISR. */
								break;
	}
	
	
	if( xHigherPriorityTaskWoken )
	{
		portYIELD_FROM_ISR();
	}

	/* Clear the ISR in the VIC. */
	VICVectAddr = serCLEAR_VIC_INTERRUPT;
}


#ifdef PAKAI_SERIAL_3
/* UART 3 */
/* Queues used to hold received characters, and characters waiting to be
transmitted. */
static xQueueHandle xRxedChars3; 
static xQueueHandle xCharsForTx3; 
static volatile portLONG lTHREEmpty3;

/*-----------------------------------------------------------*/

/* 
 * The queues are created in serialISR.c as they are used from the ISR.
 * Obtain references to the queues and THRE Empty flag. 
 */
void vSerialISRCreateQueues3( unsigned portBASE_TYPE uxQueueLength, xQueueHandle *pxRxedChars, xQueueHandle *pxCharsForTx, portLONG volatile **pplTHREEmptyFlag );

/* UART0 interrupt service routine entry point. */
void vUART3_ISR_Wrapper( void ) __attribute__ ((naked));

/* UART0 interrupt service routine handler. */
void vUART3_ISR_Handler( void );

/*-----------------------------------------------------------*/
void vSerialISRCreateQueues3(	unsigned portBASE_TYPE uxQueueLength, xQueueHandle *pxRxedChars, 
								xQueueHandle *pxCharsForTx, portLONG volatile **pplTHREEmptyFlag )
{
	/* Create the queues used to hold Rx and Tx characters. */
	//xRxedChars = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	
	portENTER_CRITICAL();
	
	xRxedChars3 = xQueueCreate( 8, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	xCharsForTx3 = xQueueCreate( uxQueueLength + 1, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );

	/* Pass back a reference to the queues so the serial API file can 
	post/receive characters. */
	*pxRxedChars = xRxedChars3;
	*pxCharsForTx = xCharsForTx3;

	/* Initialise the THRE empty flag - and pass back a reference. */
	lTHREEmpty3 = ( portLONG ) pdTRUE;
	*pplTHREEmptyFlag = &lTHREEmpty3;
	
	portEXIT_CRITICAL();
}
/*-----------------------------------------------------------*/

void vUART3_ISR_Handler( void )
{
	signed portCHAR cChar;
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* What caused the interrupt? */
	switch( U3IIR & serINTERRUPT_SOURCE_MASK )	// (U3IIR & 0x0f)
	{
		case serSOURCE_ERROR :	/* Not handling this, but clear the interrupt. */	// 0x06
								cChar = U3LSR;
								break;

		case serSOURCE_THRE	:	/* The THRE is empty.  If there is another			// 0x02
								character in the Tx queue, send it now. */
								if( xQueueReceiveFromISR( xCharsForTx3, &cChar, &xHigherPriorityTaskWoken ) == pdTRUE )
								{
									U3THR = cChar;
								}
								else
								{
									/* There are no further characters 
									queued to send so we can indicate 
									that the THRE is available. */
									lTHREEmpty3 = pdTRUE;
									
									FIO0CLR = BIT(24);
								}
								break;

		case serSOURCE_RX_TIMEOUT :													// 0x0c
		case serSOURCE_RX	:	/* A character was received.  Place it in 			// 0x04
								the queue of received characters. */
								cChar = U3RBR;
								xQueueSendFromISR( xRxedChars3, &cChar, &xHigherPriorityTaskWoken );
								break;

		default				:	/* There is nothing to do, leave the ISR. */
								break;
	}
	

	if( xHigherPriorityTaskWoken )
	{
		portYIELD_FROM_ISR();
	}

	/* Clear the ISR in the VIC. */
	VICVectAddr = serCLEAR_VIC_INTERRUPT;
}

void vUART3_ISR_Wrapper( void )
{
	/* Save the context of the interrupted task. */
	portSAVE_CONTEXT();

	/* Call the handler.  This must be a separate function from the wrapper
	to ensure the correct stack frame is set up. */
	vUART3_ISR_Handler();

	/* Restore the context of whichever task is going to run next. */
	portRESTORE_CONTEXT();
}



#endif


#ifdef PAKAI_SERIAL_2
/* UART 2 */
/* UART0 interrupt service routine entry point. */
void vUART2_ISR_Wrapper( void ) __attribute__ ((naked));

/* Queues used to hold received characters, and characters waiting to be
transmitted. */
static xQueueHandle xRxedChars2; 
static xQueueHandle xCharsForTx2; 
static volatile portLONG lTHREEmpty2;

void vSerialISRCreateQueues2(	unsigned portBASE_TYPE uxQueueLength, xQueueHandle *pxRxedChars, 
								xQueueHandle *pxCharsForTx, portLONG volatile **pplTHREEmptyFlag )
{
	/* Create the queues used to hold Rx and Tx characters. */
	//xRxedChars = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	
	portENTER_CRITICAL();
	
	xRxedChars2 = xQueueCreate( uxQueueLength + 1, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	xCharsForTx2 = xQueueCreate( uxQueueLength + 1, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );

	/* Pass back a reference to the queues so the serial API file can 
	post/receive characters. */
	*pxRxedChars = xRxedChars2;
	*pxCharsForTx = xCharsForTx2;

	/* Initialise the THRE empty flag - and pass back a reference. */
	lTHREEmpty2 = ( portLONG ) pdTRUE;
	*pplTHREEmptyFlag = &lTHREEmpty2;
	
	portEXIT_CRITICAL();
}

void vUART2_ISR_Handler( void )
{
	signed portCHAR cChar;
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* What caused the interrupt? */
	switch( U2IIR & serINTERRUPT_SOURCE_MASK )
	{
		case serSOURCE_ERROR :	/* Not handling this, but clear the interrupt. */
			cChar = U2LSR;
			break;

		case serSOURCE_THRE	:	/* The THRE is empty.  If there is another
								character in the Tx queue, send it now. */
			if( xQueueReceiveFromISR( xCharsForTx2, &cChar, &xHigherPriorityTaskWoken ) == pdTRUE )
			{
				U2THR = cChar;
			}
			else
			{
									/* There are no further characters 
									queued to send so we can indicate 
									that the THRE is available. */
				lTHREEmpty2 = pdTRUE;
									
			}
		break;

		case serSOURCE_RX_TIMEOUT :
		case serSOURCE_RX	:	/* A character was received.  Place it in 
								the queue of received characters. */
			cChar = U2RBR;
			xQueueSendFromISR( xRxedChars2, &cChar, &xHigherPriorityTaskWoken );
		break;

		default				:	/* There is nothing to do, leave the ISR. */
			break;
	}
	
	
	if( xHigherPriorityTaskWoken )
	{
		portYIELD_FROM_ISR();
	}

	/* Clear the ISR in the VIC. */
	VICVectAddr = serCLEAR_VIC_INTERRUPT;
}

void vUART2_ISR_Wrapper( void )
{
	/* Save the context of the interrupted task. */
	portSAVE_CONTEXT();

	/* Call the handler.  This must be a separate function from the wrapper
	to ensure the correct stack frame is set up. */
	vUART2_ISR_Handler();

	/* Restore the context of whichever task is going to run next. */
	portRESTORE_CONTEXT();
}

#endif


#ifdef PAKAI_SERIAL_1
/* UART 1 */
/* UART1 interrupt service routine entry point. */
void vUART1_ISR_Wrapper( void ) __attribute__ ((naked));

/* Queues used to hold received characters, and characters waiting to be
transmitted. */
static xQueueHandle xRxedChars1; 
static xQueueHandle xCharsForTx1; 
static volatile portLONG lTHREEmpty1;

void vSerialISRCreateQueues1(	unsigned portBASE_TYPE uxQueueLength, xQueueHandle *pxRxedChars, 
								xQueueHandle *pxCharsForTx, portLONG volatile **pplTHREEmptyFlag )
{
	/* Create the queues used to hold Rx and Tx characters. */
	//xRxedChars = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	
	portENTER_CRITICAL();
	
	xRxedChars1 = xQueueCreate( uxQueueLength + 1, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	xCharsForTx1 = xQueueCreate( uxQueueLength + 1, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );

	/* Pass back a reference to the queues so the serial API file can 
	post/receive characters. */
	*pxRxedChars = xRxedChars1;
	*pxCharsForTx = xCharsForTx1;

	/* Initialise the THRE empty flag - and pass back a reference. */
	lTHREEmpty1 = ( portLONG ) pdTRUE;
	*pplTHREEmptyFlag = &lTHREEmpty1;
	
	portEXIT_CRITICAL();
}

void vUART1_ISR_Handler( void )
{
	signed portCHAR cChar;
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* What caused the interrupt? */
	switch( U1IIR & serINTERRUPT_SOURCE_MASK )
	{
		case serSOURCE_ERROR :	/* Not handling this, but clear the interrupt. */
			cChar = U1LSR;
			break;

		case serSOURCE_THRE	:	/* The THRE is empty.  If there is another
								character in the Tx queue, send it now. */
			if( xQueueReceiveFromISR( xCharsForTx1, &cChar, &xHigherPriorityTaskWoken ) == pdTRUE )
			{
				U1THR = cChar;
			}
			else
			{
									/* There are no further characters 
									queued to send so we can indicate 
									that the THRE is available. */
				lTHREEmpty1 = pdTRUE;
									
			}
		break;

		case serSOURCE_RX_TIMEOUT :
		case serSOURCE_RX	:	/* A character was received.  Place it in 
								the queue of received characters. */
			cChar = U1RBR;
			xQueueSendFromISR( xRxedChars1, &cChar, &xHigherPriorityTaskWoken );
		break;

		default				:	/* There is nothing to do, leave the ISR. */
			break;
	}
	
	
	if( xHigherPriorityTaskWoken )
	{
		portYIELD_FROM_ISR();
	}

	/* Clear the ISR in the VIC. */
	VICVectAddr = serCLEAR_VIC_INTERRUPT;
}

void vUART1_ISR_Wrapper( void )
{
	/* Save the context of the interrupted task. */
	portSAVE_CONTEXT();

	/* Call the handler.  This must be a separate function from the wrapper
	to ensure the correct stack frame is set up. */
	vUART1_ISR_Handler();

	/* Restore the context of whichever task is going to run next. */
	portRESTORE_CONTEXT();
}

#endif



