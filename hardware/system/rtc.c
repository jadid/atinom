/*
	21 Juli 2009
	furkan jadid
	daun biru engineering
	
	banyak dicopy dari code bundle LPC uvision
	*/

#ifdef PAKAI_RTC

#include "FreeRTOS.h"
#include "../fatfs/integer.h" /* ambil def DWORD saja */
#include <time.h>
#include "rtc.h"


//RTCTime local_time, current_time;

void rtc_init(void)
{
	PCONP |= BIT(9);
	
	/*--- Initialize registers ---*/    
  	RTC_AMR = 0;
  	RTC_CIIR = 0;
  	
	RTC_CCR = BIT(4);		// clock dari kristal 32768
	
  	//RTC_PREINT = PREINT_RTC;
  	//RTC_PREFRAC = PREFRAC_RTC;
	
	#ifdef KONTROL_RTC
		//RTC_CCR |= 
	#endif
}

void rtc_init_irq()	{
	VIC_VectAddr6 = (portLONG) rtcISR;
	VIC_VectCntl6 = VIC_VectCntl_ENABLE | VIC_Channel_RTC;
}

unsigned char rtc_counter_irq_aktif(unsigned char x)	{
	if (x & 0x01)	RTC_CIIR |= RTC_CIIR_IMSEC;
	if (x & 0x02)	RTC_CIIR |= RTC_CIIR_IMMIN;
	if (x & 0x04)	RTC_CIIR |= RTC_CIIR_IMHOUR;
	if (x & 0x08)	RTC_CIIR |= RTC_CIIR_IMDOM;
	if (x & 0x10)	RTC_CIIR |= RTC_CIIR_IMDOW;
	if (x & 0x20)	RTC_CIIR |= RTC_CIIR_IMDOY;
	if (x & 0x40)	RTC_CIIR |= RTC_CIIR_IMMON;
	if (x & 0x80)	RTC_CIIR |= RTC_CIIR_IMYEAR;
	return (RTC_CIIR);
}

void rtc_reset(void)	{
	
	RTC_CCR = BIT(1);
	
}

void rtc_start( void ) 
{
  /*--- Start RTC counters ---*/
  RTC_CCR |= CCR_CLKEN;
  
  //RTC_ILR = ILR_RTCCIF;
  return;
}

void rtc_reset_waktu(void)	{
	RTC_SEC = 0;
	RTC_MIN = 0;
	RTC_HOUR = 0;
	RTC_DOM = 1;
	RTC_DOW = 0;
	RTC_DOY = 0;
}

void rtc_set_time( RTCTime Time ) 
{
  RTC_SEC = Time.RTC_Sec;
  RTC_MIN = Time.RTC_Min;
  RTC_HOUR = Time.RTC_Hour;
  RTC_DOM = Time.RTC_Mday;
  RTC_DOW = Time.RTC_Wday;
  RTC_DOY = Time.RTC_Yday;
  RTC_MONTH = Time.RTC_Mon;
  RTC_YEAR = Time.RTC_Year;    
  return;
}

void rtc_set_time_tm( struct rtc_time tc ) 
{
  RTC_SEC = tc.tm_sec;
  RTC_MIN = tc.tm_min;
  RTC_HOUR = tc.tm_hour;
  RTC_DOM = tc.tm_mday;
  RTC_DOW = tc.tm_wday;
  RTC_DOY = tc.tm_yday;
  RTC_MONTH = tc.tm_mon + 1;
  RTC_YEAR = tc.tm_year;    
  return;
}

RTCTime rtc_get_time( void ) 
{
  RTCTime LocalTime;
    
  LocalTime.RTC_Sec = RTC_SEC;
  LocalTime.RTC_Min = RTC_MIN;
  LocalTime.RTC_Hour = RTC_HOUR;
  LocalTime.RTC_Mday = RTC_DOM;
  LocalTime.RTC_Wday = RTC_DOW;
  LocalTime.RTC_Yday = RTC_DOY;
  LocalTime.RTC_Mon = RTC_MONTH;
  LocalTime.RTC_Year = RTC_YEAR;
  return ( LocalTime );    
}

void p_rtc_get_time( RTCTime *LocalTime ) 
{
    
  LocalTime->RTC_Sec = RTC_SEC;
  LocalTime->RTC_Min = RTC_MIN;
  LocalTime->RTC_Hour = RTC_HOUR;
  LocalTime->RTC_Mday = RTC_DOM;
  LocalTime->RTC_Wday = RTC_DOW;
  LocalTime->RTC_Yday = RTC_DOY;
  LocalTime->RTC_Mon = RTC_MONTH;
  LocalTime->RTC_Year = RTC_YEAR;
  
  return;    
}

void get_rtc_time(time_t *tt )
{
	time_t tmp;
	/*	
	tmp = 	RTC_SEC + (RTC_MIN * 60) + 
			(RTC_HOUR * 60 * 60) +
			(RTC_
			*/
}

void get_tm_time(struct tm *theTime)
{
	theTime->tm_sec   = RTC_SEC;
  theTime->tm_min   = RTC_MIN;
  theTime->tm_hour  = RTC_HOUR;
  theTime->tm_mday  = RTC_DOM;
  theTime->tm_mon   = RTC_MONTH - 1;
  theTime->tm_year  = RTC_YEAR - 1900;
  theTime->tm_wday  = RTC_DOW;
  theTime->tm_yday  = RTC_DOY - 1;
  theTime->tm_isdst = 0;
}

unsigned int get_fattime ()
{
	struct tm tm;
	unsigned int tmr;
	
	get_tm_time( &tm );
	
	if (tm.tm_year < 80) tm.tm_year = 80;

  	tmr = 0
    | ((tm.tm_year - 80) << 25)
    | ((tm.tm_mon + 1)   << 21)
    | (tm.tm_mday        << 16)
    | (tm.tm_hour        << 11)
    | (tm.tm_min         << 5)
    | (tm.tm_sec         >> 1);

	//debug_printf("%s(): %u, year = %d\r\n", __FUNCTION__, tmr, tm.tm_year);
	
  	return tmr;

}

void rtcWrite (struct tm *newTime)
{
  portENTER_CRITICAL ();

  RTC_SEC   = newTime->tm_sec;
  RTC_MIN   = newTime->tm_min;
  RTC_HOUR  = newTime->tm_hour;
  RTC_DOM   = newTime->tm_mday;
  RTC_MONTH = newTime->tm_mon + 1;
  RTC_YEAR  = newTime->tm_year + 1900;
  RTC_DOW   = newTime->tm_wday;
  RTC_DOY   = newTime->tm_yday + 1;

  portEXIT_CRITICAL ();
}

#endif
