#include <nmea/nmea.h>
#include <stdio.h>


#include <unistd.h>

int main()
{
    nmeaGENERATOR *gen;		// def struct @generator.h
    nmeaINFO info;			// def struct @info.h
    char buff[2048];
    int gen_sz;
    int it;

    nmea_zero_INFO(&info);	// f @allinnmea.c, @info.c

    if(0 == (gen = nmea_create_generator(NMEA_GEN_ROTATE, &info)))
        return -1;

    for(it = 0; it < 10; ++it)
    {
        gen_sz = nmea_generate_from(
            &buff[0], 2048, &info, gen,
            GPGGA | GPGSA | GPGSV | GPRMC | GPVTG
            );

        buff[gen_sz] = 0;
        printf("%d: \r\n%s\n", it, &buff[0]);

#ifdef NMEA_WIN
        Sleep(500);
#else
        usleep(500000);        
#endif
    }

    nmea_gen_destroy(gen);
	printf("main generator ....setelh nmea_gen_destroy...\r\n");
    return 0;
}
