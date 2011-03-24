/*
 
	perintah2 :
	---------------------
	set_cron no mnt jam  tgl bln  cmd alamat  status   : memberikan nama/awalan nama yang akan dipakai untuk
							membuat file (maks 16 karakter)
							
	set_cron  1   0   7   s   s  relay  3   [aktif|1]  : mengeset cron no 1 isinya perintah ke relay no 3 aktif
							pada waktu setiap hari disetiap bulan pada jam 7 menit ke 0.
	
	set_cron  4	s%30+1  s  1  *  relay  5   [mati|0]   : 
			// mau pake '/' ko tidak tampil
	
	set_cron  2 [aktif|1] 							   : mengeset cron no 2 dijalankan !
	 
	set_cron  default								   : set_cron default kembali baru seperti awalnya



*/


#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"

#ifdef PAKAI_CRON

#ifndef __CRON__
#define __CRON__

xTaskHandle hdl_cron;

int status_modem;
int job_cron;
int status_cron;


#ifdef PAKAI_GSM_FTP
	int saat_gsm_aksi;
#endif

#ifdef PAKAI_SMS
	int saat_sms_aksi;
#endif


static int simpan_cron( struct t_cron *pgr);



int cron(char * data, int cek) {
	char str_cron[10];
	char * pch;
	//printf("__%s__%c__", data, data[2]);
	if (data[1]=='/' || data[1]==37) {				// s/x		s%x
		ganti_kata(str_cron, data);
		pch=strchr(str_cron,'+');

		if (pch!=NULL) { 			// s/x+y
			if ((cek%atoi(str_cron)==atoi(pch+1))) {
				return 1;			// menit BENAR
			} else {
				return 0;
			}
		} else if ((cek%atoi(str_cron)==0) || cek==0) {  // s/x
			return 1;				// menit BENAR
		} else {
			return 0;
		}
	} else {							// SEMUA MENIT
		return 1;
	}
}

int lihat_cron(int argc, char **argv)
{
	int i;
	//unsigned char str_cron[24];
	int sumb=0;

	if (argc == 1)	{
		struct t_cron *p_dt;
		p_dt = (char *) ALMT_CRON;
	
		judul(" Setting Cron\r\n");
		printf("  no. :  Cron :   Menit  :    Jam   :    Tgl   :   Bulan  :  Perintah  : Almt : Status\r\n");
		garis_bawah();
	
		for (i=0; i<JML_CRON; i++)	{
			printf(" (%3d): %-5s : %-8s : %-8s : %-8s : %-8s : %-10s : %3d :  %s\r\n", (i+1), \
				(p_dt[i].set)?"Aktif":"Mati", p_dt[i].mnt, p_dt[i].jam, p_dt[i].tgl, p_dt[i].bln, p_dt[i].cmd, p_dt[i].alamat, p_dt[i].status?"Aktif [1]":"Mati  [0]");	
		}
	}
	else if (argc > 1)	{
		if (strcmp(argv[1], "help") == 0)	{
				printf(" Perintah untuk menampilkan setting data !\r\n");
				printf("    cek_cron help  : untuk menampilkan ini.\r\n");
				printf("    cek_cron       : menampilkan seluruh setting data.\r\n");
		}		
		else	{
				printf(" ERR: Perintah tidak dikenali !\r\n");
		}
	}
}

static tinysh_cmd_t cek_cron_cmd={0,"cek_cron","menampilkan konfigurasi cron",
		"help default ",lihat_cron,0,0,0};

