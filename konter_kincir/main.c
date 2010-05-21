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
#include "../monita/monita_uip.h"

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
xTaskHandle hdl_kincir;
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

#define P8_1		BIT(19)
#define P8_2		BIT(20)
#define P8_3		BIT(21)
#define P8_4		BIT(22)
#define P8_5		BIT(23)

#define TEST_LED	BIT(24)
#define RELAY_2		BIT(25)
#define RELAY_1		BIT(26)



/*
#define PCADC		BIT(12)
#define ADC_CLKDIV	(250 << 8)
#define ADC_PDN		BIT(21)
#define ADC_START	BIT(24)
#define ADC_1		BIT(0)
#define ADC_2		BIT(1)
#define ADC_DONE	BIT(31)
//*/

#define OPTO1	BIT(0)		// P1
#define OPTO2	BIT(1)		// P1
#define OPTO3	BIT(4)		// P1
#define OPTO4	BIT(8)		// P1
#define OPTO5	BIT(9)		// P1


void init_gpio_relay(void)
{
	FIO1DIR = FIO1DIR | TEST_LED | RELAY_1 | RELAY_2 | P8_4;
	FIO1CLR = P8_4;
}

void LED_Relay_on() {
	FIO1SET = TEST_LED;
}

void LED_Relay_off() {
	FIO1CLR = TEST_LED;
}

/*
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

void init_kincir(void) {
	FIO1DIR &= ~(OPTO1 | OPTO2 | OPTO3 | OPTO4 | OPTO5);		// set sebagai input
}
//*
void motor_1() {
	motor_mati();
	FIO1SET = RELAY_1;
}

void motor_2() {
	motor_mati();
	FIO1SET = RELAY_2;
}

void motor_mati() {
	FIO1CLR = RELAY_1;
	FIO1CLR = RELAY_2;
}
//*/
int nOPTO1() {
	return (FIO1PIN & OPTO1);		// aktif high, aktif low jika tanda cacing dihilangkan
}

int nOPTO2() {
	return (FIO1PIN & OPTO2);
}

int nOPTO3() {
	return (FIO1PIN & OPTO3);
}

int nOPTO4() {
	return (FIO1PIN & OPTO4);
}

int nOPTO5() {
	return (FIO1PIN & OPTO5);
}

//#define CEK_BLINK

extern unsigned int data_putaran[];
extern unsigned int data_hit[];	

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

	//xSerialPortInitMinimal( BAUD_RATE, configMINIMAL_STACK_SIZE  );
	xSerialPortInitMinimal( 115200, configMINIMAL_STACK_SIZE  );

#ifdef BOARD_KOMON_KONTER
	init_gpio();
#endif
	
	//setup_adc();
	init_kincir();
	init_gpio_relay();
#ifdef jalankan
	init_task_kincir();
	init_led_utama();
	start_ether();
	init_shell();
	//init_task_motor();
	//init_task_adc();
	


	vTaskStartScheduler();

    /* 
     Will only get here if there was insufficient memory to create the idle task. 
    */
	return 0;
#endif
}

float rpmnya[20], KTAnya=0;
int posisi_PU=0;		// posisi tidak dikenali atau start up
#define RPM_MAX 300
#define RPM_BAND 10
#define KTA_MAX 12
#define KTA_MIN 8

#define motor_gulung() motor_2()
#define motor_ulur() motor_1()

extern struct t_data_float 	data_tetangga;
//float RPM_MAX = 300;
//float RPM_BAND = 10;

