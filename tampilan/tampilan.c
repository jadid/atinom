/*
	firmware tampilan untuk FreeRTOS
	furkan jadid
	14 desember 2008
	daun biru engineering

	27 februari 2009
	di edit dari service desktop
	
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
	
	//set_awal_mesin();
	//set_awal_sumber();
	//set_awal_titik();
	
	read_sumber();
	read_mesin();
	read_titik();
	
	cls_layar();
	vTaskDelay(500);
	update_lcd();
	vTaskDelay(100);
	update_lcd_layer2();
	vTaskDelay(10);
	
	teks_h(14, 20, "Data Mesin :");
	for (i=0; i<10; i++)
	{
		sprintf(tek, "%2d     %s", (i+1), mesin[i].nama);
		teks_h(20, 30 + (i*9), tek);
	}
	cls_layar();
	vTaskDelay(800);
	update_lcd();
	vTaskDelay(100);
	update_lcd_layer2();
	vTaskDelay(10);
	
	teks_h(14, 20, "Data Sumber Data :");
	for (i=0; i<JML_SUMBER; i++)
	{
		sprintf(tek, "%2d   %s", (i+1), sumber[i].nama);
		teks_h(20, 30 + (i*9), tek);
		
		// print out IP
		sprintf(tek,"%d.%d.%d.%d", sumber[i].IP0, sumber[i].IP1, sumber[i].IP2, sumber[i].IP3);
		teks_h(120, 30 + (i*9), tek);
		
		// status
		if (sumber[i].status == 0)
			teks_h(220, 30 + (i*9), "Tdk Aktif");
		if (sumber[i].status == 1)
			teks_h(220, 30 + (i*9), "Normal");
		if (sumber[i].status == 2)
			teks_h(220, 30 + (i*9), "TimeOut");
		if (sumber[i].status == 5)
			teks_h(220, 30 + (i*9), "Daytime");	
		// init data float
		for(loop = 0; loop < 20; loop++)
		{
			s_data[i].data[loop] = 0.00;	
		}	
	}
	
	cls_layar();
	// test depth
	//fill_layar(0x0F);
	vTaskDelay(800);
	update_lcd_layer2();
	
	//cls_layar();
	//kotak(10, 10, 300, 220);
	//update_lcd_layer3();
	
	key_index = 0;
	mesin_index = 0;
	cls_layar();
	menu_monita();
	menu_pilih(key_index, mesin_index, 0);
	update_lcd();	
	loop = 0;
	jum_OK = 0;
	i = loop_key = 0;
	/*
	vSemaphoreCreateBinary( keypad_sem );
    xSemaphoreTake( keypad_sem, 0 );		
	*/
	for (;;)
	{
		if (cek_keypad())
		/* jika 1 detik tidak ada keypad, maka force untuk update lcd */
		//if ( xSemaphoreTake( keypad_sem, 1000 ) == pdTRUE )
		{	
			loop_key++;
			
			//if (loop_key > 1)
			{
				loop_key = 0;
			
				cls_layar();
			
				// cek tombol apa yang ditekan
				key_press = (FIO1PIN & KEY_DAT);
				//printf("tombol ditekan = %d\n", key_press);	
			
				if (key_press == ATAS)
				{
					key_index--;
					if (key_index == 255) key_index = 10;	// 9	   
				}
				else if ( key_press == BAWAH )
				{
					key_index++;	
					if (key_index > 10) key_index = 0;		// 9
				}
				else if ( key_press == KANAN )
				{
					mesin_index++;
					if (mesin_index > 4) mesin_index = 0;
				}
				else if ( key_press == OK)
				{
					jum_OK++;	
				}
				else if ( key_press == CANCEL)
				{
					//jum_OK--;	
				}
				menu_OK(key_index, mesin_index, jum_OK);
				jum_OK = 0;
				
				menu_monita(key_index);
				menu_pilih(key_index, mesin_index, 0);		
				
				update_lcd();
				loop = 0;
			}	
		}	
		else
		{
			loop_key = 0;	
		}
		
		if (loop > 10)
		{
			
			if (key_index != 10)
			{
			/* force untuk update layar */
			cls_layar();
			menu_monita(key_index);		
			menu_pilih(key_index, mesin_index, 0);
			menu_OK(key_index, mesin_index);
			update_lcd();
			}
			
			loop = 0;
			
			loop_per_menit++;
			if (loop_per_menit > 60)
			{
				//printf("hitung \r\n");
				loop_per_menit = 0;
				hitung_data_hitung();
			}	
		}
		vTaskDelay(100);
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

/* 
	menghitung konsumsi bahan bakar per menit,
	fungsi ini akan dipanggil 1 kali per detik jika sedang dipilih
	jadi akan dipanggil 60 kali per 1 menit.
	
	jika sudah dipanggil 60 kali, maka didapatkan konsumsi
	per menitnya
	
	hitung juga kwh.
	sfc dapat dihitung dari data permenit juga.
*/
	
void hitung_data_hitung(void)
{
	int i;
	unsigned int pas_mesin;
	float selisih_in_out;
	
	for (i=0; i<JML_MESIN; i++)
	{
		pas_mesin = TIAP_MESIN * i;
		
		data_hitung[i].bb_in = titik[pas_mesin + OFFSET_BB_IN].data - data_hitung[i].bb_in_lama;
		data_hitung[i].bb_in_lama = titik[pas_mesin + OFFSET_BB_IN].data;
		
		data_hitung[i].bb_out = titik[pas_mesin + OFFSET_BB_OUT].data - data_hitung[i].bb_out_lama;
		data_hitung[i].bb_out_lama = titik[pas_mesin + OFFSET_BB_OUT].data; 
		
		data_hitung[i].kwh = titik[pas_mesin + OFFSET_KWH].data - data_hitung[i].kwh_lama;
		data_hitung[i].kwh_lama = titik[pas_mesin + OFFSET_KWH].data; 
		
		selisih_in_out = data_hitung[i].bb_in - data_hitung[i].bb_out;
		
		if (selisih_in_out > 0 && data_hitung[i].kwh > 0)
		{
			data_hitung[i].sfc = selisih_in_out / data_hitung[i].kwh;
		}
		else
			data_hitung[i].sfc = 0;
	}
}
