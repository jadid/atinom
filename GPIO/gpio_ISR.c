/*
 * gpio_ISR.c
 *
 *  Created on: Dec 9, 2008
 *      Author: jadid
 *
 *      ini harus dicompile pada mode ARM
 * 
 *  11 Sept 2009
 * 	handler untuk GPIO input kontrol (status saja)
 * 	
 */

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"
#include "gpio.h"

#ifdef PAKAI_TAMPILAN
#include "../tampilan/tampilan.h"
#endif


#ifdef TAMPILAN_LPC_4
extern xSemaphoreHandle keypad_sem;
#endif

#ifdef PAKAI_GPIO_DIMMER
	int loop_dm = 1, loop_dimmer;
	extern int persen_pwm;
	#define FULL_PERSEN		100
	
	void timer2_ISR_Wrapper( void )		{
		portSAVE_CONTEXT();

		#if ( DEBUG_KONTER == 1) 
		togle_led_konter();
		#endif
		
		timer2_ISR_Handler();

		portRESTORE_CONTEXT();
	}

	void timer2_ISR_Handler( void )	{
		/*
		if (T2IR & TxIR_MR0_Interrupt) {
			FIO1CLR = BIT(30);
		}
		
		if (T2IR & TxIR_MR1_Interrupt) {
			FIO1SET = BIT(30);
		}
		//*/
		#if 1
		if (T2IR & TxIR_MR0_Interrupt) {
			//loop_dm = 1-loop_dm;
			loop_dimmer++;
			T2IR	= TxIR_MR0_Interrupt;
			T2TC	= 0;
			
			if (loop_dimmer>FULL_PERSEN)	loop_dimmer=0;
			
			if (loop_dimmer<persen_pwm)	FIO1CLR = BIT(30);
			//if (loop_dm)				FIO1SET = BIT(30);
			else						FIO1SET = BIT(30);
		}
		#endif
		//*/
		T2TCR	= TxTCR_Counter_Enable;         // enable timer 2

		VICVectAddr = 0;
	}

#endif

#ifdef BOARD_CNC
int itungT2=0;
int iSin=0;

#define xPWM		256
extern unsigned int nPWM[xPWM];

void timer2_ISR_Wrapper( void )
{
	portSAVE_CONTEXT();

	#if ( DEBUG_KONTER == 1) 
	togle_led_konter();
	#endif
	
	timer2_ISR_Handler();

	portRESTORE_CONTEXT();
}

void timer2_ISR_Handler( void )	{
	if (T2IR & TxIR_MR0_Interrupt) {
		if (nPWM[iSin] != maxKonter)
			FIO1CLR = BIT(31);
		T2IR	= TxIR_MR0_Interrupt;
	}
	
	if (T2IR & TxIR_MR1_Interrupt) {
		T2IR	= TxIR_MR1_Interrupt;
		T2TC	= 0;
		T2MR0	= nPWM[iSin];
		//if (nPWM[iSin] != 0)
		if (iSin>0 && iSin<255)
			FIO1SET = BIT(31);
		
	}

	
/*	
	if (T2IR & TxIR_MR0_Interrupt) {		//
		T2IR	= TxIR_MR0_Interrupt;
		FIO1CLR = BIT(31);
	}
	
	if (T2IR & TxIR_MR1_Interrupt) {
		T2IR	= TxIR_MR1_Interrupt;
		T2TC	= 0;						// reset counter
		FIO1SET = BIT(31);
		//T2MR0	= nPWM[iSin];
		//T2MR0	= 500000;
	}
//*/	
	if (iSin>255)	{
		iSin = 0;
		//T2TCR &= ~TxTCR_Counter_Enable;
	}
	else
		iSin++;
	//FIO1PIN ^= BIT(31);

/*	
	T2IR	= TxIR_MR0_Interrupt;   		// clear interrupt by writing an IR-bit
	T2TC	= 0;
	//pgm_read_byte(&sinewave[i]);
	//T2MCR 	= TxMCR_MR0I;
	//T2TCR	= TxTCR_Counter_Reset | TxTCR_Counter_Enable;
	if (itungT2==0)			T2MR0 = 1000000;		// maksimum
	else if (itungT2==1)	T2MR0 = 500000;		// maksimum
	else if (itungT2==2)	T2MR0 = 250000;		// maksimum
	else if (itungT2==3)	T2MR0 = 500000;		// maksimum
	else if (itungT2==4)	T2MR0 = 1000000;		// maksimum
	else  {
		itungT2= -1;
		T2MR0 = 2000000;		// maksimum
	}
//*/	
	T2TCR	= TxTCR_Counter_Enable;         // enable timer 2
	
	/* Clear the ISR in the VIC. */
	VICVectAddr = 0;
	//itungT2++;
}
#endif


