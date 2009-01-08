#ifndef _ENVIRO_H_
#define _ENVIRO_H_


#define IAP_LOCATION 0x7FFFFFF1
typedef void (*IAP)(unsigned int [],unsigned int[]);

struct t_kalib {
	float m;
	float y;
	char ket[64];
};

struct t_env {
	char nama_board[32];
	char IP0;
	char IP1;
	char IP2;
	char IP3;
	char GW0;
	char GW1;
	char GW2;
	char GW3;
	struct t_kalib kalib[20];
	int magic1;
	int magic2;
};

void save_env(int argc, char **argv);
void print_env(int argc, char **argv);
void set_default_ip(void);
int baca_env(char tampil);
void getdef_env(int argc, char **argv);

#endif
