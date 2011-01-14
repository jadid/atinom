#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"

//#include <time.h>

#include "../monita/monita_uip.h" 

#ifdef PAKAI_GSM_FTP

#ifndef __SYSTEM_FTP__
#define __SYSTEM_FTP__

#define sleep(c) vTaskDelay(c * 1000)



//#define tulis_serial(str_ftp,0,0) ser2_putstring(str_ftp)
char str_ftp[128];

#define CTRL_ETX	0x03	/* END of TEXT lihat ascii code http://en.wikipedia.org/wiki/ASCII */
#define CTRL_DLE	0x10	/* Data link escape, untuk memagari jika ada karakter ETX yang ikut dikirim */

FIL fd2;
unsigned int files=0;
unsigned int file_sudah=0;
unsigned int file_sukses=0;

//extern int status_modem;

//int gsm_ftp(int argc, char *argv[])
int gsm_ftp()	{
	if (status_modem==1)	{		// modem sibuk
		printf("___modem sibuk___\r\n");
		return;		
	}
	
	int i,flag,oz;
	int c, res;
	int tout;
	
	time_t timeval;
	struct tm tw;
	
	char abs_path[128];
	char pathfile[64];
	
	int fff=0;
	
	files=0;
	file_sudah=0;
	file_sukses=0;
	
	//fff=90;
	fff=konek_ftp_awal();
	if (fff!=90) {
		printf("Koneksi GPRS gagal !!!\r\n");
		printf("Create FTP sesssion error !\r\n");
		tutup_koneksi_ftp(fff);
		status_modem = 0;
		return 0;
	}
	
	
	printf("Create FTP sesssion !\r\n");
	
	flush_modem();
	status_modem = 0;
	return -1;
	
	cari_berkas("J-3", "kirim_ftp");
	
	//status_modem = 0;
	//return 4;
	//printf("path: %s\r\n", pathfile);	
	//sleep(1);
	//vTaskDelay(2000);
	//printf(" File = %d, dikirim %d, sudah dikirim %d\r\n", files, file_sukses, file_sudah); 
	//files=0; file_sudah=0; file_sukses=0;
	//printf("fff: %d\r\n", fff);
	tutup_koneksi_ftp(fff);

	return;
}

int kirim_file_ke_ftp(char *abs_path, char *nf) {
	char posisifile[128];
	char namafile[32];
	int c, res, i, flag, oz;
	unsigned int size;
	unsigned int jum_dirs;
	FILINFO fileInfo;
	char *nama;
	
	time_t timeval;
	struct tm tw;
	
	//sprintf(posisifile, "%s", abs_path);
	//sprintf(namafile, "%s", nf);
	strcpy(posisifile, abs_path);
	strcpy(namafile, nf);
	//printf("filenya: %s, nama: %s\r\n", posisifile, namafile);
	if (res = f_open(&fd2, posisifile, FA_READ | FA_WRITE)) {
		printf("%s(): Buka file error %d !\r\n", __FUNCTION__, res);					
		return 0;
	}
	
	f_lseek( &fd2, fd2.fsize - 6 );
	f_read( &fd2, posisifile, 6, &res);
	//printf("___mo dikirim, CEK %s @@@, nf: %s\r\n", posisifile, namafile);
	files++;
	if (strncmp( posisifile, "SENDED", 6) == 0)  {
		printf("file %s sudah dikirim !\r\n", posisifile);
			file_sudah++;
	}
	else	{
		f_lseek( &fd2, 0);				// kembalikan pointer //

		flag=55;
		//*
		//vTaskDelay(500);
		flush_modem();
		for (oz=0; oz<20; oz++) {
			if (flag == 55) {
				vTaskDelay(10);
				if (upload_file(namafile)==0) {
					flag = 77;
				}
			}
		}

		if (flag==77)		{
			size = sizeof (posisifile);
			for (;;)	{
				f_read( &fd2, posisifile, size, &res);
							
				for (i=0; i<res; i++)		{								
					//tulis_char( abs_path[i] );
					serX_putchar(PAKAI_GSM_FTP, &posisifile[i], 1000);
				}	
		
				if ( res < size ) break; 
			}
						
			// untuk mengakhiri data ftp //
			flag=0;
			for (oz=0; oz<50; oz++) {
				if ( flag == 0) {
					printf("...........send ETX   %d, flag: %d\r\n", oz+1, flag);
					if (send_etx() == 0) {
						flag = 1;
						continue;
					}
					vTaskDelay(500);
				}
			}
			if (flag==0) {
				f_close( &fd2 );
				return 90;
			}
			
			// tulis SENDED pada akhir file //
			sprintf(posisifile, "%s", ctime( &timeval ));	
			sprintf( &posisifile[24], "SENDED");	
			printf("TULIS %s \r\n", posisifile);
									
			f_write( &fd2, posisifile, strlen(posisifile), &res);
			file_sukses++;
		}
		else	{
			printf("Upload %s file ERROR !\r\n", namafile);
			//break;
		}
	}
	f_close( &fd2 );
	vTaskDelay(100);		// kasih waktu buat proses yg lain.	
}

