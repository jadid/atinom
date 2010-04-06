/*

	8 Maret 2010
	furkan jadid, daun biru engineering
	
	setting untuk menyimpan file2 dalam MMC
	
	perintah2 :
	---------------------
	set_file nama terong_belanda : memberikan nama/awalan nama yang akan dipakai untuk
							membuat file (maks 16 karakter)
							
	set_file periode 10		: file disimpan dalam MMC setiap 10 menit (maks 60 menit)
	set_file set 0			: mode menyimpan tidak diaktifkan
	set_file set 1			: mode menyimpan diaktifkan
	set_file detik 5		: data disimpan dalam file setiap 5 detik (maks 60 detik)
	
	set_file data 23		: menambahkan data nomer 23 pada file
	set_file data 45		: menambahkan data nomer 45 pada file
	
	set_file data 23 unset	: jika ada data nomer 23 pada setting, maka akan dibersihkan
	
	data yang bisa diset sebanyak jumlah_sumber * per_sumber, atau sekitar 400 titik 
	
	
	set_file data clear		: membersihkan semua nomer data
	
	
	Bagaimana data disimpan :
	----------------------
	- setiap file akan punya header / head nama masing2 data
	- setiap data akan dipisahkan oleh spasi saja
	- setiap periode detik/menyimpan akan dipisahkan oleh new line saja
	- pada awal line akan terdapat tag tanggal	
	
	*/
	
#include "../monita/monita_uip.h"

static int set_file_default(void);
static int simpan_sfile( struct t_simpan_file *pgr);

static int cek_file(int argc, char **argv)
{
	int i;
	struct t_simpan_file *psfile;
	struct t_dt_set *dt;
	int sumb;
	unsigned char buf[24];
	int y=0;
	
	psfile = (char *) ALMT_SFILE;
	dt = (char *) ALMT_DT_SET;
	
	printf(" Ukuran %d !\r\n", sizeof (struct t_simpan_file));
	if (argc == 1)
	{
		judul(" Setting Simpan File\r\n");
		
		printf("  Nama     = %s\r\n", psfile->nama_file);
		printf("  Periode  = %d menit\r\n", psfile->periode);
		printf("  Set      = %s\r\n", ((psfile->set)?"Aktif [1]":"Mati [0]"));
		printf("  Detik ke = %d\r\n", psfile->detik);
				
		garis_bawah();
		sumb= 0;
		printf(" No  : No_Data : Nama             : Satuan \r\n");
		
		for (i=0; i< (JML_SUMBER * PER_SUMBER); i++)
		{
			y = psfile->no_data[i];
			
			if ( y != 0) 
			{
				sumb++;
				printf(" %2d  : %7d : %-16s : %-8s\r\n", sumb, y, dt[y - 1].nama, dt[y - 1].satuan );
			}
		}
		
	}
	else if (argc > 1)
	{
		if (strcmp(argv[1], "help") == 0)
		{
			printf(" Perintah untuk menampilkan setting file !\r\n");
			printf("    cek_file help  : untuk menampilkan ini.\r\n");
		}		
		else
		{
			/*
			sumb = 0;
			sprintf(buf, "%s", argv[1]);	
			sumb = cek_nomer_valid(buf, 10);
			if (sumb > 0 && sumb < 400)		
			{		
				struct t_dt_set *p_dt;
				p_dt = (char *) ALMT_DT_SET;
		
				judul(" Group Setting\r\n");
				printf("  Nomer      = %d, ID = %d\r\n", sumb, pgroup[ sumb - 1 ].ID_group);
				printf("  Nama       = %s\r\n", pgroup[ sumb - 1 ].nama);
				printf("  Status     = %d\r\n", pgroup[ sumb - 1 ].stat);
				printf("  Keterangan = %s\r\n", pgroup[ sumb - 1 ].ket);
				
				printf("  ----- koneksi data ------\r\n");
				
				y = 0;
				for (i=0; i<40; i++)
				{
					if ( pgroup[ sumb - 1 ].no_data[i] != 0 )
					{
						y++;
						printf("  %d. data dari --> %d, %s\r\n", y, pgroup[ sumb - 1 ].no_data[i],\
						p_dt[ (pgroup[ sumb - 1 ].no_data[i] - 1)].nama );
					}
				}
			}
			else
				printf(" ERR: Perintah tidak dikenali !\r\n");
				*/
		}
	}	
}

static tinysh_cmd_t cek_file_cmd={0,"cek_file","menampilkan konfigurasi file simpan","[] nomer",
                              cek_file,0,0,0};

