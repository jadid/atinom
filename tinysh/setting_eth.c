/*
	berkaiatan dengan sumber ip address 
	afrendy bayu
	daun biru engineering

	24 Sept 2010
	Setting data id via webserver
*/

#include "../monita/monita_uip.h"




#ifdef PAKAI_PM	
extern char * judulnya_pm[];	

			  
void cek_konfig(int argc, char **argv)

#else
void cek_konfig(void)
#endif
{
	int i=0,j=0;
	struct t_setting *konfig;
	int jmlData = (sizeof(data_f)/sizeof(float));
	konfig = (char *) ALMT_KONFIG;
	
	#ifdef PAKAI_PM
	int t=0;
	unsigned char buf[24];
	
	struct t_sumber_pm *p_sbr;
	p_sbr = (char *) ALMT_SUMBER;

	if (argc < 3) {
		printf("Perintah salah!!!\r\n");
		printf("Gunakan : \r\n");
		printf("cek_konfig alamat [alamat PM]\r\n");
		printf("cek_konfig no [urutan Sumber PM]\r\n");
		return;
	} else {
		if (strcmp(argv[1], "alamat") == 0)	{
			sprintf(buf, "%s", argv[2]);	

			t = cek_nomer_sumber(buf, 250);
			if (t > 0)	{
				for(j=0; j<JML_SUMBER; j++) {
					if (((int)p_sbr->pm[j].alamat)==t) {
						break;
					}
				}
				
				if (j==JML_SUMBER) {
					printf("Alamat PM tidak ditemukan\r\n");
					return;
				}
				
				if (p_sbr->pm[j].status==0) {
					printf("PM Alamat %d tidak aktif\r\n", j);
					return;
				}
				
				printf(" alamat: %d, index: %d\r\n", t, j);
			} else	{
				printf("Alamat tidak benar !!!\r\n");
				return;
			}	
		} else if (strcmp(argv[1], "no") == 0)	{
			sprintf(buf, "%s", argv[2]);	

			j = cek_nomer_sumber(buf, JML_SUMBER);
			if (j > 0)	{
				printf(" no sumber: %d\r\n", j);
				
			} else	{
				return;
			}	
		}
	}
	
	garis_bawah2();
	printf("Konfigurasi Sumber ke-%d: %s, Alamat: %d\r\n", j+1, p_sbr->pm[j].nama, p_sbr->pm[j].alamat);
	garis_bawah2();
	
	#endif
	printf("  No.    ID      Nama                  status\r\n");
	int w=1,z=0;
	for (i=0; i<45; i++)
		printf("-");
	printf("\r\n");
	
	#ifdef BOARD_KOMON_KONTER
	for (i=0; i<KANALNYA; i++)	{		
		//if (i>6) 
		{
			z++;
			printf(" (%2d): %4d : %-18s %2d : ", z, konfig[i*2].id, "Frek/RPM kanal", i+1);
			if (konfig[i*2].status == 0)
				printf("%-16s","Tidak Aktif");
			else if (konfig[i*2].status == 1)
				printf("%-16s","Aktif / Normal");
			else if (konfig[i*2].status == 2)
				printf("%-16s","TimeOut");
			else
				printf("%-16s"," ");
			printf("\r\n");
			
			z++;
			printf(" (%2d): %4d : %-18s %2d : ", z, konfig[i*2+1].id, "Pulsa konter kanal", i+1);
			if (konfig[i*2+1].status == 0)
				printf("%-16s","Tidak Aktif");
			else if (konfig[i*2+1].status == 1)
				printf("%-16s","Aktif / Normal");
			else if (konfig[i*2+1].status == 2)
				printf("%-16s","TimeOut");
			else
				printf("%-16s"," ");
			printf("\r\n");
		}
	}
	#else
	
	for (i=0; i<PER_SUMBER; i++)	{
			#ifdef PAKAI_PM
				printf(" (%2d): %4d : %-16s : ", (i+1), konfig[i].id, judulnya_pm[i]);
			#else
				printf(" (%2d): %4d : %-16s : ", (i+1), konfig[i].id, konfig[i].ket);			// rata kiri
			#endif
			
			/* status */
			if (konfig[i].status == 0)
				printf("%-16s","Tidak Aktif");
			else if (konfig[i].status == 1)
				printf("%-16s","Aktif / Normal");
			else if (konfig[i].status == 2)
				printf("%-16s","TimeOut");
			else
				printf("%-16s"," ");
			printf("\r\n");
	}
	#endif
}							 
//*
static tinysh_cmd_t cek_konfig_cmd={0,"cek_konfig","menampilkan konfig modul","[args]", cek_konfig,0,0,0};
//*/
void set_konfig(int argc, char **argv)
{
	unsigned char buf[24];
	int sumb;
	unsigned int ret_ip;
	int stat;
	int jmlData = (sizeof(data_f)/sizeof(float));
	
	judul(" Setting Konfig\r\n");
	
	if (argc < 4) 
	{
		if (argc > 1)
		{
			if (strcmp(argv[1], "help") == 0)
			{
				printf(" Setting konfig data\r\n");
				garis_bawah();
				printf(" argument : id, ket, status, help\r\n");
				printf("   help   : printout keterangan ini\r\n");
				printf("   default : load sumber dengan default ip & setting\r\n");
				printf(" \r\n");
				printf("   id : memberikan alamat id pada sumber titik tertentu\r\n");
				printf("     Power Meter tidak dipengaruhi setting ini\r\n");
				printf("     misalnya : $ set_konfig 4 id 19\r\n");
				printf("     artinya set setting nomer 4 dengan id 19\r\n");
				printf(" \r\n");
				printf("   ket : memberikan ket pada titik tertentu\r\n");
				printf("     misalnya : $ set_konfig 4 ket RTD_SWD#2\r\n");
				printf("     artinya sumber nomer 4 diberi ket RTD_SWD#2 (maks 10 karakter)\r\n");	
				printf(" \r\n");
				printf("   status : set pada sumber tertentu\r\n");
				printf("     status 0 = tidak aktif, 1 = aktif\r\n");
				printf("     aktif artinya sumber akan dikirim terus menerus !\r\n");			
				printf("     misalnya : $ set_konfig 4 status 1\r\n");
				printf("     artinya set nomer 4 supaya aktif\r\n");	
				printf(" \r\n");	
				return;
			} 
			else if (strcmp(argv[1], "default") == 0)
			{
				printf(" Set konfig dengan data default !\n");
				set_awal_konfig();
				
				return;
			}	
		}
		printf(" ERR: argument kurang !\r\n");
		printf(" coba set_konfig help \r\n");
		return;	
	}
	
  	display_args(argc,argv);
	
	/* copy dulu yang lama kedalam buffer */
	struct t_setting *p_sbr;
	printf("Jml Data : %d\r\n", jmlData);
	p_sbr = pvPortMalloc( jmlData * sizeof (struct t_setting) );
	
	if (p_sbr == NULL)	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		return;
	}
	printf(" %s(): Mallok ok di %X\r\n", __FUNCTION__, p_sbr);	
	memcpy((char *) p_sbr, (char *) ALMT_KONFIG, (jmlData * sizeof (struct t_setting)));
	
	
	if (strcmp(argv[2], "id") == 0)
	{
		sprintf(buf, "%s", argv[1]);	
		
		sumb = cek_nomer_sumber(buf, jmlData);
		if (sumb >= 0)	{
			printf(" sumber = %d\r\n", sumb);
			sprintf(buf, "%s", argv[3]);	
			stat = cek_nomer_sumber(buf, 32765);
			
			p_sbr[sumb-1].id = stat;
			printf("ID = %d\r\n", p_sbr[sumb-1].id);
			
		}
		else
		{
			vPortFree( p_sbr );
			return;
		}	
	}
	else if (strcmp(argv[2], "ket") == 0)
	{
		//printf("masuk sini\r\n");
		sprintf(buf, "%s", argv[1]);	
		sumb = cek_nomer_sumber(buf, jmlData);
		if (sumb > 0)		
		{
			printf(" sumber = %d : ", sumb);
			
			if (strlen(argv[3]) > 15)
			{
				printf("\n ERR: nama terlalu panjang !\r\n");
				vPortFree( p_sbr );

				return;
			}
			sprintf(p_sbr[sumb-1].ket, argv[3]);
			printf(" Nama : %s\r\n", p_sbr[sumb-1].ket); 
		}
		else
		{
			vPortFree( p_sbr );
			return;
		}	
	}
	else if (strcmp(argv[2], "status") == 0)
	{
		sprintf(buf, "%s", argv[1]);	
		sumb = cek_nomer_sumber(buf, jmlData);
		if (sumb > 0)		
		{
			printf(" sumber = %d : ", sumb);
			
			/* 0 tidak dipakai, 1 dipakai / diaktifkan , 5 daytime */
			sprintf(buf, "%s", argv[3]);	
			stat = cek_nomer_sumber(buf, 5);
			
			if (stat >=0)
			{
				p_sbr[sumb-1].status = stat;
				printf("ID %d : ", p_sbr[sumb-1].id); 
				if (stat == 0) printf("Tidak diaktifkan\r\n");
				else if (stat == 1) printf("Diaktifkan\r\n");	
			}
		}
		else 
		{
			vPortFree( p_sbr );
			return;
		}	
	}
	else
	{
		printf(" ERR: perintah tidak benar !\r\n");
		printf(" coba set_sumber help \r\n");
		
		vPortFree( p_sbr );			
		return;	
	}
	
	// SEMUA TRUE dan sampai disini
	if (simpan_konfig( p_sbr ) < 0)
	{
		vPortFree( p_sbr );
		return;
	}
	vPortFree( p_sbr );
}							 

