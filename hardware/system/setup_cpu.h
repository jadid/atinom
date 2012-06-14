/*
 * setup CPU supaya siap
 
 	FCCO = (2 × M × FIN) / N
 */


#include "FreeRTOS.h"



/*-----------------------------------------------------------*/
#define PLL_MUL				(60)					// 2 * 4 MHz * 60 = 480 MHz
#define PLL_DIV             (1)

#ifdef USB_TEST
/* cristal 8 MHZ, dibagi N = 2, sehingga yang lain2 harusnya tidak berubah */
#define PLL_MUL				(60)					// 2 * 4 MHz * 60 = 480 MHz
#define PLL_DIV             (2)
#define USB_SEL				(10)
#define USBSEL				(USB_SEL - 1)

#endif

#define PLLCFG_MSEL   		((PLL_MUL - 1) << 0) 	// PLL Multiplier
#define PLLCFG_NSEL   		((PLL_DIV - 1) << 16UL) // PLL Divider
#define CCLKCFG_CCLKSEL_VAL ((CCLK_DIV -1 ) << 0UL )
#define CCLK_DIV           	(8)         			// PLL out -> CPU clock divider --> 480 / 8 = 60 MHz
//#define CCLK_DIV           	(6)         			// PLL out -> CPU clock divider --> 480 / 8 = 60 MHz
//#define CCLK_DIV           	(7)         			// PLL out -> CPU clock divider --> 480 / 7 = 69 MHz


typedef unsigned int		uint32_t;
#define MEMMAP_FLASH		1

// PLLCON Register Bit Definitions
#define PLLCON_PLLE   		(1 << 0)          // PLL Enable
#define PLLCON_PLLC   		(1 << 1)          // PLL Connect

// PLLSTAT Register Bit Definitions
#define PLLSTAT_PLLE  		(1UL<<24)
#define PLLSTAT_PLLC  		(1UL<<25)
#define PLLSTAT_PLOCK 		(1UL<<26)

static void init_PLL(void);

void ubah_clk( unsigned int clk_div);

inline static void initMAM();
static void lowInit(void);
#ifdef PAKAI_MODE_POWER
void sleep_mode();
void normal_mode();
void powerdown_mode();
#endif

void sysInit(void);
