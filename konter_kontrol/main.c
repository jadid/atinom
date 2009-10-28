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
	
	24 Juli 2009, Jakarta
	dicopy dari konter, untuk coba kontrol di kantor
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

xTaskHandle hdl_lcd;
xTaskHandle hdl_led;
xTaskHandle hdl_tampilan;
xTaskHandle hdl_shell;
xTaskHandle hdl_ether;

#if (DEBUG_KONTER == 1)
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

static unsigned int led_konter; 

void togle_led_konter(void)
{
	if (led_konter)
	{
		led_konter = 0;
		FIO1SET = LED_DEBUG;
	}
	else
	{
		led_konter = 1;
		FIO1CLR = LED_DEBUG;
	}
}
#endif
/******************* 18 Juni 2009 **************************/

static unsigned int dog_menit=0;
static unsigned int dog_lama = 0;
static unsigned int dog_hang = 0;
extern unsigned int loop_kirim;
unsigned int mati_total = 0;

/* konektor P8 */
#define P8_1		BIT(19)
#define P8_2		BIT(20)
#define P8_3		BIT(21)
#define P8_4		BIT(22)
#define P8_5		BIT(23)
#define P8_6		BIT(24)

#define PCADC		BIT(12)
#define ADC_CLKDIV	(250 << 8)
#define ADC_PDN		BIT(21)
#define ADC_START	BIT(24)
#define ADC_1		BIT(0)
#define ADC_2		BIT(1)
#define ADC_DONE	BIT(31)

void init_gpio_relay(void)
{
	FIO1DIR = FIO1DIR | P8_1 | P8_2 | P8_3 | P8_4 | P8_5 | P8_6;
}

void relay_A_on(void)
{
	FIO1SET = P8_1;
}

void relay_B_on(void)
{
	FIO1SET = P8_2;
}

void relay_C_on(void)
{
	FIO1SET = P8_3;
}

void relay_D_on(void)
{
	FIO1SET = P8_4;
}

/* mematikan relay */
void relay_A_off(void)
{
	FIO1CLR = P8_1;
}

void relay_B_off(void)
{
	FIO1CLR = P8_2;
}

void relay_C_off(void)
{
	FIO1CLR = P8_3;
}

void relay_D_off(void)
{
	FIO1CLR = P8_4;
}
#if 0
void blok_1_up(void)
{
	FIO1SET = (P8_1 | P8_2);
}

void blok_1_down(void)
{
	FIO1CLR = (P8_1 | P8_2);
}

void blok_2_up(void)
{
	FIO1SET = (P8_3 | P8_4);
}

void blok_2_down(void)
{
	FIO1CLR = (P8_3 | P8_4);
}

void blok_3_up(void)
{
	FIO1SET = (P8_5 | P8_6);
}

void blok_3_down(void)
{
	FIO1CLR = (P8_5 | P8_6);
}
#endif

void setup_adc(void)
{
	/* 19 Juni 2009, ADC power dinyalakan */
	PCONP |= PCADC;
	
	/* PCLK ADC pakai default (00) atau CCLK / 4 */
	
	/* setup PINSEL1 utk AD0 & AD1 */
	PINSEL1 |= (BIT(14) | BIT(16));
}

void start_adc_1(void)
{
	AD0CR = (ADC_CLKDIV | ADC_PDN | ADC_START | ADC_1);
}

void start_adc_2(void)
{
	AD0CR = (ADC_CLKDIV | ADC_PDN | ADC_START | ADC_2);
}

unsigned short read_adc_1(void)
{
	unsigned int stat;
	/*
	stat = AD0STAT;
	
	if (stat & 0x01)
	{
		stat = AD0DR0;	
		return (stat & 0xFFFF);
	}
	else
		return 0;
	*/
	
	stat = AD0DR0;
	if (stat & ADC_DONE)
	{
		return (stat & 0xFFFF);
	}
	else
		return 0;
}

unsigned short read_adc_2(void)
{
	unsigned int stat;
	
	stat = AD0DR1;
	if (stat & ADC_DONE)
	{
		return (stat & 0xFFFF);
	}
	else
		return 0;
}

/*-----------------------------------------------------------*/
#define jalankan



int main( void )
{
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

	xSerialPortInitMinimal( BAUD_RATE, configMINIMAL_STACK_SIZE  );

#ifdef BOARD_KOMON_KONTER
	init_gpio();
#endif
	init_gpio_relay();
	setup_adc();

#ifdef jalankan
	init_led_utama();
	start_ether();
	init_shell();
	
	#if (KONTER_MALINGPING == 1)
	init_task_adc();
	#endif

	vTaskStartScheduler();

    /* Will only get here if there was insufficient memory to create the idle
    task. */
	return 0;
#endif
}

int brace_stat;

