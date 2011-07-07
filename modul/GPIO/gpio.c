/*
 * gpio.c
 *
 *  Created on: Dec 9, 2008
 *      Author: jadid
 */

#include "FreeRTOS.h"
#include "gpio.h"
#include "hardware.h"

#ifdef BOARD_TAMPILAN
	//#include "../tampilan/tampilan.h"
#endif
#ifdef BOARD_KOMON_420_SAJA
#include "../adc/ad7708.h"
#endif

#ifdef BOARD_KOMON_420_SABANG
	#ifdef PAKAI_ADC
	#include "../adc/ad7708.h"
	#endif
#endif

#ifdef BOARD_KOMON_A_RTD
#include "../adc/ad7708.h"

#define AD7708_GPIO_KOMON
#endif

#ifdef BOARD_KOMON_B_THERMO
#include "../adc/ad7708.h"

#define AD7708_GPIO_KOMON
#endif

#define INTERRUPT_GPIO	0x20000 // EINT3

#ifdef BOARD_CNC
void init_timer2() {
	extern void ( timer2_ISR_Wrapper )( void );
	
	portENTER_CRITICAL();
	T2TCR = TxTCR_Counter_Reset;           // reset & disable timer 2

	// setup Timer 2 to count forever
	// Periode Timer 2 adalah 50 ns
	// akan overflow setiap 3.57 menit, interrupt dan
	// lakukan sesuatu pada variabel2.
	T2PR = 60 - 1;               	// set the prescale divider (60 / 60, 1 Mhz (1 us))
	T2CCR = 0;                      // disable dulu compare registers
	T2EMR = 0;                      // disable external match register
	T2TCR = TxTCR_Counter_Enable;

	//siapkan interrupt handler untuk Timer 2
	VICIntSelect &= ~(VIC_CHAN_TO_MASK(VIC_CHAN_NUM_Timer2));
	VICIntEnClr  = VIC_CHAN_TO_MASK(VIC_CHAN_NUM_Timer2);
	VICVectAddr26 = ( portLONG )timer2_ISR_Wrapper;
	VICVectPriority26 = 0x01;
	VICIntEnable = VIC_CHAN_TO_MASK(VIC_CHAN_NUM_Timer2);
	T2MR0 = 0xF4240;		// maksimum
	T2MCR = TxMCR_MR0I;     // interrupt on cmp-match0
	T2IR  = TxIR_MR0_Interrupt; // clear match0 interrupt

	T2TCR = TxTCR_Counter_Enable;         // enable timer 2
	
	portEXIT_CRITICAL();
	printf("sudah masuk init timer2...\r\n");
}
#endif


#if defined(BOARD_KOMON_KONTER) || defined(BOARD_KOMON_KONTER_3_0)
//extern struct t2_konter konter;

//extern static unsigned int giliran;

/* 	
	dibuatkan variable terpisah supaya tidak balapan
	dengan interupt 
*/

//unsigned int 	data_putaran[JUM_GPIO];
//unsigned int 	data_hit[JUM_GPIO];

//extern unsigned int data_putaran[];
//extern unsigned int data_hit[];

/*
static void reset_konter(void)
{
	int i;
	
	konter.global_hit = 0;
	konter.ovflow = 0;
	giliran = 0;
	
	for (i=0; i<JUM_GPIO; i++)
	{
		konter.t_konter[i].last_period = 0;
		konter.t_konter[i].beda = 0;
		konter.t_konter[i].hit = 0;
		konter.t_konter[i].hit_lama = 0;
	}

}
//*/

