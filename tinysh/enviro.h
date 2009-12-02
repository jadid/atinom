#ifndef _ENVIRO_H_
#define _ENVIRO_H_


#define IAP_LOCATION 0x7FFFFFF1
typedef void (*IAP)(unsigned int [],unsigned int[]);

struct t_kalib {
	float m;
	float C;
	char ket[64];
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
	struct t_kalib kalib[20];
	int magic1;
	int magic2;
	int mmc_serial;
};

void save_env(int argc, char **argv);
void print_env(int argc, char **argv);
void set_default_ip(void);
int baca_env(char tampil);

#endif
