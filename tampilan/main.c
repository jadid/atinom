/*
	FreeRTOS.org V5.1.1 - Copyright (C) 2003-2008 Richard Barry.
	
	entri dari boot.s adalah ke main
	
	27 Feb 2009
	entah kenapa keypad nya sering ngacau
	OK .. keypad sudah mulai bener
	
	28 Feb 2009
	dirapikan 
	
	set_titik diberikan tambahan alamat stack/board
	
	sementara sumber.alamat belum dipakai, nanti saat request 
	dari sumber yang banyak alamatnya, sumber/modul/server nya yang
	melakukan urutan pengiriman dari masing2 boardnya.
	
	keterangan masing2 titik langsung didapat dari struct keter
	
	led depan diaktifkan
	
	huh .. pusing keypad gak beres2.
	keypad pakai interrupt saja
	
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

/* led di keypad */
#define LED_PICKUP	BIT(14)
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

	/* reset semua port dulu */
	FIO2DIR  = 0xFFFFFFFF;
	FIO2MASK = 0xFFFFFFFF; 
	
	FIO1DIR  = 0xFFFFFFFF;
	FIO1MASK = 0xFFFFFFFF; 

	PCONP |= 0x80000000;	// USB power 

	FIO0DIR = LED_UTAMA;
	FIO0CLR = LED_UTAMA;
	
	FIO1DIR  = FIO1DIR | LED_PICKUP;
	FIO1MASK = FIO1MASK & ~(LED_PICKUP);
	
	//init_gpio_keypad();
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

	xSerialPortInitMinimal( BAUD_RATE, configMINIMAL_STACK_SIZE );	// 256 OK

#if 1
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
unsigned int waktu_buat_file=0;

void togle_led_utama(void)
{
	if (tog)
	{
		FIO0SET = LED_UTAMA;
		FIO1SET = LED_PICKUP;
		tog = 0;
		
		/* kalkulasi idle loop */
		tot_idle = loop_idle - idle_lama;
		idle_lama = loop_idle;
	}
	else
	{
		FIO0CLR = LED_UTAMA;
		FIO1CLR = LED_PICKUP;
		tog = 1;
		
		/* tiap detik buat file index */
		//waktu_buat_file++;
		//if (waktu_buat_file > 10)
		  buat_file_index();
	}
}

#define BACKLIT		BIT(20)	// PF15, P1.20

static portTASK_FUNCTION(task_led2, pvParameters )
{
	tog = 0;
	loop_idle = 0;
	idle_lama = 0;
	
	vTaskDelay(100);
	FIO1SET |= BACKLIT;

	for (;;)
	{
		togle_led_utama();
		//vTaskDelay(2000);
		vTaskDelay(500);
	}
}
void init_led_utama(void)
{
	//xTaskCreate(task_led2, ( signed portCHAR * ) "Led2", 51 , NULL, tskIDLE_PRIORITY - 2, ( xTaskHandle * ) &hdl_led );
	xTaskCreate(task_led2, ( signed portCHAR * ) "Led2",  (configMINIMAL_STACK_SIZE * 6) , \
		NULL, tskIDLE_PRIORITY - 2, ( xTaskHandle * ) &hdl_led );
}



/* LCD LCD LCD */

portTASK_FUNCTION( LCD_task, pvParameters )
{
	int lewat=0;
	
	#if 0
	char t[46];
	unsigned int hari=0;
	unsigned int jam=0;
	unsigned int menit=0;
	unsigned int detik=0;
	
	extern unsigned long tick_ku;
	#endif
	
	vSemaphoreCreateBinary( lcd_sem );
    xSemaphoreTake( lcd_sem, 0 );
	
	for(;;) {
		if ( xSemaphoreTake( lcd_sem, 100 ) == pdTRUE )
		{
			//printf("*LCD_task*\n");
			update_hard_lcd();
			lewat = 0;
		}
		else
		{
			lewat++;
			if (lewat > 10) /* 1 detik */
			{
				//update_hard_lcd();
				/*
				teks_clear(10, 232, 15);
				
				detik = tick_ku / 1000;
				if (detik > 60)
				{
					menit = detik / 60;
					if (menit > 60)
					{
						jam = menit / 24;
						
						if (jam > 24)
						{
							hari = jam / 24;	
						}	
					}	
				}
				
				//sprintf(t, "%d", tick_ku);
				//sprintf(t, "%d:%d:%d", jam, (menit % 60), (detik % 60));
				menit = menit % 60;
				detik = detik % 60;
				sprintf(t, "%d:%d:%d  ", jam, menit, detik);
				
				//	sprintf(t, "%d", (detik % 60));
				teks_h_hard(10, 232, t);
				
				//printf("\nLCD %d", uxTaskGetStackHighWaterMark(NULL));
				//
				//printf("lewat > 100\n");
				*/
				
				lewat = 0;	
			}
		}
	}
}

void init_task_lcd(void)
{
	xTaskCreate( LCD_task, ( signed portCHAR * ) "LCD", (configMINIMAL_STACK_SIZE * 1), \
		NULL, tskIDLE_PRIORITY + 1, (xTaskHandle *) &hdl_lcd );	
}

