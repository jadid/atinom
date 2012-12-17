/*
 * monita_uip.h
 *
 *  Created on: Dec 10, 2008
 *      Author: jadid
 
 	24 Juli 2009
 	ditambahkan PORT_KONTROL
	
	3 Feb 2010
	SEKTOR_DT_SET & ALMT_DT_SET ditambahkan
	dan disamakan alamatnya dengan TITIK
	
 */

//#include "../uip/uip/psock.h"
//#include "../uip/uip/uipopt.h"
#ifdef PAKAI_ETH
#include "../modul/uip/uip/psock.h"
#include "../modul/uip/uip/uipopt.h"
#endif

#ifndef MONITA_UIP_H_
#define MONITA_UIP_H_

#define MAGIC_1	0x33
#define MAGIC_2	0x81


#define MEM_RTC_CRON	100

char ayokirim;
unsigned char datakeserver[512];

#ifdef PAKAI_RELAY
//char kontrolList
#endif

#define BENAR 	1
#define AKTIF 	1
#define MATI	0


//#ifdef PAKAI_PM
#ifdef BANYAK_SUMBER
	// di sektor 26, harusnya bisa nyampe 120 sumber		--> masalahnya jml datanya terbatas
	#if defined(TAMPILAN_MONITA_STD)
		#define JML_SUMBER	15
	#elif defined(UNTUK_PLTD_AMPENAN)
		#define JML_SUMBER 12		// max 11 sumber tampilan bisa
	#else
		#define JML_SUMBER	5
	#endif
	//#define JML_SUMBER	5
	//#define JML_SUMBER	10
#else
	#define JML_SUMBER	1
#endif

#define JML_MESIN	10
#define TIAP_MESIN	100
//#define JML_KANAL	20

#define KANALNYA 10

#define PM710		0
#define PM810		1
#define	MICOM_M300	2
#define	MICOM_P127	3
#define ION8600		4
#define A2000		5
#define TFX_ULTRA	21
#define MOD_MONITA	100

#define INTEGER		1
#define FLOAT		2
#define DOUBLE		3


#define wSUMBER		3
#define wMODUL		4
#define wTES		5
#define wGROUP		6
#define wALARM		7
#define wKALIBRASI	8
#define wKANAL		9
#define wPULSA		10
#define wRELAY		101


//#define LITTLE_ENDIAN	1
//#define BIG_ENDIAN

// PER_SUMBER itu menujukkan jml data untuk 10 kanal.
#define SUMBER_420		10
#define SUMBER_KONTER	20
#define SUMBER_PM		33

#if defined(PAKAI_PM)
	#define PER_SUMBER	33
#elif defined(LIHAT_PM)
	#define PER_SUMBER 33
#elif defined(BOARD_KOMON_420_SAJA) || defined(BOARD_KOMON_420_SABANG_2_3)
	#define PER_SUMBER 10
#else
	#define PER_SUMBER	20
#endif


//unsigned int loop_idle=0;
//unsigned int idle_lama;
//unsigned int tot_idle;

typedef enum {
    CMD_SUCCESS = 0,
    INVALID_COMMAND,
    SRC_ADDR_ERROR,
    DST_ADDR_ERROR,
    SRC_ADDR_NOT_MAPPED,
    DST_ADDR_NOT_MAPPED,
    COUNT_ERROR,
    INVALID_SECTOR,
    SECTOR_NOT_BLANK,
    SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION,
    COMPARE_ERROR,
    BUSY,
} __e_iap_status;



#ifdef PAKAI_GSM_FTP
	int saat_gsm_aksi;
	int status_modem;
#endif

#ifdef PAKAI_MODEM_SERIAL
	int saat_gsm_aksi;
	int status_modem;
#endif

#ifdef PAKAI_RESOLV
	int status_webc_i;
	char urlweb[32];
#endif

#ifdef PAKAI_WEBCLIENT_INTERNET

#endif

