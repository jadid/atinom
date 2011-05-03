
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#ifdef PAKAI_GPS

//#include "gps.h"
#include <nmealib/include/nmea/info.h>
#include <nmealib/include/nmea/parser.h>
#include <nmealib/include/nmea/gmath.h>
#include <nmealib/include/nmea/info.h>

#ifndef __GPS__
#define __GPS__


xTaskHandle hdl_gps;

unsigned char pesan2[128];
int komplet=0;
int nC=0;

portTASK_FUNCTION(gps, pvParameters )	{
  	vTaskDelay(500);
  	//data_gps dgps;

    nmeaINFO info;
    nmeaPARSER parser;
	nmeaPOS dpos;
	
	//nmea_zero_INFO(&info);
    //nmea_parser_init(&parser);
	
  	printf("GPS init !!\r\n");
  	ser2_putstring("Masuk serial 2\r\n");
  	vTaskDelay(500);
  	
  	int len=0;
  	
  	for(;;) {
		vTaskDelay(10);
		
		if (len=serPoll())	{
			//printf("len: %d, isi: %s\r\n", len, pesan2);
		}
		//printf("len: %d\r\n", len);
	}
  	
}

int serPoll(void) {
	unsigned char c;
	if (ser2_getchar(1, &c, 100 ) == pdTRUE)	{
		//xSerialPutChar2(1, c, 50);
		printf("%c",c);
		if (c=='\r' || c=='\n') {
			komplet = nC;
			pesan2[nC] = '\0';
			nC=0;
			ser2_putstring("\n");
			//printf("komplet: %d isi: %s__\r\n", komplet, pesan2);
		} else {
			pesan2[nC] = c;
			komplet = 0;
			nC++;
		}
	}
	return komplet;
}

void init_gps(void)	{
	//xTaskCreate( shell, "UsrTsk1", (configMINIMAL_STACK_SIZE * 6), 
	xTaskCreate( gps, "gps_task", (configMINIMAL_STACK_SIZE * 10), NULL, tskIDLE_PRIORITY, ( xTaskHandle * ) &hdl_gps);
}

#endif

#endif
