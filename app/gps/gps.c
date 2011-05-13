
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#ifdef PAKAI_GPS

#include "gps.h"


#ifndef __GPS__
#define __GPS__

#define DEBUG_GPSs

xTaskHandle hdl_gps;

unsigned char pesanGPS[128];
int komplet=0;
int nC=0;
int jgps=0;

portTASK_FUNCTION(gps, pvParameters )	{
  	vTaskDelay(500);
	
	nmea_zero_INFO(&infoGPS);
    nmea_parser_init(&parserGPS);
	
  	printf(" Monita : GPS init !!\r\n");
  	//ser2_putstring("Masuk serial 2\r\n");
  	vTaskDelay(100);
  	
  	int len=0;
  	int parsenya=0;
  	
  	strcpy(pesanGPS, "$GPRMC,084702.000,A,0612.6782,S,10654.3289,E,0.00,278.92,030511,,,A*72\r\n");
  	
  	for(;;) {
		vTaskDelay(2);
		//if (len=serPollGPS())	
		{
			if (jgps<5) {
			parsenya = nmea_parse(&parserGPS, pesanGPS, (int)strlen(pesanGPS), &infoGPS);
			//printf("serPoll : %d :%3d: %s\r\n", parsenya, (int)strlen(pesan2), pesan2);
			nmea_info2pos(&infoGPS, &dposGPS);
			
			#ifdef DEBUG_GPSs
			
			printf(
				"-----%d------Sig: %d, Fix: %d, lat:%7.3lf lon:%7.3lf, lat:%+10.3lf° lon:%+11.3lf° elv:%.3f\r\n",  \
				parsenya, infoGPS.sig, infoGPS.fix, dposGPS.lat, dposGPS.lon, nmea_radian2degree(dposGPS.lat), \
				nmea_radian2degree(dposGPS.lon), infoGPS.elv);

			printf(
				"-----%d------speed: %6.3f, arah: %6.3f, %d-%d-%d %d:%02d:%02d %03d \r\n", parsenya, infoGPS.speed, infoGPS.direction, \
				infoGPS.utc.day, infoGPS.utc.mon, infoGPS.utc.year, infoGPS.utc.hour, infoGPS.utc.min, infoGPS.utc.sec, infoGPS.utc.hsec);
			printf(
				"-----%d------satelit: inuse: %d, inview: %d\r\n", 
				infoGPS.satinfo.inuse, infoGPS.satinfo.inview);
			
			#endif
			}
			jgps++;
		}
		#ifdef DEBUG_GPSs
		vTaskDelay(3000);
		#endif
	}
	nmea_parser_destroy(&parserGPS);
	printf("akhir GPS !!\r\n");
}

int serPollGPS(void) {
	unsigned char c;
	if (ser2_getchar(1, &c, 100 ) == pdTRUE)	{
		//xSerialPutChar2(1, c, 50);
		//printf("%c",c);
		if (c=='\r' || c=='\n') {
			komplet = nC;
			pesanGPS[nC] = '\0';
			nC=0;
			//ser2_putstring("\n");
			//printf("komplet: %d isi: %s__\r\n", komplet, pesan2);
		} else {
			pesanGPS[nC] = c;
			komplet = 0;
			nC++;
		}
	}
	return komplet;
}

void init_gps(void)	{
	xTaskCreate( gps, "gps_task", (configMINIMAL_STACK_SIZE * 10), NULL, tskIDLE_PRIORITY+3, ( xTaskHandle * ) &hdl_gps);
}

#endif

#endif
