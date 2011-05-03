#include <nmea/nmea.h>
#include <string.h>
#include <stdio.h>

#define NUM_POINTS 1

int main()
{
    const char *buff[] = {
        "$GPRMC,102041.000,A,0612.6797,S,10654.3236,E,0.00,269.65,020511,,,A*74\r\n"
    };

    nmeaPOS pos[NUM_POINTS], pos_moved[NUM_POINTS][2];
    double dist[NUM_POINTS][2]; 
    double azimuth[NUM_POINTS][2], azimuth_moved[NUM_POINTS];
    int result[2];
    int it = 0;

    nmeaPARSER parser;
    nmea_parser_init(&parser);

    for(it = 0; it < NUM_POINTS; ++it)
    {
        int result;
        nmeaINFO info;
        nmea_zero_INFO(&info);
        result = nmea_parse(&parser, buff[it], (int)strlen(buff[it]), &info);        
        nmea_info2pos(&info, &pos[it]);
    }

    nmea_parser_destroy(&parser);

    for(it = 0; it < NUM_POINTS; ++it)
    {
        dist[it][0] = nmea_distance(&pos[0], &pos[it]);
        dist[it][1] = nmea_distance_ellipsoid(
            &pos[0], &pos[it], &azimuth[it][0], &azimuth[it][1]
            );
    }

    for(it = 0; it < NUM_POINTS; ++it)
    {
        result[0] = nmea_move_horz(&pos[0], &pos_moved[it][0], azimuth[it][0], dist[it][0]);
        result[1] = nmea_move_horz_ellipsoid(
            &pos[0], &pos_moved[it][1], azimuth[it][0], dist[it][0], &azimuth_moved[it]
            );

    }

    /* Output of results */
    printf("Coordinate points:\n");
    for(it = 0; it < NUM_POINTS; ++it)
    {
        printf(
            "P%d in radians: lat:%9.6lf lon:%9.6lf  \tin degree: lat:%+010.6lf° lon:%+011.6lf°\n", 
            it, pos[it].lat, pos[it].lon, nmea_radian2degree(pos[it].lat), nmea_radian2degree(pos[it].lon)
            );
    }

    printf("\nCalculation results:\n");
    for(it = 0; it < NUM_POINTS; ++it)
    {
        printf("\n");
        printf("Distance P0 to P%d\ton spheroid:  %14.3lf m\n", it, dist[it][0]);
        printf("Distance P0 to P%d\ton ellipsoid: %14.3lf m\n", it, dist[it][1]);
        printf("Azimuth  P0 to P%d\tat start: %8.3lf°\tat end: %8.3lf°\n", it, nmea_radian2degree(azimuth[it][0]), nmea_radian2degree(azimuth[it][1]));
        printf("Move     P0 to P%d\t         \tAzimuth at end: %8.3lf°\n", it, nmea_radian2degree(azimuth_moved[it]));
        printf("Move     P0 to P%d\ton spheroid:  %3s lat:%+010.6lf° lon:%+011.6lf°\n", it, result[0] == 1 ? "OK" : "nOK", nmea_radian2degree(pos_moved[it][0].lat), nmea_radian2degree(pos_moved[it][0].lon));
        printf("Move     P0 to P%d\ton ellipsoid: %3s lat:%+010.6lf° lon:%+011.6lf°\n", it, result[0] == 1 ? "OK" : "nOK", nmea_radian2degree(pos_moved[it][1].lat), nmea_radian2degree(pos_moved[it][1].lon));
        printf("Move     P0 to P%d\toriginal:         lat:%+010.6lf° lon:%+011.6lf°\n", it, nmea_radian2degree(pos[it].lat), nmea_radian2degree(pos[it].lon));
    }

    return 0;
}
