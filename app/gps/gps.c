
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#ifdef PAKAI_GPS

#include "gps.h"


#ifndef __GPS__
#define __GPS__

#define DEBUG_GPSs

unsigned char pesanGPS[128];

int komplet=0;
int nC=0;
int jgps=0;
/*
portTASK_FUNCTION(gps, pvParameters )	{
  	vTaskDelay(500);
	
	awal_gps();
  	//ser2_putstring("Masuk serial 2\r\n");
  	vTaskDelay(100);
  	
  	int hasilGPS=0;
  	
  	
  	
  	
  	for(;;) {
		vTaskDelay(2);
		//if (len=serPollGPS())	
		{
			if (jgps<2) {
				hasilGPS = proses_gps();
			
			#ifdef DEBUG_GPSs
				info_gps(hasilGPS);

			
			#endif
			}
		}
		#ifdef DEBUG_GPSs
		vTaskDelay(3000);
		#endif
	}
	deinit_gps();
	printf("akhir GPS !!\r\n");
}
//*/
int proses_gps() {
	int parsenya=0;
	parsenya = nmea_parse(&parserGPS, pesanGPS, (int)strlen(pesanGPS), &infoGPS);
	//printf("serPoll : %d :%3d: %s\r\n", parsenya, (int)strlen(pesanGPS), pesanGPS);
	nmea_info2pos(&infoGPS, &dposGPS);
	//info_gps(parsenya);
	return parsenya;
}

void info_gps(int parsenya) {
	
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
}

int serPollGPS(void) {
	unsigned char c;
	//if (ser2_getchar(1, &c, 100 ) == pdTRUE)	
	if (ser2_getchar(1, &c, 50 ) == pdTRUE)
	{
		//xSerialPutChar2(1, c, 50);
		//printf("%c",c);
		if (c=='\r') {
			//printf("R");
		} else if (c=='\n') {
			//printf("N\r\n");
			//nC++;
			pesanGPS[nC] = '\r';
			nC++;
			pesanGPS[nC] = '\n';
			nC++;
			pesanGPS[nC] = '\0';
			komplet = nC;
			nC=0;
			//ser2_putstring("\n");
			//printf("komplet: %d isi: %s__\r\n", komplet, pesanGPS);
		} else {
			pesanGPS[nC] = c;
			komplet = 0;
			nC++;
		}
	}
	return komplet;
}

void awal_gps() {
	nmea_zero_INFO(&infoGPS);
    nmea_parser_init(&parserGPS);
	
  	printf(" Monita : GPS init !!\r\n");
  	
  	//strcpy(pesanGPS, "$GPRMC,084702.000,A,0612.6782,S,10654.3289,E,0.00,278.92,030511,,,A*72\r\n");
} 

void deinit_gps() {
	nmea_parser_destroy(&parserGPS);
}

/*
void init_gps(void)	{
	xTaskCreate( gps, "gps_task", (configMINIMAL_STACK_SIZE * 10), NULL, tskIDLE_PRIORITY+3, ( xTaskHandle * ) &hdl_gps);
}
//*/
#endif

#endif
