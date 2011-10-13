/*
	berkaiatan dengan sumber ip address 
	afrendy bayu
	daun biru engineering

	24 Sept 2010
	Setting data id via webserver
//*/

#ifndef __MEM_RTC__
#define __MEM_RTC__

#include "FreeRTOS.h"
#include "hardware.h"
#include "../app/monita/monita_uip.h"
#include "enviro.h"

#ifdef PAKAI_MEM_RTC

extern float data_f[];
extern struct t2_konter konter;

#ifdef TES_MEM_RTC
void tes_mem_rtc(int argc, char **argv)	{
	int awalsaja=0;
	
	if (argc>2 || argc==1) {
		printf("Perintah: \r\n");
		printf("  set_dimmer [%dimmer]\r\n");
		printf("  contoh: set_dimmer 50\r\n");
		return 0;
	}
	
	sscanf(argv[1], "%d", &awalsaja);
	MEM_RTC0 = awalsaja;
	MEM_RTC1 = awalsaja+10;
	printf("  MEMRTC0: %d, MEM_RTC1: %d\r\n", MEM_RTC0, MEM_RTC1);
}

static tinysh_cmd_t tes_mem_cmd={0,"tes_mem","set memori RTC","", tes_mem_rtc,0,0,0};
#endif



void set_mem_rtc(int argc, char **argv)	{
	
	if (argc>3) {
		printf("Perintah: \r\n");
		printf("  set_mem [kanal] [nilai]\r\n");
		printf("  contoh: set_mem 3 53443\r\n");
		return 0;
	} else if (argc==2) {
		if (strcmp(argv[1], "reset") == 0) {
			reset_konter();
		}

		return 0;
	}
	int kanalnya=0, nilainya=0;
	
	sscanf(argv[1], "%d", &kanalnya);
	sscanf(argv[2], "%d", &nilainya);
	
	//printf("kanal: %d, nilai: %d\r\n", kanalnya, nilainya);
	//printf("Pulsa ke-%d: %.1f -->", kanalnya, data_f[kanalnya*2-1]);
	
	konter.t_konter[kanalnya-1].hit = *(&MEM_RTC0+kanalnya-1) = (int) nilainya;

	//printf("  %x: hasil[%d]: %d <-- %d\r\n", (&MEM_RTC0+kanalnya-1), kanalnya, *(&MEM_RTC0+kanalnya-1), (int) nilainya );
	//printf("Pulsa ke-%d: %.1f -->\r\n", kanalnya, data_f[kanalnya*2-1]);
}

static tinysh_cmd_t set_mem_cmd={0,"set_mem","set memori RTC","", set_mem_rtc,0,0,0};




#endif
#endif