int cekCron(struct t_cron tes) {
	//time_t rawtime;
	//struct tm * timeinfo;
	
	//(void)time ( &rawtime );
	//timeinfo = localtime ( &rawtime );
	
	time_t timeval;
	struct tm timeinfo;
	get_tm_time( &timeinfo );
	timeval = mktime( &timeinfo );
	// tw.tm_year+1900), bulan[tw.tm_mon], tw.tm_mday, tw.tm_hour-1
	int flag=0;

	if (tes.bln[0]=='s') {					// periodik bln
		//printf("Blnsss: %d ", timeinfo.tm_mon);
		flag = cron(tes.bln, timeinfo.tm_mon);
	} else {								// tunjuk bln
		//printf("Bln: %s ", tes.bln);
		if (atoi(tes.bln) == timeinfo.tm_mon) {
			flag = 1;						// bln BENAR, lanjut cek tgl
		}  else {
			return 0;						// cron bln SALAH
		}
	}
	
	if (tes.tgl[0]=='s') {					// periodik tgl
		//printf("Tglsss: %d ", timeinfo.tm_mday);
		flag = cron(tes.tgl, timeinfo.tm_mday);
	} else {								// tunjuk tgl
		//printf("Tgl: %s ", tes.tgl);
		if (atoi(tes.tgl) == timeinfo.tm_mday) {
			flag = 1;						// tgl BENAR, lanjut cek jam
		}  else {
			return 0;						// cron tgl SALAH
		}
	}
	
	if (tes.jam[0]=='s') {					// periodik jam
		//printf("Jamsss: %d ", timeinfo.tm_hour);
		flag = cron(tes.jam, timeinfo.tm_hour);
	} else {								// tunjuk jam
		//printf("Jam: %s ", tes.jam);
		if (atoi(tes.jam) == timeinfo.tm_hour) {
			flag = 1;						// jam BENAR, lanjut cek menit
		}  else {
			return 0;						// cron JAM SALAH
		}
	}
	
	if (tes.mnt[0]=='s') {					// periodik menit
		//printf("Menitsss: %d ", timeinfo.tm_min);
		flag = cron(tes.mnt, timeinfo.tm_min);
	} else {								// periodik menit
		//printf("Menit: %s ", tes.mnt);
		if (atoi(tes.mnt) == timeinfo.tm_min) {
			flag = 1;						// menit BENAR
		}  else {
			return 0;						// cron menit SALAH
		}
	}
	return flag;
}

int hitung = 0;
int hitung_lagi=0;

