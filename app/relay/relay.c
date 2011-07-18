/*
	Afrendy Bayu
	13 Juli 2011

*/


#include "FreeRTOS.h"
#include "hardware.h"

#ifndef __APP_RELAY__
#define __APP_RELAY__

#ifdef PAKAI_RELAY

void set_selenoid( unsigned int no )	{
	#if 0
	if (no>0 && no<=JML_RELAY) {		// 1-8 
		data_f[JML_SUMBER*PER_SUMBER+no-1] = 1;
	}
	#endif

	if (no == 1)
		sRelay1();
	else if (no == 2)
		sRelay2();
	else if (no == 3)
		sRelay3();
	else if (no == 4)
		sRelay4();
	else if (no == 5)
		sRelay5();
	else if (no == 6)
		sRelay6();
	else if (no == 7)
		sRelay7();
	else if (no == 8)
		sRelay8();
	else
		printf("%s(): ERR tidak ada Relay !\r\n", __FUNCTION__);	
}


void unset_selenoid(unsigned int no )	{
	#if 0
	if (no>0 && no<=JML_RELAY) {
		data_f[JML_SUMBER*PER_SUMBER+no-1] = 0;
	}
	#endif
	
	if (no == 1)
		cRelay1();
	else if (no == 2)
		cRelay2();
	else if (no == 3)
		cRelay3();
	else if (no == 4)
		cRelay4();
	else if (no == 5)
		cRelay5();
	else if (no == 6)
		cRelay6();
	else if (no == 7)
		cRelay7();
	else if (no == 8)
		cRelay8();
	else
		printf("%s(): ERR tidak ada Relay !\r\n", __FUNCTION__);	
}

#endif

#endif
