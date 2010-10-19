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

//#include <LPC214X.h>

//#include "FreeRTOS.h"
//#include "queue.h"
//#include "task.h"

#include "../lpc23xx.h"
#include "port.h"
#include "modbus_ISR.c"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/

#define serUART3_VIC_CHANNEL			( ( unsigned portLONG ) 0x0006 )
#define serUART3_VIC_CHANNEL_BIT		( ( unsigned portLONG ) 0x0040 )
#define serUART3_VIC_ENABLE				( ( unsigned portLONG ) 0x0020 )
#define serENABLE_INTERRUPTS			( ( unsigned portCHAR ) 0x03 )

//void interupsi( void ) __attribute__ ((naked));
//static void sio_irq( void );//    __irq;
//extern static void     prvvUARTTxReadyISR( void );
//extern static void     prvvUARTRxISR( void );
//*/
/* ----------------------- Start implementation -----------------------------*/
//*
void            vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable ) {
    if( xRxEnable )     {
        U3IER |= 0x01;
    }  else   {
        U3IER &= ~0x01;
    }
    
    if( xTxEnable ) {
        U3IER |= 0x02;
        prvvUARTTxReadyISR(  );
    } else {
        U3IER &= ~0x02;
    }
}
//*/
void vMBPortClose( void ) { 
	vSerialClose(0);
}

#define serENABLE_INTERRUPTS			( ( unsigned portCHAR ) 0x03 )

BOOL xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity ) {
	
	printf("masuk serial 3 init...\r\n");
	// parity, databit di hardcode di serial/serial.c
	//serial3_init( ulBaudRate, (1 * configMINIMAL_STACK_SIZE) );

	BOOL            bInitialized = TRUE;
	USHORT          cfg = 0;
    ULONG           reload = ( ( PCLK / ulBaudRate ) / 16UL );
    volatile char   dummy;

    ( void )ucPORT;		// tidak dipakai, dianggap cuma ada 1;
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
	
	switch ( eParity ) {
		case MB_PAR_NONE:
			break;
		case MB_PAR_ODD:
			cfg |= 0x00000008;
			break;
		case MB_PAR_EVEN:
			cfg |= 0x00000018;
			break;
    }
    
    printf("_______konfig serial3: %02X\r\n", cfg);
    portENTER_CRITICAL();
    if( bInitialized )
    {
        U3LCR = cfg;            // Configure Data Bits and Parity //
        U3IER = 0;              // Disable UART1 Interrupts //

        U3LCR |= 0x80;          // Set DLAB // Divisor Latch Access Bit
        U3DLL = reload;         // Set Baud     //
        U3DLM = reload >> 8;    // Set Baud //
        U3LCR &= ~0x80;         // Clear DLAB //
			
		VICIntSelect &= ~BIT(29);
		//VICVectAddr29 = ( portLONG ) vUART3_MODBUS_ISR_Wrapper;
		//VICVectAddr29 = ( unsigned long )sio_irq;
		//VICVectAddr29 = ( unsigned long )interupsi;
		VICVectCntl29 = (serUART3_VIC_ENABLE | 29);

		//U3IER |= serENABLE_INTERRUPTS;	
		//VICIntEnable |= BIT(29);
		
        dummy = U3IIR;          // Required to Get Interrupts Started //
    }
	portEXIT_CRITICAL();
	if (!bInitialized) {
		printf("tidak bisa Init serial 3!!!\r\n");
	} else {
		printf("U3DLL: %02X, U3DLM: %02X\r\n", U3DLL, U3DLM);
	}
	return bInitialized;
}

/* BOOL dudulxMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity ) {
    BOOL            bInitialized = TRUE;
    USHORT          cfg = 0;
    ULONG           reload = ( ( PCLK / ulBaudRate ) / 16UL );
    volatile char   dummy;

    ( void )ucPORT;
    // Configure UART3 Pins //		// kudu diubah ke UART3
    PINSEL0 = 0x00050000;       // Enable RxD1 and TxD1 //

    switch ( ucDataBits )
    {
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

    switch ( eParity )
    {
    case MB_PAR_NONE:
        break;

    case MB_PAR_ODD:
        cfg |= 0x00000008;
        break;

    case MB_PAR_EVEN:
        cfg |= 0x00000018;
        break;
    }

	
    if( bInitialized )
    {
        U3LCR = cfg;            // Configure Data Bits and Parity //
        U3IER = 0;              // Disable UART3 Interrupts //

        U3LCR |= 0x80;          // Set DLAB //
        U3DLL = reload;         // Set Baud     //
        U3DLM = reload >> 8;    // Set Baud //
        U3LCR &= ~0x80;         // Clear DLAB //

        // Configure UART1 Interrupt //
        VICVectAddr0 = ( unsigned long )sio_irq;
        VICVectCntl0 = 0x20 | 27;
        VICIntEnable = 1 << 27;  // Enable UART1 Interrupt //

        dummy = U1IIR;          // Required to Get Interrupts Started //
    }

    return bInitialized;
}
//*/

/*
void vUART3_MODBUS_ISR_Wrapper( void ) {
	//portSAVE_CONTEXT();
	vUART3_MODBUS_ISR_Handler();
	//portRESTORE_CONTEXT();
}

void vUART3_MODBUS_ISR_Handler( void ) {
	
}
//*/

BOOL xMBPortSerialPutByte( CHAR ucByte ) {
/*
	if (serX_putchar(3, &ucByte, 100)== pdTRUE) {
		return TRUE;
	} else {
		return FALSE;
	}
//*/
//	INI ASLINYA	//
//*
    U3THR = ucByte;

    // Wait till U0THR and U0TSR are both empty //
    while( !( U3LSR & 0x20 ) )
    {
    }
//*/
    
}

BOOL xMBPortSerialGetByte( CHAR * pucByte ) {
/*
	if (ser3_getchar(1, &pucByte[0], 100 ) == pdTRUE) {
		printf("terima byte: %02hX\r\n", pucByte[0]);
		return TRUE;
	} else {
		return FALSE;
	}
//*/
//	INI ASLINYA	//
//*
    while( !( U3LSR & 0x01 ) )
    {
    }

    // Receive Byte //
    *pucByte = U3RBR;
//*/
}



/*
void interupsi( void ) {
	printf("___________test8ing\r\n");

}
//*/ 


//*/

/* 
 * Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */

//*

//*/
/* 
 * Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */

/*
 
static void prvvUARTRxISR( void ) {
    pxMBFrameCBByteReceived(  );
}
//*/



