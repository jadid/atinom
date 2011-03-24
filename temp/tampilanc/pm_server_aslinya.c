/*
	proses PM_sever
	
	1 Des 2009, furkan jadid
	daun biru engineering
	
*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include <math.h>
#include <float.h>

#define TIPE_PM710
#include "../modbus/low_mod.h"
struct d_pmod pmod;

xTaskHandle hdl_proses_pm;
static	int urut_PM710 = 0;
static	int konting2 = 0;
static	int loop;
static 	char buf_rx[64];

float satuan_kwh;
float satuan_kw;
float satuan_volt;
float satuan_amp;

#include "../modbus/low_mod.c"

#define clrtoeol()	do { } while(0)
#define move_ke(a,b)	goto_xy(a,b)

#define KIRI_TENGAH		20

/* menampilkan data pada layar */
void cetak_pm(void)
{
	int nomer_mod = 0;
	int ploop;
	
	ploop = loop;
	
	move_ke(2, 10);
	printw("kWh   = %.2f", asli_PM710[ nomer_mod ].kwh);
            		
	move_ke(2, 11);
	printw("kVAh  = %.2f", asli_PM710[ nomer_mod ].kvah);
	
	move_ke(2, 12);
	printw("kVArh = %.2f", asli_PM710[ nomer_mod ].kvarh);
	
	move_ke(2, 13);
	printw("kW    = %.3f", asli_PM710[ nomer_mod ].kw);
	
	move_ke(2, 15);
	printw("kVA   = %.3f", asli_PM710[ nomer_mod ].kva);
	
	move_ke(2, 16);
	printw("kVAr  = %.3f", asli_PM710[ nomer_mod ].kvar);
	
	move_ke(2, 17);
	printw("pf    = %.3f", asli_PM710[ nomer_mod ].pf);
	
	move_ke(2, 18);
	printw("Volt1 = %.2f", asli_PM710[ nomer_mod ].volt1);
	
	move_ke(2, 19);
	printw("Volt2 = %.2f", asli_PM710[ nomer_mod ].volt2);
	
	move_ke(2, 21);
	printw("frek  = %.3f", asli_PM710[ nomer_mod ].frek);

		
	move_ke(KIRI_TENGAH, 10);
	printw("AmpA  = %.2f", asli_PM710[ nomer_mod ].ampA);
	
	move_ke(KIRI_TENGAH, 11);
	printw("AmpB  = %.2f", asli_PM710[ nomer_mod ].ampB);
	
	move_ke(KIRI_TENGAH, 12);
	printw("AmpC  = %.2f", asli_PM710[ nomer_mod ].ampC);
	
	move_ke(KIRI_TENGAH, 13);
	printw("AmpN  = %.2f", asli_PM710[ nomer_mod ].ampN);
	
	move_ke(KIRI_TENGAH, 15);
	printw("VoltA_B  = %.2f", asli_PM710[ nomer_mod ].voltA_B);
	
	move_ke(KIRI_TENGAH, 16);
	printw("VoltB_C  = %.2f", asli_PM710[ nomer_mod ].voltB_C);
	
	move_ke(KIRI_TENGAH, 17);
	printw("VoltA_C  = %.2f", asli_PM710[ nomer_mod ].voltA_C);
	
	move_ke(KIRI_TENGAH, 19);
	printw("VoltA_N  = %.2f", asli_PM710[ nomer_mod ].voltA_N);
	
	move_ke(KIRI_TENGAH, 20);
	printw("VoltB_N  = %.2f", asli_PM710[ nomer_mod ].voltB_N);
	
	move_ke(KIRI_TENGAH, 21);
	printw("VoltC_N  = %.2f", asli_PM710[ nomer_mod ].voltC_N);	
	
	
	move_ke(2, 23);
	printw("loop pm %d", ploop);
}


static void proses_pm(void)
{
	unsigned int jum_balik;
	int i;
	char *st;
	int timeout=0;

	//konting2++;
	
	//goto_xy(1, 7);
	//printw("Send %1d : %2d :", urut_PM710, konting2);
	
	if (urut_PM710 == 0)
	{
	   jum_balik = get_PM710(reg_satuan, 4);
	}
	else if (urut_PM710 == 1)
	{
	   jum_balik = get_PM710(reg_kva, 7);  //kVA, kVAR, PF, volt L-L, L-N, A, Hz
	}
	else if (urut_PM710 == 2)
	{
	   jum_balik = get_PM710(reg_ampA, 4); //ampA, B, C & N
	}
	else if (urut_PM710 == 3)
	{
	   jum_balik = get_PM710(reg_voltA_C, 6); //voltA_B, B_C, A_C, A_N, B_N & C_N
	}
	else if (urut_PM710 == 4)
	{
	   jum_balik = get_PM710(reg_kwh, 6);  //kWh, kVAh, & kVArh
	   
	}	
	
	st = (char *) &pmod;
	for (i=0; i< sizeof(pmod); i++)
	{
		xSerialPutChar2( 0, *st++, 100 );			
	}
	
	/*
	st = (char *) &pmod;
	for (i=0; i< sizeof(pmod); i++)
	{
		printw("%2X ", (unsigned char) *st++);
	}*/
	
	vTaskDelay(10);
	i=0;
	
	//goto_xy(1,8);
	//printw("Recv : ");
		
	while(1)
	{			
		if (ser2_getchar(1, &buf_rx[i], 100 ) == pdTRUE)
		{	
			i++;
			if (i == jum_balik) break;
		}
		else
		{
			timeout++;
			if (timeout > 20)
			{
				printf("%s(): timeout\r\n", __FUNCTION__);
				break;
			}
		}
	}
	
	//for (i=0; i<jum_balik; i++)
	//	printw("%X ", buf_rx[i]);	

	taruh_data(0, urut_PM710);
				
	urut_PM710++;
	if (urut_PM710 > 4) urut_PM710 = 0;
}


portTASK_FUNCTION( pm_task, pvParameters )
{
	int i;
	
	urut_PM710 = 0;
	konting2 = 0;
	
	vTaskDelay(1010);
	
	serial2_init( 19200, 64);
	
	// flush RX
	for (i=0; i<100; i++)
		ser2_getchar(1, &loop, 20 );
	
	for (;;)
	{
		vTaskDelay(95);
		proses_pm();	
		loop++;
	}
		
}

void init_task_pm(void)
{
	xTaskCreate( pm_task, ( signed portCHAR * ) "PM_S", (configMINIMAL_STACK_SIZE * 10), \
		NULL, tskIDLE_PRIORITY + 1, &hdl_proses_pm );	
}