void cek_ftp() {
	if (status_modem==1)	{		// modem sibuk
		printf("___modem sibuk___\r\n");
		return;		
	}
	
	status_modem = 1;
	int fff=0;
	if (fff=konek_ftp_awal()) {
		//printf("_________Konek ke ftp_________%d\r\n", fff);
		tutup_koneksi_ftp(fff);
	} else {
		printf("Gagal konek\r\n");
	}
	status_modem = 0;
}

int konek_ftp_awal() {
	status_modem = 1;		//aktif
	char abs_path[128];
	char path[64];
	int i,flag,oz;
	//printf("Program kirim file lewat FTP GSM\r\n");
	#if ( DEBUG == 1)				
		printf(" enable debug out !!\n");
	#endif
	
	/* flush sekaligus cek jika masih +WIND */
	/*
	ulang:
	//memset(str_ftp, 0, sizeof (str_ftp));
	baca_serial(str_ftp, 100, 0);
	if ( strncmp(str_ftp, "+WIND", 5) == 0) 	{
		goto ulang;
	}
	//*/
	printf("___%s___\r\n", __FUNCTION__);
	flush_modem();
	//baca_serial(str_ftp, 100, 0);

	vTaskDelay(100);
	flag=10;
	for (oz=0; oz<5; oz++) {
		if (flag == 10) {
			printf("...........cek_awal()   %d, flag: %d\r\n", oz+1, flag);
			if (cek_awal() == 0) {
				flag = 1;
				continue;
			}
			vTaskDelay(10);
		}
 
	}
	if (flag==10)	return flag;
	
	vTaskDelay(100);
	flag=20;
	for (oz=0; oz<10; oz++) {
		if (flag == 20) {
			//printf("...........set_cpin()   %d, flag: %d\r\n", oz+1, flag);
			if (set_cpin() == 0) {
				flag = 1;
				continue;
			}
			vTaskDelay(10);
		}
	}
	if (flag==20)	return flag;
	
	vTaskDelay(100);
	flag=30;
	for (oz=0; oz<10; oz++) {
		if (flag == 30) {
			printf("...........set_wipcfg()   %d\r\n", oz+1);
			if (set_wipcfg() == 0) {
				flag = 1;
				continue;
			}
			vTaskDelay(10);
		}
	}
	if (flag==30)	return flag;
	
	vTaskDelay(1000);
	flag=40;
	for (oz=0; oz<10; oz++) {
		if (flag == 40) {
			//printf("...........set_wipbr()   %d\r\n", oz+1);
			if (set_wipbr() == 0) {
				flag = 1;
				continue;
			}
			vTaskDelay(10);
		}
	}
	if (flag==40)	return flag;

	vTaskDelay(100);
	flag=50;		// APN
	for (oz=0; oz<10; oz++) {
		if (flag == 50) {
			//printf("...........set_wipbr_apn()   %d\r\n", oz+1);
			if (set_wipbr_apn() == 0) {
				flag = 1;
				continue;
			}
			vTaskDelay(10);
		}
	}
	if (flag==50)	return flag;
	
	vTaskDelay(100);
	flag=50;		// USER
	for (oz=0; oz<5; oz++) {
		if (flag == 50) {
			//printf("...........set_wipbr_user()   %d\r\n", oz+1);
			if (set_wipbr_user() == 0) {
				flag = 1;
				continue;
			}
			vTaskDelay(10);
		}
	}
	if (flag==50)	return flag;
	
	vTaskDelay(100);
	flag=50;		// PASSWORD
	for (oz=0; oz<5; oz++) {
		if (flag == 50) {
			//printf("...........set_wipbr_passwd()   %d\r\n", oz+1);
			if (set_wipbr_passwd() == 0) {
				flag = 1;
				continue;
			}
			vTaskDelay(10);
		}
	}
	if (flag==50)	return flag;
		
	vTaskDelay(100);
	flag=70;		// GPRS
	for (oz=0; oz<20; oz++) {
		if (flag == 70) {
			printf("...........start gprs()   %d\r\n", oz+1);
			if (start_gprs() == 0) {
				flag = 1;
				continue;
			}
			vTaskDelay(10);
		}
	}
	if (flag==70)	return flag;
	
	//printf("sampe akhir gprs\r\n");
	//status_modem=0;
	//return 0;
	
	vTaskDelay(100);
	flag=80;		// FTP
	for (oz=0; oz<20; oz++) {
		if (flag == 80) {
			printf("...........bikin sesi ftp()   %d\r\n", oz+1);
			if (create_ftp_sess() == 0) {
				flag = 90;
				printf("Create FTP session OK !\r\n");		// nyampe sini !!!
				continue;
			}
			vTaskDelay(10);
		}
	}
	flush_modem();
	return flag;
}

