/*
	AD7708
	furkan jadid, daun biru engineering
	Jakarta, 8 Okt 2007
	
	15 april 2009
	porting untuk freeRTOS & LPC2368
	
	*/

#include "FreeRTOS.h"
#include "ad7708.h"
#include <math.h>
#include "../tinysh/enviro.h"
//extern struct t_env env2;

#ifndef __AD7708__
#define __AD7708__

#ifdef PAKAI_ADC

struct t_env *env2;
//env2 = (char *) ALMT_ENV;

static unsigned int kalibrated;
struct t_adc st_adc;

#include "kalibrasi_adc.c"

inline void cs_ad7708(void);
inline void uncs_ad7708(void);

#if 0
void init_spi_ad7708(void)	
{
	unsigned int sclk;
	unsigned short baute;
	
	*pSPI_CTL = (MSTR + CPHA + CPOL + 0x01);
	ssync();
   sclk = get_sclk();  
   //baute = (unsigned short) (sclk / (2 * 5000000));		// 5 MHz
   baute = (unsigned short) (sclk / (2 * 20000000));		
   
   printku("\n\r baut %d ", baute);
   
   *pSPI_BAUD = baute;
   ssync();
   *pSPI_FLG = 0xFF00;
   ssync();
   *pSPI_CTL = (SPE + MSTR + CPHA + CPOL + 0x01); 	//start saat read tdbr
   ssync();
}
#endif

#if BLACKFIN
inline void cs_ad7708(void)
{
	*pFIO_FLAG_C = port_cs_ad7708;
	ssync();
}

inline void uncs_ad7708(void)
{
	*pFIO_FLAG_S = port_cs_ad7708;
	ssync();
}
#endif

//#ifdef BOARD_KOMON_A_RTD

#define kirim_word	spiPut
#define ambil_word	spiPut

#if defined(BOARD_KOMON_420_SAJA) || defined(BOARD_KOMON_420_SABANG) || defined(BOARD_KOMON_420_SABANG_2_3)
void kirim_word_mmc(unsigned char t);
unsigned char ambil_word_mmc(void);

#define kirim_word	kirim_word_mmc
#define ambil_word	ambil_word_mmc

#endif

#define SPI_SPCCR	S0SPCCR
#define SPI_SPCR	S0SPCR
#define SPI_SPSR	S0SPSR
#define SPI_SPDR	S0SPDR
#define SPI_SPIF	0x80		// bit 7 status
#define MSTR		0x20		// bit 5
#define CPOL		0x10
#define CPHA		0x08

inline void cs_ad7708(void)
{
	portENTER_CRITICAL();
	SPI_SPCR = MSTR | CPOL;
	//FIO1CLR = port_cs_ad7708;
	//FIO0CLR = port_cs_ad7708;
	ad7708_select();
}

inline void uncs_ad7708(void)
{
	//FIO1SET = port_cs_ad7708;
	//FIO0SET = port_cs_ad7708;
	ad7708_deselect();
	SPI_SPCR = MSTR;			// ENC biar gak problem
	portEXIT_CRITICAL();
}

inline spi_rx(void)
{
	return ;
}
//#endif

unsigned char cek_status(void)
{
	unsigned char s;
	//s = ambil_word();
	cs_ad7708();
	kirim_word((m_read | reg_status));
	s = spi_rx();					//dummy read
	s= ambil_word();
	uncs_ad7708();
	return s;
}

inline unsigned short baca_data(void)
{
	//unsigned char a;
	
	unsigned char b;
	unsigned short s;
	
	//b = ambil_word();
	
	cs_ad7708();
	kirim_word(m_read | reg_data);
	s = spi_rx();					//dummy read
	s = ambil_word();
	b = ambil_word();
	
	s = ((s << 8) + b);
	uncs_ad7708();
	
	return s;
}

unsigned char set_iocon(unsigned char t)
{
	cs_ad7708();
	kirim_word(m_write | reg_iocon);
	kirim_word(t);
	uncs_ad7708();
	return 1;
}

unsigned char set_filter(unsigned char t)
{
	cs_ad7708();
	kirim_word(m_write | reg_filter);
	kirim_word(t);
	//kirim_word(250);
	uncs_ad7708();
	return 1;
}

unsigned char set_adccon(unsigned char t)
{
	cs_ad7708();
	kirim_word(m_write | reg_control);
	//kirim_word(7);		//
	kirim_word(t);
	uncs_ad7708();
	return 1;

}

unsigned char set_mode(unsigned char t)
{
	cs_ad7708();
	kirim_word(m_write | reg_mode);
	kirim_word(t);			//continus conversion
	uncs_ad7708();
	
	return 1;
}

unsigned char cek_test1adc(void)
{
	unsigned char s;
   cs_ad7708();
	kirim_word(m_read | reg_test1_adc);
	s = spi_rx();		//dummy, hanya mengosongkan RDBR
	s = ambil_word();
	uncs_ad7708();
	return s;
}

