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

#ifdef PAKAI_SHELL
xTaskHandle hdl_shell;
#endif

#ifdef PAKAI_ETH
xTaskHandle hdl_ether;
#endif

#ifdef PAKAI_TAMPILAN
xTaskHandle hdl_tampilan;
xTaskHandle hdl_lcd;
xSemaphoreHandle lcd_sem;
xSemaphoreHandle mutex_lcd_sem;
#endif

/*-----------------------------------------------------------*/
#define jalankan

int main( void )	{
	setup_hardware();				// @"../../hardware/hardware.h"
	init_hardware();

	init_led_utama();
	
	vTaskStartScheduler();

	printf("Keluar Scheduler !!!!\r\n");
    /* Will only get here if there was insufficient memory to create the idle task. */
	return 0;

}

void togle_led_utama(void)	{
	if (tog)	{
		FIO0SET = LED_UTAMA;
		#ifdef PAKAI_BACKLIT
			FIO1SET = LED_PICKUP;
			FIO1SET = BACKLIT;
		#endif
		tog = 0;
		
		/* kalkulasi idle loop */
		tot_idle = loop_idle - idle_lama;
		idle_lama = loop_idle;
		
		/* reset wdog setiap detik */
		tendang_wdog();
	}	else	{
		FIO0CLR = LED_UTAMA;
		#ifdef PAKAI_BACKLIT
			FIO1CLR = LED_PICKUP;
			FIO1CLR = BACKLIT;
		#endif
		tog = 1;
		//ser2_putstring("masuk ...\r\n");	
	}
}

static portTASK_FUNCTION(task_led2, pvParameters )	{
	tog = 0;
	loop_idle = 0;
	idle_lama = 0;
	FIO1SET = BACKLIT;
	
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

/* LCD LCD LCD */
portTASK_FUNCTION( LCD_task, pvParameters )
{
	vSemaphoreCreateBinary( lcd_sem );
    xSemaphoreTake( lcd_sem, 0 );
	
	/*
	mutex_lcd_sem = xSemaphoreCreateMutex();
	vTaskDelay(100);
	if (mutex_lcd_sem == NULL)
	{
		printf("Create mutex_copy ERROR !\r\n");
	}*/
	
	for(;;) 
	{
		if ( xSemaphoreTake( lcd_sem, 1000 ) == pdTRUE )	{
			update_hard_lcd();
		}
		
		//vTaskDelay(200);
		//update_hard_lcd();	
	}
}

void init_task_lcd(void)
{
	xTaskCreate( LCD_task, ( signed portCHAR * ) "LCD", (configMINIMAL_STACK_SIZE * 1), \
		NULL, tskIDLE_PRIORITY + 1, (xTaskHandle *) &hdl_lcd );	
}
