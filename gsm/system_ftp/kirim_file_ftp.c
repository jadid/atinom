/*

	program untuk mengirim file ke server
	sebagai bagian dari solusi Monita Performance Monitoring
	System
	
	furkan jadid
	30 Desember 2009
	daun biru engineering
	
	Alur :
	Program ini akan dipanggil oleh cron setiap 1 jam atau setengah jam
	tergantung setting cron.
	
	Program akan membuka port modem (command line), setting GPRS, aktifkan GPRS
	dan mengirimkan file dengan protokol ftp ke server daun biru, kemudian
	mematikan GPRS dan close PORT. 
	
	contoh pemakaian :
	kirim_file_ftp -f NAMA_FILE -d /dev/ttyUSB0 
	
	jika tidak ada argumen berari untuk testing dimana file akan dikirim
	test_kirim_ftp dan pada /dev/ttyUSB0
	
	Asumsi :
	GSM telkomsel kartu hallo
	* 
	
	mode harus non-canonical, supaya bisa cek tiap2 karakter yang masuk
  
	code2 serial kebanyakan didapat dari :
	http://www.faqs.org/docs/Linux-HOWTO/Serial-Programming-HOWTO.html
	
	19 Jan 2010
	dicoba untuk tampilan_lipi
	
	10 Mar 2010
	jika sudah connect, looping di direktori sejam yang lalu
	dan mengirimkan file2 sejam yang lalu.
	
	Jika error, maka diulang lagi 5 menit kemudian.
	setiap file yang sudah berhasil dikirim, diberi FLAG waktu saat dikirim
	di akhir file
	 	
*/

#if (PAKAI_GSM_FTP == 1)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if ( PAKAI_LINUX == 1 )
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>

#else

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
//#include "direktori.c"

FIL fd2;
extern char buf_lfn[];

#define sleep(c) vTaskDelay(c * 1000)
//#define gettimeofday_r(c) do{ } while(0)

/*
time_t time(time_t * tt)
{
	tt = 21;
}
*/

#endif


#define DEBUG	1
#include "serial.c"


//#define FTP_SERVER 	"daunbiru.dynalias.com"
//#define FTP_USER	"monitadata"
//#define FTP_PASSWD	"diesel2009"
#define FTP_SERVER 	"ftp.daunbiru.com"
#define FTP_USER	"malingping@daunbiru.com"
#define FTP_PASSWD	"diesel"
#define FTP_PORT	21

#define GPRS_APN2	"telkomsel"
#define GPRS_APN1	"internet"
#define GPRS_USER	"wap"
#define GPRS_PASSWD	"wap123"

#define CTRL_ETX	0x03	/* END of TEXT lihat ascii code http://en.wikipedia.org/wiki/ASCII */
#define CTRL_DLE	0x10	/* Data link escape, untuk memagari jika ada karakter ETX yang ikut dikirim */

char MODEMDEVICE[32] = {"/dev/ttyUSB0"};
char NAMA_FILE[128] = {"test_kirim_ftp"};
char buf[128];

int fd;

int cek_awal(void);
int set_cpin(void);
int set_wipbr(void);
int set_wipbr_apn(void);
int set_wipbr_passwd(void);
int set_wipbr_user(void);
int start_gprs(void);

int upload_file(char *nama_file);
int upload_data_file(char *nama_data);

int send_etx(void);

