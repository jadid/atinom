/*
	ad7708 header
	*/

#if 0
#include <cdefBF532.h>
#include <ccblkfn.h>
#include <sysreg.h>
#include <defbf532.h>
#include <stdio.h>
#include <stdlib.h>

		
#define port_cs_ad7708		PF6
#define port_rdy_ad7708		PF8
#endif

#ifdef BOARD_KOMON_A_RTD
#define port_cs_ad7708		BIT(8)
#define port_rdy_ad7708		BIT(10)
#endif

#define m_read					0x40
#define m_write				0x00
#define reg_status			0x00
#define reg_mode				0x01
#define reg_control			0x02
#define reg_filter			0x03
#define reg_data				0x04
#define reg_offset			0x05
#define reg_gain				0x06
#define reg_iocon				0x07
#define reg_id					0x0F

//#define rate_7708 	25			// 55 data per detik
#define rate_7708		71			// 20 data per detik
//#define rate_7708		69


#define range_adc		15				// 0 - 2.5 volt
//#define range_adc		14				// 0 - 1.28 volt	
//#define range_adc		13				// 0 - 640 mV	

#if (range_adc == 15)
#define faktor_pengali	2.5
#endif


struct t_adc {
	unsigned int cur_kanal;
	unsigned int count;
	unsigned short data[20];
};


void cs_ad7708(void);
void uncs_ad7708(void);
void init_spi_ad7708(void);
unsigned char cek_status(void);
unsigned short baca_data(void);
unsigned char set_iocon(unsigned char t);
unsigned char set_filter(unsigned char t);
unsigned char set_adccon(unsigned char t);
unsigned char set_mode(unsigned char t);

unsigned char cek_iocon(void);
unsigned char cek_adccon(void);
unsigned char cek_mode(void);
unsigned char cek_adc_id(void);
void stop_adc(void);
void reset_adc(void);
unsigned char set_calibrated(unsigned char c);

inline unsigned int cek_adc_rdy(void);

//static unsigned short baca_data(void);
