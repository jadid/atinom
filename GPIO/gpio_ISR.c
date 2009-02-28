/*
 * gpio_ISR.c
 *
 *  Created on: Dec 9, 2008
 *      Author: jadid
 *
 *      ini harus dicompile pada mode ARM
 */

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"
#include "gpio.h"
#include "../tampilan/tampilan.h"

extern xSemaphoreHandle keypad_sem;
struct t2_konter konter;


void gpio_ISR_Wrapper( void ) __attribute__ ((naked));
void gpio_ISR_Handler( void );

void timer1_ISR_Wrapper( void ) __attribute__ ((naked));
void timer1_ISR_Handler( void );

void gpio_ISR_Wrapper_keypad( void ) __attribute__ ((naked));
void gpio_ISR_keypad_Handler( void );


void set_konter(int st, unsigned int period);

void gpio_ISR_Wrapper( void )
{
	/* Save the context of the interrupted task. */
	portSAVE_CONTEXT();

	/* Call the handler.  This must be a separate function from the wrapper
	to ensure the correct stack frame is set up. */
	gpio_ISR_Handler();

	/* Restore the context of whichever task is going to run next. */
	portRESTORE_CONTEXT();
}

void gpio_ISR_Wrapper_keypad( void )
{
	/* Save the context of the interrupted task. */
	portSAVE_CONTEXT();

	/* Call the handler.  This must be a separate function from the wrapper
	to ensure the correct stack frame is set up. */
	gpio_ISR_keypad_Handler();

	/* Restore the context of whichever task is going to run next. */
	portRESTORE_CONTEXT();
}

void timer1_ISR_Wrapper( void )
{
	/* Save the context of the interrupted task. */
	portSAVE_CONTEXT();
	
	/* Call the handler.  This must be a separate function from the wrapper
	to ensure the correct stack frame is set up. */
	timer1_ISR_Handler();

	/* Restore the context of whichever task is going to run next. */
	portRESTORE_CONTEXT();
}

void gpio_ISR_Handler( void )
{
	int t=0;
	unsigned int new_period;
	konter.global_hit++;

	new_period = T1TC;
	//cek sumber
	if (IO2_INT_STAT_F & kont_10)
	{
		t = 9;
		set_konter(t, new_period);
		IO2_INT_CLR = kont_10;
	}
	if (IO2_INT_STAT_F & kont_9)
	{
		t = 8;
		set_konter(t, new_period);
		IO2_INT_CLR = kont_9;
	}
	if (IO2_INT_STAT_F & kont_8)
	{
		t = 7;
		set_konter(t, new_period);
		IO2_INT_CLR = kont_8;
	}
	if (IO2_INT_STAT_F & kont_7)
	{
		t = 6;
		set_konter(t, new_period);
		IO2_INT_CLR = kont_7;
	}
	if (IO2_INT_STAT_F & kont_6)
	{
		t = 5;
		set_konter(t, new_period);
		IO2_INT_CLR = kont_6;
	}

	/*
	new_period = T1TC;
	if (new_period > konter.t_konter[t].last_period)
	{
			konter.t_konter[t].beda = (new_period -
					konter.t_konter[t].last_period) * 50;	// 1 clock 50 nanosecond
	}
	else	// sudah overflow
	{
		konter.t_konter[t].beda = (new_period +
			(0xFFFFFFFF - konter.t_konter[t].last_period)) * 50;	// 1 clock 50 nanosecond

	}
	konter.t_konter[t].hit++;
	konter.t_konter[t].last_period = new_period;
	*/

	// clear GPIO interrupt
	/*
	if (t == 9)
	{
		IO2_INT_CLR = kont_10;
	}
	else if (t == 8)
	{
		IO2_INT_CLR = kont_9;
	}
	else if (t == 7)
	{
		IO2_INT_CLR = kont_8;
	}
	else if (t == 6)
	{
		IO2_INT_CLR = kont_7;
	}
	else if (t == 5)
	{
		IO2_INT_CLR = kont_6;
	} */
	/* Clear the ISR in the VIC. */
	VICVectAddr = 0;
}

void timer1_ISR_Handler( void )
{
	konter.ovflow++;

	T1IR = TxIR_MR0_Interrupt;   // clear interrupt by writing an IR-bit

	/* Clear the ISR in the VIC. */
	VICVectAddr = 0;
}

void set_konter(int st, unsigned int period)
{
	//new_period = T1TC;
	if (period > konter.t_konter[st].last_period)
	{
		konter.t_konter[st].beda = (period -
			konter.t_konter[st].last_period) * 50;	// 1 clock 50 nanosecond
	}
	else	// sudah overflow
	{
		konter.t_konter[st].beda = (period +
			(0xFFFFFFFF - konter.t_konter[st].last_period)) * 50;	// 1 clock 50 nanosecond

	}
	konter.t_konter[st].hit++;
	konter.t_konter[st].last_period = period;
}

void gpio_ISR_keypad_Handler( void )
{
	static portBASE_TYPE xHigherPriorityTaskWoken;
	 
	if (IO2_INT_STAT_F & PF14)
	{
		//xSemaphoreGive( keypad_sem );		
		xSemaphoreGiveFromISR( keypad_sem, &xHigherPriorityTaskWoken );
		
		IO2_INT_CLR = PF14;
	}
	
	/* Clear the ISR in the VIC. */
	VICVectAddr = 0;	
}
