
#ifndef __HARDWARE__
#define __HARDWARE__

#include "FreeRTOS.h"
#include "hardware.h"

void setup_hardware() {
	sysInit();
	
	/* USB Power dinyalakan supaya memory USB bisa dipakai */
	PCONP |= 0x80000000;
	FIO1DIR = 0xFFFFFFFF;
	
	#ifdef PAKAI_LED_UTAMA
		setup_led_utama();
	#endif
	
	#ifdef PAKAI_ADC
		setup_adc();
	#endif
	
	#ifdef CEK_BLINK
		blink_led();
	#endif

	#ifdef PAKAI_SHELL
		xSerialPortInitMinimal( BAUD_RATE_SHELL, configMINIMAL_STACK_SIZE  );
	#endif
	
	#ifdef PAKAI_SERIAL_2_P0
		setup_serial2_P0();
	#endif
	
		
	#ifdef PAKAI_SERIAL_3
		/* PCONP enable UART3 */
		PCONP |= BIT(25);
		
		/* PCLK UART3, PCLK = CCLK */
		PCLKSEL1 &= ~(BIT(18) | BIT(19));
		PCLKSEL1 |= BIT(18);
		
		/* init TX3, RX3 */
		PINSEL1 &= ~(BIT(18) | BIT(19) | BIT(20) | BIT(21));
		PINSEL1 |= (BIT(18) | BIT(19));
		PINSEL1 |= (BIT(20) | BIT(21));
		PINSEL1 &= ~(BIT(16) | BIT(17));
		/* TXDE di highkan */
		FIO0DIR |= TXDE;
		//FIO0SET = TXDE;		// on	---> bisa kirim
		//FIO0SET &= ~TXDE;		// off	---> gak bisa kirim
		//FIO0CLR = TXDE;
		FIO0SET = TXDE;
		
		FIO0DIR |= RXDE;
		FIO0SET  = RXDE;
	#endif

	#ifdef PAKAI_SERIAL_2
		/* PCONP enable UART2 */
		PCONP |= BIT(24);
		
		/* PCLK UART2, PCLK = CCLK */
		PCLKSEL1 &= ~(BIT(16) | BIT(17));
		PCLKSEL1 |= BIT(16);
		
		/* init TX2, RX2 */
		PINSEL0 |= (BIT(20) | BIT(22));
	#endif

}

void init_hardware()	{
	#ifdef PAKAI_ADC
		//init_gpio_adc();
		init_gpio_mmc();
		init_spi_mmc(0);		// untuk adc dan mmc
	#endif
	
	#ifdef PAKAI_ETH
		start_ether();
	#endif
	
	#ifdef PAKAI_SHELL
		init_shell();
	#endif
	
	#if (defined(PAKAI_PM) && defined(AMBIL_PM))
		init_task_pm();
	#endif
	
	#ifdef PAKAI_SERIAL_2_P0
		serial2_init( PAKAI_SERIAL_2_P0, (1 * configMINIMAL_STACK_SIZE) );
	#endif
	#ifdef PAKAI_SERIAL_3_P0
		serial3_init( PAKAI_SERIAL_3_P0, (1 * configMINIMAL_STACK_SIZE) );
	#endif
	
//***************************************************************/
/*
 *						APLIKASI ada di sini 
 */
//***************************************************************/
	
	init_ambilcepat();
}

#ifdef CEK_BLINK
	void dele(int dd)	{
		int g;
		int i;
		int dum;

		for (i=0; i<dd; i++)	{
			dum = FIO0DIR;
		}
	}

	void blink_led(void)	{
		int t=0;
		while(t<5)
		{
			dele(1000000);
			FIO0CLR = LED_UTAMA;
			dele(1000000);
			FIO0SET = LED_UTAMA;
			t++;
		}
	}
#endif

#endif
