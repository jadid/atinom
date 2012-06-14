/*
	Afrendy Bayu
	7 Juni 2012

*/



#include "FreeRTOS.h"

#ifndef __SHELL_SERIAL__
#define __SHELL_SERIAL__

#ifdef PAKAI_SHELL

void shell_kitab(char *x)	{
	printf("  %s [alamat] [register] [jml_data]\r\n", x);
	printf("    contoh : %s 5 100 2\r\n", x);
	printf("    %s alamat slave 5 pada register 100 sejumlah 2 byte\r\n", x);
}

void init_shell_sh(int argc, char **argv)	{
	xSerialPortInitMinimal( BAUD_RATE_SHELL, configMINIMAL_STACK_SIZE  );
}

static tinysh_cmd_t init_shell_cmd={0,"init_shell","init shell","", init_shell_sh,0,0,0};


#endif

#endif
