/*
	firmware tampilan untuk FreeRTOS
	furkan jadid
	14 desember 2008
	daun biru engineering
	
	*/
#include "FreeRTOS.h"
#include "task.h"
#include "../monita/monita_uip.h"

#define PF10	BIT(15)		// PF1
#define PF14	BIT(19)		// PF1

#define PF11	BIT(16)
#define PF12	BIT(17)
#define PF13	BIT(18)

#define KEY_DAT	(PF11 | PF12 | PF13)

#define ATAS		0
#define BAWAH	131072
#define KANAN	65536
#define OK		196608
#define CANCEL	262144

#define JML_SUMBER	20
#define JML_MESIN	10
#define TIAP_MESIN	70

struct t_sumber sumber[JML_SUMBER];
struct t_mesin	mesin[JML_MESIN];
struct t_titik	titik[TIAP_MESIN * JML_MESIN];

extern xTaskHandle *hdl_tampilan;

void set_awal_mesin(void);
void set_awal_sumber(void);
void set_awal_titik(void);

unsigned char tek[64];	

portTASK_FUNCTION( tampilan_task, pvParameters )
{
	unsigned int key_press;
	unsigned char key_index=0;
	unsigned char mesin_index=0;
	int i;
	
	//teks_komik(14, 4, "Tampilan Monita");
	// set PF14 & PF10 sebagai input interrupt keypad
	FIO1DIR = FIO1DIR  & ~(PF10 | PF14 | KEY_DAT);
	
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
	
	set_awal_mesin();
	set_awal_sumber();
	set_awal_titik();
	
	cls_layar();
	vTaskDelay(500);
	update_lcd();
	vTaskDelay(100);
	
	teks_h(14, 20, "Data Mesin :");
	for (i=0; i<5; i++)
	{
		teks_h(20, 30 + (i*9), mesin[i].nama);
		sprintf(tek, "%d", mesin[i].ID_mesin);	
		teks_h(100, 30 + (i*9), tek);
	}
	cls_layar();
	vTaskDelay(800);
	update_lcd();
	vTaskDelay(100);
	
	teks_h(14, 20, "Data Sumber Data :");
	for (i=0; i<20; i++)
	{
		teks_h(20, 30 + (i*9), sumber[i].nama);	
		
		// print out IP
		sprintf(tek,"%d.%d.%d.%d", sumber[i].IP0, sumber[i].IP1, sumber[i].IP2, sumber[i].IP3);
		teks_h(100, 30 + (i*9), tek);
		
		// status
		if (sumber[i].status == 0)
			teks_h(200, 30 + (i*9), "Null");
		if (sumber[i].status == 1)
			teks_h(200, 30 + (i*9), "Normal");
		if (sumber[i].status == 2)
			teks_h(200, 30 + (i*9), "TimeOut");
		
	}
	vTaskDelay(800);
	
	key_index = 0;
	mesin_index = 0;
	cls_layar();
	menu_monita();
	menu_pilih(key_index, mesin_index);
	update_lcd();	
	
		
	for (;;)
	{
		if ((FIO1PIN & PF14) == PF14)
		{
			cls_layar();
			
			// cek tombol apa yang ditekan
			key_press = (FIO1PIN & KEY_DAT);
			printf("tombol ditekan = %d\n", key_press);	
			
			if (key_press == ATAS)
			{
				key_index--;
				//if (key_index == 255) key_index = 4;
				if (key_index == 255) key_index = 6;
				else if (key_index == 5) key_index = 4;		   
			}
			else if ( key_press == BAWAH )
			{
				key_index++;	
				//if (key_index > 4) key_index = 0;
				if (key_index == 5) key_index =6;
				else if (key_index > 6) key_index = 0;
			}
			else if ( key_press == KANAN )
			{
				mesin_index++;
				if (mesin_index > 4) mesin_index = 0;
			}
			menu_monita();
			menu_pilih(key_index, mesin_index);
			update_lcd();	
			
		}
		vTaskDelay(100);	
	}
	
}

void init_task_tampilan(void)
{
	xTaskCreate( tampilan_task, ( signed portCHAR * ) "Tampilan", (configMINIMAL_STACK_SIZE * 5), NULL, tskIDLE_PRIORITY - 1, (xTaskHandle *) &hdl_tampilan);	
}

void set_awal_mesin(void)
{
	sprintf(mesin[0].nama, "Msn #1");
	sprintf(mesin[1].nama, "Msn #2");
	sprintf(mesin[2].nama, "Msn #3");
	sprintf(mesin[3].nama, "Msn #4");
	sprintf(mesin[4].nama, "Msn #5");
	
	mesin[0].ID_mesin = 1;
	mesin[1].ID_mesin = 2;
	mesin[2].ID_mesin = 3;
	mesin[3].ID_mesin = 4;
	mesin[4].ID_mesin = 5;		
}

void set_awal_sumber(void)
{
	int i;
	
	for (i=0; i<JML_SUMBER; i++)
	{
		sprintf(sumber[i].nama, "-");
		sumber[i].ID_sumber = i;
		sumber[i].status = 0;	
	}	
	
	sumber[2].status = 2;
	sumber[2].IP0 = 192;
	sumber[2].IP1 = 168;
	sumber[2].IP2 = 1;
	sumber[2].IP3 = 251;
	
}

void set_awal_titik(void)
{
	int i;
	
	for (i=0; i<(TIAP_MESIN * JML_MESIN) ; i++)
	{
		//sprintf(titik[i].nama, "-");
		//titik[i].ID_mesin = 0;
		titik[i].ID_sumber = 0;
		titik[i].kanal = 0;
	}	
	/*
	sprintf(titik[0].nama,"CA_P_L");
	sprintf(titik[1].nama,"CA_P_R");
	sprintf(titik[2].nama,"CA_T_L");
	sprintf(titik[3].nama,"CA_P_R");
	
	sprintf(titik[4].nama,"JW_P_in_1");
	sprintf(titik[5].nama,"JW_P_in_2");
	sprintf(titik[6].nama,"JW_P_ot_1");
	sprintf(titik[7].nama,"JW_P_ot_2");
	sprintf(titik[8].nama,"JW_T_in_1");
	sprintf(titik[9].nama,"JW_T_in_2");
	sprintf(titik[10].nama,"JW_T_ot_1");
	sprintf(titik[11].nama,"JW_T_ot_2");
	
	sprintf(titik[12].nama,"LO_P_in_1");
	sprintf(titik[13].nama,"LO_P_in_2");
	sprintf(titik[14].nama,"LO_P_ot_1");
	sprintf(titik[15].nama,"LO_P_ot_2");
	sprintf(titik[16].nama,"LO_T_in_1");
	sprintf(titik[17].nama,"LO_T_in_2");
	sprintf(titik[18].nama,"LO_T_ot_1");
	sprintf(titik[19].nama,"LO_T_ot_2");
	
	sprintf(titik[20].nama,"LO_P_in_1");
	*/
	
	
	
	
}
