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

static void init_PLL(void)
{
	uint32_t readback;
	
	// clock source berasal dari internal 4 MHz
	// check if PLL connected, disconnect if yes
	if ( PLLSTAT & PLLSTAT_PLLC ) {
		PLLCON = PLLCON_PLLE;       /* Enable PLL, disconnected ( PLLC = 0 )*/
		PLLFEED = 0xaa;
		PLLFEED = 0x55;
	}

	PLLCON  = 0;        /* Disable PLL, disconnected */
	PLLFEED = 0xaa;
	PLLFEED = 0x55;
	
	#ifdef USB_TEST22
	SCS |= BIT(5);   /* Enable main OSC, SCS Man p.28 */
	while( !( SCS & BIT(6) ) ) {
		;	/* Wait until main OSC is usable */
	}
	
	/* main oscilator dari kristal 8 MHZ dipakai */
	CLKSRCSEL = 0x01;
	PLLFEED = 0xaa;
	PLLFEED = 0x55;
	
	USBCLKCFG = USBSEL;
	PLLFEED = 0xaa;
	PLLFEED = 0x55;
	#endif

	PLLCFG = PLLCFG_MSEL | PLLCFG_NSEL;
	PLLFEED = 0xaa;
	PLLFEED = 0x55;

	PLLCON = PLLCON_PLLE;       /* Enable PLL, disconnected ( PLLC = 0 ) */
	PLLFEED = 0xaa;
	PLLFEED = 0x55;

	CCLKCFG = CCLKCFG_CCLKSEL_VAL;     /* Set clock divider, Manual p.45 */

	while ( ( PLLSTAT & PLLSTAT_PLOCK ) == 0 )  {
		; /* Check lock bit status */
	}

	readback = PLLSTAT & 0x00FF7FFF;
	while ( readback != (PLLCFG_MSEL | PLLCFG_NSEL) )
	{
		; // stall - invalid readback
	}

	PLLCON = ( PLLCON_PLLE | PLLCON_PLLC );  /* enable and connect */
	PLLFEED = 0xaa;
	PLLFEED = 0x55;
	while ( ((PLLSTAT & PLLSTAT_PLLC) == 0) ) {
		;  /* Check connect bit status, wait for connect */
	}
}

void ubah_clk( unsigned int clk_div)
{
	uint32_t readback;

	#if 1
	// clock source berasal dari internal 4 MHz
	// check if PLL connected, disconnect if yes
	if ( PLLSTAT & PLLSTAT_PLLC ) {
		PLLCON = PLLCON_PLLE;       /* Enable PLL, disconnected ( PLLC = 0 )*/
		PLLFEED = 0xaa;
		PLLFEED = 0x55;
	}

	PLLCON  = 0;        /* Disable PLL, disconnected */
	PLLFEED = 0xaa;
	PLLFEED = 0x55;

	PLLCFG = PLLCFG_MSEL | PLLCFG_NSEL;
	PLLFEED = 0xaa;
	PLLFEED = 0x55;

	PLLCON = PLLCON_PLLE;       /* Enable PLL, disconnected ( PLLC = 0 ) */
	PLLFEED = 0xaa;
	PLLFEED = 0x55;
	#endif
	
	CCLKCFG = (clk_div - 1);     /* Set clock divider, Manual p.45 */

	while ( ( PLLSTAT & PLLSTAT_PLOCK ) == 0 )  {
		; /* Check lock bit status */
	}

	readback = PLLSTAT & 0x00FF7FFF;
	while ( readback != (PLLCFG_MSEL | PLLCFG_NSEL) )
	{
		; // stall - invalid readback
	}

	PLLCON = ( PLLCON_PLLE | PLLCON_PLLC );  /* enable and connect */
	PLLFEED = 0xaa;
	PLLFEED = 0x55;
	while ( ((PLLSTAT & PLLSTAT_PLLC) == 0) ) {
		;  /* Check connect bit status, wait for connect */
	}
}

static void lowInit(void)
{
	init_PLL();
	// setup & enable the MAM
	MAMCR = 0;
	MAMTIM = 3;
	MAMCR = 2;

	PCLKSEL0 = 0x55555555;	/* PCLK is the same as CCLK */
	PCLKSEL1 = 0x55555555;

	return;
}

void sysInit(void)
{

	//PINSEL0 = PINSEL0 | 0x50; // enable TX & RX

	lowInit();                            			// setup clocks and processor port pins

	// set the interrupt controller defaults
	MEMMAP = MEMMAP_FLASH;                // map interrupt vectors space into FLASH

	SCS |= (1UL<<0); // set GPIOM in SCS for fast IO

	/* paksa PINSEL untuk GPIO */
	PINSEL0 = 0x00000000;
	PINSEL1 = 0x00000000;
	PINSEL2 = 0x00000000;
	PINSEL3 = 0x00000000;
	PINSEL4 = 0x00000000;
	PINSEL5 = 0x00000000;
	PINSEL6 = 0x00000000;
	PINSEL7 = 0x00000000;
	PINSEL8 = 0x00000000;
	PINSEL9 = 0x00000000;
	PINSEL10 = 0x00000000;
	
	/* paksa untuk enable pull up */
	PINMODE0 = 0x00000000;
	PINMODE1 = 0x00000000;
	PINMODE2 = 0x00000000;
	PINMODE3 = 0x00000000;
	PINMODE4 = 0x00000000;
	PINMODE5 = 0x00000000;
	PINMODE6 = 0x00000000;
	PINMODE7 = 0x00000000;
	PINMODE8 = 0x00000000;
	
	PINSEL0 = PINSEL0 | 0x50; // enable TX & RX
	
	#ifdef BOARD_ORDC
	/* power timer0 & uart0 */
	PCONP = BIT(1) | BIT(3);
	#endif
	//PCONP = BIT(1) | BIT(3);
	
	setup_wdog();
}
