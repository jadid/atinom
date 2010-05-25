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

//#include "../gsm/system_ftp/kirim_file_ftp.c"


#define BAUD_RATE	( ( unsigned portLONG ) 115200 )

#ifdef TAMPILAN_LPC
#define LED_UTAMA	(1 << 26)			// GPIO026
#endif

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


//xTaskHandle hdl_relay;
xTaskHandle hdl_muncrat;


#define TXDE	BIT(24)

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
	
	/* PCLK UART3 */
	PCLKSEL1 &= ~(BIT(18) | BIT(19));
	
	/* init TX3, RX3 */
	//PINSEL1 &= ~(BIT(18) | BIT(19) | BIT(20) | BIT(21));
	//PINSEL1 |= (BIT(18) | BIT(19));
	PINSEL1 |= (BIT(20) | BIT(21));
	
	/* TXDE di highkan */
	FIO0DIR |= TXDE;
	FIO0SET = TXDE;
	#endif
	
	#ifdef PAKAI_SERIAL_2
	/* PCONP enable UART2 */
	PCONP |= BIT(24);
	
	/* PCLK UART2, PCLK = CCLK */
	PCLKSEL1 &= ~(BIT(16) | BIT(17));
	PCLKSEL1 |= BIT(16);
	
	/* init TX2, RX2 */
	PINSEL0 |= (BIT(20) | BIT(22));
	#endif

	#ifdef PAKAI_SERIAL_1
	/* PCONP enable UART1 */
	PCONP |= BIT(4);
	
	/* PCLK UART1, PCLK = CCLK */
	PCLKSEL0 &= ~(BIT(8) | BIT(9));		// reset dulu
	PCLKSEL0 |= BIT(8);					// samain aja PCLK = CCLK
	
	/* init TX1, RX1 */
	PINSEL4 &= ~(BIT(0) | BIT(1) | BIT(2) | BIT(3));	// reset dulu
	PINSEL4 |= (BIT(1) | BIT(3));			// TXD1 & RXD1
	 
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

	xSerialPortInitMinimal( BAUD_RATE, (1 * configMINIMAL_STACK_SIZE) );
	
	#ifdef PAKAI_SERIAL_2
		serial2_init( BAUD_RATE, (1 * configMINIMAL_STACK_SIZE) );
		//serial2_init( 19200, configMINIMAL_STACK_SIZE );
	#endif
//*
	#ifdef PAKAI_SERIAL_3
		serial3_init( BAUD_RATE, (1 * configMINIMAL_STACK_SIZE) );		// serial3_init
	#endif

	#ifdef PAKAI_SERIAL_1
		serial1_init( BAUD_RATE, configMINIMAL_STACK_SIZE );
		//serial1_init( BAUD_RATE, (1 * configMINIMAL_STACK_SIZE) );	// serial 1 init
	#endif
//*/
#if 1
	init_led_utama();		// 5, -2
	init_task_lcd();		// 1, +1
//	start_ether();
	init_muncrat();
	init_shell();			// 10, 0
	init_task_tampilan();	// 10, -1

#ifdef PAKAI_SELENOID
	//init_task_relay();
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
		
		#if (PAKAI_SELENOID == 1)
		//set_selenoid( 1 );
		#endif
	
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
	
/*	
	#if (PAKAI_SELENOID == 1)
	init_selenoid();
	#endif
//*/	
	for (;;)
	{
		togle_led_utama();
		vTaskDelay(500);
		
		#if (PAKAI_GSM_FTP == 1)
		if (saat_gsm_ftp == 1)
		{
			//gsm_ftp(0, "asas");
			//printf("gsm hidup\r\n");
			saat_gsm_ftp = 0;
		}
		#endif
		
		
		
		#ifdef PAKAI_CRON
			baca_cron();
		#endif
	}
}
void init_led_utama(void)
{	
	/* tadinya stak = 5, prio - 1 */
	xTaskCreate(task_led2, ( signed portCHAR * ) "Led2",  (configMINIMAL_STACK_SIZE * 10) , \
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


char titiknya[10][50];

char * trim(char *katanya) {
	char *pch;
	pch = katanya;
	int p=strlen(katanya);
	int j=0;
	
	while(p>j) {
		if (*pch!=' ') {
			break;
		}
		pch++;
		j++;
	}

	return pch;
	//printf("________isi: %s\r\n", pch);
}

int pisah_titik(char * str) {
	int awal=0, akhir=0;
	//char str[] = "  RPM 523.54,   Tot 42   ,BB 53.1234563 ,T Exh 346.43, P Exh 6.43";
	char kata[50];
	char * pch, *chrx;
	chrx = str;
	int titik=0;

	pch=strchr(str,',');
	//printf("%s, p: %d, di %d\r\n",str, strlen(chrx), pch-str+1);

	if (pch!=NULL) {
		while (pch!=NULL)  {
			//printf("%s, panjang chrx %d, pch %d\r\n", pch, strlen(chrx), strlen(pch));
			akhir = strlen(chrx)-strlen(pch);
			
			memcpy( (char *) &kata, chrx+awal, akhir-awal );
			//printf("________isi: %s, awal: %d akhir: %d, chrx %s, pch\r\n", kata, awal, akhir, chrx);
			kata[akhir-awal]='\0';
			//printf("%s\r\n", trim(kata));
			sprintf(titiknya[titik], "%s",trim(kata));
			//printf("%s\r\n", titiknya[titik]);
			awal = akhir+1;
			pch=strchr(pch+1,',');
			titik++;
		}
		// sisanya
		//printf("%s\r\n", trim(chrx+awal));
		//strcpy(titiknya[titik], trim(chrx+awal));
		sprintf(titiknya[titik], "%s",trim(chrx+awal));
	} else {
		//printf("%s\r\n",str);
		//strcpy(titiknya[titik], trim(kata));
		sprintf(titiknya[titik], "%s",trim(kata));
		//printf("isinya: %s\r\n", titiknya[titik]);
	}
	if (strlen(str)>5)
		titik++;
	return titik;
}

portTASK_FUNCTION( muncrat_task, pvParameters )  {
	char buf[200];
	int titik=0;
	int j=0;
	
	vTaskDelay(2000);
	for(;;) {
		
		serX_putstring(2,"s\r");
		baca_serial(buf, 200, 2);
		
		//printf("isi: %s\r\n", buf);
		
		for(j=0; j<titik; j++) {
			sprintf(titiknya[j], "");
		}
		titik = pisah_titik(buf);
		
		//*
		printf("Titik ukur: %d\r\n", titik);
		for(j=0; j<titik; j++) {
			printf("isinya: %s\r\n", titiknya[j]);
		}
		//*/
		vTaskDelay(1000);
	}
}



void init_muncrat(void) {
	xTaskCreate( muncrat_task, ( signed portCHAR * ) "muncrat", (configMINIMAL_STACK_SIZE * 10), \
		NULL, tskIDLE_PRIORITY + 3, (xTaskHandle *) &hdl_muncrat );	
}
