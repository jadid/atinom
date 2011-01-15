/*
	10 Maret 2010
	furkan jadid, daun biru engineering
	
	setting file untuk kontrol modem
	
	perintah2 :
	------------------------------
	
	FTP :
	set_modem_ftp default
	set_modem_ftp set		: program ftp dijalankan
	set_modem_ftp unset		: program ftp tidak dijalankan
	set_modem_ftp period 10	: periode untuk melakukan ftp (menit)
	
	set_modem_ftp user		nama_user
	set_modem_ftp passwd	isi_passwd
	set_modem_ftp server	www.daunbiru.com
	set_modem_ftp direktori data_angin
	
	cek_modem_ftp
	
	GSM :
	set_modem_gsm set
	set_modem_gsm unset
	
*/


#include "../monita/monita_uip.h"
#include "../gsm/system_ftp.c"
//#include "../gsm/kirim_ftp.c"

int fd = 0;
//char serGSM[128];


//#define read(a, b, c) serX_getchar(PAKAI_GSM_FTP, a, b, 1000);	\ 
//				tendang_wdog();

#define read(a, b, c) ser2_getchar(a, b, 1000);	\
		tendang_wdog();


static int simpan_data_gsm_ftp( struct t_gsm_ftp *pgr);

static tinysh_cmd_t gsm_ftp_cmd={0,"gsm_ftp","menampilkan konfigurasi modem","[] nomer",
                              gsm_ftp,0,0,0};


int cek_modem(int argc, char **argv)	{	
	if (argc == 1)	{
		struct t_gsm_ftp *p_dt;
		p_dt = (char *) ALMT_GSM_FTP;
	
		judul(" Modem GSM FTP Setting\r\n");

		printf("Status FTP    : %s\r\n", (p_dt->ftp_mode>0)?"Aktif [1]":"Tidak Aktif [0]");
		printf("FTP Server    : %s\r\n", p_dt->ftp_server);
		printf("FTP User      : %s\r\n", p_dt->ftp_user);
		printf("FTP Password  : %s\r\n\r\n", p_dt->ftp_passwd);
		
		printf("Periode FTP   : %d\r\n", p_dt->ftp_periode);
		printf("Nama File     : %s\r\n", p_dt->nama_file);
		printf("Direktori     : %s\r\n\r\n", p_dt->direktori);
		
		printf("Status GPRS   : %s\r\n", (p_dt->gsm_mode>0)?"Aktif [1]":"Tidak Aktif [0]");
		printf("Jenis Kartu   : %s\r\n", p_dt->kartu);
		printf("GPRS APN1     : %s\r\n", p_dt->gprs_apn1);
		printf("GPRS APN2     : %s\r\n", p_dt->gprs_apn2);
		printf("GPRS User     : %s\r\n", p_dt->gprs_user);
		printf("GPRS Password : %s\r\n", p_dt->gprs_passwd);
		
	}
	else if (argc > 1)
	{
		if (strcmp(argv[1], "help") == 0)
		{
				printf(" Perintah untuk menampilkan setting data !\r\n");
				printf("    cek_modem help  : untuk menampilkan ini.\r\n");
		}
	} else {
		printf(" ERR: perintah tidak benar !\r\n");
		printf(" coba cek_modem help \r\n");
	}
	
}

static tinysh_cmd_t cek_modem_cmd={0,"cek_modem","menampilkan konfigurasi modem","[] nomer",
                              cek_modem,0,0,0};


