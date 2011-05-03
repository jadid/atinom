//#include <nmea/nmea.h>
#include <string.h>

#include "allinnmea.h"

int main()
{
    //*
    const char *buff[] = {
        "$GPRMC,102036.000,A,0612.6797,S,10654.3236,E,0.00,269.65,020511,,,A*74\r\n",
        "$GPGGA,102036.000,0612.6797,S,10654.3236,E,1,09,1.0,37.6,M,3.5,M,,0000*43\r\n",
        "$GPGSV,2,1,08,01,05,005,80,02,05,050,80,03,05,095,80,04,05,140,80*7f\r\n",
        "$GPGSV,2,2,08,05,05,185,80,06,05,230,80,07,05,275,80,08,05,320,80*71\r\n",
        "$GPGSA,A,3,23,16,20,03,32,24,11,19,14,,,,1.7,1.0,1.4*3D\r\n",
        "$GPRMC,111609.14,A,5001.27,N,3613.06,E,11.2,0.0,261206,0.0,E*50\r\n",
        "$GPVTG,217.5,T,208.8,M,000.00,N,000.01,K*4C\r\n"
    };

    int it;
    nmeaINFO info;
    nmeaPARSER parser;
	nmeaPOS dpos;
	int parsenya=0;
	
    nmea_zero_INFO(&info);
    nmea_parser_init(&parser);

    for(it = 0; it < 6; ++it) {
        printf("isi: %3d: %s", (int)strlen(buff[it]), buff[it]);
        parsenya = nmea_parse(&parser, buff[it], (int)strlen(buff[it]), &info);
        nmea_info2pos(&info, &dpos);

        printf(
            "--%d---------%03d, Lat: %f, Lon: %f, Sig: %d, Fix: %d\n",
            parsenya, it, dpos.lat, dpos.lon, info.sig, info.fix
            );
        printf(
            "-----------P%d in radians: lat:%9.6lf lon:%9.6lf  \tin degree: lat:%+010.6lf° lon:%+011.6lf°\n", 
            it, dpos.lat, dpos.lon, nmea_radian2degree(dpos.lat), nmea_radian2degree(dpos.lon)
            );
		//nmea_parse(&parser, &buff[0], size, &info);
    }
    nmea_parser_destroy(&parser);

    return 0;
}
