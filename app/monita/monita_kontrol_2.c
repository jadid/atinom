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
#include "monita_uip.h"
#include "uip.h"
#include <stdio.h>
#include <stdlib.h>

#define debug		printf
//#define debug(...)	do{} while(0)





//float data_sumber[ JML_SUMBER ][ PER_SUMBER ];
float data_f [ (JML_SUMBER * PER_SUMBER) ];

static int simpan_data( struct t_dt_set *pgr);
/*	
int reset_data_sumber(void)
{
	int i;
	int y;
	
	for (i=0; i< JML_SUMBER; i++)
	{
		for (y=0; y< PER_SUMBER; y++)			 
			 data_sumber[i][y] = 0.00;
	}
}*/
int cek_data(int argc, char **argv)
{
	int i;
	unsigned char buf[24];
	int sumb=0;
	
	/* jika hanya cek_data
	 * maka akam ditampilkan semua 
	 * sekitar 400 items
	 */
	if (argc == 1)
	{
		struct t_dt_set *p_dt;
		p_dt = (char *) ALMT_DT_SET;
	
		judul(" Data Setting\r\n");
	  //printf(" no.  : Nama       : Stat : Satuan : Alarm : Rly : &Memory\r\n");
		printf(" no.  : Nama       : Stat : Satuan : Alr_L : Alr_H : Rly : &Memory\r\n");
		garis_bawah();
	
		for (i=0; i< (JML_SUMBER * PER_SUMBER); i++)
		{
			printf(" (%3d): %10s :  %d   : %6s : %4.2f : %4.2f : %2d : (%X)\r\n", (i+1), \
				p_dt[i].nama, p_dt[i].aktif, p_dt[i].satuan, p_dt[i].alarm_L, \
				p_dt[i].alarm_H, p_dt[i].relay, &p_dt[i]);	
		}
	}
	else if (argc > 1)
	{
		if (strcmp(argv[1], "help") == 0)
		{
				printf(" Perintah untuk menampilkan setting data !\r\n");
				printf("    cek_data help  : untuk menampilkan ini.\r\n");
				printf("    cek_data       : menampilkan seluruh setting data.\r\n");
				printf("    cek_data 10    : manampikan data ke 10 sampai 30 (20 data saja).\r\n"); 
		}		
		else
		{
			sprintf(buf, "%s", argv[1]);	
			sumb = cek_nomer_valid(buf, 400);
			if (sumb > 0 && sumb < 400)		
			{
				struct t_dt_set *p_dt;
				p_dt = (char *) ALMT_DT_SET;
		
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

static tinysh_cmd_t cek_data_cmd={0,"cek_data","menampilkan konfigurasi mesin","[] nomer",
                              cek_data,0,0,0};

static int set_data_default(void)
{
	int i;
	struct t_dt_set *p_gr;
	
	//judul(" Set Group ke Default\r\n");
	
	p_gr = pvPortMalloc( (JML_SUMBER * PER_SUMBER) * sizeof (struct t_dt_set) );
	if (p_gr == NULL)
	{
		printf("%s(): Err allok memory gagal !\r\n", __FUNCTION__);
		return -1;
	}
	
	for (i=0; i< (JML_SUMBER * PER_SUMBER) ; i++)
	{
		sprintf(p_gr[i].nama, "dd_%d", (i+1));
		//p_gr[i].ID_group = (i+1);
		//p_gr[i].stat = 0;			// pasif/unset
		//sprintf(p_gr[i].ket, "--");
		sprintf(p_gr[i].satuan, "-");
		p_gr[i].relay = 0;
		p_gr[i].alarm_L = 0;
		p_gr[i].alarm_H = 10.0;
		p_gr[i].aktif = 0;
	}
	
	if (simpan_data( p_gr ) < 0)
	{
		vPortFree( p_gr );
		return -1;
	}
	vPortFree( p_gr );
	
}

int set_data(int argc, char **argv)
{
	unsigned char buf[24];
	int sumb=0;
	unsigned int ret_ip;
	struct t_dt_set *p_dt;
	
	judul(" Setting Data\r\n");
	
	if (argc < 4) 
	{
		if (argc > 1)
		{
			if (strcmp(argv[1], "help") == 0)
			{
				printf(" Perintah untuk setting group !\r\n");
				printf(" 1. set_group help/default\r\n");
				printf("    help    : printout keterangan ini\r\n");
				printf("    default : memberikan default setting group\r\n");
				printf("\r\n");
				
				printf(" 2. set_group x [opt1] [opt2]\r\n");
				printf("    x    : nomer group\r\n");
				printf("    opt1 : nama, set/aktif, desc/ket\r\n");
				printf("\r\n");
				printf("    [opt1]\r\n");				
				printf("    nama     : memberikan nama group yang akan ditampilkan\r\n");
				printf("    misalnya : $ set_group 2 nama GMT_#4\r\n");
				printf("    artinya memberikan nama group 1 dengan GMT_#4\r\n");
				printf("\r\n");
				printf("    set/aktif : mengaktif/nonaktifkan group di display\r\n");
				printf("    misalnya  : $ set_group 4 aktif 1\r\n");
				printf("    artinya mengaktifkan group 4\r\n");
				printf("    misalnya  : $ set_group 4 aktif 0\r\n");
				printf("    artinya me-nonaktifkan group 4\r\n");
				printf("\r\n");
				printf("    desc/ket  : memberikan keterangan/deskripsi pada group\r\n");
				printf("    misalnya  : $ set_group 7 ket ini_milik_Sulzer\r\n");
				printf("    catatan   : jangan ada spasi pada keterangannya !\r\n");
				
				return ;
			}
			else if (strcmp(argv[1], "default") == 0)
			{
				printf("set DATA dengan data default !\n");
				set_data_default();
				
				return;
			}		
		}
		printf(" ERR: argument kurang !\r\n");
		printf(" coba set_group help \r\n");
		return;	
	}
	
  	display_args(argc,argv);
	
	/* copy dulu yang lama kedalam buffer */
	p_dt = pvPortMalloc( (PER_SUMBER * JML_SUMBER) * sizeof (struct t_dt_set) );
	
	if (p_dt == NULL)
	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		return -1;
	}
	printf(" %s(): Mallok ok di %X\r\n", __FUNCTION__, p_dt);
	
	memcpy((char *) p_dt, (char *) ALMT_DT_SET, ((PER_SUMBER * JML_SUMBER) * sizeof (struct t_dt_set)));
	
	/* argumen ke dua adalah nama, argumen pertama adalah nomer */
	if (strcmp(argv[2], "nama") == 0)
	{
		sprintf(buf, "%s", argv[1]);	
		sumb = cek_nomer_valid(buf, (PER_SUMBER * JML_SUMBER));
		if (sumb > 0)		
		{
			printf(" Group %d : nama : %s\r\n", sumb, argv[3]);			
			if (strlen(argv[3]) > 16)
			{
				printf(" ERR: nama terlalu panjang (Maks 16 karakter)!\r\n");
				vPortFree( p_dt );
				return;
			}
			sprintf(p_dt[sumb-1].nama, argv[3]);	
		}
	}
	if (strcmp(argv[2], "satuan") == 0)
	{
		sprintf(buf, "%s", argv[1]);	
		sumb = cek_nomer_valid(buf, (PER_SUMBER * JML_SUMBER));
		if (sumb > 0)		
		{
			printf(" Group %d : satuan : %s\r\n", sumb, argv[3]);			
			if (strlen(argv[3]) > 8)
			{
				printf(" ERR: satuan terlalu panjang (Maks 8 karakter)!\r\n");
				vPortFree( p_dt );
				return;
			}
			sprintf(p_dt[sumb-1].satuan, argv[3]);	
		}
	}
	/*
	else if ((strcmp(argv[2], "set") == 0) || (strcmp(argv[2], "aktif") == 0))
	{
		sprintf(buf, "%s", argv[1]);	
		sumb = cek_nomer_valid(buf, 10);
		if (sumb > 0)		
		{
			printf(" Group %d : set : %s\r\n", sumb, argv[3]);	
			if (( argv[3][0] == '1') || (argv[3][0] == '0'))
			{
				p_gr[sumb - 1].stat = (argv[3][0] - '0');
			}
			else
			{
				printf(" ERR: pilih set 1 atau 0 !\r\n");
				vPortFree( p_gr );
				return;
			} 
		}
	}
	else if ((strcmp(argv[2], "ket") == 0) || (strcmp(argv[2], "desc") == 0))
	{
		sprintf(buf, "%s", argv[1]);	
		sumb = cek_nomer_valid(buf, 10);
		if (sumb > 0)		
		{
			printf(" Group %d : ket : %s\r\n", sumb, argv[3]);			
			if (strlen(argv[3]) > 32)
			{
				printf(" ERR: ket terlalu panjang (Maks 32 karakter)!\r\n");
				vPortFree( p_gr );
				return;
			}
			sprintf(p_gr[sumb-1].ket, argv[3]);	
		}
	}
	else
	{
		printf(" ERR: perintah tidak benar !\r\n");
		printf(" coba set_mesin help \r\n");
		vPortFree( p_gr );
		return;
	}
	*/
	
	/* cek apakah pemeriksaan angka valid */
	if (sumb <= 0)
	{
		vPortFree( p_dt );
		return ;	
	}
	
	// SEMUA TRUE dan sampai disini
	if (simpan_data( p_dt ) < 0)
	{
		vPortFree( p_dt );
		return -1;
	}
	vPortFree( p_dt );

}

static tinysh_cmd_t set_data_cmd={0,"set_data","menampilkan konfigurasi mesin","help default nama ket",
                              set_data,0,0,0};
							  
static int simpan_data( struct t_dt_set *pgr)
{
	printf(" Save struct DATA_SET ke flash ..");
	if(prepare_flash(SEKTOR_DT_SET, SEKTOR_DT_SET)) return -1;
	printf("..");
	
	if(hapus_flash(SEKTOR_DT_SET, SEKTOR_DT_SET)) return -1;
	printf("..");
	
	if(prepare_flash(SEKTOR_DT_SET, SEKTOR_DT_SET)) return -1;
	printf("..");
	
	if(tulis_flash(ALMT_DT_SET, (unsigned short *) pgr, (sizeof (struct t_dt_set) * (PER_SUMBER * JML_SUMBER) ))) return -1;
	
	printf(".. OK\r\n");
	return 0;
}
