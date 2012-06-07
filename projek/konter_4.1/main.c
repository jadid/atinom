/*
	FreeRTOS.org V5.1.1 - Copyright (C) 2003-2008 Richard Barry.
	
	entri dari boot.s adalah ke main
	
	20 feb 2009
	buat buffer file index.html dilakukan di task led.
	
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
#include "hardware.h"
#include "monita/monita_uip.h"
#include "sys.h"

unsigned int loop_idle=0;
unsigned int idle_lama;
unsigned int tot_idle;


//static void sysInit(void);
void init_led_utama(void);
//void init_spi_mmc(char min)	;

xTaskHandle hdl_led;
xTaskHandle hdl_shell;
xTaskHandle hdl_relay;

#ifdef PAKAI_ETH
	xTaskHandle hdl_ether;
#endif

#ifdef PAKAI_PUSHBUTTON
	#include "../../app/monita/monita_uip.h"
	extern unsigned int debound[KANALNYA];
#endif


/*-----------------------------------------------------------*/
#define jalankan

int main( void )	{
	setup_hardware();				// @"../../hardware/hardware.h"

	init_led_utama();
	init_hardware();

	vTaskStartScheduler();

	printf("Keluar dari Scheduler !!!!\r\n");
	vTaskDelay(4000);
    /* Will only get here if there was insufficient memory to create the idle task. */
	return 0;

}
void IdleTaskHook()	{
	
}

void togle_led_utama(char tog)	{
	//printf("debound 8 : %d, relay 8 : %s\r\n", debound[7], (data_f[(JML_SUMBER*PER_SUMBER)+7])?"Aktif":"Mati");
	//FIO0PIN ^= LED_UTAMA;

	if (tog)	{
		//FIO0SET = RXDE;
		/* kalkulasi idle loop */
		tot_idle = loop_idle - idle_lama;
		idle_lama = loop_idle;
		
		//printf("isi: %x: %d\r\n", &MEM_RTC0+1, *(&MEM_RTC0+1));

		tendang_wdog();		// reset wdog setiap detik //
	}
}

#ifdef PAKAI_PUSHBUTTON
	void kurangi_delay_push()	{
		char g;
		for (g=0; g<KANALNYA; g++) {
			if (debound[g]) {
				debound[g]--;
			}
		}
	}
#endif

extern xTaskHandle hdl_kirimcepat;
extern xTaskHandle hdl_ambilcepat;

static portTASK_FUNCTION(task_led2, pvParameters )	{
	static char tog = 0;
	loop_idle = 0;
	idle_lama = 0;
	for (;;)	{
		if (status_power()>0)	{
			//vTaskSuspend( hdl_kirimcepat );
			vTaskSuspend( hdl_ambilcepat );
			//vTaskSuspend( hdl_shell );

			setup_hardware();

			vTaskDelay(200);
			
			//vTaskResume( hdl_kirimcepat );
			vTaskResume( hdl_ambilcepat );
			//vTaskResume( hdl_shell );
			flagRTCc = 0;
			set_power_rtc(0);
			printf("  kembali normal.... !!\r\n");
		}
		
		
		togle_led_utama(tog=1-tog);
		cek_tik();
		
		#ifdef PAKAI_PUSHBUTTON
			kurangi_delay_push();
		#endif
		
		vTaskDelay(500);
		//printf(" - Tanggal   : %d-%d-%d\r\n", infoGPS.utc.day, infoGPS.utc.mon, infoGPS.utc.year);
	}
}
void init_led_utama(void) {
	xTaskCreate(task_led2, ( signed portCHAR * ) "Led2",  (configMINIMAL_STACK_SIZE * 2) ,\
		 NULL, tskIDLE_PRIORITY - 2, ( xTaskHandle * ) &hdl_led );
}
