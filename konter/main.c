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

	PCONP |= 0x80000000;	// USB power 

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

	xSerialPortInitMinimal( BAUD_RATE, 128 );	// 256 OK

#ifdef BOARD_KOMON
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
		//titik_siap++;
		//FIO1SET = (1 << 19);
		FIO0CLR = LED_UTAMA;
		//tes_low();
		tog = 1;
		//teks_h(20, 60, "ini langsung ke LCD");
		//printf_lcd("ini %d", jum);
		//if (titik_siap > 3)
		//	sambungan_connect();
	}
}

#define BACKLIT		BIT(20)	// PF15, P1.20

static portTASK_FUNCTION(task_led2, pvParameters )
{
	tog = 0;
	
	vTaskDelay(100);
	FIO1SET |= BACKLIT;

	vTaskDelay(2000);
	
	for (;;)
	{
		togle_led_utama();
		buat_file_index();
		vTaskDelay(1100);
	}
}
void init_led_utama(void)
{
	//xTaskCreate(task_led2, ( signed portCHAR * ) "Led2", 51 , NULL, tskIDLE_PRIORITY - 2, ( xTaskHandle * ) &hdl_led );
	xTaskCreate(task_led2, ( signed portCHAR * ) "Led2",  (configMINIMAL_STACK_SIZE * 8) , NULL, tskIDLE_PRIORITY - 2, ( xTaskHandle * ) &hdl_led );
}