void baca_cron() {
	if (hitung_lagi==120) {			// tunggu sampai 120 untuk 1 menit, 60 untuk 30 detik, 
		hitung_lagi=0;				// reset;
	}
	
	if ((hitung_lagi>10) && (hitung_lagi<(10+JML_CRON+1))) {			// masuk cron !!
		//printf("hitung_lagi: %d, hitung: %d\r\n", hitung_lagi, hitung);
		struct t_cron *p_dt;
		p_dt = (char *) ALMT_CRON;
		
		if (hitung==JML_CRON) {
			hitung=0;
		}
		//*
		time_t timeval;
		struct tm timeinfo;
		get_tm_time( &timeinfo );
		timeval = mktime( &timeinfo );
		//*/
		
		
			
		if (cekCron(p_dt[hitung])==1) {	
			
			if (p_dt[hitung].set) {
				printf(" %d:%d %d-%d ",timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_mday,timeinfo.tm_mon+1, (timeinfo.tm_year+1900)); //, bulan[], , , 
				/*
				//printf(" (%3d): %-5s : %-8s : %-8s : %-8s : %-8s : %-10s : %3d :  %s\r\n", (hitung+1), (p_dt[hitung].set)?"Aktif":"Mati", \
				//		p_dt[hitung].mnt, p_dt[hitung].jam, p_dt[hitung].tgl, p_dt[hitung].bln, \ 
				//		p_dt[hitung].cmd, p_dt[hitung].alamat, p_dt[hitung].status?"Aktif [1]":"Mati [0]");
				//*/
				#ifdef PAKAI_SELENOID
				if (strcmp(p_dt[hitung].cmd,"relay")==0) {
					if (p_dt[hitung].status==1) {
						set_selenoid((uint) p_dt[hitung].alamat);
						data_f[(PER_SUMBER*JML_SUMBER)+p_dt[hitung].alamat] = 1;
						#ifdef DEBUG_DATA
							//set_selenoid(2);
							printf(".......NYALA %d\r\n", p_dt[hitung].alamat);
						#endif
					} else {
						unset_selenoid((uint)p_dt[hitung].alamat);
						data_f[(PER_SUMBER*JML_SUMBER)+p_dt[hitung].alamat] = 0;
						#ifdef DEBUG_DATA
							//unset_selenoid(2);
							printf(".......MATI %d\r\n", p_dt[hitung].alamat);
						#endif
					}
				} else if (strcmp(p_dt[hitung].cmd,"reset")==0) {
					printf("reset relay 6, modem\r\n");
					unset_selenoid(6);
					vTaskDelay(1000);
					set_selenoid(6);
				}
				#endif
				
				#ifdef PAKAI_GSM_FTP
				if (strcmp(p_dt[hitung].cmd,"ftp")==0) {
					saat_gsm_aksi=1;
					printf("......KIRIM ftp\r\n");
					//gsm_ftp();
				}
				#endif
				
				#ifdef PAKAI_SMS
				if (strcmp(p_dt[hitung].cmd,"bacasms")==0) {
					printf("....baca sms\r\n");
					saat_sms_aksi = 1;
					//baca_sms_cron();
				}
				
				if (strcmp(p_dt[hitung].cmd,"pulsa")==0) {
					printf("....CEK PULSA\r\n");
					saat_sms_aksi = 2;
					//kirim_sisa_pulsa();
				}
				
				if (strcmp(p_dt[hitung].cmd,"pulsatipis")==0) {
					printf("....PULSA TIPIS\r\n");
					saat_sms_aksi = 3;
					//kirim_sisa_pulsa();
				}
				#endif
				
				#ifdef PAKAI_FILE_SIMPAN
				if (strcmp(p_dt[hitung].cmd,"hapus")==0) {
					printf("Hapus file !!!\r\n");
					job_cron = 1;
					/*
					if (status_modem==1)		// biar nggak konflik baca file
						return;
					printf("......HAPUS file J-12\r\n");			
					//cari_berkas("J-12", "lihat");
					cari_berkas("J-12","hapus");
					vTaskDelay(100);
					//*/
				}
				
				if (strcmp(p_dt[hitung].cmd,"list")==0) {
					printf("......list file J-12\r\n");			
					cari_berkas("J-12", "lihat");
				}
				#endif
				
				if (strcmp(p_dt[hitung].cmd,"cek")==0) {
					printf("...............cek_cron, printf ini !!!\r\n");
				}
			}
			
		}
		hitung++;
	}
	hitung_lagi++;
}

/*
 * struct t_cron {
	char 	mnt[10];
	char 	jam[10];
	char	tgl[10];
	char	bln[10];
	char	cmd[20];
	char 	alamat;
	char 	status;
//*/
static int set_cron_default(void)
{
	int i;
	struct t_cron * p_gr;
	
	judul(" Set Cron ke Default\r\n");
	
	p_gr = pvPortMalloc( (JML_CRON) * sizeof (struct t_cron) );
	//p_gr = pvPortMalloc( (PER_SUMBER) * sizeof (struct t_dt_set) );
	if (p_gr == NULL)	{
		printf("%s(): Err allok memory gagal !\r\n", __FUNCTION__);
		return -1;
	}
	
	for (i=0; i< (JML_CRON) ; i++)	{
		//defaultnya(p_gr[i]);
		sprintf(p_gr[i].mnt, "-");
		sprintf(p_gr[i].jam, "-");
		sprintf(p_gr[i].tgl, "-");
		sprintf(p_gr[i].bln, "-");
		sprintf(p_gr[i].cmd, "-");
		p_gr[i].alamat = 0;
		p_gr[i].status = 0;
		p_gr[i].set = 0;
	}
	
	if (simpan_cron( p_gr ) < 0) 	{
		vPortFree( p_gr );
		return -1;
	}
	vPortFree( p_gr );
	
}

