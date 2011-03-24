/*
	Proses simpan file
	
	5 Maret 2010
	furkan jadid, daun biru engineering
	
	ini akan dipanggil setiap detik, lewat task shell
	
	supaya akses2 MMC lewat task shell lain tidak 
	bertubrukan !
	
	File akan dibuat dan close setiap 10 menit
	sehingga dalam folder jam akan terdapat 6 file
	
	*/
#include <time.h>	
#include <stdio.h>
#include <errno.h>
#include <float.h>
#include <math.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"


#include "../monita/monita_uip.h"
#ifdef PAKAI_FILE_SIMPAN

#ifdef PAKAI_MMC
//extern int status_MMC;
#endif

#include "ff.h"

//#define debug_printf		printf
#define debug_printf(...)	do{} while(0)

static char * buat_direktori(char *base, time_t timeval, int menit);
	
static int jum_dipanggil=0;
static int jum_detik=0;
static int sudah_buka = 0;

static FIL fd;
extern float data_f[];
static FIL fd_foto;

/*
time_t timeval;
struct tm timeinfo;
get_tm_time( &timeinfo );
return timeinfo.tm_hour;	
//*/


int simpan_error_terus() {
	
}
int error_simpan=0;

int handle_tulis_file(int errornya) {
	time_t timeval;
	struct tm tw;
	int ret;
	char path[64];
	
	get_tm_time( &tw );
	timeval = mktime( &tw );
	//*
	sprintf(path, "%04d%02d%02d_%02d%02d%02d\t", tw.tm_year+1900, tw.tm_mon+1, tw.tm_mday, tw.tm_hour, tw.tm_min, tw.tm_sec);
	buat_direktori("data", timeval, 0);
	if (ret = f_open( &fd, path, FA_CREATE_ALWAYS | FA_WRITE ))		{
		printf("%s(): Buat file %s error %d !\r\n", __FUNCTION__, path, ret);				
		error_simpan++;
		if (error_simpan>10) {
			reset_cpu();
			error_simpan=0;
		}
		//reset_cpu();		// 
		return;
	}
	f_sync( &fd );
	jum_detik = 0;
	//*/
}