void togle_led_utama(void)
{
	if (tog)
	{
		FIO0SET = LED_UTAMA;
		tog = 0;
		
		/* kalkulasi idle loop */
		tot_idle = loop_idle - idle_lama;
		idle_lama = loop_idle;
		
		/* reset wdog setiap detik */
		tendang_wdog();
	
		#if 0	
		/* wdog malimping */
		dog_menit++;
		if (dog_menit > (60 * 5)) /* 5 menit */
		//if (dog_menit > 10) /* 5 menit */
		{
			/* 	jika 5 menit tidak ada perubahan jumlah request
			 	berarti server ngehang */
			 	
			if (dog_lama == loop_kirim)
			{
				/* 
					normally close, sehingga jadi open
					supply tegangan ke server putus, jadi hard reset */
				
				if (mati_total == 0)	
					blok_2_up();						
				
				dog_hang = 1;
			}
			else
			{
				dog_lama = loop_kirim;
				dog_hang = 0;
			}
			dog_menit = 0;
		}
		
		/* tunggu sekitar 10 detik baru release relaynya */
		if (dog_hang == 1 && dog_menit > 5)
		{
			/* release relay, sehingga close lagi */
			if (mati_total == 0)
				blok_2_down();
			
			dog_hang = 0;
		}
		
		#endif
		
		#if 0
		relay_A_off();
		delay(100);
		relay_B_off();
		delay(100);
		relay_C_off();
		delay(100);
		relay_D_off();
		#endif
		
		if (brace_stat == 1)
			relay_C_on();
	}
	else
	{
		FIO0CLR = LED_UTAMA;
		tog = 1;
		
		if (brace_stat == 1)
			relay_C_off();
			
		#if 0
		relay_A_on();
		delay(100);
		relay_B_on();
		delay(100);
		relay_C_on();
		delay(100);
		relay_D_on();
		#endif
		
		//unsigned short d_adc = read_adc_1();	
		//printf("ADC 1 = 0x%4X = %.3f\r\n", d_adc, (float) (d_adc * 3.3 / 0xFFFF));
	}
}

//int armed = 0;
extern unsigned char koil_stat[];
extern stat_input[];
extern int jum_tutup;
		
static portTASK_FUNCTION(task_led2, pvParameters )
{
	unsigned int muter = 0;
	int wait_30 = 0;
	int warning = 0;
	int armed = 0;

	
	tog = 0;
	loop_idle = 0;
	idle_lama = 0;
	brace_stat = 0;
	
	vTaskDelay(2000);
	
	#if (PAKAI_KONTROL == 1)
	
	for (tog = 0; tog < 8; tog++)
		koil_stat[tog] = 0;
	
	/* reset stat_input */	
	
	for (tog=0; tog<= 10; tog++)
		stat_input[tog] = 0;
	
	jum_tutup = 0;
	
	#endif
	
	tog = 0;
	
	for (;;)
	{		
		/* 
			setiap 100 ms, cek apakah rpm masih jalan,
			dicek satu per satu, supaya tidak balapan 
			(race condition)
			
			kemudian setiap 500 jalankan togle led
			seperti biasa
			
			*/
		
		#if (PAKAI_KONTROL == 0)
		hitung_rpm();
		#endif
		
		muter++;		
		if (muter > 5)
		{
			togle_led_utama();
			muter = 0;
		}	
		
		vTaskDelay(100);
		
		#if (PAKAI_KONTROL == 1)
		/*
		extern unsigned char koil_stat[];
		if (koil_stat[0] == 0) relay_A_off();
		else if (koil_stat[0] == 1) relay_A_on(); 
		
		if (koil_stat[1] == 0) relay_B_off();
		else if (koil_stat[1] == 1) relay_B_on(); 
		
		if (koil_stat[2] == 0) relay_C_off();
		else if (koil_stat[2] == 1) relay_C_on(); 
		
		if (koil_stat[3] == 0) relay_D_off();
		else if (koil_stat[3] == 1) relay_D_on(); 
		*/

		
		/* 
		 * untuk menyalakan, armed = 1, tunggu sampai 30 detik
		 * keluar, tutup pintu, dan OK aktif
		 */
		if ( stat_input[9] == 1)
		{
			wait_30 ++;
			if (wait_30 == 300)
			{
				armed = 1;
				jum_tutup = 0;
				warning = 0;
				/* relay B diaktifkan untuk nyalain apa saja */
				relay_B_on();
			}
		}
		
		/* 
		 * berarti sudah armed dan ada yang buka pintu 
		 * tunggu 20 detik untuk matikan armed
		 */
		if (armed == 1 && jum_tutup > 0)
		{
			warning++;
			if (warning == 300)
			{
				if (stat_input[9] == 1)
				{
					/* brace brace .. somebody enter */
					relay_A_on();
					brace_stat = 1;
				}
				else
				{
					/* 
					 * ok, berarti armed di nolkan 
					 * alarm tidak sempat berbunyi
					 */
					armed = 0;
					wait_30 = 0;
					jum_tutup = 0;
					relay_B_off();
				}
			}
		}
		
		
		/* untuk mereset jika sudah kadung bunyi, 
		 * armed = 0, dan buka tutup 5 kali */
		if ( stat_input[9] == 0 && jum_tutup > 20)
		{
			jum_tutup = 0;
			armed = 0;
			wait_30 = 0;
			brace_stat = 0;
			
			relay_A_off();
			relay_B_off();
			relay_C_off();
		}
		
		/* jika gak ke buru ke pintu, tinggal matikan / nyalakan
		 * armed lagi untuk reset waktu tunggu 30 
		 */
		if (stat_input[9] == 0)
		{
			wait_30 = 0;
		}
		
		#endif
	}
}
void init_led_utama(void)
{
	xTaskCreate(task_led2, ( signed portCHAR * ) "Led2",  (configMINIMAL_STACK_SIZE * 4) , NULL, \
		tskIDLE_PRIORITY + 1, ( xTaskHandle * ) &hdl_led );
}