#ifdef PAKAI_MODEM_GSM
	char flagModem;
	char saat_gsm_aksi;
	char status_modem;
	char index_gsm;
	
	#define CEK_AT				100
	#define M_SMS_PENUH			200
	#define BACA_SMS			1
	#define HAPUS_SMS			2
	#define CEK_PULSA			4
	#define BACA_SMS_TUNGGAL	8
	#define KIRIM_SMS			16
	
#endif

#ifdef PAKAI_TIMER_2
	char flagT2;
	int  iTim2;
#endif

#ifdef PAKAI_RTC
	unsigned char flagRTCc;
	unsigned char flagRTCs;
	unsigned char flagRTCm;
	unsigned char flagRTCh;
	unsigned char flagRTCd;
	
typedef struct __attribute__ ((packed)) {
	union  {
		struct	{
			unsigned int counter   : 14;
			unsigned int rsvd15_31 : 18;
		};
		unsigned int i;
	};
} rtcCTC_t;

typedef struct __attribute__ ((packed))	{
	union	{
		struct	{
			unsigned int seconds   : 6;
			unsigned int rsvd7_6   : 2;
			unsigned int minutes   : 6;
			unsigned int rsvd14_15 : 2;
			unsigned int hours     : 5;
			unsigned int rsvd21_23 : 3;
			unsigned int dow       : 3;
			unsigned int rsvd27_31 : 5;
		};
		unsigned int i;
	};
} rtcCTIME0_t;

typedef struct __attribute__ ((packed))	{
	union	{
		struct	{
			unsigned int dom       : 5;
			unsigned int rsvd5_7   : 3;
			unsigned int month     : 4;
			unsigned int rsvd12_15 : 4;
			unsigned int year      : 12;
			unsigned int rsvd28_31 : 4;
		};
		unsigned int i;
	};
} rtcCTIME1_t;

typedef struct __attribute__ ((packed))	{
	union	{
		struct 	{
			unsigned int doy       : 12;
			unsigned int rsvd12_31 : 20;
		};
		unsigned int i;
	};
} rtcCTIME2_t;
	
#endif

#ifdef KIRIM_KE_SER_2
	char statKirimSer;
#endif

#ifdef BOARD_KOMON_KONTER
	
	unsigned char status_konter[KANALNYA];
#endif

#ifdef PAKAI_PUSHBUTTON
	#define DELAY_DEBOUND	6		// 2 * 500 ms
	unsigned int debound[KANALNYA];
	char statPush [8];
#endif

#ifdef PAKAI_FILE_SIMPAN
	//#define SEKTOR_SFILE	16
	//#define ALMT_SFILE		0x48000		//0x7A000
	#define SEKTOR_SFILE	18
	#define ALMT_SFILE		0x58000		//0x7A000
	//
#endif

// 32 KB
#define ALMT_SEKTOR_8	0x08000
#define ALMT_SEKTOR_9	0x10000
#define ALMT_SEKTOR_10	0x18000
#define ALMT_SEKTOR_11	0x20000
#define ALMT_SEKTOR_12	0x28000
#define ALMT_SEKTOR_13	0x30000
#define ALMT_SEKTOR_14	0x38000
#define ALMT_SEKTOR_15	0x40000
#define ALMT_SEKTOR_16	0x48000
#define ALMT_SEKTOR_17	0x50000
#define ALMT_SEKTOR_18	0x58000
#define ALMT_SEKTOR_19	0x60000
#define ALMT_SEKTOR_20	0x68000
#define ALMT_SEKTOR_21	0x70000

// 4 KB
#define ALMT_SEKTOR_22	0x78000
#define ALMT_SEKTOR_23	0x79000
#define ALMT_SEKTOR_24	0x7A000
#define ALMT_SEKTOR_25	0x7B000
#define ALMT_SEKTOR_26	0x7C000
#define ALMT_SEKTOR_27	0x7D000