int proses_simpan_file(void)	{
	time_t timeval;
	struct tm tw;
	char path[64];
	char temp[64];
	int ret;
	int jm;
	int i;
	struct t_simpan_file *ts;
	struct t_dt_set *dt;
	char waktus[15];
		
	jum_dipanggil++;
	ts = (char *) ALMT_SFILE;
	dt = (char *) ALMT_DT_SET;
	
	//printf("\tts->set: %d, sudah_buka: %d, jml_dipanggil: %d\r\n", ts->set, sudah_buka);
	
	if (ts->set==1)		
	{
		
		/* jika sepuluh menit lewat */
		if (jum_dipanggil > (ts->periode * 60))	{						
			if (sudah_buka == 1)		{		
				f_close( &fd );
				sudah_buka = 0;
			}
						
			jum_dipanggil=0;
		}
		else /* per detik */
		{
			if (sudah_buka == 1)	{
				jum_detik++;
				if (jum_detik >= ts->detik)		{
					get_tm_time( &tw );
					timeval = mktime( &tw );
					
					//sprintf(path, "%04d%02d%02d_%02d%02d%02d ", tw.tm_year+1900, tw.tm_mon+1, tw.tm_mday, tw.tm_hour, tw.tm_min, tw.tm_sec);
					sprintf(path, "%04d%02d%02d_%02d%02d%02d\t", tw.tm_year+1900, tw.tm_mon+1, tw.tm_mday, tw.tm_hour, tw.tm_min, tw.tm_sec);
					/* kasih tag waktu */
					//sprintf(path, "%s", ctime(&timeval));
					
					/* buang new linenya */
					//path[24] = ' ';
					
					/*
					if (ret = f_write( &fd, path, strlen(path), &jm))	{
						//printf("%s(): Tulis error %d !\r\n", __FUNCTION__, ret);
						handle_tulis_file(ret);
						return ;
					}
					//*/
					/* tulis data */
					for (i=0; i< (JML_SUMBER * PER_SUMBER); i++)	{
						jm = ts->no_data[i];
			
						if ( jm != 0) {
							//sprintf(path, "(%d)%s(%s) ", jm, dt[jm - 1].nama, dt[jm - 1].satuan);
							
							//sprintf(path, "%.3f ", data_f[ jm - 1] );
							sprintf(path, "%.3f\t", data_f[jm-1]);
							printf("%.2f ", data_f[jm-1]);
							/*
							if (ret = f_write( &fd, path, strlen(path), &jm))	{
								//printf("%s(): Tulis error %d !\r\n", __FUNCTION__, ret);
								handle_tulis_file(ret);
								return ;
							}
							//*/							
						}
					}
					
					/* kasih new line */
					sprintf(path, "\r\n");
					printf("\r\n");
					/*
					if (ret = f_write( &fd, path, strlen(path), &jm))	{
						//printf("%s(): Tulis error %d !\r\n", __FUNCTION__, ret);
						handle_tulis_file(ret);
						return ;
					}
					
					// di sync dulu //
					f_sync( &fd );
					//*/
					jum_detik = 0;
				}
			}			
			else /* file belum dibuat */
			{
				//int tahunx=0,bulanx=0,tglx=0,jamx=0,menitx=0;
				get_tm_time( &tw );
				timeval = mktime( &tw );
				
				//debug
				//printf("TIMEVAL = %d\r\n", timeval);
				//printf("cek %s, %04d%02d%02d_%02d%02d\r\n",  ctime(&timeval), tw.tm_year+1900, tw.tm_mon+1, tw.tm_mday, tw.tm_hour, tw.tm_min);
				sprintf(waktus, "%04d%02d%02d_%02d%02d", tw.tm_year+1900, tw.tm_mon+1, tw.tm_mday, tw.tm_hour, tw.tm_min);
				//printf("waktus: %s\r\n", waktus);
				/* close file */
				//printf("test buat %s\r\n", buat_direktori( timeval ));
							
				/* buat file baru */
				//sprintf(temp, "angin_%s", ctime(&timeval));
				//sprintf(temp, "%s_%s", ts->nama_file, ctime(&timeval));
				sprintf(temp, "%s_%s.txt", ts->nama_file, waktus);
				
				/* cari spasi dan jadikan underscore */
				/*
				jm = strlen(temp);
				for (i=0; i<jm; i++)	{
					if ( temp[i] == ' ' ) temp[i] = '_';
					if ( temp[i] == ':' ) temp[i] = '_';
				}
				//*/
				/* buat direktori */
				sprintf(path, "%s\\%s", buat_direktori("data", timeval, 0), temp);
				//printf("OK SAAT FILE %s\r\n", path);
				/*
				if (ret = f_open( &fd, path, FA_CREATE_ALWAYS | FA_WRITE ))		{
					printf("%s(): Buat file %s error %d !\r\n", __FUNCTION__, path, ret);				
					reset_cpu();		// 
					return;
				}
				else	
				//*/
				{
					sudah_buka = 1;
					jum_detik = 0;
					
					/* kasih tag waktu */
					//sprintf(path, "Hari Bln Tgl Jam Tahun ");
					sprintf(path, "Waktu ");
					/*
					if (ret = f_write( &fd, path, strlen(path), &jm))	{
						printf("%s(): Tulis error %d !\r\n", __FUNCTION__, ret);
						reset_cpu();
						return;
					}
					//*/
					/* print out nama dan satuan dan nomer data dulu */
					for (i=0; i< (JML_SUMBER * PER_SUMBER); i++)	{
						jm = ts->no_data[i];
			
						if ( jm != 0) 	{
							/* nomer, nama, satuan, spasi ! */
							sprintf(path, "(%d)%s(%s) ", jm, dt[jm - 1].nama, dt[jm - 1].satuan);
							printf("%s", path);
							/*
							if (ret = f_write( &fd, path, strlen(path), &jm))		{
								//printf("%s(): Tulis error %d !\r\n", __FUNCTION__, ret);
								handle_tulis_file(ret);
								return ;
							}
							//*/							
						}
					}
					/* kasih new line */
					sprintf(path, "\r\n");
					printf("\r\n");
					/*
					if (ret = f_write( &fd, path, strlen(path), &jm))	{
						//printf("%s(): Tulis error %d !\r\n", __FUNCTION__, ret);
						handle_tulis_file(ret);
						return ;
					}
					// di sync dulu //
					f_sync( &fd );
					//*/
				}
			}
		}
	} /* jika set simpan = 1 */
}