unsigned char cek_test2adc(void)
{
	unsigned char s;
   cs_ad7708();
	kirim_word(m_read | reg_test2_adc);
	s = spi_rx();		//dummy, hanya mengosongkan RDBR
	s = ambil_word();
	uncs_ad7708();
	return s;
}

//9 oktober

unsigned char cek_iocon(void)
{
	unsigned char s;
   cs_ad7708();
	kirim_word(m_read | reg_iocon);
	s = spi_rx();		//dummy, hanya mengosongkan RDBR
	s = ambil_word();
	uncs_ad7708();
	return s;
}

unsigned char cek_adccon(void)
{
	unsigned char s;
   cs_ad7708();
	kirim_word(m_read | reg_control);
	s = spi_rx();	//dummy, hanya mengosongkan RDBR
	s = ambil_word();
	uncs_ad7708();
	return s;
}

unsigned char cek_mode(void)
{
	unsigned char s;
   cs_ad7708();
	kirim_word(m_read | reg_mode);
	s = spi_rx();	//dummy, hanya mengosongkan RDBR
	s = ambil_word();
	uncs_ad7708();
	//printf(" mode: %02x__  ", s);
	return s;
}

unsigned char cek_filter(void)
{
	unsigned char s;
   	cs_ad7708();
	kirim_word(m_read | reg_filter);
	s = spi_rx();	//dummy, hanya mengosongkan RDBR
	s = ambil_word();
	uncs_ad7708();
	return s;
}

void stop_ad7708(void)
{
	cs_ad7708();
	kirim_word(m_write | reg_mode);
	kirim_word(1);
	
	//ditambah 14 Nov 2007
	uncs_ad7708();
}

void stop_adc(void)
{
	set_mode(1);	
	
	return;
}

unsigned char set_calibrated(unsigned char c)
{
	portENTER_CRITICAL();
	
	kalibrated = c;
	
	portEXIT_CRITICAL();
}

void reset_adc(void)
{
	int i;
	cs_ad7708();	
	for (i=0; i<6; i++) 
		kirim_word(0xFF);
	
	uncs_ad7708();
	
	memset( &st_adc, 0, sizeof (st_adc));
	
	st_adc.cur_kanal = 0;
}

unsigned char cek_adc_id(void)
{
	unsigned char r;
	
	
	cs_ad7708();
	kirim_word(m_read | reg_id);
	//spiPut(m_read | 0x0F);
	//r = spiPut(0x00);
	r = ambil_word();
	uncs_ad7708();

	return r;
}

void start_adc_1(int fdx)
{
	int i;
	unsigned char temp_char;
	unsigned char kanal_aktif;
	st_adc.cur_kanal = 0;
	
	printf("Starting ADC collecting data !\r\n");
	
	/* 	
		range adc thermocouple adalah fix di range_adc
		lihat di ad7708.h header 
	
		sedangkan range_adc untuk thermocouple adalah
		tergantung masing2 kanal
		
		kanal 1 -  5 adalah RTD
		kanal 6 - 10 adalah 4-20 mA
		
		output dari opamp RTD, jika 100 C adalah 0.3 Volt
		sehingga kanal 1 - 5 harus dipakai range
		320 mV
	*/
	
	#ifdef BOARD_KOMON_B_THERMO
	temp_char = (unsigned char) ((st_adc.cur_kanal << 4) + range_adc);
	#endif
	
	#ifdef BOARD_KOMON_A_RTD
	temp_char = (unsigned char) ((st_adc.cur_kanal << 4) + range_RTD);
	#endif
	
	#ifdef BOARD_KOMON_420_SAJA
	temp_char = (unsigned char) ((st_adc.cur_kanal << 4) + range_420);
	#endif

	#ifdef BOARD_KOMON_420_SABANG_2_3
	temp_char = (unsigned char) ((st_adc.cur_kanal << 4) + range_420);
	#endif
		
	set_adccon(temp_char);
	set_mode((unsigned char) (3 + 16));			//continuous sampling + CHCON	// 0x03 | 0x10
	
	//cek filter
	printf(" FILTER ADC = %d ;", (unsigned char) cek_filter());
	printf(" ADCCON = %d ;", (unsigned char) cek_adccon());
	printf(" MODE = %d\r\n",(unsigned char)  cek_mode());
	
}

inline unsigned int cek_adc_rdy(void)
{
	/* jika low, maka ada data baru */
	
	//if (FIO1DIR & port_rdy_ad7708) return 0;
	//else return 1;
	
	if (kalibrated == 1)
	{
		if ((cek_status() & 0x80) == 0x80) return 1;
		else return 0;
	}
	else
		return 0;
}