int tutup_koneksi_ftp(int hasil) {
	int flag,oz;
	printf("hasil: %d\r\n", hasil);
	switch (hasil) {
		case 90:
			goto TUTUP_FTP;
			break;
		case 80:
			goto TUTUP_GPRS;
			break;
		case 50:
			goto TUTUP_WIPCFG;
			break;
		default:
			goto TUTUP_WIPCFG;
			break;
	}
	
	TUTUP_FTP:
	flush_modem();
	flag=0;
	for (oz=0; oz<10; oz++) {
		if ( flag == 0) {
			printf("...........stop_sess ftp()    %d\r\n", oz+1);
			if (close_ftp_sess() == 0) {
				flag = 1;
				continue;
			}
			vTaskDelay(10);
		}
	}
	
	status_modem=0;
	return;
	
	TUTUP_GPRS:
	flag=0;
	for (oz=0; oz<5; oz++) {
		if ( flag == 0) {
			printf("...........stop_gprs()    %d\r\n", oz+1);
			if (stop_gprs() == 0) {
				flag = 1;
				continue;
			}
			vTaskDelay(50);
		}
	}
	
	TUTUP_WIPCFG:
	flag=0;
	for (oz=0; oz<5; oz++) {
		if ( flag == 0) {
			//printf("...........stop_tcpip_stack()    %d\r\n", oz+1);
			if (stop_tcpip_stack() == 0) {
				flag = 1;
				continue;
			}
			vTaskDelay(50);
		}
	}
	status_modem = 0;
	printf("modem longgar : %d\r\n", status_modem);
}

int cek_awal(void) {
	/* paksa ATE0, supaya echo disable */
	//tulis_serial("ATE0\r\n", 6, 0);
	serX_putstring(PAKAI_GSM_FTP, "ATE0\r\n");
	
	baca_serial(str_ftp, 12, 10);	/* flush ATE0 jika mungkin*/
	printf("cek_awal FTP GSM\r\n");
	//baca_serial(str_ftp, 12, 10);	/* flush OK */
	
	//tulis_serial("AT\r\n", 4, 0);
	serX_putstring(PAKAI_GSM_FTP, "AT\r\n");
	
	/* harus ada jawaban OK */
	if ( baca_serial(str_ftp, 12, 10) < 0 )	{
		printf("Modem tidak terdeteksi\r\n");
		return -1;
	}
	
	if (strncmp(str_ftp,"AT", 2) == 0)	{
		printf(" Modem ECHO enabled, harus didisable dulu\r\n");
		//tulis_serial("ATE0\r\n", 6, 0);
		serX_putstring(PAKAI_GSM_FTP, "ATE0\r\n");
		
		//memset(str_ftp, 0, sizeof(str_ftp));
		
		/* akan tetap terbaca ATEO */
		if ( baca_serial(str_ftp, 12, 10) < 0 )		{
			printf("Modem tidak respon oleh ATE0 !\r\n");
			return -1;
		}
		
		/* flush OK nya */
		baca_serial(str_ftp, 12, 10);
		
		//tulis_serial("AT\r\n", 4, 0);
		serX_putstring(PAKAI_GSM_FTP, "AT\r\n");
		
		if ( baca_serial(str_ftp, 12, 10) < 0 )		{
			printf("Modem tidak respon oleh ATE0 !\r\n");
			return -1;
		}
	}
	
	if (strncmp(str_ftp, "OK", 2) == 0)	{
		printf(" OK modem sudah siap\r\n");
		return 0;
	}
	else {
		printf(" Entah kenapa !\r\n");
		unset_selenoid(1);
		vTaskDelay(1000);
		set_selenoid(1);
		vTaskDelay(2000);		// kasih jeda buat start up modem
	}
	return -1;
}

