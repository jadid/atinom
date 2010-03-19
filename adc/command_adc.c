/* 
	shell command berkaitan dengan adc
	15 april 2009
	furkan jadid, daun biru engineering
	
	*/

#include "ad7708.h"

extern struct t_adc st_adc;

void status_adc(void)
{
	int i;
	float f;
	
	printf(" ADC count = %d\r\n", st_adc.count);
	
	#ifdef BOARD_KOMON_B_THERMO
	for (i=0; i<10; i++)
	{
		f = st_adc.data[i] * faktor_pengali / 0xffff;
		printf("%2d : %6d : %1.4f V\r\n", (i+1), st_adc.data[i], f);
	}	
	#endif
	
	#ifdef BOARD_KOMON_A_RTD
	printf("Voltage RTD\r\n");
	for (i=0; i<5; i++)
	{
		f = st_adc.data[i] * faktor_pengali_RTD / 0xffff;
		printf("%2d : %6d : %1.4f V\r\n", (i+1), st_adc.data[i], f);
	}
	
	printf("Voltage 4-20 mA\r\n");
	for (i=5; i<10; i++)
	{
		f = st_adc.data[i] * faktor_pengali_420 / 0xffff;
		printf("%2d : %6d : %1.4f V\r\n", (i+1), st_adc.data[i], f);
	}
	#endif
	
	#ifdef BOARD_KOMON_420_SAJA
	printf("Voltage 4-20 mA\r\n");
	for (i=0; i<10; i++)
	{
		f = st_adc.data[i] * faktor_pengali_420 / 0xffff;
		printf("%2d : %6d : %1.4f V\r\n", (i+1), st_adc.data[i], f);
	}
	#endif
}							 

static tinysh_cmd_t cek_adc_cmd={0,"status_adc","konversi adc & voltnya","[args]",
                              status_adc,0,0,0};
