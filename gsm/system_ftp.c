#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"

//#include <time.h>

#include "../monita/monita_uip.h" 

#define sleep(c) vTaskDelay(c * 1000)

//#define tulis_serial(buf,0,0) ser2_putstring(buf)
char buf[128];

#define CTRL_ETX	0x03	/* END of TEXT lihat ascii code http://en.wikipedia.org/wiki/ASCII */
#define CTRL_DLE	0x10	/* Data link escape, untuk memagari jika ada karakter ETX yang ikut dikirim */

FIL fd2;
unsigned int files;
unsigned int file_sudah;
unsigned int file_sukses;


//int gsm_ftp(int argc, char *argv[])
int gsm_ftp()
{
	int i,flag,oz;
	int c, res;
	int tout;
	
	time_t timeval;
	struct tm tw;
	
	char abs_path[128];
	char path[64];
//*
	if (konek_ftp_awal()==0) 
	{
		printf("Koneksi GPRS gagal !!!\r\n");
		printf("Create FTP sesssion error !\r\n");
		return 0;
	} else {
		printf("Create FTP sesssion !\r\n");
	}
//*/
	
	
	DIR dirs;
	unsigned int size;
//	unsigned int files;
//	unsigned int file_sudah=0;
//	unsigned int file_sukses=0;
	unsigned int jum_dirs;
	FILINFO fileInfo;
	char *nama;
		
	if (TRUE)
	{
		file_sudah=0;
		file_sukses=0;
		files=0;

		//cari_waktu(path, "j-1");
		cari_berkas("J-1", "kirim_ftp");
	}	
		//printf("path: %s\r\n", path);	
		
/*		
		fileInfo.lfname = buf_lfn;
		fileInfo.lfsize = 255;//sizeof (buf_lfn);

		if ((res = f_opendir (&dirs, path)))
		{ 
			printf("%s(): ERROR = %d\r\n", __FUNCTION__, res);
			return 0;
		} 
		//printf("%s(): Open dir %s OK\r\n", __FUNCTION__, path);

		// MULAI LOOP DIREKTORI //
		for (size = files = jum_dirs = 0;;)
		{
			if (((res = f_readdir (&dirs, &fileInfo)) != FR_OK) || !fileInfo.fname [0]) 
				break;

			if (fileInfo.fattrib & AM_DIR) 
				jum_dirs++;
			else 
			{
				files++; 
				size += fileInfo.fsize;
			
				if (fileInfo.lfname[0] == 0)
					nama = &(fileInfo.fname [0]);
				else
					nama = &(fileInfo.lfname[0]);
				
				sprintf(abs_path,"%s\\%s", path, nama);
				//printf("_________NAMANYA : %s_________________\r\n",abs_path);
				
				if (res = f_open(&fd2, abs_path, FA_READ | FA_WRITE))
				{
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
				f_read( &fd2, abs_path, 6, &res);
				printf("CEK %s @@@\r\n", abs_path);
				
				if (strncmp( abs_path, "SENDED", 6) == 0)
				{
					printf("file %s sudah dikirim !\r\n", nama);
					file_sudah++;
				}
				else
				{				
					// kembalikan pointer //
					f_lseek( &fd2, 0);
					
					//if ( upload_file(nama) == 0)
					if (TRUE)
					{
						//upload_data_file("AAAAAAAAABBBBBBBB");
						size = sizeof (abs_path);
						for (;;)
						{
							f_read( &fd2, abs_path, size, &res);
							
							for (i=0; i<res; i++)
							{
								
								//tulis_char( abs_path[i] );
								serX_putchar(PAKAI_GSM_FTP, &abs_path[i], 1000);
							}	
							//printf("loop res\r\n");
							if ( res < size ) break; 
						}
						
						// untuk mengakhiri data ftp //
						//
						flag=0;
						for (oz=0; oz<5; oz++) {
							if ( flag == 0) {
								printf("...........send ETX   %d\r\n", oz+1);
								//if (send_etx() == 0) {
								if (TRUE) {
									flag = 1;
									continue;
								}
								vTaskDelay(500);
							}
						}

						
						// tulis SENDED pada akhir file //
						sprintf(abs_path, "%s", ctime( &timeval ));	
						sprintf( &abs_path[24], "SENDED");	
						printf("TULIS %s \r\n", abs_path);
											
						f_write( &fd2, abs_path, strlen(abs_path), &res);
						file_sukses++;
					}
					else
					{
						printf("Upload %s file ERROR !\r\n", nama);
						break;
					}
				}
				f_close( &fd2 );
				vTaskDelay(100);		// kasih waktu buat proses yg lain.
			}	// file archive //
			
		}	// loop direktori //	
	}
//*/	
	sleep(1);
	printf(" File = %d, dikirim %d, sudah dikirim %d\r\n", files, file_sukses, file_sudah); 
	
	tutup_koneksi_ftp();

	return;
}

