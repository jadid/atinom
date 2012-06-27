
#include "FreeRTOS.h"

#ifdef PAKAI_RTC

#ifndef __MODUL_RTC__
#define __MODUL_RTC__

#include "lpc23xx.h"
#include "monita/monita_uip.h"
#include "rtc.h"
//#include "rtc_ISR.c"
#include <time.h>

//unsigned int uptime_ovflow=0;
//unsigned int tik_lama=0;

void rtcWrite(struct tm *newTime)	{
	portENTER_CRITICAL ();

	RTC_CCR &= ~RTC_CCR_CLKEN;
	RTC_CCR |=  RTC_CCR_CTCRST;

	RTC_SEC   = newTime->tm_sec;
	RTC_MIN   = newTime->tm_min;
	RTC_HOUR  = newTime->tm_hour;
	RTC_DOM   = newTime->tm_mday;
	RTC_MONTH = newTime->tm_mon + 1;
	RTC_YEAR  = newTime->tm_year + 1900;
	RTC_DOW   = newTime->tm_wday;
	RTC_DOY   = newTime->tm_yday + 1;

	RTC_CCR &= ~RTC_CCR_CTCRST;
	RTC_CCR |=  RTC_CCR_CLKEN;

	portEXIT_CRITICAL ();
}





//static


void start_rtc( void )	{
	RTC_CCR |= RTC_CCR_CLKEN;					// CCR_CLKEN;
	//RTC_ILR = RTC_ILR_RTCCIF | RTC_ILR_RTCALF;	//
	#ifdef PAKAI_KONTROL_RTC
	RTC_ILR  = BIT(1) | BIT(0);
	#endif
}

void rtc_reset(void)	{
	RTC_CCR = BIT(1);
	vTaskDelay(10);
	RTC_CCR &= ~BIT(1);
}

static inline void rtcSleep (void)	{
	RTC_CCR = (RTC_CCR_CLKEN | RTC_CCR_CLKSRC);
	PCONP &= ~BIT(9);
}

static inline void rtcWake (void)	{
	RTC_CCR = (RTC_CCR_CLKEN | RTC_CCR_CLKSRC);
	PCONP |= BIT(9);
}

void init_rtc()	{
	//printf("----%s()\r\n", __FUNCTION__);
	PCONP |= BIT(9);
	
	//--- Initialize registers ---//
	RTC_CCR  = 0;
  	RTC_AMR  = 0;
  	RTC_CIIR = 0;
  	
	RTC_CCR = BIT(4);		// clock dari kristal 32768
	//flagRTCc = 120;
	
  	//RTC_PREINT = PREINT_RTC;
  	//RTC_PREFRAC = PREFRAC_RTC;
	
	
	
	#ifdef PAKAI_KONTROL_RTC
		//flagRTCc = 130;
		//printf("________init masuk IRQ RTC_________\r\n");
		init_rtc_irq();
		RTC_ILR = RTC_ILR_RTCCIF;
		//flagRTCc = 140;
	#endif
	
	//printf("___________init selesai RTC_________\r\n");
	printf("   init RTC() ....\r\n");
}

#ifdef PAKAI_KONTROL_RTC

#endif

int rtcPeriodicAlarm (int mode)	{
	int state;
    
	rtcWake ();

	state = RTC_CIIR & RTC_CIIR_IMMIN;

	if (!mode)
		RTC_CIIR &= ~RTC_CIIR_IMMIN;
	else if (mode > 0)
		RTC_CIIR |= RTC_CIIR_IMMIN;

	rtcSleep ();

	//return state ? 1 : 0;
	return state;
}

#if PAKAI_KONTROL_RT
struct tm *rtcGetAlarmTm (struct tm *tm)	{
	if (tm)	{
		memset (tm, 0, sizeof (* tm));

		rtcWake ();

		if (RTC_AMR != RTC_AMR_AMRMASK)		{
			tm->tm_year = RTC_ALYEAR - 1900;
			tm->tm_mon = RTC_ALMON - 1;
			tm->tm_mday = RTC_ALDOM;
			tm->tm_hour = RTC_ALHOUR;
			tm->tm_min = RTC_ALMIN;
			tm->tm_sec = RTC_ALSEC;
		} else {
			time_t now = 0;
			memcpy (tm, localtime (&now), sizeof (* tm));
		}

		rtcSleep ();
	}
	return tm;
}
#endif


#if PAKAI_KONTROL_RT
int rtcSetAlarm (struct tm *tm)	{
	if (tm && (mktime (tm) < time (NULL)))
		return -1;

	rtcWake ();

	RTC_AMR = RTC_AMR_AMRMASK;

	if (tm)	  {
		RTC_ALYEAR = tm->tm_year + 1900;
		RTC_ALMON = tm->tm_mon + 1;
		RTC_ALDOM = tm->tm_mday;
		RTC_ALHOUR = tm->tm_hour;
		RTC_ALMIN = tm->tm_min;
		RTC_ALSEC = tm->tm_sec;
		RTC_ALDOW = 0;
		RTC_ALDOY = 0;

		RTC_AMR = RTC_AMR_AMRDOW | RTC_AMR_AMRDOY;
	}

	rtcSleep ();
	return 0;
}
#endif

#ifdef PAKAI_KONTROL_RTC

unsigned char rtc_alarm_aktif(unsigned char x)	{
	RTC_AMR = x;
	printf("  %s() masuk 0x%04x\r\n", __FUNCTION__, RTC_AMR);
	return RTC_AMR;
}

unsigned char rtc_counter_irq_aktif(unsigned char x)	{
	//rtc_alarm_aktif(0xFF);
	
	if (RTC_ILR & RTC_ILR_RTCCIF)	{		// counter irq
		RTC_ILR = RTC_ILR_RTCCIF;
		//printf("CIF diclear !\r\n");
	}
	if (RTC_ILR & RTC_ILR_RTCALF)	{
		RTC_ILR = RTC_ILR_RTCALF;
		//printf("ALF diclear !\r\n");
	}
	
	//RTC_ILR = BIT(0);
	RTC_CIIR = x;

	printf("  %s() masuk CIIR: 0x%04x, ILR: 0x%04x\r\n", __FUNCTION__, RTC_CIIR, RTC_ILR);
	return (RTC_CIIR);
}
#endif	// PAKAI_KONTROL_RTC


#endif	// __MODUL_RTC__
#endif	// PAKAI_RTC
