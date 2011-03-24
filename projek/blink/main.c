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
char console_buf[1024];


static char tog;
static void sysInit(void);
void init_led_utama(void);
//void init_task_SER(void);

//xTaskHandle hdl_lcd;
xTaskHandle hdl_led;
//xTaskHandle hdl_tampilan;
xTaskHandle hdl_shell;
//xTaskHandle hdl_ether;

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

	//xSerialPortInitMinimal( BAUD_RATE, 32 );


#ifdef jalankan
	init_led_utama();
	//init_task_SER();
	//init_task_lcd();
	//start_ether();
	//init_shell();
	//init_task_tampilan();

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
		FIO0SET = LED_UTAMA;
		tog = 0;
		//xSemaphoreGive( lcd_sem );
	}
	else
	{
		FIO0CLR = LED_UTAMA;
		tog = 1;

	}
}

#define BACKLIT		BIT(20)	// PF15, P1.20

void kirim_s(char *s)
{
	strcpy(console_buf, *s);
	lcd_sem = 1;	
}


static portTASK_FUNCTION(task_led2, pvParameters )
{
	tog = 0;
	
	vTaskDelay(100);
	FIO1SET |= BACKLIT;
	
	//printf("\nSTART\n");

	for (;;)
	{
		togle_led_utama();
		vTaskDelay(1000);
		//xSemaphoreGive( lcd_sem );	
		//lcd_sem = 1;	
		//kirim_s("ini dia");
	}
}
void init_led_utama(void)
{
	xTaskCreate(task_led2, ( signed portCHAR * ) "Led2", 52 , NULL, tskIDLE_PRIORITY - 2, ( xTaskHandle * ) &hdl_led );
}
/*
#include <stdarg.h>
int printf2 (const char *fmt, ...)
{
   va_list args;
   uint i;
   char printbuffer[64];

   va_start (args, fmt);

   i = vsprintf (printbuffer, fmt, args);
   va_end (args);

   //serial_puts(printbuffer);
   return 0;
} 

portTASK_FUNCTION( SER_task, pvParameters )
{
	int lewat=0;
	char t[46];
	
	xSerialPortInitMinimal( BAUD_RATE, 32 );
	
	printf("\n start SER_task \n");
	vSemaphoreCreateBinary( lcd_sem );
    xSemaphoreTake( lcd_sem, 0 );
	
	for(;;) 
	{
		if ( xSemaphoreTake( lcd_sem, 100 ) == pdTRUE )
		{
			lewat++;
			//UART0_THR = 'd';
			//serial_puts("semput");
			printf("semapor %d, %f", lewat, 1.12401);
			//printf("%s", console_buf);
			//printf("*LCD_task*\n");
			//update_hard_lcd();
			//lewat = 0;
		}
	}
}

void init_task_SER(void)
{
	xTaskCreate( SER_task, ( signed portCHAR * ) "LCD", (configMINIMAL_STACK_SIZE * 2), NULL, tskIDLE_PRIORITY + 1, (xTaskHandle *) &hdl_lcd );	
}
//*/