int kirim_file_ke_ftp(char *abs_path) {
	int c, res, i, flag, oz;
	unsigned int size;
//	unsigned int files;
//	unsigned int file_sudah=0;
//	unsigned int file_sukses=0;
	unsigned int jum_dirs;
	FILINFO fileInfo;
	char *nama;
	
	time_t timeval;
	struct tm tw;
	
	if (res = f_open(&fd2, abs_path, FA_READ | FA_WRITE)) {
		printf("%s(): Buka file error %d !\r\n", __FUNCTION__, res);					
		return 0;
	}
	
	f_lseek( &fd2, fd2.fsize - 6 );
	f_read( &fd2, abs_path, 6, &res);
	printf("CEK %s @@@\r\n", abs_path);
	files++;
	if (strncmp( abs_path, "SENDED", 6) == 0)  {
		printf("file %s sudah dikirim !\r\n", abs_path);
			file_sudah++;
	}
	else	{
		// kembalikan pointer //
		f_lseek( &fd2, 0);
					
		//if ( upload_file(nama) == 0)
		if ( upload_file(abs_path) == 0)
		//if (TRUE)
		{
						//upload_data_file("AAAAAAAAABBBBBBBB");
			size = sizeof (abs_path);
			for (;;)
			{
				f_read( &fd2, abs_path, size, &res);
							
				for (i=0; i<res; i++)
				{								
					//tulis_char( abs_path[i] );
					serX_putchar(PAKAI_GSM_FTP, &abs_path[i], 1000);
				}	
		
				if ( res < size ) break; 
			}
						
			// untuk mengakhiri data ftp //
			flag=0;
			for (oz=0; oz<10; oz++) {
				if ( flag == 0) {
					printf("...........send ETX   %d\r\n", oz+1);
					if (send_etx() == 0) {
					//if (TRUE) {
						flag = 1;
						continue;
					}
					vTaskDelay(500);
				}
			}
						
			// tulis SENDED pada akhir file //
			sprintf(abs_path, "%s", ctime( &timeval ));	
			sprintf( &abs_path[24], "SENDED");	
			printf("TULIS %s \r\n", abs_path);
									
			f_write( &fd2, abs_path, strlen(abs_path), &res);
			file_sukses++;
		}
		else
		{
			printf("Upload %s file ERROR !\r\n", nama);
			//break;
		}
	}
	f_close( &fd2 );
	vTaskDelay(100);		// kasih waktu buat proses yg lain.	
}

int cek_ftp() {
	if (konek_ftp_awal()) {
		printf("_________Konek ke ftp_________\r\n");
		tutup_koneksi_ftp();
	} else {
		printf("Gagal konek\r\n");
	}
}