#ifdef BOARD_KOMON_A_RTD
void proses_data_adc(void)
{
	unsigned char temp;
	
	if (cek_adc_rdy() == 1)
	{
		st_adc.count++;
		st_adc.data[ st_adc.cur_kanal ] = baca_data();
		 	
		st_adc.cur_kanal++;
		if (st_adc.cur_kanal == 10)
		{
		 	/* satu round 10 kanal sudah selesai */
		 	st_adc.cur_kanal = 0;
		}
		
		if (st_adc.cur_kanal < 5)
		{
			temp = (unsigned char) ((st_adc.cur_kanal << 4) + range_RTD);
		}
		else
		{
			if (st_adc.cur_kanal == 8)
			temp = (unsigned char) ((14 << 4) + range_420);
		
			else if (st_adc.cur_kanal == 9)
			temp = (unsigned char) ((15 << 4) + range_420);
		
			else
			temp = (unsigned char) ((st_adc.cur_kanal << 4) + range_420);
		}
		
		set_adccon(temp);
	}
}
#endif

#ifdef BOARD_KOMON_B_THERMO
void proses_data_adc(void)
{
	unsigned char temp;
	
	if (cek_adc_rdy() == 1)
	{
		st_adc.count++;
		st_adc.data[ st_adc.cur_kanal ] = baca_data();
		 	
		st_adc.cur_kanal++;
		if (st_adc.cur_kanal == 10)
		{
		 	/* satu round 10 kanal sudah selesai */
		 	st_adc.cur_kanal = 0;
		}
		
		if (st_adc.cur_kanal == 8)
		temp = (unsigned char) ((14 << 4) + range_adc);
		
		else if (st_adc.cur_kanal == 9)
		temp = (unsigned char) ((15 << 4) + range_adc);
		
		else
		temp = (unsigned char) ((st_adc.cur_kanal << 4) + range_adc);
		
		set_adccon(temp);
	}
}
#endif

#if defined(BOARD_KOMON_420_SAJA) || defined(BOARD_KOMON_420_SABANG) || defined(BOARD_KOMON_420_SABANG_2_3)
void proses_data_adc(void)
{
	unsigned char temp;
	
	if (cek_adc_rdy() == 1)
	{
		st_adc.count++;
		st_adc.data[ st_adc.cur_kanal ] = baca_data();
		 	
		st_adc.cur_kanal++;
		if (st_adc.cur_kanal == 10)
		{
		 	/* satu round 10 kanal sudah selesai */
		 	st_adc.cur_kanal = 0;
		}
		
		
		if (st_adc.cur_kanal == 8)
			temp = (unsigned char) ((14 << 4) + range_420);
		
		else if (st_adc.cur_kanal == 9)
			temp = (unsigned char) ((15 << 4) + range_420);
		
		else
			temp = (unsigned char) ((st_adc.cur_kanal << 4) + range_420);
		
		set_adccon(temp);
	}
}
#endif

void hitung_data_float(void)
{
	int i;
	float fl;
	float temp_rpm;
	
#ifdef BOARD_KOMON_B_THERMO	
	for (i=0; i< 10; i++)
	{		
		/*  tegangan */		
		temp_rpm = st_adc.data[i] * faktor_pengali / 0xffff;
				
		/* satuan yang diinginkan */
		//st_adc.flt_data[i] = (float) (temp_rpm * env2.kalib[i].m) + env2.kalib[i].C;		
		st_adc.flt_data[i] = (float) (temp_rpm * env2->kalib[i].m) + env2->kalib[i].C;			}
#endif

#ifdef BOARD_KOMON_A_RTD
	for (i=0; i< 5; i++)
	{		
		/*  tegangan */
		
		temp_rpm = st_adc.data[i] * faktor_pengali_RTD / 0xffff;
		
		/* satuan yang diinginkan */
		//st_adc.flt_data[i] = (float) (temp_rpm * env2.kalib[i].m) + env2.kalib[i].C;				
		st_adc.flt_data[i] = (float) (temp_rpm * env2->kalib[i].m) + env2->kalib[i].C;				
	}
	
	/* data 4-20 mA */
	for (i=5; i< 10; i++)
	{		
		/*  tegangan */		
		temp_rpm = st_adc.data[i] * faktor_pengali_420 / 0xffff;
		
		/* satuan yang diinginkan */
		//st_adc.flt_data[i] = (float) (temp_rpm * env2.kalib[i].m) + env2.kalib[i].C;			
		st_adc.flt_data[i] = (float) (temp_rpm * env2->kalib[i].m) + env2->kalib[i].C;			
	}
#endif

#ifdef BOARD_KOMON_420_SAJA
	
	/* data 4-20 mA */
	for (i=0; i< KANALNYA; i++)
	{		
		/*  tegangan */
		temp_rpm = st_adc.data[i] * faktor_pengali_420 / 0xffff;
		
		/* satuan yang diinginkan */
		//st_adc.flt_data[i] = (float) (temp_rpm * env2.kalib[i].m) + env2.kalib[i].C;			
		st_adc.flt_data[i] = (float) (temp_rpm * env2->kalib[i].m) + env2->kalib[i].C;			
	}
#endif
}

#endif

#endif		// #define __AD7708__