static int set_modem_ftp_default(void) {
	int i;
	struct t_gsm_ftp *p_gr;
	
	judul(" Set Modem FTP ke Default\r\n");
	
	p_gr = pvPortMalloc( sizeof (struct t_gsm_ftp) );

	if (p_gr == NULL)
	{
		printf("%s(): Err allok memory gagal !\r\n", __FUNCTION__);
		return -1;
	}
	
	printf("tes: %d\r\n", PAKAI_GSM_FTP);
//*
	sprintf(p_gr->ftp_user, 	"malingping@daunbiru.com");
	sprintf(p_gr->ftp_passwd, 	"diesel");
	sprintf(p_gr->ftp_server, 	"ftp.daunbiru.com");
	sprintf(p_gr->nama_file, 	"Monita");
	sprintf(p_gr->direktori, 	"monitaangin");
	p_gr->ftp_periode = 30;
	p_gr->ftp_mode = 0;
	p_gr->ftp_port = 21;
	p_gr->gsm_mode = 0;
	sprintf(p_gr->kartu,		"telkomsel");
	sprintf(p_gr->gprs_apn1,	"internet");
	sprintf(p_gr->gprs_apn2, 	"telkomsel");
	sprintf(p_gr->gprs_user, 	"wap");
	sprintf(p_gr->gprs_passwd, 	"wap123");
//*/
	
	if (simpan_data_gsm_ftp( p_gr ) < 0)	{
		vPortFree( p_gr );
		return -1;
	}
	vPortFree( p_gr );
	
}