int set_file(int argc, char **argv)
{
	unsigned char buf[24];
	int sumb=0;
	int ndata=0;
	unsigned int ret_ip;
	struct t_simpan_file *p_gr;
	int i;
	int y;
	
	if(argc<2) {
		printf(" ERR: argument kurang !\r\n");
		printf(" coba set_modem_ftp help \r\n");
		return;
	}
	
	judul(" Setting Simpan File\r\n");
	
			if (strcmp(argv[1], "help") == 0)	{
				printf(" Perintah untuk setting SIMPAN_FILE !\r\n");
				printf(" 1. set_file help/default\r\n");
				printf("    help    : printout keterangan ini\r\n");
				printf("    default : memberikan default setting simpan file\r\n");
				printf("\r\n");
				
				//*
				printf(" 2. set_file [opt1] [opt2]\r\n");
				printf("    set_file [nama|set|periode] [opt2]\r\n");
				printf("    opt1 : nama, set/aktif, periode, data\r\n");
				printf("    opt2 : nilai dari opt 1 yang tepat\r\n");
				printf("\r\n");
				printf("    [opt1]\r\n");				
				printf("    nama      : memberikan nama file yang akan ditampilkan\r\n");
				printf("    misalnya  : $ set_file nama MonitaFile. Nama Max 16 karakter.\r\n");
				printf("    artinya memberikan nama file dengan nilai MonitaFile\r\n");
				printf("\r\n");
				printf("    set/aktif : mengaktif/nonaktifkan penyimpanan file ke memori MMC\r\n");
				printf("    misalnya  : $ set_file [set|aktif]\r\n");
				printf("    artinya mengaktifkan penyimpanan file\r\n");
				printf("    misalnya  : $ set_file [unset|mati]\r\n");
				printf("    artinya me-nonaktifkan penyimpanan file\r\n");
				printf("\r\n");
				printf("    periode   : memberikan nilai periode penyimpanan file dalam MMC\r\n");
				printf("    misalnya  : $ set_file periode 10\r\n");
				printf("    artinya   : file disimpan dalam MMC setiap 10 menit\r\n");
				//*/

				return ;
			}
			else if (strcmp(argv[1], "default") == 0)
			{
				printf("set SIMPAN_FILE dengan data default !\n");
				set_file_default();
				
				return;
			}
	
  	//display_args(argc,argv);
	
	/* copy dulu yang lama kedalam buffer */
	p_gr = pvPortMalloc( sizeof (struct t_simpan_file) );
	
	if (p_gr == NULL)
	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		return -1;
	}
	printf(" %s(): Mallok ok di %X\r\n", __FUNCTION__, p_gr);
	
	memcpy((char *) p_gr, (char *) ALMT_SFILE, (sizeof (struct t_simpan_file)));
	
	
	
	if (argc>2) {
		if (strcmp(argv[1], "nama") == 0)	{	// set_file nama monita 3
			sprintf(buf, "%s", argv[2]);
			sumb = strlen(buf);
			
			if (sumb > 16)
			{
				printf(" ERR: Nama (%s) terlalu panjang !\r\n", buf);
				vPortFree( p_gr );
				return;			
			}	
			sprintf( p_gr->nama_file, "%s", buf);

		}
		else if (strcmp(argv[1], "periode") == 0)	{	// set_file periode 10 (simpan file tiap 10 menit ke MMC) 3
			sprintf(buf, "%s", argv[2]);	
			sumb = cek_nomer_valid(buf, 60);
			if (sumb > 0)		
			{
				p_gr->periode = sumb;
				printf(" Setting periodenya = %d\r\n", sumb);	
			}
		}
		else if (strcmp(argv[1], "detik") == 0) 	{	// set_file detik 10		3
			sprintf(buf, "%s", argv[2]);	
			sumb = cek_nomer_valid(buf, 60);
			if (sumb > 0)		
			{
				p_gr->detik = sumb;
				printf(" Setting detik = %d\r\n", sumb);	
			}
		}
		else if (strcmp(argv[1], "data") == 0) 	{
			if (strcmp( argv[2], "clear") == 0 )	{
				for (i=0; i< (JML_SUMBER * PER_SUMBER) ; i++)
					p_gr->no_data[i] = 0;
				
				printf(" Semua data yang berkontribusi pada file dibersihkan !\r\n");
				sumb = 2; // supaya true
			} else	{		
				sprintf(buf, "%s", argv[2]);	
				sumb = cek_nomer_valid(buf, (JML_SUMBER * PER_SUMBER));
				if (sumb > 0)		
				{				
						/* jika argument terakhir masih ada unset maka hanya pada 
						* nomer data itu saja yang dihapus */
						if ( argc > 3)
						{
							if (strcmp(argv[3], "unset") == 0 )		// set_file data x unset
							{
							printf(" Unset koneksi data %d dari setting SIMPAN_FILE !\r\n", sumb);
							for (i=0; i< (JML_SUMBER * PER_SUMBER); i++)
							{
								if ( p_gr->no_data[i] == sumb )
								{
									p_gr->no_data[i] = 0;
									break;
								}
							}
							if (i == (JML_SUMBER * PER_SUMBER))
								printf(" nomer data dimaksud tidak ada !\r\n");
							}
							else
								printf(" ERR: Perintah salah !\r\n");
								
						}
						else
						{
							printf(" Set koneksi data %d ke SIMPAN_FILE !\r\n", sumb);
							/* cari slot no_data yang masih nol */
							for (i=0; i<(JML_SUMBER * PER_SUMBER); i++)
							{
								if ( p_gr->no_data[i] == 0 )
								{
									p_gr->no_data[i] = sumb;
									break;
								}
							}
						}
				}
			} /* nomer group valid */
		} else {
			printf(" ERR: perintah tidak benar !\r\n");
			printf(" coba set_file help \r\n");
			vPortFree( p_gr );
			return;
		}
	} else if (argc==2) {
		if ( (strcmp(argv[1], "set") == 0) || (strcmp(argv[1], "aktif") == 0) ) {	// 2
			p_gr->set = (int) 1;
			printf(" Sistem simpan diaktifkan !\r\n");
		} else if ( (strcmp(argv[1], "unset") == 0) || (strcmp(argv[1], "mati") == 0) ) {	// 2
			p_gr->set = (int) 0;
			printf(" Sistem simpan tidak diaktifkan !\r\n");
		} else {
			printf(" ERR: perintah tidak benar / argumen kurang !\r\n");
			printf(" coba set_file help \r\n");
			vPortFree( p_gr );
			return;
		}
	} else {
		printf(" ERR: argument kurang !\r\n");
		printf(" coba set_file help \r\n");
		vPortFree( p_gr );
		return;
	}
	
	/* cek apakah pemeriksaan angka valid */
	/*
	if (sumb <= 0)
	{
		vPortFree( p_gr );
		return ;	
	}
	//*/
	
	// SEMUA TRUE dan sampai disini
	if (simpan_sfile( p_gr ) < 0)
	{
		vPortFree( p_gr );
		return -1;
	}
	vPortFree( p_gr );

}

