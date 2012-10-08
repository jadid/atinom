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

#ifdef PAKAI_TIMER_2
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

#ifdef PAKAI_KONTROL_RTC

extern unsigned char flagRTCc;

void init_rtc_irq(void)	{
	extern void rtc_ISR_Wrapper( void );
	//flagRTCc = 150;
	//printf("   %s masuk \r\n", __FUNCTION__);

	portENTER_CRITICAL();
	VICIntSelect &= ~(VIC_CHAN_TO_MASK(VIC_CHAN_NUM_RTC));		// set ke 0: IRQ [1: FOQ]
	VICIntEnClr  = VIC_CHAN_TO_MASK(VIC_CHAN_NUM_RTC);			// disable int
	VICVectAddr13 = ( portLONG )rtc_ISR_Wrapper;
	VICVectPriority13 = 0x02;
	VICIntEnable = VIC_CHAN_TO_MASK(VIC_CHAN_NUM_RTC);
	portEXIT_CRITICAL();
	
	//flagRTCc = 160;
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

//*
int setup_konter_onoff(unsigned int aaa, unsigned char statk) {
	//printf("\r\n aaa: %d, statk: %d\r\n", aaa, statk);
	int bbb = 0;
	if (statk==1) {
		/*
		if (aaa==0) {	IO2_INT_EN_R |= kont_1;		bbb = 101;	}
		if (aaa==1) {	IO2_INT_EN_R |= kont_2;		bbb = 102;	}
		//if (aaa==2) {	IO2_INT_EN_R |= kont_3;		bbb = 103;	}
		if (aaa==3) {	IO2_INT_EN_R |= kont_4;		bbb = 104;	}
		if (aaa==4) {	IO2_INT_EN_R |= kont_5;		bbb = 105;	}
		if (aaa==5) {	IO2_INT_EN_R |= kont_6;		bbb = 106;	}
		if (aaa==6) {	IO2_INT_EN_R |= kont_7;		bbb = 107;	}
		if (aaa==7) {	IO2_INT_EN_R |= kont_8;		bbb = 108;	}
		if (aaa==8) {	IO2_INT_EN_R |= kont_9;		bbb = 109;	}
		if (aaa==9) {	IO2_INT_EN_R |= kont_10;	bbb = 110;	}
		//*/
	} else if (statk==3)	{
		if (aaa==0) {	;	bbb = 201;	}
		if (aaa==1) {	;	bbb = 202;	}
		if (aaa==2) {	;	bbb = 203;	}
		if (aaa==3) {	;	bbb = 204;	}
		if (aaa==4) {	;	bbb = 205;	}
		if (aaa==5) {	;	bbb = 206;	}
		if (aaa==6) {	;	bbb = 207;	}
		if (aaa==7) {	;	bbb = 208;	}
		if (aaa==8) {	;	bbb = 209;	}
		if (aaa==9) {	;	bbb = 210;	}
	} else {
		if (aaa==0) {	IO2_INT_EN_R &= ~kont_1;	bbb = 1;	}
		if (aaa==1) {	IO2_INT_EN_R &= ~kont_2;	bbb = 2;	}
		if (aaa==2) {	IO2_INT_EN_R &= ~kont_3;	bbb = 3;	}
		if (aaa==3) {	IO2_INT_EN_R &= ~kont_4;	bbb = 4;	}
		if (aaa==4) {	IO2_INT_EN_R &= ~kont_5;	bbb = 5;	}
		if (aaa==5) {	IO2_INT_EN_R &= ~kont_6;	bbb = 6;	}
		if (aaa==6) {	IO2_INT_EN_R &= ~kont_7;	bbb = 7;	}
		if (aaa==7) {	IO2_INT_EN_R &= ~kont_8;	bbb = 8;	}
		if (aaa==8) {	IO2_INT_EN_R &= ~kont_9;	bbb = 9;	}
		if (aaa==9) {	IO2_INT_EN_R &= ~kont_10;	bbb = 10;	}
		
	}
	return bbb;
}
//*/

void init_gpio(void)
{
	//printf("   %s() masuk...", __FUNCTION__);
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


	// setup GPIO direction as input & interrupt
	FIO2DIR = FIO2DIR & ~(kont_10 | kont_9 | kont_8 | kont_7 | kont_6);
	
	#ifdef BOARD_KOMON_KONTER_3_1
	FIO2DIR |= FIO2DIR & ~(kont_1 | kont_2 | kont_3 | kont_4 | kont_5);
	#endif
	
	// enable falling edge interrupt
	// inverse input
	
	IO2_INT_EN_F = kont_10 | kont_9 | kont_8 | kont_7 | kont_6;
	
	#ifdef BOARD_KOMON_KONTER_3_1
	IO2_INT_EN_F |= kont_1 | kont_2 | kont_3 | kont_4 | kont_5;
	#endif
	
	#if (PAKAI_KONTROL == 1)
	/* untuk kontrol pintu, ketika ditutup / rising edge lagi juga di cek */
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
