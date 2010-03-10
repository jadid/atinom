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

#include "ff.h"
#include "../monita/monita_uip.h"

//#define debug		printf
#define debug_printf(...)	do{} while(0)

char * buat_direktori(time_t timeval);
	
static int jum_dipanggil=0;
static int jum_detik=0;
static int sudah_buka = 0;

static FIL fd;
extern float data_f[];

int proses_simpan_file(void)
{
	time_t timeval;
	struct tm tw;
	char path[64];
	char temp[64];
	int ret;
	int jm;
	int i;
	struct t_simpan_file *ts;
	struct t_dt_set *dt;
		
	jum_dipanggil++;
	ts = (char *) ALMT_SFILE;
	dt = (char *) ALMT_DT_SET;
	
	if (ts->set == 1)
	{
		/* jika sepuluh menit lewat */
		if (jum_dipanggil > (ts->periode * 60))
		{						
			if (sudah_buka == 1)
			{		
				f_close( &fd );
				sudah_buka = 0;
			}
						
			jum_dipanggil=0;
		}
		else /* per detik */
		{
			if (sudah_buka == 1)
			{
				jum_detik++;
				if (jum_detik >= ts->detik)
				{
					get_tm_time( &tw );
					timeval = mktime( &tw );
				
					/* kasih tag waktu */
					sprintf(path, "%s", ctime(&timeval));
					
					/* buang new linenya */
					path[24] = ' ';
					
					if (ret = f_write( &fd, path, strlen(path), &jm))
					{
								printf("%s(): Tulis error %d !\r\n", __FUNCTION__, ret);
								return ;
					}
					
					/* tulis data */
					for (i=0; i< (JML_SUMBER * PER_SUMBER); i++)
					{
						jm = ts->no_data[i];
			
						if ( jm != 0) 
						{
							//sprintf(path, "(%d)%s(%s) ", jm, dt[jm - 1].nama, dt[jm - 1].satuan);
							
							sprintf(path, "%.3f ", data_f[ jm - 1] );
							
							if (ret = f_write( &fd, path, strlen(path), &jm))
							{
								printf("%s(): Tulis error %d !\r\n", __FUNCTION__, ret);
								return ;
							}							
						}
					}
					
					/* kasih new line */
					sprintf(path, "\r\n");
					if (ret = f_write( &fd, path, strlen(path), &jm))
					{
								printf("%s(): Tulis error %d !\r\n", __FUNCTION__, ret);
								return ;
					}
					
					/* di sync dulu */
					f_sync( &fd );
					
					jum_detik = 0;
				}
			}			
			else /* file belum dibuat */
			{
				get_tm_time( &tw );
				timeval = mktime( &tw );
				
				//debug
				//printf("TIMEVAL = %d\r\n", timeval);
				//printf("cek %s\r\n",  ctime(&timeval));
		
				/* close file */
				//printf("test buat %s\r\n", buat_direktori( timeval ));
							
				/* buat file baru */
				sprintf(temp, "angin_%s", ctime(&timeval));
				
				/* cari spasi dan jadikan underscore */
				jm = strlen(temp);
				for (i=0; i<jm; i++)
				{
					if ( temp[i] == ' ' ) temp[i] = '_';
					if ( temp[i] == ':' ) temp[i] = '_';
				}
				
				/* buat direktori */
				sprintf(path, "%s\\%s", buat_direktori(timeval), temp);
				//printf("OK SAAT FILE %s\r\n", path);
				if (ret = f_open( &fd, path, FA_CREATE_ALWAYS | FA_WRITE ))
				{
					printf("%s(): Buat file %s error %d !\r\n", __FUNCTION__, path, ret);
				}
				else
				{
					sudah_buka = 1;
					jum_detik = 0;
					
					/* kasih tag waktu */
					sprintf(path, "Hari Bln Tgl Jam Tahun ");
					if (ret = f_write( &fd, path, strlen(path), &jm))
					{
						printf("%s(): Tulis error %d !\r\n", __FUNCTION__, ret);
					}
					
					/* print out nama dan satuan dan nomer data dulu */
					for (i=0; i< (JML_SUMBER * PER_SUMBER); i++)
					{
						jm = ts->no_data[i];
			
						if ( jm != 0) 
						{
							/* nomer, nama, satuan, spasi ! */
							sprintf(path, "(%d)%s(%s) ", jm, dt[jm - 1].nama, dt[jm - 1].satuan);
							if (ret = f_write( &fd, path, strlen(path), &jm))
							{
								printf("%s(): Tulis error %d !\r\n", __FUNCTION__, ret);
								return ;
							}							
						}
					}
					/* kasih new line */
					sprintf(path, "\r\n");
					if (ret = f_write( &fd, path, strlen(path), &jm))
					{
								printf("%s(): Tulis error %d !\r\n", __FUNCTION__, ret);
								return ;
					}
					/* di sync dulu */
					f_sync( &fd );
				}
			}
		}
	} /* jika set simpan = 1 */
}

char * buat_direktori(time_t timeval)
{
	int res;
	char tek[64];
	char thn[32];
	char bln[64];
	char tgl[128];
	char temp[32];
	char jam[128];
				
	if (res = f_mkdir("\\data") != 0)
	{
		debug_printf("ERR: %s() : log : %d\r\n", __FUNCTION__, res);
	}	
	
	#if 1		
	// tahun
	sprintf(tek,"%s", ctime(&timeval));
	snprintf(thn, 17, "\\data\\tahun_%s", &tek[20]);	// 11
	if (res = f_mkdir(thn) != 0)
		debug_printf("ERR: %s() : tahun : %d\r\n", __FUNCTION__, res);
	
			
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
	
	
	snprintf(temp, 3, "%s", &tek[11]);	// 11
	sprintf(jam,"%s\\jam_%s", tgl, temp);
	if (res = f_mkdir(jam) != 0)
		debug_printf("ERR: %s() : jam : %d\r\n", __FUNCTION__, res);
	
	
	/*
	printw("err = %d", res);		
	if (res != 0)
	{
		perror("MKDIR creation failed");
	}*/
	#endif
	
	return jam;
}
