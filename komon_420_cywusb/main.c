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
	
	3 Nov 2009
	Untuk Komon_420 saja !
*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define BAUD_RATE	( ( unsigned portLONG ) 115200 )

//#define CEK_BLINK

#ifdef BOARD_KOMON_A_RTD
#define LED_UTAMA	BIT(26)
#endif

#ifdef BOARD_KOMON_420_SAJA
#define LED_UTAMA	BIT(27)
#endif

xSemaphoreHandle lcd_sem;
unsigned int loop_idle=0;
unsigned int idle_lama;
unsigned int tot_idle;

static int tog;
static void sysInit(void);
void init_led_utama(void);
void init_task_cywusb(void);
void init_spi_mmc(char min)	;

//xTaskHandle hdl_lcd;
xTaskHandle hdl_led;
//xTaskHandle hdl_tampilan;
xTaskHandle hdl_shell;
xTaskHandle hdl_ether;

xTaskHandle hdl_cyswusb;

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

	FIO0DIR = LED_UTAMA;
	FIO0CLR = LED_UTAMA;	
	
	FIO1DIR = 0xFFFFFFFF;
	
	#ifdef PAKAI_SERIAL_2
	/* PCONP enable UART2 */
	PCONP |= BIT(24);
	
	/* PCLK UART2, PCLK = CCLK */
	PCLKSEL1 &= ~(BIT(16) | BIT(17));
	PCLKSEL1 |= BIT(16);
	
	/* init TX2, RX2 */
	PINSEL0 |= (BIT(20) | BIT(22));
	#endif
	
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

	#ifdef PAKAI_SERIAL_2
		serial2_init( 38400, (1 * configMINIMAL_STACK_SIZE) );
		//serial2_init( 19200, configMINIMAL_STACK_SIZE );
	#endif

#ifdef BOARD_KOMON_KONTER
	init_gpio();
#endif

	//init_gpio_adc();
	
#ifdef BOARD_KOMON_420_SAJA
	/* init spi #1 untuk adc & MMC */
	//init_spi_mmc(0);
#endif

#ifdef jalankan
	init_led_utama();
	start_ether();
	init_task_cywusb();			// harus sebelum init_shell
	init_shell();

	vTaskStartScheduler();

    /* Will only get here if there was insufficient memory to create the idle
    task. */
	return 0;
#endif
}

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
		
		#ifdef BOARD_KOMON_KONTER
		/* tiap detik cek apakah rpm mati */
		cek_gpio_lama();
		#endif
	}
	else
	{
		FIO0CLR = LED_UTAMA;
		tog = 1;		
	}
}


static portTASK_FUNCTION(task_led2, pvParameters )
{
	tog = 0;
	loop_idle = 0;
	idle_lama = 0;
	
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

static char passin[16];
static int  passc;
extern int rpmnya;

int wusbnya_in(char *c) {
	if ( *c == 0x0D || *c == 0x0A )			// enter !!
	{
		//printf("dicocokkan %s !\r\n", passin);
		passc = 0;
				
		if (strncmp(passin, "w_rpm", 5) == 0)	{
			paket_rpm(passin);
			//printf("datanya __%s__\r\n", passin);
			passc = 0;
			memset(passin, 0, sizeof (passin));
			return 1;
		}
		return 0;
	} else {
		//printf("%c",*c);		// DEBUG_RPM
		passin[ passc ] = (unsigned char) *c;
		passc++;
	}
	
	if (passc > sizeof (passin)) {
		passc = 0;
	}
	return 0;
}

//#ifdef PAKAI_CYWUSB
 //unsigned char x [50];
static 	char buf_rx[64];
extern struct t_data_float 	data_tetangga;

static portTASK_FUNCTION(task_cywusb, pvParameters )  {
	unsigned int muterx=0;
	unsigned int x=0;
	unsigned int qq=0;
	int i=0;

	unsigned int status=0, status_lama=0, status_baru=0;
	
	int c;
	int flag_aksi=0;
	vTaskDelay(2000);
	//ser2_putstring("serial 2 task cysusb\r\n");

	for (;;)
	{	
		if (ser2_getchar(1, &c, 1000) == pdTRUE)	{
			status += wusbnya_in( &c );
			status_baru = status;
		}

		muterx++;
		if (muterx > 500)	{	// tiap 5 detik
			//printf("RPM kincir: %d, lama: %d baru: %d status: %d\r\n", rpmnya, status_lama, status_baru, status);
			muterx=0;
			
			if(status_lama == status_baru) {
				// reset wireless jika tidak terima data baru
				reboot_wusb();
			}
			status_lama = status_baru;
		}
		
		if ((muterx%20)==0) {
			kontrol_target();
		}

		vTaskDelay(10);
	}
}

void init_task_cywusb(void)  {
	xTaskCreate(task_cywusb, ( signed portCHAR * ) "cyswusb",  (configMINIMAL_STACK_SIZE * 10) , NULL, \
		tskIDLE_PRIORITY + 3, ( xTaskHandle * ) &hdl_cyswusb );
}
//#endif


