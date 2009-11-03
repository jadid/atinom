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

#define BAUD_RATE	( ( unsigned portLONG ) 115200 )

//#define CEK_BLINK

#ifdef BOARD_KOMON_A_RTD
#define LED_UTAMA	BIT(26)
#endif

#ifdef BOARD_KOMON_420_SAJA
#define LED_UTAMA	BIT(27)
#endif

xSemaphoreHandle lcd_sem;
unsigned int loop_idle=0;
unsigned int idle_lama;
unsigned int tot_idle;

static int tog;
static void sysInit(void);
void init_led_utama(void);
void init_spi_mmc(char min)	;

xTaskHandle hdl_lcd;
xTaskHandle hdl_led;
xTaskHandle hdl_tampilan;
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

#ifdef BOARD_KOMON_KONTER
	init_gpio();
#endif

	init_gpio_adc();
	
#ifdef BOARD_KOMON_420_SAJA
	/* init spi #1 untuk adc & MMC */
	init_spi_mmc(0);
#endif

#ifdef jalankan
	init_led_utama();
	start_ether();
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
		
		#ifdef BOARD_KOMON_KONTER
		/* tiap detik cek apakah rpm mati */
		cek_gpio_lama();
		#endif
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
void init_led_utama(void)
{
	xTaskCreate(task_led2, ( signed portCHAR * ) "Led2",  (configMINIMAL_STACK_SIZE * 2) ,\
		 NULL, tskIDLE_PRIORITY - 2, ( xTaskHandle * ) &hdl_led );
}