int set_cpin(void) {
	/* cek dulu apakah pin sudah dimasukkan ? */	
	//printf("masuk set_cpin\r\n");
	sprintf(str_ftp, "AT+CPIN?\r\n");
	//tulis_serial(str_ftp, strlen(str_ftp), 0);
	serX_putstring(PAKAI_GSM_FTP, str_ftp);
	//printf("setelah masuk set_cpin\r\n");
	baca_serial(str_ftp, 20, 5);
	
	if (strncmp(str_ftp, "+CPIN: READY", 12) == 0 )	{
		printf(" %s(): OK Ready\r\n", __FUNCTION__);
		return 0;
	}
	
	if (strncmp(str_ftp, "+CPIN: SIM PIN", 14) == 0 )	{
		/* pin belum dimasukkan */
		sprintf(str_ftp, "AT+CPIN=1234\r\n");
		//tulis_serial(str_ftp, strlen(str_ftp), 0);	
		serX_putstring(PAKAI_GSM_FTP, str_ftp);
		baca_serial(str_ftp, 20, 5);
	
		if (strncmp(str_ftp, "+CPIN", 5) == 0 || strncmp(str_ftp, "ERROR", 5) == 0)		{
			printf(" %s(): ERR :%s\r\n", __FUNCTION__, str_ftp);
			return -1;
		}	
	
		if (strncmp(str_ftp, "OK", 2) == 0)		{
			printf(" %s(): OK\r\n", __FUNCTION__);
			printf(" %s(): Sleep 10\r\n", __FUNCTION__);			// kalo di sini harus diulang
			/* sleep dulu 5 detik, biasanya ada WIND dll */
			sleep(10);
			if (!cek_awal())
				return 0;
		}
		else	{
			printf(" %s(): ERR ??\r\n", __FUNCTION__);
			return -1;
		}
	}
	else
		return -1;
}

int set_wipcfg(void) {	
	/* nyalakan dulu, jika tidak bisa matikan dulu */
	sprintf(str_ftp, "AT+WIPCFG=1\r\n");
	//tulis_serial(str_ftp, strlen(str_ftp), 0);	
	serX_putstring(PAKAI_GSM_FTP, str_ftp);
	baca_serial(str_ftp, 20, 5);
	
	if (strncmp(str_ftp, "+CME", 4) == 0 || strncmp(str_ftp, "ERROR", 5) == 0)		{
		/* barangkali masih aktif */
		sprintf(str_ftp, "AT+WIPCFG=0\r\n");
		//tulis_serial(str_ftp, strlen(str_ftp), 0);
		serX_putstring(PAKAI_GSM_FTP, str_ftp);
		baca_serial(str_ftp, 20, 10);
		
		if (strncmp(str_ftp, "OK", 2) == 0)		{
			sprintf(str_ftp, "AT+WIPCFG=1\r\n");
			//tulis_serial(str_ftp, strlen(str_ftp), 0);	
			serX_putstring(PAKAI_GSM_FTP, str_ftp);
			baca_serial(str_ftp, 20, 10);
			
			if (strncmp(str_ftp, "OK", 2) != 0)		{
				printf(" %s(): ERR\r\n", __FUNCTION__);
				return -1;
			}
		}
	}
	
	else if (strncmp(str_ftp, "OK", 2) == 0)	{
		printf(" %s(): OK\r\n", __FUNCTION__);
		return 0;
	}
	else if (strncmp(str_ftp, "AT+", 3) == 0)	{
		/* berarti ATE0 nya belum berhasil */
		
	}
	else	{
		printf(" %s(): ERR ??\r\n", __FUNCTION__);
		return -1;
	}
}