int konek_ftp_awal() {
	char abs_path[128];
	char path[64];
	int i,flag,oz;
	printf("Program kirim file lewat FTP GSM\r\n");
	#if ( DEBUG == 1)				
		printf(" enable debug out !!\n");
	#endif
	
	/* flush sekaligus cek jika masih +WIND */
	ulang:
	memset(buf, 0, sizeof (buf));
	baca_serial(buf, 100, 0);
	if ( strncmp(buf, "+WIND", 5) == 0) 	{
		goto ulang;
	}
	
	//baca_serial(buf, 100, 0);
	/*
	if (cek_awal() < 0) 	{
		printf("Modem tidak terdeteksi !\r\n");
		return;
	}
	//*/
	flag=0;
	for (oz=0; oz<5; oz++) {
		if (flag == 0) {
			printf("...........cek_awal()   %d\r\n", oz+1);
			if (cek_awal() == 0) {
				flag = 1;
				break;
			}
			vTaskDelay(500);
		}
		return flag;
	}
	
	
	/*
	if (set_cpin() < 0) 	{
		printf("Setting CPIN error !\r\n");
		return;
	}
	//*/
	flag=0;
	for (oz=0; oz<5; oz++) {
		if (flag == 0) {
			printf("...........set_cpin()   %d\r\n", oz+1);
			if (set_cpin() == 0) {
				flag = 1;
				break;
			}
			vTaskDelay(500);
		}
		return flag;
	}
	
	/*
	if (set_wipcfg() < 0)	{
		printf("Set wipcfg error !\r\n");
		return;
	}
	//*/
	flag=0;
	for (oz=0; oz<5; oz++) {
		if (flag == 0) {
			printf("...........set_wipcfg()   %d\r\n", oz+1);
			if (set_wipcfg() == 0) {
				flag = 1;
				break;
			}
			vTaskDelay(500);
		}
		return flag;
	}
	
	/* sleep dulu satu detik supaya siap2 */
	printf(" Sleep dulu 1 detik\r\n");
	sleep(1);
	/*
	if (set_wipbr() < 0)	{
		printf("Set wipbr error !\r\n");
		return;
	}
	//*/
	flag=0;
	for (oz=0; oz<5; oz++) {
		if (flag == 0) {
			printf("...........set_wipbr()   %d\r\n", oz+1);
			if (set_wipbr() == 0) {
				flag = 1;
				break;
			}
			vTaskDelay(500);
		}
		return flag;
	}
	
	/*
	if (set_wipbr_apn() < 0) 	{
		printf("Set wipbr_apn error !\r\n");
		return;
	}
	//*/
	flag=0;
	for (oz=0; oz<5; oz++) {
		if (flag == 0) {
			printf("...........set_wipbr_apn()   %d\r\n", oz+1);
			if (set_wipbr_apn() == 0) {
				flag = 1;
				break;
			}
			vTaskDelay(500);
		}
		return flag;
	}
	
	/*
	if (set_wipbr_user() < 0)	{
		printf("Set wipbr_user error !\r\n");
		return;
	}
	//*/
	flag=0;
	for (oz=0; oz<5; oz++) {
		if (flag == 0) {
			printf("...........set_wipbr_user()   %d\r\n", oz+1);
			if (set_wipbr_user() == 0) {
				flag = 1;
				break;
			}
			vTaskDelay(500);
		}
		return flag;
	}
	
	/*
	if (set_wipbr_passwd() < 0) 	{
		printf("Set wipbr_passwd error !\r\n");
		return;
	}
	//*/
	flag=0;
	for (oz=0; oz<5; oz++) {
		if (flag == 0) {
			printf("...........set_wipbr_passwd()   %d\r\n", oz+1);
			if (set_wipbr_passwd() == 0) {
				flag = 1;
				break;
			}
			vTaskDelay(500);
		}
		return flag;
	}
	
	/*
	if (start_gprs() < 0) 	{
		printf("Start GPRS error !\r\n");
		return;
	}
	//*/
	
	flag=0;
	for (oz=0; oz<5; oz++) {
		if (flag == 0) {
			printf("...........start gprs()   %d\r\n", oz+1);
			if (start_gprs() == 0) {
				flag = 1;
				break;
			}
			vTaskDelay(500);
		}
		return flag;
	}
	
	flag=0;
	for (oz=0; oz<5; oz++) {
		if (flag == 0) {
			printf("...........bikin sesi ftp()   %d\r\n", oz+1);
			if (create_ftp_sess() == 0) {
				flag = 1;
				printf("Create FTP session OK !\r\n");		// nyampe sini !!!
				continue;
			}
			vTaskDelay(500);
		}
	}
	return flag;
}

