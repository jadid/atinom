/*
 * gpio.c
 *
 *  Created on: Dec 9, 2008
 *      Author: jadid
 */

#include "FreeRTOS.h"
#include "gpio.h"



#define INTERRUPT_GPIO	0x20000 // EINT3


void init_gpio(void)
{
	extern void ( gpio_ISR_Wrapper )( void );
	extern void ( timer1_ISR_Wrapper )( void );

	portENTER_CRITICAL();

	// Timer1 untuk free running clock
	T1TCR = TxTCR_Counter_Reset;           // reset & disable timer 0

	// setup Timer 1 to count forever
	// Periode Timer 1 adalah 50 ns
	// akan overflow setiap 3.57 menit, interrupt dan
	// lakukan sesuatu pada variabel2.
	T1PR = 3 - 1;               	// set the prescale divider (60 / 3, 20 Mhz (50 ns))
	T1CCR = 0;                      // disable dulu compare registers
	T1EMR = 0;                      // disable external match register
	T1TCR = TxTCR_Counter_Enable;

	//siapkan interrupt handler untuk Timer 1
	VICIntSelect &= ~(VIC_CHAN_TO_MASK(VIC_CHAN_NUM_Timer1));
	VICIntEnClr  = VIC_CHAN_TO_MASK(VIC_CHAN_NUM_Timer1);
	VICVectAddr5 = ( portLONG )timer1_ISR_Wrapper;
	VICVectPriority5 = 0x08;
	VICIntEnable = VIC_CHAN_TO_MASK(VIC_CHAN_NUM_Timer1);
	T1MR0 = 0xFFFFFFFF;		// maksimum
	T1MCR = TxMCR_MR0I;     // interrupt on cmp-match0
	T1IR  = TxIR_MR0_Interrupt; // clear match0 interrupt

	// karena masih 3 menit yang akan datang, harusnya dienable dari sini gak masalah
	T1TCR = TxTCR_Counter_Enable;         // enable timer 0

	//siapkan interrupt handler untuk GPIO
	VICIntSelect    &= ~VIC_CHAN_TO_MASK(VIC_CHAN_NUM_EINT3);
	VICIntEnClr      = VIC_CHAN_TO_MASK(VIC_CHAN_NUM_EINT3);
	VICVectAddr17 = ( portLONG )gpio_ISR_Wrapper;
	VICVectPriority17 = 0x05;
	VICIntEnable = VIC_CHAN_TO_MASK(VIC_CHAN_NUM_EINT3);


	// setup GPIO direction & interrupt
	FIO2DIR = FIO2DIR & ~(kont_10 | kont_9 | kont_8 | kont_7 | kont_6);
	IO2_INT_EN_F = kont_10 | kont_9 | kont_8 | kont_7 | kont_6;

	portEXIT_CRITICAL();
}
