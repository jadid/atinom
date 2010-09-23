/*
  * FreeModbus Libary: LPC214X Port
  * Copyright (C) 2007 Tiago Prado Lone <tiago@maxwellbohr.com.br>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.1 2007/04/24 23:15:18 wolti Exp $
 */

#include "../lpc23xx.h"

#include "FreeRTOS.h" 
#include "queue.h"
#include "task.h"

#include "port.h"
#include "portserial.h"


/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/

/* ----------------------- Start implementation -----------------------------*/

void vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable ) {
    if( xRxEnable )     {
        U3IER |= 0x01;
        printf("Rx bisa ");
    } else {
        U3IER &= ~0x01;
    }
    if( xTxEnable ) {
        U3IER |= 0x02;
        printf("TX bisa");
        prvvUARTTxReadyISR(  );
    } else {
        U3IER &= ~0x02;
    }
    printf("\r\n");
}

void vMBPortClose( void ) {
}

#define serENABLE_INTERRUPTS			( ( unsigned portCHAR ) 0x03 )
#define serFIFO_ON						( ( unsigned portCHAR ) 0x01 )
#define serCLEAR_FIFO					( ( unsigned portCHAR ) 0x06 )

#define serUART3_VIC_ENABLE				( ( unsigned portLONG ) 0x0020 )



BOOL xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity ) {
    BOOL            bInitialized = TRUE;
    USHORT          cfg = 0;
    //ULONG           reload = ( ( PCLK / ulBaudRate ) / 16UL );
    ULONG           reload = ( ( CCLK / ulBaudRate ) / 16UL );
    volatile char   dummy;
	printf("serial 3 init...reload: %d\r\n", reload);
	
    ( void )ucPORT;


    switch ( ucDataBits )     {
    case 5:
        break;
    case 6:
        cfg |= 0x00000001;
        break;
    case 7:
        cfg |= 0x00000002;
        break;
    case 8:
        cfg |= 0x00000003;
        break;
    default:
        bInitialized = FALSE;
    }

    switch ( eParity )     {
    case MB_PAR_NONE:
        break;
    case MB_PAR_ODD:
        cfg |= 0x00000008;
        break;
    case MB_PAR_EVEN:
        cfg |= 0x00000018;
        break;
    }

    if( bInitialized )     {
        U3LCR = cfg;            // set Data Bits and Parity 8 data, 1 stop, parity none: 0x07 //
        U3IER = 0;              // Disable UART3 Interrupts //

        U3LCR |= 0x80;          // Set DLAB		//
        U3DLL = (reload & (unsigned portLONG) 0xff);         // Set Baud		//
        U3DLM = ((reload >> 8) & (unsigned portLONG) 0xff);  // Set Baud		//
        U3FCR = ( serFIFO_ON | serCLEAR_FIFO );
        U3LCR &= ~0x80;         // Clear DLAB, dipasang iki malah ga bisa//

        /* Configure UART3 Interrupt */
        VICIntSelect &= ~BIT(29);		// VIC
        extern void (sio_irq)( void );
        VICVectAddr29 = (portLONG) sio_irq;
        //VICVectCntl29 = 3;
        //VICIntEnable = 1 << 29;
		VICIntEnable |= BIT(29);  /* Enable UART3 Interrupt */
        dummy = U3IIR;          /* Required to Get Interrupts Started */
    }
    return bInitialized;
}

BOOL xMBPortSerialPutByte( CHAR ucByte ) {
    U3THR = ucByte;

    /* Wait till U3THR and U3TSR are both empty */
    while( !( U3LSR & 0x20 ) )
    {
    }

    return TRUE;
}

BOOL xMBPortSerialGetByte( CHAR * pucByte ) {
    //*
    while( !( U3LSR & 0x01 ) )
    {
    }

    // Receive Byte //
    *pucByte = U3RBR;
    return TRUE;
    //*/
    /*
	if(ser3_getchar(1, &pucByte[0], 100 ) == pdTRUE)
	    return TRUE;
	else 
		return FALSE;
	//*/
}


/* 
 * Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */

void prvvUARTTxReadyISR( void ) {
    pxMBFrameCBTransmitterEmpty(  );
}

/* 
 * Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
//*
void prvvUARTRxISR( void ) {
    pxMBFrameCBByteReceived(  );
}
//*/

