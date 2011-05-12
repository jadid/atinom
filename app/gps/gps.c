
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#ifdef PAKAI_GPS

#include "gps.h"


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
	
	nmea_zero_INFO(&info);
    nmea_parser_init(&parser);
	
  	printf(" Monita : GPS init !!\r\n");
  	//ser2_putstring("Masuk serial 2\r\n");
  	vTaskDelay(500);
  	
  	int len=0;
  	int parsenya=0;
  	//strcpy(pesan2, "$GPRMC,084702.000,A,0612.6782,S,10654.3289,E,0.00,278.92,030511,,,A*72\r\n");
  	
  	for(;;) {
		
		vTaskDelay(1000);
		if (len=serPoll())	
		{
			parsenya = nmea_parse(&parser, pesan2, (int)strlen(pesan2), &info);
			printf("serPoll : %d :%3d: %s\r\n", parsenya, (int)strlen(pesan2), pesan2);
			nmea_info2pos(&info, &dpos);
			printf(
				"-----%d------Sig: %d, Fix: %d, lat:%7.3lf lon:%7.3lf, lat:%+10.3lf° lon:%+11.3lf° elv:%.3f\r\n",  \
				parsenya, info.sig, info.fix, dpos.lat, dpos.lon, nmea_radian2degree(dpos.lat), nmea_radian2degree(dpos.lon), info.elv);

			printf(
				"-----%d------speed: %6.3f, arah: %6.3f, %d-%d-%d %d:%02d:%02d %02d \r\n", parsenya, info.speed, info.direction, \
				info.utc.day, info.utc.mon, info.utc.year, info.utc.hour, info.utc.min, info.utc.sec, info.utc.hsec);
		}
	}
	nmea_parser_destroy(&parser);
}

int serPoll(void) {
	unsigned char c;
	if (ser2_getchar(1, &c, 100 ) == pdTRUE)	{
		//xSerialPutChar2(1, c, 50);
		//printf("%c",c);
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
	xTaskCreate( gps, "gps_task", (configMINIMAL_STACK_SIZE * 15), NULL, tskIDLE_PRIORITY+3, ( xTaskHandle * ) &hdl_gps);
}

#endif

#endif
