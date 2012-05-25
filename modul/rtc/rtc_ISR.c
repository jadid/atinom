
/************************
       Afrendy Bayu
	Depok, 24 Mei 2012


*************************/

#ifdef KONTROL_RTC

#include "FreeRTOS.h"
//#include "queue.h"
//#include "task.h"
//#include "semphr.h"
//#include "gpio.h"
#include "lpc23xx.h"
#include "system/rtc.h"

static void rtcISR_Handler (void)	{
	if (RTC_ILR & RTC_ILR_RTCCIF)	{		// counter irq
		flagRTCc = 1;
		RTC_ILR |= RTC_ILR_RTCCIF;
	}
	if (RTC_ILR & RTC_ILR_RTCALF)	{
		flagRTCc = 2;
		RTC_ILR |= RTC_ILR_RTCALF;
	}
	
	VICVectAddr = (unsigned portLONG) 0;
/*
	//portBASE_TYPE higherPriorityTaskWoken = pdFALSE;
	RTC_CCR = (RTC_CCR_CLKEN | RTC_CCR_CLKSRC);
	SCB_PCONP |= SCB_PCONP_PCRTC;

	VIC_VectAddr = (unsigned portLONG) 0;

	RTC_CCR = (RTC_CCR_CLKEN | RTC_CCR_CLKSRC);
	SCB_PCONP &= ~SCB_PCONP_PCRTC;

	if (higherPriorityTaskWoken)
		portYIELD_FROM_ISR ();
//*/
}

void rtcISR_Wrapper (void) __attribute__ ((naked));
void rtcISR_Wrapper (void)	{
	portSAVE_CONTEXT ();
	rtcISR_Handler ();
	portRESTORE_CONTEXT ();
}

#endif
