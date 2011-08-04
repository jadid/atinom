/*
	FreeRTOS.org V5.1.1 - Copyright (C) 2003-2008 Richard Barry.
	
	entri dari boot.s adalah ke main
	
	20 feb 2009
	buat buffer file index.html dilakukan di task led.
	
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
#include "hardware.h"


unsigned int loop_idle=0;
unsigned int idle_lama;
unsigned int tot_idle;

static int tog;
static void sysInit(void);
void init_led_utama(void);
//void init_spi_mmc(char min)	;

xTaskHandle hdl_led;
xTaskHandle hdl_shell;

#ifdef PAKAI_ETH
xTaskHandle hdl_ether;
#endif


/*-----------------------------------------------------------*/
#define jalankan

int main( void )	{
	setup_hardware();				// @"../../hardware/hardware.h"

	init_led_utama();
	init_hardware();

	vTaskStartScheduler();

	printf("Keluar dari Scheduler !!!!\r\n");
	vTaskDelay(4000);
    /* Will only get here if there was insufficient memory to create the idle task. */
	return 0;

}

void togle_led_utama(void)	{
	if (tog)	{
		FIO0SET = LED_UTAMA;
		tog = 0;
		
		/* kalkulasi idle loop */
		tot_idle = loop_idle - idle_lama;
		idle_lama = loop_idle;
		
		//printf("isi: %x: %d\r\n", &MEM_RTC0+1, *(&MEM_RTC0+1));
		
		/* reset wdog setiap detik */
		tendang_wdog();
	}	else	{
		FIO0CLR = LED_UTAMA;
		tog = 1;
		//ser2_putstring("masuk ...\r\n");	
	}
}



static portTASK_FUNCTION(task_led2, pvParameters )	{
	tog = 0;
	loop_idle = 0;
	idle_lama = 0;
	
	for (;;)	{
		togle_led_utama();
		vTaskDelay(500);
		
		//printf(" - Tanggal   : %d-%d-%d\r\n", infoGPS.utc.day, infoGPS.utc.mon, infoGPS.utc.year);
	}
}
void init_led_utama(void) {
	xTaskCreate(task_led2, ( signed portCHAR * ) "Led2",  (configMINIMAL_STACK_SIZE * 2) ,\
		 NULL, tskIDLE_PRIORITY - 2, ( xTaskHandle * ) &hdl_led );
}
