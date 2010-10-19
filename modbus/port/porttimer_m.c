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
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
//#include "modbus_ISR.c"
/* ----------------------- static functions ---------------------------------*/

/* ----------------------- Start implementation -----------------------------*/
extern (unsigned int) prvvTIMERExpiredISR;

BOOL xMBPortTimersInit( USHORT usTim1Timerout50us ) {
	portENTER_CRITICAL();
	
	PCONP |= BIT(23);
	
	// set PCLK = CCLK
	PCLKSEL1 &= ~(BIT(14) | BIT(15));
	PCLKSEL1 |= BIT(14);
	
    // Timer0 Configuration
    T3TCR	= 0x02;			// Stop and reset the timer //
	T3CTCR	= 0x0;			// Timer mode               //
	T3TC	= 0x0;
	T3PR	= 0x0;			// Prescaler Register = 0	//
	T3PC	= 0x0;			// Prescaler Counter = 0	//
	
	T3MCR	= BIT(0) | BIT(1);			// Bit0: INT ketika MR3 = TC, Bit1: Reset TC setelah match
	//T3MR0	= 30000000L;		//PCLK*2;		//(PCLK/20000) * usTim1Timerout50us;		// 2 detik
	
    T3MR0 = ( PCLK / 20000 ) * usTim1Timerout50us;      // Interval of (50us * usTim1Timerout50us)
    //T3MCR = 3;                  // Bit 0 = 1 - Interruption on MR0
    // Bit 1 = 1 - Reset on MR0

    T3TCR = 0;                  // Timer Counter and Prescale Counter Disabled

    // Configure Timer3 Interruption
    //extern unsigned int ( Timer3_Wrapper )( void );
	VICIntSelect &= ~BIT(27);		// RESET (default 0), jadikan VIC, bukan FIQ
	//VICVectAddr27 = ( unsigned int )Timer3_Wrapper;
	
	VICVectAddr27 = ( unsigned int )prvvTIMERExpiredISR; // Timer3 Interruption - Priority 1
	VICIntEnable |= BIT(27);		// Enable Timer3 Interruption

    return TRUE;
}

void vMBPortTimersEnable(  ) {
    T3TCR = 0x02;               // Disable Timer and Reset Counter
    T3TCR = 0x01;               // Enable Timer
}

void vMBPortTimersDisable(  ) {
    T3TCR = 0x02;               // Disable Timer and Reset Counter
}
