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

//#define debug		printf
#define debug_printf(...)	do{} while(0)

char * buat_direktori(time_t timeval);
	
static int jum_dipanggil=0;
static int sudah_buka = 0;

FIL fd;

int proses_simpan_file(void)
{
	time_t timeval;
	struct tm tw;
	char path[64];
	char temp[64];
	int ret;
	int jm;
	int i;
		
	jum_dipanggil++;
	
	/* jika sepuluh menit lewat */
	if (jum_dipanggil > 20) 
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
		
		sprintf(path, "%s\\%s", buat_direktori(timeval), temp);
		printf("%s\r\n", path);
		//sprintf(path, "%s\\%s", buat_direktori( timeval )
		
		if (sudah_buka == 1)
		{
			/*
			if (ret = f_close( &fd ))
			{
				printf("%s(): Close file %s error %d !\r\n", __FUNCTION__, path, ret);
			}
			else
			*/
			
			f_close( &fd );
			sudah_buka = 0;
		}
		
		if (sudah_buka == 0)
		{
			if (ret = f_open( &fd, path, FA_CREATE_ALWAYS | FA_WRITE ))
			{
				printf("%s(): Buat file %s error %d !\r\n", __FUNCTION__, path, ret);
			}
			else
			{
				sudah_buka = 1;
				sprintf(path, "Hari Bln Tgl Jam Tahun Wind_speed Wind_direct Turbin_rpm Turbin_current Turbin_voltage ");
				if (ret = f_write( &fd, path, strlen(path), &jm))
				{
					printf("%s(): Tulis error %d !\r\n", __FUNCTION__, ret);
				}
			}
		}
		
		jum_dipanggil=0;
	}
	else
	{
		if (sudah_buka == 1)
		{
			sprintf(path, "ini nomer %d\r\n", jum_dipanggil);
			if (ret = f_write( &fd, path, strlen(path), &jm))
			{
				printf("%s(): Tulis error %d !\r\n", __FUNCTION__, ret);
			}
		}
	}


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
				
	if (res = f_mkdir(".data") != 0)
	{
		debug_printf("ERR: %s() : log : %d\r\n", __FUNCTION__, res);
	}	
	
	#if 1		
	// tahun
	sprintf(tek,"%s", ctime(&timeval));
	snprintf(thn, 17, ".data\\tahun_%s", &tek[20]);	// 11
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
