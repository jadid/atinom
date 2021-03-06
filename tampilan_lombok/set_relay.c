
#include "FreeRTOS.h"
#include "task.h"
#include "../monita/monita_uip.h"
//#include "../GPIO/selenoid.c"

#include <stdio.h>
#include <stdlib.h>

#ifdef PAKAI_SELENOID

extern float data_f [ (JML_SUMBER * PER_SUMBER)+JML_RELAY ];		// variable ini nyimpan semua data modul monita 
extern xTaskHandle hdl_relay;

void cek_alarm_relay(no_sumber);
#define nALARM 8
char fAlarm[nALARM];

portTASK_FUNCTION( relay_task, pvParameters ) {
	int no_nya=0;
	char *p, c;
	int loopnya=0;

	init_selenoid();
	vTaskDelay(1000);
	vTaskDelay(1000);

//*
	reset_flag_alarm();
	for (;;)
	{
		vTaskDelay(200);
		//reset_flag_alarm();
		cek_alarm_relay(no_nya);
		no_nya++;
		if (no_nya>JML_SUMBER) no_nya=0;
		
		//*
		if(loopnya>50) {
			//printf("jam sekarang: %d", jam_sekarang());
			//cek_penduduk();
			//printf("Solar volt: %f, Solar cur: %f, relay1: %f\r\n", data_f[26-1], data_f[28-1], data_f[PER_SUMBER*JML_SUMBER]);
			loopnya=0;
		}
		//*/
		//cek_penduduk();
		
		loopnya++;
	}
}

void reset_flag_alarm() {
	int k=0;
	for (k=0; k<nALARM; k++) {
		fAlarm[k] = 0;
	}
}

void init_task_relay(void) {
	xTaskCreate( relay_task, ( signed portCHAR * ) "Relay", (configMINIMAL_STACK_SIZE * 2), \
		NULL, tskIDLE_PRIORITY + 1, (xTaskHandle *) &hdl_relay );	
}

void cek_alarm_relay(no_sumber) {
	int i=0, j=0, index=0;

	struct t_dt_set *p_dt;
	p_dt = (char *) ALMT_DT_SET;

	for (j=0; j<PER_SUMBER; j++) {
		index = JML_SUMBER*no_sumber+j;
		if (p_dt[index].aktif == 1) {
			
			//printf("%d. %s data: %.2f, alarmH: %.2f, alarmHH: %.2f, falarm: %d\r\n", \ 
			//	 index, p_dt[index].nama, data_f[index], p_dt[index].alarm_H, p_dt[index].alarm_HH, p_dt[index].relay);
			/*
			if ((data_f[index]>p_dt[index].alarm_HH) ) {
				set_selenoid(p_dt[index].relay);
				//printf("__%d___%d\r\n", (PER_SUMBER*JML_SUMBER)+p_dt[index].relay-2, data_f[(PER_SUMBER*JML_SUMBER)+p_dt[index].relay-2]);
				fAlarm[p_dt[index].relay] = 0;
				vTaskDelay(1000);
			}
			
			if ( (fAlarm[p_dt[index].relay] != 1) && ((data_f[index]<p_dt[index].alarm_H) ) ) {
				unset_selenoid(p_dt[index].relay);
				//printf("__%d___%d\r\n", (PER_SUMBER*JML_SUMBER)+p_dt[index].relay-2, data_f[(PER_SUMBER*JML_SUMBER)+p_dt[index].relay-2]);
				fAlarm[p_dt[index].relay] = 1;
				vTaskDelay(1000);
			}
			//*/
			/*
			if ((data_f[index]<p_dt[index].alarm_L) || (data_f[index]>p_dt[index].alarm_H) ) {
				set_selenoid(p_dt[index].relay);
				fAlarm[p_dt[index].relay] = 1;
			}
			if (fAlarm[p_dt[index].relay] != 1) {
				unset_selenoid(p_dt[index].relay);
				fAlarm[p_dt[index].relay] = 0;
			}
			//*/
			if (index==1) {		// Sensor Aki
				if (fAlarm[p_dt[index].relay] != 1 && data_f[index]<p_dt[index].alarm_H) {		// charger Aki nyala
					printf("Aki dicharge, nyambung !! Aki: %f, batas: %f\r\n", data_f[index], p_dt[index].alarm_H);
					set_selenoid(p_dt[index].relay);
					fAlarm[p_dt[index].relay] = 1;
					vTaskDelay(1000);
				}

				if (fAlarm[p_dt[index].relay] != 0 && data_f[index]>p_dt[index].alarm_HH) {		// charger Aki mati
					printf("Aki penuh .......lepaskan Aki: %f, batas: %f\r\n", data_f[index], p_dt[index].alarm_H);
					unset_selenoid(p_dt[index].relay);
					fAlarm[p_dt[index].relay] = 0;
					vTaskDelay(1000);
				}
			}

			if (index==41) {	// Listrik Turbin
				if (fAlarm[p_dt[index].relay] != 1 && data_f[index]<p_dt[index].alarm_H) {		// Listrik ngedrop
					printf("Listrik ngedrop %f, lepas !!, batas %f\r\n", data_f[index], p_dt[index].alarm_H);
					set_selenoid(p_dt[index].relay);
					fAlarm[p_dt[index].relay] = 1;
					vTaskDelay(1000);
				}
				
				if (fAlarm[p_dt[index].relay] != 0 && data_f[index]>p_dt[index].alarm_HH) {		// Listrik Mantap
					printf("Listrik baik, pakai saja !! Listrik %f, batas %f\r\n", data_f[index], p_dt[index].alarm_H);
					unset_selenoid(p_dt[index].relay);
					fAlarm[p_dt[index].relay] = 0;
					vTaskDelay(1000);
				}
			}
						
		}
	}
	//*/
}

int jam_sekarang() {
	time_t timeval;
	struct tm timeinfo;
	get_tm_time( &timeinfo );
	return timeinfo.tm_hour;	
}


// fuel cell jalan ketika malam, mem-back-up batere cadangan
// fuel cell menggantikan solar cell yg bekerja pada siang
#if 0
void cek_penduduk() {
	int jam = jam_sekarang();
	int teg_batere = (int) data_f[25];		// solar_volt
	int i=0;
	
	if(i>20) {
		printf("jam: %d, teg_batere: %d\r\n", jam, teg_batere);
		i=0;
	}
	i++;
	
	if (jam>18 || jam<5) 
	{
		// penduduk hidup
		if (teg_batere>125) {
			set_selenoid(1);		// listrik penduduk hidup, relay1 NO - normally open - BUKA
			unset_selenoid(7);		// fuel cell mati, relay1 NC - normally close - NYAMBUNG
		} else if (teg_batere<120) {
			//printf("_____________________________ masuk sini\r\n");
			unset_selenoid(1);		// listrik penduduk mati
			set_selenoid(7);		// fuel cell hidup
		}
		//printf("jam: %d, volt: %f, %d, %f, relay1: %f\r\n", jam, data_f[25], (int) data_f[25], data_f[27], data_f[PER_SUMBER*JML_SUMBER]);
	} 
	else {		// diluar jam / sistem kontrol masalah
		// set default setting
		unset_selenoid(1);	// listrik penduduk mati
		unset_selenoid(7);	// fuel cell mati
	}
	
}
#endif


#endif
