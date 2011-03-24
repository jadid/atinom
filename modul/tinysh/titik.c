/*
	berkaitan dengan titik
	12 jan 09
	furkan jadid
	daun biru engineering  
	
	setiap mesin dialokasikan 70 titik
	sehingga jika ada 10 mesin, diperlukan 
	sekitar 10 * 70 * 2 byte = 1400 byte untuk struknya
	dan 10 * 70 * 4 byte (float) = 2800 byte untuk datanya
	
*/

#include "../monita/monita_uip.h"
extern struct t_mesin mesin[];
extern struct t_titik titik[TIAP_MESIN * JML_MESIN];

/* keterangan masing2 titik lihat di menu_monita.c */
extern char keter[100][25];

char *f_out(float ff);
							  
void cek_titik(int no_mes)
{
	int i;
	int pasnya;
	
	printf(" No. Sbr. Knl. Modul    Data : Keterangan\r\n");
	
	for (i=0; i<55; i++)
		printf("-");
	
	printf("\r\n");
	
	pasnya = TIAP_MESIN * (no_mes - 1);

	for (i=0; i<60; i++)
	{
		
		printf("%3d. %3d. %3d. %3d. %9s: %s\r\n", pasnya+1, titik[pasnya].ID_sumber, \
			titik[pasnya].kanal, titik[pasnya].alamat, f_out(titik[pasnya].data), \
			keter[i]);
		
		pasnya++;	
	}

#if 0	
	printf("%3d. %3d. %3d. %3d. %9s: Charge air pressure kiri\r\n", \ 			
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, titik[pasnya].alamat, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %3d. %9s: Charge air pressure kanan\r\n", \			
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, titik[pasnya].alamat, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %3d. %9s: Charge air temperature kiri\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, titik[pasnya].alamat, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %3d. %9s: Charge air temperature kanan\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	
	printf("%3d. %3d. %3d. %3d. %9s: Jacket water inlet pressure 1\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %3d. %9s: Jacket water inlet pressure 2\r\n", \		
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %3d. %9s: Jacket water outlet pressure 1\r\n", \	
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %3d. %9s: Jacket water outlet pressure 2\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %3d. %9s: Jacket water inlet temperature 1\r\n", \	
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %3d. %9s: Jacket water inlet temperature 2\r\n", 	\
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %3d. %9s: Jacket water outlet temperature 1\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %3d. %9s: Jacket water outlet temperature 2\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));

	printf("%3d. %3d. %3d. %3d. %9s: Lub oil inlet pressure 1\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %3d. %9s: Lub oil inlet pressure 2\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %3d. %9s: Lub oil outlet pressure 1\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));	
	printf("%3d. %3d. %3d. %3d. %9s: Lub oil outlet pressure 2\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %3d. %9s: Lub oil inlet temperature 1\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %3d. %9s: Lub oil inlet temperature 2\r\n", 	\
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %3d. %9s: Lub oil outlet temperature 1\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %3d. %9s: Lub oil outlet temperature 2\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));

	for (i=0; i<20; i++)
	{
		printf("%3d. %3d. %3d. %3d. %9s: Exhaust temperature %d\r\n",  \
			pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data), (i+1));
	}
	
	for(i=0; i<5; i++)
	{
		printf("%3d. %3d. %3d. %3d. %9s: --\r\n", \
			pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	}
	
	printf("%3d. %3d. %3d. %3d. %9s: Engine rpm \r\n" , 	\
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %3d. %9s: Turbo rpm 1\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %3d. %9s: Turbo rpm 2\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %3d. %9s: Fuel inlet counter\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %3d. %9s: Fuel outlet counter\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %3d. %9s: Power (kW)\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %3d. %9s: Energi (kWh)\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %3d. %9s: Voltage (V)\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %3d. %9s: Ampere (A)\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));	
		
	printf("cek = %d\r\n", pasnya);
#endif	
	
	
}							 
/*
static tinysh_cmd_t cek_titik_cmd={0,"cek_titik","menampilkan konfigurasi titik","[args]",
                              cek_titik,0,0,0};
*/

char *f_out(float ff)
{
	char tek[64];
	sprintf(tek, "%5.3f", ff);	
	
	return tek;	
}


void set_titik(int argc, char **argv)
{
	unsigned char buf[24];
	int sumb;
	int kanal;
	int ttk;
	int almt;
	
	if (argc < 4) 
	{
		if (argc > 1)
		{
			if (strcmp(argv[1], "help") == 0)
			{
				printf(" Setting titik yang akan ditampilkan\r\n");
				garis_bawah();
				printf(" argument : help, default, a b c d\r\n");
				printf("   help   : printout keterangan ini\r\n");
				printf("   default : load default setting titik\r\n");
				printf("\r\n");
				printf("   misalnya : $ set_titik a b c d\r\n");
				printf("     a nomer titik, b nomer sumber, c kanal, d alamat/stack modul\r\n");
				printf("     jika alamat kosong, default = 1\r\n");
				printf("\r\n");
				printf("   Catatan :\r\n");
				printf("   Satu mesin dialokasikan 100 titik, sehingga mesin 1 akan\r\n"); 
				printf("   mencakup titik 1 sampai titik 100, sedangkan mesin 2 akan\r\n"); 
				printf("   mencakup titik 101 sampai dengan 200, dst.\r\n");
				printf("\r\n");
				printf("   Titik 1 selalu mempunyai keterangan yang sama dengan titik 101,\r\n");
				printf("   misalnya, Titik 1 = Charge air pressure kiri, maka titik 101\r\n");
				printf("   juga Charge air pressure kiri, tetapi untuk mesin #2\r\n");
				printf("   begitu seterusnya.\r\n");
				printf("\r\n");
				printf("   Untuk disable, cukup set_titik dengan nomer sumber = 0 dst.\r\n");
				
				return;
			} 
			else if (strcmp(argv[1], "default") == 0)
			{
				printf("set titik dengan data default !\n");
				set_awal_titik();
				
				return;
			}	
		}
		printf(" ERR: argument kurang !\r\n");
		printf(" coba set_titik help \r\n");
		return;	
	}
	
	printf(" set_titik %s dipanggil\r\n", argv[1]);
	garis_bawah();
  	display_args(argc,argv);
	
	sprintf(buf, "%s", argv[1]);	
	ttk = cek_nomer_sumber(buf, (TIAP_MESIN * JML_MESIN));
	if (ttk > 0)
	{
		sprintf(buf, "%s", argv[2]);	
		sumb = cek_nomer_sumber(buf, JML_SUMBER);
		if (sumb >= 0)
		{
			if (sumb == 0)
			{
				printf("Titik %d tidak diaktifkan\r\n", ttk);
				titik[ttk - 1].ID_sumber = sumb;
				//titik[ttk - 1].kanal = kanal;
				
				return;
			}
			
			sprintf(buf, "%s", argv[3]);	
			kanal = cek_nomer_sumber(buf, JML_KANAL);			
			if (kanal > 0)
			{
				titik[ttk - 1].ID_sumber = sumb;
				titik[ttk - 1].kanal = kanal;
			
				/* argument ke 4 adalah alamat */
				if (argc >= 5)
				{
					sprintf(buf, "%s", argv[4]);	
					almt = cek_nomer_sumber(buf, 10);
					
					if (almt <= 0) almt = 1;
				}
				else
					almt = 1;
				
				titik[ttk - 1].alamat = almt;
				printf("Set titik %d, pada sumber %d, pada kanal %d, pada alamat %d\r\n", \
				ttk, sumb, kanal, almt); 
			}
			else
				printf(" ERR: Argumen nomer kanal salah !\r\n");
				//printf(" ERR !\r\n");
		}
		else
			printf(" ERR: Argumen nomer sumber salah !\r\n"); 
	}
	else
		printf(" ERR: Argumen nomer titik salah !\r\n");
		
	return;
}							 

static tinysh_cmd_t set_titik_cmd={0,"set_titik","set konfigurasi titik","(titik sumber kanal alamat) default",
                              set_titik,0,0,0};


void save_titik(void)
{
	int i;
	
	/* 	
		data titik harus dinol kan dulu, supaya tidak ikut 
		di hard coded ke flash 
	*/
	for (i=0; i<(TIAP_MESIN * JML_MESIN); i++)
		titik[i].data = (float) 0.0;
	
	printf("Save struct titik ke flash ..");
	if(prepare_flash(SEKTOR_TITIK, SEKTOR_TITIK)) return;
	printf("..");
	
	if(hapus_flash(SEKTOR_TITIK, SEKTOR_TITIK)) return;
	printf("..");
	
	if(prepare_flash(SEKTOR_TITIK, SEKTOR_TITIK)) return;
	printf("..");
	
	if(tulis_flash(ALMT_TITIK, (unsigned short *) &titik, sizeof (titik))) return;
	printf(".. OK\r\n");
}


static tinysh_cmd_t save_titik_cmd={0,"save_titik","menyimpan konfigurasi titik ke flash","[args]",
                              save_titik,0,0,0};

void read_titik(void)
{
	taskENTER_CRITICAL();
	memcpy((char *)&titik, (char *) ALMT_TITIK, sizeof (titik));
	taskEXIT_CRITICAL();		
}


void set_awal_titik(void)
{
	int i;
	
	for (i=0; i<(TIAP_MESIN * JML_MESIN) ; i++)
	{
		titik[i].ID_sumber = 0;
		titik[i].kanal = 0;
		titik[i].alamat = 1;
		titik[i].data = 0.00;
	}	
	
}
