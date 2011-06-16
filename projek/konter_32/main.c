/*
	FreeRTOS.org V5.1.1 - Copyright (C) 2003-2008 Richard Barry.
	
	entri dari boot.s adalah ke main
	
	20 feb 2009
	buat buffer file index.html dilakukan di 
	task led.
	
	21 April 2009
	Clean up untuk release stable versi 1.4
	
	19 Juni 2009, malimping
	ADC internal dijalankan
	ingat bahwa adc1 keluar adalah adc0 internal
	
	26 Juni 2009, Malimping
	sudah ok, data adc1 dimasukkan pada data kanal1
	
	28 Juni 2009, Malimping
	Blok_3 untuk drive ke penduduk, jika level lebih dari setting 24 Volt
*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "hardware.h"

unsigned int loop_idle=0;
unsigned int idle_lama;
unsigned int tot_idle;

static char tog;
static void sysInit(void);
void init_led_utama(void);

xTaskHandle hdl_led;

#ifdef PAKAI_SHELL
xTaskHandle hdl_shell;
#endif

#ifdef PAKAI_ETH
xTaskHandle hdl_ether;
#endif


static unsigned int dog_menit=0;
static unsigned int dog_lama = 0;
static unsigned int dog_hang = 0;
extern unsigned int loop_kirim;
unsigned int mati_total = 0;

/*-----------------------------------------------------------*/

int main( void )	{
	setup_hardware();				// @"../../hardware/hardware.h"
	
	init_led_utama();
	init_hardware();

	vTaskStartScheduler();

	printf("Keluar dari Scheduler !!!!\r\n");
    /* Will only get here if there was insufficient memory to create the idle task. */
	return 0;

}

void togle_led_utama(void)	{
	if (tog)	{
		FIO0SET = LED_UTAMA;
		tog = 0;
		
		// kalkulasi idle loop //
		tot_idle = loop_idle - idle_lama;
		idle_lama = loop_idle;
		
		// reset wdog setiap detik //
		tendang_wdog();
	}
	else	{
		FIO0CLR = LED_UTAMA;
		tog = 1;
	}
}

static portTASK_FUNCTION(task_led2, pvParameters )	{
	unsigned int muter=0;
	
	tog = 0;
	loop_idle = 0;
	idle_lama = 0;
	
	vTaskDelay(2000);
	
	for (;;)	{
		/* 
			setiap 100 ms, cek apakah rpm masih jalan,
			dicek satu per satu, supaya tidak balapan 
			(race condition)
			
			kemudian setiap 500 jalankan togle led
			seperti biasa
			
			*/
		
		//hitung_rpm();
		
		muter++;		
		if (muter > 5)		{
			togle_led_utama();
			muter = 0;
		}	
		
		vTaskDelay(100);
	}
}

void init_led_utama(void)	{
	xTaskCreate(task_led2, ( signed portCHAR * ) "Led2",  (configMINIMAL_STACK_SIZE * 2) , NULL, \
		tskIDLE_PRIORITY - 2, ( xTaskHandle * ) &hdl_led );
}
