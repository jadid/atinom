/*
	10 Mar 2010
	furkan jadid, daun biru engineering
	
	cari direktori sejam lalu 
	
	dicopy dari simpan_file.c
*/
#include <time.h>	
#include <stdio.h>
#include <errno.h>
#include <float.h>
#include <math.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "../fatfs/ff.h"
#include "../monita/monita_uip.h"

//#define debug		printf
#define debug_printf(...)	do{} while(0)

/* return path sejam lalu 
 * 
 * INGAT untuk mengubah juga file simpan_file.c
 * jika nama2 direktori diganti
 * 
 * */

char * cari_sejam_lalu(time_t timeval)
{
	int res;
	char tek[64];
	char thn[32];
	char bln[64];
	char tgl[128];
	char temp[32];
	char jam[128];
	int djam;
	
	#if 1		
	// tahun
	sprintf(tek,"%s", ctime(&timeval));
	snprintf(thn, 17, "\\data\\tahun_%s", &tek[20]);	// 11
		
	// bulan
	snprintf(temp, 4, "%s", &tek[4]);	// 11
	sprintf(bln,"%s\\%s", thn, temp);
		
	// tgl
	snprintf(temp, 3, "%s", &tek[8]);	// 11
	/* cek jika tgl adalah 1 angka, supaya tidak ada spasi */
	if (temp[0] == ' ')
	{
		temp[0] = temp[1];
		temp[1] = 0;
	}	
	sprintf(tgl,"%s\\tgl_%s", bln, temp);	
	
	// jam
	snprintf(temp, 3, "%s", &tek[11]);	// 11
	sscanf(temp, "%d", &djam);
	
	if (djam == 0) 
	{
		/* cari hari kemarin */
		
	}
	djam = djam - 1;
	
	//sprintf(jam,"%s\\jam_%s", tgl, temp);
	sprintf(jam,"%s\\jam_%02d", tgl, djam);

	#endif
	
	return jam;
}
