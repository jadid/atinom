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

//#define TIPE_PM710
#include "../modbus/low_mod.h"
//#include "../monita/monita_uip.h"

#ifdef PAKAI_KTA
	unsigned char addr_KTA;
//unsigned short get_KTA(unsigned short reg, unsigned char uk);
#endif

#ifdef PAKAI_PM

extern float data [ (JML_SUMBER * PER_SUMBER) ];

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
#define TXDE	BIT(24)

/* menampilkan data pada layar */
/*
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
//*/

static void proses_pm(void)
{
	unsigned int jum_balik;
	int i,j;
	char *st;
	int timeout=0;
	int muternya=0;

	konting2++;
	
	//goto_xy(1, 7);
	//printw("Send %1d : %2d :", urut_PM710, konting2);
	
	if (urut_PM710 == 0)	{
	   jum_balik = get_PM710(reg_satuan, 4);
	   //printf("_________masuk  urut %d, jmlBalik: %d\r\n", urut_PM710, jum_balik );
	} else if (urut_PM710 == 1)	{
	   //jum_balik = get_PM710(reg_kva, 7);  //kVA, kVAR, PF, volt L-L, L-N, A, Hz		// 4010
	   //printf("_________masuk  urut %d, jmlBalik: %d\r\n", urut_PM710, jum_balik );
	} else if (urut_PM710 == 2)	{
	   //jum_balik = get_PM710(reg_ampA, 4); //ampA, B, C & N								// 4020
	   //printf("_________masuk  urut %d, jmlBalik: %d\r\n", urut_PM710, jum_balik );
	} else if (urut_PM710 == 3) {
	   //jum_balik = get_PM710(reg_voltA_C, 6); //voltA_B, B_C, A_C, A_N, B_N & C_N
	   //printf("_________masuk  urut %d, jmlBalik: %d\r\n", urut_PM710, jum_balik );
	} else if (urut_PM710 == 4)	{
	   jum_balik = get_PM710(reg_kwh, 6);  //kWh, kVAh, & kVArh
	} else if (urut_PM710 == 5) {
		#ifdef PAKAI_KTA
		jum_balik = get_KTA(reg_KTA, 9);			// 30, 9
		//printf("_________masuk  urut 5, jmlBalik: %d\r\n", jum_balik );
		#endif
	} else if (urut_PM710 == 6) {
		#ifdef PAKAI_KTA
		jum_balik = get_KTA(reg_satuan_KTA, 6);
		//printf("_________masuk  urut 6, jmlBalik: %d\r\n", jum_balik );
		#endif
	}

	//printf("Minta ke PM -%d : %d : ", urut_PM710, sizeof(pmod));
	st = (char *) &pmod;
	FIO0SET = TXDE;		// on	---> bisa kirim
	for (i=0; i< sizeof(pmod); i++)
	{
		//xSerialPutChar2( 0, *st++, 100 );
		//printf("%02hX ", *st);
		serX_putchar(PAKAI_PM, st++, 100);
		/*
		#if (PAKAI_PM == 1) 
			ser1_getchar(1, &buf_rx[0], 0);
		#elif (PAKAI_PM == 2) 			
			ser2_getchar(1, &buf_rx[0], 0);
		#elif (PAKAI_PM == 3)
			ser3_getchar(1, &buf_rx[0], 0);
		#endif
		//*/
	}
	//FIO0SET = TXDE;		// on	---> bisa kirim
	//printf("\r\n");
	
	//printf("Send %1d : %2d :", urut_PM710, jum_balik);
	
	/*
	st = (char *) &pmod;
	for (i=0; i< sizeof(pmod); i++)
	{
		printw("%2X ", (unsigned char) *st++);
	}*/
	
	
	i=0;

		
	while(1)
	{
		//*
		#if (PAKAI_PM == 1) 
			if (ser1_getchar(1, &buf_rx[i], 100 ) == pdTRUE)
		#elif (PAKAI_PM == 2) 			
			if (ser2_getchar(1, &buf_rx[i], 100 ) == pdTRUE)
		#elif (PAKAI_PM == 3)
			if (ser3_getchar(1, &buf_rx[i], 100 ) == pdTRUE)
		#endif 
		//*/
		//if (serX_getchar(PAKAI_PM, buf_rx[i], 100 ) == pdTRUE)
		{
			/*
			if (urut_PM710==4) {
				printf("%02hX ",buf_rx[i]);
			}
			//*/
			i++;
			#if (PAKAI_MAX485 == 1) 
				if (i == jum_balik+sizeof(pmod)) break;
			#else
				if (i == jum_balik) break;
			#endif

		}
		else
		{
			timeout++;
			if (timeout > 20)
			{
				//printf("%s(): timeout\r\n", __FUNCTION__);
				break;
			}
		}
	}
	//printf("\r\n");
	//vTaskDelay(100);
	/*
	for (i=0; i<jum_balik; i++) {
		printf("%02hhX ", buf_rx[i]);	
	}
	printf("\r\n");
	//*/
	//for (i=0; i<jum_balik; i++)
	//	printw("%X ", buf_rx[i]);	
	/*
	if (urut_PM710==5) {
		printf("Nilai KTA   : ");
		for (i=0; i<jum_balik; i++) {
			printf("%02hhX ", buf_rx[i]);	
		}
		printf("\r\n");
	}
	if (urut_PM710==6) {
		printf("Satuan  KTA : ");
		for (i=0; i<jum_balik; i++) {
			printf("%02hhX ", buf_rx[i]);	
		}
		printf("\r\n");
	}
	//*/
	
	taruh_data(0, urut_PM710);
	
	//struct t_sumber *sumber;
	//sumber = (char *) ALMT_SUMBER;
	
	memcpy( (char *) &data_f[0], (char *) &asli_PM710[0], (PER_SUMBER*sizeof (float)) );
	
	/*
	// simpan data //
	for (i=0; i<JML_SUMBER; i++) {
		if (sumber[i].status == 1 && sumber[i].alamat > 0) {
			//printf("masuk data SUMBER: %d\r\n",i+1);
			memcpy( (char *) &data_f[i*PER_SUMBER], (char *) &asli_PM710[0], (PER_SUMBER*sizeof (float)) );
			
		}
	}
	//*/
	
	//urut_PM710++;
	//if (urut_PM710 > 6) urut_PM710 = 0;
	urut_PM710++;
	#ifdef PAKAI_KTA
	if (urut_PM710 > 6) 
	#else
	if (urut_PM710 > 4) 
	#endif
	urut_PM710 = 0;
	
}


