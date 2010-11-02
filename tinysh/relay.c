/*

	data 
	3 Feb 2010, furkan jadid, daun biru engineering
	
	hal-hal yang berkaitan dengan data
	
	data diperoleh dari bacaan dari sumber.
	semua data dari sumber disimpan.
	
	tetapi data ada yang dimonitor (diaktifkan) atau tidak (pasif).
	
	data dapat disambungkan untuk mengaktifkan relay atau mematikan
	relay.
	
	satu buah data dapat menyambung ke 2 relay ?
	
	Sumber 1 : slot data 1 - 20
	Sumber 2 : slot data 21 - 40
	
	set_data x nama aaabbbccc
			: memberikan nama pada data, ini akan ditampilkan di LCD
	
	set_data x alarm 2.43	
			: batas alarm, sehingga pada display dapat
			  memunculkan blink2 atau bintang2.
			
	set_data x ONR	A	
			: jika batas alarm dilewati, data akan trigger ON relay A 
	
	set_data x OFFR B
			: jika batas alarm dilewati, data akan trigger OFF relay B
	
	set_data x set/aktif 0/1
			: mengaktifkan sistem relay

	struct t_dt_set adalah setting yang disimpan pada flash
	
	
	*/
//#include "monita_uip.h"
#include "../monita/monita_uip.h"
//#include "uip.h"
#include "../GPIO/selenoid.c";
#include <stdio.h>
#include <stdlib.h>

#define debug		printf
//#define debug(...)	do{} while(0)

//static int simpan_data( struct t_dt_set *pgr);

int set_relay(int argc, char **argv)
{
	int i;
	unsigned char str_relay[24];
	int sumb=0;
	
	/* jika hanya cek_data
	 * maka akam ditampilkan semua 
	 * sekitar 400 items
	 */
	if (argc == 1)	{
		judul(" Data Setting\r\n");
	  //printf(" no.  : Nama       : Stat : Satuan : Alarm : Rly : &Memory\r\n");
		printf(" no.  : Nama       : Stat : Satuan : Alr_L : Alr_H : Rly : &Memory\r\n");
		garis_bawah();
	

	}
	else if (argc > 1)	{
		if (strcmp(argv[1], "help") == 0)	{
				printf(" Perintah untuk menampilkan setting data !\r\n");
				printf("    cek_data help  : untuk menampilkan ini.\r\n");
				printf("    cek_data       : menampilkan seluruh setting data.\r\n");
				printf("    cek_data 10    : manampikan data ke 10 sampai 30 (20 data saja).\r\n"); 
		}		
		else	{
			sprintf(str_relay, "%s", argv[1]);	
			sumb = cek_nomer_valid(str_relay, 400);
			if (sumb > 0 && sumb < 400)		{
				judul(" Data Setting\r\n");
				//printf(" no.  : Nama       : Stat : Satuan : Alarm : Rly : &Memory\r\n");
				printf(" no.  : Nama       : Stat : Satuan : Alr_L : Alr_H : Rly : &Memory\r\n");
				garis_bawah();
				
				sumb--;
		
				for (i = sumb; i< (sumb + 20); i++)
				{
					if (i >= (JML_SUMBER * PER_SUMBER)) break;
					
					printf(" (%3d): %10s :  %d   : %6s : %4.2f : %4.2f : %2d : (%X)\r\n", (i+1), \
						p_dt[i].nama, p_dt[i].aktif, p_dt[i].satuan, p_dt[i].alarm_L, \
						p_dt[i].alarm_H, p_dt[i].relay, &p_dt[i]);	
					
				}
			}
			else
				printf(" ERR: Perintah tidak dikenali !\r\n");
		}
	}
	
}
/*
static tinysh_cmd_t cek_data_cmd={0,"cek_data","menampilkan konfigurasi mesin","[] nomer",
                              cek_data,0,0,0};
//*/
