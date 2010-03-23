
#include "FreeRTOS.h"
#include "task.h"
#include "../monita/monita_uip.h"
#include "../GPIO/selenoid.c"


extern float data_f [ (JML_SUMBER * PER_SUMBER) ];
extern xTaskHandle hdl_relay;

void cek_alarm_relay(no_sumber);

portTASK_FUNCTION( relay_task, pvParameters ) {
	int no_nya=0;
	init_selenoid();
	vTaskDelay(1000);
	vTaskDelay(1000);
	for (;;)
	{
		vTaskDelay(50);
		cek_alarm_relay(no_nya);
		no_nya++;
		if (no_nya>JML_SUMBER) no_nya=0;
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
			if ((data_f[index]<p_dt[index].alarm_L) || (data_f[index]>p_dt[index].alarm_H) ) {
				set_selenoid(p_dt[index].relay);
			} else {
				unset_selenoid(p_dt[index].relay);
			}
		}
	}
	//*/
}
