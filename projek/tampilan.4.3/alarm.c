/*
	FreeRTOS.org V5.1.1 - Copyright (C) 2003-2008 Richard Barry.

	30 Jun 2011
	+task Alarm
	
*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "hardware.h"

#include "monita/monita_uip.h"
//#include "LCD/menu_monita.c"



#ifdef PAKAI_ALARM

xTaskHandle hdl_alarm;
unsigned char cek_alarmkan();
unsigned char ACK=0;
unsigned char stAlarmDisplay[100];

extern float data_f[];
extern unsigned char key_index;
extern unsigned char mesin_index;

unsigned char awalPilih=0;

void ack_invert(unsigned char group, unsigned char pilih) {
	//printf("ACK group: %d, pilih: %d\r\n", group, pilih);
	stAlarmDisplay[group*10+pilih] = 0;
}

unsigned char cek_batas_alarm(float nilainya, float batasA, float batasB) {
	unsigned char balik=0;
	if (nilainya < batasB) {
		balik = 1;		//	kabel pelitur lepas = -50
	} else if (nilainya < 0) {
		balik = 2;		//  kabel sensor B lepas = -34
	} else if (nilainya > batasA) {
		balik = 3;		//	kabel sensor A lepas = 261
	} else {
		balik = 0;
	}
	return balik;
}

static portTASK_FUNCTION(alarm_task, pvParameters )	{	
	unsigned char muter=0, dd=0, ja=0;
	unsigned char ii;

	vTaskDelay(400);
	printf("Init Task Alarm .............!!!\r\n");
	vTaskDelay(200);


	for (;;)	{
		muter++;
		ja = cek_alarmkan(1);		

		if (muter>5) {
			muter = 0;
			dd = 1-dd;
			lampukan(dd, ja);
			#if 0
				printf("m index: %d\r\n", mesin_index);
				//printf("dd: %d, ja: %d, ACK: %d, mesin_index: %d\r\n", dd, ja, ACK, mesin_index);
				//printf("dd: %d, ja: %d, ACK: %d\r\n", dd, ja, ACK);
				for (ii=0; ii<10; ii++) {
					printf("%d:%d  ", ii+1, stAlarmDisplay[ii]);
				}
				printf("\r\n");
			#endif
		}
		vTaskDelay(100);
	}
}

void lampukan(unsigned char q, unsigned char w) {
	if (q) {
		if (ACK==2) {
			ACK = 2;
			lampu_merah();
		} else if (ACK==1) {
			if (w==2) {
				ACK = 2;
				lampu_merah();
			} else {
				ACK = 1;
				lampu_kuning();
			}
		} else if (ACK==0) {
			if (w==2) {
				ACK = 2;
				lampu_merah();
			} else if (w==1) {
				ACK = 1;
				lampu_kuning();
			} else {
				ACK = 0;
				lampu_hijau();
			}
		}
	} else {
		lampu_mati();
	}
}

unsigned char cek_alarmkan() {
	unsigned char qwe=0, cek_kon=0;
	
	struct t_dt_set *p_dtq;
	struct t_sumber *p_smbrq;
	struct t_group *p_grpq;
	
	p_dtq   = (char *) ALMT_DT_SET;
	p_smbrq = (char *) ALMT_SUMBER;
	p_grpq  = (char *) ALMT_GROUP;
	
	int ii=0, jj=0, kk=0;
	
	// CEK status alarm untuk per GROUP aktif
	#if 0
	jj=0;
	for (ii=awalPilih; ii<(awalPilih+10); ii++) {		// hanya 10 saja
		//stAlarmDisplay[mesin_index*10+jj]=0;			// direset
		#if 1
		kk = p_grpq[mesin_index].no_data[ii];
		//if (kk) {
		if (p_dtq[kk-1].aktif == 1) {			//
			if (data_f[kk-1]> p_dtq[kk-1].alarm_H) {
				stAlarmDisplay[mesin_index*10+jj]=1;
				//printf("%d: %d   ", jj, kk);
			}
		}
		jj++;
		//}
		
		#endif
	}
	#endif
	
	#if 1
	jj=0;
	// cek tiap group, aktifkan alarm !!
	for (ii=0; ii<10; ii++) {		// hanya 10 saja
		//stAlarmDisplay[mesin_index*10+jj]=0;			// direset
		#if 1
		for (jj=0; jj<10; jj++) {
			kk = p_grpq[ii].no_data[jj];
			if (p_dtq[kk-1].aktif == 1) {
				cek_kon = cek_batas_alarm(data_f[kk-1], p_dtq[kk-1].batas_atas, p_dtq[kk-1].batas_bawah);

				//printf("%d. %-10s : %6.1f, st: %d, A: %.0f, B: %.0f\r\n", \
				//	jj, p_dtq[kk-1].nama, data_f[kk-1], cek_kon, p_dtq[kk-1].batas_atas, p_dtq[kk-1].batas_bawah);
				if ((data_f[kk-1]>p_dtq[kk-1].alarm_H) && !cek_kon) {
					stAlarmDisplay[ii*10+jj]=1;
				//	printf("%d: %d : %d  \r\n", jj, kk, stAlarmDisplay[ii*10+jj]);
				}
			}
		}
		#endif
	}
	#endif
	//printf("\r\n");
	
	//	CEK ALARM untuk semua TITIK aktif
	for (jj=0; jj<JML_SUMBER; jj++) {
		if (p_smbrq[jj].status==1) {
			#ifdef DEBUG_ALARM
				printf("-- sumber %d aktif\r\n", jj);
			#endif
			for (ii=0; ii<PER_SUMBER; ii++) {
				kk = PER_SUMBER*jj+ii;
				if (p_dtq[kk].aktif == 1) {
					cek_kon = cek_batas_alarm(data_f[kk], p_dtq[kk].batas_atas, p_dtq[kk].batas_bawah);
					#ifdef DEBUG_ALARM
					printf("%d. %-10s : %6.1f, st: %d, A: %.0f, B: %.0f\r\n", \
						ii, p_dtq[kk].nama, data_f[kk], cek_kon, p_dtq[kk].batas_atas, p_dtq[kk].batas_bawah);
					#endif
					if ((data_f[kk]>p_dtq[kk].alarm_HH) && !cek_kon){
						qwe = 2;		// 2: merah
						break;
					} else if ((data_f[kk]>p_dtq[kk].alarm_H) && !cek_kon) {
						if (qwe<2)		// < 2 : merah
							qwe = 1;		// 2: kuning, loop, cek lagi !!
					}
					#ifdef DEBUG_ALARM
						printf("---- alarm data %d aktif\r\n", (PER_SUMBER*jj+ii));
					#endif
				}
			}
		}
	}
	//printf("\r\n");
	return qwe;		// 0: hijau;
}

void lampu_mati() {
	FIO1CLR = LED_PICKUP;
	FIO1CLR = BACKLIT;
}

void lampu_hijau() {
	FIO1SET = BACKLIT;
	#ifdef PAKAI_RELAY
	unset_selenoid(1);
	unset_selenoid(2);
	#endif
}

void lampu_merah() {
	FIO1SET = LED_PICKUP;	
	#ifdef PAKAI_RELAY
	set_selenoid(1);
	unset_selenoid(2);
	#endif
}

void lampu_kuning() {
	FIO1SET = LED_PICKUP;
	FIO1SET = BACKLIT;
	#ifdef PAKAI_RELAY
	unset_selenoid(1);
	set_selenoid(2);
	#endif
}

void init_task_alarm(void)
{
	xTaskCreate( alarm_task, ( signed portCHAR * ) "Alarm", (configMINIMAL_STACK_SIZE * 10), \
		NULL, tskIDLE_PRIORITY + 1, (xTaskHandle *) &hdl_alarm );	
}

#endif
