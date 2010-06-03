/*
	FreeRTOS.org V5.1.1 - Copyright (C) 2003-2008 Richard Barry.
	
	entri dari boot.s adalah ke main
	

*/


/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define LED_UTAMA	BIT(26)
#define BAUD_RATE	( ( unsigned portLONG ) 115200 )




unsigned int loop_idle=0;
unsigned int idle_lama;
unsigned int tot_idle;

static char tog;
static void sysInit(void);
void init_led_utama(void);

//xTaskHandle hdl_lcd;
xTaskHandle hdl_led;
//xTaskHandle hdl_tampilan;
xTaskHandle hdl_shell;
//xTaskHandle hdl_ether;


//xTaskHandle hdl_relay;
xTaskHandle hdl_muncrat;


#define TXDE	BIT(24)
//#define CEK_BLINK

void dele(int dd)	{
	int g;
	int i;
	int dum;

	for (i=0; i<dd; i++)	{
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
	
	//FIO1DIR  = FIO1DIR | LED_PICKUP;
	//FIO1MASK = FIO1MASK & ~(LED_PICKUP);
	
	dele(1000000);
	FIO0SET = LED_UTAMA;
	
	/*	untuk cek blinking saat system boot */
	#ifdef CEK_BLINK
		int t=0;
		//while(t<5)
		{
			dele(1000000);
			FIO0CLR = LED_UTAMA;
			dele(1000000);
			FIO0SET = LED_UTAMA;
			t++;
		}
	#endif
	
	while(1);
	
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



	//xSerialPortInitMinimal( BAUD_RATE, (1 * configMINIMAL_STACK_SIZE) );
	
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
#if 0
	init_led_utama();		// 5, -2

//	start_ether();
	init_muncrat();
	init_shell();			// 10, 0

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
		

	}
	else
	{
		FIO0CLR = LED_UTAMA;
		tog = 1;
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
	
	vTaskDelay(500);
	//FIO1SET = BACKLIT;
	
/*	
	#if (PAKAI_SELENOID == 1)
	init_selenoid();
	#endif
//*/	
	for (;;)
	{
		togle_led_utama();
		vTaskDelay(500);
		

	}
}
void init_led_utama(void)
{	
	/* tadinya stak = 5, prio - 1 */
	xTaskCreate(task_led2, ( signed portCHAR * ) "Led2",  (configMINIMAL_STACK_SIZE * 10) , \
		NULL, tskIDLE_PRIORITY, ( xTaskHandle * ) &hdl_led );
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
		
		//serX_putstring(2,"s\r");
		//baca_serial(buf, 200, 2);
		

		//*/
		vTaskDelay(1000);
	}
}



void init_muncrat(void) {
	xTaskCreate( muncrat_task, ( signed portCHAR * ) "muncrat", (configMINIMAL_STACK_SIZE * 10), \
		NULL, tskIDLE_PRIORITY + 3, (xTaskHandle *) &hdl_muncrat );	
}
