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

//#ifdef PAKAI_WEBCLIENT_INTERNET
	void set_semuaRelay(int id, int nilai)	{
		int r=0, p=0, rm = PER_SUMBER*JML_SUMBER;
		struct t_setting *konfig;
		konfig = (char *) ALMT_KONFIG;
		
		for (r=0; r<JML_RELAY; r++)	{
			if (id==konfig[rm+r].id)	{
				p = r+1;
				//printf("id: %d, konfig: %d, p: %d, nilai: %d\r\n", id, konfig[rm+r].id, p, nilai);
				break;
			}
		}
		
		if (nilai == 1)		
			set_selenoid(p);
		else unset_selenoid(p);
	}

	void parsing_cmd(char * cmd)	{
		//printf("cmdnya: %s\r\n", cmd);
		char status[30], rel[100];
		char *pch;
		int id, nilai;
		int inc;
		
		sscanf (cmd,"<html><body>%s %d %s", status, &inc, rel);
		pch = strrchr (rel,';');		
		rel[pch-rel] = 0;
		//printf("cmd   : %s\r\n", rel);
		
		pch = strtok (rel,";");
		while (pch != NULL)	{
			//printf ("isi: %s\n",pch);
			sscanf (pch,"%d,%d", &id, &nilai);
			//printf("id: %d, nilai: %d\r\n", id, nilai);
			set_semuaRelay(id, nilai);
			pch = strtok (NULL, ";");
		}
		
		#if 0
		rel[8] = '0';
		printf("%d : cmd   : %s\r\n", inc, rel);		// 3 101,0;102,1;103,1
		printf("status: %s, n: %d\r\n", status, inc);
		printf("1: %d\r\n", (int)(rel[0]-'0'));
		printf("2: %d\r\n", (int)(rel[1]-'0'));
		printf("3: %d\r\n", (int)(rel[2]-'0'));
		#endif
		
	}
//#endif

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
