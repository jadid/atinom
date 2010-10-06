/* 
	shell command berkaitan dengan adc
	15 april 2009
	furkan jadid, daun biru engineering
	
	*/

#include "ad7708.h"
#include "../tinysh/enviro.h"

#ifdef PAKAI_ADC
extern struct t_adc st_adc;


void simpan_ke_data_f() {
	#ifdef BOARD_KOMON_420_SAJA
		int g;
		float temp;
		//extern struct t_adc st_adc;
		struct t_env *envx;
		envx = (char *) ALMT_ENV;
		
		for (g=0; g<KANALNYA; g++) {
			if (envx->kalib[g].status==0) {		// tegangan
				temp = st_adc.data[g] * faktor_pengali_420 / 0xffff;
				data_f[g] = (float) (temp * envx->kalib[g].m) + envx->kalib[g].C;
				//printf("%d -Suhu/Tegangan- %d, nilai: %6.2f\r\n",(g+1), st_adc.data[g], data_f[g]);//(float) (temp_rpm * env2->kalib[i].m) + env2->kalib[i].C;
			} else {		// onoff
				if (st_adc.data[g]>10000) {		// on/tertutup
					data_f[g] = 1;
				} else {		// off/terbuka
					data_f[g] = 0;
				}
				//printf("%d -OnOff- %2.0f\r\n",(g+1), st_adc.data[g], data_f[g]);
			}
		}
	#endif
}

void status_adc(void) {
	
	int i;
	float f;
	
	struct t_env *envx;
	envx = (char *) ALMT_ENV;
	
	printf(" ADC count = %d\r\n", st_adc.count);
	
	#ifdef BOARD_KOMON_B_THERMO
	for (i=0; i<10; i++)
	{
		f = st_adc.data[i] * faktor_pengali / 0xffff;
		printf("%2d : %6d : %1.4f V\r\n", (i+1), st_adc.data[i], f);
	}	
	#endif
	
	#ifdef BOARD_KOMON_A_RTD
	printf("Voltage RTD\r\n");
	for (i=0; i<5; i++)
	{
		f = st_adc.data[i] * faktor_pengali_RTD / 0xffff;
		printf("%2d : %6d : %1.4f V\r\n", (i+1), st_adc.data[i], f);
	}
	
	printf("Voltage 4-20 mA\r\n");
	for (i=5; i<10; i++)
	{
		f = st_adc.data[i] * faktor_pengali_420 / 0xffff;
		printf("%2d : %6d : %1.4f V\r\n", (i+1), st_adc.data[i], f);
	}
	#endif
	
	#ifdef BOARD_KOMON_420_SAJA
		printf("Status ADC: Voltage 4-20 mA / OnOff\r\n");
		for (i=0; i<KANALNYA; i++)
		{
			//*
			if(envx->kalib[i].status) {
				printf("%2d : %6d : %s\r\n", (i+1), st_adc.data[i], (st_adc.data[i]>10000)?"On/Tertutup":"Off/Terbuka");
				// 
			} else 
			//*/
			{
				f = st_adc.data[i] * faktor_pengali_420 / 0xffff;
				printf("%2d : %6d : %1.4f V\r\n", (i+1), st_adc.data[i], f);
			}
			
		}
	#endif
}							 

static tinysh_cmd_t cek_adc_cmd={0,"status_adc","konversi adc & voltnya","[args]",
                              status_adc,0,0,0};
#endif