#ifdef BOARD_KOMON_KONTER
struct t2_konter konter;
#endif

void gpio_ISR_Wrapper( void ) __attribute__ ((naked));
void gpio_ISR_Handler( void );

void timer1_ISR_Wrapper( void ) __attribute__ ((naked));
void timer1_ISR_Handler( void );
void timer1_ISR_KONTROL( void );

void timer2_ISR_Wrapper( void ) __attribute__ ((naked));
void timer2_ISR_Handler( void );

void gpio_ISR_Wrapper_keypad( void ) __attribute__ ((naked));
void gpio_ISR_keypad_Handler( void );


void set_konter(int st, unsigned int period);

#ifdef BOARD_KOMON_KONTER
void gpio_ISR_Wrapper( void )
{
	/* Save the context of the interrupted task. */
	portSAVE_CONTEXT();

	#if ( DEBUG_KONTER == 1) 
	togle_led_konter();
	#endif
	
	/* Call the handler.  This must be a separate function from the wrapper
	to ensure the correct stack frame is set up. */
	
#if (PAKAI_KONTROL == 1)
	/* 11 Sept 2009, jika GPIO hanya untuk status on off */
	gpio_ISR_KONTROL();

#else
	
	gpio_ISR_Handler();
#endif
	/* Restore the context of whichever task is going to run next. */
	portRESTORE_CONTEXT();
}
#endif

#ifdef TAMPILAN_LPC_4
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
#endif

#ifdef BOARD_KOMON_KONTER
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
	
	#ifdef BOARD_KOMON_KONTER_3_1
		if (IO2_INT_STAT_F & kont_5)	{
			t = 4;
			set_konter(t, new_period);
			IO2_INT_CLR = kont_5;
		}
		
		if (IO2_INT_STAT_F & kont_4)	{
			t = 3;
			set_konter(t, new_period);
			IO2_INT_CLR = kont_4;
		}
		
		if (IO2_INT_STAT_F & kont_3)	{
			t = 2;
			set_konter(t, new_period);
			IO2_INT_CLR = kont_3;
		}
		
		if (IO2_INT_STAT_F & kont_2)	{
			t = 1;
			set_konter(t, new_period);
			IO2_INT_CLR = kont_2;
		}
		
		if (IO2_INT_STAT_F & kont_1)	{
			t = 0;
			set_konter(t, new_period);
			IO2_INT_CLR = kont_1;
		}
	#endif

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
#endif // KOMON_KONTER

#ifdef TAMPILAN_LPC_4
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
#endif

#if (PAKAI_KONTROL == 1)
/* ingat bahwa input sesuai dengan nomer kanal, bukan array */

int stat_input[11];
int jum_tutup;

void gpio_ISR_KONTROL( void )
{
	//cek sumber
	if (IO2_INT_STAT_F & kont_10)	// falling, pintu nutup, switch on
	{
		stat_input[10] = 1;
		jum_tutup++;
		
		IO2_INT_CLR = kont_10;
	}	
	if (IO2_INT_STAT_R & kont_10)
	{
		stat_input[10] = 0;		// rising, pintu buka, swith off / open
		jum_tutup++;
		
		IO2_INT_CLR = kont_10;
	}
	
	
	if (IO2_INT_STAT_F & kont_9) // armed on
	{
		stat_input[9] = 1;
		
		IO2_INT_CLR = kont_9;
	}
	if (IO2_INT_STAT_R & kont_9) // armed off
	{
		stat_input[9] = 0;
		
		IO2_INT_CLR = kont_9;
	}
	
	
	if (IO2_INT_STAT_F & kont_8)
	{
		stat_input[8] = 1;
		
		IO2_INT_CLR = kont_8;
	}
	if (IO2_INT_STAT_F & kont_7)
	{
		stat_input[7] = 1;
		
		IO2_INT_CLR = kont_7;
	}
	
	
	if (IO2_INT_STAT_F & kont_6) 
	{
		stat_input[6] = 1;
		
		IO2_INT_CLR = kont_6;
	}
	
	if (IO2_INT_STAT_R & kont_6) 
	{
		stat_input[6] = 0;
		
		IO2_INT_CLR = kont_6;
	}
	
	/* Clear the ISR in the VIC. */
	VICVectAddr = 0;
}
#endif
