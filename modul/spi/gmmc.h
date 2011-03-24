#ifndef __GMMC__
#define __GMMC__

#define BESAR_MMC_BLOK	512

/*
	//#define ssync() asm("ssync;");
	//#define cli()	embuh()
	//#define sti()	embuh()
*/
typedef unsigned char BOOL;



void mmc_error_out(void);
void panic(char *s);

BOOL getbit(void* ptr, unsigned int n) ;
unsigned int getvalue(void* ptr, unsigned int n, unsigned int len);
unsigned char get_cid_csd(void);
unsigned char tulis_mmc_multi(unsigned int alamat, unsigned char *p, int jum);
unsigned char baca_mmc_multi(unsigned int alamat, unsigned char *p, int jum);

inline void rx_busy(void);
inline void spi_tx(unsigned char txd);
inline void tx_busy(void);

unsigned char stop_mmc(void);
void mmc_crc_off(void);
unsigned short mmc_status(void);
void init_spi(unsigned short set_baud);
short tes_mmc_awal(void);
short init_mmc(void);
void cs_mmc(void);
void uncs_mmc(void);
char set_pjg_blok(long besar);
unsigned char tulis_mmc_blok(unsigned int alamat, unsigned char *p);
unsigned char baca_mmc_blok(unsigned int alamat, unsigned char *p);
void komand_mmc(unsigned char cmd, unsigned int argument,unsigned char crc);
unsigned char mmc_respon2(void);

void kirim_word(unsigned char t);
unsigned char ambil_word(void);
unsigned char spi_rx(void);

#endif
