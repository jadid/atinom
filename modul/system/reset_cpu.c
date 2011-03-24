/*
	reset_cpu
	
	furkan jadid
	daun biru engineering
	12 Des 2008
*/
#include "FreeRTOS.h"

#define WDOG_TO_RESET	(1000000 * 60)		/* 4/4 MHz * 60 detik atau satu menit */

#define WDEN	BIT(0)
#define WDRESET BIT(1)

void tendang_wdog(void);

void reset_cpu(void)
{
	printf("Resetting cpu ...\n");
	
	WDTC = 300;
	WDMOD = 0x03;
	WDFEED = 0xAA;
	WDFEED = 0x55;
}

void setup_wdog(void)
{
	WDCLKSEL = 0;		// internal clock 4 MHz
	WDTC = WDOG_TO_RESET;
	WDMOD = WDEN | WDRESET;
	
	// start the watchdog
	tendang_wdog();
}

void tendang_wdog(void)
{
	// harusnya saat ini interupt disable
	// tapi masak tidak ada yang sukses di tendang tiap detik
	WDFEED = 0xAA;
	WDFEED = 0x55;
}
