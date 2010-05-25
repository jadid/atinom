
#include "FreeRTOS.h"
#include "task.h"
#include "../monita/monita_uip.h"
#include "../GPIO/selenoid.c"

#include <stdio.h>
#include <stdlib.h>

//extern float data_f [ (JML_SUMBER * PER_SUMBER) ];
extern xTaskHandle hdl_relay;

void cek_alarm_relay(no_sumber);
#define nALARM 8
char fAlarm[nALARM];

portTASK_FUNCTION( relay_task, pvParameters ) {
	int no_nya=0;
	char *p, c;

	init_selenoid();
	vTaskDelay(1000);
	vTaskDelay(1000);
	

	//printf("serial default\r\n");
	//ser2_putstring("pake ser 2\r\n");
	//serX_putstring(2, "pake PM di serial \r\n");
	//printf("sesudahnya\r\n");
	//c = 'A';	p = &c;
	//serX_putchar(2, &c, 1000);
	//serX_putstring(2, "pake serial 2\r\n");
	
	reset_flag_alarm();
	for (;;)
	{
		vTaskDelay(50);
		reset_flag_alarm();
		cek_alarm_relay(no_nya);
		no_nya++;
		if (no_nya>JML_SUMBER) no_nya=0;
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
			
			//printf("%s data: %.2f, alarmL: %.2f, alarmH: %.2f\r\n", \ 
			//	 p_dt[index].nama, data_f[index], p_dt[index].alarm_L, p_dt[index].alarm_H);
			
			if ((data_f[index]>p_dt[index].alarm_HH) ) {
				set_selenoid(p_dt[index].relay);
				fAlarm[p_dt[index].relay] = 1;
			}
			
			if ( (fAlarm[p_dt[index].relay] != 1) && ((data_f[index]<p_dt[index].alarm_H) ) ) {
				unset_selenoid(p_dt[index].relay);
				fAlarm[p_dt[index].relay] = 0;
			}
			
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
		}
	}
	//*/
}
