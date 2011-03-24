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
//#ifdef PAKAI_CYWUSB
xTaskHandle hdl_cyswusb;
//#endif

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
/*
#define P8_1		BIT(19)
#define P8_2		BIT(20)
#define P8_3		BIT(21)
#define P8_4		BIT(22)
#define P8_5		BIT(23)
#define P8_6		BIT(24)
#define P8_7		BIT(25)

#define PCADC		BIT(12)
#define ADC_CLKDIV	(250 << 8)
#define ADC_PDN		BIT(21)
#define ADC_START	BIT(24)
#define ADC_1		BIT(0)
#define ADC_2		BIT(1)
#define ADC_DONE	BIT(31)
//*/



/*
void init_gpio_relay(void)
{
	FIO1DIR = FIO1DIR | P8_1 | P8_2 | P8_3 | P8_4 | P8_5 | P8_6 | P8_7;
}


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

void setup_adc(void)
{
	// 19 Juni 2009, ADC power dinyalakan //
	PCONP |= PCADC;
	
	// PCLK ADC pakai default (00) atau CCLK / 4 //
	
	// setup PINSEL1 utk AD0 & AD1 //
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
//*/
unsigned short read_adc_1(void)
{
	unsigned int stat;
	/* memang dimatikan
	stat = AD0STAT;
	
	if (stat & 0x01)
	{
		stat = AD0DR0;	
		return (stat & 0xFFFF);
	}
	else
		return 0;
	*/
	/*
	stat = AD0DR0;
	if (stat & ADC_DONE)
	{
		return (stat & 0xFFFF);
	}
	else
		return 0;
	//*/
}
/*
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
//*/
/*-----------------------------------------------------------*/
#define jalankan

#define CEK_BLINK

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
	
	//setup_adc();
	
#ifdef jalankan
	init_led_utama();
	start_ether();
	init_shell();
	//init_task_adc();

#ifdef PAKAI_CYWUSB
	//init_task_cywusb();
	init_modul_cywusb();
#endif

	vTaskStartScheduler();

    /* 
     Will only get here if there was insufficient memory to create the idle task. 
    */
	return 0;
#endif
}

void togle_led_utama(void) {
	if (tog)
	{
		FIO0SET = LED_UTAMA;
		#ifdef PAKAI_CYWUSB
		LED_cywusb_on();
		#endif
		tog = 0;
		
		/* kalkulasi idle loop */
		tot_idle = loop_idle - idle_lama;
		idle_lama = loop_idle;
		
		/* reset wdog setiap detik */
		tendang_wdog();
		
		//blok_1_up();
		//blok_2_down();
		//blok_3_up();
		
		// wdog malimping //
		dog_menit++;
		if (dog_menit > (60 * 5)) /* 5 menit */
		//if (dog_menit > 10) /* 5 menit */
		{
			// 	jika 5 menit tidak ada perubahan jumlah request
			// 	berarti server ngehang */
			 	
			if (dog_lama == loop_kirim)
			{
				// 
				//	normally close, sehingga jadi open
				//	supply tegangan ke server putus, jadi hard reset //
				
				if (mati_total == 0)	
					//blok_2_up();						
				
				dog_hang = 1;
			}
			else
			{
				dog_lama = loop_kirim;
				dog_hang = 0;
			}
			dog_menit = 0;
		}
		
		// tunggu sekitar 10 detik baru release relaynya //
		if (dog_hang == 1 && dog_menit > 5)
		{
			// release relay, sehingga close lagi //
			if (mati_total == 0)
				//blok_2_down();
			
			dog_hang = 0;
		}
		
		
	}
	else
	{
		FIO0CLR = LED_UTAMA;
		#ifdef PAKAI_CYWUSB
		LED_cywusb_off();
		#endif
		tog = 1;
		
		//blok_1_down();
		//blok_2_up();
		//blok_3_down();
		
		//unsigned short d_adc = read_adc_1();	
		//printf("ADC 1 = 0x%4X = %.3f\r\n", d_adc, (float) (d_adc * 3.3 / 0xFFFF));
	}
}



static portTASK_FUNCTION(task_led2, pvParameters )
{
	unsigned int muter=0;
	unsigned int x=0;
	tog = 0;
	loop_idle = 0;
	idle_lama = 0;
	//unsigned char tes=41;
	
	//*
	vTaskDelay(500);
	#ifdef PAKAI_CYWUSB
		konfig_WUSB('b');
	#endif
	//*/
	
	vTaskDelay(500);
	for (;;)
	{	
		/*
		portENTER_CRITICAL();
		for(x=0; x<100000; x++) {
			CYWM_WriteReg(0xA5, 0xA5);
		}
		portEXIT_CRITICAL();
		//*/
		
		//*
		#ifdef PAKAI_CYWUSB
			portENTER_CRITICAL();
			unsigned char tes;
				tes = CYWM_ReadReg( 0x08 );		// REG_RX_INT_STAT
				//printf("read reg: 0x%x___",tes);
				if (tes & 0x01 || tes & 0x02) { // FULL A || EOF A
					CYWM_WriteReg( 0x07, 0x03 );	// REG_RX_INT_EN Enable EOF and FULL interrupts for channel A
					//printf("write reg: 0x%x___",tes);
					if (tes & 0x08) { // Valid A
						tes = CYWM_ReadReg( 0x09 );	//REG_RX_DATA_A
						//wusb_in(tes);
						printf("%c",tes);
					}
					//data = CYWM_ReadReg( REG_RX_VALID_A );
				}
			portEXIT_CRITICAL();
		#endif
		//*/	
		/* 
			setiap 100 ms, cek apakah rpm masih jalan,
			dicek satu per satu, supaya tidak balapan 
			(race condition)
			
			kemudian setiap 500 jalankan togle led
			seperti biasa
			
			*/
		
		//hitung_rpm();
		
		muter++;		
		if (muter > 50)
		{
			togle_led_utama();
			//printf("%4d___Reg ID         : 0x%x\r\n", x++, CYWM_ReadReg(0x00));	// REG_ID
			muter = 0;
		}	
		
		vTaskDelay(10);
	}
}
void init_led_utama(void)
{
	xTaskCreate(task_led2, ( signed portCHAR * ) "Led2",  (configMINIMAL_STACK_SIZE * 2) , NULL, \
		tskIDLE_PRIORITY - 2, ( xTaskHandle * ) &hdl_led );
}

