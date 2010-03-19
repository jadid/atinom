/*
	berkaitan dengan data2 mesin
	12 Jan 09
	furkan jadid
	daun biru engineering
	*/

#include "../monita/monita_uip.h"
void cek_titik(int no_mes);

extern struct t_mesin mesin[JML_MESIN];

							  
void cek_mesin(int argc, char **argv)
{
	int i;
	unsigned char buf[24];
	int no_mesin;
	
	printf(" Mesin data");
	if (argc < 2) 
	{
		printf(" global\r\n");
		garis_bawah();
	
		for (i=0; i<10; i++)
		{
			printf(" (%2d): %10s\r\n", (i+1), mesin[i].nama);	
		}
		return;	
	}
	else
	{
		// cek titik dari nomer mesin yang dimasukkan
		sprintf(buf, "%s", argv[1]);	
		no_mesin = cek_nomer_sumber(buf, 10);
		if (no_mesin > 0)		
		{
			printf(" pada mesin = %d\r\n", no_mesin);
			
			cek_titik((int) no_mesin);
		}
		else return;
			
	}
	
}							 

static tinysh_cmd_t cek_mesin_cmd={0,"cek_mesin","menampilkan konfigurasi mesin","[] nomer",
                              cek_mesin,0,0,0};


void set_mesin(int argc, char **argv)
{
	unsigned char buf[24];
	int sumb;
	unsigned int ret_ip;
	
	if (argc < 4) 
	{
		if (argc > 1)
		{
			if (strcmp(argv[1], "help") == 0)
			{
				printf(" Setting mesin yang akan ditampilkan\r\n");
				garis_bawah();
				printf(" argument : help, default, nama\r\n");
				printf("   help   : printout keterangan ini\r\n");
				printf("   default : load default setting mesin\r\n");
				printf(" \r\n");
				printf("   nama : memberikan nama mesin yang akan ditampilkan\r\n");
				printf("     misalnya : $ set_mesin nama 1 GMT_#4\r\n");
				printf("     artinya memberikan nama mesin 1 dengan GMT_#4\r\n");
			
				return ;
			}
			else if (strcmp(argv[1], "default") == 0)
			{
				printf("set mesin dengan data default !\n");
				set_awal_mesin();
				
				return;
			}		
		}
		printf(" ERR: argument kurang !\r\n");
		printf(" coba set_mesin help \r\n");
		return;	
	}
	
	printf(" set_mesin %s dipanggil\r\n", argv[1]);
	garis_bawah();
  	display_args(argc,argv);
	
	if (strcmp(argv[1], "nama") == 0)
	{
		sprintf(buf, "%s", argv[2]);	
		sumb = cek_nomer_sumber(buf, 10);
		if (sumb > 0)		
		{
			printf(" mesin = %d : ", sumb);
			
			if (strlen(argv[3]) > 10)
			{
				printf("nama terlalu panjang !\r\n");
				return;
			}
			sprintf(mesin[sumb-1].nama, argv[3]);
			printf(" Nama : %s\r\n", mesin[sumb-1].nama); 
		}
		else return ; 	
	}
	else
	{
		printf(" ERR: perintah tidak benar !\r\n");
		printf(" coba set_mesin help \r\n");
		return;
	}
}							 

static tinysh_cmd_t set_mesin_cmd={0,"set_mesin","nama","[args]",
                              set_mesin,0,0,0};


void save_mesin(void)
{
	printf("Save struct mesin ke flash ..");
	if(prepare_flash(SEKTOR_MESIN, SEKTOR_MESIN)) return;
	printf("..");
	
	if(hapus_flash(SEKTOR_MESIN, SEKTOR_MESIN)) return;
	printf("..");
	
	if(prepare_flash(SEKTOR_MESIN, SEKTOR_MESIN)) return;
	printf("..");
	
	if(tulis_flash(ALMT_MESIN, (unsigned short *) &mesin, sizeof (mesin))) return;
	printf(".. OK\r\n");
	
}

static tinysh_cmd_t save_mesin_cmd={0,"save_mesin","menyimpan konfigurasi mesin ke flash","[args]",
                              save_mesin,0,0,0};


void read_mesin(void)
{
	taskENTER_CRITICAL();
	memcpy((char *)&mesin, (char *) ALMT_MESIN, sizeof (mesin));
	taskEXIT_CRITICAL();		
}

void set_awal_mesin(void)
{
	sprintf(mesin[0].nama, "Msn #1");
	sprintf(mesin[1].nama, "Msn #2");
	sprintf(mesin[2].nama, "Msn #3");
	sprintf(mesin[3].nama, "Msn #4");
	sprintf(mesin[4].nama, "Msn #5");
	sprintf(mesin[5].nama, "Msn #6");
	sprintf(mesin[6].nama, "Msn #7");
	sprintf(mesin[7].nama, "Msn #8");
	sprintf(mesin[8].nama, "Msn #9");
	sprintf(mesin[9].nama, "Msn #10");
	
	mesin[0].ID_mesin = 1;
	mesin[1].ID_mesin = 2;
	mesin[2].ID_mesin = 3;
	mesin[3].ID_mesin = 4;
	mesin[4].ID_mesin = 5;	
	mesin[5].ID_mesin = 6;
	mesin[6].ID_mesin = 7;
	mesin[7].ID_mesin = 8;
	mesin[8].ID_mesin = 9;
	mesin[9].ID_mesin = 10;		
}
