#ifndef _ENVIRO_H_
#define _ENVIRO_H_

#include "FreeRTOS.h"

#define IAP_LOCATION 0x7FFFFFF1
typedef void (*IAP)(unsigned int [],unsigned int[]);

#include "../app/monita/monita_uip.h"

#define	 sONOFF			1
#define	 sPUSHBUTTON	2
#define	 sFLOW1			3
#define	 sFLOW2			4
#define	 nFLOW1			100
#define	 ssFLOW2		201
#define	 nFLOW2			202
#define	 fFLOW			203
#define	 DAYA			6
#define  fENERGI		251
#define	 sADC_ORI		9


/*
#ifdef BOARD_KOMON_KONTER
	#define KANALNYA 10
#else
	#define KANALNYA 10
#endif
//*/
struct t_kalib {
	float m;
	float C;
	char status;		// [status lihat define di atas !!!] //
	char ket[32];
	#ifdef PAKAI_ADC_ORI
	char adc;
	#endif
	char formula[32];
};

struct t_webclient {
	unsigned char IP0;
	unsigned char IP1;
	unsigned char IP2;
	unsigned char IP3;
	char hostname[16];
	char berkas[16];
	char burst;
	char status;		// status 0: default (pake IP), 1: pake URL.
};

struct t_env {
	char nama_board[32];
	unsigned char IP0;
	unsigned char IP1;
	unsigned char IP2;
	unsigned char IP3;
	unsigned char GW0;
	unsigned char GW1;
	unsigned char GW2;
	unsigned char GW3;
	struct t_kalib kalib[KANALNYA];
	int magic1;
	int magic2;
	int mmc_serial;
	char SN[20];
	char berkas[32];
	char statusWebClient;
	char banyak_sumber;
	char burst;
	unsigned char wIP0;
	unsigned char wIP1;
	unsigned char wIP2;
	unsigned char wIP3;
	char statusWebClientI;
	#ifdef PAKAI_TSC
	unsigned char k1;
	unsigned char k2;
	#endif
	#ifdef PAKAI_RTC
	unsigned char uRTC;
	#endif
	//#ifdef KIRIM_KE_SER_2
	char statusSerClient;
	int  intSer;
	//#endif
};

//#define SEKTOR_ENV	24
//#define ALMT_ENV	0x7A000

void save_env(int argc, char **argv);
void print_env(int argc, char **argv);
void set_default_ip(void);
int baca_env(char tampil);
int simpan_env( struct t_env *pgr);
#endif