portTASK_FUNCTION( pm_task, pvParameters )	{
	int i,j=0;
	
	urut_PM710 = 0;
	konting2 = 0;
	#ifdef PAKAI_KTA
	addr_KTA = 1;
	#endif
	int muternya=0;
	
	vTaskDelay(1010);
	
	//serial2_init( 19200, 64);
	
	// flush RX
	for (i=0; i<100; i++)
		//*
		#ifdef PAKAI_SERIAL_1
			ser1_getchar(1, &loop, 20 );
		#endif
		#ifdef PAKAI_SERIAL_2
			ser2_getchar(1, &loop, 100 );
		#endif
		#ifdef PAKAI_SERIAL_3
			ser3_getchar(1, &loop, 100 );
		#endif
		//*/
	for (;;)	{
		vTaskDelay(95);
		proses_pm();
		/*
		if (muternya>5) {
			printf("__________data %d [0]: %.2f, [1]: %.2f, [7]: %.2f, [10]: %.2f\r\n", j++, data_f[0], data_f[1], data_f[7], data_f[10]);
			//printf("__________data V_f: %f, angin: %.2f, %d, arah: %e\r\n", asli_PM710[0].frek, f_wind_speed, wind_speed, wind_dir_tr);
			//printf("__________data V_f: %.2f, volt: %.2f\r\n", asli_PM710[0].frek, asli_PM710[0].volt1);
			muternya=0;
		}
		muternya++;
		//*/
		loop++;
	}
		
}

void init_task_pm(void)
{
	xTaskCreate( pm_task, ( signed portCHAR * ) "PM_S", (configMINIMAL_STACK_SIZE * 10), \
		NULL, tskIDLE_PRIORITY + 1, &hdl_proses_pm );	
}

#endif 

/*
#ifdef PAKAI_KTA
//unsigned short 	wind_dir;
//float 			f_wind_speed;

//unsigned short wind_satuan;
//unsigned short wind_speed_tr;		// retransmision value
//unsigned short wind_dir_tr;



unsigned short get_KTA(unsigned short reg, unsigned char uk) {
   unsigned short dcrc;
   int i;

   reg_flag = reg;

   if (reg != reg_kwh)
   reg = reg - 1;

   pmod.addr = (unsigned char)   addr_KTA;
   pmod.command = (unsigned char) command_baca;
   pmod.reg_hi = (unsigned char) ((reg & 0xFF00) >> 8);
   pmod.reg_lo = (unsigned char) (reg & 0x00FF);
   pmod.jum_hi = (unsigned char) ((uk & 0xFF00) >> 8);
   pmod.jum_lo = (unsigned char) (uk & 0x00FF);;

   dcrc = usMBCRC16((unsigned char *) &pmod, sizeof (pmod)-2);
   pmod.crc_lo = (unsigned char) ((dcrc & 0xFF00) >> 8);
   pmod.crc_hi = (unsigned char) (dcrc & 0x00FF);;

   	return (1 + 1 + 1 + (uk * 2) + 2);
}


#endif
//*/



