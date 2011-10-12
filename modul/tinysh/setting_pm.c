/*
	afrendy bayu
	daun biru engineering

	6 Okt 2010
	Setting pm di modul monita
*/

//#include "../monita/monita_uip.h"
#include "monita/monita_uip.h"

#ifdef PAKAI_PM

void ket(char* nama, char no) {
	char s[20];
	switch (no) {
		case 0:
			sprintf(nama,"Power Meter");		// 
			//printf("Power Meter");
			break;
		case 1:
			sprintf(nama,"KTA");
			//printf("KTA");
			break;
		default:
			sprintf(nama,"-");
			//printf("-");
			break;
	}
}

void cek_setting_pmnyax(int nox)	{
	int i;
	char s[20];
	struct t_sumber_pm *pmx;
	pmx = (char *) ALMT_SUMBER;
	
	if (pmx->status==0) {
		printf("Status Request Modul Modbus tidak aktif\r\n");
	} else if (pmx->status==1) {
		printf("Status Request Modul Modbus Aktif\r\n");
	}
	
	printf("======================================================================\r\n");
	printf("  No. Alamat     Modul     Nama                           Status\r\n");
	printf("----------------------------------------------------------------------\r\n");
	
	for (i=0; i<JML_SUMBER; i++)	{
		ket(s, pmx->pm[i].modul);
		printf(" (%2d): %3d : %-12s  %-24s : ", (i+1), pmx->pm[i].alamat, s, pmx->pm[i].nama);			// rata kiri
		
		/* status */
		if (pmx->pm[i].status == 1)
			printf("%-16s","Aktif / Normal");
		else
			printf("%-16s","Tidak Aktif");
		printf("\r\n");
	}
}							 
//*
static tinysh_cmd_t cek_konfig_pmnya_cmd={0,"cek_pm_konfig","menampilkan konfig modul","[args]", cek_setting_pmnyax,0,0,0};

void cek_pm(int argc, char **argv) {
	int i=0,j=0,sumb=0;
	unsigned char string_pm[24];
	
	if (argc<2) {
		printf("Perintah salah !!!\r\n");
		printf("Gunakan : cek_pm_data [alamat PM]\r\n");
		return;
	}
	
	sprintf(string_pm, "%s", argv[1]);	
	sumb = cek_nomer_sumber(string_pm, 250);
	
	struct t_sumber_pm *p_sbr;
	p_sbr = (char *) ALMT_SUMBER;
	
	if (sumb > 0)	{
		for(j=0; j<JML_SUMBER; j++) {
			//printf("i: %d, j: %d, alm: %d, status %d\r\n", i, j, p_sbr->pm[j].alamat, p_sbr->pm[j].status);
			if (((int)p_sbr->pm[j].alamat)==sumb) {
				break;
			}
		}
		
		if (j==JML_SUMBER) {
			printf("Alamat PM tidak ditemukan\r\n");
			return;
		}
		
		if (p_sbr->pm[j].status==0) {
			printf("PM Alamat %d tidak aktif\r\n", sumb);
			return;
		}
			
		printf("Nilai Parameter Power Meter alamat %d\r\n", sumb);
		garis_bawah2();
	} else {
		printf("Alamat PM tidak ditemukan\r\n");
		return;
	}

	printf("   No  :    Parameter    :      Nilai     :  Satuan\r\n", i, judulnya_pm[i], data_f[j*PER_SUMBER+i], satuannya_pm[i]);
	garis_bawah();
	for(i=0; i<PER_SUMBER; i++) {
		printf(" (%3d) : %-15s : %14.2f    %s\r\n", i+1, judulnya_pm[i], data_f[j*PER_SUMBER+i], (satuannya_pm[i]!=NULL)?satuannya_pm[i]:" ");		// 41
	}

}
	
static tinysh_cmd_t cek_pm_cmd={0,"cek_pm_data","mengecek nilai parameter PM","[args]", cek_pm,0,0,0};