void defaultnya(struct t_cron * p_cr) {
	sprintf(p_cr->mnt, "-");
	sprintf(p_cr->jam, "-");
	sprintf(p_cr->tgl, "-");
	sprintf(p_cr->bln, "-");
	sprintf(p_cr->cmd, "-");
	p_cr->alamat = '-';
	p_cr->status = '-';
}

int set_cron(int argc, char **argv)
{
	unsigned char str_cron[24];
	int sumb=0;
	int ndata=0;
	unsigned int alamat=0;
	struct t_cron *p_gr;
	int i;
	int y;
	
	if(argc<2) {
		printf(" ERR: argument kurang !\r\n");
		printf(" coba set_cron help \r\n");
		return;
	}
	
	judul(" Setting Cron\r\n");
	
			if (strcmp(argv[1], "help") == 0)	{
				printf(" Perintah untuk setting CRON !\r\n");
				printf(" 1. set_cron help/default\r\n");
				printf("    help    : printout keterangan ini\r\n");
				printf("    default : memberikan default setting simpan file\r\n");
				printf("\r\n");
				
				//*
				printf(" 2. set_cron [no]   [mnt]    [jam]    [tgl]  [bln]    [cmd]                 [alamat] [status] \r\n");
				printf("    set_cron [1-10] [0-59 s] [0-23 s] [1-30] [1-12 s] [cek|ftp|hapus|relay|list|pulsa|bacasms] [1-8]    [1|aktif|0|mati] \r\n");
				printf("\r\n");
				//*/

				return ;
			}
			else if (strcmp(argv[1], "default") == 0)
			{
				printf("set CRON dengan data default !\n");
				set_cron_default();
				
				return;
			}
	
  	display_args(argc,argv);
	
	p_gr = pvPortMalloc( JML_CRON * sizeof (struct t_cron) );
	//printf("JMl cron : %d\r\n", JML_CRON * sizeof (struct t_cron));
	
	
	if (p_gr == NULL) {
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		return -1;
	}
	printf(" %s(): Mallok ok di %X, argc: %d\r\n", __FUNCTION__, p_gr, argc);
	
	memcpy((char *) p_gr, (char *) ALMT_CRON, (JML_CRON * sizeof (struct t_cron)));
	
	//vPortFree( p_gr );
	//return 0;
	
	if (argc>8) {
		//if ()
		sprintf(str_cron, "%s", argv[1]);			// no cron
		sumb = cek_nomer_valid(str_cron, JML_CRON);
		if (sumb <= 0)	{
			printf("No cron salah !\r\n");
			vPortFree( p_gr );
			return 0;	
		}
		
		sprintf(str_cron, "%s", argv[2]);			// menit
		if ((strlen(str_cron) < 10) ) {
			if ((atoi(str_cron)<0 || atoi(str_cron)>60) && (str_cron[0]!='s')) {
				printf("Input menit salah !\r\n");
				vPortFree( p_gr );
				return 0;
			}
		} else {
			printf("Input menit terlalu panjang !\r\n");
			vPortFree( p_gr );
			return 0;
		}
		
		sprintf(str_cron, "%s", argv[3]);			// jam
		if ((strlen(str_cron) < 10) ) {
			if ((atoi(str_cron)<0 || atoi(str_cron)>60) && (str_cron[0]!='s')) {
				printf("Input Jam salah !\r\n");
				vPortFree( p_gr );
				return 0;
			}
		} else {
			printf("Input jam terlalu panjang !\r\n");
			vPortFree( p_gr );
			return 0;
		}
		
		sprintf(str_cron, "%s", argv[4]);			// tgl
		if ((strlen(str_cron) < 10) ) {
			if ( (atoi(str_cron)<1 || atoi(str_cron)>31) && (str_cron[0]!='s') ) {
				printf("Input Tanggal salah !\r\n");
				vPortFree( p_gr );
				return 0;
			}
		} else {
			printf("Input Tanggal terlalu panjang !\r\n");
			vPortFree( p_gr );
			return 0;
		}
		
		sprintf(str_cron, "%s", argv[5]);			// bln
		if ((strlen(str_cron) < 10) ) {
			if ( (atoi(str_cron)<1 || atoi(str_cron)>12) && (str_cron[0]!='s') ) {
				printf("Input Bulan salah !\r\n");
				vPortFree( p_gr );
				return 0;
			}
		} else {
			printf("Input Bulan terlalu panjang !\r\n");
			vPortFree( p_gr );
			return 0;
		}
		
		sprintf(str_cron, "%s", argv[6]);			// cmd
		if( (strcmp(str_cron,"reset")==0) || (strcmp(str_cron,"list")==0) || (strcmp(str_cron,"relay")==0) || (strcmp(str_cron,"ftp")==0) || (strcmp(str_cron,"hapus")==0) || (strcmp(str_cron,"pulsa")== 0) || (strcmp(str_cron,"bacasms")== 0) || (strcmp(str_cron,"pulsatipis")== 0) || (strcmp(str_cron,"cek")==0) ) {
			sprintf(p_gr[sumb-1].cmd, argv[6]);
		} else {
			printf("Input perintah/COMMAND salah !\r\n");
			vPortFree( p_gr );
			return 0;
		}
		//*
		#ifdef PAKAI_SELENOID
		sprintf(str_cron, "%s", argv[7]);			// alamat
		alamat = cek_nomer_valid(str_cron, JML_RELAY);
		if (alamat <= 0)	{
			printf("Alamat salah. Range Relay 1 s/d %d !\r\n", JML_RELAY);
			vPortFree( p_gr );
			return 0;	
		}
		#endif
		
		//*/
		sprintf(str_cron, "%s", argv[8]);			// status
		if ( (strcmp(str_cron, "aktif") == 0) || (str_cron[0]=='1') || (strcmp(str_cron, "hidup") == 0) )	{
			p_gr[sumb-1].status = 1;
		} else if ( (strcmp(str_cron, "mati")==0) || (str_cron[0]=='0') ) {
			printf("perintah cron %d mati !\r\n", sumb);
			p_gr[sumb-1].status = 0;
		} else {
			printf("Input perintah salah !\r\n");
			vPortFree( p_gr );
			return 0;
		}
		
		//sprintf(p_gr[sumb-1].mnt, argv[2]);
		strcpy(p_gr[sumb-1].mnt, argv[2]);
		//printf("isi /menit: %s %s\r\n", p_gr[sumb-1].mnt, argv[2]);
		strcpy(p_gr[sumb-1].jam, argv[3]);
		strcpy(p_gr[sumb-1].tgl, argv[4]);
		strcpy(p_gr[sumb-1].bln, argv[5]);
		p_gr[sumb-1].alamat = (unsigned char) alamat;
		
		//printf("isi %d mnt: %s, jam: %s, tgl: %s, bln: %s\r\n", \ 
		//	sumb, p_gr[sumb-1].mnt, p_gr[sumb-1].jam, p_gr[sumb-1].tgl, p_gr[sumb-1].bln);
		//sprintf(p_gr[sumb-1].cmd, argv[6]);
		//sprintf(p_gr[sumb-1].status, argv[7]);
		
	} else if (argc==3) {
		sprintf(str_cron, "%s", argv[1]);			// alamat
		sumb = cek_nomer_valid(str_cron, JML_CRON);
		if (sumb <= 0)	{
			printf("No cron salah !\r\n");
			vPortFree( p_gr );
			return 0;	
		}
		
		if ( (strcmp(argv[2], "set") == 0) || (strcmp(argv[2], "aktif") == 0) ) {	// 3
			p_gr[sumb-1].set = 1;
			printf(" Sistem cron diaktifkan !\r\n");
		} else if ( (strcmp(argv[2], "unset") == 0) || (strcmp(argv[2], "mati") == 0) ) {	// 3
			p_gr[sumb-1].set = 0;
			printf(" Sistem cron tidak diaktifkan !\r\n");
		} else if ( (strncmp(argv[2], "default",7) == 0)) {	// 3
			sprintf(p_gr[sumb-1].mnt, "-");
			sprintf(p_gr[sumb-1].jam, "-");
			sprintf(p_gr[sumb-1].tgl, "-");
			sprintf(p_gr[sumb-1].bln, "-");
			sprintf(p_gr[sumb-1].cmd, "-");
			p_gr[sumb-1].alamat = 0;
			p_gr[sumb-1].status = 0;
			p_gr[sumb-1].set = 0;
		} else {
			printf(" ERR: perintah tidak benar / argumen kurang !\r\n");
			printf(" coba set_cron help \r\n");
			vPortFree( p_gr );
			return 0;
		}
	} else {
		printf(" ERR: argument kurang !\r\n");
		printf(" coba set_cron help \r\n");
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
	if (simpan_cron( p_gr ) < 0) 	{
		vPortFree( p_gr );
		return -1;
	}
	vPortFree( p_gr );

}

static tinysh_cmd_t set_cron_cmd={0,"set_cron","menampilkan konfigurasi cron",
		"help default ",set_cron,0,0,0};

static int simpan_cron( struct t_cron *pgr)
{
	printf(" Save struct CRON ke flash ..");
	if(prepare_flash(SEKTOR_CRON, SEKTOR_CRON)) return -1;
	printf("..");
	
	if(hapus_flash(SEKTOR_CRON, SEKTOR_CRON)) return -1;
	printf("..");
	
	if(prepare_flash(SEKTOR_CRON, SEKTOR_CRON)) return -1;
	printf("..");
	
	if(tulis_flash(ALMT_CRON, (unsigned short *) pgr, (sizeof (struct t_cron) * (JML_CRON) ))) return -1;
	
	printf(".. OK\r\n");
	return 0;
}


portTASK_FUNCTION(cron_task, pvParameters)	{
	int i=0, j=0;
	vTaskDelay(500);
	printf("Cron : init\r\n");
	
	for(i=0; i<10; i++)
		vTaskDelay(1000);

	for(;;) {
		//*
		#if 1
		#ifdef PAKAI_FILE_SIMPAN
		if (job_cron==1) {
			
			#if 1
			#ifdef PAKAI_GSM_FTP
				if (status_cron=0 && status_modem==0)	{	// biar nggak konflik baca file
					printf("___modem sibuk -cron-___\r\n");
				} else {
					status_cron = 1;
					printf("......HAPUS file J-3\r\n");			
					//cari_berkas("J-2", "lihat");
					cari_berkas("J-3","hapus");
					status_cron = 0;
				}			
			#else
				status_cron = 1;
				printf("......HAPUS file J-3\r\n");			
				//cari_berkas("J-12", "lihat");
				cari_berkas("J-3","hapus");
				status_cron = 0;
			#endif	
			#endif
			job_cron = 0;		// reset kerjaan cron
		}
		#endif
		#endif
		//*/
		/*
		i++;
		if (i>10)	{
			printf("______task cron: %d\r\n", j++);
			i=0;
		}
		//*/
		vTaskDelay(100);
	}
}

void init_task_cron(void)	{
	xTaskCreate( cron_task, ( signed portCHAR * ) "CRON", (configMINIMAL_STACK_SIZE * 4), \
		NULL, tskIDLE_PRIORITY + 1, (xTaskHandle *) &hdl_cron );	
}

#endif

#endif
