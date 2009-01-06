/*
	reset_cpu
	
	furkan jadid
	daun biru engineering
	12 Des 2008
*/
#include "FreeRTOS.h"


void reset_cpu(void)
{
	printf("Resetting cpu ...\n");
	
	WDTC = 300;
	WDMOD = 0x03;
	WDFEED = 0xAA;
	WDFEED = 0x55;
}
