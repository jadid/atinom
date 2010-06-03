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


xTaskHandle hdl_led;
xTaskHandle hdl_shell;
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
	
	xSerialPortInitMinimal( BAUD_RATE, (1 * configMINIMAL_STACK_SIZE) );

	
/*	#ifdef PAKAI_SERIAL_2_P0
	// PCONP enable UART2 //
	PCONP |= BIT(24);
	
	// PCLK UART2, PCLK = CCLK //
	PCLKSEL1 &= ~(BIT(16) | BIT(17));
	PCLKSEL1 |= BIT(16);
	
	// init TX2, RX2 //
	PINSEL0 |= (BIT(20) | BIT(22));
	#endif
//*/

	#ifdef PAKAI_SERIAL_2_P2
	// PCONP enable UART2 //
	PCONP |= BIT(24);
	
	// PCLK UART2, PCLK = CCLK //
	PCLKSEL1 &= ~(BIT(16) | BIT(17));
	PCLKSEL1 |= BIT(16);
	
	// init TX2, RX2 //
	PINSEL4 &= ~(BIT(16) | BIT(17));		// TXD2
	PINSEL4 |= BIT(17);
	PINSEL4 &= ~(BIT(18) | BIT(19));		// RXD2
	PINSEL4 |= BIT(19);
	
	#endif

	#ifdef PAKAI_SERIAL_2
		serial2_init( BAUD_RATE, (1 * configMINIMAL_STACK_SIZE) );
	#endif

#ifdef jalankan
	init_led_utama();		// 5, -2
	
	init_shell();			// 10, 0
	init_muncrat();
	vTaskStartScheduler();

    /* Will only get here if there was insufficient memory to create the idle
    task. */
    printf("sampe kene jika kentekan memori");
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

static portTASK_FUNCTION(task_led2, pvParameters )	{
//	int i;
//	int c;
//	int x;
	
	tog = 0;
	loop_idle = 0;
	idle_lama = 0;
	
	vTaskDelay(500);

	for (;;)	{
		togle_led_utama();
	
		vTaskDelay(500);
		

	}
}

void init_led_utama(void)	{	
	/* tadinya stak = 5, prio - 1 */
	xTaskCreate(task_led2, ( signed portCHAR * ) "Led2",  (configMINIMAL_STACK_SIZE * 10) , \
		NULL, tskIDLE_PRIORITY, ( xTaskHandle * ) &hdl_led );
}


portTASK_FUNCTION( muncrat_task, pvParameters )  {
	char bufx[200];
	
	vTaskDelay(500);
	for(;;) {
/*
		printf("ke serial 0   muncrat.....\r\n");		
		serX_putstring(2,"muncrat.......\r\n");
//*/
//*
		baca_serial(bufx, 200, 2);
		if (bufx[0]>32 && bufx[0]<126)
			serX_putstring(2,bufx);
		bufx[0]='\0';
//*/
		vTaskDelay(1000);

	}
}



void init_muncrat(void) {
	xTaskCreate( muncrat_task, ( signed portCHAR * ) "muncrat", (configMINIMAL_STACK_SIZE * 10), \
		NULL, tskIDLE_PRIORITY + 1, (xTaskHandle *) &hdl_muncrat );	
}

#define read(a, b, c) ser2_getchar(a, b, 1000);	\
		tendang_wdog();

int baca_serial(char *buf, int len, int timeout) {
	int res, fd=0;
	int c;
	int tout=0;
	int masuk = 0;
	
	while(len--)
	{
		res = read( fd, &c, 1);
		if (res != 0)
		{
			if ( (char) c == 0x0A || (char) c == 0x0D )
			{
				if (masuk > 0) 
				{	
					/* supaya buffer lama tidak ikut di printout */
					buf[masuk] = 0;
					return 0;
				}
			}
			else
			{
				buf[ masuk ] = (char ) c;								
				masuk++;
				
				#if (DEBUG == 1)
				printf(" %s(): res=%d : msk=%02d : 0x%02X : %c\r\n", __FUNCTION__, res, masuk, (char) c, (char) c);
				#endif
				/*
				if ( (char) c == 0x0A && masuk > 2)
				{
					if (buf[ masuk - 2 ] == 0x0D)
					return 0;
				}*/
			}
		}
		else
		{
			//printf(" %s(): %d :timeout\r\n", __FUNCTION__, tout);
			len++;
			tout++;
			if ( tout > timeout) 
				return 0;
				//return -1;
		}
	}
}