int tutup_koneksi_ftp() {
	int flag,oz;
	flag=0;
	for (oz=0; oz<5; oz++) {
		if ( flag == 0) {
			printf("...........stop_gprs()    %d\r\n", oz+1);
			if (stop_gprs() == 0) {
				flag = 1;
				continue;
			}
			vTaskDelay(500);
		}
	}
	/*
	if (stop_gprs() < 0) 	{
		printf("Stop GPRS error !\r\n");
		return;
	}
	//*/
	vTaskDelay(500);
	flag=0;
	for (oz=0; oz<5; oz++) {
		if ( flag == 0) {
			printf("...........stop_tcpip_stack()    %d\r\n", oz+1);
			if (stop_tcpip_stack() == 0) {
				flag = 1;
				continue;
			}
			vTaskDelay(500);
		}
	}
	/*
	if (stop_tcpip_stack() < 0) 	{
		printf("Stop TCP/IP error !\r\n");
		return;
	}
	//*/
}

int cek_awal(void) {
	//char buf[32];
	/* paksa ATE0, supaya echo disable */
	//tulis_serial("ATE0\r\n", 6, 0);
	serX_putstring(PAKAI_GSM_FTP, "ATE0\r\n");
	
	baca_serial(buf, 12, 10);	/* flush ATE0 jika mungkin*/
	printf("cek_awal FTP GSM\r\n");
	//baca_serial(buf, 12, 10);	/* flush OK */
	
	//tulis_serial("AT\r\n", 4, 0);
	serX_putstring(PAKAI_GSM_FTP, "AT\r\n");
	
	/* harus ada jawaban OK */
	if ( baca_serial(buf, 12, 10) < 0 )
	{
		printf("Modem tidak terdeteksi\r\n");
		return -1;
	}
	
	if (strncmp(buf,"AT", 2) == 0)
	{
		printf(" Modem ECHO enabled, harus didisable dulu\r\n");
		//tulis_serial("ATE0\r\n", 6, 0);
		serX_putstring(PAKAI_GSM_FTP, "ATE0\r\n");
		
		memset(buf, 0, sizeof(buf));
		
		/* akan tetap terbaca ATEO */
		if ( baca_serial(buf, 12, 10) < 0 )
		{
			printf("Modem tidak respon oleh ATE0 !\r\n");
			return -1;
		}
		
		/* flush OK nya */
		baca_serial(buf, 12, 10);
		
		//tulis_serial("AT\r\n", 4, 0);
		serX_putstring(PAKAI_GSM_FTP, "AT\r\n");
		
		if ( baca_serial(buf, 12, 10) < 0 )
		{
			printf("Modem tidak respon oleh ATE0 !\r\n");
			return -1;
		}
	}
	
	if (strncmp(buf, "OK", 2) == 0)
	{
		printf(" OK modem sudah siap\r\n");
		return 0;
	}
	else
		printf(" Entah kenapa !\r\n");
	
	return -1;
}

int set_cpin(void) {
	/* cek dulu apakah pin sudah dimasukkan ? */	
	//printf("masuk set_cpin\r\n");
	sprintf(buf, "AT+CPIN?\r\n");
	//tulis_serial(buf, strlen(buf), 0);
	serX_putstring(PAKAI_GSM_FTP, buf);
	//printf("setelah masuk set_cpin\r\n");
	baca_serial(buf, 20, 5);
	
	if (strncmp(buf, "+CPIN: READY", 12) == 0 )	
	{
		printf(" %s(): OK Ready\r\n", __FUNCTION__);
		return 0;
	}
	
	if (strncmp(buf, "+CPIN: SIM PIN", 14) == 0 )	
	{
		/* pin belum dimasukkan */
		sprintf(buf, "AT+CPIN=1234\r\n");
		//tulis_serial(buf, strlen(buf), 0);	
		serX_putstring(PAKAI_GSM_FTP, buf);
		baca_serial(buf, 20, 5);
	
		if (strncmp(buf, "+CPIN", 5) == 0 || strncmp(buf, "ERROR", 5) == 0)	
		{
			printf(" %s(): ERR :%s\r\n", __FUNCTION__, buf);
			return -1;
		}	
	
		if (strncmp(buf, "OK", 2) == 0)
		{
			printf(" %s(): OK\r\n", __FUNCTION__);
			printf(" %s(): Sleep 10\r\n", __FUNCTION__);			// kalo di sini harus diulang
			/* sleep dulu 5 detik, biasanya ada WIND dll */
			sleep(10);
			if (!cek_awal())
				return 0;
		}
		else
		{
			printf(" %s(): ERR ??\r\n", __FUNCTION__);
			return -1;
		}
	}
	else
		return -1;
}