#define SEKTOR_ENV	24
#define ALMT_ENV	0x7A000

// pengakses: tinysh/setting_pm.c dipakai PM di board 420_SABANG
#define SEKTOR_SUMBER	25
#define ALMT_SUMBER		0x7B000

//------------------------------------------------------------//
#define SEKTOR_MESIN	26
#define ALMT_MESIN		0x7C000

#define SEKTOR_GROUP	26
#define ALMT_GROUP		0x7C000

// pengakses: tinysh/setting_eth.c
//#define SEKTOR_KONFIG	26			// konfig id titik ukur
//#define ALMT_KONFIG	0x7C000
//------------------------------------------------------------//

//------------------------------------------------------------//
#define SEKTOR_TITIK	21
#define ALMT_TITIK		ALMT_SEKTOR_21

// pengakses: tinysh/setting_eth.c, dipindah dari sektor 26 ke 21
#define SEKTOR_KONFIG	19			// konfig id titik ukur
#define ALMT_KONFIG		ALMT_SEKTOR_19
//------------------------------------------------------------//

#define SEKTOR_DT_SET	20			// 32 kB
//#define ALMT_DT_SET		ALMT_SEKTOR_20
#define ALMT_DT_SET		0x00068000 

//#ifdef PAKAI_GSM_FTP
	#define SEKTOR_GSM_FTP	27
	#define ALMT_GSM_FTP	0x7D000
//#endif

#ifdef PAKAI_RTC

#endif

#ifdef PAKAI_CRON				// berlebihan kah ? 32 kB 
	#define JML_CRON	10		// baca_cron() setiap 1/2 detik mulai detik ke 5
								// ubah mulai baca_cron di tinysh/cron.c
								// max JML_CRON = 108.
	#define SEKTOR_CRON	22		// 32 kB/64 = 512.
	//#define ALMT_CRON	0x68000
	#define ALMT_CRON	ALMT_SEKTOR_22
#endif

//#ifdef PAKAI_SELENOID
#ifdef PAKAI_RELAY
	#define JML_RELAY 8
#endif

#define 	PORT_MONITA 	5001
#define 	PORT_DAYTIME	13
#define 	ISO_nl 			0x0a
#define		PORT_KONTROL	7235

#ifdef PAKAI_KIRIM_BALIK
	#define		PORT_KIRIM_BALIK	6543
#endif

#ifdef PAKAI_HTTP
	#define		PORT_HTTP	80
#endif

#ifdef PAKAI_WEBCLIENT
	#define		PORT_HTTP	80
#endif

#ifdef PAKAI_TELNET
	#define		PORT_TELNET	23
#endif

#define		PORT_MODBUSTCP	502

#ifdef PAKAI_MODBUSTCP
	#define		PORT_MODBUSTCP	502
#endif

#ifdef PAKAI_ETH_TEST
	#define 	PORT_ETH_TEST	9876
#endif

/* offset masing2 data pada array */
#if 0
#define OFFSET_CAP_L	1	- 1
#define OFFSET_CAP_R	2	- 1
#define OFFSET_CAT_L	3	- 1
#define OFFSET_CAT_R	4	- 1

#define OFFSET_JWP_IN1	5	- 1
#define OFFSET_JWP_IN2	6	- 1
#define OFFSET_JWP_OUT1	7	- 1
#define OFFSET_JWP_OUT2	8	- 1
	
#define OFFSET_JWT_IN1	9	- 1
#define OFFSET_JWT_IN2	10	- 1
#define OFFSET_JWT_OUT1	11	- 1
#define OFFSET_JWT_OUT2	12	- 1

#define OFFSET_LOP_IN1	13	- 1
#define OFFSET_LOP_IN2	14	- 1
#define OFFSET_LOP_OUT1	15	- 1
#define OFFSET_LOP_OUT2	16	- 1

