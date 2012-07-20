/*
 * rtc.h
 *
 *  Created on: 30 Mei 2012
 *      Author: Afrendy Bayu
 */

#include "FreeRTOS.h"
#include "lpc23xx.h"
#include "monita/monita_uip.h"

#if 1
#define RTC_ILR_RTCCIF  (0x00000001)
#define RTC_ILR_RTCALF  (0x00000002)
#define RTC_ILR_MASK    (0x00000003)

#define RTC_CCR_CLKEN   (0x00000001)
#define RTC_CCR_CTCRST  (0x00000002)
#define RTC_CCR_TEST    (0x0000000c)
#define RTC_CCR_CLKSRC  (0x00000010)

#define RTC_CIIR_IMSEC  (0x00000001)
#define RTC_CIIR_IMMIN  (0x00000002)
#define RTC_CIIR_IMHOUR (0x00000004)
#define RTC_CIIR_IMDOM  (0x00000008)
#define RTC_CIIR_IMDOW  (0x00000010)
#define RTC_CIIR_IMDOY  (0x00000020)
#define RTC_CIIR_IMMON  (0x00000040)
#define RTC_CIIR_IMYEAR (0x00000080)
#define RTC_CIIR_IMMASK (0x000000ff)

#define RTC_AMR_AMRSEC  (0x00000001)
#define RTC_AMR_AMRMIN  (0x00000002)
#define RTC_AMR_AMRHOUR (0x00000004)
#define RTC_AMR_AMRDOM  (0x00000008)
#define RTC_AMR_AMRDOW  (0x00000010)
#define RTC_AMR_AMRDOY  (0x00000020)
#define RTC_AMR_AMRMON  (0x00000040)
#define RTC_AMR_AMRYEAR (0x00000080)
#define RTC_AMR_AMRMASK (0x000000ff)
#endif


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

