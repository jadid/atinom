/*
	tampilan.c
	
	firmware tampilan untuk FreeRTOS
	furkan jadid
	14 desember 2008
	daun biru engineering

	27 februari 2009
	di edit dari service desktop
	
	12 Juli 2009
	dicopy ke ordc.c
	
	*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "../monita/monita_uip.h"
#include "tampilan.h"

/* 
 * struct berikut harus ditaruh di ram utama supaya bisa baca
 * tulis dari flash, entah kenapa kalau ditaruh di flash tidak bisa
 * baca & tulis
 */

struct t_sumber sumber[JML_SUMBER];
struct t_mesin	mesin[JML_MESIN];
struct t_titik	titik[TIAP_MESIN * JML_MESIN];
struct t_data_hitung data_hitung[JML_MESIN];

struct t_data_float s_data[JML_SUMBER]; /* __attribute__ ((section (".eth_test"))); */

extern xTaskHandle *hdl_tampilan;

void set_awal_mesin(void);
void set_awal_sumber(void);
void set_awal_titik(void);
int cek_keypad(void);

unsigned char tek[64];	
unsigned char daytime[32];

xSemaphoreHandle keypad_sem;

portTASK_FUNCTION( tampilan_task, pvParameters )
{
	unsigned int key_press;
	unsigned char key_index=0;
	unsigned char mesin_index=0;
	int i;
	int loop;
	unsigned char jum_OK;
	char loop_key;
	int loop_per_menit=0;
	int backlit_status = 1;
	int mode_hemat_status = 0;
	
	// set PF14 & PF10 sebagai input interrupt keypad
	#ifdef TAMPILAN_LPC
	FIO1DIR = FIO1DIR  & ~(PF10 | PF14 | KEY_DAT);
	#endif
	
	#ifdef TAMPILAN_LPC_4
	FIO2DIR = FIO2DIR  & ~PF14;
	
	/* masking dengan 0 supaya bisa dibaca */
	FIO1DIR  = FIO1DIR  & ~(KEY_DAT);
	FIO1MASK = FIO1MASK & ~(KEY_DAT);
	
	/* masking dengan 0 supaya bisa dibaca */
	FIO2MASK = FIO2MASK & ~(PF14);	
	#endif
	
	
	// kasih delay dulu, supaya skeduler jalan
	vTaskDelay(100);
	printf("Starting Tampilan\n");
	teks_h(14, 20, "Daun Biru Engineering");
	teks_h(14, 29, "FreeRTOS 5.1.1 by Richard Barry");
	teks_h(14, 38, "uIP TCP/IP by Adam Dunkels");
	teks_h(14, 47, "NXP LPC2367, 60 MHz");
	
	sprintf(tek, "ARM-GCC %s : %s : %s", __VERSION__, __DATE__, __TIME__);
	teks_h(14, 56, tek);
	
	teks_arial(22, 70, "Monita");
	teks_komik(18, 87, "Online Monitoring System");
	update_lcd();
	vTaskDelay(100);
	teks_h(14, 110, "Loading setting ...");
	
	cls_layar();
	vTaskDelay(500);
	update_lcd();
	vTaskDelay(100);
	update_lcd_layer2();
	vTaskDelay(10);
	
	key_index = 0;
	mesin_index = 0;
	cls_layar();
	menu_ordc(key_index);
	menu_pilih_utama(key_index, 0);
	update_lcd();	
	loop = 0;
	jum_OK = 0;
	i = loop_key = 0;
	
	vSemaphoreCreateBinary( keypad_sem );
    xSemaphoreTake( keypad_sem, 0 );		
	
	for (;;)
	{
		//if (cek_keypad())
		/* jika 1 detik tidak ada keypad, maka force untuk update lcd */
		if ( xSemaphoreTake( keypad_sem, 500 ) == pdTRUE )
		{	
			loop_key++;
			
			//if (loop_key > 1)
			{
				loop_key = 0;
						
				// cek tombol apa yang ditekan
				key_press = (FIO1PIN & KEY_DAT);
				//printf("tombol ditekan = %d\n", key_press);	
				cls_layar();
				
				if (backlit_status == 1)
				{
					on_backlit();
				}
				
				if (mode_hemat_status == 1)
				{
					mode_normal();
					mode_hemat_status = 0;
				}
				
				if (key_press == ATAS)
				{
					key_index--;
					if (key_index == 255) key_index = 4;		   
				}
				else if ( key_press == BAWAH )
				{
					key_index++;	
					if (key_index > 4) key_index = 0;
				}
				else if ( key_press == KANAN )
				{
					mesin_index++;
					if (mesin_index > 4) mesin_index = 0;
				}
				else if ( key_press == OK & key_index == 0)
				{
					// backlit
					if (backlit_status == 0)
					{
						on_backlit();
						backlit_status = 1;
					}
					else
					{
						off_backlit();
						backlit_status = 0;
					}
				}
				else if ( key_press == OK & key_index == 1)
				{
				//get_data_recip();	
				
				/* Pilih Rute */
				//menu_file_manager(0);
				}
				else if ( key_press == OK & key_index == 2)
				{
				/* Lihat data */
				
				//fungsi_rotating();	
				}
				else if ( key_press == OK & key_index == 3)
				{
				#if (SIMULATOR == 0)
				//komuni_xmodem();
				#endif
				}
				else if ( key_press == OK & key_index == 4)
				{
					//fungsi_lain();	
				}
				//cls_layar();
				menu_ordc(key_index);
				menu_pilih_utama(key_index, 0);
				update_lcd();
				loop = 0;
			}	
		}	
		else
		{
			loop_key++;		
			if (loop_key > 10)
			{
				off_backlit();	
			}	
			
			if (loop_key > 20)
			{
				mode_hemat();
				mode_hemat_status = 1;
			}
		}
		#if 1
		if (loop > 3)
		{
			show_battery();					
			loop = 0;
		}
		#endif
		
		//vTaskDelay(100);
		loop++;	
	}
	
}

void init_task_tampilan(void)
{
	xTaskCreate( tampilan_task, ( signed portCHAR * ) "Tampilan", (configMINIMAL_STACK_SIZE * 7), \
		NULL, tskIDLE_PRIORITY - 1, (xTaskHandle *) &hdl_tampilan);	
}

int cek_keypad(void)
{
	//portENTER_CRITICAL();
	if ((FIO_KEYPAD & PF14) == PF14)
	{
		//portEXIT_CRITICAL();		
		return 1;	
	}
	else
	{
		//portEXIT_CRITICAL();	
		return 0;	
	}
}