#define OFFSET_LOT_IN1	17	- 1
#define OFFSET_LOT_IN2	18	- 1
#define OFFSET_LOT_OUT1	19	- 1
#define OFFSET_LOT_OUT2	20	- 1

#define OFFSET_EXHT1	21	- 1
#define OFFSET_EXHT2	22	- 1
#define OFFSET_EXHT3	23	- 1
#define OFFSET_EXHT4	24	- 1
#define OFFSET_EXHT5	25	- 1
#define OFFSET_EXHT6	26	- 1
#define OFFSET_EXHT7	27	- 1
#define OFFSET_EXHT8	28	- 1
#define OFFSET_EXHT9	29	- 1
#define OFFSET_EXHT10	30	- 1
#define OFFSET_EXHT11	31	- 1
#define OFFSET_EXHT12	32	- 1
#define OFFSET_EXHT13	33	- 1
#define OFFSET_EXHT14	34	- 1
#define OFFSET_EXHT15	35	- 1
#define OFFSET_EXHT16	36	- 1
#define OFFSET_EXHT17	37	- 1
#define OFFSET_EXHT18	38	- 1
#define OFFSET_EXHT19	39	- 1
#define OFFSET_EXHT20	40	- 1


#define OFFSET_FREK		45	- 1
#define OFFSET_GEN_RPM	46	- 1

#define OFFSET_BB_IN	49	- 1
#define OFFSET_BB_OUT	50	- 1

#define OFFSET_KW		51	- 1
#define OFFSET_KWH		52	- 1
#define OFFSET_VOLT		53	- 1
#define OFFSET_AMP		54	- 1
#endif


//#ifdef  PAKAI_SELENOID
#ifdef PAKAI_RELAY
float data_f [ (JML_SUMBER * PER_SUMBER)+JML_RELAY ];
//*
#else
float data_f [ (JML_SUMBER * PER_SUMBER) ];
#endif
//*/

#ifdef TIPE_TFX_ULTRA
	int iCountTFX;
	unsigned int nFlowTFXlama;
#endif

struct t_data_float {
	float data[PER_SUMBER];
};

#ifdef PAKAI_MODBUSTCP
struct t_reg_modbus {
	unsigned char ref[5];
	unsigned char len;
	unsigned char ui;
	unsigned char func;
	unsigned char nReg;
	unsigned char data[PER_SUMBER*2];
	unsigned int  reg;
};
#endif

struct t_xdata {
	char mon[8];             // id bahwa data monita
    unsigned int nomer;      // urutan pengiriman data
    unsigned char flag;      // belum di definisikan
    unsigned char alamat;    // alamat stacking board (address power meter)

	char buf[232];				//16 jan 08, disesuaikan dengan Client Utama
};

struct sambungan_state {
	/*
	struct psock p;
	unsigned int state;
	unsigned int nomer_samb;
	unsigned int timer;
	*/
};

typedef struct sambungan_state uip_sambungan_state;
//typedef struct sambungan_state uip_tcp_appstate_t;

/*
sumber status :
	0 : tidak ada
	1 : time_out
	2 : normal
*/

struct t_setting {
	int id;
	char status;		// tidak aktif, timeout, dll
	//char ket[20];
};

struct t_sumber {
	char nama[24];
	char alamat;		/* untuk alamat modbus Power meter atau stack board (jika ada) */
	unsigned char IP0;			// klo sumber berupa modul monita 
	unsigned char IP1;
	unsigned char IP2;
	unsigned char IP3;
	//char modul;			// khusus modbus, jenis modul 0: PM, 1: KTA, 2:????, dst
	char stack;			// jika modul berisi BANYAK_SUMBER : adc, pm, dll
	char status;		// tidak aktif, timeout, dll
	char tipe;			// 0:PM_710, 1:PM_810, 2:KTA, 3:MICOM
};

