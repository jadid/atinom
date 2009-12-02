/*
	21 Juli 2009
	furkan jadid
	daun biru engineering
	
	banyak dicopy dari code bundle LPC uvision
	*/

#include "FreeRTOS.h"
#include "../fatfs/integer.h" /* ambil def DWORD saja */
#include "rtc.h"

RTCTime local_time, current_time;

void rtc_init(void)
{
	PCONP |= BIT(9);
	
	/*--- Initialize registers ---*/    
  	RTC_AMR = 0;
  	RTC_CIIR = 0;
  	
	RTC_CCR = BIT(4);		// clock dari kristal 32768
	
  	//RTC_PREINT = PREINT_RTC;
  	//RTC_PREFRAC = PREFRAC_RTC;

}

void rtc_reset(void)
{
	RTC_CCR = BIT(1);

}

void rtc_start( void ) 
{
  /*--- Start RTC counters ---*/
  RTC_CCR |= CCR_CLKEN;
  
  //RTC_ILR = ILR_RTCCIF;
  return;
}

void rtc_reset_waktu(void)
{
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


unsigned int get_fattime ()
{


}
