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

#include <math.h>

#define BAUD_RATE	( ( unsigned portLONG ) 115200 )

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

xTaskHandle hdl_led;
xTaskHandle hdl_shell;
xTaskHandle hdl_ether;
xTaskHandle hdl_cnc;

static unsigned int dog_menit=0;
static unsigned int dog_lama = 0;
static unsigned int dog_hang = 0;
extern unsigned int loop_kirim;
unsigned int mati_total = 0;



/*-----------------------------------------------------------*/
#define jalankan

int main( void )	{
	sysInit();

	/* USB Power dinyalakan supaya memory USB bisa dipakai */
	PCONP |= 0x80000000; 
	
	FIO0DIR = LED_UTAMA;
	FIO0CLR = LED_UTAMA;	
	
	#if ( DEBUG_KONTER == 1) 
	FIO1DIR = FIO0DIR | LED_DEBUG;
	FIO1SET = LED_DEBUG;
	#endif
	
	FIO2DIR = 0x0;
	
	/*	untuk cek blinking saat system boot */

	xSerialPortInitMinimal( BAUD_RATE, configMINIMAL_STACK_SIZE  );

#ifdef BOARD_KOMON_KONTER
	init_gpio();
#endif

#ifdef jalankan
	init_led_utama();
	//start_ether();
	init_cnc();
	init_shell();
	
	vTaskStartScheduler();

    /* Will only get here if there was insufficient memory to create the idle
    task. */
	return 0;
#endif
}

void togle_led_utama(void)	{
	if (tog)	{
		//FIO0SET = LED_UTAMA;
		tog = 0;
		
		// kalkulasi idle loop //
		tot_idle = loop_idle - idle_lama;
		idle_lama = loop_idle;
		
		// reset wdog setiap detik //
		tendang_wdog();
		
	}
	else	{
		//FIO0CLR = LED_UTAMA;
		tog = 1;
	}
}

static portTASK_FUNCTION(task_led2, pvParameters )	{
	unsigned int muter=0;
	
	tog = 0;
	loop_idle = 0;
	idle_lama = 0;
	
	vTaskDelay(1000);
	
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
			FIO0PIN ^= LED_UTAMA;
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



static portTASK_FUNCTION(task_cnc, pvParameters )	{
	unsigned int muter=0, menit=0;

	vTaskDelay(400);
	
	int sudut=6;
	
	printf("Init CNC ...\r\n");
	
	PCONP |= BIT(22);						// Timer 2 aktif
	PCLKSEL1 &= ~(BIT(12) | BIT(13));		// PCLK = CCLK
	PCLKSEL1 |= BIT(12);
	init_timer2();
	
	FIO1DIR = BIT(31);
	FIO1CLR = BIT(31);
	
	FIO1DIR |= BIT(19);
	FIO1CLR |= BIT(19);	
	
	vTaskDelay(1000);
	tabelSin();
	//initPWM();
	FIO1SET = BIT(31);
	vTaskDelay(1000);
	for (;;)	{
		//
		
		muter++;
		if (muter>60) {
			printf("menit: %d\r\n", menit);
			muter=0;
			menit++;
		}
		vTaskDelay(1000);
	}
}

void init_cnc(void)	{
	xTaskCreate(task_cnc, ( signed portCHAR * ) "cnc",  (configMINIMAL_STACK_SIZE * 20) , NULL, \
		tskIDLE_PRIORITY + 3, ( xTaskHandle * ) &hdl_cnc );
}

void initPWM() {
/*
	PCONP |= BIT(6);
	PINSEL3 &= ~(BIT(5) | BIT(4));
	PINSEL3 |= BIT(5);
	
	PWM1TCR = PWMxTCR_Counter_Reset;		// reset dulu
	PWM1PR	= 60 - 1;						// set the prescale divider (60 / 60, 1 Mhz (1 us))
	
	PWM_MR0  = (configCPU_CLOCK_HZ / pwmPulseWidth);
  PWM_MCR |= PWM_MCR_MR0R;
  PWM_PCR |= PWM_PCR_ENA5;
  PWM_TCR  = (PWM_TCR_CE | PWM_TCR_PWME);
	
//	PWM1MR0	=
//*/
}

unsigned int nPWM[xPWM] = {};

void tabelSin() {

	int i,j=0;
	float rad, tmp;
	//float sudut;
	
	//printf("\r\n");
	for (i=0; i<xPWM; i++) {
		//sudut = (i*360.00)/xPWM;
		rad	  = ((i+0.75*xPWM)*PI*2.00)/xPWM;
		
		tmp = atan(2);
		tmp = (sin(rad)/2.00)+0.5;
		nPWM[i] = (int) (maxKonter*tmp);

		//printf("%.0f____%d\t", tmp*maxKonter, nPWM[i]);
		j++;
		if (j>7)	{
			j=0;
			//printf("\r\n");
		}
	}
	//printf("\r\n");


/*
	j=0;
	for (i=0; i<xPWM; i++) {
		printf("%.2f\t", nilai[i]);
		j++;
		if (j>15)	{
			j=0;
			printf("\r\n");
		}
	}
	printf("\r\n");
//*/
}
