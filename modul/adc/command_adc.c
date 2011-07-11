/* 
	shell command berkaitan dengan adc
	15 april 2009
	furkan jadid, daun biru engineering
	
	*/

#ifndef __COMMAND_ADC__
#define __COMMAND_ADC__

#include "ad7708.h"
#include "../tinysh/enviro.h"

#if defined(BOARD_KOMON_420_SABANG_2_3)
//	#include "../../hardware/hardware.h"
#endif

#ifdef PAKAI_ADC
extern struct t_adc st_adc;

void simpan_ke_data_f() {
	
	#if defined(BOARD_KOMON_420_SAJA) || defined(BOARD_KOMON_420_SABANG_2_3) || defined(BOARD_KOMON_420_SABANG)
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

	#ifdef BANYAK_SUMBER
		struct t_sumber *sumber;
		sumber = (char *) ALMT_SUMBER;
		
		// kenapa bisa redeclaration ???
		//struct t_env *envx;
		//envx = (char *) ALMT_ENV;
		
		//float temp;
		//int g=0;
		int sararumber=-1;
		#if defined(BOARD_KOMON_420_SABANG) || defined (BOARD_KOMON_420_SABANG_2_3)
			
			for (g=0; g<JML_SUMBER; g++) {
				if (sumber[g].status==1) {
					/* yang ini ko ga bisa ????
					if ((sumber[g].IP0==envx->IP0) && (sumber[g].IP1==envx->IP1) && (sumber[g].IP2==envx->IP2) && (sumber[g].IP3==envx->IP3) ) {
						sararumber=g;
					}
					//*/
					//*
					if (sumber[g].IP0==envx->IP0) {
						//printf("..1");
						if (sumber[g].IP1==envx->IP1) {
							//printf("..2");
							if (sumber[g].IP2==envx->IP2) {
								//printf("..3");
								if (sumber[g].IP3==envx->IP3) {
									if (sumber[g].alamat==0) {
										sararumber=g;
										break;
									}
									//printf("\r\n");
								}
							}
						}
					}
					//*/
				}
			}
			
			//printf("sumber: %d, IP: %d.%d.%d.%d\r\n", sararumber, envx->IP0, envx->IP1, envx->IP2, envx->IP3);
			if (sararumber>=0) {
				for (g=0; g<KANALNYA; g++) {
					if (envx->kalib[g].status==0) {		// tegangan
						temp = st_adc.data[g] * faktor_pengali_420 / 0xffff;
						data_f[sararumber*PER_SUMBER+g] = (float) (temp * envx->kalib[g].m) + envx->kalib[g].C;
						//printf("%d -Suhu/Tegangan- %d, nilai: %6.2f\r\n",(g+1), st_adc.data[g], data_f[g]);//(float) (temp_rpm * env2->kalib[i].m) + env2->kalib[i].C;
					} else {		// onoff
						if (st_adc.data[g]>10000) {		// on/tertutup
							data_f[sararumber*PER_SUMBER+g] = 1;
						} else {		// off/terbuka
							data_f[sararumber*PER_SUMBER+g] = 0;
						}
						//printf("%d -OnOff- %2.0f\r\n",(g+1), st_adc.data[g], data_f[g]);
					}
					//printf("%d - %4.2f\r\n",(g+1), data_f[sararumber*PER_SUMBER+g]);
				}
			}
		#endif	
	#else 	// 1 sumber saja
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
	
	#if defined(BOARD_KOMON_420_SABANG) || defined(BOARD_KOMON_420_SABANG_2_3)
		printf("Status ADC: Voltage 4-20 mA / OnOff\r\n");
		for (i=0; i<KANALNYA; i++)	{
			//*
			if(envx->kalib[i].status) {
				printf("%2d : %6d : %s\r\n", (i+1), st_adc.data[i], (st_adc.data[i]>10000)?"On/Tertutup":"Off/Terbuka");
				// 
			} else 	{
				f = st_adc.data[i] * faktor_pengali_420 / 0xffff;
				printf("%2d : %6d : %1.4f V\r\n", (i+1), st_adc.data[i], f);
			}
		}
	#endif
}							 

static tinysh_cmd_t cek_adc_cmd={0,"status_adc","konversi adc & voltnya","[args]",
                              status_adc,0,0,0};
#endif

#endif
