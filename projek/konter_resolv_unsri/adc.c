/*
	25 Mei 2010
	furkan jadid
	daun biru engineering
	
	baca adc
	dicopy dari konter
	
	ditaruh di mode arm juga, biar cepat 
	
	8 Mar 2012
	ADC pakai loop saja
	dan difilter dengan IIR
	
	12 Mar 2012
	Mendapatkan data vakum dan throttle dengan
	throttle body PGMFI rada ribet, karena dipengaruhi
	oleh temperatur udara masuk.
	
	berdasarkan data2 pengambilan dan file spreadsheet ee.ods
	yang ada di folder SEM
	
	hubungan vakum terhadap temperatur :
	
	Vakum(T) = -0.43*(T) + 3138
	tekanan vakum pada temperatur tertentu.
	
	hubungan throttle thd temperatur
	Throttle(T) = 0.04(T) + 2881
	
	13 Maret 2012
	===================
	temperatur vs voltage bikin runyam
	pakai 3 linearisasi, dari pada bikin LUT
	
	jika temperatur < 2423 (menuju ke panas)
	V = -2.7796(T) + 9295
	
	jika 2423 < T < 2704
	V = -3.0966(T) + 10071
	
	jika T > 2704 (makin dingin)
	V = -3.6094(T) + 11469 
	
	error maksimum 2 %
	
	*/
	
#include "FreeRTOS.h"
#include <stdio.h> 
#include <stdlib.h>

#include <math.h>
#include <float.h>


#define PCADC		BIT(12)
//#define ADC_CLKDIV	(250 << 8)		// perlu 463 uS untuk 8 kanal
#define ADC_CLKDIV	(5 << 8)			// perlu 20 uS untuk 8 kanal


#define ADC_PDN		BIT(21)
#define ADC_START	BIT(24)
#define ADC_0		BIT(0)
#define ADC_1		BIT(1)
#define ADC_2		BIT(2)
#define ADC_3		BIT(3)
#define ADC_4		BIT(4)
#define ADC_5		BIT(5)
#define ADC_6		BIT(6)
#define ADC_7		BIT(7)


#define ADC_DONE	BIT(31)
#define ADC_BURST	BIT(16)

#define FAKTOR_BAT	5.0	// (100 k / 20 k)
#define REV_VOLTAGE	3300 

#define JUM_DATA		80
#define JUM_KANAL		8
#define LEN_FILTER		63


short data_adc_in[ JUM_KANAL ];
short data_adc_out[ JUM_KANAL ];
short data_adc_lama[ JUM_KANAL ];
int data_adc[ JUM_KANAL ];
int data_adc2[ JUM_KANAL ];			/* data lama dari IIR setiap putaran */

int count_data_total = 0;


#define E_NATURAL	2.718281828

short koef_1;
short koef_2;
short koef_12;
short koef_22;

//#define koef_iir_K	0.03125		// 1/32
//#define koef_iir_K	0.0078125		// 1/128
#define koef_iir_K	0.125		// 1/8


#define koef_iir_K2	0.15

void setup_adc(void)
{
	/* 19 Juni 2009, ADC power dinyalakan */
	PCONP |= PCADC;
	
	/* PCLK ADC pakai default (00) atau CCLK / 4 */

	/* P0.23 untuk AD00 */
	PINSEL1 &= ~(BIT(14) | BIT(15));
	PINSEL1 |= BIT(14);

	/* P0.24 untuk AD01 */
	PINSEL1 &= ~(BIT(16) | BIT(17));
	PINSEL1 |= BIT(16);
	
	/* P0.25 sebagai AD02 */
	PINSEL1 &= ~(BIT(19) | BIT(18));
	PINSEL1 |= BIT(18);

	/* P1.30 sebagai AD04 */
	PINSEL3 |= BIT(28) | BIT(29);

	/* P1.31 sebagai AD05 */
	PINSEL3 |= BIT(30) | BIT(31);	
	PINMODE3 |= BIT(30) | BIT(31);
		
}

/* dalam mili volt */
unsigned int read_bat_level(void)
{
	return (int) ( data_adc[ 0 ] * FAKTOR_BAT);
   //return 5;
}


int read_gas(void)
{
	
	//data_adc2[ 2 ] = filter_iir2( data_adc[ 2 ],  data_adc2[ 2 ] );
	//data_adc2[ 2 ] = filter_iir2( throt,  data_adc2[ 2 ] );
	return (int) data_adc[ 2 ];
}

/* milivolt temperature */
int read_temp(void)
{
	//return (int) data_adc[ 4 ];
	
	//data_adc2[ 0 ] = filter_iir2( data_adc[ 0 ],  data_adc2[ 0 ] );
	return (int) data_adc[ 5 ];
}

/* milivolt vakum pressure */
int read_vakum(void)
{
	return (int) data_adc[ 4 ];
}

void hitung_koef_IIR(void)
{
	float pangkat;
	float xf;
	float a0;
	float b1;
	
	//pangkat = (float) ( 0.50 / (float) (srate * 1.0) );
	//pangkat = (float) ( 0.317 / (float) (srate * 1.0) );		// masih kurang separo
	//pangkat = (float) ( 0.158 / (float) (srate * 1.0) );
	
	/* disini menghitung koefisien filter recursive */	
	//pangkat = (float) (2.0 * M_PI * pangkat);
	
	//xf = (float) pow( E_NATURAL, pangkat);
	//xf = 1.00 / xf;
	
	//a0 = (float) (1 - xf);
	//b1 = (float) xf;

	//a0 = 0.15;
	//b1 = 0.85;

	a0 = (float) koef_iir_K;
	b1 = (float) (1 - a0);

	koef_1 = (short) (a0 * 32768);
	koef_2 = (short) (b1 * 32768);
	
	a0 = (float) koef_iir_K2;
	b1 = (float) (1 - a0);

	koef_12 = (short) (a0 * 32768);
	koef_22 = (short) (b1 * 32768);
	
	printf("%s(): %d, 0x%X, %d, 0x%X\r\n", __FUNCTION__, koef_1, koef_1, koef_2, koef_2);
	return;
}

