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

#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"

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
//static int hapus_direktori(int argc, char **argv) {
int hapus_SENDED() {
	DIR dirs;
	FIL fd2;
	unsigned int size,oz=0, flag;
	unsigned int files;
	unsigned int jum_dirs;
	FILINFO fileInfo;
	char *nama;
	
	unsigned int file_sudah=0;
	int c, res;
	
	unsigned char buf_lfn[255];
	char abs_path[128];
	char path[64];
	char isi[10];

	//cari_2jam_lalu(path);
	cari_waktu(path, "J-2");
	//printf("Posisi: %s\r\n",path);
	fileInfo.lfname = buf_lfn;
	fileInfo.lfsize = 255;//sizeof (buf_lfn);
	
	if ((res = f_opendir (&dirs, path))) { 
		printf("%s(): ERROR = %d\r\n", __FUNCTION__, res);
		return 0;
	}

	for (size = files = jum_dirs = 0;;) 	{
		if (((res = f_readdir (&dirs, &fileInfo)) != FR_OK) || !fileInfo.fname [0]) {
			printf("***************  gak ada file lagi, jml file: %d, terhapus: %d!!\r\n", files, file_sudah);
			
			// hapus direktori //
			// cek dulu apakah direktorinya kosong //
			//*
			if (files == file_sudah) {		// direktori kosong, tanpa file
				printf("+++++++++++++++++++ hapus direktori\r\n");
				if (dihapus(path) == 0)  {
					//printf("................direktori terhapus\r\n");
				} else {
					printf("..........................direktori......GAGAL\r\n");
				}
			} else {
				printf("ada %d file di direktori%s\r\n", files, path);
			}
			//*/
			break;
		} else {
			
		}
		if (fileInfo.fattrib & AM_DIR) 
			jum_dirs++;
		else 	{
			files++; 
			size += fileInfo.fsize;
			
			
			if (fileInfo.lfname[0] == 0)
				nama = &(fileInfo.fname [0]);
			else
				nama = &(fileInfo.lfname[0]);
				
			sprintf(abs_path,"%s\\%s", path, nama);
			printf("________________mcari file, ke-%d = %s \r\n", files, nama);
			if (res = f_open(&fd2, abs_path, FA_READ | FA_WRITE)) {
				printf("%s(): Buka file error %d !\r\n", __FUNCTION__, res);					
				return 0;
			}
				
				// cek apakah sudah pernah dikirim ke server 
				// sudah dikirim akan ada tag time dan SEND
				// dan ini akan berada di akhir file
				// 
				// sehingga kira2 begini 
				// Thu Mar 11 13:27:42 2010SEND
				// 
				// kira2 29 karakater dari akhir file
				// 
				// sehingga hanya perlu dicek 6 karakter terakhir saja
				// apakah ada SENDED
				 
				
			f_lseek( &fd2, fd2.fsize - 6 );
			f_read( &fd2, isi, 6, &res);
			//printf("CEK %s @@@\r\n", isi);
				
			if (strncmp( isi, "SENDED", 6) == 0) {
				if(f_close( &fd2 )==0) {}
				// hapus file
				if (dihapus(abs_path) == 0)  {
					printf("................terhapus\r\n");
				} else {
					printf("...............%s GAGAL dihapus\r\n", nama);
					break;
				}
				file_sudah++;
			} else	{				
				// kembalikan pointer //
				f_lseek( &fd2, 0);
				printf("...........................................tidak SENDED !!!\r\n");
			}
			vTaskDelay(10);
		}	
		
	}
	printf("%d File pada direktori %s dihapus.\r\n", file_sudah, path);
	vTaskDelay(500);
}

int dihapus(char *nama) {
	FRESULT res;
	int flag=10;
	printf("________________NAMANYA : %s",nama);
	res = f_unlink(nama);
	if (res == FR_OK) {
		printf("......dihapus\r\n");
		return 0;
	} 
	if (res == FR_WRITE_PROTECTED) {
		printf("......FR_WRITE_PROTECTED\r\n");
		flag= -2;
	}
	if (res == FR_NO_FILE) {
		printf("......FR_NO_FILE\r\n");
		flag= -2; 
	}
	if (res == FR_NO_PATH) {
		printf("......FR_NO_PATH\r\n");
		flag= -2; 
	}
	if (res == FR_INVALID_NAME) {
		printf("......FR_INVALID_NAME\r\n");
		flag= -2; 
	}
	if (res == FR_DENIED) {
		printf("......FR_DENIED\r\n");
		flag= -1; 
	}
	return flag;
}

/*
	posisi 
		H-7	: 7 hari sebelumnya
		J-2 : 2 jam sebelumnya
		B-1 : 1 bulan sebelumnya
//*/

