

#include "FreeRTOS.h"
#include "task.h"

#ifndef __SHELL_RPM__
#define __SHELL_RPM__

#if defined(BOARD_KOMON_KONTER) || defined(BOARD_KOMON_KONTER_3_0)

#include "GPIO/gpio.h"
#include "monita/monita_uip.h"
#include "enviro.h"
#include "hardware.h"

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

unsigned int data_putaran[KANALNYA];
unsigned int data_hit[KANALNYA];	
struct t2_konter konter;

//int rtcRate[KANALNYA];	// 	__attribute__ ((section (".rtcram_rate")));
extern float data_f[];

unsigned int giliran;


void cek_input_onoff(void)	{
	struct t_env *env2;
	env2 = (char *) ALMT_ENV;
	int i=0, ww=0, zzz;
	
	for (i=0; i<KANALNYA; i++)	{
		ww = env2->kalib[i].status; 
		if ( (ww==3) || (ww==4) || (ww==201) )	{
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

void hitung_rpm(void)
{	
	//printf("%s() masuk ...\r\n", __FUNCTION__);
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
	#ifdef PAKAI_PILIHAN_FLOW
	
	konter.t_konter[giliran].hit_lama2 = konter.t_konter[giliran].hit2; 
	#endif
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

void set_konter_onoff(int i, int onoff) {
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

void data_frek_rpm() {
	//printf("%s() masuk ...\r\n", __FUNCTION__);
	unsigned int i;
	float temp_f;
	float temp_rpm;
	
	struct t_env *env2;
	env2 = (char *) ALMT_ENV;
	
	for (i=0; i<KANALNYA; i++)	{
		if (env2->kalib[i].status==0) 				// RPM
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
			//rtcRate[i] = (int) konter.t_konter[i].hit;
			*(&MEM_RTC0+i) = data_f[(i*2)+1];
		}
		else if (env2->kalib[i].status==1) {		// OnOff
			data_f[i*2] = (float) konter.t_konter[i].onoff;
		#ifdef PAKAI_PILIHAN_FLOW
		} else if (env2->kalib[i].status==3) {		// selector_flow
			data_f[i*2]   = (float) konter.t_konter[i].onoff;
			data_f[i*2+1] = (float) konter.t_konter[i].hit;
			
			*(&MEM_RTC0+(i*2))	 = data_f[i*2];		// konter.t_konter[i].onoff;
			*(&MEM_RTC0+(i*2+1)) = data_f[i*2+1];	// konter.t_konter[i].hit;
			
			//printf("   %d : %d, %d : %d,", (i*2), *(&MEM_RTC0+(i*2)), (i*2+1), *(&MEM_RTC0+(i*2+1)));
		} else if (env2->kalib[i].status==100) {		// flow_setelah_selector
			data_f[i*2]   = (float) konter.t_konter[i].hit;
			data_f[i*2+1] = (float) konter.t_konter[i].hit2;
			
			*(&MEM_RTC0+(i*2))	 = data_f[i*2];			// konter.t_konter[i].hit;
			*(&MEM_RTC0+(i*2+1)) = data_f[i*2+1];		// konter.t_konter[i].hit2;
			//printf("   %d : %d, %d : %d", (i*2), *(&MEM_RTC0+(i*2)), (i*2+1), *(&MEM_RTC0+(i*2+1)));
		} else if (env2->kalib[i].status==4) {		// selector_flow
			data_f[i*2]   = (float) konter.t_konter[i].onoff;
			data_f[i*2+1] = (float) konter.t_konter[i].hit;
			
			*(&MEM_RTC0+(i*2))	 = data_f[i*2];		// konter.t_konter[i].onoff;
			*(&MEM_RTC0+(i*2+1)) = data_f[i*2+1];	// konter.t_konter[i].hit;
		} else if (env2->kalib[i].status==201) {		// selector_flow
			data_f[i*2]   = (float) konter.t_konter[i].onoff;
			data_f[i*2+1] = (float) konter.t_konter[i].hit;
			
			*(&MEM_RTC0+(i*2))	 = data_f[i*2];		// konter.t_konter[i].onoff;
			*(&MEM_RTC0+(i*2+1)) = data_f[i*2+1];	// konter.t_konter[i].hit;
		} else if (env2->kalib[i].status==202) {		// flow_setelah_selector
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
			*(&MEM_RTC0+(i*2))	 = konter.t_konter[i].hit;		// konter.t_konter[i].hit;
			*(&MEM_RTC0+(i*2+1)) = konter.t_konter[i].hit2;		// konter.t_konter[i].hit2;
		} else if (env2->kalib[i].status==203) {		// flow_setelah_selector
			data_f[i*2]   = (float) konter.global_hit;
			data_f[i*2+1] = (float) konter.ovflow;
			
			*(&MEM_RTC0+(i*2))	 = konter.global_hit;
			*(&MEM_RTC0+(i*2+1)) = konter.ovflow;
		#endif
		}
		//printf("\r\n");
	}
}

void cek_rpm()	{
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

				printf(" %2d : F = %6.2f Hz, rpm = %7.2f, hit = %8.0f\r\n", \
					(i+1), temp_f, data_f[(i*2)], data_f[i*2+1]);
			}
		} else if (penv->kalib[i].status==1) {		// onoff
			#if 0
				#ifndef BOARD_KOMON_KONTER_3_1
				if (i>6) 
				#endif
			#endif
			{
				printf(" %2d : Status: %3d [%s], Nilai: %d : %s\r\n", \
					(i+1), status_konter[i], (status_konter[i]==1)?"OnOff":"ERROR1", \
						konter.t_konter[i].onoff, (konter.t_konter[i].onoff==1)?"ON":"OFF");
			}
		#ifdef PAKAI_PUSHBUTTON
		} else if (penv->kalib[i].status==2) {		// pushbutton
			#if 0
				#ifndef BOARD_KOMON_KONTER_3_1
				if (i>6) 
				#endif
			#endif
			{
				printf(" %2d : Status: %d [%s], Nilai: %d : %s\r\n", \
					(i+1), status_konter[i], (status_konter[i]==2)?"PushButton":"ERROR2", konter.t_konter[i].onoff, (konter.t_konter[i].onoff==1)?"on":"off");
			}
		#endif
		#ifdef PAKAI_PILIHAN_FLOW
		} else if (penv->kalib[i].status==3) {		// selector_flow
			{
				printf(" %2d : Status: %3d [%s], Nilai: %d : %s, x: %d\r\n", \
					(i+1), status_konter[i], (status_konter[i]==3)?"Selector flow":"ERROR3", \
						konter.t_konter[i].onoff, (konter.t_konter[i].onoff==1)?"on":"off", konter.t_konter[i].hit);
			}
		} else if (penv->kalib[i].status==100) {		// flow_setelah_selector
			{
				printf(" %2d : Status: %3d [%s],  Nilai %d: %8.0f, Nilai %d : %8.0f\r\n", \
					(i+1), status_konter[i], (status_konter[i]==100)?"Flow":"ERROR100", \
						(i*2+1), /*konter.t_konter[i].hit*/data_f[i*2], (i*2+2), data_f[i*2+1]);
			}
		} else if (penv->kalib[i].status==4) {		// selector_flow
			{
				printf(" %2d : Status: %3d [%s], Nilai: %d : %s, x: %d\r\n", \
					(i+1), status_konter[i], (status_konter[i]==4)?"Selector flow2":"ERROR4", \
						konter.t_konter[i].onoff, (konter.t_konter[i].onoff==1)?"on":"off", konter.t_konter[i].hit);
			}
		} else if (penv->kalib[i].status==201) {	// selector_flow
			{
				printf(" %2d : Status: %3d [%s], Nilai: %d : %s, x: %d\r\n", \
					(i+1), status_konter[i], (status_konter[i]==201)?"Selector flow2":"ERROR4", \
						konter.t_konter[i].onoff, (konter.t_konter[i].onoff==1)?"on":"off", konter.t_konter[i].hit);
			}
		} else if (penv->kalib[i].status==202) {	// flow_setelah_selector
			{
				printf(" %2d : Status: %3d [%s],  Nilai %d: %8.0f, Nilai %d : %8.0f\r\n", \
					(i+1), status_konter[i], (status_konter[i]==202)?"Flow":"ERROR4", \
						(i*2+1), /*konter.t_konter[i].hit*/data_f[i*2], (i*2+2), data_f[i*2+1]);
			}
		} else if (penv->kalib[i].status==203) {	// flow_setelah_selector
			{
				printf(" %2d : Status: %3d [%s],  global : %d\r\n", \
					(i+1), status_konter[i], (status_konter[i]==203)?"Global":"ERROR4", konter.global_hit);
			}
		#endif
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