//*
static tinysh_cmd_t set_konfig_cmd={0,"set_konfig","set konfig untuk beberapa hal","help id ket status",
                              set_konfig,0,0,0};
//*/
int cek_nomer_sumber(char *arg, int maks)
{
	unsigned char buf[24];
	int ss;
	
	sprintf(buf, "%s", arg);
	ss = baca_kanal(buf);
	
	if (ss > 0 && ss < (maks+1))
	{
		return ss;	
	}	
	//else if (ss == 0 || ss > maks)
	else if (ss == 0)
	{
		return 0;	
	}
	else if (ss > maks)
	{
		printf("\r\n ERR: %d diluar range !\r\n", ss);
		return -1;
	}
	else
	{
		printf("\r\n ERR: format salah !\r\n");
		return -2;
	}
}

void read_konfig(void)
{
	struct t_setting *setting;
	
	setting = (char *) ALMT_KONFIG;
	printf("%s(): OK\r\n", __FUNCTION__);		
}

void set_awal_konfig(void)	{
	int i;	
	struct t_setting *p_sbr;
	int jmlData = (sizeof(data_f)/sizeof(float));
	//judul(" Set Sumber ke Default\r\n");
	printf("Jml data: %d\r\n", jmlData);
	p_sbr = pvPortMalloc( jmlData * sizeof (struct t_setting) );
	if (p_sbr == NULL)	{
		printf("%s(): Err allok memory gagal !\r\n");
		return;
	}
	
	for (i=0; i<(PER_SUMBER*JML_SUMBER); i++)	{
		sprintf(p_sbr[i].ket, "-");
		p_sbr[i].id = 0;		/* default alamat = 0 : board Monita, PM = 1 s/d 247 / stack */
		p_sbr[i].status = 0;	

	}	
	
	if (simpan_konfig( p_sbr ) < 0)
	{
		vPortFree( p_sbr );
		return;
	}
	vPortFree( p_sbr );
	
}

 int simpan_konfig( struct t_setting *pgr)
{
	printf(" Save struct KONFIG ke flash ..");
	if(prepare_flash(SEKTOR_KONFIG, SEKTOR_KONFIG)) return -1;
	printf("..");
	
	if(hapus_flash(SEKTOR_KONFIG, SEKTOR_KONFIG)) return -1;
	printf("..");
	
	if(prepare_flash(SEKTOR_KONFIG, SEKTOR_KONFIG)) return -1;
	printf("..");
	
	if(tulis_flash(ALMT_KONFIG, (unsigned short *) pgr, (sizeof (struct t_setting) * (sizeof(data_f)/sizeof(float))))) return -1;
	
	printf(".. OK\r\n");
	return 0;
}