/*
#ifdef PAKAI_PM
	struct t_sumber_pmx {
		char nama[32];
		char alamat;		// alamat PM //
		char status;		// aktif tidak aktif
		char modul;			// 0: PM, 1: KTA, 2:????, dst
	};

	struct t_sumber_pm {
		char magic1;
		char magic2;
		char status;
		struct t_sumber_pmx pm[JML_SUMBER];
	};
#endif
//*/
struct t_titik {
	char ID_sumber;			// sumber mulai dari 1, 1 untuk array sumber ke nol
	char kanal;				// kanal mulai dari 1, bukan nol, kanal 1 untuk array ke nol
	char alamat;			// alamat stack board
	float data;
};

struct t_mesin {
	char nama[10];
	char ID_mesin;
};

struct t_status {
	unsigned int reply;
	unsigned int lport;
	unsigned int timer;
	unsigned int reply_lama;
	unsigned int stat;
};

/* 
	untuk mendapatkan data2 yang perlu di kalkulasi 
	misalnya :
	BB in per menit.
	kWh per menit, sehingga bisa didapat SFC yang dihitung
	per menit
	
*/

struct t_data_hitung {
	float bb_in;
	float bb_in_lama;
	float bb_out;
	float bb_out_lama;
	float kwh;
	float kwh_lama;
	float sfc;
};

struct t_xcontrol {
	char  header[12];
 	short out_kanal;	// 16 bit (akan ada 64 K obyek kontrol)
 	short status;	// akan ada 64 kemungkinan status
 	float analog;	// future use
};

/* Konfigurasi versi 2 */

/* group adalah nama mesin yang ditampilkan pada tabbook atas
 * pada tampilan */
struct t_group {
	char nama[10];
	char ID_group;
	char stat;			// 1 aktif (ditampilkan), 0 mati / unset (tidak ditampilkan) 
	char ket[32];		// keterangan group yang ditampilkan pada bagian bawah
	short no_data[40];	// nomer data yang perlu ditampilkan.
};

struct t_dt_set  {
	float 	alarm_HH;			// 4
	float 	alarm_LL;
	char	relay;
	char 	aktif;
	//char	ket[32];	
	char	nama[16];
	char 	satuan[8];
	float 	alarm_H;
	float 	alarm_L;
	//#ifdef UNTUK_PLTD_LOPANA
		float batas_bawah;
		float batas_atas;
	//#endif
};

#ifdef PAKAI_FILE_SIMPAN
struct t_simpan_file {
	int 	periode;		// periode disimpan dalam MMC
	int 	set;			// aktif atau tidak
	int 	detik;			// periode data disimpan dalam file
	short 	no_data[ JML_SUMBER * PER_SUMBER ];
	char	nama_file[16];	// awalan nama file, misalnya angin
	char	flag[80];
	char	direktori[40];
};
#endif

#ifdef PAKAI_GSM_FTP
struct t_gsm_ftp {
	int 	ftp_periode;		// 1 periode disimpan dalam MMC
	int 	ftp_mode;			// 2 aktif atau tidak mode ftp
	int		ftp_port;			// 3 portnya ftp
	char 	ftp_user[40];		// 4 periode data disimpan dalam file
	char 	ftp_passwd[16];		// 5
	char 	ftp_server[40];		// 6
	char	nama_file[40];		// 7 awalan nama file, misalnya angin
	char	direktori[40];		// 8
	int		gsm_mode;			// 9 aktif atau tidak mode gsm
	char	kartu[15];
	char	gprs_apn1[16];		// 10
	char	gprs_apn2[16];		// 11
	char	gprs_user[16];		// 12
	char	gprs_passwd[16];	// 13
};
#endif

#ifdef PAKAI_CRON
struct t_cron {
	char 	mnt[20];
	char 	jam[20];
	char	tgl[20];
	char	bln[20];
	char	cmd[20];
	char 	alamat;
	char	status;			// status perintahnya : suruh dihidup ato mati
	char 	set;			// status cron : aktif atau mati
};
#endif 

#endif /* MONITA_UIP_H_ */
