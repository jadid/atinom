/*
	Afrendy Bayu
	13 Juli 2011

*/


#include "FreeRTOS.h"

#ifndef __SHELL_RELAY__
#define __SHELL_RELAY__

#ifdef PAKAI_RELAY

void set_selenoid( unsigned int no )	{
	if (no>0 && no<=JML_RELAY) {		// 1-8 
		data_f[JML_SUMBER*PER_SUMBER+no-1] = 1;
	}
	
	if (no == 1)
		sRelay1();
		//FIO3SET = RLY_1;
	else if (no == 2)
		sRelay2();
		//FIO3SET = RLY_2;
	else if (no == 3)
		sRelay3();
		//FIO1SET = RLY_3;
	else if (no == 4)
		sRelay4();
		//FIO1SET = RLY_4;
	else if (no == 5)
		sRelay5();
		//FIO1SET = RLY_5;
	else if (no == 6)
		sRelay6();
		//FIO4SET = RLY_6;
	else if (no == 7)
		sRelay7();
		//FIO4SET = RLY_7;
	else if (no == 8)
		sRelay8();
		//FIO2SET = RLY_8;
	else
		printf("%s(): ERR tidak ada !\r\n", __FUNCTION__);
		
	
}


void unset_selenoid(unsigned int no )	{
	if (no>0 && no<=JML_RELAY) {
		data_f[JML_SUMBER*PER_SUMBER+no-1] = 0;
	}
	
	if (no == 1)
		//FIO3CLR = RLY_1;
		cRelay1();
	else if (no == 2)
		//FIO3CLR = RLY_2;
		cRelay2();
	else if (no == 3)
		//FIO1CLR = RLY_3;
		cRelay3();
	else if (no == 4)
		//FIO1CLR = RLY_4;
		cRelay4();
	else if (no == 5)
		//FIO1CLR = RLY_5;
		cRelay5();
	else if (no == 6)
		//FIO4CLR = RLY_6;
		cRelay6();
	else if (no == 7)
		//FIO4CLR = RLY_7;
		cRelay7();
	else if (no == 8)
		//FIO2CLR = RLY_8;
		cRelay8();
	else
		printf("%s(): ERR tidak ada !\r\n", __FUNCTION__);	
}


void set_relay(int argc, char **argv) {
	int sumb=0;
	unsigned char str_selenoid[24];
	
	if (argc < 3) {
		printf("Perintah salah atau Parameter kurang !!\r\n");
		return;
	} else {
		sprintf(str_selenoid, "%s", argv[1]);			// no cron
		sumb = cek_nomer_valid(str_selenoid, JML_RELAY);
		if (sumb <= 0)	{
			printf("No relay salah !\r\n");
			return;	
		}
		
		sprintf(str_selenoid, "%s", argv[2]);
		if ( (strcmp(str_selenoid, "aktif") == 0) || (str_selenoid[0]=='1') || (strcmp(str_selenoid, "hidup") == 0) )	{
			set_selenoid(sumb);
		} else if ( (strcmp(str_selenoid, "mati")  == 0) || (str_selenoid[0]=='0') ) {
			unset_selenoid(sumb);
		}
	}
}

#endif

#endif
