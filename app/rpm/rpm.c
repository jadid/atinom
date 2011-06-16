

#include "FreeRTOS.h"

#ifndef __RPM__
#define __RPM__

#ifdef BOARD_KOMON_KONTER

#include "monita/monita_uip.h"
#include "../../tinysh/enviro.h"
#include "GPIO/gpio.h"

extern unsigned int data_putaran[];
extern unsigned int data_hit[];	
extern struct t2_konter konter;

int rtcRate[KANALNYA];	// 	__attribute__ ((section (".rtcram_rate")));
extern float data_f[];

void baca_rtc_mem() {
	int i;
	for (i=0; i<KANALNYA; i++) {
		konter.t_konter[i].hit = rtcRate[i];
	}
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

#endif
#endif
