/*
	berkaiatan dengan sumber ip address 
	afrendy bayu
	daun biru engineering

	24 Sept 2010
	Setting data id via webserver
//*/

#ifndef __SHELL_DIMMER__
#define __SHELL_DIMMER__

#include "FreeRTOS.h"
#include "pwm_dimmer/pwmdimmer.c"

#ifdef PAKAI_GPIO_DIMMER

extern int persen_pwm;

void cek_dimmer(int argc, char **argv)	{
	printf("  Persen dimmer: %d\r\n", persen_pwm);
}

void set_dimmer(int argc, char **argv)	{
	if (argc>2) {
		printf("Perintah: \r\n");
		printf("  set_dimmer [%dimmer]\r\n");
		printf("  contoh: set_dimmer 50\r\n");
		return 0;
	}
	
	sscanf(argv[1], "%d", &persen_pwm);
	if (persen_pwm>100) persen_pwm=100;
	if (persen_pwm<0)	persen_pwm=0;
	printf("  Persen dimmer: %d\r\n", persen_pwm);
}

static tinysh_cmd_t set_dimmer_cmd={0,"set_dimmer","set dimmer untuk melihat parameter","", set_dimmer,0,0,0};
static tinysh_cmd_t cek_dimmer_cmd={0,"cek_dimmer","cek dimmer untuk melihat parameter","", cek_dimmer,0,0,0};

#endif
#endif
