/*
	Afrendy Bayu
	13 Juli 2011

*/


#include "FreeRTOS.h"
#include "hardware.h"
#include "../monita/monita_uip.h"

#ifndef __APP_RELAY__
#define __APP_RELAY__

#ifdef PAKAI_RELAY

#ifdef PAKAI_WEBCLIENT_INTERNET
	void set_semuaRelay(char * rel)	{
		char r;
		for (r=0; r<JML_RELAY; r++) {
			(rel[r]-'0')?set_selenoid(r+1):unset_selenoid(r+1);
		}
	}

	void parsing_cmd(char * cmd)	{
		printf("cmdnya: %s\r\n", cmd);
		char status[120], rel[30];
		int inc;
		
		sscanf (cmd,"<html><body>%s %d %s", status, &inc, rel);
		rel[8] = '0';
		
		#if 1
		printf("status: %s, n: %d\r\n", status, inc);
		printf("cmd   : %s\r\n", rel);

		printf("1: %d\r\n", (int)(rel[0]-'0'));
		printf("2: %d\r\n", (int)(rel[1]-'0'));
		printf("3: %d\r\n", (int)(rel[2]-'0'));
		#endif
		set_semuaRelay(rel);
	}
#endif

void set_selenoid( unsigned int no )	{
	#if 1
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
	#if 1
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
		printf("%s(): ERR tidak ada Relay %d !\r\n", __FUNCTION__, no);	
}

unsigned char toogle_selenoid(unsigned int no) {
	if (no<1 && no>JML_RELAY) {
		return 0;
	}

	(data_f[(JML_SUMBER*PER_SUMBER)+no-1])?unset_selenoid(no):set_selenoid(no);
	return no;
}

#endif

#endif
