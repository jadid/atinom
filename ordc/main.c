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
	
	12 Juli 2009
	Untuk ordc
	
	jika mode normal, led blink setiap detik
	jika mode hemat, nyala bentar dan mati
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

/* led di keypad */
#define LED_PICKUP	BIT(14)

#define BACKLIT		BIT(20)	// PF15, P1.20
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

#define TIPE_PM710
#include "../modbus/low_mod.h"
struct d_pmod pmod;
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
	
	#if 0
	/* PCONP enable UART3 */
	PCONP |= BIT(25);
	
	/* PCLK UART3 */
	PCLKSEL1 &= ~(BIT(18) | BIT(19));
	
	/* init TX3, RX3 */
	//PINSEL1 &= ~(BIT(18) | BIT(19) | BIT(20) | BIT(21));
	PINSEL1 |= (BIT(18) | BIT(19));
	PINSEL1 |= (BIT(20) | BIT(21));
	
	/* TXDE di highkan */
	FIO0DIR |= TXDE;
	FIO0SET = TXDE;
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
	setup_adc();
	init_gpio_keypad();
	xSerialPortInitMinimal( BAUD_RATE, configMINIMAL_STACK_SIZE );
#if 1
	init_led_utama();
	init_task_lcd();
	init_shell();
	init_task_tampilan();
	
	vTaskStartScheduler();

    /* Will only get here if there was insufficient memory to create the idle
    task. */
	return 0;
#endif
}

float volt_bat;

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
		start_adc_1();
	}
	else
	{
		FIO0CLR = LED_UTAMA;
		FIO1CLR = LED_PICKUP;
		tog = 1;
		
		volt_bat = (float) (read_adc_1() * 3.3 / 0xFFFF);
		volt_bat = (4.545 * volt_bat) + volt_bat; /* resistor 1 M dan 220 */	
	}
}

//unsigned int jum_balik;
static int mode_hemat_status;

static portTASK_FUNCTION(task_led2, pvParameters )
{
	tog = 0;
	loop_idle = 0;
	idle_lama = 0;
	
	vTaskDelay(100);
	mode_hemat_status = 0;
	FIO1SET = BACKLIT;

	for (;;)
	{
		if (mode_hemat_status == 0)
		{
			togle_led_utama();
			vTaskDelay(500);
		}
		else
		{
			vTaskDelay(50);
			FIO1SET = LED_PICKUP;
			vTaskDelay(5);
			FIO1CLR = LED_PICKUP;
			tendang_wdog();
		}
	}
}
void init_led_utama(void)
{
	xTaskCreate(task_led2, ( signed portCHAR * ) "Led2",  (configMINIMAL_STACK_SIZE * 4) , \
		NULL, tskIDLE_PRIORITY - 2, ( xTaskHandle * ) &hdl_led );
}



/* LCD LCD LCD */
portTASK_FUNCTION( LCD_task, pvParameters )
{
	vSemaphoreCreateBinary( lcd_sem );
    xSemaphoreTake( lcd_sem, 0 );
	
	for(;;) 
	{
		if ( xSemaphoreTake( lcd_sem, 100 ) == pdTRUE )
		{
			update_hard_lcd();
		}	
	}
}

void init_task_lcd(void)
{
	xTaskCreate( LCD_task, ( signed portCHAR * ) "LCD", (configMINIMAL_STACK_SIZE * 1), \
		NULL, tskIDLE_PRIORITY + 1, (xTaskHandle *) &hdl_lcd );	
}

void togle_backlit(void)
{
	if (FIO1PIN & BACKLIT) FIO1CLR = BACKLIT;
	else
		FIO1SET = BACKLIT;

}

void off_backlit(void)
{
	FIO1CLR = BACKLIT;
}

void on_backlit(void)
{
	FIO1SET = BACKLIT;
}

void mode_hemat(void)
{
	
	/* 	tidak usah ke idle mode, karena tidak
		ada pheriperal yang jalan, termasuk led
		
		jadi hanya speednya saja yang dikurangi 
		
		clk 8 + Baklit : 176 mA
		clk 8 	: 105 mA
		clk 128 : 81 mA
		clk 250 : 80 mA
		
		clk 8 & pheriperal mati (kecuali timer0) : 87 mA
		clk 250 & pheriperal mati (kecuali timer0) : 70 mA
		
		*/
	portENTER_CRITICAL();
	
	/* matikan semua pheripheral kecual TIMER 0 */
	AD0CR &= ~BIT(21); // adc power down
	
	PCONP = BIT(1);
	ubah_clk(250);
	mode_hemat_status = 1;

	portEXIT_CRITICAL();
}

void mode_normal(void)
{
	portENTER_CRITICAL();
	
	/* power timer0 & uart0 */
	PCONP = BIT(1) | BIT(3);	
	setup_adc();
	ubah_clk(8);
	mode_hemat_status = 0;
	
	portEXIT_CRITICAL();
}