int set_modem_ftp(int argc, char **argv)	{
	unsigned char serGSM[24];
	int sumb=0;
	unsigned int ret_ip;
	struct t_gsm_ftp *p_dt;
	
	if(argc<2) {
		printf(" ERR: argument kurang !\r\n");
		printf(" coba set_modem_ftp help \r\n");
		return;
	}
	
	judul(" Setting FTP\r\n");
	
			if (strcmp(argv[1], "help") == 0) {
				printf(" Perintah untuk setting modem ftp !\r\n");
				printf(" 1. set_modem_ftp help/default\r\n");
				printf("    help    : printout keterangan ini\r\n");
				printf("    default : memberikan default setting modem ftp\r\n");
				printf("\r\n");
				
				printf(" 2. set_modem_ftp x [opt1] [opt2]\r\n");
				printf("    x    : nomer data\r\n");
				printf("    opt1 : nama, set/aktif, desc/ket\r\n");
				printf("\r\n");
				printf("    [opt1]\r\n");				
				printf("    nama     : memberikan nama data yang akan ditampilkan\r\n");
				printf("    misalnya : $ set_data 2 nama GMT_#4\r\n");
				printf("    artinya memberikan nama data 1 dengan GMT_#4\r\n");
				printf("\r\n");
				printf("    alarm    : memberikan setting batasan alarm data\r\n");
				printf("    misalnya : $ set_data 2 alarmH 84.3\r\n");
				printf("    artinya memberikan setting batasan alarm tinggi data 2 dengan nilai 84.3\r\n");
				printf("    misalnya : $ set_data 5 alarmL 1.25\r\n");
				printf("    artinya memberikan setting batasan alarm rendah data 5 dengan nilai 1.25\r\n");
				printf("\r\n");
				printf("    relay : mengaktif/nonaktifkan relay pada kanal tertentu\r\n");
				printf("    misalnya  : $ set_data 4 relay [1|aktif|hidup] 7 \r\n");
				printf("    artinya mengaktifkan relay untuk data ke 4 pada kanal 7\r\n");
				printf("    misalnya  : $ set_group 8 relay [0|mati] 2\r\n");
				printf("    artinya me-nonaktifkan relay untuk data ke 8 pada kanal 2\r\n");
				printf("\r\n");
				
				return ;
			}
			else if (strcmp(argv[1], "default") == 0) 	{
				printf("set MODEM ftp dengan data default !\n");
				set_modem_ftp_default();
				return;
			}
	
	p_dt = pvPortMalloc( sizeof (struct t_gsm_ftp) );
	//printf("Jml alikasi : %d, p_dt: %d, isi: %d\r\n", (JML_SUMBER * PER_SUMBER) * sizeof (struct t_dt_set), p_dt, *p_dt);
	
	if (p_dt == NULL) {
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		return -1;
	}
	printf(" %s(): Mallok ok di %X\r\n", __FUNCTION__, p_dt);
	
	memcpy((char *) p_dt, (char *) ALMT_GSM_FTP, (sizeof (struct t_gsm_ftp)));
	
	if (argc>2) {
		if (strcmp(argv[1], "user") == 0) 	{				// 3	
			if (strlen(argv[2]) > 40) {
				printf(" ERR: nama terlalu panjang (Maks 40 karakter)!\r\n");
				vPortFree( p_dt );
				return;
			}
			sprintf(p_dt->ftp_user, argv[2]);
			printf(" User FTP : %s - %s\r\n", p_dt->ftp_user, argv[2]);
		}
		else if (strcmp(argv[1], "password") == 0)	{		// 3
			if (strlen(argv[2]) > 16) {
				printf(" ERR: password terlalu panjang (Maks 16 karakter)!\r\n");
				vPortFree( p_dt );
				return;
			}
			sprintf(p_dt->ftp_passwd, argv[2]);	
		}
		else if (strcmp(argv[1], "server") == 0)	{		// 3
			if (strlen(argv[2]) > 40) {
				printf(" ERR: nama server terlalu panjang (Maks 40 karakter)!\r\n");
				vPortFree( p_dt );
				return;
			}
			sprintf(p_dt->ftp_server, argv[2]);	
		}
		else if (strcmp(argv[1], "port") == 0)	{			// 3
			sprintf(serGSM, "%s", argv[2]);	
			sumb = cek_nomer_valid(serGSM, 5000);		// 5 jam
			if (sumb > 0)		
			{
				printf(" Port-nya  : %d\r\n", sumb);			
				p_dt->ftp_port = sumb;
			} else {
				vPortFree( p_dt );
				return;
			}
		}
		else if (strcmp(argv[1], "periode") == 0)  {		// 3
			sprintf(serGSM, "%s", argv[2]);	
			sumb = cek_nomer_valid(serGSM, 60*5);		// 5 jam
			if (sumb > 0)		
			{
				printf(" Periodenya  : %d\r\n", sumb);			
				p_dt->ftp_periode = sumb;
			} else {
				vPortFree( p_dt );
				return;
			}
		}
		else if (strcmp(argv[1], "direktori") == 0)	{		// 3
			if (strlen(argv[2]) > 40) {
				printf(" ERR: nama server terlalu panjang (Maks 40 karakter)!\r\n");
				vPortFree( p_dt );
				return;
			}
			sprintf(p_dt->direktori, argv[2]);	
		}
		else if (strcmp(argv[1], "file") == 0)	{			// 3
			if (strlen(argv[2]) > 40) {
				printf(" ERR: nama file terlalu panjang (Maks 40 karakter)!\r\n");
				vPortFree( p_dt );
				return;
			}
			sprintf(p_dt->nama_file, argv[2]);	
		} else {
			printf(" ERR: perintah tidak benar !\r\n");
			printf(" coba set_modem_ftp help \r\n");
			vPortFree( p_dt );
			return;
		}
	} else if (argc==2) {
		if ( (strcmp(argv[1], "set") == 0) || (strcmp(argv[1], "aktif") == 0) ) {	// 2
			p_dt->ftp_mode = 1;
			printf(" Sistem FTP diaktifkan !\r\n");
		}
		else if ( (strcmp(argv[1], "unset") == 0) || (strcmp(argv[1], "mati") == 0) ) {	// 2
			p_dt->ftp_mode = 0;
			printf(" Sistem FTP tidak diaktifkan !\r\n");
		} else {
			printf(" ERR: perintah tidak benar / argumen kurang !\r\n");
			printf(" coba set_modem_ftp help \r\n");
			vPortFree( p_dt );
			return;
		}
	} else {
		printf(" ERR: argument kurang !\r\n");
		printf(" coba set_modem_ftp help \r\n");
		vPortFree( p_dt );
		return;
	}

	// SEMUA TRUE dan sampai disini
	if (simpan_data_gsm_ftp( p_dt ) < 0)	{
		vPortFree( p_dt );
		return -1;
	}
	printf(" Data telah tersimpan\r\n");
	vPortFree( p_dt );

}

static tinysh_cmd_t set_modem_ftp_cmd={0,"set_modem_ftp","mengubah konfigurasi modem ftp",
		"help default user password periode server direktori",set_modem_ftp,0,0,0};

static tinysh_cmd_t cek_ftp_cmd={0,"cek_ftp","mengecek koneksi modem: gprs dan ftp",
		"",cek_ftp,0,0,0};