void set_pm(int argc, char **argv)
{
	unsigned char string_pm[24];
	int sumb;
	unsigned int ret_ip;
	int stat;
	//int jmlData = (sizeof(data_f)/sizeof(float));
	
	judul(" Setting PM\r\n");
	
	if (argc < 4) 	{
		if (argc > 1)	{
			if (strcmp(argv[1], "help") == 0)	{
				printf(" Setting konfig PM\r\n");
				garis_bawah();
				printf(" argument : id, nama, status, help\r\n");
				printf("   help   : printout keterangan ini\r\n");
				printf("   default : load sumber dengan default ip & setting\r\n");
				printf(" \r\n");
				printf("   alamat : memberikan alamat pada sumber Power Meter\r\n");
				printf("     misalnya : $ set_pm 4 alamat 19\r\n");
				printf("     artinya set koneksi sumber nomer 4 ke Power Meter beralamat 19\r\n");
				printf(" \r\n");
				printf("   nama : memberikan nama pada sumber Power Meter\r\n");
				printf("     misalnya : $ set_pm 1 nama PM_M#2\r\n");
				printf("     artinya set koneksi sumber nomer 1 diberi nama PM_M#2 (maks 16 karakter)\r\n");	
				printf(" \r\n");
				printf("   status : status pengambilan data sumber Power Meter\r\n");
				printf("     status 0 = tidak aktif, 1 = aktif\r\n");
				printf("     aktif artinya sumber akan meminta data ke Power Meter terus menerus !\r\n");			
				printf("     misalnya : $ set_pm 3 status 1\r\n");
				printf("     artinya set nomer 3 supaya aktif meminta data Power Meter\r\n");	
				printf(" \r\n");	
				return;
			} else if (strcmp(argv[1], "default") == 0)	{
				printf(" Set konfig dengan data default !\n");
				set_awal_pm();
				return;
			} else if (strcmp(argv[1], "status") == 0)	{
				struct t_sumber_pm *p_sbrx;
				p_sbrx = pvPortMalloc( sizeof (struct t_sumber_pm) );
				
				if (p_sbrx == NULL)	{
					printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
					vPortFree( p_sbrx );
					return;
				}
				printf(" %s(): Mallok ok di %X\r\n", __FUNCTION__, p_sbrx);	
				memcpy((char *) p_sbrx, (char *) ALMT_SUMBER, (sizeof (struct t_sumber_pm)));
				
				
				if (( argv[2][0] == '1') || (argv[2][0] == '0')) {
					p_sbrx->status = (argv[2][0] - '0');
				} else if ( strcmp(argv[2], "aktif")==0 || strcmp(argv[2], "hidup")==0 ) {
					p_sbrx->status = 1;
				} else if ( strcmp(argv[2], "mati")==0 ) 	{
					p_sbrx->status = 0;
				} else {
					printf(" Setting PM masih salah\r\n");	
					vPortFree( p_sbrx );
					return;
				}
				
				if (simpan_titik_pm( p_sbrx ) < 0)	{
					vPortFree( p_sbrx );
					return;
				}
				vPortFree( p_sbrx );
				return;
			}
		}
		printf(" ERR: argument kurang !\r\n");
		printf(" coba set_pm help \r\n");
		return;	
	}
	
  	display_args(argc,argv);
	
	/* copy dulu yang lama kedalam buffer */
	struct t_sumber_pm *p_sbr;
	//printf("Jml Data : %d\r\n", jmlData);
	p_sbr = pvPortMalloc( sizeof (struct t_sumber_pm) );
	
	if (p_sbr == NULL)	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		vPortFree( p_sbr );
		return;
	}
	printf(" %s(): Mallok ok di %X\r\n", __FUNCTION__, p_sbr);	
	memcpy((char *) p_sbr, (char *) ALMT_SUMBER, (sizeof (struct t_sumber_pm)));
	
	
	if (strcmp(argv[2], "alamat") == 0) 	{
		sprintf(string_pm, "%s", argv[1]);	
		
		sumb = cek_nomer_sumber(string_pm, JML_SUMBER);
		if (sumb > 0)		{
			printf(" sumber = %d\r\n", sumb);
			sprintf(string_pm, "%s", argv[3]);	
			stat = cek_nomer_sumber(string_pm, 250);		// batas max alamat Modbus
			
			p_sbr->pm[sumb-1].alamat = stat;
			printf("Alamat = %d\r\n", p_sbr->pm[sumb-1].alamat);	
		}
		else	{
			vPortFree( p_sbr );
			return;
		}	
	}
	else if (strcmp(argv[2], "nama") == 0)	{
		sprintf(string_pm, "%s", argv[1]);	
		sumb = cek_nomer_sumber(string_pm, PER_SUMBER);
		if (sumb > 0)		{
			printf(" sumber = %d : ", sumb);
			
			if (strlen(argv[3]) > 16)	{
				printf("\n ERR: nama terlalu panjang !\r\n");
				vPortFree( p_sbr );
				return;
			}
			sprintf(p_sbr->pm[sumb-1].nama, argv[3]);
			printf(" Nama : %s\r\n", p_sbr->pm[sumb-1].nama); 
		}
		else {
			vPortFree( p_sbr );
			return;
		}	
	}
	else if (strcmp(argv[2], "modul") == 0)	{
		sprintf(string_pm, "%s", argv[1]);	
		sumb = cek_nomer_sumber(string_pm, PER_SUMBER);
		if (sumb > 0)		{
			/* 0 tidak dipakai, 1 dipakai / diaktifkan */
			if (( argv[3][0] == '1') || (argv[3][0] == '0')) {
				p_sbr->pm[sumb-1].modul = (argv[3][0] - '0');
			}
		}
		else {
			vPortFree( p_sbr );
			return;
		}	
	}
	else if (strcmp(argv[2], "status") == 0)	{
		sprintf(string_pm, "%s", argv[1]);	
		sumb = cek_nomer_sumber(string_pm, PER_SUMBER);
		if (sumb > 0)	{
			printf(" sumber = %d : ", sumb);
			/* 0 tidak dipakai, 1 dipakai / diaktifkan */
			if (( argv[3][0] == '1') || (argv[3][0] == '0')) {
				p_sbr->pm[sumb-1].status = (argv[3][0] - '0');
			} 
			else if (strcmp(argv[3], "aktif")==0) {
				p_sbr->pm[sumb-1].status = 1;
			} else if (strcmp(argv[3], "mati")==0)  {
				p_sbr->pm[sumb-1].status = 0;
			} else {
				p_sbr->pm[sumb-1].status = 0;
			}
		}
		else {
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
	if (simpan_titik_pm( p_sbr ) < 0)	{
		vPortFree( p_sbr );
		return;
	}
	vPortFree( p_sbr );
}							 

static tinysh_cmd_t set_pm_cmd={0,"set_pm","set PM","help alamat nama status", set_pm,0,0,0};


void set_awal_pm(void) {
	int i;	
	struct t_sumber_pm *p_sbr;
	
	p_sbr = pvPortMalloc(sizeof (struct t_sumber_pm) );
	if (p_sbr == NULL) 	{
		printf("%s(): Err allok memory gagal !\r\n");
		vPortFree( p_sbr );
		return;
	}
	
	p_sbr->status = 0;
	printf("Set Konfig PM ke default\r\n");
	for (i=0; i<JML_SUMBER; i++) 	{
		sprintf(p_sbr->pm[i].nama, "-");
		p_sbr->pm[i].alamat = 0;		/* default alamat = 0 : board Monita, PM = 1 s/d 247 / stack */
		p_sbr->pm[i].status = 0;
		p_sbr->pm[i].modul = 0;			// default modul 0: Power Meter
		//printf("PM-%d: alamat: %d, nama: %-s, status: %s\r\n", i+1, p_sbr->pm[i].alamat, p_sbr->pm[i].nama, p_sbr->pm[i].status==0?"Tidak aktif":"Aktif/Normal");
	}	
	
	if (simpan_titik_pm( p_sbr ) < 0)	{
		vPortFree( p_sbr );
		return;
	}
	vPortFree( p_sbr );
}

 int simpan_titik_pm( struct t_sumber_pm *pgr) {
	printf("Save struct titik ke flash ..");
	if(prepare_flash(SEKTOR_SUMBER, SEKTOR_SUMBER)) return;
	printf("..");
	
	if(hapus_flash(SEKTOR_SUMBER, SEKTOR_SUMBER)) return;
	printf("..");
	
	if(prepare_flash(SEKTOR_SUMBER, SEKTOR_SUMBER)) return;
	printf("..");
	
	
	if(tulis_flash(ALMT_SUMBER, (unsigned short *) pgr, (sizeof (struct t_sumber_pm) ))) return -1;
	
	printf(".. OK\r\n");
	return 0;
}
#endif
