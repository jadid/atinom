

#include "FreeRTOS.h"
#include "task.h"

#ifndef __SHELL_RPM__
#define __SHELL_RPM__

#if defined(BOARD_KOMON_KONTER) || defined(BOARD_KOMON_KONTER_3_0)

#include "GPIO/gpio.h"
#include "monita/monita_uip.h"
#include "enviro.h"

void reset_konter(void);

unsigned int data_putaran[KANALNYA];
unsigned int data_hit[KANALNYA];	
struct t2_konter konter;

int rtcRate[KANALNYA];	// 	__attribute__ ((section (".rtcram_rate")));
extern float data_f[];

unsigned int giliran;

void reset_konter(void)
{
	int i;
	
	konter.global_hit = 0;
	konter.ovflow = 0;
	giliran = 0;
	
	for (i=0; i<KANALNYA; i++)
	{
		konter.t_konter[i].last_period = 0;
		konter.t_konter[i].beda = 0;
		konter.t_konter[i].hit = 0;
		konter.t_konter[i].hit_lama = 0;
	}

}

void hitung_rpm(void)
{	
	portENTER_CRITICAL();
	if (konter.t_konter[giliran].hit_lama == konter.t_konter[giliran].hit)
	{
		//konter.t_konter[giliran].beda = 0;		
		data_putaran[giliran] = 0;
		data_hit[giliran] = konter.t_konter[giliran].hit;
	}
	else
	{
		/* didapatkan frekuensi (HZ) putaran */
		//temp_rpm = konter.t_konter[i].beda; // beda msh dlm nS
		
		/* rpm */
		data_putaran[giliran] = konter.t_konter[giliran].beda;
		data_hit[giliran] = konter.t_konter[giliran].hit;
	}
		
	konter.t_konter[giliran].hit_lama = konter.t_konter[giliran].hit; 
	portEXIT_CRITICAL();
	
	giliran++;
	if (giliran == KANALNYA) giliran = 0;
	
	#if 0
	//portENTER_CRITICAL();
	for (i=0; i<KANALNYA; i++)
	{
		if (konter.t_konter[i].hit_lama == konter.t_konter[i].hit)
		{
			konter.t_konter[i].beda = 0;
		}
		
		konter.t_konter[i].hit_lama = konter.t_konter[i].hit; 
	}
	//portEXIT_CRITICAL();
	#endif
	
}

void baca_rtc_mem() {
	int i;
	for (i=0; i<KANALNYA; i++) {
		konter.t_konter[i].hit = rtcRate[i];
	}
}

void set_konter(int st, unsigned int period)
{
	//new_period = T1TC;
	if (period > konter.t_konter[st].last_period)
	{
		konter.t_konter[st].beda = (period -
			konter.t_konter[st].last_period) * 50;	// 1 clock 50 nanosecond
	}
	else	// sudah overflow
	{
		konter.t_konter[st].beda = (period +
			(0xFFFFFFFF - konter.t_konter[st].last_period)) * 50;	// 1 clock 50 nanosecond

	}
	konter.t_konter[st].hit++;
	konter.t_konter[st].last_period = period;
}

void data_frek_rpm() {
	unsigned int i;
	float temp_f;
	float temp_rpm;
	
	struct t_env *env2;
	env2 = (char *) ALMT_ENV;
	
	for (i=0; i<KANALNYA; i++)	{
//		if (i>6) 
		{
			if (data_putaran[i])	{
				// cari frekuensi
				temp_f = (float) 1000000000.00 / data_putaran[i]; // beda msh dlm nS
				// rpm
				temp_rpm = temp_f * 60;
			}
			else	{
				temp_f = 0;
				temp_rpm = 0;
			}
			data_f[(i*2)+1] = (konter.t_konter[i].hit*env2->kalib[i].m)+env2->kalib[i].C;
			data_f[i*2] = (temp_rpm*env2->kalib[i].m)+env2->kalib[i].C;
			
			if (data_f[(i*2)+1]>10000000) {		// reset setelah 10juta, 7 digit
			//if (data_f[(i*2)+1]>1000) {		// tes saja, reset setelah 10juta, 7 digit
				data_f[(i*2)+1] = 0;
				konter.t_konter[i].hit = 0;
			}
			rtcRate[i] = (int) konter.t_konter[i].hit;
		}
	}	
}

void cek_rpm()	{
	unsigned int i;
	float temp_f;
	float temp_rpm;
	
	printf("Global hit = %d\n", konter.global_hit);
	printf("Ov flow = %d\n", konter.ovflow);

	for (i=0; i<10; i++)	{
		#ifndef BOARD_KOMON_KONTER_3_1
		if (i>6) 
		#endif
		{
			if (data_putaran[i])	{
				// cari frekuensi
				temp_f = (float) 1000000000.00 / data_putaran[i]; // beda msh dlm nS
				// rpm
				temp_rpm = temp_f * 60;
			}
			else	{
				temp_f = 0;
				temp_rpm = 0;
			}	
	
			printf(" %2d : F = %6.2f Hz, rpm = %7.2f, hit = %8.0f\r\n", \
				(i+1), temp_f, data_f[(i*2)], data_f[i*2+1]);
		}
	}
	
	#if (KONTER_MALINGPING == 1)
	// data kanal 1 adalah adc1 (adc0 internal) //
	extern float volt_supply;
	printf("\r\nADC 1 = %.3f\r\n", volt_supply );						
	#endif
}

static tinysh_cmd_t cek_rpm_cmd={0,"cek_rpm","data kounter/rpm","[args]", cek_rpm,0,0,0};

#endif		// BOARD_KOMON_KONTER
#endif		// __RPM__