int gsm_ftp(int argc, char *argv[])
{
	int i;
	int c, res;
	int tout;
	time_t timeval;
	//struct tm tw;
	struct tw;
	
	char abs_path[128];
	char path[64];
	
	printf("Program kirim file lewat FTP GSM\r\n");
	#if ( DEBUG == 1)				
		printf(" enable debug out !!\n");
	#endif
	
	/* power on modem */
	/* cek wind */
	/* masukkan pin (jika ada) */
	
	// cek argumen
    for (i=0; i<argc; i++)
	{
		if (argv[i][0] == '-')
		{
			if (argv[i][1] == 'd')
			{
				sprintf(MODEMDEVICE, "%s", argv[i+1]);				
				#if ( DEBUG == 1)					
					printf(" dev = %s\n", MODEMDEVICE);
				#endif
			}
			if (argv[i][1] == 'f')
			{
				sprintf(NAMA_FILE, "%s", argv[i+1]);
				#if ( DEBUG == 1)	
					printf(" file = %s\n", NAMA_FILE);
				#endif
				
			}
		}
	}	
	
	buka_serial( MODEMDEVICE );
	
	#if ( DEBUG == 1)
	printf(" Flush serial !\r\n");
	#endif
	
	/* flush sekaligus cek jika masih +WIND */
	ulang:
	memset(buf, 0, sizeof (buf));
	baca_serial(buf, 100, 0);
	if ( strncmp(buf, "+WIND", 5) == 0)
	{
		goto ulang;
	}
	
	//baca_serial(buf, 100, 0);
	
	if (cek_awal() < 0)
	{
		printf("Modem tidak terdeteksi !\r\n");
		return;
	}
	
	if ( set_cpin() < 0)
	{
		printf("Setting CPIN error !\r\n");
		return;
	}
	
	if (set_wipcfg() < 0)
	{
		printf("Set wipcfg error !\r\n");
		return;
	}
	/* sleep dulu satu detik supaya siap2 */
	printf(" Sleep dulu 1 detik\r\n");
	sleep(1);
	
	if (set_wipbr() < 0)
	{
		printf("Set wipbr error !\r\n");
		return;
	}
	
	if (set_wipbr_apn() < 0)
	{
		printf("Set wipbr_apn error !\r\n");
		return;
	}
	
	if (set_wipbr_user() < 0)
	{
		printf("Set wipbr_user error !\r\n");
		return;
	}
	
	if (set_wipbr_passwd() < 0)
	{
		printf("Set wipbr_passwd error !\r\n");
		return;
	}
	
	if (start_gprs() < 0)
	{
		printf("Start GPRS error !\r\n");
		return;
	}
	
	DIR dirs;
	unsigned int size;
	unsigned int files;
	unsigned int file_sudah=0;
	unsigned int file_sukses=0;
	unsigned int jum_dirs;
	FILINFO fileInfo;
	char *nama;
		
	if (create_ftp_sess() == 0)
	{
		printf("Create FTP sesssion OK !\r\n");
		
		//(void)time(&timeval); 
		get_tm_time( &tw );
		timeval = mktime( &tw );
		
		sprintf( path, "%s", cari_sejam_lalu( timeval ));		
		//sprintf(buf_nama, "file_%d.chc", timeval);		
		
		fileInfo.lfname = buf_lfn;
		fileInfo.lfsize = 255;//sizeof (buf_lfn);
	
		if ((res = f_opendir (&dirs, path)))
		{ 
			printf("%s(): ERROR = %d\r\n", __FUNCTION__, res);
			return 0;
		}
		printf("%s(): Open dir %s OK\r\n", __FUNCTION__, path);
		
		/* MULAI LOOP DIREKTORI */
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
				
				/*		
				printf ("\r\n%c%c%c%c%c %u/%02u/%02u %02u:%02u %9u  %s",
					(fileInfo.fattrib & AM_DIR) ? 'D' : '-',
					(fileInfo.fattrib & AM_RDO) ? 'R' : '-',
					(fileInfo.fattrib & AM_HID) ? 'H' : '-',
					(fileInfo.fattrib & AM_SYS) ? 'S' : '-',
					(fileInfo.fattrib & AM_ARC) ? 'A' : '-',
					(fileInfo.fdate >> 9) + 1980, (fileInfo.fdate >> 5) & 15, fileInfo.fdate & 31,
					(fileInfo.ftime >> 11), (fileInfo.ftime >> 5) & 63,
					fileInfo.fsize, nama);
				*/
				sprintf(abs_path,"%s\\%s", path, nama);
				
				if (res = f_open(&fd2, abs_path, FA_READ | FA_WRITE))
				{
					printf("%s(): Buka file error %d !\r\n", __FUNCTION__, res);					
					return 0;
				}
				
				/* cek apakah sudah pernah dikirim ke server 
				 * sudah dikirim akan ada tag time dan SEND
				 * dan ini akan berada di akhir file
				 * 
				 * sehingga kira2 begini 
				 * Thu Mar 11 13:27:42 2010SEND
				 * 
				 * kira2 29 karakater dari akhir file
				 * 
				 * sehingga hanya perlu dicek 6 karakter terakhir saja
				 * apakah ada SENDED
				 */
				
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
					/* kembalikan pointer */
					f_lseek( &fd2, 0);
					
					if ( upload_file(nama) == 0)
					{
						//upload_data_file("AAAAAAAAABBBBBBBB");
						size = sizeof (abs_path);
						for (;;)
						{
							f_read( &fd2, abs_path, size, &res);
							
							for (i=0; i<res; i++)
							{
								tulis_char( abs_path[i] );
							}	
						
							if ( res < size ) break; 
						}
						
						/* untuk mengakhiri data ftp */
						if ( send_etx() < 0)
						{
							break;
						}
						
						/* tulis SENDED pada akhir file */
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
				
			}	/* file archive */
		}	/* loop direktori */	
	}
	else
		printf("Create FTP sesssion error !\r\n");
	
	
	sleep(5);
	printf(" File = %d, dikirim %d, sudah dikirim %d\r\n", files, file_sukses, file_sudah); 
	
	if (stop_gprs() < 0)
	{
		printf("Stop GPRS error !\r\n");
		return;
	}
	
	sleep(5);
	if (stop_tcpip_stack() < 0)
	{
		printf("Stop TCP/IP error !\r\n");
		return;
	}
	
	
	/* power off device */
	
	//printf("Modem OK");
	
	/*
	tulis_serial( "AT\r", 3, 100);
	baca_serial( buf, 10, 10);
	printf("didapat = %s\r\n", buf);
	*/
	tutup_serial();
	
	return;
}

