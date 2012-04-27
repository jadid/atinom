

#include "FreeRTOS.h"
#include "task.h"

#ifndef __SHELL_RPM__
#define __SHELL_RPM__

#if defined(BOARD_KOMON_KONTER) || defined(BOARD_KOMON_KONTER_3_0)

#include "GPIO/gpio.h"
#include "monita/monita_uip.h"
#include "enviro.h"
#include "hardware.h"

/*
#define	 sONOFF			1	 ada @ tinysh/enviro.h
#define	 sPUSHBUTTON	2
//*/

void reset_konter(void);
void hitung_rpm(void);

#ifdef PAKAI_RTC
void baca_rtc_mem(void);
void reset_mem_rtc(void);
#endif

void set_konter(int, unsigned int);

void set_konter_onoff(int i, int onoff);

#ifdef PAKAI_PILIHAN_FLOW
void set_konter_flow_pilih(int i, int onoff);
#endif

#ifdef PAKAI_ADC_ORI
	#include "../adc/adc_ori.h"
	extern struct t_adc_ori adco;
#endif

unsigned int data_putaran[KANALNYA];
unsigned int data_hit[KANALNYA];	
struct t2_konter konter;		// struct ada di gpio.h

//int rtcRate[KANALNYA];	// 	__attribute__ ((section (".rtcram_rate")));
extern float data_f[];

unsigned int giliran;


void cek_input_onoff(void)	{
	struct t_env *env2;
	env2 = (char *) ALMT_ENV;
	int i=0, ww=0, zzz;
	
	for (i=0; i<KANALNYA; i++)	{
		ww = env2->kalib[i].status; 
		if ( (ww==1) || (ww==2) || (ww==3) || (ww==4) || (ww==201) )	{
			switch (i)	{
				case 0:	zzz = 5; break;
				case 1: zzz = 6; break;
				case 2: zzz = 7; break;
				case 3: zzz = 8; break;
				case 4: zzz = 9; break;
				case 5: zzz = 0; break;
				case 6: zzz = 1; break;
				case 7: zzz = 2; break;
				case 8: zzz = 3; break;
				case 9: zzz = 4; break;
			}
			konter.t_konter[i].onoff = PORT_INPUT_KONTER(zzz);
		}
		
	}
}


void reset_konter(void)	{
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
		konter.t_konter[i].onoff = 0;		// 
		
		#ifdef PAKAI_PILIHAN_FLOW
		konter.t_konter[i].hit2 = 0;
		konter.t_konter[i].hit_lama2 = 0;
		#endif
		
		#ifdef PAKAI_MEM_RTC
		//	*(&MEM_RTC0+i) = 0;
		#endif
	}

}

