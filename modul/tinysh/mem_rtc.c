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

//#define TES_MEM_RTC

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


void reset_mem_rtc()	{
	unsigned char ii;
	for (ii=0; ii<40; ii++)	{
		*(&MEM_RTC0+ii) = 0;
	}
	#ifdef BOARD_KOMON_KONTER
	for (ii=0; ii<KANALNYA; ii++)	{
		#ifdef PAKAI_PILIHAN_FLOW
			konter.t_konter[ii].hit  = 0;
			konter.t_konter[ii].hit2 = 0;
			konter.t_konter[ii].onoff = 0;
		#endif
	}
	#endif
}

void cek_nilai_rtc()	{
	int i, q=1;
  	for (i=0; i<10; i++)	{
		q=1-q;
		if (q)		// ganjil
			printf("   %2d F1 : %4d,     F2 : %4d\r\n", i+1, *(&MEM_RTC0+(i*2)), *(&MEM_RTC0+(i*2)+1));
		else 		// genap
			printf("   %2d St : %4d,    Hit : %4d\r\n", i+1, *(&MEM_RTC0+(i*2)), *(&MEM_RTC0+(i*2)+1));

		/*
		printf("   %2d : %4d,    %2d: %4d,   %2d : %4d,    %2d : %4d\r\n", \
			loopambil*4, *(&MEM_RTC0+(loopambil*4)), (loopambil*4+1), \
			*(&MEM_RTC0+(loopambil*4)+1), (loopambil*4+2), *(&MEM_RTC0+(loopambil*4)+2),\
			(loopambil*4+3), *(&MEM_RTC0+(loopambil*4)+3) );
		//*/
	}
}

static tinysh_cmd_t reg_rtc_cmd={0,"reg_rtc","cek semua nilai memori RTC","", cek_nilai_rtc,0,0,0};

void set_mem_rtc(int argc, char **argv)	{
	
	if (argc>3) {
		printf("Perintah: \r\n");
		printf("  set_mem [kanal] [nilai]\r\n");
		printf("  contoh: set_rtc 3 53443\r\n");
		return 0;
	} else if (argc==2) {
		if (strcmp(argv[1], "reset") == 0) {
			reset_mem_rtc();
		}

		return 0;
	}
	int kanalnya=0, nilainya=0;
	
	sscanf(argv[1], "%d", &kanalnya);
	sscanf(argv[2], "%d", &nilainya);
	
	//printf("kanal: %d, nilai: %d\r\n", kanalnya, nilainya);
	//printf("Pulsa ke-%d: %.1f -->", kanalnya, data_f[kanalnya*2-1]);
	
	#ifdef PAKAI_PILIHAN_FLOW
		
		konter.t_konter[kanalnya-1].hit = *(&MEM_RTC0+kanalnya-1) = (int) nilainya;
	#else
	
		//konter.t_konter[kanalnya-1].hit = *(&MEM_RTC0+kanalnya-1) = (int) nilainya;
	#endif
	//printf("  %x: hasil[%d]: %d <-- %d\r\n", (&MEM_RTC0+kanalnya-1), kanalnya, *(&MEM_RTC0+kanalnya-1), (int) nilainya );
	//printf("Pulsa ke-%d: %.1f -->\r\n", kanalnya, data_f[kanalnya*2-1]);
}

static tinysh_cmd_t set_mem_cmd={0,"set_rtc","set memori RTC","", set_mem_rtc,0,0,0};

void cek_mem_rtc(int argc, char **argv)	{
	int aa=0;
	if (argc>2) {
		printf("Perintah: \r\n");
		printf("  cek_rtc [kanal]\r\n");
		printf("  contoh: set_mem 3\r\n");
		return 0;
	} else if (argc==1) {
		return 0;
	}
	int kanalnya=0, nilainya=0;
	
	sscanf(argv[1], "%d", &kanalnya);
	
	aa = *(&MEM_RTC0+kanalnya-1);
	
	printf("kanal: %d --> nilai : %d\r\n", kanalnya, aa);

	//printf("  %x: hasil[%d]: %d <-- %d\r\n", (&MEM_RTC0+kanalnya-1), kanalnya, *(&MEM_RTC0+kanalnya-1), (int) nilainya );
	//printf("Pulsa ke-%d: %.1f -->\r\n", kanalnya, data_f[kanalnya*2-1]);
}

static tinysh_cmd_t cek_mem_cmd={0,"cek_rtc","cek memori RTC","", cek_mem_rtc,0,0,0};


#endif
#endif
