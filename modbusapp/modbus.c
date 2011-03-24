
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

xTaskHandle hdl_modbus;

static portTASK_FUNCTION(task_modbus, pvParameters )
{
	
	for (;;)
	{
		//togle_led_utama();
		vTaskDelay(500);
	}
}
void init_task_modbus(void) {
	xTaskCreate(task_modbus, ( signed portCHAR * ) "modbus",  (configMINIMAL_STACK_SIZE * 20) ,\
		 NULL, tskIDLE_PRIORITY - 2, ( xTaskHandle * ) &hdl_modbus );
}