int set_wipcfg(void) {	
	/* nyalakan dulu, jika tidak bisa matikan dulu */
	sprintf(buf, "AT+WIPCFG=1\r\n");
	//tulis_serial(buf, strlen(buf), 0);	
	serX_putstring(PAKAI_GSM_FTP, buf);
	baca_serial(buf, 20, 5);
	
	if (strncmp(buf, "+CME", 4) == 0 || strncmp(buf, "ERROR", 5) == 0)	
	{
		/* barangkali masih aktif */
		sprintf(buf, "AT+WIPCFG=0\r\n");
		//tulis_serial(buf, strlen(buf), 0);
		serX_putstring(PAKAI_GSM_FTP, buf);
		baca_serial(buf, 20, 10);
		
		if (strncmp(buf, "OK", 2) == 0)	
		{
			sprintf(buf, "AT+WIPCFG=1\r\n");
			//tulis_serial(buf, strlen(buf), 0);	
			serX_putstring(PAKAI_GSM_FTP, buf);
			baca_serial(buf, 20, 10);
			
			if (strncmp(buf, "OK", 2) != 0)
			{
				printf(" %s(): ERR\r\n", __FUNCTION__);
				return -1;
			}
		}
	}
	
	else if (strncmp(buf, "OK", 2) == 0)
	{
		printf(" %s(): OK\r\n", __FUNCTION__);
		return 0;
	}
	else if (strncmp(buf, "AT+", 3) == 0)
	{
		/* berarti ATE0 nya belum berhasil */
		
	}
	else
	{
		printf(" %s(): ERR ??\r\n", __FUNCTION__);
		return -1;
	}
}

int set_wipbr(void) {
	sprintf(buf, "AT+WIPBR=1,6\r\n");
	//tulis_serial(buf, strlen(buf), 0);	
	serX_putstring(PAKAI_GSM_FTP, buf);
	baca_serial(buf, 20, 5);
	
	if (strncmp(buf, "+CME", 4) == 0 || strncmp(buf, "ERROR", 5) == 0)	
	{
		printf(" %s(): ERR :%s\r\n", __FUNCTION__, buf);
		return -1;
	}	
	
	if (strncmp(buf, "OK", 2) == 0)
	{
		printf(" %s(): OK\r\n", __FUNCTION__);
		return 0;
	}
	else
	{
		printf(" %s(): ERR ??\r\n", __FUNCTION__);
		return -1;
	}
}

//*
int set_wipbr_apn(void) {
	struct t_gsm_ftp *p_dt;
	p_dt = (char *) ALMT_GSM_FTP;
	//char buf[64];
	sprintf(buf, "AT+WIPBR=2,6,11,%s\r\n", p_dt->gprs_apn1);
	//tulis_serial(buf, strlen(buf), 0);	
	serX_putstring(PAKAI_GSM_FTP, buf);
	
	baca_serial(buf, 20, 5);
	
	if (strncmp(buf, "+CME", 4) == 0 || strncmp(buf, "ERROR", 5) == 0)	
	{
		printf(" %s(): ERR :%s\r\n", __FUNCTION__, buf);
		return -1;
	}	
	
	if (strncmp(buf, "OK", 2) == 0)
	{
		printf(" %s(): OK\r\n", __FUNCTION__);
		return 0;
	}
	else
	{
		printf(" %s(): ERR ??\r\n", __FUNCTION__);
		return -1;
	}
}

