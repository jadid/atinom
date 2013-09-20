
#ifndef __HARDWARE__
#define __HARDWARE__

#include "FreeRTOS.h"
#include "hardware.h"

//#include "uip/uip_task.c"

/*
 * dipanggil pertama kali ketika booting 
 * 
 */
void setup_hardware() {
	sysInit();
	
	/* USB Power dinyalakan supaya memory USB bisa dipakai */
	PCONP   |= 0x80000000;
	FIO1DIR  = 0xFFFFFFFF;
	
	#ifdef PAKAI_RELAY
		setup_relay();
	#endif
	
	#ifdef PAKAI_LED_UTAMA
		setup_led_utama();
	#endif
	
	#ifdef PAKAI_BACKLIT
		setup_backlit();
	#endif

	#ifdef PAKAI_ADC
		setup_adc();
	//	init_ss1();
	#endif
	
	#ifdef CEK_BLINK
		blink_led();
	#endif

	#ifdef PAKAI_SHELL
		#ifdef PAKAI_KONTROL_RTC
			#ifdef PAKAI_MODE_POWER
			if (status_power()==0)
			#endif
				xSerialPortInitMinimal( BAUD_RATE_SHELL, configMINIMAL_STACK_SIZE  );
		#else
			xSerialPortInitMinimal( BAUD_RATE_SHELL, configMINIMAL_STACK_SIZE  );
		#endif
	#endif
	
	#ifdef PAKAI_I2C
		setup_gpio_i2c();
	#endif
	
	#ifdef PAKAI_GPIO_DIMMER
		setup_timer2();
	#endif
	
	#ifdef PAKAI_TIMER_2
		setup_timer2();
	#endif
	
	#ifdef PAKAI_SERIAL_3
		/* PCONP enable UART3 */
		
		#ifdef PAKAI_SERIAL_3_P0
			setup_serial3_P0();
		#endif
		
		/* PCLK UART3, PCLK = CCLK */
		//PCLKSEL1 &= ~(BIT(18) | BIT(19));
		//PCLKSEL1 |= BIT(18);
		
	#endif

	#ifdef PAKAI_SERIAL_2			
		/* PCONP enable UART2 */

		
		#ifdef PAKAI_SERIAL_2_P0
			setup_serial2_P0();
		#endif
		
		/* PCLK UART2, PCLK = CCLK */
		//PCLKSEL1 &= ~(BIT(16) | BIT(17));
		//PCLKSEL1 |= BIT(16);
		
		/* init TX2, RX2 */
		//PINSEL0 |= (BIT(20) | BIT(22));
	#endif
	
	#ifdef PAKAI_ADC_ORI
		setup_adc_ori();
	#endif

}

void init_hardware()	{
	int hsl = 0;
	#ifdef PAKAI_ADC
		init_gpio_mmc();
		init_spi_mmc(0);		// untuk adc dan mmc
	#endif
	
	#if defined(BOARD_KOMON_KONTER) || defined(BOARD_KOMON_KONTER_3_0)
		init_gpio();
	#endif	
	
	#ifdef PAKAI_BACKLIT
		
	#endif
	
	#ifdef PAKAI_ETH
		start_ether();
	#endif
	
	#ifdef PAKAI_SHELL
		#ifndef BLINK
		// #if 0
		init_shell();
		#endif
	#endif

	#ifdef PAKAI_RTC
		//init_rtc();
	#endif

	#ifdef PAKAI_TAMPILAN
		init_port_lcd();
		init_lcd();
		init_task_lcd();		// 1, +1
		init_task_tampilan();
	#endif

	#ifdef PAKAI_ALARM	
		init_task_alarm();
	#endif

	#if 0
		#if (defined(PAKAI_PM) && defined(AMBIL_PM))
			init_task_pm();
		#endif
	#endif
	
	#ifdef PAKAI_SERIAL_2_P0
		serial2_init( PAKAI_SERIAL_2_P0, (1 * configMINIMAL_STACK_SIZE) );
	#endif
	
	#ifdef PAKAI_SERIAL_3_P0
		hsl = serial3_init( PAKAI_SERIAL_3_P0, (1 * configMINIMAL_STACK_SIZE) );

		printf("init serial 3 .... %d : SUKSES\r\n", hsl);
		//ser3_putstring("init kirim via 3\r\n");
	#endif
	
	#ifdef PAKAI_I2C
		
	#endif
	
	#ifdef PAKAI_TIMER_2
		printf("init timer 2\r\n");
		init_timer2();
	#endif
	
//***************************************************************/
/*
 *						APLIKASI ada di sini 
 */
//***************************************************************/
	//init_ambilcepat();
	
	#ifndef BLINK
	#ifndef BOARD_TAMPILAN
	
	//init_kirimcepat();
	#endif
	#endif
	
	#ifdef PAKAI_MODBUS_SLAVE
	//	init_task_modbus();
	#endif
}

void dele(int dd)	{
	int g;
	int i;
	int dum;
		for (i=0; i<dd; i++)	{
		dum = FIO0DIR;
	}
}

#ifdef CEK_BLINK
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
