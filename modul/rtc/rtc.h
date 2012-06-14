/*
 * rtc.h
 *
 *  Created on: 30 Mei 2012
 *      Author: Afrendy Bayu
 */

#include "FreeRTOS.h"

#if 1
//*
typedef struct  {
    int RTC_Sec;     // Second value - [0,59] //
    int RTC_Min;     // Minute value - [0,59] //
    int RTC_Hour;    // Hour value - [0,23] //
    int RTC_Mday;    // Day of the month value - [1,31] //
    int RTC_Mon;     // Month value - [1,12] //
    int RTC_Year;    // Year value - [0,4095] //
    int RTC_Wday;    // Day of week value - [0,6] //
    int RTC_Yday;    // Day of year value - [1,365] //
} RTCTimex;


struct rtc_time {
  int	tm_sec;
  int	tm_min;
  int	tm_hour;
  int	tm_mday;	// 1 - 31 /
  int	tm_mon;		// 0 - 11 //
  int	tm_year;
  int	tm_wday;
  int	tm_yday;
  int	tm_isdst;
};
//*/
#endif
//void rtcWrite(struct tm *time);
void init_rtc(void);
void start_rtc(void);
void init_rtc_irq(void);
static inline void rtcWake (void);
static inline void rtcSleep (void);
void rtc_reset(void);

