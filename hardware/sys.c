#include "FreeRTOS.h"
#include "sys.h"

void init_PLLnya()	{
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
