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
//#include <LPC214X.h>
#include "../lpc23xx.h"

#include "FreeRTOS.h" 
#include "queue.h"
#include "task.h"

#include "port.h"
#include "porttimer.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/


/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortTimersInit( USHORT usTim1Timerout50us ) {
    // Timer3 Configuration
    PCONP |= BIT(23);
	
	// set PCLK = CCLK
	PCLKSEL1 &= ~(BIT(14) | BIT(15));
	PCLKSEL1 |= BIT(14);
	
    T3PR = 0;                   // Prscaler Register = 0
    T3PC = 0;                   // Prscaler Counter = 0

    T3TC = 0;                   // Timer Counter = 0

    T3MR0 = ( PCLK / 20000 ) * usTim1Timerout50us;      // Interval of (50us * usTim1Timerout50us)
    T3MCR = 3;                  // Bit 0 = 1 - Interruption on MR0
    // Bit 1 = 1 - Reset on MR0

    T3TCR = 0;                  // Timer Counter and Prescale Counter Disabled

    // Configure Timer3 Interruption
    VICIntSelect &= ~BIT(27);		// RESET (default 0), jadikan VIC, bukan FIQ
	//VICVectAddr27 = ( unsigned int )Timer3_Wrapper;
	extern void (prvvTIMERExpiredISR)( void );
	VICVectAddr27 = (portLONG) prvvTIMERExpiredISR; // Timer3 Interruption - Priority 1
	VICIntEnable |= BIT(27);		// Enable Timer3 Interruption
    
    
    // aslinya //
    //VICVectCntl1 = 0x20 | 4;
    //VICIntEnable = ( 1 << 4 );  // Enable Timer0 Interruption

    return TRUE;
}

void vMBPortTimersEnable(  ) {
    T3TCR = 0x02;               // Disable Timer and Reset Counter
    T3TCR = 0x01;               // Enable Timer
}

void vMBPortTimersDisable(  ) {
    T3TCR = 0x02;               // Disable Timer and Reset Counter
}