/* task untuk adc, setiap 2 ms */

#if (KONTER_MALINGPING == 1)

float volt_supply;

static portTASK_FUNCTION( task_adc , pvParameters )
{
	int loop=0;
	unsigned long tot=0;
	int loop_mati = 1000;
	mati_total = 1;
	
	/* 	saat awal, server tidak usah mati dulu
		sehingga jika supply kurang maka akan 
		mati dengan sendirinya */
		
	vTaskDelay(3000);
	
	
	for (;;)
	{
//		start_adc_1();
		vTaskDelay(2);
//		tot = tot + read_adc_1();
		
		loop++;
		if (loop > 500)
		{			
			tot = (tot / 500);
			
			volt_supply = (float) (tot * 3.3 / 0xFFFF);	
			//printf("ADC 1 = 0x%4X = %.3f\r\n", tot, volt_supply );
			
			/* 	cek jika diatas setting, maka relay diaktifkan !
				relay normaly open.
				
				pertimbangkan juga saat waktunya on (histerisis ?)
				jangan sampai malah hidup/mati hidup/mati
				terlalu sering.
				
				beri jarak setiap 2 menit. 
				
				atau, mati pada < 22 Volt
				dan nyala jika > 22.5 Volt atau 23 Volt sekalian
			*/
			
			//if (volt_supply > 2.86) /* line > 22.5 Volt */
			//if (volt_supply > 2.83) /* line > 22.5 Volt */
			if (volt_supply > 2.65) /* line > 22.5 Volt */
			{
				if (loop_mati > (60 * 2))
				{
//					blok_1_up();
					
					/* server juga nyala */
//					blok_2_down();
					
					/* ke penduduk nyala */
//					blok_3_up();
					
					mati_total = 0;
				}
				else
					loop_mati++;
			}
			
			//if (volt_supply < 2.80) /* line < 22 Volt */
			//if (volt_supply < 2.77) /* line < 22 Volt */
			if (volt_supply < 2.52)   /* line <  20.286 */	
			{
//				blok_1_down();
				
				/* server juga harus ikutan mati */
//				blok_2_up();
				
				/* ke penduduk mati */
//				blok_3_down();
				
				mati_total = 1;				
				loop_mati=0;
			}
				
			
			tot = 0;
			loop = 0;
		}
	}
}

void init_task_adc(void)
{
	xTaskCreate(task_adc, ( signed portCHAR * ) "on_adc",  (configMINIMAL_STACK_SIZE * 2) , NULL, \
		tskIDLE_PRIORITY - 2, ( xTaskHandle * ) &hdl_led );
}

#endif

#ifdef PAKAI_CYWUSB
static portTASK_FUNCTION(task_cywusb, pvParameters )  {
	unsigned int muterx=0;
	unsigned int x=0;
	//init_modul_cywusb();
	
	vTaskDelay(500);
	//konfig_WUSB('b');
	
	//vTaskDelay(500);
	//lihatKonfig();
	for (;;)
	{	
		/*
		portENTER_CRITICAL();
		for(x=0; x<1000000; x++) {
			kirim_SSP0(0xA5);
			//CYWM_WriteReg(0xA5,0xA5);
		}
		portEXIT_CRITICAL();
		//*/
		
		/*
		#ifdef PAKAI_CYWUSBxxx
			portENTER_CRITICAL();
			unsigned char tes;
				tes = CYWM_ReadReg( 0x08 );		// REG_RX_INT_STAT
				//printf("read reg: 0x%x___",tes);
				if (tes & 0x01 || tes & 0x02) { // FULL A || EOF A
					CYWM_WriteReg( 0x07, 0x03 );	// REG_RX_INT_EN Enable EOF and FULL interrupts for channel A
					//printf("write reg: 0x%x___",tes);
					if (tes & 0x08) { // Valid A
						tes = CYWM_ReadReg( 0x09 );	//REG_RX_DATA_A
						//wusb_in(tes);
						printf("%c",tes);
					}
					//data = CYWM_ReadReg( REG_RX_VALID_A );
				}
			portEXIT_CRITICAL();
		#endif
		//*/
		
		muterx++;		
		if (muterx > 50)
		{
			//togle_led_utama();
			//printf("%4d___Reg ID         : 0x%x\r\n", x++, CYWM_ReadReg(0x00));	// REG_ID

			muterx = 0;
		}	
		
		vTaskDelay(100);
	}
}

void init_task_cywusb(void)  {
	xTaskCreate(task_cywusb, ( signed portCHAR * ) "cyswusb",  (configMINIMAL_STACK_SIZE * 4) , NULL, \
		tskIDLE_PRIORITY - 3, ( xTaskHandle * ) &hdl_cyswusb );
}
#endif
