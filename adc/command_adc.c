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
	
	for (i=0; i<10; i++)
	{
		f = st_adc.data[i] * faktor_pengali / 0xffff;
		printf("%2d : %6d : %1.4f V\r\n", (i+1), st_adc.data[i], f);
	}	
}							 

static tinysh_cmd_t cek_adc_cmd={0,"status_adc","data kounter/rpm","[args]",
                              status_adc,0,0,0};