char * bulan[]={"Jan","Feb","Mar","Apr","Mei","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
char tgl[]    ={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
char kabisat[]={31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void cari_waktu(char *dest, char *posisi) {
	
	if ((posisi[0]!='H') && (posisi[0]!='h') && (posisi[0]!='J') && (posisi[0]!='j') && (posisi[0]!='B') && (posisi[0]!='b') ) {
		printf("Argumen tidak benar !!\r\n");
		printf("Contoh : H-7, J-2, B-1\r\n");
		sprintf(dest,"\\");
		return;
	}

	struct t_gsm_ftp *p_dt;
	p_dt = (char *) ALMT_GSM_FTP;
	
	time_t timeval;
	struct tm timeinfo;
	get_tm_time( &timeinfo );
	timeval = mktime( &timeinfo );
	
	char *pch, str[10];
	int tmp, tmpx;
	int ijam, itgl, ibulan, ithn, count;

	printf("%s sekarang: %d:%d %d-%d[%s]-%d\r\n", posisi, timeinfo.tm_hour, timeinfo.tm_min, \ 
			timeinfo.tm_mday, timeinfo.tm_mon+1, bulan[timeinfo.tm_mon], timeinfo.tm_year+1900);
	
	pch=strstr(posisi,"-");
  	if (pch!=NULL)
  		strcpy(str, pch+1);
  	else
  		return 0;

	itgl = timeinfo.tm_mday;
	ibulan = timeinfo.tm_mon;
	ithn = timeinfo.tm_year+1900;

	if ( (posisi[0]=='H') || (posisi[0]=='h') ) {
		tmp = atoi(str);
		
		count = cek_waktu(&tmp, 2);
		itgl = tmp;
		
		tmp = count;
		if (count>0) {
			count = cek_waktu(&tmp, 3);
			ibulan = tmp;
		}
		
		tmp = count;
		if (count>0) {
			count = cek_waktu(&tmp, 4);
			ithn = tmp;
		}
		sprintf(dest,"\\%s\\tahun_%d\\%s\\tgl_%d", p_dt->direktori,  \ 
				ithn, bulan[ibulan], itgl);
	} else if ( (posisi[0]=='J') || (posisi[0]=='j') ) {
		tmp = atoi(str);
		count = cek_waktu(&tmp, 1);					// x tgl sebelumnya, count = 2
		ijam = tmp;

		tmp = count;
		if (count>0) {
			count = cek_waktu(&tmp, 2);
			itgl = tmp;
		}
		
		tmp = count;
		if (count>0) {
			count = cek_waktu(&tmp, 3);
			ibulan = tmp;
		}
		
		tmp = count;
		if (count>0) {
			count = cek_waktu(&tmp, 4);
			ithn = tmp;
		}
		sprintf(dest,"\\%s\\tahun_%d\\%s\\tgl_%d\\jam_%02d", p_dt->direktori,  \ 
				ithn, bulan[ibulan], itgl, ijam);
	} else if ( (posisi[0]=='B') || (posisi[0]=='b') ) {
		printf("%d Bulan sebelumnya: %d\r\n", atoi(str), timeinfo.tm_mon+1-atoi(str));
	} else {
		printf("Input salah\r\n");
	}
}

int cek_kabisat(int tahun) {
	if (tahun%400==0) {
		return 1;
	} else if( (tahun%400!=0) && (tahun%100!=0) && (tahun%4==0) ) {
		return 1;
	} else 
		return 0;
}

// opsi 1: jam 
// opsi 2: tgl
// opsi 3: bulan
// opsi 4: tahun
// return: count
int cek_waktu(int *waktu, int opsi) {		
	time_t timeval;
	struct tm timeinfo;
	get_tm_time( &timeinfo );
	timeval = mktime( &timeinfo );
	
	int count=0, flag;
	int tglnya_1;
	int tmp, tmpx;
	if (opsi<0 || opsi>4) {
		return -1;
	}
	//printf("_____________________awal: %d\r\n", *waktu);
	if (opsi==1) {		// jam
		tmp = timeinfo.tm_hour;		// tmp = 13
		tmp -= *waktu;				// tmp = 13-13
		
		while(tmp<0) { 				// tmp = 13-55
			tmp += 24;
			count++;
		}
		*waktu = tmp;
		return count;
	} else if(opsi==2) {	// tgl
		tmp = timeinfo.tm_mday;	// tmp = 1
		tmpx = timeinfo.tm_mon;	// tmpx= 1
		tmp -= *waktu;		// tmp = 1-1=0

		while(tmp<1) {
			if (cek_kabisat(timeinfo.tm_year+1900))	{	// tahun kabisat
				tglnya_1 = (int)kabisat[tmpx-count-1];// [1-0-1] = 0 : januari
			} else {
				tglnya_1 = (int)tgl[tmpx-count-1];
			}

			if(tmp<1) {	
				count++;					// count = 1
				tmp += tglnya_1;			// tmp = 0+31
				// jika lintas tahun //
				if (count>=tmpx) {			// 1, 1
					tmpx=count+12;
				}
			}
		}

		*waktu = tmp;
		return count;
	} else if (opsi==3) {
		tmp = timeinfo.tm_mon;
		tmpx = tmp-*waktu;
		tmp-=*waktu;
		
		while (tmp<0) {			// bulan bisa = 0 karena array. bulan[0] = januari.
			tmp += 12;
			count++;			// 1 th sebelumnya
		}
		*waktu = tmp;
		return count;
	} else if (opsi==4) {
		tmp = timeinfo.tm_year+1900;
		tmp-=*waktu;
		*waktu = tmp;
		return count;
	}
}

int hapus_paksa() {
	
}


FRESULT cari_files (char* path, char *aksi) {
    FRESULT res;
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;
    char abs_path[128];
#if _USE_LFN
    static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);
#endif


    res = f_opendir(&dir, path);
    if (res == FR_OK) {
        i = strlen(path);
        for (;;) {
            res = f_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0) break;
            if (fno.fname[0] == '.') continue;
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            if (fno.fattrib & AM_DIR) {
                sprintf(&path[i], "\\%s", fn);
                res = cari_files(path, aksi);
                if (res != FR_OK) break;
                path[i] = 0;
            } else {
				if (strcmp(aksi,"kirim_ftp")==0) {
					sprintf(abs_path, "%s\\%s", path, fn);
					kirim_file_ke_ftp(abs_path);
					//printf("kirim_ftpnya: %s\\%s\r\n", path, fn);
				} else
	                printf("%s\\%s\r\n", path, fn);
            }
        }
    }

    return res;
}

int cari_doku(int argc, char **argv) {
	display_args(argc,argv);
	printf("Jml arg: %d\r\n", argc);
	if (argc<2) {
		printf("Argumen kurang.\r\n");
		printf("cari [x-y: H-3 | J-1] [aksinya: ftp | lihat]\r\n");
		return -1;
	}
	char buf[127];
	sprintf(buf, "%s", argv[1]);
	if ((buf[0]!='H') && (buf[0]!='h') && (buf[0]!='J') && (buf[0]!='j') && (buf[0]!='B') && (buf[0]!='b') ) {
		printf("Argumen tidak benar !!\r\n");
		printf("Contoh : H-7, J-2, B-1\r\n");
		return -1;
	}
	
	//printf("isi buf: %s\r\n");
	
	if (strcmp(argv[2], "ftp") == 0) {
		if (konek_ftp_awal()==0) 
		{
			printf("Koneksi GPRS gagal !!!\r\n");
			printf("Create FTP sesssion error !\r\n");
			return 0;
		} else {
			printf("Create FTP sesssion !\r\n");
		}
		
		cari_berkas(buf, "kirim_ftp");
		
		tutup_koneksi_ftp();
	} else if (strcmp(argv[2], "lihat") == 0) {
		cari_berkas(buf, "lihat");
	} else {
		printf("Perintah salah\r\n");
		printf("cari [x-y: H-3 | J-1] [aksinya: ftp | lihat]\r\n");
	}
/*	
	char path[127];
	char *pch, str[10];
	int i=0;
	sprintf(buf, "%s", argv[1]);
	printf("Awalnya posisi: %s\r\n",buf);
	
	pch=strstr(buf,"-");
  	if (pch!=NULL)
  		strcpy(str, pch+1);

  	for(i=atoi(str); i>0; i--) {
		sprintf(buf, "%c-%d", buf[0],i);
		cari_waktu(path, buf);
		//printf("_____________--path: %s\r\n",path);
		cari_files(path);
	}
//*/
  	//printf("isi: %d\r\n", atoi(str));
	//	cari_waktu(path, buf);
	//	printf("Path: %s\r\n", path);
	//cari_files(path);
}

int cari_berkas(char *buf, char *aksi) {
	if ((buf[0]!='H') && (buf[0]!='h') && (buf[0]!='J') && (buf[0]!='j') && (buf[0]!='B') && (buf[0]!='b') ) {
		printf("Argumen tidak benar !!\r\n");
		printf("Contoh : H-7, J-2, B-1\r\n");
		return -1;
	}
	
	char path[127];
	char *pch, str[10], waktu[10], aksinya[20];
	int i=0;
	strcpy(waktu,buf);
	strcpy(aksinya, aksi);
	//printf("Awalnya posisi: %s\r\n",waktu);
	
	
	pch=strstr(waktu,"-");
  	if (pch!=NULL)
  		strcpy(str, pch+1);
	//printf("______aksinya: %s, waktu: %s, str: %s_________\r\n\r\n", aksinya, waktu, str);
	//*
  	for(i=atoi(str); i>0; i--) {
		sprintf(waktu, "%c-%d", waktu[0],i);
		cari_waktu(path, waktu);
		//printf("_______________path: %s\r\n",path);
		cari_files(path, aksinya);
	}
	
	//*/
}

static tinysh_cmd_t del_direktori_cmd={0,"rm","hapus file","", hapus_SENDED,0,0,0};

static tinysh_cmd_t cari_doku_cmd={0,"cari","cari file","", cari_doku,0,0,0};

static int simpan_sfile( struct t_simpan_file *pgr) {
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
