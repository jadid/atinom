/*
	adc internal LPC2368 / LPC2387
	banyak dipindah dari konter/main.c
	
	12 Juli 2009
	furkan jadid, daun biru engineering
	
	*/
/* Scheduler includes. */
#include "FreeRTOS.h"

#define PCADC		BIT(12)
#define ADC_CLKDIV	(250 << 8)
#define ADC_PDN		BIT(21)
#define ADC_START	BIT(24)
#define ADC_1		BIT(0)
#define ADC_2		BIT(1)
#define ADC_DONE	BIT(31)	


void setup_adc(void)
{
	/* 19 Juni 2009, ADC power dinyalakan */
	PCONP |= PCADC;
	
	/* PCLK ADC pakai default (00) atau CCLK / 4 */
	
	/* setup PINSEL1 utk AD0 & AD1 */
	PINSEL1 |= (BIT(14) | BIT(16));
}

void start_adc_1(void)
{
	AD0CR = (ADC_CLKDIV | ADC_PDN | ADC_START | ADC_1);
}

void start_adc_2(void)
{
	AD0CR = (ADC_CLKDIV | ADC_PDN | ADC_START | ADC_2);
}

unsigned short read_adc_1(void)
{
	unsigned int stat;
	/*
	stat = AD0STAT;
	
	if (stat & 0x01)
	{
		stat = AD0DR0;	
		return (stat & 0xFFFF);
	}
	else
		return 0;
	*/
	
	stat = AD0DR0;
	if (stat & ADC_DONE)
	{
		return (stat & 0xFFFF);
	}
	else
		return 0;
}

unsigned short read_adc_2(void)
{
	unsigned int stat;
	
	stat = AD0DR1;
	if (stat & ADC_DONE)
	{
		return (stat & 0xFFFF);
	}
	else
		return 0;
}

