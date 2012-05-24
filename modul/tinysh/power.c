/*
	Afrendy Bayu
	23 Mei 2012

*/

// LM7805 + LM2575 16.1 mA (tanpa macjack)
// sleep mode : 
// powerdown mode : 45.3 mA (ketika blink mati)
// 45.2 mA --> batere RTC dicopot


#include "FreeRTOS.h"

#ifndef __SHELL_MODE_POWER__
#define __SHELL_MODE_POWER__

#ifdef PAKAI_MODE_POWER

void modepower_kitab(char *x)	{
	printf("  %s [alamat] [register] [jml_data]\r\n", x);
	printf("    contoh : %s 5 100 2\r\n", x);
	printf("    %s alamat slave 5 pada register 100 sejumlah 2 byte\r\n", x);
}

int cek_modepower_sh(int argc, char **argv)	{
	printf("  Cek mode Power\r\n");
	printf("  mode : 0x%02x\r\n", PCON);
	//cek_coil(almx, regx, jmlx);
}


static tinysh_cmd_t cek_power_cmd={0,"cek_power","menampilkan status power modul","", cek_modepower_sh,0,0,0};

void set_modepower_sh(int argc, char **argv) {
	if ( (argc<2) || (argc>2) )	{	// cek_relay
		modepower_kitab(argv[0]);
		return 0;
	}

	printf("  Set mode power\r\n");
	vTaskDelay(1000);
	//sleep_mode();
	powerdown_mode();

}

static tinysh_cmd_t set_power_cmd={0,"set_power","set mode power", "help default ",set_modepower_sh,0,0,0};

#endif

#endif
