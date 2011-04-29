
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#ifdef PAKAI_GPS

#include "gps.h"

#ifndef __GPS__
#define __GPS__


xTaskHandle hdl_gps;

portTASK_FUNCTION(gps, pvParameters )	{
  	vTaskDelay(500);
  	printf("GPS init !!\r\n");
  	vTaskDelay(500);
  	
  	for(;;) {
		vTaskDelay(1000);
		
	}
  	
}

void init_gps(void)	{
	//xTaskCreate( shell, "UsrTsk1", (configMINIMAL_STACK_SIZE * 6), 
	xTaskCreate( gps, "gps_task", (configMINIMAL_STACK_SIZE * 1), NULL, tskIDLE_PRIORITY, ( xTaskHandle * ) &hdl_gps);
}

#endif

#endif
