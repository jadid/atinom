/*
	berkaiatan dengan sumber ip address 
	afrendy bayu
	daun biru engineering

	24 Sept 2010
	Setting data id via webserver
//*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#ifndef __SHELL_GPS__
#define __SHELL_GPS__


#include "../app/gps/gps.h"

#ifdef PAKAI_GPS

//char sSig[][] = {};


void cek_gps(int argc, char **argv)	{
	extern nmeaINFO infoGPS;
	extern nmeaPOS dposGPS;
	double lintang=0.0, bujur=0.0;
	
	if (argc != 1) {
		printf("Perintah salah!!!\r\n");
		printf("Gunakan : \r\n");
		printf("  cek_gps\r\n");
		return;
	} else {
		judul (" Info GPS\r\n");
		lintang = nmea_radian2degree(dposGPS.lat);
		printf("   Lintang   : %7.3f° %s\r\n", nmea_radian2degree(dposGPS.lat), ((nmea_radian2degree(dposGPS.lat)>0)?"LU":"LS") );
		bujur = nmea_radian2degree(dposGPS.lon);
		printf("   Bujur     : %7.3f° %s\r\n", nmea_radian2degree(dposGPS.lon), ((nmea_radian2degree(dposGPS.lon)>0)?"BT":"BB") );
		printf("   Elevasi   : %7.3f meter\r\n", infoGPS.elv);
		printf("   Deklanasi : %7.3f°\r\n", infoGPS.declination);
		printf("   Arah      : %7.3f° \r\n", infoGPS.direction);
		printf("   Kecepatan : %7.3f km/j\r\n", infoGPS.speed);
		printf("   Tanggal   : %d-%d-%d\r\n", infoGPS.utc.day, infoGPS.utc.mon, infoGPS.utc.year);
		printf("   Waktu     : %d:%02d:%02d %03d\r\n", infoGPS.utc.hour, infoGPS.utc.min, infoGPS.utc.sec, infoGPS.utc.hsec);
		printf("   PDOP      : %7.3f, HDOP: %7.3f, VDOP: %7.3f\r\n", infoGPS.PDOP, infoGPS.HDOP, infoGPS.VDOP);
		printf("   Satelit   : inuse: %d, inview: %d\r\n", infoGPS.satinfo.inuse, infoGPS.satinfo.inview);
		printf("   Signal    : %d, Fix: %d\r\n", infoGPS.sig, infoGPS.fix);
	}
}

static tinysh_cmd_t cek_gps_cmd={0,"cek_gps","cek gps untuk melihat parameter","", cek_gps,0,0,0};

#endif
#endif