int set_modem_gsm(int argc, char **argv)	{
	unsigned char serGSM[24];
	int sumb=0;
	unsigned int ret_ip;
	struct t_gsm_ftp *p_dt;
	
	if (argc<2) {
		printf(" ERR: argumen kurang !\r\n");
		printf(" coba set_modem_gsm help \r\n");
		return;
	}
	
	judul(" Setting GSM\r\n");
	
			if (strcmp(argv[1], "help") == 0) 	{
				printf(" Perintah untuk setting modem ftp !\r\n");
				printf(" 1. set_modem_ftp help/default\r\n");
				printf("    help    : printout keterangan ini\r\n");
				printf("    default : memberikan default setting modem ftp\r\n");
				printf("\r\n");
				
				printf(" 2. set_modem_ftp x [opt1] [opt2]\r\n");
				printf("    x    : nomer data\r\n");
				printf("    opt1 : nama, set/aktif, desc/ket\r\n");
				printf("\r\n");
				printf("    [opt1]\r\n");				
				printf("    nama     : memberikan nama data yang akan ditampilkan\r\n");
				printf("    misalnya : $ set_data 2 nama GMT_#4\r\n");
				printf("    artinya memberikan nama data 1 dengan GMT_#4\r\n");
				printf("\r\n");
				printf("    alarm    : memberikan setting batasan alarm data\r\n");
				printf("    misalnya : $ set_data 2 alarmH 84.3\r\n");
				printf("    artinya memberikan setting batasan alarm tinggi data 2 dengan nilai 84.3\r\n");
				printf("    misalnya : $ set_data 5 alarmL 1.25\r\n");
				printf("    artinya memberikan setting batasan alarm rendah data 5 dengan nilai 1.25\r\n");
				printf("\r\n");
				printf("    relay : mengaktif/nonaktifkan relay pada kanal tertentu\r\n");
				printf("    misalnya  : $ set_data 4 relay [1|aktif|hidup] 7 \r\n");
				printf("    artinya mengaktifkan relay untuk data ke 4 pada kanal 7\r\n");
				printf("    misalnya  : $ set_group 8 relay [0|mati] 2\r\n");
				printf("    artinya me-nonaktifkan relay untuk data ke 8 pada kanal 2\r\n");
				printf("\r\n");
				
				return ;
			}
			else if (strcmp(argv[1], "default") == 0)
			{
				printf("set MODEM ftp dengan data default !\n");
				set_modem_ftp_default();
				
				return;
			}		
	
  	//display_args(argc,argv);
	
	p_dt = pvPortMalloc( sizeof (struct t_gsm_ftp) );
	//printf("Jml alikasi : %d, p_dt: %d, isi: %d\r\n", (JML_SUMBER * PER_SUMBER) * sizeof (struct t_dt_set), p_dt, *p_dt);
	
	if (p_dt == NULL) {
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		return -1;
	}
	printf(" %s(): Mallok ok di %X\r\n", __FUNCTION__, p_dt);
	
	memcpy((char *) p_dt, (char *) ALMT_GSM_FTP, (sizeof (struct t_gsm_ftp)));
	
	if (argc>2) {
		if (strcmp(argv[1], "user") == 0) 	{		
			if (strlen(argv[2]) > 16) {
				printf(" ERR: nama terlalu panjang (Maks 16 karakter)!\r\n");
				vPortFree( p_dt );
				return;
			}
			sprintf(p_dt->gprs_user, argv[2]);	
		}
		else if (strcmp(argv[1], "password") == 0)	{
			if (strlen(argv[2]) > 16) {
				printf(" ERR: password terlalu panjang (Maks 16 karakter)!\r\n");
				vPortFree( p_dt );
				return;
			}
			sprintf(p_dt->gprs_passwd, argv[2]);	
		}
		else if (strcmp(argv[1], "apn1") == 0)	{
			if (strlen(argv[2]) > 16) {
				printf(" ERR: nama APN 1 terlalu panjang (Maks 16 karakter)!\r\n");
				vPortFree( p_dt );
				return;
			}
			sprintf(p_dt->gprs_apn1, argv[2]);	
		}
		else if (strcmp(argv[1], "apn2") == 0)	{
			if (strlen(argv[2]) > 16) {
				printf(" ERR: nama APN 2 terlalu panjang (Maks 16 karakter)!\r\n");
				vPortFree( p_dt );
				return;
			}
			sprintf(p_dt->gprs_apn2, argv[2]);	
		} else {
			printf(" ERR: perintah tidak benar !\r\n");
			printf(" coba set_modem_gsm help \r\n");
			vPortFree( p_dt );
			return;
		}
	} else if (argc==2) {
		if ( (strcmp(argv[1], "set") == 0) || (strcmp(argv[1], "aktif") == 0) ) {
			p_dt->gsm_mode = 1;
		}
		else if ( (strcmp(argv[1], "unset") == 0) || (strcmp(argv[1], "mati") == 0) ) {
			p_dt->gsm_mode = 0;
		} else {
			printf(" ERR: perintah tidak benar / argumen kurang !\r\n");
			printf(" coba set_modem_gsm help \r\n");
			vPortFree( p_dt );
			return;
		}
	} else {
		printf(" ERR: perintah tidak benar !\r\n");
		printf(" coba set_modem_gsm help \r\n");
		vPortFree( p_dt );
		return;
	}
	//*/
	
	
	/* cek apakah pemeriksaan angka valid */
	/*
	if (sumb <= 0) 	{
		vPortFree( p_dt );
		return ;	
	}
	//*/
	
	// SEMUA TRUE dan sampai disini
	if (simpan_data_gsm_ftp( p_dt ) < 0) 	{
		vPortFree( p_dt );
		return -1;
	}
	printf(" Data telah tersimpan\r\n");
	vPortFree( p_dt );

}

