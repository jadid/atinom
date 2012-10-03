/*
	Afrendy Bayu
	13 Juli 2011

*/


#include "FreeRTOS.h"

#ifndef __SHELL_RELAY__
#define __SHELL_RELAY__

#ifdef PAKAI_RELAY

#define AWAL_RELAY (JML_SUMBER*PER_SUMBER)

int cek_relays(int argc, char **argv)	{
	int sumb=0;
	if (argc == 1)	{	// cek_relay
		//printf("Salah asuhan, arg = 1 !!\r\n  cek_relay [no_relay]");
		int jmlData = (sizeof(data_f)/sizeof(float));
		struct t_setting *konfig;
		konfig = (char *) ALMT_KONFIG;
		
		for (sumb=0; sumb<JML_RELAY; sumb++) {
			printf("  Relay-%-2d : ID: %5d : %s\r\n", sumb+1, konfig[AWAL_RELAY+sumb].id, (data_f[(AWAL_RELAY)+sumb]==1)?"Aktif":"Mati");
		}
		return 0;
	}
	else if (argc > 2)	{
		printf("Salah asuhan, arg > 2 !!\r\n  cek_relay [no_relay]");
		return 0;
	}
	
	if (strcmp("id", argv[1]))	{
		
	} else {
	
		unsigned char str_relay[24];
		
		sprintf(str_relay, "%s", argv[1]);	
		sumb = cek_nomer_valid(str_relay, 8);
		printf("  Relay %d : %s\r\n", sumb, (data_f[(AWAL_RELAY)+sumb-1]==1)?"Aktif":"Mati");
	}
}


static tinysh_cmd_t cek_relay_cmd={0,"cek_relay","menampilkan status relay","[] nomer", cek_relays,0,0,0};

#if 0
void set_selenoid( unsigned int no )	{
	if (no>0 && no<=JML_RELAY) {		// 1-8 
		data_f[AWAL_RELAY+no-1] = 1;
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
		data_f[AWAL_RELAY+no-1] = 0;
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
#endif

void set_relays(int argc, char **argv) {
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
		
		if (argc == 4)	{
			int stat;
			int jmlData = (sizeof(data_f)/sizeof(float));
			
			struct t_setting *p_sbr;
			printf("Jml Data : %d\r\n", jmlData);

			p_sbr = pvPortMalloc( jmlData * sizeof (struct t_setting) );
			
			if (p_sbr == NULL)	{
				printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
				return;
			}
			printf(" %s(): Mallok ok di %X\r\n", __FUNCTION__, p_sbr);	
			memcpy((char *) p_sbr, (char *) ALMT_KONFIG, (jmlData * sizeof (struct t_setting)));

			if (strcmp(argv[2], "id") == 0)		{
				sprintf(str_selenoid, "%s", argv[3]);	
				stat = cek_nomer_sumber(str_selenoid, 32765);
				p_sbr[AWAL_RELAY+sumb-1].id = stat;
				printf("ID = %d\r\n", p_sbr[AWAL_RELAY+sumb-1].id);
			}
			
			if (simpan_konfignya( p_sbr ) < 0)		{
				vPortFree( p_sbr );
				return;
			}
			vPortFree( p_sbr );

		}
		else {		//  cmd: set_relay [no_relay] [aktif|1| mati|0]
			sprintf(str_selenoid, "%s", argv[2]);
			if ( (strcmp(str_selenoid, "aktif") == 0) || (str_selenoid[0]=='1') || (strcmp(str_selenoid, "hidup") == 0) )	{
				set_selenoid(sumb);
			} else if ( (strcmp(str_selenoid, "mati")  == 0) || (str_selenoid[0]=='0') ) {
				unset_selenoid(sumb);
			}
		}
	}
}

static tinysh_cmd_t set_relay_cmd={0,"set_relay","setting relay", "help default ",set_relays,0,0,0};

#endif

#endif
