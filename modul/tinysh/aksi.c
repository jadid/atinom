
#include "FreeRTOS.h"
#include "tinysh/enviro.h"
#include "monita/monita_uip.h"
//#include "rtc/rtc.h"
#include <time.h>

#ifndef __SHELL_AKSI__
#define __SHELL_AKSI__


extern char resetTFX;
extern struct tm awalWaktuTFX;
extern struct tm akhirWaktuTFX;

char sh_rpm_nol(char no)		{
	flagWtfx = 1;
}

static tinysh_cmd_t ambil_waktu_cmd={0,"ambil_waktu","","",  sh_rpm_nol,0,0,0};

inline unsigned int selisih_TFX()		{
	unsigned int selisih = difftime(mktime(&akhirWaktuTFX), mktime(&awalWaktuTFX));
	printf("selisih waktu: %d detik\r\n", selisih);
	return selisih;
}

void cek_reset_tfx()	{
	struct t_env *envw;
	envw = (char *) ALMT_ENV;
	if (selisih_TFX()==(envw->jedaResetTFX/2))	{
		reset_tfx();					// jika sudah mereset flagWtfx = 0
		
	}
}

inline void reset_tfx()	{
	resetTFX = 1;
	flagWtfx = 0;
}

char waktu_skrg(char awal)		{
	rtcCTIME0_t ctime0;
	rtcCTIME1_t ctime1;
	rtcCTIME2_t ctime2;
	ctime0.i = RTC_CTIME0; 
	ctime1.i = RTC_CTIME1; 
	ctime2.i = RTC_CTIME2;

	if (awal)	{
		awalWaktuTFX.tm_sec		= ctime0.seconds; 
		awalWaktuTFX.tm_min		= ctime0.minutes;
		awalWaktuTFX.tm_hour	= ctime0.hours;
		awalWaktuTFX.tm_mday	= ctime1.dom;
		awalWaktuTFX.tm_mon		= ctime1.month;
		awalWaktuTFX.tm_year	= ctime1.year - 1900;
	}
	
	//printf(	"  2. Waktu : %d-%d-%04d %d:%02d:%02d  ++++  ", 	\
	//	awalWaktuTFX.tm_mday, awalWaktuTFX.tm_mon, awalWaktuTFX.tm_year, \
	//	awalWaktuTFX.tm_hour, awalWaktuTFX.tm_min, awalWaktuTFX.tm_sec);

	akhirWaktuTFX.tm_sec	= ctime0.seconds; 
	akhirWaktuTFX.tm_min	= ctime0.minutes;
	akhirWaktuTFX.tm_hour	= ctime0.hours;
	akhirWaktuTFX.tm_mday	= ctime1.dom;
	akhirWaktuTFX.tm_mon	= ctime1.month;
	akhirWaktuTFX.tm_year	= ctime1.year - 1900;
	
	//printf(	"  %d-%d-%04d %d:%02d:%02d\r\n", 			\
	//	akhirWaktuTFX.tm_mday, akhirWaktuTFX.tm_mon, akhirWaktuTFX.tm_year, \
	//	akhirWaktuTFX.tm_hour, akhirWaktuTFX.tm_min, akhirWaktuTFX.tm_sec);
	//vTaskDelay(10);
	flagWtfx = 2;
	
	return 0;
}



#endif