static tinysh_cmd_t set_modem_gsm_cmd={0,"set_modem_gsm","mengubah konfigurasi modem gsm",
		"help default user password periode server direktori",set_modem_gsm,0,0,0};

static int simpan_data_gsm_ftp( struct t_gsm_ftp *pgr)
{
	printf(" Save struct DATA_SET ke flash ..");
	if(prepare_flash(SEKTOR_GSM_FTP, SEKTOR_GSM_FTP)) return -1;
	printf("..");
	
	if(hapus_flash(SEKTOR_GSM_FTP, SEKTOR_GSM_FTP)) return -1;
	printf("..");
	
	if(prepare_flash(SEKTOR_GSM_FTP, SEKTOR_GSM_FTP)) return -1;
	printf("..");
	
	if(tulis_flash(ALMT_GSM_FTP, (unsigned short *) pgr, (sizeof (struct t_gsm_ftp) ))) return -1;
	
	printf(".. OK\r\n");
	return 0;
}

void cetak_tulisan(char *isi) {
	printf("%s", isi);
}

void baca_hasil() {
	char serGSM[24];
	baca_serial(serGSM, 50, 5);
	printf(" MODEM: %s\r\n",serGSM);
	if (strncmp(serGSM,"AT",2)==0) {
		baca_serial(serGSM, 20, 5);
		printf(" MODEM: %s\r\n",serGSM);
	}
}


int baca_serial(char *string_modem, int len, int timeout)
{
	int res;
	char c;
	int tout=0;
	int masuk = 0;	

	while(len--)	{
		//res = read( fd, &c, 1);
		res = ser2_getchar(1, &c, 100);
		if (res != 0)	{
			if ( (char) c == 0x0A || (char) c == 0x0D )		{
				if (masuk > 0) 	{	
					/* supaya data lama tidak ikut di printout */
					string_modem[masuk] = 0;
					return 0;
				}
			}
			else	{
				string_modem[ masuk ] = (char ) c;								
				masuk++;
				
				#if (DEBUG == 1)
				printf(" %s(): res=%d : msk=%02d : 0x%02X : %c\r\n", __FUNCTION__, res, masuk, (char) c, (char) c);
				#endif
			}
		}
		else		{
			//printf(" %s(): %d :timeout\r\n", __FUNCTION__, tout);
			len++;
			tout++;
			if ( tout > timeout) 
				return 0;
				//return -1;
		}
	}
}