void init_gpio(void)
{
	extern void ( gpio_ISR_Wrapper )( void );
	extern void ( timer1_ISR_Wrapper )( void );

	reset_konter();

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
	
	#ifdef BOARD_KOMON_KONTER_3_1
	FIO2DIR |= FIO2DIR & ~(kont_1 | kont_2 | kont_3 | kont_4 | kont_5);
	#endif
	
	// enable falling edge interrupt
	IO2_INT_EN_F = kont_10 | kont_9 | kont_8 | kont_7 | kont_6;
	
	#ifdef BOARD_KOMON_KONTER_3_1
	IO2_INT_EN_F |= kont_1 | kont_2 | kont_3 | kont_4 | kont_5;
	#endif
	
	#if (PAKAI_KONTROL == 1)
	/* untuk kontrol pintu, ketika ditutup / rising edge lagi 
	 * juga di cek 
	 */
	
	IO2_INT_EN_R = kont_10 | kont_9 | kont_8 | kont_7 | kont_6;
	
	#endif

	portEXIT_CRITICAL();
}
/*
void hitung_rpm(void)
{	
	portENTER_CRITICAL();
	if (konter.t_konter[giliran].hit_lama == konter.t_konter[giliran].hit)
	{
		//konter.t_konter[giliran].beda = 0;		
		data_putaran[giliran] = 0;
		data_hit[giliran] = konter.t_konter[giliran].hit;
	}
	else
	{
		// didapatkan frekuensi (HZ) putaran //
		//temp_rpm = konter.t_konter[i].beda; // beda msh dlm nS
		
		// rpm //
		data_putaran[giliran] = konter.t_konter[giliran].beda;
		data_hit[giliran] = konter.t_konter[giliran].hit;
	}
		
	konter.t_konter[giliran].hit_lama = konter.t_konter[giliran].hit; 
	portEXIT_CRITICAL();
	
	giliran++;
	if (giliran == JUM_GPIO) giliran = 0;
	
	#if 0
	//portENTER_CRITICAL();
	for (i=0; i<JUM_GPIO; i++)
	{
		if (konter.t_konter[i].hit_lama == konter.t_konter[i].hit)
		{
			konter.t_konter[i].beda = 0;
		}
		
		konter.t_konter[i].hit_lama = konter.t_konter[i].hit; 
	}
	//portEXIT_CRITICAL();
	#endif
	
}
//*/
#endif

#ifdef BOARD_TAMPILAN
void init_gpio_keypad(void)
{
#ifdef TAMPILAN_LPC_4
	extern void ( gpio_ISR_Wrapper_keypad )( void );

	portENTER_CRITICAL();
	
	//siapkan interrupt handler untuk GPIO
	VICIntSelect    &= ~VIC_CHAN_TO_MASK(VIC_CHAN_NUM_EINT3);
	VICIntEnClr      = VIC_CHAN_TO_MASK(VIC_CHAN_NUM_EINT3);
	VICVectAddr17 = ( portLONG )gpio_ISR_Wrapper_keypad;
	VICVectPriority17 = 0x05;
	VICIntEnable = VIC_CHAN_TO_MASK(VIC_CHAN_NUM_EINT3);


	// setup GPIO direction & interrupt
	FIO2DIR = FIO2DIR  & ~PF14;
	/* masking dengan 0 supaya bisa dibaca */
	FIO2MASK = FIO2MASK & ~(PF14);	
	
	IO2_INT_EN_F = PF14;

	portEXIT_CRITICAL();
#endif
}
#endif

#ifdef AD7708_GPIO_KOMON
void init_gpio_adc(void)
{
	portENTER_CRITICAL();
	
	/* GPIO CS diset output */
	FIO1DIR = FIO1DIR | port_cs_ad7708;
	
	uncs_ad7708();
	
	/* 	
		port RDY sebagai input 
		sayang ini pada port1, sehingga tidak bisa mode 
		interrupt.
		
		coba nanti perbaiki boardnya
	*/
	FIO1DIR = FIO1DIR & ~port_rdy_ad7708;	
		
	portEXIT_CRITICAL();
}
#endif

/* 3 Nov 2009 */
#ifdef BOARD_KOMON_420_SAJA
void init_gpio_adc(void)
{
	portENTER_CRITICAL();
	
	/* GPIO CS diset output */
	FIO1DIR = FIO1DIR | port_cs_ad7708;
	/* port 2, nanti ganti ke mode interrupt */
	FIO2DIR = FIO2DIR & ~port_rdy_ad7708;
	
	uncs_ad7708();
	
	
	//portEXIT_CRITICAL();
}
#endif

#ifdef BOARD_KOMON_420_SABANG

#ifdef PAKAI_ADC
void init_gpio_adc(void)
{
	portENTER_CRITICAL();
	
	/* GPIO CS diset output */
	FIO1DIR = FIO1DIR | port_cs_ad7708;
	/* port 2, nanti ganti ke mode interrupt */
	FIO2DIR = FIO2DIR & ~port_rdy_ad7708;
	
	uncs_ad7708();
	
	
	//portEXIT_CRITICAL();
}
#endif

#endif
