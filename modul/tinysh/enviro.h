#ifndef _ENVIRO_H_
#define _ENVIRO_H_


#define IAP_LOCATION 0x7FFFFFF1
typedef void (*IAP)(unsigned int [],unsigned int[]);

#ifdef BOARD_KOMON_KONTER
	#define KANALNYA 10
#else
	#define KANALNYA 10
#endif

struct t_kalib {
	float m;
	float C;
	char status;		// 0: adc tegangan		1: sensor pintu
	char ket[32];
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
};

#define SEKTOR_ENV	24
#define ALMT_ENV	0x7A000

void save_env(int argc, char **argv);
void print_env(int argc, char **argv);
void set_default_ip(void);
int baca_env(char tampil);
int simpan_env( struct t_env *pgr);
#endif