void togle_led_utama(void) {
	if (tog)
	{
		FIO0SET = LED_UTAMA;
		LED_Relay_on();
		//motor_1();
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
		LED_Relay_off();
		//motor_2();
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
	unsigned int muter=0, muterlagi=0;
	unsigned int x=0;
	tog = 0;
	loop_idle = 0;
	idle_lama = 0;
	//unsigned char tes=41;
	

	vTaskDelay(500);
	for (;;)
	{	
		hitung_rpm();
		//printf("_____________OPTO 1: %s, OPTO 2: %s, OPTO 3: %s, OPTO 4: %s,\r\n", nOPTO1()?"mati":"hidup", nOPTO2()?"mati":"hidup", nOPTO3()?"mati":"hidup", nOPTO4()?"mati":"hidup");
		muter++;		
		if (muter > 5)
		{
			togle_led_utama();
			//printf("__rpmnya: %4.2f, posisi_PU: %d, KTAnya: %f\r\n", rpmnya[9], posisi_PU, KTAnya);
			muter = 0;
		}	
		muterlagi++;
		if (muterlagi>100) {
			printf("__rpmnya: %4.2f, posisi_PU: %d, KTAnya: %f\r\n", rpmnya[9], posisi_PU, KTAnya);
			muterlagi=0;
		}
		vTaskDelay(100);
	}
}

void init_led_utama(void) {
	xTaskCreate(task_led2, ( signed portCHAR * ) "Led2",  (configMINIMAL_STACK_SIZE * 2) , NULL, \
		tskIDLE_PRIORITY - 2, ( xTaskHandle * ) &hdl_led );
}


#ifdef PAKAI_KINCIR
//unsigned char x [50];

void cari_rpmnya() {
	unsigned int i;
	float temp_f;
	float temp_rpm;

	for (i=0; i<10; i++)
	{	
		if (data_putaran[i])
		{
			// cari frekuensi
			temp_f = (float) 1000000000.00 / data_putaran[i]; // beda msh dlm nS
			// rpm
			temp_rpm = temp_f * 60 * 0.333;
			rpmnya[i] = temp_rpm;
			//data_float.data[t] = (unsigned int) (data_hit[i] * env2.kalib[i].m);
			
		}
		else
		{
			temp_f = 0;
			temp_rpm = 0;
		}	
		
		//printf(" %2d : F = %4.2f Hz, rpm = %4.2f\n", (i+1), temp_f, temp_rpm);

	}
	
}

void cek_posisi_awal() {
	printf("posisi PU: %d\r\n", posisi_PU);
	motor_gulung();			// ganti pada gerakan motor yg sesuai
	do {
		KTAnya = data_tetangga.data[16];
		vTaskDelay(10);
		cari_posisinya();
		/*
		if (nOPTO1()==0) {
			posisi_PU=1;
		} else if (nOPTO2()==0) {
			posisi_PU=2;
		} else if (nOPTO3()==0) {
			posisi_PU=3;
		} else if (nOPTO4()==0) {
			posisi_PU=4;
		}
		//*/
	} while(posisi_PU==0);
	motor_mati();
	printf("posisi PU now: %d\r\n", posisi_PU);
}

void cari_posisinya() {
	if (nOPTO1()==0) {
		posisi_PU=1;
	} else if (nOPTO2()==0) {
		posisi_PU=2;
	} else if (nOPTO3()==0) {
		posisi_PU=3;
	} else if (nOPTO4()==0) {
		posisi_PU=4;
	}
}

static portTASK_FUNCTION(task_kincir, pvParameters )  {
	unsigned int muterx=0;
	unsigned int x=0;
	unsigned int qq=0;
	
	unsigned char tes;
	unsigned char status;
	vTaskDelay(2000);
	
	cari_rpmnya();
	printf("posisi awal PU: %d\r\n", posisi_PU);
	if (posisi_PU==0) {
		cek_posisi_awal();		// posisi akan cenderung mingkup 
	}
	//printf("posisi PU: %d\r\n", posisi_PU);
	
	//printf("nilai: %f, nilai2: %f\r\n", data_tetangga.data[1], data_tetangga.data[2]);
	for (;;)	{	
		cari_rpmnya();
		KTAnya = data_tetangga.data[16];
		
		if (rpmnya[9] > RPM_MAX || KTAnya>KTA_MAX) {
			//printf("^^^^^^^^^^ posisi awal: %d, rpm: %4.2f, RPM_MAX: %d\r\n", posisi_PU, rpmnya[9], RPM_MAX);
			if (posisi_PU==1) {
				motor_gulung();
				do {
					cari_rpmnya();
					vTaskDelay(10);
					cari_posisinya();
					/*
					if(nOPTO2()==0) {
						posisi_PU=2;
					}
					//*/
				} while(posisi_PU==1);
			} else if (posisi_PU==2) {
				motor_gulung();
				do {
					cari_rpmnya();
					vTaskDelay(10);
					cari_posisinya();
					/*
					if(nOPTO3()==0) {
						posisi_PU=3;
					}
					//*/
				} while(posisi_PU==2);
			} else if (posisi_PU==3) {
				motor_gulung();
				do {
					cari_rpmnya();
					vTaskDelay(10);
					cari_posisinya();
					/*
					if(nOPTO4()==0) {
						posisi_PU=4;
					}
					//*/
				} while(posisi_PU==3);
			}
			//printf("^^^^^^^^^^ posisi akhir: %d\r\n", posisi_PU);
		}
		
		else if (rpmnya[9]<(RPM_MAX-RPM_BAND) || KTAnya<KTA_MIN)
		{
			//printf("_________ posisi awal balik: %d\r\n", posisi_PU);
			if (posisi_PU==2) {
				motor_ulur();
				do {
					cari_rpmnya();
					vTaskDelay(10);
					cari_posisinya();
					/*
					if(nOPTO1()==0) {
						posisi_PU=1;
					}
					//*/
				} while(posisi_PU==2);
			} else if (posisi_PU==3) {
				motor_ulur();
				do {
					cari_rpmnya();
					vTaskDelay(10);
					cari_posisinya();
					/*
					if(nOPTO2()==0) {
						posisi_PU=2;
					}
					//*/
				} while(posisi_PU==3);
			} else if (posisi_PU==4) {
				motor_ulur();
				do {
					cari_rpmnya();
					vTaskDelay(10);
					cari_posisinya();
					/*
					if(nOPTO3()==0) {
						posisi_PU=3;
					}
					//*/
				} while(posisi_PU==4);
			}
			//printf("_________ posisi akhir balik: %d\r\n", posisi_PU);
		} else {
			printf("BINGUNG_______________: %d\r\n", posisi_PU);
		}
		
		motor_mati();
		vTaskDelay(100);
	}
}

void init_task_kincir(void)  {
	xTaskCreate(task_kincir, ( signed portCHAR * ) "kincir",  (configMINIMAL_STACK_SIZE * 10) , NULL, \
		tskIDLE_PRIORITY - 3, ( xTaskHandle * ) &hdl_kincir );
}
#endif



