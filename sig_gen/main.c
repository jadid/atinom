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
*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define BAUD_RATE	( ( unsigned portLONG ) 115200 )

#define CEK_BLINK

#ifdef BOARD_KOMON_A_RTD
#define LED_UTAMA	BIT(26)
#endif

#if (BOARD_SIG_GEN == 1)
#define LED_UTAMA	BIT(3)		/* DCD1 / P2.3 */
#endif

xSemaphoreHandle lcd_sem;
unsigned int loop_idle=0;
unsigned int idle_lama;
unsigned int tot_idle;

static int tog;
static void sysInit(void);
void init_led_utama(void);
void set_da(unsigned int d);

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

	//FIO0DIR = LED_UTAMA;
	//FIO0CLR = LED_UTAMA;
	FIO2DIR = LED_UTAMA;
	FIO2CLR = LED_UTAMA;	
		
	FIO1DIR = 0xFFFFFFFF;
	
	/* setup PINSEL1 untuk AOUT */
	PINSEL1 |= BIT(21);
	set_da(0);			/* supaya nol saat mulai nyala */
	
	/*	untuk cek blinking saat system boot */
#ifdef CEK_BLINK
	int t=0;
	while(t<5)
	{
		dele(1000000);
		FIO2CLR = LED_UTAMA;
		dele(1000000);
		FIO2SET = LED_UTAMA;
		t++;
	}
#endif

	xSerialPortInitMinimal( BAUD_RATE, configMINIMAL_STACK_SIZE  );

#ifdef BOARD_KOMON_KONTER
	init_gpio();
#endif

	//init_gpio_adc();

#ifdef jalankan
	init_led_utama();
	//init_sinus();
	init_shell();

	vTaskStartScheduler();

    /* Will only get here if there was insufficient memory to create the idle
    task. */
	return 0;
#endif
}

void set_da(unsigned int d)
{
	DACR = (d & 0xFFC0);	/* bit 0 sampai 5 harus nol */
}

void togle_led_utama(void)
{
	if (tog)
	{
		FIO2SET = LED_UTAMA;
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
		FIO2CLR = LED_UTAMA;
		tog = 1;		
	}
}

static portTASK_FUNCTION(task_led2, pvParameters )
{
	tog = 0;
	loop_idle = 0;
	idle_lama = 0;
	
	/* */
	gen_tabel();
	vTaskDelay(100);
	init_timer1();
	vTaskDelay(100);
	
	
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

#if (BOARD_SIG_GEN == 1)
unsigned short tabl[362];

unsigned int gen_sinus(int t)
{
	float rad = (float) (t * 2 * 3.14 / 360);
	float hasil = (float) (((sin(rad)/2) + 0.5) * 0xFFFF);	
	
	return (unsigned int) hasil;
}

/* generate tabel sinus saat booting */
void gen_tabel(void)
{
	int i;
	
	for (i=0; i<361; i++)
	{
		tabl[i] = (unsigned short) gen_sinus(i);
	}
}

/* 	
	timer1 untuk interupt periodik 18 KHz
	dimana 50 Hz * 360 data = 18000
	
	timer disuply oleh clock 60 MHz dibagi 4
	CCLK / 4 = 15 MHz
	
	15 MHz / 18 kHz = 833.3
	
*/
void init_timer1(void)
{
	portENTER_CRITICAL();
	PCLKSEL0 &= ~(BIT(4) | BIT(5));	// dinolkan dulu
	PCLKSEL0 |= BIT(4);				// di 01 kan
	
	T1TCR  = 2;         /* Stop and reset the timer */
	T1CTCR = 0;         /* Timer mode               */
	T1PR = 0;
	
	/* akan interrupt jika match */
	T1MR1 = 3333;
	
	/* Generate tick with timer 0 compare match. */
	T1MCR  = (3 << 3);  /* Reset timer on match and generate interrupt */
	
	/* Setup the VIC for the timer. */
	VICIntEnable |= 0x00000020;
	
	extern void ( sinus_interupt )( void );
	VICVectAddr5 = sinus_interupt;
	
	VICVectCntl5 = 1;

	/* Start the timer - interrupts are disabled when this function is called
	so it is okay to do this here. */
	T1TCR = 1;
	portEXIT_CRITICAL();
}

static portTASK_FUNCTION(task_sinus, pvParameters )
{	
	int time=0;
	unsigned int dat;
	vTaskDelay(1200);
	printf("Sinus Mulai\r\n");
	
	for (;;)
	{
		//set_da(tabl[time]);
		//time++;
		//if (time == 360) time = 0;
		
		vTaskDelay(1000);			// berarti ada 500 x per detik
	}
}
void init_sinus(void)
{
	xTaskCreate(task_sinus, ( signed portCHAR * ) "sinus",  (configMINIMAL_STACK_SIZE * 6) ,\
		 NULL, tskIDLE_PRIORITY, ( xTaskHandle * ) &hdl_led );
}
#endif