static char * buat_direktori(char *base, time_t timeval, int menit)	{
	int res;
	char tek[64];
	char thn[64];
	char bln[64];
	char tgl[64];
	char temp[32];
	char jam[64];
	char mnt[64];
	
	sprintf(jam, "\\%s", base);
				
	//if (res = f_mkdir("\\data") != 0)
	if (res = f_mkdir(jam) != 0)		{
		debug_printf("ERR: %s() : %s : %d\r\n", __FUNCTION__, jam, res);
	}	
		
	// tahun
	sprintf(tek,"%s", ctime(&timeval));
	res = strlen(tek);
	tek[res - 1] = 0;
	
	//snprintf(thn, 17, "\\%s\\tahun_%s", base, &tek[20]);	// 11
	sprintf(thn, "\\%s\\tahun_%s", base, &tek[20]);
	if (res = f_mkdir(thn) != 0)
		debug_printf("ERR: %s() : %s : %d\r\n", __FUNCTION__, thn, res);
	
			
	// bulan
	snprintf(temp, 4, "%s", &tek[4]);	// 11
	sprintf(bln,"%s\\%s", thn, temp);
	if (res = f_mkdir(bln) != 0)
		debug_printf("ERR: %s() : bulan : %d\r\n", __FUNCTION__, res);
		
	// tgl
	snprintf(temp, 3, "%s", &tek[8]);	// 11
	/* cek jika tgl adalah 1 angka, supaya tidak ada spasi */
	if (temp[0] == ' ')
	{
		temp[0] = temp[1];
		temp[1] = 0;
	}	
	sprintf(tgl,"%s\\tgl_%s", bln, temp);
	if (res = f_mkdir(tgl) != 0)
		debug_printf("ERR: %s() : tgl : %d\r\n", __FUNCTION__, res);
	
	/* jam */
	snprintf(temp, 3, "%s", &tek[11]);	// 11
	sprintf(jam,"%s\\jam_%s", tgl, temp);
	if (res = f_mkdir(jam) != 0)
		debug_printf("ERR: %s() : jam : %d\r\n", __FUNCTION__, res);
	
	/* menit juga dibuat direktorinya */
	if (menit == 1)		{
		sprintf(temp,"");
		snprintf(temp, 3, "%s", &tek[14]);	// 14
		sprintf(mnt,"%s\\mnt_%s", jam, temp);
		if (res = f_mkdir(mnt) != 0)
		debug_printf("ERR: %s() : %s : %d\r\n", __FUNCTION__, mnt, res);
		
		return mnt;
	}
	else	
		return jam;
}

#ifdef BUAT_FOTO
void webclient_connected(void)	{
	char path[128];
	char temp[64];
	time_t timeval;
	struct tm tw;
	int ret;
	int jm;
	int i;
	
	/* siapin direktori dan buka file untuk buat */
	get_tm_time( &tw );
	timeval = mktime( &tw );
	
	sprintf(temp, "%s_%s", "foto", ctime(&timeval));
				
	/* cari spasi dan jadikan underscore */
	jm = strlen(temp);
	for (i=0; i<jm; i++)
	{
		if ( temp[i] == ' ' ) temp[i] = '_';
		if ( temp[i] == ':' ) temp[i] = '_';
	}
	
	jm = strlen(temp);	
	sprintf( &temp[jm - 1], ".jpg");
	//printf(temp);	
	
	/* buat direktori */
	sprintf(path, "%s\\%s", buat_direktori("gambar", timeval, 1), temp);
	//printf(path);
	
	if (ret = f_open( &fd_foto, path, FA_CREATE_ALWAYS | FA_WRITE ))
	{
		printf("%s(): Buat file %s error %d !\r\n", __FUNCTION__, path, ret);
	}
}


void tulis_foto(char *data, unsigned int len)
{
	int ret;
	int jm;
	
	if (len != 0)
	{
		if (ret = f_write( &fd_foto, data, len, &jm))
		{
				printf("%s(): Tulis error %d !\r\n", __FUNCTION__, ret);
				return ;
		}
	}
	else
	{
		if ( fd_foto.fs != NULL)
			f_close( &fd_foto );
	}
}
#endif

#endif