void hitung_rpm(void)	{	
	//printf("%s() masuk ...\r\n", __FUNCTION__);
	struct t_env *env2;
	env2 = (char *) ALMT_ENV;
	
	if (env2->kalib[giliran].status==0)	
	{
		
		portENTER_CRITICAL();
		/*
		if (giliran==6)	{
			printf("hit: %d, lama: %d\r\n", konter.t_konter[giliran].hit, konter.t_konter[giliran].hit_lama);
		}
		//*/
		
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
			
			//if (giliran==6)
			//printf("data_putaran[%d]: %d\r\n", giliran, data_putaran[giliran]);
		}
		
		konter.t_konter[giliran].hit_lama = konter.t_konter[giliran].hit; 
		#ifdef PAKAI_PILIHAN_FLOW
		
		konter.t_konter[giliran].hit_lama2 = konter.t_konter[giliran].hit2; 
		#endif
		portEXIT_CRITICAL();
	}
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
	//printf("%s() .....\r\n", __FUNCTION__);
	int i,w;
	struct t_env *env2;
	env2 = (char *) ALMT_ENV;
	
	for (i=0; i<KANALNYA; i++) {
		#if defined(PAKAI_PILIHAN_FLOW)
			if ((env2->kalib[i].status==3) || (env2->kalib[i].status==100))	{
				if (i%2)	{	// 
					konter.t_konter[i].hit  = *(&MEM_RTC0+(i*2));
					konter.t_konter[i].hit2 = *(&MEM_RTC0+(i*2)+1);
					printf("   hit   %2d: %d : %d\r\n", \
						i+1, konter.t_konter[i].hit, konter.t_konter[i].hit2);
				} else {		// 
					konter.t_konter[i].onoff = *(&MEM_RTC0+(i*2));
					konter.t_konter[i].hit   = *(&MEM_RTC0+(i*2)+1);
					printf("   onoff %2d: %d : %d\r\n", \
						i+1, konter.t_konter[i].onoff, konter.t_konter[i].hit);
				}
			} else if ((env2->kalib[i].status==4) || (env2->kalib[i].status==201) || (env2->kalib[i].status==202) || (env2->kalib[i].status==203))	{
				if (env2->kalib[i].status==202)	{	// 
					konter.t_konter[i].hit  = *(&MEM_RTC0+(i*2));
					konter.t_konter[i].hit2 = *(&MEM_RTC0+(i*2)+1);
					printf("   hit   %2d: %d : %d\r\n", \
						i+1, konter.t_konter[i].hit, konter.t_konter[i].hit2);
				} else if (env2->kalib[i].status==203)	{
					konter.global_hit  = *(&MEM_RTC0+(i*2));
				} else {		// 
					konter.t_konter[i].onoff = *(&MEM_RTC0+(i*2));
					konter.t_konter[i].hit   = *(&MEM_RTC0+(i*2)+1);
					printf("   onoff %2d: %d : %d\r\n", \
						i+1, konter.t_konter[i].onoff, konter.t_konter[i].hit);
				}
			} 
		#else
			if (env2->kalib[i].status==0)	{
				konter.t_konter[i*2].hit = *(&MEM_RTC0+i*2);
				printf(" pulsa %d : %d\r\n", i+1, konter.t_konter[i*2].hit);
			}
		#endif
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

void set_konter_onoff(int i, int onoff) {	// hanya menambah jml counter hit tiap kanal saja
	//konter.t_konter[i].onoff = onoff;		// status dilihat dari GPIO (polling) saja
	konter.t_konter[i].hit++;
}

#ifdef PAKAI_PILIHAN_FLOW
void set_konter_flow_pilih(int i, int onoff)	{
	
	if (onoff==202)	{
		if (konter.t_konter[i-2].onoff==1)	
			konter.t_konter[i].hit++;
		else if (konter.t_konter[i-1].onoff==1)	
			konter.t_konter[i].hit2++;
	} else {
		if (onoff)	konter.t_konter[i].hit++;
		else		konter.t_konter[i].hit2++;
	}
}
#endif

int qwe=0;

void data_frek_rpm() {
	//printf("%s() masuk ...\r\n", __FUNCTION__);
	unsigned int i;
	char status;
	float temp_f;
	float temp_rpm;
	
	struct t_env *env2;
	env2 = (char *) ALMT_ENV;
	
	for (i=0; i<KANALNYA; i++)	{
		status = env2->kalib[i].status;
		if (status==0) 				// RPM
//		if (i>6) 
		{
			if (data_putaran[i])	{
				// cari frekuensi
				temp_f = (float) 1000000000.00 / data_putaran[i]; // beda msh dlm nS
				// rpm
				temp_rpm = temp_f;		// ganti ke rps * 60;
				
				if (i==6) {
					//printf("data_putaran[%d]: %d, temp_f: %.3f\r\n", i, data_putaran[i], temp_f);
				}
			}
			else	{
				temp_f = 0;
				temp_rpm = 0;
			}
			
			if (i==6) {
			//		printf("data_putaran[%d]: %d, temp_f: %.3f\r\n", i, data_putaran[i], temp_f);
			}
			
			data_f[(i*2)+1] = (konter.t_konter[i].hit*env2->kalib[i].m)+env2->kalib[i].C;
			data_f[i*2] = (float) (temp_rpm*env2->kalib[i].m)+env2->kalib[i].C;
			
			/*
			qwe++;
			if (qwe>2000)	{
				if (i==6)	{
					printf("giliran: %d, data_putaran[%d]: %d, temp_f: %.3f, hit: %d, hitlama: %d\r\n", \
						giliran, i, data_putaran[i], temp_f, konter.t_konter[i].hit, konter.t_konter[i].hit_lama);
					printf("RPM%d: temp_f: %.3f, data_f: %f, data_putaran: %d, lp: %d, beda: %d\r\n", \
						i+1, temp_rpm, data_f[12], data_putaran[i], \
						konter.t_konter[i].last_period, konter.t_konter[i].beda);
				}
				qwe=0;
			}
			//*/
			
			/*
			if (data_f[(i*2)+1]>10000000) {		// reset setelah 10juta, 7 digit
			//if (data_f[(i*2)+1]>1000) {		// tes saja, reset setelah 10juta, 7 digit
				data_f[(i*2)+1] = 0;
				konter.t_konter[i].hit = 0;
			}
			//*/
			//rtcRate[i] = (int) konter.t_konter[i].hit;
			#ifdef PAKAI_RTC
			//*(&MEM_RTC0+(i*2))   = data_f[i*2];		// konter.t_konter[i].onoff;
			//*(&MEM_RTC0+(i*2+1)) = data_f[i*2+1];		// konter.t_konter[i].hit;
			#endif
		}	else if (status==sONOFF || status==sFLOW1 || status==sFLOW2 || status==ssFLOW2) {		// OnOff
			data_f[i*2] = (float) konter.t_konter[i].onoff;
			data_f[i*2+1] = (float) konter.t_konter[i].hit;
			
			#ifdef PAKAI_RTC
			*(&MEM_RTC0+(i*2))	 = data_f[i*2];		// konter.t_konter[i].onoff;
			*(&MEM_RTC0+(i*2+1)) = data_f[i*2+1];	// konter.t_konter[i].hit;
			#endif
		/*
		} else if (status==3) {		// selector_flow
			data_f[i*2]   = (float) konter.t_konter[i].onoff;
			data_f[i*2+1] = (float) konter.t_konter[i].hit;
			
			#ifdef PAKAI_RTC
			*(&MEM_RTC0+(i*2))	 = data_f[i*2];		// konter.t_konter[i].onoff;
			*(&MEM_RTC0+(i*2+1)) = data_f[i*2+1];	// konter.t_konter[i].hit;
			#endif
			//printf("   %d : %d, %d : %d,", (i*2), *(&MEM_RTC0+(i*2)), (i*2+1), *(&MEM_RTC0+(i*2+1)));
		//*/

		/*
		} else if (status==4) {		// selector_flow
			data_f[i*2]   = (float) konter.t_konter[i].onoff;
			data_f[i*2+1] = (float) konter.t_konter[i].hit;
			
			*(&MEM_RTC0+(i*2))	 = data_f[i*2];		// konter.t_konter[i].onoff;
			*(&MEM_RTC0+(i*2+1)) = data_f[i*2+1];	// konter.t_konter[i].hit;
		} else if (status==201) {		// selector_flow
			data_f[i*2]   = (float) konter.t_konter[i].onoff;
			data_f[i*2+1] = (float) konter.t_konter[i].hit;
			
			*(&MEM_RTC0+(i*2))	 = data_f[i*2];		// konter.t_konter[i].onoff;
			*(&MEM_RTC0+(i*2+1)) = data_f[i*2+1];	// konter.t_konter[i].hit;
		//*/
		#ifdef PAKAI_PILIHAN_FLOW
		} else if (status==nFLOW1) {		// flow_setelah_selector
			data_f[i*2]   = (float) konter.t_konter[i].hit;
			data_f[i*2+1] = (float) konter.t_konter[i].hit2;
			
			#ifdef PAKAI_RTC
			*(&MEM_RTC0+(i*2))	 = data_f[i*2];			// konter.t_konter[i].hit;
			*(&MEM_RTC0+(i*2+1)) = data_f[i*2+1];		// konter.t_konter[i].hit2;
			#endif
			//printf("   %d : %d, %d : %d", (i*2), *(&MEM_RTC0+(i*2)), (i*2+1), *(&MEM_RTC0+(i*2+1)));
		} else if (status==nFLOW2) {		// Hitung flow_setelah_selector, dengan kalibrasi
			//data_f[i*2]   = (float) konter.t_konter[i].hit;
			//data_f[i*2+1] = (float) konter.t_konter[i].hit2;

			data_f[i*2]   = (konter.t_konter[i].hit*env2->kalib[i].m)+env2->kalib[i].C;
			data_f[i*2+1] = (konter.t_konter[i].hit2*env2->kalib[i].m)+env2->kalib[i].C;
			
			#if 1
			if (data_f[i*2]>9999999.99)	{
				konter.t_konter[i].hit = 0;
			}
			
			if (data_f[i*2+1]>9999999.99)	{
				konter.t_konter[i].hit2 = 0;
			}
			#endif
			
			//*(&MEM_RTC0+(i*2))   = data_f[i*2];		// konter.t_konter[i].hit;
			//*(&MEM_RTC0+(i*2+1)) = data_f[i*2+1];		// konter.t_konter[i].hit2;
			#ifdef PAKAI_RTC
			*(&MEM_RTC0+(i*2))	 = konter.t_konter[i].hit;		// konter.t_konter[i].hit;
			*(&MEM_RTC0+(i*2+1)) = konter.t_konter[i].hit2;		// konter.t_konter[i].hit2;
			#endif
		} else if (status==fFLOW) {		// flow_setelah_selector = 203
			data_f[i*2]   = (float) konter.global_hit;
			data_f[i*2+1] = (float) konter.ovflow;
			
			#ifdef PAKAI_RTC
			*(&MEM_RTC0+(i*2))	 = konter.global_hit;
			*(&MEM_RTC0+(i*2+1)) = konter.ovflow;
			#endif
		#endif

		#ifdef PAKAI_ADC_ORI
		} else if (status==sADC_ORI)	{
			//printf("kanal[%d]: %d\r\n", kanalx-1, adco.data[kanalx-1]);
			konter.t_konter[i].hit = adco.data[(env2->kalib[i].adc-1)];
			
			data_f[i*2]   = (float) (konter.t_konter[i].hit*env2->kalib[i].m)+env2->kalib[i].C;
			data_f[i*2+1] = (float) konter.t_konter[i].hit;
		#endif
		}
		//printf("\r\n");
	}
}

void cek_kanal()	{
	unsigned int i;
	float temp_f;
	float temp_rpm;
	
	printf("Global hit = %d\n", konter.global_hit);
	printf("Ov flow = %d\n", konter.ovflow);
	
	struct t_env *penv;
	penv = (char *) ALMT_ENV;

	for (i=0; i<10; i++)	{
		if (penv->kalib[i].status==0) {		// pulsa
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

				//printf(" %2d : F = %6.2f Hz, rpm = %7.2f, hit = %8.0f, lp: %d, beda: %d\r\n", \
				//	(i+1), temp_f, data_f[(i*2)], data_f[i*2+1], konter.t_konter[i].last_period, konter.t_konter[i].beda);
				printf(" %2d : F = %6.2f Hz, rpm = %7.2f, hit = %8.0f\r\n", \
					(i+1), temp_f, data_f[(i*2)], data_f[i*2+1]);

			}
		} else if (penv->kalib[i].status == sONOFF) {		// onoff = 1
			#if 0
				#ifndef BOARD_KOMON_KONTER_3_1
				if (i>6) 
				#endif
			#endif
			{
				printf(" %2d : Status: %2d [%s], Nilai: %d : %s, Hit: %3d\r\n", \
					(i+1), status_konter[i], (status_konter[i]==1)?"OnOff":"ERROR1", \
						konter.t_konter[i].onoff, (konter.t_konter[i].onoff==1)?"ON":"OFF", konter.t_konter[i].hit);
			}
		#ifdef PAKAI_PUSHBUTTON
		} else if (penv->kalib[i].status == sPUSHBUTTON) {		// pushbutton
			#if 0
				#ifndef BOARD_KOMON_KONTER_3_1
				if (i>6) 
				#endif
			#endif
			{
				printf(" %2d : Status: %2d [%s], Nilai: %d : %s, Hit: %d\r\n", \
					(i+1), status_konter[i], (status_konter[i]==2)?"PushButton":"ERROR2",	\
					 konter.t_konter[i].onoff, (konter.t_konter[i].onoff==1)?"on":"off", konter.t_konter[i].hit);
			}
		#endif
		#ifdef PAKAI_PILIHAN_FLOW
		} else if (penv->kalib[i].status == sFLOW1) {		// selector_flow = 3
			{
				printf(" %2d : Status: %2d [%s], Nilai: %d : %s, x: %d\r\n", \
					(i+1), status_konter[i], (status_konter[i]==sFLOW1)?"Selector flow":"ERROR3", \
						konter.t_konter[i].onoff, (konter.t_konter[i].onoff==1)?"on":"off", konter.t_konter[i].hit);
			}
		} else if (penv->kalib[i].status == nFLOW1) {		// flow_setelah_selector = 100
			{
				printf(" %2d : Status: %3d [%s],  Nilai %d: %8.0f, Nilai %d : %8.0f\r\n", \
					(i+1), status_konter[i], (status_konter[i]==nFLOW1)?"Flow":"ERROR100", \
						(i*2+1), /*konter.t_konter[i].hit*/data_f[i*2], (i*2+2), data_f[i*2+1]);
			}
		} else if (penv->kalib[i].status == sFLOW2) {		// selector_flow = 4
			{
				printf(" %2d : Status: %3d [%s], Nilai: %d : %s, x: %d\r\n", \
					(i+1), status_konter[i], (status_konter[i]==sFLOW2)?"Selector flow2":"ERROR4", \
						konter.t_konter[i].onoff, (konter.t_konter[i].onoff==1)?"on":"off", konter.t_konter[i].hit);
			}
		} else if (penv->kalib[i].status == ssFLOW2) {	// selector_flow = 201
			{
				printf(" %2d : Status: %3d [%s], Nilai: %d : %s, x: %d\r\n", \
					(i+1), status_konter[i], (status_konter[i]==ssFLOW2)?"Selector flow2":"ERROR4", \
						konter.t_konter[i].onoff, (konter.t_konter[i].onoff==1)?"on":"off", konter.t_konter[i].hit);
			}
		} else if (penv->kalib[i].status == nFLOW2) {	// flow_setelah_selector = 202
			{
				printf(" %2d : Status: %3d [%s],  Nilai %d: %8.0f, Nilai %d : %8.0f\r\n", \
					(i+1), status_konter[i], (status_konter[i]==nFLOW2)?"Flow":"ERROR4", \
						(i*2+1), /*konter.t_konter[i].hit*/data_f[i*2], (i*2+2), data_f[i*2+1]);
			}
		} else if (penv->kalib[i].status == fFLOW) {	// flow_setelah_selector = 203
			{
				printf(" %2d : Status: %3d [%s],  global : %d\r\n", \
					(i+1), status_konter[i], (status_konter[i]==fFLOW)?"Global":"ERROR4", konter.global_hit);
			}
		#endif
		#ifdef PAKAI_ADC_ORI
		} else if (penv->kalib[i].status == sADC_ORI)	{
			printf(" %2d : Status: %2d [%s], ADCori[%d]: %d\r\n", \
					(i+1), status_konter[i], (status_konter[i]==sADC_ORI)?"ADC Ori":"ERROR9", \
					penv->kalib[i].adc, konter.t_konter[i].hit);
		#endif
		}
		
	}

	#if (KONTER_MALINGPING == 1)
	// data kanal 1 adalah adc1 (adc0 internal) //
	extern float volt_supply;
	printf("\r\nADC 1 = %.3f\r\n", volt_supply );						
	#endif
}

static tinysh_cmd_t cek_rpm_cmd={0,"cek_kanal","data kounter/rpm","[args]", cek_kanal,0,0,0};

#endif		// BOARD_KOMON_KONTER
#endif		// __RPM__