int set_wipbr_user(void) {
	struct t_gsm_ftp *p_dt;
	p_dt = (char *) ALMT_GSM_FTP;
	//char buf[64];
	sprintf(buf, "AT+WIPBR=2,6,0,%s\r\n", p_dt->gprs_user);
	//tulis_serial(buf, strlen(buf), 0);	
	serX_putstring(PAKAI_GSM_FTP, buf);
	
	baca_serial(buf, 20, 5);
	
	if (strncmp(buf, "+CME", 4) == 0 || strncmp(buf, "ERROR", 5) == 0)	
	{
		printf(" %s(): ERR :%s\r\n", __FUNCTION__, buf);
		return -1;
	}	
	
	if (strncmp(buf, "OK", 2) == 0)
	{
		printf(" %s(): OK\r\n", __FUNCTION__);
		return 0;
	}
	else
	{
		printf(" %s(): ERR ??\r\n", __FUNCTION__);
		return -1;
	}
}

int set_wipbr_passwd(void) {
	struct t_gsm_ftp *p_dt;
	p_dt = (char *) ALMT_GSM_FTP;
	//char buf[64];
	sprintf(buf, "AT+WIPBR=2,6,1,%s\r\n", p_dt->gprs_passwd);
	//tulis_serial(buf, strlen(buf), 0);	
	serX_putstring(PAKAI_GSM_FTP, buf);
	
	baca_serial(buf, 20, 5);
	
	if (strncmp(buf, "+CME", 4) == 0 || strncmp(buf, "ERROR", 5) == 0)	
	{
		printf(" %s(): ERR :%s\r\n", __FUNCTION__, buf);
		return -1;
	}	
	
	if (strncmp(buf, "OK", 2) == 0)
	{
		printf(" %s(): OK\r\n", __FUNCTION__);
		return 0;
	}
	else
	{
		printf(" %s(): ERR ??\r\n", __FUNCTION__);
		return -1;
	}
}
//*/
int start_gprs(void) {
	//char buf[64];
	sprintf(buf, "AT+WIPBR=4,6,0\r\n");
	
	//tulis_serial(buf, strlen(buf), 0);	
	serX_putstring(PAKAI_GSM_FTP, buf);
	
	/* timeout dibuat agak lama */
	baca_serial(buf, 20, 50);
	
	if (strncmp(buf, "+CME", 4) == 0 || strncmp(buf, "ERROR", 5) == 0)	
	{
		printf(" %s(): ERR :%s\r\n", __FUNCTION__, buf);
		return -1;
	}
	
	if (strncmp(buf, "OK", 2) == 0)
	{
		printf(" %s(): OK\r\n", __FUNCTION__);
		return 0;
	}
	else
	{
		printf(" %s(): ERR ??\r\n", __FUNCTION__);
		return -1;
	}
}

int stop_gprs(void) {
	sprintf(buf, "AT+WIPBR=5,6\r\n");
	//tulis_serial(buf, strlen(buf), 0);	
	serX_putstring(PAKAI_GSM_FTP, buf);
	
	baca_serial(buf, 20, 10);
	
	if (strncmp(buf, "+CME", 4) == 0 || strncmp(buf, "ERROR", 5) == 0)	
	{
		printf(" %s(): ERR :%s\r\n", __FUNCTION__, buf);
		return -1;
	}	
	
	if (strncmp(buf, "OK", 2) == 0)
	{
		printf(" %s(): OK\r\n", __FUNCTION__);
		return 0;
	}
	else
	{
		printf(buf);
		printf(" %s(): ERR ??\r\n", __FUNCTION__);
		return -1;
	}
}

