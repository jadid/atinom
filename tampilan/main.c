/*
	FreeRTOS.org V5.1.1 - Copyright (C) 2003-2008 Richard Barry.
	
	entri dari boot.s adalah ke main
	
*/


/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define BAUD_RATE	( ( unsigned portLONG ) 115200 )
#define LED_UTAMA	(1 << 26)			// GPIO026

xSemaphoreHandle lcd_sem;


static char tog;
static void sysInit(void);
void init_led_utama(void);

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

	FIO0DIR = LED_UTAMA;
	FIO0CLR = LED_UTAMA;
	
	init_port_lcd();
	init_lcd();
	
	
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

	xSerialPortInitMinimal( BAUD_RATE, 128 );
	//init_gpio();


#ifdef jalankan
	init_led_utama();
	init_task_lcd();
	start_ether();
	init_shell();
	init_task_tampilan();

	vTaskStartScheduler();

    /* Will only get here if there was insufficient memory to create the idle
    task. */
	return 0;
#endif
}

//int jum=0;

void togle_led_utama(void)
{
	//jum++;
	if (tog)
	{
		//FIO1CLR = (1 << 19);
		FIO0SET = LED_UTAMA;
		//tes_high();
		tog = 0;
		//xSemaphoreGive( lcd_sem );
	}
	else
	{
		//FIO1SET = (1 << 19);
		FIO0CLR = LED_UTAMA;
		//tes_low();
		tog = 1;
		//teks_h(20, 60, "ini langsung ke LCD");
		//printf_lcd("ini %d", jum);
		sambungan_connect();
	}
}

#define BACKLIT		BIT(20)	// PF15, P1.20

static portTASK_FUNCTION(task_led2, pvParameters )
{
	tog = 0;
	
	vTaskDelay(100);
	FIO1SET |= BACKLIT;

	for (;;)
	{
		togle_led_utama();
		vTaskDelay(2000);
	}
}
void init_led_utama(void)
{
	xTaskCreate(task_led2, ( signed portCHAR * ) "Led2", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY - 2, ( xTaskHandle * ) NULL );
}



/* LCD LCD LCD */

portTASK_FUNCTION( LCD_task, pvParameters )
{
	int lewat=0;
	vSemaphoreCreateBinary( lcd_sem );
    xSemaphoreTake( lcd_sem, 0 );
	for(;;) {
	    //while( xSemaphoreTake( lcd_sem, 100 ) != pdPASS );
		if ( xSemaphoreTake( lcd_sem, 100 ) == pdTRUE )
		{
			//printf("*LCD_task*\n");
			update_hard_lcd();
			//vTaskDelay( 5 );
		}
		else
		{
			lewat++;
			if (lewat > 100)
			{
				//printf("lewat > 100\n");
				lewat = 0;	
			}
		}
	}
}

void init_task_lcd(void)
{
	xTaskCreate( LCD_task, ( signed portCHAR * ) "LCD", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL );	
}



