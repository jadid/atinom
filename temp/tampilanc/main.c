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
	
	21 April 2009
	Clean up untuk release stable versi 1.4
	
		 
	3 Juli 2009
	Coba menggunakan Modbus
	modbus memakai MAX485 dan UART3
	
	30 Nov 2009
	untuk tampilan lipi : malingping
	
	19 Jan 2010
	Coba untuk GSM FTP
	
	11 Mar 2010
	tendang_wdog dipindah ke task LCD
*/


/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define BAUD_RATE	( ( unsigned portLONG ) 115200 )

#ifdef TAMPILAN_LPC_43
#define LED_UTAMA	BIT(27)

/* led di keypad */
#define LED_PICKUP	BIT(14)

#define BACKLIT		BIT(20)	// PF15, P1.20
#endif

xSemaphoreHandle lcd_sem;
xSemaphoreHandle mutex_lcd_sem;

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


xTaskHandle hdl_relay;



#define TXDE	BIT(24)

void dele(int dd)	{
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

	/* USB Power dinyalakan supaya memory USB bisa dipakai */
	PCONP |= 0x80000000;

	FIO0DIR = LED_UTAMA;
	FIO0CLR = LED_UTAMA;
	
	FIO1DIR  = FIO1DIR | LED_PICKUP | BACKLIT;
	FIO1MASK = FIO1MASK & ~(LED_PICKUP | BACKLIT);
	
	init_port_lcd();
	init_lcd();
	
	#ifdef PAKAI_SERIAL_3
	/* PCONP enable UART3 */
	PCONP |= BIT(25);
	
	/* PCLK UART3 PCLK = CCLK */
	PCLKSEL1 &= ~(BIT(18) | BIT(19));		// reset
	PCLKSEL1 |= BIT(18);
	
	/* init TX3, RX3 */
	PINSEL1 &= ~(BIT(18) | BIT(19) | BIT(20) | BIT(21));
	PINSEL1 |= (BIT(18) | BIT(19));
	PINSEL1 |= (BIT(20) | BIT(21));
	
	/* TXDE di highkan */
	FIO0DIR |= TXDE;
	FIO0SET  = TXDE;
	#endif
	
	/*	untuk cek blinking saat system boot */
#ifdef CEK_BLINK
	int t=0;
	while(t<5)	{
		dele(1000000);
		FIO0CLR = LED_UTAMA;
		dele(1000000);
		FIO0SET = LED_UTAMA;
		t++;
	}
#endif

	xSerialPortInitMinimal( BAUD_RATE, (1 * configMINIMAL_STACK_SIZE) );

	#ifdef PAKAI_SERIAL_3
		serial3_init( BAUD_RATE, (1 * configMINIMAL_STACK_SIZE) );		// serial3_init
	#endif

#if 1
	init_led_utama();		// 5, -2
	init_task_lcd();		// 1, +1
	start_ether();
	init_shell();			// 10, 0
	init_task_tampilan();	// 10, -1
#ifdef PAKAI_PM
	init_task_pm();			// 10, +1
#endif

	vTaskStartScheduler();

    /* Will only get here if there was insufficient memory to create the idle
    task. */
	return 0;
#endif
}

unsigned int waktu_buat_file=0;
int saat_gsm_ftp;
int timer_saat_gsm;

void togle_led_utama(void)
{
	if (tog)
	{
		//printf("masuk toogle %d\r\n", tog);
		FIO0SET = LED_UTAMA;
		FIO1SET = LED_PICKUP;
		tog = 0;
		
		/* kalkulasi idle loop */
		tot_idle = loop_idle - idle_lama;
		idle_lama = loop_idle;
		
		/* reset wdog setiap detik */
		tendang_wdog();
		
		/* timer_saat_gsm diclock setiap detik */
		timer_saat_gsm++;
		//if (timer_saat_gsm > (60 * 10))
		if (timer_saat_gsm > (30))
		{
			saat_gsm_ftp = 1;
			timer_saat_gsm = 0;
		}

	}
	else
	{
		FIO0CLR = LED_UTAMA;
		FIO1CLR = LED_PICKUP;
		tog = 1;
		
		#if (PAKAI_SELENOID == 1)
		//unset_selenoid( 1 );
		#endif
	}
}

//#include "../monita/monita_uip.h"

static portTASK_FUNCTION(task_led2, pvParameters )
{
	int i;
	int c;
	int x;
	
	tog = 0;
	loop_idle = 0;
	idle_lama = 0;
	timer_saat_gsm = 0;
	
	vTaskDelay(500);
	FIO1SET = BACKLIT;

	for (;;)
	{
		togle_led_utama();
		vTaskDelay(500);
	}
}
void init_led_utama(void)
{	
	/* tadinya stak = 5, prio - 1 */
	xTaskCreate(task_led2, ( signed portCHAR * ) "Led2",  (configMINIMAL_STACK_SIZE * 4) , \
		NULL, tskIDLE_PRIORITY, ( xTaskHandle * ) &hdl_led );
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
		if ( xSemaphoreTake( lcd_sem, 1000 ) == pdTRUE )
		{
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