//*
int create_ftp_sess(void) {
	struct t_gsm_ftp *p_dt;
	p_dt = (char *) ALMT_GSM_FTP;
	//char buf[128];
	sprintf(buf, "AT+WIPCREATE=4,1,\"%s\",%d,\"%s\", \"%s\"\r\n", p_dt->ftp_server, p_dt->ftp_port, \
		p_dt->ftp_user, p_dt->ftp_passwd);
	printf(buf);
	//tulis_serial(buf, strlen(buf), 0);	
	serX_putstring(PAKAI_GSM_FTP, buf);
	
	// timeout 10 menit jika koneksi buruk //
	baca_serial(buf, 20, 120);
	
	if (strncmp(buf, "+CME", 4) == 0 || strncmp(buf, "ERROR", 5) == 0)	 {
		printf(" %s(): ERR :%s\r\n", __FUNCTION__, buf);
		return -1;
	}	
	
	if (strncmp(buf, "OK", 2) == 0) {
		printf(" %s(): FTP SESSION OK\r\n", __FUNCTION__);
		return 0;
	}
	else
	{
		printf(" %s(): ERR ??\r\n", __FUNCTION__);
		return -1;
	}
}
//*/
int upload_file(char *nama_file) {
	//char buf[64];
	//struct t_gsm_ftp *p_dt;
	//p_dt = (char *) ALMT_GSM_FTP;
	//printf("Nama file upload_file: %s", nama_file);
	sprintf(buf, "AT+WIPFILE=4,1,2,\"%s.txt\"\r\n", nama_file);
	//printf(buf);
	//tulis_serial(buf, strlen(buf), 0);	
	serX_putstring(PAKAI_GSM_FTP, buf);
	
	baca_serial(buf, 20, 20);
	
	if (strncmp(buf, "+CME", 4) == 0 || strncmp(buf, "ERROR", 5) == 0)	
	{
		printf(" %s(): ERR :%s\r\n", __FUNCTION__, buf);
		return -1;
	}	
	
	if (strncmp(buf, "CONNECT", 2) == 0)
	{
		printf(" %s(): CONNECT OK\r\n", __FUNCTION__);
		return 0;
	}
	else
	{
		printf(" %s(): ERR ??\r\n", __FUNCTION__);
		return -1;
	}
}

int upload_data_file(char *nama_data)
{
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
	//sprintf(buf, "%c", CTRL_ETX);
	//serX_putstring(PAKAI_GSM_FTP, buf);
	serX_putchar(PAKAI_GSM_FTP, p, 1000);
	
	
	baca_serial(buf, 20, 10);
	if (strncmp(buf, "OK", 2) == 0) 	{
		printf(" %s(): OK\r\n", __FUNCTION__);
		return 0;
	} 	else	{
		printf(buf);
		printf(" %s(): ERR ??\r\n", __FUNCTION__);
		return -1;
	}
}

/* seharusnya saat [ETX] / ctrl-c dikirim, koneksi sudah langsung putus */
int close_ftp_sess(void) {
	//char buf[64];
	sprintf(buf, "AT+WIPCLOSE=4,1\r\n");
	//tulis_serial(buf, strlen(buf), 0);	
	serX_putstring(PAKAI_GSM_FTP, buf);
	
	baca_serial(buf, 20, 5);
	
	if (strncmp(buf, "+CME", 4) == 0 || strncmp(buf, "ERROR", 5) == 0)	
	{
		printf(" %s(): ERR :%s\r\n", __FUNCTION__, buf);
		return -1;
	}	
	
	if (strncmp(buf, "OK", 2) == 0)
	{
		printf(" %s(): OK\r\n", __FUNCTION__);
		return 0;
	}
	else
	{
		printf(buf);
		printf(" %s(): ERR ??\r\n", __FUNCTION__);
		return -1;
	}
}

int stop_tcpip_stack(void) {
	//char buf[64];
	sprintf(buf, "AT+WIPCFG=0\r\n");
	//tulis_serial(buf, strlen(buf), 0);	
	serX_putstring(PAKAI_GSM_FTP, buf);
	
	baca_serial(buf, 20, 10);
	
	if (strncmp(buf, "+CME", 4) == 0 || strncmp(buf, "ERROR", 5) == 0)	
	{
		printf(" %s(): ERR :%s\r\n", __FUNCTION__, buf);
		return -1;
	}	
	
	if (strncmp(buf, "OK", 2) == 0)
	{
		printf(" %s(): OK\r\n", __FUNCTION__);
		return 0;
	}
	else
	{
		printf(" %s(): ERR ??\r\n", __FUNCTION__);
		return -1;
	}	
	
}
