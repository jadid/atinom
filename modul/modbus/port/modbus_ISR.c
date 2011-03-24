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
 * File: $Id: porttimer.c,v 1.1 2007/04/24 23:15:18 wolti Exp $
 */

/* ----------------------- Platform includes --------------------------------*/
#include <stdlib.h>

#include "../lpc23xx.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/

#include "portserial.h"
#include "porttimer.h"
/* ----------------------- static functions ---------------------------------*/

void prvvTIMERExpiredISR( void ) __attribute__ ((naked));
void sio_irq( void )			 __attribute__ ((naked));
/* ----------------------- Start implementation -----------------------------*/

extern int berhitung_timer3;
extern int berhitung_serial3;

extern void prvvUARTRxISR();
extern void prvvUARTTxReadyISR();


#define serCLEAR_VIC_INTERRUPT			( ( unsigned portLONG ) 0 )

void sio_irq( void ) {
	portSAVE_CONTEXT();
	
    volatile char   dummy;
    volatile char   IIR;
	
	berhitung_serial3++;
    while( ( ( IIR = U3IIR ) & 0x01 ) == 0 )
    {
        switch ( IIR & 0x0E )
        {
        case 0x06:             // Receive Line Status //
            dummy = U3LSR;      // Just clear the interrupt source //
            break;

        case 0x04:             // Receive Data Available //
        case 0x0C:             // Character Time-Out //
            prvvUARTRxISR();
            break;

        case 0x02:             // THRE Interrupt //
            prvvUARTTxReadyISR();
            break;
//
//        case 0x00:             // Modem Interrupt //
//            //dummy = U3MSR;      // Just clear the interrupt source //
//            break;

        default:
            break;
        }
    }
//*/
    VICVectAddr = serCLEAR_VIC_INTERRUPT;         // Acknowledge Interrupt //
    portRESTORE_CONTEXT();
}

void prvvTIMERExpiredISR( void ) {
	portSAVE_CONTEXT();
	
	( void )pxMBPortCBTimerExpired(  );

    T3IR = 0xFF;
    berhitung_timer3++;
	
	VICVectAddr = serCLEAR_VIC_INTERRUPT;         // Acknowledge Interrupt //
    portRESTORE_CONTEXT();
}