int set_wipbr(void) {
	char cmd_ftp[100];
	int rr = -1;
	
	flush_modem();
	sprintf(cmd_ftp, "AT+WIPBR=1,6\r\n");
	//tulis_serial(str_ftp, strlen(str_ftp), 0);	
	serX_putstring(PAKAI_GSM_FTP, cmd_ftp);
	baca_serial(cmd_ftp, 80, 120);
	
	printf("cmd_ftp: %s\r\n", cmd_ftp);
	if (strncmp(cmd_ftp, "AT+WIPBR", 8) == 0)	{
		baca_serial(cmd_ftp, 80, 120);
		printf("respon1 : %s\r\n", cmd_ftp);
	}
	
	if (strncmp(cmd_ftp, "+CME", 4) == 0 || strncmp(cmd_ftp, "ERROR", 5) == 0)	{
		printf(" %s(): ERR :%s\r\n", __FUNCTION__, cmd_ftp);
		//return -1;
	}	
	
	if (strncmp(cmd_ftp, "OK", 2) == 0)	{
		printf(" %s(): OK\r\n", __FUNCTION__);
		rr = 0;
	}
	else	{
		printf(" %s(): ERR ??\r\n", __FUNCTION__);
		//return -1;
	}
	
	if (rr==0) return 0;
	
	baca_serial(cmd_ftp, 80, 20);
	printf("cmd_ftp: %s\r\n", cmd_ftp);
	
	baca_serial(cmd_ftp, 80, 20);
	printf("cmd_ftp: %s\r\n", cmd_ftp);
	
	return rr;
}

//*
int set_wipbr_apn(void) {
	struct t_gsm_ftp *p_dt;
	p_dt = (char *) ALMT_GSM_FTP;

	sprintf(str_ftp, "AT+WIPBR=2,6,11,%s\r\n", p_dt->gprs_apn1);
	//tulis_serial(str_ftp, strlen(str_ftp), 0);	
	serX_putstring(PAKAI_GSM_FTP, str_ftp);
	
	baca_serial(str_ftp, 20, 5);
	
	if (strncmp(str_ftp, "+CME", 4) == 0 || strncmp(str_ftp, "ERROR", 5) == 0)	{
		printf(" %s(): ERR :%s\r\n", __FUNCTION__, str_ftp);
		return -1;
	}	
	
	if (strncmp(str_ftp, "OK", 2) == 0)	{
		printf(" %s(): OK\r\n", __FUNCTION__);
		return 0;
	}
	else	{
		printf(" %s(): ERR ??\r\n", __FUNCTION__);
		return -1;
	}
}

int set_wipbr_user(void) {
	struct t_gsm_ftp *p_dt;
	p_dt = (char *) ALMT_GSM_FTP;

	sprintf(str_ftp, "AT+WIPBR=2,6,0,%s\r\n", p_dt->gprs_user);
	//tulis_serial(str_ftp, strlen(str_ftp), 0);	
	serX_putstring(PAKAI_GSM_FTP, str_ftp);
	
	baca_serial(str_ftp, 20, 5);
	
	if (strncmp(str_ftp, "+CME", 4) == 0 || strncmp(str_ftp, "ERROR", 5) == 0)	{
		printf(" %s(): ERR :%s\r\n", __FUNCTION__, str_ftp);
		return -1;
	}	
	
	if (strncmp(str_ftp, "OK", 2) == 0)	{
		printf(" %s(): OK\r\n", __FUNCTION__);
		return 0;
	}
	else	{
		printf(" %s(): ERR ??\r\n", __FUNCTION__);
		return -1;
	}
}