#if 0
short filter_iir(short data_in, short data)
{
	int AA;
	int BB;
	int CC;

	AA = (data_in * koef_1 / 32768);
	BB = (data * koef_2 / 32768);

	CC = (AA + BB);
	if ( CC > 0xFFFF)
	{
		printf("S");
		/* saturated */
		return (short) 0x7FFF;
	}
	return (short) CC;
}

void proses_ADC(int count)
{
	int stat;
	int i;
	int kanal = 1;		// mulai dari kanal 0
	float ft;

	short new_data;

	count_data_total++;
	
	if (count_data > JUM_DATA) count_data = 0;

	/* start ADC convertion dan tunggu */
	for (i=0; i<8; i++)
	{
		/* clear dulu */
		stat = AD0GDR;
	
		/* distart dulu, terus ditungguin */
		AD0CR = (ADC_CLKDIV | ADC_PDN | kanal | ADC_START );
	
		for (;;)
		{
			stat = AD0STAT;
			if (stat & BIT(16))
			{
				break;	
			}	
		}	

		//data_adc[ i ][ count_data ] = (short) (AD0GDR & 0xFFFF);
		//printf("0x%X, GDR %X\r\n", kanal, AD0GDR);
		//printf("0x%X, GDR %X\r\n", kanal, (short) data_adc[ i ][ count_data ]);

		new_data = (short) ((AD0GDR & 0xFFFF) >> 1);	// supaya jadi Q15
		data_adc_out[ i ] = filter_iir( new_data,  data_adc_lama[ i ] );

		// copy data untuk next conversion
		data_adc_lama[ i ] = data_adc_out[ i ];
		
		kanal = kanal << 1;
	}
	count_data++;
}

#endif


/* hemat 1 us untuk 8 loop jika pakai integer semua */
int filter_iir(int data_in, int data)
{
	int AA;
	int BB;
	int CC;

	AA = (data_in * koef_1 / 32768);
	BB = (data * koef_2 / 32768);

	CC = (AA + BB);
	if ( CC > 0xFFFF)
	{
		printf("S");
		/* saturated */
		return (int) 0x7FFF;
	}
	return (int) CC;
}

/* untuk proses setiap siklus */
int filter_iir2(int data_in, int data)
{
	int AA;
	int BB;
	int CC;

	AA = (data_in * koef_12 / 32768);
	BB = (data * koef_22 / 32768);

	CC = (AA + BB);
	if ( CC > 0xFFFF)
	{
		printf("S");
		/* saturated */
		return (int) 0x7FFF;
	}
	return (int) CC;
}


void proses_ADC(int count)
{
	int stat;
	int i;
	int y;
	int flag;
	int kanal = 1;		// mulai dari kanal 0
	float ft;

	count_data_total++;

	/*  start ADC convertion dan tunggu 
		sambil memfilter kanal sebelumnya

		bisa hemat 5 uS
	*/
	for (i=0; i<8; i++)
	{
		/* clear dulu */
		stat = AD0GDR;
	
		/* distart dulu, terus ditungguin */
		AD0CR = (ADC_CLKDIV | ADC_PDN | kanal | ADC_START );

		/* flag boleh difilter */
		flag = 1;
	
		for (;;)
		{
			stat = AD0STAT;
			if (stat & BIT(16))
			{
				break;	
			}	

			/* filter kanal sebelumnya */
			if (i != 0 && flag)
			{
				y = i - 1;
				data_adc_out[ y ] = filter_iir( data_adc_in[ y ],  data_adc_lama[ y ] );

				// copy data untuk next conversion
				data_adc_lama[ y ] = data_adc_out[ y ];

				/* flag sudah difilter */
				flag = 0;	
			}
		}	

		data_adc_in[ i ]  = (short) ((AD0GDR & 0xFFFF) >> 1);	// supaya jadi Q15		
		kanal = kanal << 1;
	}

	/* tinggal filter kanal 7 */
	data_adc_out[ 7 ] = filter_iir( data_adc_in[ 7 ],  data_adc_lama[ 7 ] );
	data_adc_lama[ 7 ] = data_adc_out[ 7 ];
}


void cek_data()
{
	int i;
	int f;

	printf("D (T%d)=", count_data_total);
	for (i=0; i<8; i++)
	{
		if (i==0)
		{ 
			f = (int) (data_adc_out[i] << 1); 	/* dikembalikan ke format 16 bit */
			f = (int) ( f * REV_VOLTAGE / 0xFFFF);
			f = f * FAKTOR_BAT;
			printf("%05d V (%05d), ", (unsigned short) f,  (unsigned short) data_adc_out[i]);
		}
		else
			printf("%05d, ", (unsigned short) data_adc_out[i]);
	}
	printf("\r\n");
}

void copy_data_ADC(void)
{
	int i;
	
	portENTER_CRITICAL();
	for (i=0; i<8; i++)
	{
		/* dikembalikan ke format 16 bit, dijadikan milivolt sekalian */
		data_adc[i] = (int) ((data_adc_out[i] << 1) * REV_VOLTAGE / 0xFFFF);
	}
	portEXIT_CRITICAL();
}