static tinysh_cmd_t set_file_cmd={0,"set_file","konfigurasi SIMPAN_FILE","help default nama ket",
                              set_file,0,0,0};


static int set_file_default(void)
{
	int i;
	int y;
	struct t_simpan_file *p_gr;
	
	judul(" Set SIMPAN_FILE ke Default\r\n");
	
	p_gr = pvPortMalloc( sizeof (struct t_simpan_file) );
	if (p_gr == NULL)
	{
		printf("%s(): Err allok memory gagal !\r\n");
		return -1;
	}
	
	sprintf(p_gr->nama_file, "Monita");
	p_gr->periode = 10;
	p_gr->detik = 1;
	p_gr->set = 0;
	
	for (i=0; i< (JML_SUMBER * PER_SUMBER); i++)
	{
		p_gr->no_data[i] = 0;
	}
	
	if (simpan_sfile( p_gr ) < 0)
	{
		vPortFree( p_gr );
		return -1;
	}
	vPortFree( p_gr );	
}

//FRESULT scan_files (char* path)
static int hapus_file(int argc, char **argv) {
	DIR dirs;
	unsigned int size,oz=0, flag;
	unsigned int files;
	unsigned int file_sudah=0;
	unsigned int file_sukses=0;
	unsigned int jum_dirs;
	FILINFO fileInfo;
	char *nama;
	
	
}


static tinysh_cmd_t del_file_cmd={0,"rm","hapus file","", hapus_file,0,0,0};

static int simpan_sfile( struct t_simpan_file *pgr)
{
	printf(" Save struct SIMPAN_FILE ke flash ..");
	if(prepare_flash(SEKTOR_SFILE, SEKTOR_SFILE)) return -1;
	printf("..");
	
	if(hapus_flash(SEKTOR_SFILE, SEKTOR_SFILE)) return -1;
	printf("..");
	
	if(prepare_flash(SEKTOR_SFILE, SEKTOR_SFILE)) return -1;
	printf("..");
	
	if(tulis_flash(ALMT_SFILE, (unsigned short *) pgr, (sizeof (struct t_simpan_file)))) return -1;
	
	printf(".. OK\r\n");
	return 0;
}