int set_wipbr_passwd(void) {
	struct t_gsm_ftp *p_dt;
	p_dt = (char *) ALMT_GSM_FTP;

	sprintf(str_ftp, "AT+WIPBR=2,6,1,%s\r\n", p_dt->gprs_passwd);
	//tulis_serial(str_ftp, strlen(str_ftp), 0);	
	serX_putstring(PAKAI_GSM_FTP, str_ftp);
	
	baca_serial(str_ftp, 20, 5);
	
	if (strncmp(str_ftp, "+CME", 4) == 0 || strncmp(str_ftp, "ERROR", 5) == 0)	{
		printf(" %s(): ERR :%s\r\n", __FUNCTION__, str_ftp);
		return -1;
	}	
	
	if (strncmp(str_ftp, "OK", 2) == 0)	{
		printf(" %s(): OK\r\n", __FUNCTION__);
		return 0;
	}
	else	{
		printf(" %s(): ERR ??\r\n", __FUNCTION__);
		return -1;
	}
}
//*/
int start_gprs(void) {
	char cmd_ftp[100];
	int rr=-1;
	
	vTaskDelay(100);
	flush_modem();
	sprintf(cmd_ftp, "AT+WIPBR=4,6,0\r\n");
	
	serX_putstring(PAKAI_GSM_FTP, cmd_ftp);
	
	/* timeout dibuat agak lama */
	baca_serial(cmd_ftp, 80, 50);
	printf("cmd_ftp: %s\r\n", cmd_ftp);	
	if (strncmp(cmd_ftp, "AT+WIPBR", 8) == 0)	{
		baca_serial(cmd_ftp, 80, 50);
		printf("cmd_ftp: %s\r\n", cmd_ftp);	
	}
	
	if (strncmp(str_ftp, "+CME", 4) == 0 || strncmp(str_ftp, "ERROR", 5) == 0)	{
		printf(" %s(): ERR :%s\r\n", __FUNCTION__, str_ftp);
		//return -1;
	}
	
	if (strncmp(str_ftp, "OK", 2) == 0)	{
		printf(" %s(): OK\r\n", __FUNCTION__);
		//return 0;
		rr=0;
	}
	else	{
		printf(" %s(): ERR ??\r\n", __FUNCTION__);
		//return -1;
	}
	/*
	baca_serial(cmd_ftp, 80, 50);
	printf("cmd_ftp: %s\r\n", cmd_ftp);	
	
	baca_serial(cmd_ftp, 80, 50);
	printf("cmd_ftp: %s\r\n", cmd_ftp);	
	//*/
	return rr;
}

int stop_gprs(void) {
	sprintf(str_ftp, "AT+WIPBR=5,6\r\n");
	//tulis_serial(str_ftp, strlen(str_ftp), 0);	
	serX_putstring(PAKAI_GSM_FTP, str_ftp);
	
	baca_serial(str_ftp, 20, 10);
	
	if (strncmp(str_ftp, "AT+", 3) == 0) {		// ambil lagi datanya
		baca_serial(str_ftp, 20, 20);
		if (strncmp(str_ftp, "+CME", 4) == 0 || strncmp(str_ftp, "ERROR", 5) == 0)	{
			printf(" %s(): ERR :%s\r\n", __FUNCTION__, str_ftp);
			return -1;
		}	
		
		if (strncmp(str_ftp, "OK", 2) == 0)	{
			printf(" %s(): OK\r\n", __FUNCTION__);
			return 0;
		}
		else	{
			printf(" %s(): ERR ?? : %s\r\n", __FUNCTION__, str_ftp);
			return -1;
		}
	} else {
		if (strncmp(str_ftp, "+CME", 4) == 0 || strncmp(str_ftp, "ERROR", 5) == 0)	{
			printf(" %s(): ERR :%s\r\n", __FUNCTION__, str_ftp);
			return -1;
		}	
		
		if (strncmp(str_ftp, "OK", 2) == 0)	{
			printf(" %s(): OK\r\n", __FUNCTION__);
			return 0;
		}
		else	{
			printf(" %s(): ERR ?? : %s\r\n", __FUNCTION__, str_ftp);
			return -1;
		}
	}
}