int cek_awal(void)
{
	//char buf[32];
	/* paksa ATE0, supaya echo disable */
	tulis_serial("ATE0\r\n", 6, 0);
	baca_serial(buf, 12, 10);	/* flush ATE0 jika mungkin*/
	baca_serial(buf, 12, 10);	/* flush OK */
	
	tulis_serial("AT\r\n", 4, 0);
	
	/* harus ada jawaban OK */
	if ( baca_serial(buf, 12, 10) < 0 )
	{
		printf("Modem tidak terdeteksi\r\n");
		return -1;
	}
	
	if (strncmp(buf,"AT", 2) == 0)
	{
		printf(" Modem ECHO enabled, harus didisable dulu\r\n");
		tulis_serial("ATE0\r\n", 6, 0);
		memset(buf, 0, sizeof(buf));
		
		/* akan tetap terbaca ATEO */
		if ( baca_serial(buf, 12, 10) < 0 )
		{
			printf("Modem tidak respon oleh ATE0 !\r\n");
			return -1;
		}
		
		/* flush OK nya */
		baca_serial(buf, 12, 10);
		
		tulis_serial("AT\r\n", 4, 0);
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

int set_cpin(void)
{
	/* cek dulu apakah pin sudah dimasukkan ? */	
	sprintf(buf, "AT+CPIN?\r\n");
	tulis_serial(buf, strlen(buf), 0);	
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
		tulis_serial(buf, strlen(buf), 0);	
		baca_serial(buf, 20, 5);
	
		if (strncmp(buf, "+CPIN", 5) == 0 || strncmp(buf, "ERROR", 5) == 0)	
		{
			printf(" %s(): ERR :%s\r\n", __FUNCTION__, buf);
			return -1;
		}	
	
		if (strncmp(buf, "OK", 2) == 0)
		{
			printf(" %s(): OK\r\n", __FUNCTION__);
			printf(" %s(): Sleep 10\r\n", __FUNCTION__);
			/* sleep dulu 10 detik, biasanya ada WIND dll */
			sleep(10);
			
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

int set_wipcfg(void)
{	
	/* nyalakan dulu, jika tidak bisa matikan dulu */
	sprintf(buf, "AT+WIPCFG=1\r\n");
	tulis_serial(buf, strlen(buf), 0);	
	baca_serial(buf, 20, 5);
	
	if (strncmp(buf, "+CME", 4) == 0 || strncmp(buf, "ERROR", 5) == 0)	
	{
		/* barangkali masih aktif */
		sprintf(buf, "AT+WIPCFG=0\r\n");
		tulis_serial(buf, strlen(buf), 0);
		baca_serial(buf, 20, 10);
		
		if (strncmp(buf, "OK", 2) == 0)	
		{
			sprintf(buf, "AT+WIPCFG=1\r\n");
			tulis_serial(buf, strlen(buf), 0);	
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

int set_wipbr(void)
{
	sprintf(buf, "AT+WIPBR=1,6\r\n");
	tulis_serial(buf, strlen(buf), 0);	
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

int set_wipbr_apn(void)
{
	//char buf[64];
	sprintf(buf, "AT+WIPBR=2,6,11,%s\r\n", GPRS_APN1);
	tulis_serial(buf, strlen(buf), 0);	
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

int set_wipbr_user(void)
{
	//char buf[64];
	sprintf(buf, "AT+WIPBR=2,6,0,%s\r\n", GPRS_USER);
	tulis_serial(buf, strlen(buf), 0);	
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

int set_wipbr_passwd(void)
{
	//char buf[64];
	sprintf(buf, "AT+WIPBR=2,6,1,%s\r\n", GPRS_PASSWD);
	tulis_serial(buf, strlen(buf), 0);	
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

int start_gprs(void)
{
	//char buf[64];
	sprintf(buf, "AT+WIPBR=4,6,0\r\n");
	tulis_serial(buf, strlen(buf), 0);	
	
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

int stop_gprs(void)
{
	sprintf(buf, "AT+WIPBR=5,6\r\n");
	tulis_serial(buf, strlen(buf), 0);	
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

int create_ftp_sess(void)
{
	//char buf[128];
	sprintf(buf, "AT+WIPCREATE=4,1,\"%s\",%d,\"%s\", \"%s\"\r\n", FTP_SERVER, FTP_PORT, \
		FTP_USER, FTP_PASSWD);
		
	tulis_serial(buf, strlen(buf), 0);	
	
	/* timeout 10 menit jika koneksi buruk */
	baca_serial(buf, 20, 120);
	
	if (strncmp(buf, "+CME", 4) == 0 || strncmp(buf, "ERROR", 5) == 0)	
	{
		printf(" %s(): ERR :%s\r\n", __FUNCTION__, buf);
		return -1;
	}	
	
	if (strncmp(buf, "OK", 2) == 0)
	{
		printf(" %s(): FTP SESSION OK\r\n", __FUNCTION__);
		return 0;
	}
	else
	{
		printf(" %s(): ERR ??\r\n", __FUNCTION__);
		return -1;
	}
}

/* nama_file adalah nama file yang akan ditulis di server penerima */
int upload_file(char *nama_file)
{
	//char buf[64];
	sprintf(buf, "AT+WIPFILE=4,1,2,\"%s\"\r\n", nama_file);
	tulis_serial(buf, strlen(buf), 0);	
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

/* nama_data adalah nama file pada lokal file disini */
int upload_data_file(char *nama_data)
{
	int i;
	
	while( *nama_data != 0)
	{
		tulis_char( *nama_data++);
	}
	
	/* untuk mengakhiri data ftp */
	send_etx();
}

int send_etx(void)
{
	tulis_char((char) CTRL_ETX );
	
	baca_serial(buf, 20, 5);
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

/* seharusnya saat [ETX] / ctrl-c dikirim, koneksi sudah langsung putus */
int close_ftp_sess(void)
{
	//char buf[64];
	sprintf(buf, "AT+WIPCLOSE=4,1\r\n");
	tulis_serial(buf, strlen(buf), 0);	
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

int stop_tcpip_stack(void)
{
	//char buf[64];
	sprintf(buf, "AT+WIPCFG=0\r\n");
	tulis_serial(buf, strlen(buf), 0);	
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

#endif /* PAKAI_GSM_FTP */
