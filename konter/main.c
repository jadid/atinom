/*
	FreeRTOS.org V5.1.1 - Copyright (C) 2003-2008 Richard Barry.
	
	entri dari boot.s adalah ke main
	
	20 feb 2009
	buat buffer file index.html dilakukan di 
	task led.
	
*/


/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define BAUD_RATE	( ( unsigned portLONG ) 115200 )

#ifdef TAMPILAN_LPC
#define LED_UTAMA	(1 << 26)			// GPIO026
#endif

#ifdef TAMPILAN_LPC_4
#define LED_UTAMA	BIT(27)
#endif

#ifdef BOARD_KOMON
#define LED_UTAMA	BIT(27)
#endif

#ifdef DEBUG_KONTER
#define LED_DEBUG	BIT(19)		// P1.19
#endif

xSemaphoreHandle lcd_sem;
unsigned int loop_idle=0;
unsigned int idle_lama;
unsigned int tot_idle;

static char tog;
static void sysInit(void);
void init_led_utama(void);

xTaskHandle hdl_lcd;
xTaskHandle hdl_led;
xTaskHandle hdl_tampilan;
xTaskHandle hdl_shell;
xTaskHandle hdl_ether;

#if (DEBUG_KONTER == 1)
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

static unsigned int led_konter; 

void togle_led_konter(void)
{
	if (led_konter)
	{
		led_konter = 0;
		FIO1SET = LED_DEBUG;
	}
	else
	{
		led_konter = 1;
		FIO1CLR = LED_DEBUG;
	}
}
#endif

/*-----------------------------------------------------------*/
#define jalankan

int main( void )
{
	sysInit();

	PCONP |= 0x80000000;	// USB power 

	FIO0DIR = LED_UTAMA;
	FIO0CLR = LED_UTAMA;	
	
	#if ( DEBUG_KONTER == 1) 
	FIO1DIR = FIO0DIR | LED_DEBUG;
	FIO1SET = LED_DEBUG;
	#endif
	
	FIO2DIR = 0x0;
	//FIO2DIR = 0xFFFFFFFF;
	
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

	xSerialPortInitMinimal( BAUD_RATE, 128 );	// 256 OK

#ifdef BOARD_KOMON_KONTER
	init_gpio();
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
		
		#ifdef BOARD_KOMON_KONTER
		/* tiap detik cek apakah rpm mati */
		//cek_gpio_lama();
		#endif
	}
	else
	{
		FIO0CLR = LED_UTAMA;
		tog = 1;
		
		/* tiap detik buat file index */
		buat_file_index();
	}
}

static portTASK_FUNCTION(task_led2, pvParameters )
{
	unsigned int muter=0;
	
	tog = 0;
	loop_idle = 0;
	idle_lama = 0;

	vTaskDelay(2000);
	
	for (;;)
	{
		//togle_led_utama();
		//vTaskDelay(500);
		
		/* 
			setiap 100 ms, cek apakah rpm masih jalan,
			dicek satu per satu, supaya tidak balapan 
			(race condition)
			
			kemudian setiap 500 jalankan togle led
			seperti biasa
			
			*/
		
		hitung_rpm();
		
		muter++;		
		if (muter > 5)
		{
			togle_led_utama();
			muter = 0;
		}	
		
		vTaskDelay(100);
	}
}
void init_led_utama(void)
{
	xTaskCreate(task_led2, ( signed portCHAR * ) "Led2",  (configMINIMAL_STACK_SIZE * 8) , NULL, tskIDLE_PRIORITY - 2, ( xTaskHandle * ) &hdl_led );
}