//*
int create_ftp_sess(void) {
	char cmd_ftp[200];
	int rr = -1;
	
	struct t_gsm_ftp *p_dt;
	p_dt = (char *) ALMT_GSM_FTP;
	
	flush_modem();
	sprintf(cmd_ftp, "AT+WIPCREATE=4,1,\"%s\",%d,\"%s\", \"%s\"\r\n", p_dt->ftp_server, p_dt->ftp_port, \
		p_dt->ftp_user, p_dt->ftp_passwd);
	printf("%s\r\n", cmd_ftp);
	
	status_modem = 0;
	return 0;
	
	//tulis_serial(str_ftp, strlen(str_ftp), 0);	
	serX_putstring(PAKAI_GSM_FTP, cmd_ftp);
	
	status_modem = 0;
	return 0;
	
	baca_serial(cmd_ftp, 120, 50);
	printf("respon: %s\r\n", cmd_ftp);
	
	status_modem = 0;
	return 0;
	
	if (strncmp(cmd_ftp, "AT+WIPCREATE", 12) == 0) {		// ambil lagi datanya
		baca_serial(cmd_ftp, 120, 20);
		printf("respon: %s\r\n", cmd_ftp);
	}
	
	status_modem = 0;
	return 0;
	
	if (strncmp(cmd_ftp, "+CME", 4) == 0 || strncmp(cmd_ftp, "ERROR", 5) == 0)	{
		printf(" %s(): ERR :%s\r\n", __FUNCTION__, cmd_ftp);
		//return -1;
	}	
		
	if (strncmp(cmd_ftp, "OK", 2) == 0)	{
		printf(" %s(): FTP SESSION OK\r\n", __FUNCTION__);
		rr = 0;
		//return 0;
	}
	else	{
		printf(" %s(): ERR ?? : %s\r\n", __FUNCTION__, cmd_ftp);
		//return -1;
	}
	return rr;
}
//*/
int upload_file(char *nama_file) {
	char namafile[32];
	char cmd_ftp[100];
	int jadinya=999;
	
	//sprintf(namafile, "%s", nama_file);
	//sprintf(str_ftp, "AT+WIPFILE=4,1,2,\"%s\"\r\n", namafile);
	//printf(str_ftp);
	strcpy(cmd_ftp, "AT+WIPFILE=4,1,2,\"");
	strcat(cmd_ftp, nama_file);
	strcat(cmd_ftp, "\"\r\n");
	
	//tulis_serial(str_ftp, strlen(str_ftp), 0);	
	serX_putstring(PAKAI_GSM_FTP, cmd_ftp);
	
	baca_serial(cmd_ftp, 90, 20);
	printf("respon1: %s\r\n", cmd_ftp);
	
	
	if (strncmp(cmd_ftp, "AT+WIPFILE", 10) == 0) {		// ambil lagi datanya
		strcpy(cmd_ftp, "");
		baca_serial(cmd_ftp, 90, 20);
		printf("respon: %s\r\n", cmd_ftp);
	}
	if (strncmp(str_ftp, "+CME", 4) == 0 || strncmp(str_ftp, "ERROR", 5) == 0)	{
		printf(" %s(): ERR :%s\r\n", __FUNCTION__, str_ftp);
		//return -1;
	}
		
	if (strncmp(str_ftp, "CONNECT", 6) == 0)	{
		printf(" %s(): CONNECT OK\r\n", __FUNCTION__);
		jadinya=0;
		//return 0;
	}
	else	{
		printf(" %s(): ERR ?? : %s\r\n", __FUNCTION__, str_ftp);
		//return -1;
	}
	
	baca_serial(cmd_ftp, 90, 20);
	printf("respon2: %s\r\n", cmd_ftp);
	
	baca_serial(cmd_ftp, 90, 20);
	printf("respon3: %s\r\n", cmd_ftp);
	
	baca_serial(cmd_ftp, 90, 20);
	printf("respon4: %s\r\n", cmd_ftp);
	
	baca_serial(cmd_ftp, 90, 20);
	printf("respon5: %s\r\n", cmd_ftp);
	
	if (jadinya==999)
		return 0;
	else 
		return -1;
	/*
	if (strncmp(cmd_ftp, "AT+WIPFILE", 10) == 0) {		// ambil lagi datanya
		strcpy(cmd_ftp, "");
		baca_serial(cmd_ftp, 90, 20);
		printf("respon: %s\r\n", cmd_ftp);
		if (strncmp(str_ftp, "+CME", 4) == 0 || strncmp(str_ftp, "ERROR", 5) == 0)	{
			printf(" %s(): ERR :%s\r\n", __FUNCTION__, str_ftp);
			return -1;
		}
		
		if (strncmp(str_ftp, "CONNECT", 6) == 0)	{
			printf(" %s(): CONNECT OK\r\n", __FUNCTION__);
			return 0;
		}
		else	{
			printf(" %s(): ERR ?? : %s\r\n", __FUNCTION__, str_ftp);
			return -1;
		}
	} else {
		if (strncmp(str_ftp, "+CME", 4) == 0 || strncmp(str_ftp, "ERROR", 5) == 0)	{
			printf(" %s(): ERR :%s\r\n", __FUNCTION__, str_ftp);
			return -1;
		}	
		
		if (strncmp(str_ftp, "CONNECT", 6) == 0)	{
			printf(" %s(): CONNECT OK\r\n", __FUNCTION__);
			return 0;
		}
		else	{
			printf(" %s(): ERR ?? : %s\r\n", __FUNCTION__, str_ftp);
			return -1;
		}
	}
	//*/
}

