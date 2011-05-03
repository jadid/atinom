/*
 *
 * NMEA library
 * URL: http://nmea.sourceforge.net
 * Author: Tim (xtimor@gmail.com)
 * Licence: http://www.gnu.org/licenses/lgpl.html
 * $Id: info.c 17 2008-03-11 11:56:11Z xtimor $
 *
 */

#include <string.h>
#include "nmea/allinnmea.h"
#include "nmea/info.h"

#include <stdarg.h>
#include <stdio.h>

void nmea_error(const char *str, ...)
{
    int size;
    va_list arg_list;
    char buff[NMEA_DEF_PARSEBUFF];
    nmeaErrorFunc func = nmea_property()->error_func;

    if(func)
    {
        va_start(arg_list, str);
        size = NMEA_POSIX(vsnprintf)(&buff[0], NMEA_DEF_PARSEBUFF - 1, str, arg_list);
        va_end(arg_list);

        if(size > 0)
            (*func)(&buff[0], size);
    }
}

int nmea_parser_init(nmeaPARSER *parser)
{
    int resv = 0;
    int buff_size = nmea_property()->parse_buff_size;

    NMEA_ASSERT(parser);

    if(buff_size < NMEA_MIN_PARSEBUFF)
        buff_size = NMEA_MIN_PARSEBUFF;

    memset(parser, 0, sizeof(nmeaPARSER));

    if(0 == (parser->buffer = malloc(buff_size)))
        nmea_error("Insufficient memory!");
    else
    {
        parser->buff_size = buff_size;
        resv = 1;
    }    

    return resv;
}
