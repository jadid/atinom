/*
	FreeRTOS.org V5.1.1 - Copyright (C) 2003-2008 Richard Barry.
	
	entri dari boot.s adalah ke main
	
	20 feb 2009
	buat buffer file index.html dilakukan di 
	task led.
	
	15 april 2009
	coba porting untuk Komon_A (RTD & 4-20 mA)	
	21 April 2009
	Clean up untuk release stable versi 1.4
	
	3 Nov 2009
	Untuk Komon_420 saja !
*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#define BAUD_RATE	( ( unsigned portLONG ) 115200 )	// 19200

//#define CEK_BLINK


#ifdef BOARD_KOMON_420_SABANG_2_3
#define LED_UTAMA	BIT(27)
#define TXDE	BIT(24)
#define RXDE	BIT(23)
#endif

//xSemaphoreHandle lcd_sem;
unsigned int loop_idle=0;
unsigned int idle_lama;
unsigned int tot_idle;

static int tog;
static void sysInit(void);
void init_led_utama(void);
//void init_spi_mmc(char min)	;

xTaskHandle hdl_led;
xTaskHandle hdl_shell;
xTaskHandle hdl_ether;

void dele(int dd)
{
	int g;
	int i;
	int dum;

	for (i=0; i<dd; i++)
	{
		dum = FIO0DIR;
	}
}


/*-----------------------------------------------------------*/
#define jalankan

int main( void )
{
	sysInit();

	/* USB Power dinyalakan supaya memory USB bisa dipakai */
	PCONP |= 0x80000000; 

	FIO0DIR = LED_UTAMA;
	FIO0CLR = LED_UTAMA;	
	
	FIO1DIR = 0xFFFFFFFF;

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

	/*	untuk cek blinking saat system boot */
#ifdef CEK_BLINK
	int t=0;
	while(t<5)
	{
		dele(1000000);
		FIO0CLR = LED_UTAMA;
		dele(1000000);
		FIO0SET = LED_UTAMA;
		t++;
	}
#endif

	xSerialPortInitMinimal( BAUD_RATE, configMINIMAL_STACK_SIZE  );
	#ifdef PAKAI_SERIAL_2
		serial2_init( BAUD_PM, (1 * configMINIMAL_STACK_SIZE) );
	#endif
	#ifdef PAKAI_SERIAL_3
		serial3_init( BAUD_PM, (1 * configMINIMAL_STACK_SIZE) );
	#endif

#ifdef PAKAI_ADC
	init_gpio_adc();
	init_gpio_mmc();
	init_spi_mmc(0);		// untuk adc dan mmc
#endif

#ifdef jalankan
	init_led_utama();
	start_ether();

#if (defined(PAKAI_PM) && defined(AMBIL_PM))
	//init_task_pm();
#endif
	init_shell();
	vTaskStartScheduler();

    /* Will only get here if there was insufficient memory to create the idle
    task. */
	return 0;
#endif
}

void togle_led_utama(void)
{
	if (tog)
	{
		FIO0SET = LED_UTAMA;
		tog = 0;
		
		/* kalkulasi idle loop */
		tot_idle = loop_idle - idle_lama;
		idle_lama = loop_idle;
		
		/* reset wdog setiap detik */
		tendang_wdog();

	}
	else
	{
		FIO0CLR = LED_UTAMA;
		tog = 1;		
	}
}

static portTASK_FUNCTION(task_led2, pvParameters )
{
	tog = 0;
	loop_idle = 0;
	idle_lama = 0;
	
	for (;;)
	{
		togle_led_utama();
		vTaskDelay(500);
	}
}
void init_led_utama(void) {
	xTaskCreate(task_led2, ( signed portCHAR * ) "Led2",  (configMINIMAL_STACK_SIZE * 2) ,\
		 NULL, tskIDLE_PRIORITY - 2, ( xTaskHandle * ) &hdl_led );
}