int upload_data_file(char *nama_data)	{
	int i;
	char* ch;
	/*
	while( *nama_data != 0)
	{
		// apa ini ???
		//tulis_char( *nama_data++);
		ch++ = nama_data++;
		//serX_putchar(PAKAI_GSM_FTP, &ch++, 1000);
	}
	//*/
	serX_putstring(PAKAI_GSM_FTP, nama_data);
	
	/* untuk mengakhiri data ftp */
	send_etx();
}

int send_etx(void) {
	//tulis_char((char) CTRL_ETX );
	
	// #define CTRL_ETX	0x03
	char ch = 0x03, *p;
	p = &ch;
	//printf();
	//sprintf(str_ftp, "%c", CTRL_ETX);
	//serX_putstring(PAKAI_GSM_FTP, str_ftp);
	serX_putchar(PAKAI_GSM_FTP, p, 1000);
	
	
	baca_serial(str_ftp, 20, 10);
	if (strncmp(str_ftp, "OK", 2) == 0) 	{
		printf(" %s(): OK\r\n", __FUNCTION__);
		return 0;
	} 	else	{
		printf(str_ftp);
		printf(" %s(): ERR ??\r\n", __FUNCTION__);
		return -1;
	}
}

/* seharusnya saat [ETX] / ctrl-c dikirim, koneksi sudah langsung putus */
int close_ftp_sess(void) {
	char cmd_ftp[100];
	int rr=99;
	
	sprintf(cmd_ftp, "AT+WIPCLOSE=4,1\r\n");
	printf("cmd: %s\r\n", cmd_ftp);
	//tulis_serial(str_ftp, strlen(str_ftp), 0);	
	serX_putstring(PAKAI_GSM_FTP, cmd_ftp);
	
	baca_serial(cmd_ftp, 90, 5);
	printf("respon1: %s\r\n", cmd_ftp);
	
	if (strncmp(cmd_ftp, "AT+WIPCLOSE", 11) == 0)	{
		baca_serial(cmd_ftp, 90, 5);
		printf("respon2: %s\r\n", cmd_ftp);
	}
	
	if (strncmp(cmd_ftp, "+CME", 4) == 0 || strncmp(cmd_ftp, "ERROR", 5) == 0)	{
		printf(" %s(): ERR :%s\r\n", __FUNCTION__, cmd_ftp);
		//return -1;
	}	
	
	if (strncmp(cmd_ftp, "OK", 2) == 0)	{
		printf(" %s(): OK\r\n", __FUNCTION__);
		rr=0;
		//return 0;
	}
	else	{
		printf(cmd_ftp);
		printf(" %s(): ERR ??\r\n", __FUNCTION__);
		//return -1;
	}
	
	baca_serial(cmd_ftp, 90, 5);
	printf("respon3: %s\r\n", cmd_ftp);
	
	baca_serial(cmd_ftp, 90, 5);
	printf("respon4: %s\r\n", cmd_ftp);
	
	if (rr==9)	return 0;
	else 		return -1;
}

int stop_tcpip_stack(void) {
	
	flush_modem();
	sprintf(str_ftp, "AT+WIPCFG=0\r\n");
	//tulis_serial(str_ftp, strlen(str_ftp), 0);	
	serX_putstring(PAKAI_GSM_FTP, str_ftp);
	
	baca_serial(str_ftp, 20, 10);
	
	if (strncmp(str_ftp, "+CME", 4) == 0 || strncmp(str_ftp, "ERROR", 5) == 0)	{
		printf(" %s(): ERR :%s\r\n", __FUNCTION__, str_ftp);
		return -1;
	}	
	
	if (strncmp(str_ftp, "OK", 2) == 0)	{
		printf(" %s(): OK\r\n", __FUNCTION__);
		return 0;
	}
	else	{
		printf(" %s(): ERR ??\r\n", __FUNCTION__);
		return -1;
	}	
	
}
#endif
#endif
