/***
	Pakai ADC original LPC
	Source code nyonto ECU
	21 Maret 2012
//*/

#include "FreeRTOS.h"
#include "lpc23xx.h"
#include "hardware.h"
#include <math.h>
#include "../tinysh/enviro.h"
#include "adc_ori.h"

//extern struct t_env env2;

#ifndef __ADC_ORI__
#define __ADC_ORI__

#ifdef PAKAI_ADC_ORI

short koef_1;
short koef_2;
short koef_12;
short koef_22;

short data_adc_in[ JML_ADC ];
short data_adc_out[ JML_ADC ];
short data_adc_lama[ JML_ADC ];
int data_adc[ JML_ADC ];
int data_adc2[ JML_ADC ];			/* data lama dari IIR setiap putaran */

void hitung_koef_IIR(void)
{
	float pangkat;
	float xf;
	float a0;
	float b1;
	
	/*
	//pangkat = (float) ( 0.50 / (float) (srate * 1.0) );
	//pangkat = (float) ( 0.317 / (float) (srate * 1.0) );		// masih kurang separo
	//pangkat = (float) ( 0.158 / (float) (srate * 1.0) );
	
	// disini menghitung koefisien filter recursive //	
	//pangkat = (float) (2.0 * M_PI * pangkat);
	
	//xf = (float) pow( E_NATURAL, pangkat);
	//xf = 1.00 / xf;
	
	//a0 = (float) (1 - xf);
	//b1 = (float) xf;

	//a0 = 0.15;
	//b1 = 0.85;
	//*/

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

//extern struct t2_konter konter;

void init_adc_ori(void)	{
	char ii;
	adco.count = 0;
	adco.cur_kanal = 0;
	for(ii=0; ii<JML_ADC; ii++)	{
		adco.data[ii] = 0;
		adco.data_lama[ii] = 1;
	}
	hitung_koef_IIR();
}

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

#ifdef BOARD_KOMON_A_RTD
void proses_data_adc(void)	{
	
}
#endif

#ifdef BOARD_KOMON_B_THERMO
void proses_data_adc(void)	{

}
#endif

#if defined(BOARD_KOMON_420_SAJA) || defined(BOARD_KOMON_420_SABANG) || defined(BOARD_KOMON_420_SABANG_2_3)
void proses_data_adc_ori(void)	{

}
#endif

#if defined(BOARD_KOMON_KONTER)

int proses_data_adc_ori(char kanalx)	{
	unsigned int du;
	unsigned char k,ll, flag;
	
	du = AD0GDR;		// dibaca, DONE clear
	k = BIT(kanalx);
	adco.count++;
	
	#ifdef DEBUG_ADC_ORI
	printf("du : 0x%08x, chl: %d, DONE: %d, nilai: %d\r\n", du, (du & (GESER_NM(7,24))),  (du & BIT(31))>>31, (0x3FF & GESER_NM_KA(du,6)) );
	#endif
	AD0CR = (ADC_CLKDIV | ADC_PDN | k | ADC_START );
	
	flag = 1;
	ll = 1-kanalx;	//	---> hanya berlaku jika JML_ADC = 2
	while (!(AD0STAT & BIT(16))) {
		if (flag)	{
			adco.data[ll] = filter_iir( adco.data_masuk[ll], adco.data_lama[ll] );
			adco.data_lama[ll] = adco.data[ll];
			flag = 0;
		}
	}
	
	
	du = AD0GDR;
	#ifdef DEBUG_ADC_ORI
	printf("du : 0x%08x, chl: %d, DONE: %d, nilai: %d\r\n", du, (du & (GESER_NM(7,24))),  (du & BIT(31))>>31, (0x3FF & GESER_NM_KA(du,6)) );
	printf("AD0DR0: 0x%08x\r\n", AD0DR0);
	printf("-------------------\r\n\r\n");
	#endif
	
	//du = ( 0x3FF & GESER_NM_KA(du,6) );
	//data_adc_in[kanalx]  = (short) ((AD0GDR & 0xFFFF) >> 1);
	adco.data_masuk[kanalx]  = (short) ( 0x3FF & GESER_NM_KA(du,6) );
	
	
	//if (kanalx>0 && kanalx<JML_ADC)	
	{
		//adco.data[ll] = du;
		#ifdef DEBUG_ADC_ORI
		printf("kanal[%d]: %d\r\n", kanalx, adco.data[kanalx]);
		#endif
	}
	
	return (du);
}
#endif

void hitung_data_float(void)
{
	#ifdef BOARD_KOMON_KONTER
		
	#endif
	
	#ifdef BOARD_KOMON_B_THERMO	

	#endif

	#ifdef BOARD_KOMON_A_RTD

	#endif

	#ifdef BOARD_KOMON_420_SAJA

	#endif
}

#endif

#endif		// #define __ADC_ORI__
