/*********************************************************************
 *
 *	Hardware specific definitions
 *
 *********************************************************************
 * FileName:        HardwareProfile.h
 * Dependencies:    None
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.10 or higher
 *					Microchip C30 v3.12 or higher
 *					Microchip C18 v3.34 or higher
 *					HI-TECH PICC-18 PRO 9.63PL2 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2009 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *      digital signal controller product ("Device") which is
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *		ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *		used in conjunction with a Microchip ethernet controller for
 *		the sole purpose of interfacing with the ethernet controller.
 *
 * You should refer to the license agreement accompanying this
 * Software for additional information regarding your rights and
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 *
 * Author               Date		Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder		10/03/06	Original, copied from Compiler.h
 * Ken Hesky            07/01/08    Added ZG2100-specific features
 ********************************************************************/
#ifndef __HARDWARE_PROFILE_H
#define __HARDWARE_PROFILE_H

#include "GenericTypeDefs.h"
#include "Compiler.h"

// Choose which hardware profile to compile for here.  See 
// the hardware profiles below for meaning of various options.  
//#define PICDEMNET2
//#define PIC18_EXPLORER
//#define HPC_EXPLORER
//#define PIC24FJ64GA004_PIM		// Explorer 16, but with the PIC24FJ64GA004 PIM module, which has significantly differnt pin mappings
//#define EXPLORER_16				// PIC24FJ128GA010, PIC24HJ256GP610, dsPIC33FJ256GP710, PIC32MX360F512L, PIC32MX460F512L, PIC32MX795F512L, etc. PIMs
//#define DSPICDEM11
%PIC32_STARTER_KIT%#define PIC32_STARTER_KIT		// PIC32MX360F512L Starter Kit, PIC32MX460F512L USB Starter Board, or PIC32MX795F512L USB Starter Kit II
%PIC32_ETH_STARTER_KIT%#define PIC32_ETH_STARTER_KIT	// PIC32MX795F512L Ethernet Starter Kit board with embedded Ethernet controller
//#define YOUR_BOARD

// If no hardware profiles are defined, assume that we are using 
// a Microchip demo board and try to auto-select the correct profile
// based on processor selected in MPLAB
#if !defined(PICDEMNET2) && !defined(PIC18_EXPLORER) && !defined(HPC_EXPLORER) && !defined(EXPLORER_16) && !defined(PIC24FJ64GA004_PIM) && !defined(DSPICDEM11) && !defined(PICDEMNET2) && !defined(INTERNET_RADIO) && !defined(YOUR_BOARD) && !defined(__PIC24FJ128GA006__) && !defined(PIC32_STARTER_KIT) && !defined(PIC32_ETH_STARTER_KIT)
	#if defined(__18F97J60) || defined(_18F97J60)
		#define PICDEMNET2
	#elif defined(__18F67J60) || defined(_18F67J60)
		#define INTERNET_RADIO
	#elif defined(__18F8722) || defined(__18F87J10) || defined(_18F8722) || defined(_18F87J10) || defined(__18F87J11) || defined(_18F87J11)|| defined(__18F87J50) || defined(_18F87J50)
		#define PIC18_EXPLORER
		//#define HPC_EXPLORER
	#elif defined(__PIC24FJ64GA004__)
		#define PIC24FJ64GA004_PIM
	#elif defined(__PIC24F__) || defined(__PIC24H__) || defined(__dsPIC33F__) || defined(__PIC32MX__)
		#define EXPLORER_16
	#elif defined(__dsPIC30F__)
		#define DSPICDEM11
	#endif
#endif

// Set configuration fuses (but only once)
#if defined(THIS_IS_STACK_APPLICATION)
	#if defined(__18CXX)
		#if defined(__EXTENDED18__)
			#pragma config XINST=ON
		#elif !defined(HI_TECH_C)
			#pragma config XINST=OFF
		#endif
	
		#if defined(__18F8722) && !defined(HI_TECH_C)
			// PICDEM HPC Explorer or PIC18 Explorer board
			#pragma config OSC=HSPLL, FCMEN=OFF, IESO=OFF, PWRT=OFF, WDT=OFF, LVP=OFF
		#elif defined(_18F8722)	// HI-TECH PICC-18 compiler
			// PICDEM HPC Explorer or PIC18 Explorer board with HI-TECH PICC-18 compiler
			__CONFIG(1, HSPLL);
			__CONFIG(2, WDTDIS);
			__CONFIG(3, MCLREN);
			__CONFIG(4, XINSTDIS & LVPDIS);
		#elif defined(__18F87J10) && !defined(HI_TECH_C)
			// PICDEM HPC Explorer or PIC18 Explorer board
			#pragma config WDTEN=OFF, FOSC2=ON, FOSC=HSPLL
		#elif defined(__18F87J11) && !defined(HI_TECH_C)
			// PICDEM HPC Explorer or PIC18 Explorer board
			#pragma config WDTEN=OFF, FOSC=HSPLL
		#elif defined(__18F87J50) && !defined(HI_TECH_C)
			// PICDEM HPC Explorer or PIC18 Explorer board
			#pragma config WDTEN=OFF, FOSC=HSPLL, PLLDIV=3, CPUDIV=OSC1
		#elif (defined(__18F97J60) || defined(__18F96J65) || defined(__18F96J60) || defined(__18F87J60) || defined(__18F86J65) || defined(__18F86J60) || defined(__18F67J60) || defined(__18F66J65) || defined(__18F66J60)) && !defined(HI_TECH_C)
			// PICDEM.net 2 or any other PIC18F97J60 family device
			#pragma config WDT=OFF, FOSC2=ON, FOSC=HSPLL, ETHLED=ON
		#elif defined(_18F97J60) || defined(_18F96J65) || defined(_18F96J60) || defined(_18F87J60) || defined(_18F86J65) || defined(_18F86J60) || defined(_18F67J60) || defined(_18F66J65) || defined(_18F66J60) 
			// PICDEM.net 2 board with HI-TECH PICC-18 compiler
			__CONFIG(1, WDTDIS & XINSTDIS);
			__CONFIG(2, HSPLL);
			__CONFIG(3, ETHLEDEN);
		#elif defined(__18F4620) && !defined(HI_TECH_C)
			#pragma config OSC=HSPLL, WDT=OFF, MCLRE=ON, PBADEN=OFF, LVP=OFF
		#endif
	#elif defined(__PIC24F__)
		// Explorer 16 board
		_CONFIG2(FNOSC_PRIPLL & POSCMOD_XT)		// Primary XT OSC with 4x PLL
		_CONFIG1(JTAGEN_OFF & FWDTEN_OFF)		// JTAG off, watchdog timer off
	#elif defined(__dsPIC33F__) || defined(__PIC24H__)
		// Explorer 16 board
		_FOSCSEL(FNOSC_PRIPLL)			// PLL enabled
		_FOSC(OSCIOFNC_OFF & POSCMD_XT)	// XT Osc
		_FWDT(FWDTEN_OFF)				// Disable Watchdog timer
		// JTAG should be disabled as well
	#elif defined(__dsPIC30F__)
		// dsPICDEM 1.1 board
		_FOSC(XT_PLL16)					// XT Osc + 16X PLL
		_FWDT(WDT_OFF)					// Disable Watchdog timer
		_FBORPOR(MCLR_EN & PBOR_OFF & PWRT_OFF)
	#elif defined(__PIC32MX__)
		#pragma config FPLLODIV = DIV_1, FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FWDTEN = OFF, FPBDIV = DIV_1, POSCMOD = XT, FNOSC = PRIPLL, CP = OFF
		#if defined(PIC32_ETH_STARTER_KIT)
			#pragma config FMIIEN = OFF, FETHIO = OFF	// external PHY in RMII/alternate configuration
		#endif
	#endif
#endif // Prevent more than one set of config fuse definitions

// Clock frequency value.
// This value is used to calculate Tick Counter value
#if defined(__18CXX)
	// All PIC18 processors
	#if defined(PICDEMNET2) || defined(INTERNET_RADIO)
		#define GetSystemClock()		(41666667ul)      // Hz
		#define GetInstructionClock()	(GetSystemClock()/4)
		#define GetPeripheralClock()	GetInstructionClock()
	#elif defined(__18F87J50) || defined(_18F87J50)
		#define GetSystemClock()		(48000000ul)      // Hz
		#define GetInstructionClock()	(GetSystemClock()/4)
		#define GetPeripheralClock()	GetInstructionClock()
	#else
		#define GetSystemClock()		(40000000ul)      // Hz
		#define GetInstructionClock()	(GetSystemClock()/4)
		#define GetPeripheralClock()	GetInstructionClock()
	#endif
#elif defined(__PIC24F__)	
	// PIC24F processor
	#define GetSystemClock()		(32000000ul)      // Hz
	#define GetInstructionClock()	(GetSystemClock()/2)
	#define GetPeripheralClock()	GetInstructionClock()
#elif defined(__PIC24H__)	
	// PIC24H processor
	#define GetSystemClock()		(80000000ul)      // Hz
	#define GetInstructionClock()	(GetSystemClock()/2)
	#define GetPeripheralClock()	GetInstructionClock()
#elif defined(__dsPIC33F__)	
	// dsPIC33F processor
	#define GetSystemClock()		(80000000ul)      // Hz
	#define GetInstructionClock()	(GetSystemClock()/2)
	#define GetPeripheralClock()	GetInstructionClock()
#elif defined(__dsPIC30F__)
	// dsPIC30F processor
	#define GetSystemClock()		(117920000ul)      // Hz
	#define GetInstructionClock()	(GetSystemClock()/4)
	#define GetPeripheralClock()	GetInstructionClock()
#elif defined(__PIC32MX__)
	// PIC32MX processor
	#define GetSystemClock()		(80000000ul)      // Hz
	#define GetInstructionClock()	(GetSystemClock()/1)
	#define GetPeripheralClock()	(GetInstructionClock()/1)	// Set your divider according to your Peripheral Bus Frequency configuration fuse setting
#endif

// Hardware mappings
#if defined(PIC18_EXPLORER) && !defined(HI_TECH_C)
// PIC18 Explorer + Fast 100Mbps Ethernet PICtail Plus or Ethernet PICtail
	// I/O pins
	#define LED0_TRIS			(TRISDbits.TRISD0)
	#define LED0_IO				(LATDbits.LATD0)
	#define LED1_TRIS			(TRISDbits.TRISD1)
	#define LED1_IO				(LATDbits.LATD1)
	#define LED2_TRIS			(TRISDbits.TRISD2)
	#define LED2_IO				(LATDbits.LATD2)
	#define LED3_TRIS			(TRISDbits.TRISD3)
	#define LED3_IO				(LATDbits.LATD3)
	#define LED4_TRIS			(TRISDbits.TRISD4)
	#define LED4_IO				(LATDbits.LATD4)
	#define LED5_TRIS			(TRISDbits.TRISD5)
	#define LED5_IO				(LATDbits.LATD5)
	#define LED6_TRIS			(TRISDbits.TRISD6)
	#define LED6_IO				(LATDbits.LATD6)
	#define LED7_TRIS			(TRISDbits.TRISD7)
	#define LED7_IO				(LATDbits.LATD7)
	#define LED_GET()			(LATD)
	#define LED_PUT(a)			(LATD = (a))

	#define BUTTON0_TRIS		(TRISAbits.TRISA5)
	#define	BUTTON0_IO			(PORTAbits.RA5)
	#define BUTTON1_TRIS		(TRISBbits.TRISB0)
	#define	BUTTON1_IO			(PORTBbits.RB0)
	#define BUTTON2_TRIS		(PRODL)				// No Button2 on this board
	#define	BUTTON2_IO			(1u)
	#define BUTTON3_TRIS		(PRODL)				// No Button3 on this board
	#define	BUTTON3_IO			(1u)


%ENC100_COMMENTS%	// ENC424J600/624J600 Fast 100Mbps Ethernet PICtail Plus defines
%ENC100_COMMENTS%	#define ENC100_INTERFACE_MODE			0		// Uncomment this line to use the ENC424J600/624J600 Ethernet controller (SPI mode) or comment it out to use some other network controller
%ENC100_COMMENTS%	
%ENC100_COMMENTS%	// ENC100_MDIX, ENC100_POR, and ENC100_INT are all optional.  Simply leave 
%ENC100_COMMENTS%	// them commented out if you don't have such a hardware feature on your 
%ENC100_COMMENTS%	// board.
%ENC100_COMMENTS%	#define ENC100_MDIX_TRIS				(TRISBbits.TRISB4)
%ENC100_COMMENTS%	#define ENC100_MDIX_IO					(LATBbits.LATB4)
%ENC100_COMMENTS%//	#define ENC100_POR_TRIS					(TRISBbits.TRISB5)
%ENC100_COMMENTS%//	#define ENC100_POR_IO					(LATBbits.LATB5)
%ENC100_COMMENTS%//	#define ENC100_INT_TRIS					(TRISBbits.TRISB2)
%ENC100_COMMENTS%//	#define ENC100_INT_IO					(PORTBbits.RB2)
%ENC100_COMMENTS%
%ENC100_COMMENTS%	// ENC424J600/624J600 SPI pinout
%ENC100_COMMENTS%	#define ENC100_CS_TRIS					(TRISBbits.TRISB3)
%ENC100_COMMENTS%	#define ENC100_CS_IO					(LATBbits.LATB3)
%ENC100_COMMENTS%	#define ENC100_SO_WR_B0SEL_EN_TRIS		(TRISCbits.TRISC4)	// NOTE: SO is ENC624J600 Serial Out, which needs to connect to the PIC SDI pin for SPI mode
%ENC100_COMMENTS%	#define ENC100_SO_WR_B0SEL_EN_IO		(PORTCbits.RC4)
%ENC100_COMMENTS%	#define ENC100_SI_RD_RW_TRIS			(TRISCbits.TRISC5)	// NOTE: SI is ENC624J600 Serial In, which needs to connect to the PIC SDO pin for SPI mode
%ENC100_COMMENTS%	#define ENC100_SI_RD_RW_IO				(LATCbits.LATC5)
%ENC100_COMMENTS%	#define ENC100_SCK_AL_TRIS				(TRISCbits.TRISC3)
%ENC100_COMMENTS%	#define ENC100_SCK_AL_IO				(PORTCbits.RC3)		// NOTE: This must be the PORT, not the LATch like it is for the PSP interface.
%ENC100_COMMENTS%
%ENC100_COMMENTS%	// ENC424J600/624J600 SPI SFR register selection (controls which SPI 
%ENC100_COMMENTS%	// peripheral to use on PICs with multiple SPI peripherals).
%ENC100_COMMENTS%//	#define ENC100_ISR_ENABLE		(INTCON3bits.INT2IE)
%ENC100_COMMENTS%//	#define ENC100_ISR_FLAG			(INTCON3bits.INT2IF)
%ENC100_COMMENTS%//	#define ENC100_ISR_POLARITY		(INTCON2bits.INTEDG2)
%ENC100_COMMENTS%//	#define ENC100_ISR_PRIORITY		(INTCON3bits.INT2IP)
%ENC100_COMMENTS%	#define ENC100_SPI_ENABLE		(ENC100_SPISTATbits.SPIEN)
%ENC100_COMMENTS%	#define ENC100_SPI_IF			(PIR1bits.SSPIF)
%ENC100_COMMENTS%	#define ENC100_SSPBUF			(SSP1BUF)
%ENC100_COMMENTS%	#define ENC100_SPISTAT			(SSP1STAT)
%ENC100_COMMENTS%	#define ENC100_SPISTATbits		(SSP1STATbits)
%ENC100_COMMENTS%	#define ENC100_SPICON1			(SSP1CON1)
%ENC100_COMMENTS%	#define ENC100_SPICON1bits		(SSP1CON1bits)
%ENC100_COMMENTS%	#define ENC100_SPICON2			(SSP1CON2)

%ENC28J60_COMMENTS%	// ENC28J60 I/O pins
%ENC28J60_COMMENTS%	#define ENC_RST_TRIS		(TRISBbits.TRISB5)
%ENC28J60_COMMENTS%	#define ENC_RST_IO			(LATBbits.LATB5)
%ENC28J60_COMMENTS%	#define ENC_CS_TRIS			(TRISBbits.TRISB3)	// Uncomment this line to use the ENC28J60 Ethernet controller or comment it out to use some other network controller
%ENC28J60_COMMENTS%	#define ENC_CS_IO			(LATBbits.LATB3)
%ENC28J60_COMMENTS%	#define ENC_SCK_TRIS		(TRISCbits.TRISC3)
%ENC28J60_COMMENTS%	#define ENC_SDI_TRIS		(TRISCbits.TRISC4)
%ENC28J60_COMMENTS%	#define ENC_SDO_TRIS		(TRISCbits.TRISC5)
%ENC28J60_COMMENTS%	#define ENC_SPI_IF			(PIR1bits.SSPIF)
%ENC28J60_COMMENTS%	#define ENC_SSPBUF			(SSP1BUF)
%ENC28J60_COMMENTS%	#define ENC_SPISTAT			(SSP1STAT)
%ENC28J60_COMMENTS%	#define ENC_SPISTATbits		(SSP1STATbits)
%ENC28J60_COMMENTS%	#define ENC_SPICON1			(SSP1CON1)
%ENC28J60_COMMENTS%	#define ENC_SPICON1bits		(SSP1CON1bits)
%ENC28J60_COMMENTS%	#define ENC_SPICON2			(SSP1CON2)

%EEPROM_COMMENTS%	// 25LC256 I/O pins
%EEPROM_COMMENTS%	#define EEPROM_CS_TRIS		(TRISAbits.TRISA3)
%EEPROM_COMMENTS%	#define EEPROM_CS_IO		(LATAbits.LATA3)
%EEPROM_COMMENTS%	#define EEPROM_SCK_TRIS		(TRISCbits.TRISC3)
%EEPROM_COMMENTS%	#define EEPROM_SDI_TRIS		(TRISCbits.TRISC4)
%EEPROM_COMMENTS%	#define EEPROM_SDO_TRIS		(TRISCbits.TRISC5)
%EEPROM_COMMENTS%	#define EEPROM_SPI_IF		(PIR1bits.SSPIF)
%EEPROM_COMMENTS%	#define EEPROM_SSPBUF		(SSP1BUF)
%EEPROM_COMMENTS%	#define EEPROM_SPICON1		(SSP1CON1)
%EEPROM_COMMENTS%	#define EEPROM_SPICON1bits	(SSP1CON1bits)
%EEPROM_COMMENTS%	#define EEPROM_SPICON2		(SSP1CON2)
%EEPROM_COMMENTS%	#define EEPROM_SPISTAT		(SSP1STAT)
%EEPROM_COMMENTS%	#define EEPROM_SPISTATbits	(SSP1STATbits)

	// LCD I/O pins
	// TODO: Need to add support for LCD behind MCP23S17 I/O expander.  This 
	// requires code that isn't in the TCP/IP stack, not just a hardware 
	// profile change.

	// Serial Flash/SRAM/UART PICtail
//	#define SPIRAM_CS_TRIS			(TRISBbits.TRISB5)
//	#define SPIRAM_CS_IO			(LATBbits.LATB5)
//	#define SPIRAM_SCK_TRIS			(TRISCbits.TRISC3)
//	#define SPIRAM_SDI_TRIS			(TRISCbits.TRISC4)
//	#define SPIRAM_SDO_TRIS			(TRISCbits.TRISC5)
//	#define SPIRAM_SPI_IF			(PIR1bits.SSPIF)
//	#define SPIRAM_SSPBUF			(SSP1BUF)
//	#define SPIRAM_SPICON1			(SSP1CON1)
//	#define SPIRAM_SPICON1bits		(SSP1CON1bits)
//	#define SPIRAM_SPICON2			(SSP1CON2)
//	#define SPIRAM_SPISTAT			(SSP1STAT)
//	#define SPIRAM_SPISTATbits		(SSP1STATbits)
//
//	// NOTE: You must also set the SPI_FLASH_SST/SPI_FLASH_SPANSION, 
//	//       SPI_FLASH_SECTOR_SIZE, and SPI_FLASH_PAGE_SIZE macros in 
//	//       SPIFlash.h to match your particular Flash memory chip!!!
//	#define SPIFLASH_CS_TRIS		(TRISBbits.TRISB4)
//	#define SPIFLASH_CS_IO			(LATBbits.LATB4)
//	#define SPIFLASH_SCK_TRIS		(TRISCbits.TRISC3)
//	#define SPIFLASH_SDI_TRIS		(TRISCbits.TRISC4)
//	#define SPIFLASH_SDI_IO			(PORTCbits.RC4)
//	#define SPIFLASH_SDO_TRIS		(TRISCbits.TRISC5)
//	#define SPIFLASH_SPI_IF			(PIR1bits.SSPIF)
//	#define SPIFLASH_SSPBUF			(SSP1BUF)
//	#define SPIFLASH_SPICON1		(SSP1CON1)
//	#define SPIFLASH_SPICON1bits	(SSP1CON1bits)
//	#define SPIFLASH_SPICON2		(SSP1CON2)
//	#define SPIFLASH_SPISTAT		(SSP1STAT)
//	#define SPIFLASH_SPISTATbits	(SSP1STATbits)

	// Register name fix up for certain processors
	#define SPBRGH				SPBRGH1
	#if defined(__18F87J50) || defined(_18F87J50) || defined(__18F87J11) || defined(_18F87J11)
		#define ADCON2		ADCON1
	#endif

#elif defined(PIC18_EXPLORER) && defined(HI_TECH_C)
// PIC18 Explorer + Fast 100Mbps Ethernet PICtail Plus or Ethernet PICtail + HI-TECH PICC-18 compiler
	// I/O pins
	#define LED0_TRIS			(TRISD0)
	#define LED0_IO				(LATD0)
	#define LED1_TRIS			(TRISD1)
	#define LED1_IO				(LATD1)
	#define LED2_TRIS			(TRISD2)
	#define LED2_IO				(LATD2)
	#define LED3_TRIS			(TRISD3)
	#define LED3_IO				(LATD3)
	#define LED4_TRIS			(TRISD4)
	#define LED4_IO				(LATD4)
	#define LED5_TRIS			(TRISD5)
	#define LED5_IO				(LATD5)
	#define LED6_TRIS			(TRISD6)
	#define LED6_IO				(LATD6)
	#define LED7_TRIS			(TRISD7)
	#define LED7_IO				(LATD7)
	#define LED_GET()			(LATD)
	#define LED_PUT(a)			(LATD = (a))

	#define BUTTON0_TRIS		(TRISA5)
	#define	BUTTON0_IO			(RA5)
	#define BUTTON1_TRIS		(TRISB0)
	#define	BUTTON1_IO			(RB0)
	#define BUTTON2_TRIS		(PRODL)				// No Button2 on this board
	#define	BUTTON2_IO			(1u)
	#define BUTTON3_TRIS		(PRODL)				// No Button3 on this board
	#define	BUTTON3_IO			(1u)

%ENC100_COMMENTS%	// ENC424J600/624J600 Fast 100Mbps Ethernet PICtail Plus defines
%ENC100_COMMENTS%	#define ENC100_INTERFACE_MODE			0		// Uncomment this line to use the ENC424J600/624J600 Ethernet controller (SPI mode) or comment it out to use some other network controller
%ENC100_COMMENTS%
%ENC100_COMMENTS%	// ENC100_MDIX, ENC100_POR, and ENC100_INT are all optional.  Simply leave 
%ENC100_COMMENTS%	// them commented out if you don't have such a hardware feature on your 
%ENC100_COMMENTS%	// hardware.
%ENC100_COMMENTS%	#define ENC100_MDIX_TRIS				(TRISB4)
%ENC100_COMMENTS%	#define ENC100_MDIX_IO					(LATB4)
%ENC100_COMMENTS%//	#define ENC100_POR_TRIS					(TRISB5)
%ENC100_COMMENTS%//	#define ENC100_POR_IO					(LATB5)
%ENC100_COMMENTS%//	#define ENC100_INT_TRIS					(TRISB2)
%ENC100_COMMENTS%//	#define ENC100_INT_IO					(RB2)
%ENC100_COMMENTS%
%ENC100_COMMENTS%	// ENC424J600/624J600 SPI pinout
%ENC100_COMMENTS%	#define ENC100_CS_TRIS					(TRISB3)
%ENC100_COMMENTS%	#define ENC100_CS_IO					(LATB3)
%ENC100_COMMENTS%	#define ENC100_SO_WR_B0SEL_EN_TRIS		(TRISC4)	// NOTE: SO is ENC624J600 Serial Out, which needs to connect to the PIC SDI pin for SPI mode
%ENC100_COMMENTS%	#define ENC100_SO_WR_B0SEL_EN_IO		(RC4)
%ENC100_COMMENTS%	#define ENC100_SI_RD_RW_TRIS			(TRISC5)	// NOTE: SI is ENC624J600 Serial In, which needs to connect to the PIC SDO pin for SPI mode
%ENC100_COMMENTS%	#define ENC100_SI_RD_RW_IO				(LATC5)
%ENC100_COMMENTS%	#define ENC100_SCK_AL_TRIS				(TRISC3)
%ENC100_COMMENTS%	#define ENC100_SCK_AL_IO				(RC3)		// NOTE: This must be the PORT, not the LATch like it is for the PSP interface.
%ENC100_COMMENTS%
%ENC100_COMMENTS%	// ENC424J600/624J600 SPI SFR register selection (controls which SPI 
%ENC100_COMMENTS%	// peripheral to use on PICs with multiple SPI peripherals).
%ENC100_COMMENTS%//	#define ENC100_ISR_ENABLE		(INT2IE)
%ENC100_COMMENTS%//	#define ENC100_ISR_FLAG			(INT2IF)
%ENC100_COMMENTS%//	#define ENC100_ISR_POLARITY		(INTEDG2)
%ENC100_COMMENTS%//	#define ENC100_ISR_PRIORITY		(INT2IP)
%ENC100_COMMENTS%	#define ENC100_SPI_ENABLE		(ENC100_SPISTATbits.SPIEN)
%ENC100_COMMENTS%	#define ENC100_SPI_IF			(SSP1IF)
%ENC100_COMMENTS%	#define ENC100_SSPBUF			(SSP1BUF)
%ENC100_COMMENTS%	#define ENC100_SPISTAT			(SSP1STAT)
%ENC100_COMMENTS%	#define ENC100_SPISTATbits		(SSP1STATbits)
%ENC100_COMMENTS%	#define ENC100_SPICON1			(SSP1CON1)
%ENC100_COMMENTS%	#define ENC100_SPICON1bits		(SSP1CON1bits)
%ENC100_COMMENTS%	#define ENC100_SPICON2			(SSP1CON2)

%ENC28J60_COMMENTS%	// ENC28J60 I/O pins
%ENC28J60_COMMENTS%	#define ENC_RST_TRIS		(TRISB5)
%ENC28J60_COMMENTS%	#define ENC_RST_IO			(LATB5)
%ENC28J60_COMMENTS%	#define ENC_CS_TRIS			(TRISB3)	// Uncomment this line to use the ENC28J60 Ethernet controller or comment it out to use some other network controller
%ENC28J60_COMMENTS%	#define ENC_CS_IO			(LATB3)
%ENC28J60_COMMENTS%	#define ENC_SCK_TRIS		(TRISC3)
%ENC28J60_COMMENTS%	#define ENC_SDI_TRIS		(TRISC4)
%ENC28J60_COMMENTS%	#define ENC_SDO_TRIS		(TRISC5)
%ENC28J60_COMMENTS%	#define ENC_SPI_IF			(SSP1IF)
%ENC28J60_COMMENTS%	#define ENC_SSPBUF			(SSP1BUF)
%ENC28J60_COMMENTS%	#define ENC_SPISTAT			(SSP1STAT)
%ENC28J60_COMMENTS%	#define ENC_SPISTATbits		(SSP1STATbits)
%ENC28J60_COMMENTS%	#define ENC_SPICON1			(SSP1CON1)
%ENC28J60_COMMENTS%	#define ENC_SPICON1bits		(SSP1CON1bits)
%ENC28J60_COMMENTS%	#define ENC_SPICON2			(SSP1CON2)

%EEPROM_COMMENTS%	// 25LC256 I/O pins
%EEPROM_COMMENTS%	#define EEPROM_CS_TRIS		(TRISA3)
%EEPROM_COMMENTS%	#define EEPROM_CS_IO		(LATA3)
%EEPROM_COMMENTS%	#define EEPROM_SCK_TRIS		(TRISC3)
%EEPROM_COMMENTS%	#define EEPROM_SDI_TRIS		(TRISC4)
%EEPROM_COMMENTS%	#define EEPROM_SDO_TRIS		(TRISC5)
%EEPROM_COMMENTS%	#define EEPROM_SPI_IF		(SSP1IF)
%EEPROM_COMMENTS%	#define EEPROM_SSPBUF		(SSP1BUF)
%EEPROM_COMMENTS%	#define EEPROM_SPICON1		(SSP1CON1)
%EEPROM_COMMENTS%	#define EEPROM_SPICON1bits	(SSP1CON1bits)
%EEPROM_COMMENTS%	#define EEPROM_SPICON2		(SSP1CON2)
%EEPROM_COMMENTS%	#define EEPROM_SPISTAT		(SSP1STAT)
%EEPROM_COMMENTS%	#define EEPROM_SPISTATbits	(SSP1STATbits)

	// LCD I/O pins
	// TODO: Need to add support for LCD behind MCP23S17 I/O expander.  This 
	// requires code that isn't in the TCP/IP stack, not just a hardware 
	// profile change.

	// Register name fix up for certain processors
	#define SPBRGH				SPBRGH1
	#define TXSTAbits			TXSTA1bits
	#define RCSTAbits			RCSTA1bits
	#define BAUDCONbits			BAUDCON1bits
	#define GO					GODONE
	#if defined(__18F87J50) || defined(_18F87J50) || defined(__18F87J11) || defined(_18F87J11)
		#define ADCON2		ADCON1
	#endif

#elif defined(HPC_EXPLORER) && !defined(HI_TECH_C)
// PICDEM HPC Explorer + Fast 100Mbps Ethernet PICtail Plus or Ethernet PICtail
	// I/O pins
	#define LED0_TRIS			(TRISDbits.TRISD0)
	#define LED0_IO				(LATDbits.LATD0)
	#define LED1_TRIS			(TRISDbits.TRISD1)
	#define LED1_IO				(LATDbits.LATD1)
	#define LED2_TRIS			(TRISDbits.TRISD2)
	#define LED2_IO				(LATDbits.LATD2)
	#define LED3_TRIS			(TRISDbits.TRISD3)
	#define LED3_IO				(LATDbits.LATD3)
	#define LED4_TRIS			(TRISDbits.TRISD4)
	#define LED4_IO				(LATDbits.LATD4)
	#define LED5_TRIS			(TRISDbits.TRISD5)
	#define LED5_IO				(LATDbits.LATD5)
	#define LED6_TRIS			(TRISDbits.TRISD6)
	#define LED6_IO				(LATDbits.LATD6)
	#define LED7_TRIS			(TRISDbits.TRISD7)
	#define LED7_IO				(LATDbits.LATD7)
	#define LED_GET()			(LATD)
	#define LED_PUT(a)			(LATD = (a))


	#define BUTTON0_TRIS		(TRISBbits.TRISB0)
	#define	BUTTON0_IO			(PORTBbits.RB0)
	#define BUTTON1_TRIS		(TRISBbits.TRISB0)	// No Button1 on this board, remap to Button0
	#define	BUTTON1_IO			(PORTBbits.RB0)
	#define BUTTON2_TRIS		(TRISBbits.TRISB0)	// No Button2 on this board, remap to Button0
	#define	BUTTON2_IO			(PORTBbits.RB0)
	#define BUTTON3_TRIS		(TRISBbits.TRISB0)	// No Button3 on this board, remap to Button0
	#define	BUTTON3_IO			(PORTBbits.RB0)

%ENC100_COMMENTS%	// ENC424J600/624J600 Fast 100Mbps Ethernet PICtail Plus defines
%ENC100_COMMENTS%	#define ENC100_INTERFACE_MODE			0		// Uncomment this line to use the ENC424J600/624J600 Ethernet controller (SPI mode) or comment it out to use some other network controller
%ENC100_COMMENTS%
%ENC100_COMMENTS%	// ENC424J600/624J600 Fast 100Mbps Ethernet PICtail Plus I/O pins
%ENC100_COMMENTS%	#define ENC100_MDIX_TRIS				(TRISBbits.TRISB4)
%ENC100_COMMENTS%	#define ENC100_MDIX_IO					(LATBbits.LATB4)
%ENC100_COMMENTS%	#define ENC100_POR_TRIS					(TRISBbits.TRISB5)
%ENC100_COMMENTS%	#define ENC100_POR_IO					(LATBbits.LATB5)
%ENC100_COMMENTS%//	#define ENC100_INT_TRIS					(TRISBbits.TRISB2)
%ENC100_COMMENTS%//	#define ENC100_INT_IO					(PORTBbits.RB2)
%ENC100_COMMENTS%
%ENC100_COMMENTS%	// ENC424J600/624J600 SPI pinout
%ENC100_COMMENTS%	#define ENC100_CS_TRIS					(TRISBbits.TRISB3)
%ENC100_COMMENTS%	#define ENC100_CS_IO					(LATBbits.LATB3)
%ENC100_COMMENTS%	#define ENC100_SO_WR_B0SEL_EN_TRIS		(TRISCbits.TRISC4)	// NOTE: SO is ENC624J600 Serial Out, which needs to connect to the PIC SDI pin for SPI mode
%ENC100_COMMENTS%	#define ENC100_SO_WR_B0SEL_EN_IO		(PORTCbits.RC4)
%ENC100_COMMENTS%	#define ENC100_SI_RD_RW_TRIS			(TRISCbits.TRISC5)	// NOTE: SI is ENC624J600 Serial In, which needs to connect to the PIC SDO pin for SPI mode
%ENC100_COMMENTS%	#define ENC100_SI_RD_RW_IO				(LATCbits.LATC5)
%ENC100_COMMENTS%	#define ENC100_SCK_AL_TRIS				(TRISCbits.TRISC3)
%ENC100_COMMENTS%	#define ENC100_SCK_AL_IO				(PORTCbits.RC3)		// NOTE: This must be the PORT, not the LATch like it is for the PSP interface.
%ENC100_COMMENTS%
%ENC100_COMMENTS%	// ENC424J600/624J600 SPI SFR register selection (controls which SPI 
%ENC100_COMMENTS%	// peripheral to use on PICs with multiple SPI peripherals).
%ENC100_COMMENTS%//	#define ENC100_ISR_ENABLE		(INTCON3bits.INT2IE)
%ENC100_COMMENTS%//	#define ENC100_ISR_FLAG			(INTCON3bits.INT2IF)
%ENC100_COMMENTS%//	#define ENC100_ISR_POLARITY		(INTCON2bits.INTEDG2)
%ENC100_COMMENTS%//	#define ENC100_ISR_PRIORITY		(INTCON3bits.INT2IP)
%ENC100_COMMENTS%	#define ENC100_SPI_ENABLE		(ENC100_SPISTATbits.SPIEN)
%ENC100_COMMENTS%	#define ENC100_SPI_IF			(PIR1bits.SSPIF)
%ENC100_COMMENTS%	#define ENC100_SSPBUF			(SSP1BUF)
%ENC100_COMMENTS%	#define ENC100_SPISTAT			(SSP1STAT)
%ENC100_COMMENTS%	#define ENC100_SPISTATbits		(SSP1STATbits)
%ENC100_COMMENTS%	#define ENC100_SPICON1			(SSP1CON1)
%ENC100_COMMENTS%	#define ENC100_SPICON1bits		(SSP1CON1bits)
%ENC100_COMMENTS%	#define ENC100_SPICON2			(SSP1CON2)

%ENC28J60_COMMENTS%	// ENC28J60 I/O pins
%ENC28J60_COMMENTS%	#define ENC_RST_TRIS		(TRISBbits.TRISB5)
%ENC28J60_COMMENTS%	#define ENC_RST_IO			(LATBbits.LATB5)
%ENC28J60_COMMENTS%	#define ENC_CS_TRIS			(TRISBbits.TRISB3)	// Uncomment this line to use the ENC28J60 Ethernet controller or comment it out to use some other network controller
%ENC28J60_COMMENTS%	#define ENC_CS_IO			(LATBbits.LATB3)
%ENC28J60_COMMENTS%	#define ENC_SCK_TRIS		(TRISCbits.TRISC3)
%ENC28J60_COMMENTS%	#define ENC_SDI_TRIS		(TRISCbits.TRISC4)
%ENC28J60_COMMENTS%	#define ENC_SDO_TRIS		(TRISCbits.TRISC5)
%ENC28J60_COMMENTS%	#define ENC_SPI_IF			(PIR1bits.SSPIF)
%ENC28J60_COMMENTS%	#define ENC_SSPBUF			(SSP1BUF)
%ENC28J60_COMMENTS%	#define ENC_SPISTAT			(SSP1STAT)
%ENC28J60_COMMENTS%	#define ENC_SPISTATbits		(SSP1STATbits)
%ENC28J60_COMMENTS%	#define ENC_SPICON1			(SSP1CON1)
%ENC28J60_COMMENTS%	#define ENC_SPICON1bits		(SSP1CON1bits)
%ENC28J60_COMMENTS%	#define ENC_SPICON2			(SSP1CON2)

%EEPROM_COMMENTS%	// 25LC256 I/O pins
%EEPROM_COMMENTS%	#define EEPROM_CS_TRIS		(TRISBbits.TRISB4)
%EEPROM_COMMENTS%	#define EEPROM_CS_IO		(LATBbits.LATB4)
%EEPROM_COMMENTS%	#define EEPROM_SCK_TRIS		(TRISCbits.TRISC3)
%EEPROM_COMMENTS%	#define EEPROM_SDI_TRIS		(TRISCbits.TRISC4)
%EEPROM_COMMENTS%	#define EEPROM_SDO_TRIS		(TRISCbits.TRISC5)
%EEPROM_COMMENTS%	#define EEPROM_SPI_IF		(PIR1bits.SSPIF)
%EEPROM_COMMENTS%	#define EEPROM_SSPBUF		(SSP1BUF)
%EEPROM_COMMENTS%	#define EEPROM_SPICON1		(SSP1CON1)
%EEPROM_COMMENTS%	#define EEPROM_SPICON1bits	(SSP1CON1bits)
%EEPROM_COMMENTS%	#define EEPROM_SPICON2		(SSP1CON2)
%EEPROM_COMMENTS%	#define EEPROM_SPISTAT		(SSP1STAT)
%EEPROM_COMMENTS%	#define EEPROM_SPISTATbits	(SSP1STATbits)

	// Register name fix up for certain processors
	#define SPBRGH				SPBRGH1
	#if defined(__18F87J50) || defined(_18F87J50) || defined(__18F87J11) || defined(_18F87J11)
		#define ADCON2		ADCON1
	#endif
	
#elif defined(HPC_EXPLORER) && defined(HI_TECH_C)
// PICDEM HPC Explorer + Ethernet PICtail	
	#define TXSTA				TXSTA1
	#define RCSTA				RCSTA1
	#define SPBRG				SPBRG1
	#define SPBRGH				SPBRGH1
	#define RCREG				RCREG1
	#define TXREG				TXREG1

	// I/O pins
	#define LED0_TRIS			(TRISD0)
	#define LED0_IO				(LATD0)
	#define LED1_TRIS			(TRISD1)
	#define LED1_IO				(LATD1)
	#define LED2_TRIS			(TRISD2)
	#define LED2_IO				(LATD2)
	#define LED3_TRIS			(TRISD3)
	#define LED3_IO				(LATD3)
	#define LED4_TRIS			(TRISD4)
	#define LED4_IO				(LATD4)
	#define LED5_TRIS			(TRISD5)
	#define LED5_IO				(LATD5)
	#define LED6_TRIS			(TRISD6)
	#define LED6_IO				(LATD6)
	#define LED7_TRIS			(TRISD7)
	#define LED7_IO				(LATD7)
	#define LED_GET()			(LATD)
	#define LED_PUT(a)			(LATD = (a))

	#define BUTTON0_TRIS		(TRISB0)
	#define	BUTTON0_IO			(RB0)
	#define BUTTON1_TRIS		(TRISB0)	// No Button1 on this board, remap to Button0
	#define	BUTTON1_IO			(RB0)
	#define BUTTON2_TRIS		(TRISB0)	// No Button2 on this board, remap to Button0
	#define	BUTTON2_IO			(RB0)
	#define BUTTON3_TRIS		(TRISB0)	// No Button3 on this board, remap to Button0
	#define	BUTTON3_IO			(RB0)

%ENC28J60_COMMENTS%	// ENC28J60 I/O pins
%ENC28J60_COMMENTS%	#define ENC_RST_TRIS		(TRISB5)
%ENC28J60_COMMENTS%	#define ENC_RST_IO			(LATB5)
%ENC28J60_COMMENTS%	#define ENC_CS_TRIS			(TRISB3)
%ENC28J60_COMMENTS%	#define ENC_CS_IO			(LATB3)
%ENC28J60_COMMENTS%	#define ENC_SCK_TRIS		(TRISC3)
%ENC28J60_COMMENTS%	#define ENC_SDI_TRIS		(TRISC4)
%ENC28J60_COMMENTS%	#define ENC_SDO_TRIS		(TRISC5)
%ENC28J60_COMMENTS%	#define ENC_SPI_IF			(SSP1IF)
%ENC28J60_COMMENTS%	#define ENC_SSPBUF			(SSP1BUF)
%ENC28J60_COMMENTS%	#define ENC_SPISTAT			(SSP1STAT)
%ENC28J60_COMMENTS%	#define ENC_SPISTATbits		(SSP1STATbits)
%ENC28J60_COMMENTS%	#define ENC_SPICON1			(SSP1CON1)
%ENC28J60_COMMENTS%	#define ENC_SPICON1bits		(SSP1CON1bits)
%ENC28J60_COMMENTS%	#define ENC_SPICON2			(SSP1CON2)

%EEPROM_COMMENTS%	// 25LC256 I/O pins
%EEPROM_COMMENTS%	#define EEPROM_CS_TRIS		(TRISB4)
%EEPROM_COMMENTS%	#define EEPROM_CS_IO		(LATB4)
%EEPROM_COMMENTS%	#define EEPROM_SCK_TRIS		(TRISC3)
%EEPROM_COMMENTS%	#define EEPROM_SDI_TRIS		(TRISC4)
%EEPROM_COMMENTS%	#define EEPROM_SDO_TRIS		(TRISC5)
%EEPROM_COMMENTS%	#define EEPROM_SPI_IF		(SSP1IF)
%EEPROM_COMMENTS%	#define EEPROM_SSPBUF		(SSP1BUF)
%EEPROM_COMMENTS%	#define EEPROM_SPICON1		(SSP1CON1)
%EEPROM_COMMENTS%	#define EEPROM_SPICON1bits	(SSP1CON1bits)
%EEPROM_COMMENTS%	#define EEPROM_SPICON2		(SSP1CON2)
%EEPROM_COMMENTS%	#define EEPROM_SPISTAT		(SSP1STAT)
%EEPROM_COMMENTS%	#define EEPROM_SPISTATbits	(SSP1STATbits)

#elif defined(PIC24FJ64GA004_PIM)
// Explorer 16 + PIC24FJ64GA004 PIM + Ethernet PICtail Plus

	// Push Button I/O pins
	#define BUTTON3_TRIS		TRISAbits.TRISA10		// Mutliplexed with LED0
	#define BUTTON3_IO			PORTAbits.RA10
	#define BUTTON2_TRIS		TRISAbits.TRISA9		// Multiplexed with LED4
	#define BUTTON2_IO			PORTAbits.RA9
	#define BUTTON1_TRIS		TRISCbits.TRISC6		// Multiplexed with LED7
	#define BUTTON1_IO			PORTCbits.RC6
	#define BUTTON0_TRIS		TRISAbits.TRISA7		// Multiplexed with LED1
	#define BUTTON0_IO			PORTAbits.RA7
		
	// LED I/O pins
	#define LED0_TRIS			TRISAbits.TRISA10		// Multiplexed with BUTTON3
	#define LED0_IO				LATAbits.LATA10
	#define LED1_TRIS			TRISAbits.TRISA7		// Multiplexed with BUTTON0
	#define LED1_IO				LATAbits.LATA7				
	#define LED2_TRIS			TRISBbits.TRISB8		// Multiplexed with LCD_DATA4
	#define LED2_IO				LATBbits.LATB8
	#define LED3_TRIS			TRISBbits.TRISB9		// Multiplexed with LCD_DATA3
	#define LED3_IO				LATBbits.LATB9
	#define LED4_TRIS			TRISAbits.TRISA9		// Multiplexed with BUTTON2
	#define LED4_IO				LATAbits.LATA9
	#define LED5_TRIS			TRISAbits.TRISA8		// Multiplexed with EEPROM_CS
	#define LED5_IO				LATAbits.LATA8
	#define LED6_TRIS			TRISBbits.TRISB12		// Multiplexed with LCD_DATA0
	#define LED6_IO				LATBbits.LATB12
	#define LED7_TRIS			TRISCbits.TRISC6		// Multiplexed with BUTTON1
	#define LED7_IO				LATCbits.LATC6
	#define LED_GET()			(0u)
	#define LED_PUT(a)			
	
	// UART I/O Mapping
	#define UARTTX_TRIS			(TRISCbits.TRISC9)
	#define UARTTX_IO			(PORTCbits.RC9)
	#define UARTRX_TRIS			(TRISCbits.TRISC3)
	#define UARTRX_IO			(PORTCbits.RC3)

%ENC28J60_COMMENTS%	// ENC28J60 I/O pins
%ENC28J60_COMMENTS%	#define ENC_RST_TRIS		(TRISCbits.TRISC8)	// Not connected by default
%ENC28J60_COMMENTS%	#define ENC_RST_IO			(PORTCbits.RC8)
%ENC28J60_COMMENTS%	#define ENC_CS_TRIS			(TRISBbits.TRISB3)
%ENC28J60_COMMENTS%	#define ENC_CS_IO			(PORTBbits.RB3)
%ENC28J60_COMMENTS%	// SPI SCK, SDI, SDO pins are automatically controlled by the 
%ENC28J60_COMMENTS%	// PIC24/dsPIC/PIC32 SPI module 
%ENC28J60_COMMENTS%	#define ENC_SPI_IF			(IFS0bits.SPI1IF)
%ENC28J60_COMMENTS%	#define ENC_SSPBUF			(SPI1BUF)
%ENC28J60_COMMENTS%	#define ENC_SPISTAT			(SPI1STAT)
%ENC28J60_COMMENTS%	#define ENC_SPISTATbits		(SPI1STATbits)
%ENC28J60_COMMENTS%	#define ENC_SPICON1			(SPI1CON1)
%ENC28J60_COMMENTS%	#define ENC_SPICON1bits		(SPI1CON1bits)
%ENC28J60_COMMENTS%	#define ENC_SPICON2			(SPI1CON2)

%EEPROM_COMMENTS%	// 25LC256 I/O pins
%EEPROM_COMMENTS%	#define EEPROM_CS_TRIS		(TRISAbits.TRISA8)
%EEPROM_COMMENTS%	#define EEPROM_CS_IO		(PORTAbits.RA8)
%EEPROM_COMMENTS%	#define EEPROM_SCK_TRIS		(TRISCbits.TRISC8)
%EEPROM_COMMENTS%	#define EEPROM_SDI_TRIS		(TRISCbits.TRISC4)
%EEPROM_COMMENTS%	#define EEPROM_SDO_TRIS		(TRISCbits.TRISC5)
%EEPROM_COMMENTS%	#define EEPROM_SPI_IF		(IFS2bits.SPI2IF)
%EEPROM_COMMENTS%	#define EEPROM_SSPBUF		(SPI2BUF)
%EEPROM_COMMENTS%	#define EEPROM_SPICON1		(SPI2CON1)
%EEPROM_COMMENTS%	#define EEPROM_SPICON1bits	(SPI2CON1bits)
%EEPROM_COMMENTS%	#define EEPROM_SPICON2		(SPI2CON2)
%EEPROM_COMMENTS%	#define EEPROM_SPISTAT		(SPI2STAT)
%EEPROM_COMMENTS%	#define EEPROM_SPISTATbits	(SPI2STATbits)

	// LCD Module I/O pins
	#define LCD_DATA0_TRIS		(TRISBbits.TRISB12)		// Multiplexed with LED6
	#define LCD_DATA0_IO		(LATBbits.LATB12)
	#define LCD_DATA1_TRIS		(TRISBbits.TRISB11)
	#define LCD_DATA1_IO		(LATBbits.LATB11)
	#define LCD_DATA2_TRIS		(TRISBbits.TRISB10)
	#define LCD_DATA2_IO		(LATBbits.LATB10)
	#define LCD_DATA3_TRIS		(TRISBbits.TRISB9)		// Multiplexed with LED3
	#define LCD_DATA3_IO		(LATBbits.LATB9)
	#define LCD_DATA4_TRIS		(TRISBbits.TRISB8)		// Multiplexed with LED2
	#define LCD_DATA4_IO		(LATBbits.LATB8)
	#define LCD_DATA5_TRIS		(TRISBbits.TRISB7)
	#define LCD_DATA5_IO		(LATBbits.LATB7)
	#define LCD_DATA6_TRIS		(TRISBbits.TRISB6)
	#define LCD_DATA6_IO		(LATBbits.LATB6)
	#define LCD_DATA7_TRIS		(TRISBbits.TRISB5)
	#define LCD_DATA7_IO		(LATBbits.LATB5)
	#define LCD_RD_WR_TRIS		(TRISBbits.TRISB13)
	#define LCD_RD_WR_IO		(LATBbits.LATB13)
	#define LCD_RS_TRIS			(TRISCbits.TRISC7)
	#define LCD_RS_IO			(LATCbits.LATC7)
	#define LCD_E_TRIS			(TRISBbits.TRISB14)
	#define LCD_E_IO			(LATBbits.LATB14)

	// Peripheral Pin Select Outputs
	#define NULL_IO		0
	#define C1OUT_IO	1
	#define C2OUT_IO	2
	#define U1TX_IO		3
	#define U1RTS_IO	4
	#define U2TX_IO		5
	#define U2RTS_IO	6
	#define SDO1_IO		7
	#define SCK1OUT_IO	8
	#define SS1OUT_IO	9
	#define SDO2_IO		10
	#define SCK2OUT_IO	11
	#define SS2OUT_IO	12
	#define OC1_IO		18
	#define OC2_IO		19
	#define OC3_IO		20
	#define OC4_IO		21
	#define OC5_IO		22

#elif defined(EXPLORER_16)
// Explorer 16 + PIC24FJ128GA010/PIC24HJ256GP610/dsPIC33FJ256GP710/
//				 PIC32MX460F512L/PIC32MX360F512L/PIC32MX795F512L PIM + 
//               Fast 100Mbps Ethernet PICtail Plus or Ethernet PICtail Plus or ZeroG ZG2100M WiFi PICtail Plus

	#define LED0_TRIS			(TRISAbits.TRISA0)	// Ref D3
	#define LED0_IO				(LATAbits.LATA0)	
	#define LED1_TRIS			(TRISAbits.TRISA1)	// Ref D4
	#define LED1_IO				(LATAbits.LATA1)
	#define LED2_TRIS			(TRISAbits.TRISA2)	// Ref D5
	#define LED2_IO				(LATAbits.LATA2)
	#define LED3_TRIS			(TRISAbits.TRISA3)	// Ref D6
	#define LED3_IO				(LATAbits.LATA3)
	#define LED4_TRIS			(TRISAbits.TRISA4)	// Ref D7
	#define LED4_IO				(LATAbits.LATA4)
	#define LED5_TRIS			(TRISAbits.TRISA5)	// Ref D8
	#define LED5_IO				(LATAbits.LATA5)
	#define LED6_TRIS			(TRISAbits.TRISA6)	// Ref D9
	#define LED6_IO				(LATAbits.LATA6)
	#define LED7_TRIS			(TRISAbits.TRISA7)	// Ref D10	// Note: This is multiplexed with BUTTON1
	#define LED7_IO				(LATAbits.LATA7)
	#define LED_GET()			(*((volatile unsigned char*)(&LATA)))
	#define LED_PUT(a)			(*((volatile unsigned char*)(&LATA)) = (a))


	#define BUTTON0_TRIS		(TRISDbits.TRISD13)	// Ref S4
	#define	BUTTON0_IO			(PORTDbits.RD13)
	#define BUTTON1_TRIS		(TRISAbits.TRISA7)	// Ref S5	// Note: This is multiplexed with LED7
	#define	BUTTON1_IO			(PORTAbits.RA7)
	#define BUTTON2_TRIS		(TRISDbits.TRISD7)	// Ref S6
	#define	BUTTON2_IO			(PORTDbits.RD7)
	#define BUTTON3_TRIS		(TRISDbits.TRISD6)	// Ref S3
	#define	BUTTON3_IO			(PORTDbits.RD6)

	#define UARTTX_TRIS			(TRISFbits.TRISF5)
	#define UARTTX_IO			(PORTFbits.RF5)
	#define UARTRX_TRIS			(TRISFbits.TRISF4)
	#define UARTRX_IO			(PORTFbits.RF4)

%ENC28J60_COMMENTS%	// ENC28J60 I/O pins
%ENC28J60_COMMENTS%	#define ENC_CS_TRIS			(TRISDbits.TRISD14)	// Comment this line out if you are using the ENC424J600/624J600, ZeroG ZG2100, or other network controller.
%ENC28J60_COMMENTS%	#define ENC_CS_IO			(PORTDbits.RD14)
%ENC28J60_COMMENTS%	//#define ENC_RST_TRIS		(TRISDbits.TRISD15)	// Not connected by default.  It is okay to leave this pin completely unconnected, in which case this macro should simply be left undefined.
%ENC28J60_COMMENTS%	//#define ENC_RST_IO			(PORTDbits.RD15)
%ENC28J60_COMMENTS%	// SPI SCK, SDI, SDO pins are automatically controlled by the 
%ENC28J60_COMMENTS%	// PIC24/dsPIC/PIC32 SPI module 
%ENC28J60_COMMENTS%	#if defined(__C30__)	// PIC24F, PIC24H, dsPIC30, dsPIC33
%ENC28J60_COMMENTS%		#define ENC_SPI_IF			(IFS0bits.SPI1IF)
%ENC28J60_COMMENTS%		#define ENC_SSPBUF			(SPI1BUF)
%ENC28J60_COMMENTS%		#define ENC_SPISTAT			(SPI1STAT)
%ENC28J60_COMMENTS%		#define ENC_SPISTATbits		(SPI1STATbits)
%ENC28J60_COMMENTS%		#define ENC_SPICON1			(SPI1CON1)
%ENC28J60_COMMENTS%		#define ENC_SPICON1bits		(SPI1CON1bits)
%ENC28J60_COMMENTS%		#define ENC_SPICON2			(SPI1CON2)
%ENC28J60_COMMENTS%	#else					// PIC32
%ENC28J60_COMMENTS%		#define ENC_SPI_IF			(IFS0bits.SPI1RXIF)
%ENC28J60_COMMENTS%		#define ENC_SSPBUF			(SPI1BUF)
%ENC28J60_COMMENTS%		#define ENC_SPISTATbits		(SPI1STATbits)
%ENC28J60_COMMENTS%		#define ENC_SPICON1			(SPI1CON)
%ENC28J60_COMMENTS%		#define ENC_SPICON1bits		(SPI1CONbits)
%ENC28J60_COMMENTS%		#define ENC_SPIBRG			(SPI1BRG)
%ENC28J60_COMMENTS%	#endif


%ENC100_COMMENTS%	// ENC624J600 Interface Configuration
%ENC100_COMMENTS%	// Comment out ENC100_INTERFACE_MODE if you don't have an ENC624J600 or 
%ENC100_COMMENTS%	// ENC424J600.  Otherwise, choose the correct setting for the interface you 
%ENC100_COMMENTS%	// are using.  Legal values are:
%ENC100_COMMENTS%	//  - Commented out: No ENC424J600/624J600 present or used.  All other 
%ENC100_COMMENTS%	//                   ENC100_* macros are ignored.
%ENC100_COMMENTS%	//	- 0: SPI mode using CS, SCK, SI, and SO pins
%ENC100_COMMENTS%	//  - 1: 8-bit demultiplexed PSP Mode 1 with RD and WR pins
%ENC100_COMMENTS%	//  - 2: *8-bit demultiplexed PSP Mode 2 with R/Wbar and EN pins
%ENC100_COMMENTS%	//  - 3: *16-bit demultiplexed PSP Mode 3 with RD, WRL, and WRH pins
%ENC100_COMMENTS%	//  - 4: *16-bit demultiplexed PSP Mode 4 with R/Wbar, B0SEL, and B1SEL pins
%ENC100_COMMENTS%	//  - 5: 8-bit multiplexed PSP Mode 5 with RD and WR pins
%ENC100_COMMENTS%	//  - 6: *8-bit multiplexed PSP Mode 6 with R/Wbar and EN pins
%ENC100_COMMENTS%	//  - 9: 16-bit multiplexed PSP Mode 9 with AL, RD, WRL, and WRH pins
%ENC100_COMMENTS%	//  - 10: *16-bit multiplexed PSP Mode 10 with AL, R/Wbar, B0SEL, and B1SEL 
%ENC100_COMMENTS%	//        pins
%ENC100_COMMENTS%	// *IMPORTANT NOTE: DO NOT USE PSP MODE 2, 4, 6, OR 10 ON EXPLORER 16! 
%ENC100_COMMENTS%	// Attempting to do so will cause bus contention with the LCD module which 
%ENC100_COMMENTS%	// shares the PMP.  Also, PSP Mode 3 is risky on the Explorer 16 since it 
%ENC100_COMMENTS%	// can randomly cause bus contention with the 25LC256 EEPROM.
%ENC100_COMMENTS%	#define ENC100_INTERFACE_MODE			%ENC100_INTERFACE_MODE%
%ENC100_COMMENTS%
%ENC100_COMMENTS%	// If using a parallel interface, direct RAM addressing can be used (if all 
%ENC100_COMMENTS%	// addresses wires are connected), or a reduced number of pins can be used 
%ENC100_COMMENTS%	// for indirect addressing.  If using an SPI interface or PSP Mode 9 or 10 
%ENC100_COMMENTS%	// (multiplexed 16-bit modes), which require all address lines to always be 
%ENC100_COMMENTS%	// connected, then this option is ignored. Comment out or uncomment this 
%ENC100_COMMENTS%	// macro to match your hardware connections.
%ENC100_COMMENTS%	#define ENC100_PSP_USE_INDIRECT_RAM_ADDRESSING
%ENC100_COMMENTS%
%ENC100_COMMENTS%	// ENC424J600/624J600 parallel indirect address remapping macro function.
%ENC100_COMMENTS%	// This section translates SFR and RAM addresses presented to the 
%ENC100_COMMENTS%	// ReadMemory() and WriteMemory() APIs in ENCX24J600.c to the actual 
%ENC100_COMMENTS%	// addresses that must be presented on the parallel interface.  This macro 
%ENC100_COMMENTS%	// must be modified to match your hardware if you are using an indirect PSP 
%ENC100_COMMENTS%	// addressing mode (ENC100_PSP_USE_INDIRECT_RAM_ADDRESSING is defined) and 
%ENC100_COMMENTS%	// have some of your address lines tied off to Vdd.  If you are using the 
%ENC100_COMMENTS%	// SPI interface, then this section can be ignored or deleted.
%ENC100_COMMENTS%	#if (ENC100_INTERFACE_MODE == 1) || (ENC100_INTERFACE_MODE == 2) || (ENC100_INTERFACE_MODE == 5) || (ENC100_INTERFACE_MODE == 6) // 8-bit PSP
%ENC100_COMMENTS%		#define ENC100_TRANSLATE_TO_PIN_ADDR(a)		((((a)&0x0100)<<6) | ((a)&0x00FF))
%ENC100_COMMENTS%	#elif (ENC100_INTERFACE_MODE == 3) || (ENC100_INTERFACE_MODE == 4) // 16-bit PSP
%ENC100_COMMENTS%		#define ENC100_TRANSLATE_TO_PIN_ADDR(a)		(a)
%ENC100_COMMENTS%	#endif
%ENC100_COMMENTS%
%ENC100_COMMENTS%	// Auto-crossover pins on Fast 100Mbps Ethernet PICtail/PICtail Plus.  If 
%ENC100_COMMENTS%	// your circuit doesn't have such a feature, delete these two defines.
%ENC100_COMMENTS%	#define ENC100_MDIX_TRIS				(TRISBbits.TRISB3)
%ENC100_COMMENTS%	#define ENC100_MDIX_IO					(LATBbits.LATB3)
%ENC100_COMMENTS%
%ENC100_COMMENTS%	// ENC624J600 I/O control and status pins
%ENC100_COMMENTS%	// If a pin is not required for your selected ENC100_INTERFACE_MODE 
%ENC100_COMMENTS%	// interface selection (ex: WRH/B1SEL for PSP modes 1, 2, 5, and 6), then 
%ENC100_COMMENTS%	// you can ignore, delete, or put anything for the pin definition.  Also, 
%ENC100_COMMENTS%	// the INT and POR pins are entirely optional.  If not connected, comment 
%ENC100_COMMENTS%	// them out.
%ENC100_COMMENTS%	#if defined(__dsPIC33FJ256GP710__) || defined(__PIC24HJ256GP610__)
%ENC100_COMMENTS%		#define ENC100_INT_TRIS				(TRISAbits.TRISA13)		// INT signal is optional and currently unused in the Microchip TCP/IP Stack.  Leave this pin disconnected and comment out this pin definition if you don't want it.
%ENC100_COMMENTS%		#define ENC100_INT_IO				(PORTAbits.RA13)
%ENC100_COMMENTS%	#else
%ENC100_COMMENTS%		#define ENC100_INT_TRIS				(TRISEbits.TRISE9)		// INT signal is optional and currently unused in the Microchip TCP/IP Stack.  Leave this pin disconnected and comment out this pin definition if you don't want it.
%ENC100_COMMENTS%		#define ENC100_INT_IO				(PORTEbits.RE9)
%ENC100_COMMENTS%	#endif
%ENC100_COMMENTS%	#if (ENC100_INTERFACE_MODE >= 1)	// Parallel mode
%ENC100_COMMENTS%		// PSP control signal pinout
%ENC100_COMMENTS%		#define ENC100_CS_TRIS					(TRISAbits.TRISA5)	// CS is optional in PSP mode.  If you are not sharing the parallel bus with another device, tie CS to Vdd and comment out this pin definition.
%ENC100_COMMENTS%		#define ENC100_CS_IO					(LATAbits.LATA5)
%ENC100_COMMENTS%		#define ENC100_POR_TRIS					(TRISCbits.TRISC1)	// POR signal is optional.  If your application doesn't have a power disconnect feature, comment out this pin definition.
%ENC100_COMMENTS%		#define ENC100_POR_IO					(LATCbits.LATC1)
%ENC100_COMMENTS%		#define ENC100_SO_WR_B0SEL_EN_TRIS		(TRISDbits.TRISD4)
%ENC100_COMMENTS%		#define ENC100_SO_WR_B0SEL_EN_IO		(LATDbits.LATD4)
%ENC100_COMMENTS%		#define ENC100_SI_RD_RW_TRIS			(TRISDbits.TRISD5)
%ENC100_COMMENTS%		#define ENC100_SI_RD_RW_IO				(LATDbits.LATD5)
%ENC100_COMMENTS%		#define ENC100_SCK_AL_TRIS				(TRISBbits.TRISB15)
%ENC100_COMMENTS%		#define ENC100_SCK_AL_IO				(LATBbits.LATB15)
%ENC100_COMMENTS%	#else	
%ENC100_COMMENTS%		// SPI pinout
%ENC100_COMMENTS%		#define ENC100_CS_TRIS					(TRISDbits.TRISD14)	// CS is mandatory when using the SPI interface
%ENC100_COMMENTS%		#define ENC100_CS_IO					(LATDbits.LATD14)
%ENC100_COMMENTS%		#define ENC100_POR_TRIS					(TRISDbits.TRISD15)	// POR signal is optional.  If your application doesn't have a power disconnect feature, comment out this pin definition.
%ENC100_COMMENTS%		#define ENC100_POR_IO					(LATDbits.LATD15)
%ENC100_COMMENTS%		#define ENC100_SO_WR_B0SEL_EN_TRIS		(TRISFbits.TRISF7)	// SO is ENCX24J600 Serial Out, which needs to connect to the PIC SDI pin for SPI mode
%ENC100_COMMENTS%		#define ENC100_SO_WR_B0SEL_EN_IO		(PORTFbits.RF7)
%ENC100_COMMENTS%		#define ENC100_SI_RD_RW_TRIS			(TRISFbits.TRISF8)	// SI is ENCX24J600 Serial In, which needs to connect to the PIC SDO pin for SPI mode
%ENC100_COMMENTS%		#define ENC100_SI_RD_RW_IO				(LATFbits.LATF8)
%ENC100_COMMENTS%		#define ENC100_SCK_AL_TRIS				(TRISFbits.TRISF6)
%ENC100_COMMENTS%	#endif
%ENC100_COMMENTS%
%ENC100_COMMENTS%	// ENC624J600 Bit Bang PSP I/O macros and pin configuration for address and 
%ENC100_COMMENTS%	// data.  If using the SPI interface (ENC100_INTERFACE_MODE is 0) then this 
%ENC100_COMMENTS%	// section is not used and can be ignored or deleted.  If using the PIC PMP
%ENC100_COMMENTS%	// hardware module (if present), then ENC100_BIT_BANG_PMP must be commented 
%ENC100_COMMENTS%	// out and the remaining definitions will be ignored/can be deleted.  
%ENC100_COMMENTS%	// Otherwise, if you are using a parallel interface mode, but do not have a 
%ENC100_COMMENTS%	// PMP (or want to interface using different pins), define 
%ENC100_COMMENTS%	// ENC100_BIT_BANG_PMP and properly configure the applicable macros.
%ENC100_COMMENTS%	%ENC100_BIT_BANG_PMP%#define ENC100_BIT_BANG_PMP
%ENC100_COMMENTS%	#if defined(ENC100_BIT_BANG_PMP)
%ENC100_COMMENTS%		#if ENC100_INTERFACE_MODE == 1 || ENC100_INTERFACE_MODE == 2	// Dumultiplexed 8-bit address/data modes
%ENC100_COMMENTS%			// SPI2 CANNOT BE ENABLED WHEN ACCESSING THE ENC624J600 FOR THESE TWO MODES AS THE PINS OVERLAP WITH ADDRESS LINES.
%ENC100_COMMENTS%			#if defined(ENC100_PSP_USE_INDIRECT_RAM_ADDRESSING)	// Only ENC624J600 address pins A0-A8 connected (A9-A14 tied to Vdd)
%ENC100_COMMENTS%				#define ENC100_INIT_PSP_BIT_BANG()	do{((volatile BYTE*)&AD1PCFGH)[1] = 0xFF; ((volatile BYTE*)&AD1PCFGL)[1] |= 0xC0;}while(0)	// Disable AN24-AN31 and AN14-AN15 analog inputs on RE0-RE7 and RB14-RB15 pins (ENCX24J600 AD0-AD7, A1-A0)
%ENC100_COMMENTS%				#define ENC100_SET_ADDR_TRIS_OUT()	do{TRISB &= 0x3FFF; TRISG &= 0xFC3F; TRISA &= 0xF9FF; TRISDbits.TRISD11 = 0;}while(0)
%ENC100_COMMENTS%				#define ENC100_SET_ADDR_IO(a)		do{WORD _SetMacro = (a); LATBbits.LATB15 = 0; LATBbits.LATB14 = 0; LATG &= 0xFC3F; LATAbits.LATA10 = 0; LATAbits.LATA9 = 0; LATDbits.LATD11 = 0; if(_SetMacro & 0x0001) LATBbits.LATB15 = 1; if(_SetMacro & 0x0002) LATBbits.LATB14 = 1; if(_SetMacro & 0x0004) LATGbits.LATG9 = 1; if(_SetMacro & 0x0008) LATGbits.LATG8 = 1; if(_SetMacro & 0x0010) LATGbits.LATG7 = 1; if(_SetMacro & 0x0020) LATGbits.LATG6 = 1; if(_SetMacro & 0x0040) LATAbits.LATA10 = 1; if(_SetMacro & 0x0080) LATAbits.LATA9 = 1; if(_SetMacro & 0x4000) LATDbits.LATD11 = 1;}while(0)
%ENC100_COMMENTS%				#define ENC100_SET_AD_TRIS_IN()		(((volatile BYTE*)&TRISE)[0] = 0xFF)
%ENC100_COMMENTS%				#define ENC100_SET_AD_TRIS_OUT()	(((volatile BYTE*)&TRISE)[0] = 0x00)
%ENC100_COMMENTS%				#define ENC100_GET_AD_IO()			(((volatile BYTE*)&PORTE)[0])
%ENC100_COMMENTS%				#define ENC100_SET_AD_IO(data)		(((volatile BYTE*)&LATE)[0] = (data))
%ENC100_COMMENTS%			#else 	// All ENC624J600 address pins A0-A14 connected
%ENC100_COMMENTS%				#define ENC100_INIT_PSP_BIT_BANG()	do{((volatile BYTE*)&AD1PCFGH)[1] = 0xFF; ((volatile BYTE*)&AD1PCFGL)[1] |= 0xFC;}while(0)	// Disable AN24-AN31 and AN10-AN15 analog inputs on RE0-RE7 and RB10-RB15 pins (ENCX24J600 AD0-AD7, A1-A0, A13-A10)
%ENC100_COMMENTS%				#define ENC100_SET_ADDR_TRIS_OUT()	do{TRISB &= 0x03FF; TRISG &= 0xFC3F; TRISA &= 0xF9FF; TRISF &= 0xFFCF; TRISDbits.TRISD11 = 0;}while(0)
%ENC100_COMMENTS%				#define ENC100_SET_ADDR_IO(a)		do{WORD _SetMacro = (a); LATB &= 0x03FF; LATG &= 0xFC3F; LATAbits.LATA10 = 0; LATAbits.LATA9 = 0; LATFbits.LATF5 = 0; LATFbits.LATF4 = 0; LATDbits.LATD11 = 0; if(_SetMacro & 0x0001) LATBbits.LATB15 = 1; if(_SetMacro & 0x0002) LATBbits.LATB14 = 1; if(_SetMacro & 0x0004) LATGbits.LATG9 = 1; if(_SetMacro & 0x0008) LATGbits.LATG8 = 1; if(_SetMacro & 0x0010) LATGbits.LATG7 = 1; if(_SetMacro & 0x0020) LATGbits.LATG6 = 1; if(_SetMacro & 0x0040) LATAbits.LATA10 = 1; if(_SetMacro & 0x0080) LATAbits.LATA9 = 1; if(_SetMacro & 0x0100) LATFbits.LATF5 = 1; if(_SetMacro & 0x0200) LATFbits.LATF4 = 1; if(_SetMacro & 0x0400) LATBbits.LATB13 = 1; if(_SetMacro & 0x0800) LATBbits.LATB12 = 1; if(_SetMacro & 0x1000) LATBbits.LATB11 = 1; if(_SetMacro & 0x2000) LATBbits.LATB10 = 1; if(_SetMacro & 0x4000) LATDbits.LATD11 = 1;}while(0)
%ENC100_COMMENTS%				#define ENC100_SET_AD_TRIS_IN()		(((volatile BYTE*)&TRISE)[0] = 0xFF)
%ENC100_COMMENTS%				#define ENC100_SET_AD_TRIS_OUT()	(((volatile BYTE*)&TRISE)[0] = 0x00)
%ENC100_COMMENTS%				#define ENC100_GET_AD_IO()			(((volatile BYTE*)&PORTE)[0])
%ENC100_COMMENTS%				#define ENC100_SET_AD_IO(data)		(((volatile BYTE*)&LATE)[0] = (data))
%ENC100_COMMENTS%			#endif
%ENC100_COMMENTS%		#elif ENC100_INTERFACE_MODE == 3 || ENC100_INTERFACE_MODE == 4	// Dumultiplexed 16-bit address/data modes
%ENC100_COMMENTS%			#if defined(ENC100_PSP_USE_INDIRECT_RAM_ADDRESSING)	// Only ENC624J600 address pins A0-A7 connected (A8-A13 tied to Vdd)
%ENC100_COMMENTS%				#define ENC100_INIT_PSP_BIT_BANG()	do{AD1PCFGH = 0xFFFF; AD1PCFGL = 0xFFFF; AD2PCFGL = 0xFFFF;}while(0)
%ENC100_COMMENTS%				#define ENC100_SET_ADDR_TRIS_OUT()	do{TRISB &= 0x3FFF; TRISG &= 0xFC3F; TRISA &= 0xF9FF;}while(0)
%ENC100_COMMENTS%				#define ENC100_SET_ADDR_IO(a)		do{WORD _SetMacro = (a); LATBbits.LATB15 = 0; LATBbits.LATB14 = 0; LATG &= 0xFC3F; LATAbits.LATA10 = 0; LATAbits.LATA9 = 0; if(_SetMacro & 0x0001) LATBbits.LATB15 = 1; if(_SetMacro & 0x0002) LATBbits.LATB14 = 1; if(_SetMacro & 0x0004) LATGbits.LATG9 = 1; if(_SetMacro & 0x0008) LATGbits.LATG8 = 1; if(_SetMacro & 0x0010) LATGbits.LATG7 = 1; if(_SetMacro & 0x0020) LATGbits.LATG6 = 1; if(_SetMacro & 0x0040) LATAbits.LATA10 = 1; if(_SetMacro & 0x0080) LATAbits.LATA9 = 1;}while(0)
%ENC100_COMMENTS%				#define ENC100_WRH_B1SEL_TRIS		ENC100_SO_WR_B0SEL_EN_TRIS
%ENC100_COMMENTS%				#define ENC100_WRH_B1SEL_IO			ENC100_SO_WR_B0SEL_EN_IO
%ENC100_COMMENTS%				#define ENC100_SET_AD_TRIS_IN()		do{((volatile BYTE*)&TRISE)[0] = 0xFF; TRISG |= 0x0003; TRISF |= 0x0003; TRISD |= 0x30C0;}while(0)
%ENC100_COMMENTS%				#define ENC100_SET_AD_TRIS_OUT()	do{((volatile BYTE*)&TRISE)[0] = 0x00; TRISG &= 0xFFFC; TRISF |= 0xFFFC; TRISD |= 0xCF3F;}while(0)
%ENC100_COMMENTS%				#define ENC100_GET_AD_IOL()			(((volatile BYTE*)&PORTE)[0])
%ENC100_COMMENTS%				#if defined(__ENCX24J600_C)
%ENC100_COMMENTS%				    static inline __attribute__((__always_inline__)) BYTE ENC100_GET_AD_IOH(void)
%ENC100_COMMENTS%				    {
%ENC100_COMMENTS%					    BYTE_VAL ret = {0}; if(PORTGbits.RG0) ret.bits.b0 = 1; if(PORTGbits.RG1) ret.bits.b1 = 1; if(PORTFbits.RF1) ret.bits.b2 = 1; if(PORTFbits.RF0) ret.bits.b3 = 1; if(PORTDbits.RD12) ret.bits.b4 = 1; if(PORTDbits.RD13) ret.bits.b5 = 1; if(PORTDbits.RD6) ret.bits.b6 = 1; if(PORTDbits.RD7) ret.bits.b7 = 1; return ret.Val;
%ENC100_COMMENTS%				    }
%ENC100_COMMENTS%				#endif
%ENC100_COMMENTS%				#define ENC100_GET_AD_IO()			((((volatile BYTE*)&PORTE)[0]) | ENC100_GET_AD_IOH();)
%ENC100_COMMENTS%				#define ENC100_SET_AD_IO(data)		do{WORD _SetMacro = (data); ((volatile BYTE*)&LATE)[0] = (BYTE)_SetMacro; LATG &= 0xFFFC; LATF |= 0xFFFC; LATD |= 0xCF3F; if(_SetMacro & 0x0100) LATGbits.LATG0 = 1; if(_SetMacro & 0x0200) LATGbits.LATG1 = 1; if(_SetMacro & 0x0400) LATFbits.LATF1 = 1; if(_SetMacro & 0x0800) LATFbits.LATF0 = 1; if(_SetMacro & 0x1000) LATDbits.LATD12 = 1; if(_SetMacro & 0x2000) LATDbits.LATD13 = 1; if (_SetMacro & 0x4000) LATDbits.LATD6 = 1; if(_SetMacro & 0x8000) LATDbits.LATD7 = 1;}while(0)
%ENC100_COMMENTS%			#else 	// All ENC624J600 address pins A0-A13 connected
%ENC100_COMMENTS%				#define ENC100_INIT_PSP_BIT_BANG()	do{AD1PCFGH = 0xFFFF; AD1PCFGL = 0xFFFF; AD2PCFGL = 0xFFFF;}while(0)
%ENC100_COMMENTS%				#define ENC100_SET_ADDR_TRIS_OUT()	do{TRISE &= 0xFF00; TRISG &= 0x8FF0;}while(0)
%ENC100_COMMENTS%				#define ENC100_SET_ADDR_IO(a)		do{WORD _wAddrSetMacro = (a); LATBbits.LATB15 = ((((BYTE*)&_wAddrSetMacro)[0]) & 0x1) == 0x1; LATBbits.LATB14 = ((((BYTE*)&_wAddrSetMacro)[0]) & 0x2) == 0x2; LATGbits.LATG9 = ((((BYTE*)&_wAddrSetMacro)[0]) & 0x4) == 0x4; LATG8bits.LATG8 = ((((BYTE*)&_wAddrSetMacro)[0]) & 0x8) == 0x8; LATGbits.LATG7 = ((((BYTE*)&_wAddrSetMacro)[0]) & 0x10) == 0x10; LATGbits.LATG6 = ((((BYTE*)&_wAddrSetMacro)[0]) & 0x20) == 0x20; LATAbits.LATA10 = ((((BYTE*)&_wAddrSetMacro)[0]) & 0x40) == 0x40; LATAbits.LATA9 = ((((BYTE*)&_wAddrSetMacro)[0]) & 0x80) == 0x80; LATDbits.LATD11 = ((((BYTE*)&_wAddrSetMacro)[1]) & 0x40) == 0x40;}while(0)
%ENC100_COMMENTS%				#define ENC100_WRH_B1SEL_TRIS		ENC100_SO_WR_B0SEL_EN_TRIS
%ENC100_COMMENTS%				#define ENC100_WRH_B1SEL_IO			ENC100_SO_WR_B0SEL_EN_IO
%ENC100_COMMENTS%				#define ENC100_SET_AD_TRIS_IN		do{TRISD = 0xFFFF;}while(0)
%ENC100_COMMENTS%				#define ENC100_SET_AD_TRIS_OUT		do{TRISD = 0x0000;}while(0)
%ENC100_COMMENTS%				#define ENC100_GET_AD_IO()			(((volatile WORD*)&PORTD)[0])
%ENC100_COMMENTS%				#define ENC100_SET_AD_IO(data)		(((volatile WORD*)&LATD)[0] = (data))
%ENC100_COMMENTS%			#endif
%ENC100_COMMENTS%		#elif ENC100_INTERFACE_MODE == 5 || ENC100_INTERFACE_MODE == 6	// Mutliplexed 8-bit address/data modes
%ENC100_COMMENTS%			#if defined(ENC100_PSP_USE_INDIRECT_RAM_ADDRESSING)	// Only ENCX24J600 address pins AD0-AD8 connected (AD9-AD14 tied to Vdd)
%ENC100_COMMENTS%				#define ENC100_INIT_PSP_BIT_BANG()	do{((volatile BYTE*)&AD1PCFGH)[1] = 0xFF;}while(0)	// Disable AN24-AN31 analog inputs on RE0-RE7 pins (ENCX24J600 AD0-AD7)
%ENC100_COMMENTS%				#define ENC100_SET_AD_TRIS_IN		do{((volatile BYTE*)&TRISE)[0] = 0xFF;}while(0)
%ENC100_COMMENTS%				#define ENC100_SET_AD_TRIS_OUT		do{((volatile BYTE*)&TRISE)[0] = 0x00; TRISDbits.TRISD11 = 0;}while(0)
%ENC100_COMMENTS%				#define ENC100_GET_AD_IO()			(((volatile BYTE*)&PORTE)[0])
%ENC100_COMMENTS%				#define ENC100_SET_AD_IO(data)		do{WORD _wSetMacro = (data); ((volatile BYTE*)&LATE)[0] = (BYTE)_wSetMacro; LATDbits.LATD11 = 0; if(_wSetMacro & 0x4000) LATDbits.LATD11 = 1;}while(0)
%ENC100_COMMENTS%				#define ENC100_SET_AD_IOL(data)		(((volatile BYTE*)&LATE)[0] = (BYTE)(data))
%ENC100_COMMENTS%			#else 	// All ENCX24J600 address pins AD0-AD14 connected
%ENC100_COMMENTS%				// This pinout is bad for doing 8-bit bit-bang operations with all address lines.  The Fast 100Mbps Ethernet PICtail Plus hardware is wired for PMP hardware support, which requires this pinout.  However, if you are designing a custom board, you can simplify these read/write operations dramatically if you wire things more logically by putting all 15 I/O pins, in order, on PORTB or PORTD.  Such a change would enhance performance.
%ENC100_COMMENTS%				// UART2 CANNOT BE USED OR ENABLED FOR THESE TWO MODES AS THE PINS OVERLAP WITH ADDRESS LINES.
%ENC100_COMMENTS%				#define ENC100_INIT_PSP_BIT_BANG()	do{AD1PCFGL |= 0x3C00; ((volatile BYTE*)&AD1PCFGH)[1] = 0xFF;}while(0)	// Disable AN10-AN13 and AN24-AN31 analog inputs on RB10-RB13 and RE0-RE7 pins (ENCX24J600 AD13-AD10 and AD0-AD7)
%ENC100_COMMENTS%				#define ENC100_SET_AD_TRIS_IN		do{((volatile BYTE*)&TRISE)[0] = 0xFF; TRISF |= 0x0030; TRISB |= 0x3C00; TRISD |= 0x0C00;}while(0)
%ENC100_COMMENTS%				#define ENC100_SET_AD_TRIS_OUT		do{((volatile BYTE*)&TRISE)[0] = 0x00; TRISF &= 0xFFCF; TRISB &= 0xC3FF; TRISD &= 0xF3FF;}while(0)
%ENC100_COMMENTS%				#define ENC100_GET_AD_IO()			(((volatile BYTE*)&PORTE)[0])
%ENC100_COMMENTS%				#define ENC100_SET_AD_IO(data)		do{WORD _wDataSetMacro = (data); ((volatile BYTE*)&LATE)[0] = ((BYTE*)&_wDataSetMacro)[0]; LATFbits.LATF5 = ((((BYTE*)&_wDataSetMacro)[1]) & 0x1) == 0x1; LATFbits.LATF4 = ((((BYTE*)&_wDataSetMacro)[1]) & 0x2) == 0x2; LATBbits.LATB13 = ((((BYTE*)&_wDataSetMacro)[1]) & 0x4) == 0x4; LATBbits.LATB12 = ((((BYTE*)&_wDataSetMacro)[1]) & 0x8) == 0x8; LATBbits.LATB11 = ((((BYTE*)&_wDataSetMacro)[1]) & 0x10) == 0x10; LATBbits.LATB10 = ((((BYTE*)&_wDataSetMacro)[1]) & 0x20) == 0x20; LATDbits.LATD10 = ((((BYTE*)&_wDataSetMacro)[1]) & 0x40) == 0x40; LATDbits.LATD11 = ((((BYTE*)&_wDataSetMacro)[1]) & 0x80) == 0x80;}while(0)
%ENC100_COMMENTS%				#define ENC100_SET_AD_IOL(data)		(((volatile BYTE*)&LATE)[0] = (BYTE)(data))
%ENC100_COMMENTS%			#endif
%ENC100_COMMENTS%		#elif ENC100_INTERFACE_MODE == 9 || ENC100_INTERFACE_MODE == 10	// Mutliplexed 16-bit address/data modes
%ENC100_COMMENTS%			// All ENC624J600 adddress/data pins AD0-AD15 connected (required for 16-bit data, so there is no differentiation for indirect versus direct addressing mode)
%ENC100_COMMENTS%			// This pinout is awful for doing 16-bit bit-bang operations.  The Fast 100Mbps Ethernet PICtail Plus hardware is wired for PMP hardware support, which requires this pinout.  However, if you are designing a custom board, you can simplify these read/write operations dramatically if you wire things more logically by putting all 16 I/O pins, in order, on PORTB or PORTD.  Such a change would enhance performance.
%ENC100_COMMENTS%			#define ENC100_INIT_PSP_BIT_BANG()	do{((volatile BYTE*)&AD1PCFGH)[1] = 0xFF;}while(0)	// Disable AN24-AN31 analog inputs on RE0-RE7 pins (ENCX24J600 AD0-AD7)
%ENC100_COMMENTS%			#define ENC100_SET_AD_TRIS_IN		do{((volatile BYTE*)&TRISE)[0] = 0xFF; TRISG |= 0x0003; TRISF |= 0x0003; TRISD |= 0x30C0;}while(0)
%ENC100_COMMENTS%			#define ENC100_SET_AD_TRIS_OUT		do{((volatile BYTE*)&TRISE)[0] = 0x00; TRISG &= 0xFFFC; TRISF &= 0xFFFC; TRISD &= 0xCF3F;}while(0)
%ENC100_COMMENTS%			#define ENC100_GET_AD_IO()			(((volatile BYTE*)&PORTE)[0] | (PORTGbits.RG0<<8) | (PORTGbits.RG1<<9) | (PORTFbits.RF1<<10) | (PORTFbits.RF0<<11) | (PORTDbits.RD12<<12) | (PORTDbits.RD13<<13) | (PORTDbits.RD6<<14) | (PORTDbits.RD7<<15)
%ENC100_COMMENTS%			#define ENC100_SET_AD_IO(data)		do{WORD _wDataSetMacro = (data); ((volatile BYTE*)&LATE)[0] = ((BYTE*)&_wDataSetMacro)[0]; LATGbits.LATG0 = ((((BYTE*)&_wDataSetMacro)[1]) & 0x1) == 0x1; LATGbits.LATG1 = ((((BYTE*)&_wDataSetMacro)[1]) & 0x2) == 0x2; LATFbits.LATF1 = ((((BYTE*)&_wDataSetMacro)[1]) & 0x4) == 0x4; LATFbits.LATF0 = ((((BYTE*)&_wDataSetMacro)[1]) & 0x8) == 0x8; LATDbits.LATD12 = ((((BYTE*)&_wDataSetMacro)[1]) & 0x10) == 0x10; LATDbits.LATD13 = ((((BYTE*)&_wDataSetMacro)[1]) & 0x20) == 0x20; LATDbits.LATD6 = ((((BYTE*)&_wDataSetMacro)[1]) & 0x40) == 0x40; LATDbits.LATD7 = ((((BYTE*)&_wDataSetMacro)[1]) & 0x80) == 0x80;}while(0)
%ENC100_COMMENTS%		#endif
%ENC100_COMMENTS%	#endif
%ENC100_COMMENTS%
%ENC100_COMMENTS%	// ENC624J600 SPI SFR register selection (controls which SPI peripheral to 
%ENC100_COMMENTS%	// use on PICs with multiple SPI peripherals).  If a parallel interface is 
%ENC100_COMMENTS%	// used (ENC100_INTERFACE_MODE is >= 1), then the SPI is not used and this 
%ENC100_COMMENTS%	// section can be ignored or deleted.
%ENC100_COMMENTS%	#if defined(__C30__)	// PIC24F, PIC24H, dsPIC30, dsPIC33
%ENC100_COMMENTS%		#define ENC100_ISR_ENABLE		(IEC1bits.INT2IE)
%ENC100_COMMENTS%		#define ENC100_ISR_FLAG			(IFS1bits.INT2IF)	
%ENC100_COMMENTS%		#define ENC100_ISR_POLARITY		(INTCON2bits.INT2EP)
%ENC100_COMMENTS%		#define ENC100_ISR_PRIORITY		(IPC7bits.INT2IP)
%ENC100_COMMENTS%		#define ENC100_SPI_ENABLE		(ENC100_SPISTATbits.SPIEN)
%ENC100_COMMENTS%		#define ENC100_SPI_IF			(IFS0bits.SPI1IF)
%ENC100_COMMENTS%		#define ENC100_SSPBUF			(SPI1BUF)
%ENC100_COMMENTS%		#define ENC100_SPISTAT			(SPI1STAT)
%ENC100_COMMENTS%		#define ENC100_SPISTATbits		(SPI1STATbits)
%ENC100_COMMENTS%		#define ENC100_SPICON1			(SPI1CON1)
%ENC100_COMMENTS%		#define ENC100_SPICON1bits		(SPI1CON1bits)
%ENC100_COMMENTS%		#define ENC100_SPICON2			(SPI1CON2)
%ENC100_COMMENTS%	#else					// PIC32MX
%ENC100_COMMENTS%		#define ENC100_ISR_ENABLE		(IEC0bits.INT2IE)
%ENC100_COMMENTS%		#define ENC100_ISR_FLAG			(IFS0bits.INT2IF)
%ENC100_COMMENTS%		#define ENC100_ISR_POLARITY		(INTCONbits.INT2EP)	
%ENC100_COMMENTS%		#define ENC100_ISR_PRIORITY		(IPC2bits.INT2IP)	
%ENC100_COMMENTS%		#define ENC100_SPI_ENABLE		(ENC100_SPICON1bits.ON)
%ENC100_COMMENTS%		#define ENC100_SPI_IF			(IFS0bits.SPI1RXIF)
%ENC100_COMMENTS%		#define ENC100_SSPBUF			(SPI1BUF)
%ENC100_COMMENTS%		#define ENC100_SPICON1			(SPI1CON)
%ENC100_COMMENTS%		#define ENC100_SPISTATbits		(SPI1STATbits)
%ENC100_COMMENTS%		#define ENC100_SPICON1bits		(SPI1CONbits)
%ENC100_COMMENTS%		#define ENC100_SPIBRG			(SPI1BRG)
%ENC100_COMMENTS%	#endif


%EEPROM_COMMENTS%	// 25LC256 I/O pins
%EEPROM_COMMENTS%	#define EEPROM_CS_TRIS		(TRISDbits.TRISD12)
%EEPROM_COMMENTS%	#define EEPROM_CS_IO		(PORTDbits.RD12)
%EEPROM_COMMENTS%	#define EEPROM_SCK_TRIS		(TRISGbits.TRISG6)
%EEPROM_COMMENTS%	#define EEPROM_SDI_TRIS		(TRISGbits.TRISG7)
%EEPROM_COMMENTS%	#define EEPROM_SDO_TRIS		(TRISGbits.TRISG8)
%EEPROM_COMMENTS%	#if defined(__C30__)	// PIC24F, PIC24H, dsPIC30, dsPIC33
%EEPROM_COMMENTS%		#define EEPROM_SPI_IF		(IFS2bits.SPI2IF)
%EEPROM_COMMENTS%		#define EEPROM_SSPBUF		(SPI2BUF)
%EEPROM_COMMENTS%		#define EEPROM_SPICON1		(SPI2CON1)
%EEPROM_COMMENTS%		#define EEPROM_SPICON1bits	(SPI2CON1bits)
%EEPROM_COMMENTS%		#define EEPROM_SPICON2		(SPI2CON2)
%EEPROM_COMMENTS%		#define EEPROM_SPISTAT		(SPI2STAT)
%EEPROM_COMMENTS%		#define EEPROM_SPISTATbits	(SPI2STATbits)
%EEPROM_COMMENTS%	#else					// PIC32
%EEPROM_COMMENTS%		#define EEPROM_SPI_IF		(IFS1bits.SPI2RXIF)
%EEPROM_COMMENTS%		#define EEPROM_SSPBUF		(SPI2BUF)
%EEPROM_COMMENTS%		#define EEPROM_SPICON1		(SPI2CON)
%EEPROM_COMMENTS%		#define EEPROM_SPICON1bits	(SPI2CONbits)
%EEPROM_COMMENTS%		#define EEPROM_SPIBRG		(SPI2BRG)
%EEPROM_COMMENTS%		#define EEPROM_SPISTAT		(SPI2STAT)
%EEPROM_COMMENTS%		#define EEPROM_SPISTATbits	(SPI2STATbits)
%EEPROM_COMMENTS%	#endif


	// LCD Module I/O pins.  NOTE: On the Explorer 16, the LCD is wired to the 
	// same PMP lines required to communicate with an ENCX24J600 in parallel 
	// mode.  Since the LCD does not have a chip select wire, if you are using 
	// the ENC424J600/624J600 in parallel mode, the LCD cannot be used.
	#if !defined(ENC100_INTERFACE_MODE) || (ENC100_INTERFACE_MODE == 0)	// SPI only
		#define LCD_DATA_TRIS		(*((volatile BYTE*)&TRISE))
		#define LCD_DATA_IO			(*((volatile BYTE*)&LATE))
		#define LCD_RD_WR_TRIS		(TRISDbits.TRISD5)
		#define LCD_RD_WR_IO		(LATDbits.LATD5)
		#define LCD_RS_TRIS			(TRISBbits.TRISB15)
		#define LCD_RS_IO			(LATBbits.LATB15)
		#define LCD_E_TRIS			(TRISDbits.TRISD4)
		#define LCD_E_IO			(LATDbits.LATD4)
	#endif


//	// Serial Flash/SRAM/UART PICtail Plus attached to SPI2 (middle pin group)
//  // This daughter card is not in production, but if you custom attach an SPI 
//  // RAM or SPI Flash chip to your board, then use these definitions as a 
//  // starting point.
//	#define SPIRAM_CS_TRIS			(TRISGbits.TRISG9)
//	#define SPIRAM_CS_IO			(LATGbits.LATG9)
//	#define SPIRAM_SCK_TRIS			(TRISGbits.TRISG6)
//	#define SPIRAM_SDI_TRIS			(TRISGbits.TRISG7)
//	#define SPIRAM_SDO_TRIS			(TRISGbits.TRISG8)
//	#if defined(__C30__)	// PIC24F, PIC24H, dsPIC30, dsPIC33
//		#define SPIRAM_SPI_IF			(IFS2bits.SPI2IF)
//		#define SPIRAM_SSPBUF			(SPI2BUF)
//		#define SPIRAM_SPICON1			(SPI2CON1)
//		#define SPIRAM_SPICON1bits		(SPI2CON1bits)
//		#define SPIRAM_SPICON2			(SPI2CON2)
//		#define SPIRAM_SPISTAT			(SPI2STAT)
//		#define SPIRAM_SPISTATbits		(SPI2STATbits)
//	#else					// PIC32
//		#define SPIRAM_SPI_IF			(IFS1bits.SPI2RXIF)
//		#define SPIRAM_SSPBUF			(SPI2BUF)
//		#define SPIRAM_SPICON1			(SPI2CON)
//		#define SPIRAM_SPICON1bits		(SPI2CONbits)
//		#define SPIRAM_SPIBRG			(SPI2BRG)
//	#endif
//
//	// NOTE: You must also set the SPI_FLASH_SST/SPI_FLASH_SPANSION, 
//	//       SPI_FLASH_SECTOR_SIZE, and SPI_FLASH_PAGE_SIZE macros in 
//	//       SPIFlash.h to match your particular Flash memory chip!!!
//	#define SPIFLASH_CS_TRIS		(TRISBbits.TRISB8)
//	#define SPIFLASH_CS_IO			(LATBbits.LATB8)
//	#define SPIFLASH_SCK_TRIS		(TRISGbits.TRISG6)
//	#define SPIFLASH_SDI_TRIS		(TRISGbits.TRISG7)
//	#define SPIFLASH_SDI_IO			(PORTGbits.RG7)
//	#define SPIFLASH_SDO_TRIS		(TRISGbits.TRISG8)
//	#if defined(__C30__)	// PIC24F, PIC24H, dsPIC30, dsPIC33
//		#define SPIFLASH_SPI_IF			(IFS2bits.SPI2IF)
//		#define SPIFLASH_SSPBUF			(SPI2BUF)
//		#define SPIFLASH_SPICON1		(SPI2CON1)
//		#define SPIFLASH_SPICON1bits	(SPI2CON1bits)
//		#define SPIFLASH_SPICON2		(SPI2CON2)
//		#define SPIFLASH_SPISTAT		(SPI2STAT)
//		#define SPIFLASH_SPISTATbits	(SPI2STATbits)
//	#else					// PIC32
//		#define SPIFLASH_SPI_IF			(IFS1bits.SPI2RXIF)
//		#define SPIFLASH_SSPBUF			(SPI2BUF)
//		#define SPIFLASH_SPICON1		(SPI2CON)
//		#define SPIFLASH_SPICON1bits	(SPI2CONbits)
//		#define SPIFLASH_SPISTATbits	(SPI2STATbits)
//		#define SPIFLASH_SPIBRG			(SPI2BRG)
//	#endif

%ZG2100_COMMENTS%    //----------------------------
%ZG2100_COMMENTS%    // ZeroG ZG2100M WiFi I/O pins
%ZG2100_COMMENTS%    //----------------------------
%ZG2100_COMMENTS%	// If you have a ZeroG ZG2100M WiFi PICtail, you must uncomment one of 
%ZG2100_COMMENTS%	// these two lines to use it.  SPI1 is the top-most slot in the Explorer 16 
%ZG2100_COMMENTS%	// (closer to the LCD and prototyping area) while SPI2 corresponds to 
%ZG2100_COMMENTS%	// insertion of the PICtail into the middle of the side edge connector slot.
%ZG2100_COMMENTS%	%ZG2100_IN_SPI1%#define ZG2100_IN_SPI1
%ZG2100_COMMENTS%	%ZG2100_IN_SPI2%#define ZG2100_IN_SPI2
%ZG2100_COMMENTS%	
%ZG2100_COMMENTS%    #if defined( ZG2100_IN_SPI1 ) && !defined(__32MX460F512L__) && !defined(__32MX795F512L__)
%ZG2100_COMMENTS%        // ZG2100 in SPI1 slot
%ZG2100_COMMENTS%        #define ZG_CS_TRIS			(TRISBbits.TRISB2)
%ZG2100_COMMENTS%    	#define ZG_CS_IO			(LATBbits.LATB2)
%ZG2100_COMMENTS%    	#define ZG_SDI_TRIS			(TRISFbits.TRISF7)
%ZG2100_COMMENTS%    	#define ZG_SCK_TRIS			(TRISFbits.TRISF6)
%ZG2100_COMMENTS%    	#define ZG_SDO_TRIS			(TRISFbits.TRISF8)
%ZG2100_COMMENTS%      	#define ZG_RST_TRIS			(TRISFbits.TRISF0)	
%ZG2100_COMMENTS%    	#define ZG_RST_IO			(LATFbits.LATF0)  
%ZG2100_COMMENTS%        #if defined(__dsPIC33FJ256GP710__) || defined(__PIC24HJ256GP610__)
%ZG2100_COMMENTS%			#define ZG_EINT_TRIS	(TRISAbits.TRISA12)
%ZG2100_COMMENTS%			#define ZG_EINT_IO		(PORTAbits.RA12)
%ZG2100_COMMENTS%        #else
%ZG2100_COMMENTS%			#define ZG_EINT_TRIS	(TRISEbits.TRISE8)  // INT1
%ZG2100_COMMENTS%			#define ZG_EINT_IO		(PORTEbits.RE8)     
%ZG2100_COMMENTS%        #endif
%ZG2100_COMMENTS%    	#define XCEN33_TRIS		    (TRISFbits.TRISF1)  
%ZG2100_COMMENTS%    	#define	XCEN33_IO			(PORTFbits.RF1)
%ZG2100_COMMENTS%    	#if defined( __C30__ )
%ZG2100_COMMENTS%            #define ZG_EINT_EDGE		(INTCON2bits.INT1EP)
%ZG2100_COMMENTS%        	#define ZG_EINT_IE			(IEC1bits.INT1IE)
%ZG2100_COMMENTS%        	#define ZG_EINT_IF			(IFS1bits.INT1IF)
%ZG2100_COMMENTS%    	#elif defined( __PIC32MX__ )
%ZG2100_COMMENTS%            #define ZG_EINT_EDGE		(INTCONbits.INT1EP)
%ZG2100_COMMENTS%        	#define ZG_EINT_IE			(IEC0bits.INT1IE)
%ZG2100_COMMENTS%        	#define ZG_EINT_IF			(IFS0bits.INT1IF)
%ZG2100_COMMENTS%        	#define ZG_EINT_IE_CLEAR    IEC0CLR
%ZG2100_COMMENTS%        	#define ZG_EINT_IF_CLEAR    IFS0CLR
%ZG2100_COMMENTS%        	#define ZG_EINT_IE_SET      IEC0SET
%ZG2100_COMMENTS%        	#define ZG_EINT_IF_SET      IFS0SET
%ZG2100_COMMENTS%        	#define ZG_EINT_BIT         0x00000080
%ZG2100_COMMENTS%        	#define ZG_EINT_IPCSET      IPC1SET
%ZG2100_COMMENTS%        	#define ZG_EINT_IPCCLR      IPC1CLR
%ZG2100_COMMENTS%        	#define ZG_EINT_IPC_MASK    0xFF000000
%ZG2100_COMMENTS%        	#define ZG_EINT_IPC_VALUE   0x0C000000
%ZG2100_COMMENTS%        #else
%ZG2100_COMMENTS%            #error Determine ZG2100 external interrupt
%ZG2100_COMMENTS%        #endif
%ZG2100_COMMENTS%
%ZG2100_COMMENTS%    	#define ZG_SSPBUF			(SPI1BUF)
%ZG2100_COMMENTS%    	#define ZG_SPISTAT			(SPI1STAT)
%ZG2100_COMMENTS%    	#define ZG_SPISTATbits		(SPI1STATbits)
%ZG2100_COMMENTS%    	#if defined( __C30__ )
%ZG2100_COMMENTS%        	#define ZG_SPICON1			(SPI1CON1)
%ZG2100_COMMENTS%        	#define ZG_SPICON1bits		(SPI1CON1bits)
%ZG2100_COMMENTS%        	#define ZG_SPICON2			(SPI1CON2)
%ZG2100_COMMENTS%        	#define ZG_SPI_IE			(IEC0bits.SPI1IE)
%ZG2100_COMMENTS%    //    	#define ZG_SPI_IP			(IPC2bits.SPI1IP)
%ZG2100_COMMENTS%        	#define ZG_SPI_IF			(IFS0bits.SPI1IF)
%ZG2100_COMMENTS%        #elif defined( __PIC32MX__ )
%ZG2100_COMMENTS%        	#define ZG_SPICON1			(SPI1CON)
%ZG2100_COMMENTS%        	#define ZG_SPICON1bits		(SPI1CONbits)
%ZG2100_COMMENTS%            #define ZG_SPI_IE_CLEAR     IEC0CLR
%ZG2100_COMMENTS%            #define ZG_SPI_IF_CLEAR     IFS0CLR
%ZG2100_COMMENTS%            #define ZG_SPI_INT_BITS     0x03800000
%ZG2100_COMMENTS%    		#define ZG_SPI_BRG		    (SPI1BRG)
%ZG2100_COMMENTS%            #define ZG_MAX_SPI_FREQ     (10000000ul)	// Hz
%ZG2100_COMMENTS%        #else
%ZG2100_COMMENTS%            #error Determine ZG2100 SPI information
%ZG2100_COMMENTS%        #endif
%ZG2100_COMMENTS%        
%ZG2100_COMMENTS%    #elif defined( ZG2100_IN_SPI2 ) && !defined(__32MX460F512L__) && !defined(__32MX795F512L__)
%ZG2100_COMMENTS%        // ZG2100 in SPI2 slot
%ZG2100_COMMENTS%        #define ZG_CS_TRIS			(TRISGbits.TRISG9)
%ZG2100_COMMENTS%    	#define ZG_CS_IO			(LATGbits.LATG9)
%ZG2100_COMMENTS%    	#define ZG_SDI_TRIS			(TRISGbits.TRISG7)
%ZG2100_COMMENTS%    	#define ZG_SCK_TRIS			(TRISGbits.TRISG6)
%ZG2100_COMMENTS%    	#define ZG_SDO_TRIS			(TRISGbits.TRISG8)
%ZG2100_COMMENTS%      	#define ZG_RST_TRIS			(TRISGbits.TRISG0)	
%ZG2100_COMMENTS%    	#define ZG_RST_IO			(LATGbits.LATG0)  
%ZG2100_COMMENTS%    	#define ZG_EINT_TRIS		(TRISAbits.TRISA14) // INT3 
%ZG2100_COMMENTS%    	#define ZG_EINT_IO			(PORTAbits.RA14)     
%ZG2100_COMMENTS%    	#define XCEN33_TRIS		    (TRISGbits.TRISG1)  
%ZG2100_COMMENTS%    	#define	XCEN33_IO			(PORTGbits.RG1)
%ZG2100_COMMENTS%    	#if defined( __C30__ )
%ZG2100_COMMENTS%            #define ZG_EINT_EDGE		(INTCON2bits.INT3EP)
%ZG2100_COMMENTS%        	#define ZG_EINT_IE			(IEC3bits.INT3IE)
%ZG2100_COMMENTS%        	#define ZG_EINT_IF			(IFS3bits.INT3IF)
%ZG2100_COMMENTS%    	#elif defined( __PIC32MX__ )
%ZG2100_COMMENTS%            #define ZG_EINT_EDGE		(INTCONbits.INT3EP)
%ZG2100_COMMENTS%        	#define ZG_EINT_IE			(IEC0bits.INT3IE)
%ZG2100_COMMENTS%        	#define ZG_EINT_IF			(IFS0bits.INT3IF)
%ZG2100_COMMENTS%        	#define ZG_EINT_IE_CLEAR    IEC0CLR
%ZG2100_COMMENTS%        	#define ZG_EINT_IF_CLEAR    IFS0CLR
%ZG2100_COMMENTS%        	#define ZG_EINT_IE_SET      IEC0SET
%ZG2100_COMMENTS%        	#define ZG_EINT_IF_SET      IFS0SET
%ZG2100_COMMENTS%        	#define ZG_EINT_BIT         0x00008000
%ZG2100_COMMENTS%        	#define ZG_EINT_IPCSET      IPC3SET
%ZG2100_COMMENTS%        	#define ZG_EINT_IPCCLR      IPC3CLR
%ZG2100_COMMENTS%        	#define ZG_EINT_IPC_MASK    0xFF000000
%ZG2100_COMMENTS%        	#define ZG_EINT_IPC_VALUE   0x0C000000
%ZG2100_COMMENTS%        #else
%ZG2100_COMMENTS%            #error Determine ZG2100 external interrupt
%ZG2100_COMMENTS%        #endif
%ZG2100_COMMENTS%        
%ZG2100_COMMENTS%    	#define ZG_SSPBUF			(SPI2BUF)
%ZG2100_COMMENTS%    	#define ZG_SPISTAT			(SPI2STAT)
%ZG2100_COMMENTS%    	#define ZG_SPISTATbits		(SPI2STATbits)
%ZG2100_COMMENTS%    	#if defined( __C30__ )
%ZG2100_COMMENTS%        	#define ZG_SPICON1			(SPI2CON1)
%ZG2100_COMMENTS%        	#define ZG_SPICON1bits		(SPI2CON1bits)
%ZG2100_COMMENTS%        	#define ZG_SPICON2			(SPI2CON2)
%ZG2100_COMMENTS%        	#define ZG_SPI_IE			(IEC2bits.SPI2IE)
%ZG2100_COMMENTS%    //    	#define ZG_SPI_IP			(IPC8bits.SPI2IP)
%ZG2100_COMMENTS%        	#define ZG_SPI_IF			(IFS2bits.SPI2IF)
%ZG2100_COMMENTS%        #elif defined( __PIC32MX__ )
%ZG2100_COMMENTS%        	#define ZG_SPICON1			(SPI2CON)
%ZG2100_COMMENTS%        	#define ZG_SPICON1bits		(SPI2CONbits)
%ZG2100_COMMENTS%            #define ZG_SPI_IE_CLEAR     IEC1CLR
%ZG2100_COMMENTS%            #define ZG_SPI_IF_CLEAR     IFS1CLR
%ZG2100_COMMENTS%            #define ZG_SPI_INT_BITS     0x000000e0
%ZG2100_COMMENTS%    		#define ZG_SPI_BRG		    (SPI2BRG)
%ZG2100_COMMENTS%            #define ZG_MAX_SPI_FREQ     (10000000ul)	// Hz
%ZG2100_COMMENTS%        #else
%ZG2100_COMMENTS%            #error Determine ZG2100 SPI information
%ZG2100_COMMENTS%        #endif
%ZG2100_COMMENTS%
%ZG2100_COMMENTS%	#elif defined( ZG2100_IN_SPI1 ) && (defined(__32MX460F512L__) || defined(__32MX795F512L__))
%ZG2100_COMMENTS%        // ZG2100 in SPI1 slot
%ZG2100_COMMENTS%        #define ZG_CS_TRIS			(TRISDbits.TRISD9)
%ZG2100_COMMENTS%    	#define ZG_CS_IO			(LATDbits.LATD9)
%ZG2100_COMMENTS%    	#define ZG_SDI_TRIS			(TRISCbits.TRISC4)
%ZG2100_COMMENTS%    	#define ZG_SCK_TRIS			(TRISDbits.TRISD10)
%ZG2100_COMMENTS%    	#define ZG_SDO_TRIS			(TRISDbits.TRISD0)
%ZG2100_COMMENTS%      	#define ZG_RST_TRIS			(TRISFbits.TRISF0)	
%ZG2100_COMMENTS%    	#define ZG_RST_IO			(LATFbits.LATF0)  
%ZG2100_COMMENTS%    	#define ZG_EINT_TRIS		(TRISEbits.TRISE8)  // INT1  
%ZG2100_COMMENTS%    	#define ZG_EINT_IO			(PORTEbits.RE8)     
%ZG2100_COMMENTS%    	#define XCEN33_TRIS		    (TRISFbits.TRISF1)  
%ZG2100_COMMENTS%    	#define	XCEN33_IO			(PORTFbits.RF1)
%ZG2100_COMMENTS%    	#if defined( __C30__ )
%ZG2100_COMMENTS%            #define ZG_EINT_EDGE		(INTCON2bits.INT1EP)
%ZG2100_COMMENTS%        	#define ZG_EINT_IE			(IEC1bits.INT1IE)
%ZG2100_COMMENTS%        	#define ZG_EINT_IF			(IFS1bits.INT1IF)
%ZG2100_COMMENTS%    	#elif defined( __PIC32MX__ )
%ZG2100_COMMENTS%            #define ZG_EINT_EDGE		(INTCONbits.INT1EP)
%ZG2100_COMMENTS%        	#define ZG_EINT_IE			(IEC0bits.INT1IE)
%ZG2100_COMMENTS%        	#define ZG_EINT_IF			(IFS0bits.INT1IF)
%ZG2100_COMMENTS%        	#define ZG_EINT_IE_CLEAR    IEC0CLR
%ZG2100_COMMENTS%        	#define ZG_EINT_IF_CLEAR    IFS0CLR
%ZG2100_COMMENTS%        	#define ZG_EINT_IE_SET      IEC0SET
%ZG2100_COMMENTS%        	#define ZG_EINT_IF_SET      IFS0SET
%ZG2100_COMMENTS%        	#define ZG_EINT_BIT         0x00000080
%ZG2100_COMMENTS%        	#define ZG_EINT_IPCSET      IPC1SET
%ZG2100_COMMENTS%        	#define ZG_EINT_IPCCLR      IPC1CLR
%ZG2100_COMMENTS%        	#define ZG_EINT_IPC_MASK    0xFF000000
%ZG2100_COMMENTS%        	#define ZG_EINT_IPC_VALUE   0x0C000000
%ZG2100_COMMENTS%        #else
%ZG2100_COMMENTS%            #error Determine ZG2100 external interrupt
%ZG2100_COMMENTS%        #endif
%ZG2100_COMMENTS%
%ZG2100_COMMENTS%    	#define ZG_SSPBUF			(SPI1BUF)
%ZG2100_COMMENTS%    	#define ZG_SPISTAT			(SPI1STAT)
%ZG2100_COMMENTS%    	#define ZG_SPISTATbits		(SPI1STATbits)
%ZG2100_COMMENTS%    	#if defined( __C30__ )
%ZG2100_COMMENTS%        	#define ZG_SPICON1			(SPI1CON1)
%ZG2100_COMMENTS%        	#define ZG_SPICON1bits		(SPI1CON1bits)
%ZG2100_COMMENTS%        	#define ZG_SPICON2			(SPI1CON2)
%ZG2100_COMMENTS%        	#define ZG_SPI_IE			(IEC0bits.SPI1IE)
%ZG2100_COMMENTS%    //    	#define ZG_SPI_IP			(IPC2bits.SPI1IP)
%ZG2100_COMMENTS%        	#define ZG_SPI_IF			(IFS0bits.SPI1IF)
%ZG2100_COMMENTS%        #elif defined( __PIC32MX__ )
%ZG2100_COMMENTS%        	#define ZG_SPICON1			(SPI1CON)
%ZG2100_COMMENTS%        	#define ZG_SPICON1bits		(SPI1CONbits)
%ZG2100_COMMENTS%            #define ZG_SPI_IE_CLEAR     IEC0CLR
%ZG2100_COMMENTS%            #define ZG_SPI_IF_CLEAR     IFS0CLR
%ZG2100_COMMENTS%            #define ZG_SPI_INT_BITS     0x03800000
%ZG2100_COMMENTS%    		#define ZG_SPI_BRG		    (SPI1BRG)
%ZG2100_COMMENTS%            #define ZG_MAX_SPI_FREQ     (10000000ul)	// Hz
%ZG2100_COMMENTS%        #else
%ZG2100_COMMENTS%            #error Determine ZG2100 SPI information
%ZG2100_COMMENTS%        #endif
%ZG2100_COMMENTS%    #elif defined( ZG2100_IN_SPI2 ) && (defined(__32MX460F512L__) || defined(__32MX795F512L__))
%ZG2100_COMMENTS%    	#error "/RST and /CE are on RG2 and RG3 which are multiplexed with USB D+ and D-."
%ZG2100_COMMENTS%    #endif

#elif defined(DSPICDEM11)
// dsPICDEM 1.1 Development Board + Ethernet PICtail airwired. There 
// is no PICtail header on this development board.  The following 
// airwires must be made:
// 1. dsPICDEM GND <-> PICtail GND (PICtail pin 27)
// 2. dsPICDEM Vdd <- PICtail VPIC (PICtail pin 25)
// 3. dsPICDEM RG2 -> PICtail ENC28J60 CS (PICtail pin 22)
// 4. dsPICDEM RF6 -> PICtail SCK (PICtail pin 11)
// 5. dsPICDEM RF7 <- PICtail SDI (PICtail pin 9)
// 6. dsPICDEM RF8 -> PICtail SDO (PICtail pin 7)
// 7. dsPICDEM RG3 -> PICtail 25LC256 CS (PICtail pin 20)

	#define LED0_TRIS			(TRISDbits.TRISD3)	// Ref LED4
	#define LED0_IO				(PORTDbits.RD3)	
	#define LED1_TRIS			(TRISDbits.TRISD2)	// Ref LED3
	#define LED1_IO				(PORTDbits.RD2)
	#define LED2_TRIS			(TRISDbits.TRISD1)	// Ref LED2
	#define LED2_IO				(PORTDbits.RD1)
	#define LED3_TRIS			(TRISDbits.TRISD0)	// Ref LED1
	#define LED3_IO				(PORTDbits.RD0)
	#define LED4_TRIS			(TRISDbits.TRISD3)	// No LED, Remapped to Ref LED4
	#define LED4_IO				(PORTDbits.RD3)	
	#define LED5_TRIS			(TRISDbits.TRISD2)	// No LED, Remapped to Ref LED3
	#define LED5_IO				(PORTDbits.RD2)
	#define LED6_TRIS			(TRISDbits.TRISD1)	// No LED, Remapped to Ref LED2
	#define LED6_IO				(PORTDbits.RD1)
	#define LED7_TRIS			(TRISDbits.TRISD0)	// No LED, Remapped to Ref LED1
	#define LED7_IO				(PORTDbits.RD0)
	#define LED_GET()			((LED3_IO<<3) | (LED2_IO<<2) | (LED1_IO<<1) | LED0_IO)
	#define LED_PUT(a)			do{BYTE vTemp = (a); LED0_IO = vTemp&0x1; LED1_IO = vTemp&0x2; LED2_IO = vTemp&0x4; LED3_IO = vTemp&0x8;} while(0)

	#define BUTTON0_TRIS		(TRISAbits.TRISA15)	// Ref SW4
	#define	BUTTON0_IO			(PORTAbits.RA15)
	#define BUTTON1_TRIS		(TRISAbits.TRISA14)	// Ref SW3
	#define	BUTTON1_IO			(PORTAbits.RA14)
	#define BUTTON2_TRIS		(TRISAbits.TRISA13)	// Ref SW2
	#define	BUTTON2_IO			(PORTAbits.RA13)
	#define BUTTON3_TRIS		(TRISAbits.TRISA12)	// Ref SW1
	#define	BUTTON3_IO			(PORTAbits.RA12)

	#define UARTTX_TRIS			(TRISFbits.TRISF3)
	#define UARTTX_IO			(PORTFbits.RF3)
	#define UARTRX_TRIS			(TRISFbits.TRISF2)
	#define UARTRX_IO			(PORTFbits.RF2)

%ENC28J60_COMMENTS%	// ENC28J60 I/O pins
%ENC28J60_COMMENTS%	#define ENC_CS_TRIS			(TRISGbits.TRISG2)		// User must airwire this
%ENC28J60_COMMENTS%	#define ENC_CS_IO			(PORTGbits.RG2)
%ENC28J60_COMMENTS%	// SPI SCK, SDI, SDO pins are automatically controlled by the 
%ENC28J60_COMMENTS%	// PIC24/dsPIC/PIC32 SPI module 
%ENC28J60_COMMENTS%	#define ENC_SPI_IF			(IFS0bits.SPI1IF)
%ENC28J60_COMMENTS%	#define ENC_SSPBUF			(SPI1BUF)
%ENC28J60_COMMENTS%	#define ENC_SPICON1			(SPI1CON)
%ENC28J60_COMMENTS%	#define ENC_SPICON1bits		(SPI1CONbits)
%ENC28J60_COMMENTS%	#define ENC_SPICON2			(SPI1BUF)				// SPI1CON2 doesn't exist, remap to unimportant register
%ENC28J60_COMMENTS%	#define ENC_SPISTAT			(SPI1STAT)
%ENC28J60_COMMENTS%	#define ENC_SPISTATbits		(SPI1STATbits)

%EEPROM_COMMENTS%	// 25LC256 I/O pins
%EEPROM_COMMENTS%	#define EEPROM_CS_TRIS		(TRISGbits.TRISG3)	// User must airwire this
%EEPROM_COMMENTS%	#define EEPROM_CS_IO		(PORTGbits.RG3)
%EEPROM_COMMENTS%	#define EEPROM_SCK_TRIS		(TRISGbits.TRISG6)
%EEPROM_COMMENTS%	#define EEPROM_SDI_TRIS		(TRISGbits.TRISG7)
%EEPROM_COMMENTS%	#define EEPROM_SDO_TRIS		(TRISGbits.TRISG8)
%EEPROM_COMMENTS%	#define EEPROM_SPI_IF		(IFS0bits.SPI1IF)
%EEPROM_COMMENTS%	#define EEPROM_SSPBUF		(SPI1BUF)
%EEPROM_COMMENTS%	#define EEPROM_SPICON1		(SPI1CON)
%EEPROM_COMMENTS%	#define EEPROM_SPICON1bits	(SPI1CONbits)
%EEPROM_COMMENTS%	#define EEPROM_SPICON2		(SPI1BUF)			// SPI1CON2 doesn't exist, remap to unimportant register
%EEPROM_COMMENTS%	#define EEPROM_SPISTAT		(SPI1STAT)
%EEPROM_COMMENTS%	#define EEPROM_SPISTATbits	(SPI1STATbits)

	// SI3000 codec pins
	#define CODEC_RST_TRIS		(TRISFbits.TRISF6)
	#define CODEC_RST_IO		(PORTFbits.RF6)
	
	// PIC18F252 LCD Controller
	#define LCDCTRL_CS_TRIS		(TRISGbits.TRISG9)
	#define LCDCTRL_CS_IO		(PORTGbits.RG9)

#elif defined(PICDEMNET2) && !defined(HI_TECH_C)
// PICDEM.net 2 (PIC18F97J60 + ENC28J60 or ENC624J600)

	// I/O pins
	#define LED0_TRIS			(TRISJbits.TRISJ0)
	#define LED0_IO				(LATJbits.LATJ0)
	#define LED1_TRIS			(TRISJbits.TRISJ1)
	#define LED1_IO				(LATJbits.LATJ1)
	#define LED2_TRIS			(TRISJbits.TRISJ2)
	#define LED2_IO				(LATJbits.LATJ2)
	#define LED3_TRIS			(TRISJbits.TRISJ3)
	#define LED3_IO				(LATJbits.LATJ3)
	#define LED4_TRIS			(TRISJbits.TRISJ4)
	#define LED4_IO				(LATJbits.LATJ4)
	#define LED5_TRIS			(TRISJbits.TRISJ5)
	#define LED5_IO				(LATJbits.LATJ5)
	#define LED6_TRIS			(TRISJbits.TRISJ6)
	#define LED6_IO				(LATJbits.LATJ6)
	#define LED7_TRIS			(TRISJbits.TRISJ7)
	#define LED7_IO				(LATJbits.LATJ7)
	#define LED_GET()			(LATJ)
	#define LED_PUT(a)			(LATJ = (a))

	#define BUTTON0_TRIS		(TRISBbits.TRISB3)
	#define	BUTTON0_IO			(PORTBbits.RB3)
	#define BUTTON1_TRIS		(TRISBbits.TRISB2)
	#define	BUTTON1_IO			(PORTBbits.RB2)
	#define BUTTON2_TRIS		(TRISBbits.TRISB1)
	#define	BUTTON2_IO			(PORTBbits.RB1)
	#define BUTTON3_TRIS		(TRISBbits.TRISB0)
	#define	BUTTON3_IO			(PORTBbits.RB0)

%ENC100_COMMENTS%	// ENC424J600/624J600 Fast 100Mbps Ethernet PICtail Plus defines
%ENC100_COMMENTS%	#define ENC100_INTERFACE_MODE			0	// Uncomment this to use the ENC424J600/624J600 Ethernet controller
%ENC100_COMMENTS%
%ENC100_COMMENTS%	// ENC424J600/624J600 Fast 100Mbps Ethernet PICtail Plus I/O pins
%ENC100_COMMENTS%	#define ENC100_MDIX_TRIS				(TRISBbits.TRISB4)
%ENC100_COMMENTS%	#define ENC100_MDIX_IO					(LATBbits.LATB4)
%ENC100_COMMENTS%	#define ENC100_POR_TRIS					(TRISBbits.TRISB5)
%ENC100_COMMENTS%	#define ENC100_POR_IO					(LATBbits.LATB5)
%ENC100_COMMENTS%	#define ENC100_INT_TRIS					(TRISBbits.TRISB2)
%ENC100_COMMENTS%	#define ENC100_INT_IO					(PORTBbits.RB2)
%ENC100_COMMENTS%
%ENC100_COMMENTS%	// ENC424J600/624J600 SPI pinout
%ENC100_COMMENTS%	#define ENC100_CS_TRIS					(TRISBbits.TRISB3)
%ENC100_COMMENTS%	#define ENC100_CS_IO					(LATBbits.LATB3)
%ENC100_COMMENTS%	#define ENC100_SO_WR_B0SEL_EN_TRIS		(TRISCbits.TRISC4)	// NOTE: SO is ENC624J600 Serial Out, which needs to connect to the PIC SDI pin for SPI mode
%ENC100_COMMENTS%	#define ENC100_SO_WR_B0SEL_EN_IO		(PORTCbits.RC4)
%ENC100_COMMENTS%	#define ENC100_SI_RD_RW_TRIS			(TRISCbits.TRISC5)	// NOTE: SI is ENC624J600 Serial In, which needs to connect to the PIC SDO pin for SPI mode
%ENC100_COMMENTS%	#define ENC100_SI_RD_RW_IO				(LATCbits.LATC5)
%ENC100_COMMENTS%	#define ENC100_SCK_AL_TRIS				(TRISCbits.TRISC3)
%ENC100_COMMENTS%	#define ENC100_SCK_AL_IO				(PORTCbits.RC3)		// NOTE: This must be the PORT, not the LATch like it is for the PSP interface.
%ENC100_COMMENTS%
%ENC100_COMMENTS%	// ENC424J600/624J600 SPI SFR register selection (controls which SPI 
%ENC100_COMMENTS%	// peripheral to use on PICs with multiple SPI peripherals).
%ENC100_COMMENTS%//	#define ENC100_ISR_ENABLE		(INTCON3bits.INT2IE)
%ENC100_COMMENTS%//	#define ENC100_ISR_FLAG			(INTCON3bits.INT2IF)
%ENC100_COMMENTS%//	#define ENC100_ISR_POLARITY		(INTCON2bits.INTEDG2)
%ENC100_COMMENTS%//	#define ENC100_ISR_PRIORITY		(INTCON3bits.INT2IP)
%ENC100_COMMENTS%	#define ENC100_SPI_ENABLE		(ENC100_SPISTATbits.SPIEN)
%ENC100_COMMENTS%	#define ENC100_SPI_IF			(PIR1bits.SSP1IF)
%ENC100_COMMENTS%	#define ENC100_SSPBUF			(SSP1BUF)
%ENC100_COMMENTS%	#define ENC100_SPISTAT			(SSP1STAT)
%ENC100_COMMENTS%	#define ENC100_SPISTATbits		(SSP1STATbits)
%ENC100_COMMENTS%	#define ENC100_SPICON1			(SSP1CON1)
%ENC100_COMMENTS%	#define ENC100_SPICON1bits		(SSP1CON1bits)
%ENC100_COMMENTS%	#define ENC100_SPICON2			(SSP1CON2)


%ENC28J60_COMMENTS%	// ENC28J60 I/O pins
%ENC28J60_COMMENTS%	#define ENC_RST_TRIS		(TRISDbits.TRISD2)	// Not connected by default
%ENC28J60_COMMENTS%	#define ENC_RST_IO			(LATDbits.LATD2)
%ENC28J60_COMMENTS%	%EXTRA_ENC28J60_COMMENT_FOR_PICDEMNET2%#define ENC_CS_TRIS			(TRISDbits.TRISD3)	// Uncomment this line if you wish to use the ENC28J60 on the PICDEM.net 2 board instead of the internal PIC18F97J60 Ethernet module
%ENC28J60_COMMENTS%	#define ENC_CS_IO			(LATDbits.LATD3)
%ENC28J60_COMMENTS%	#define ENC_SCK_TRIS		(TRISCbits.TRISC3)
%ENC28J60_COMMENTS%	#define ENC_SDI_TRIS		(TRISCbits.TRISC4)
%ENC28J60_COMMENTS%	#define ENC_SDO_TRIS		(TRISCbits.TRISC5)
%ENC28J60_COMMENTS%	#define ENC_SPI_IF			(PIR1bits.SSP1IF)
%ENC28J60_COMMENTS%	#define ENC_SSPBUF			(SSP1BUF)
%ENC28J60_COMMENTS%	#define ENC_SPISTAT			(SSP1STAT)
%ENC28J60_COMMENTS%	#define ENC_SPISTATbits		(SSP1STATbits)
%ENC28J60_COMMENTS%	#define ENC_SPICON1			(SSP1CON1)
%ENC28J60_COMMENTS%	#define ENC_SPICON1bits		(SSP1CON1bits)
%ENC28J60_COMMENTS%	#define ENC_SPICON2			(SSP1CON2)

%ZG2100_COMMENTS%    //----------------
%ZG2100_COMMENTS%    // ZG2100 I/O pins
%ZG2100_COMMENTS%    //----------------
%ZG2100_COMMENTS%	#define ZG_CS_TRIS			(TRISCbits.TRISC2)	// Uncomment this line if you wish to use the ZG2100 on the PICDEM.net 2 board instead of the internal PIC18F97J60 Ethernet module
%ZG2100_COMMENTS%	#define ZG_SDI_TRIS			(TRISCbits.TRISC4)
%ZG2100_COMMENTS%	#define ZG_SCK_TRIS			(TRISCbits.TRISC3)
%ZG2100_COMMENTS%	#define ZG_SDO_TRIS			(TRISCbits.TRISC5)
%ZG2100_COMMENTS%	#define ZG_RST_TRIS			(TRISBbits.TRISB1)	
%ZG2100_COMMENTS%	#define ZG_RST_IO			(LATBbits.LATB1)  
%ZG2100_COMMENTS%	#define ZG_EINT_TRIS		(TRISBbits.TRISB0)  
%ZG2100_COMMENTS%	#define ZG_EINT_IO			(PORTBbits.RB0)     
%ZG2100_COMMENTS%	#define ZG_CS_IO			(LATCbits.LATC2)
%ZG2100_COMMENTS%	#define XCEN33_TRIS		    (TRISBbits.TRISB2)  
%ZG2100_COMMENTS%	#define	XCEN33_IO			(PORTBbits.RB2)
%ZG2100_COMMENTS%	#define ZG_EINT_EDGE		(INTCON2bits.INTEDG0)
%ZG2100_COMMENTS%	#define ZG_EINT_IE			(INTCONbits.INT0IE)
%ZG2100_COMMENTS%	#define ZG_EINT_IF			(INTCONbits.INT0IF)
%ZG2100_COMMENTS%	#define ZG_SPI_IF			(PIR1bits.SSPIF)
%ZG2100_COMMENTS%	#define ZG_SSPBUF			(SSP1BUF)
%ZG2100_COMMENTS%	#define ZG_SPISTAT			(SSP1STAT)
%ZG2100_COMMENTS%	#define ZG_SPISTATbits		(SSP1STATbits)
%ZG2100_COMMENTS%	#define ZG_SPICON1			(SSP1CON1)
%ZG2100_COMMENTS%	#define ZG_SPICON1bits		(SSP1CON1bits)
%ZG2100_COMMENTS%	#define ZG_SPICON2			(SSP1CON2)
%ZG2100_COMMENTS%	#define ZG_SPI_IE			(PIE1bits.SSPIE)
%ZG2100_COMMENTS%	#define ZG_SPI_IP			(IPR1bits.SSPIP)

%EEPROM_COMMENTS%	// 25LC256 I/O pins
%EEPROM_COMMENTS%	#define EEPROM_CS_TRIS		(TRISDbits.TRISD7)
%EEPROM_COMMENTS%	#define EEPROM_CS_IO		(LATDbits.LATD7)
%EEPROM_COMMENTS%	#define EEPROM_SCK_TRIS		(TRISCbits.TRISC3)
%EEPROM_COMMENTS%	#define EEPROM_SDI_TRIS		(TRISCbits.TRISC4)
%EEPROM_COMMENTS%	#define EEPROM_SDO_TRIS		(TRISCbits.TRISC5)
%EEPROM_COMMENTS%	#define EEPROM_SPI_IF		(PIR1bits.SSP1IF)
%EEPROM_COMMENTS%	#define EEPROM_SSPBUF		(SSP1BUF)
%EEPROM_COMMENTS%	#define EEPROM_SPICON1		(SSP1CON1)
%EEPROM_COMMENTS%	#define EEPROM_SPICON1bits	(SSP1CON1bits)
%EEPROM_COMMENTS%	#define EEPROM_SPICON2		(SSP1CON2)
%EEPROM_COMMENTS%	#define EEPROM_SPISTAT		(SSP1STAT)
%EEPROM_COMMENTS%	#define EEPROM_SPISTATbits	(SSP1STATbits)

	// LCD I/O pins
	#define LCD_DATA_TRIS		(TRISE)
	#define LCD_DATA_IO			(LATE)
	#define LCD_RD_WR_TRIS		(TRISHbits.TRISH1)
	#define LCD_RD_WR_IO		(LATHbits.LATH1)
	#define LCD_RS_TRIS			(TRISHbits.TRISH2)
	#define LCD_RS_IO			(LATHbits.LATH2)
	#define LCD_E_TRIS			(TRISHbits.TRISH0)
	#define LCD_E_IO			(LATHbits.LATH0)

	// Serial Flash/SRAM/UART PICtail
//	#define SPIRAM_CS_TRIS			(TRISBbits.TRISB5)
//	#define SPIRAM_CS_IO			(LATBbits.LATB5)
//	#define SPIRAM_SCK_TRIS			(TRISCbits.TRISC3)
//	#define SPIRAM_SDI_TRIS			(TRISCbits.TRISC4)
//	#define SPIRAM_SDO_TRIS			(TRISCbits.TRISC5)
//	#define SPIRAM_SPI_IF			(PIR1bits.SSPIF)
//	#define SPIRAM_SSPBUF			(SSP1BUF)
//	#define SPIRAM_SPICON1			(SSP1CON1)
//	#define SPIRAM_SPICON1bits		(SSP1CON1bits)
//	#define SPIRAM_SPICON2			(SSP1CON2)
//	#define SPIRAM_SPISTAT			(SSP1STAT)
//	#define SPIRAM_SPISTATbits		(SSP1STATbits)
//
//	// NOTE: You must also set the SPI_FLASH_SST/SPI_FLASH_SPANSION, 
//	//       SPI_FLASH_SECTOR_SIZE, and SPI_FLASH_PAGE_SIZE macros in 
//	//       SPIFlash.h to match your particular Flash memory chip!!!
//	#define SPIFLASH_CS_TRIS		(TRISBbits.TRISB4)
//	#define SPIFLASH_CS_IO			(LATBbits.LATB4)
//	#define SPIFLASH_SCK_TRIS		(TRISCbits.TRISC3)
//	#define SPIFLASH_SDI_TRIS		(TRISCbits.TRISC4)
//	#define SPIFLASH_SDI_IO			(PORTCbits.RC4)
//	#define SPIFLASH_SDO_TRIS		(TRISCbits.TRISC5)
//	#define SPIFLASH_SPI_IF			(PIR1bits.SSPIF)
//	#define SPIFLASH_SSPBUF			(SSP1BUF)
//	#define SPIFLASH_SPICON1		(SSP1CON1)
//	#define SPIFLASH_SPICON1bits	(SSP1CON1bits)
//	#define SPIFLASH_SPICON2		(SSP1CON2)
//	#define SPIFLASH_SPISTAT		(SSP1STAT)
//	#define SPIFLASH_SPISTATbits	(SSP1STATbits)

#elif defined(PICDEMNET2) && defined(HI_TECH_C)
// PICDEM.net 2 (PIC18F97J60 + ENC28J60) + HI-TECH PICC-18 compiler
// Optionally may use Fast 100Mbps Ethernet PICtail Plus (ENC624J600)
	// I/O pins
	#define LED0_TRIS			(TRISJ0)
	#define LED0_IO				(LATJ0)
	#define LED1_TRIS			(TRISJ1)
	#define LED1_IO				(LATJ1)
	#define LED2_TRIS			(TRISJ2)
	#define LED2_IO				(LATJ2)
	#define LED3_TRIS			(TRISJ3)
	#define LED3_IO				(LATJ3)
	#define LED4_TRIS			(TRISJ4)
	#define LED4_IO				(LATJ4)
	#define LED5_TRIS			(TRISJ5)
	#define LED5_IO				(LATJ5)
	#define LED6_TRIS			(TRISJ6)
	#define LED6_IO				(LATJ6)
	#define LED7_TRIS			(TRISJ7)
	#define LED7_IO				(LATJ7)
	#define LED_GET()			(LATJ)
	#define LED_PUT(a)			(LATJ = (a))

	#define BUTTON0_TRIS		(TRISB3)
	#define	BUTTON0_IO			(RB3)
	#define BUTTON1_TRIS		(TRISB2)
	#define	BUTTON1_IO			(RB2)
	#define BUTTON2_TRIS		(TRISB1)
	#define	BUTTON2_IO			(RB1)
	#define BUTTON3_TRIS		(TRISB0)
	#define	BUTTON3_IO			(RB0)

%ENC28J60_COMMENTS%	// ENC28J60 I/O pins
%ENC28J60_COMMENTS%	#define ENC_RST_TRIS		(TRISD2)	// Not connected by default
%ENC28J60_COMMENTS%	#define ENC_RST_IO			(LATD2)
%ENC28J60_COMMENTS%	%EXTRA_ENC28J60_COMMENT_FOR_PICDEMNET2%#define ENC_CS_TRIS			(TRISD3)	// Uncomment this line if you wish to use the ENC28J60 on the PICDEM.net 2 board instead of the internal PIC18F97J60 Ethernet module
%ENC28J60_COMMENTS%	#define ENC_CS_IO			(LATD3)
%ENC28J60_COMMENTS%	#define ENC_SCK_TRIS		(TRISC3)
%ENC28J60_COMMENTS%	#define ENC_SDI_TRIS		(TRISC4)
%ENC28J60_COMMENTS%	#define ENC_SDO_TRIS		(TRISC5)
%ENC28J60_COMMENTS%	#define ENC_SPI_IF			(SSPIF)
%ENC28J60_COMMENTS%	#define ENC_SSPBUF			(SSP1BUF)
%ENC28J60_COMMENTS%	#define ENC_SPISTAT			(SSP1STAT)
%ENC28J60_COMMENTS%	#define ENC_SPISTATbits		(SSP1STATbits)
%ENC28J60_COMMENTS%	#define ENC_SPICON1			(SSP1CON1)
%ENC28J60_COMMENTS%	#define ENC_SPICON1bits		(SSP1CON1bits)
%ENC28J60_COMMENTS%	#define ENC_SPICON2			(SSP1CON2)

%ENC100_COMMENTS%	// ENC424J600/624J600 Fast 100Mbps Ethernet PICtail Plus defines
%ENC100_COMMENTS%	#define ENC100_INTERFACE_MODE			0	// Uncomment this to use the ENC424J600/624J600 Ethernet controller
%ENC100_COMMENTS%
%ENC100_COMMENTS%	// ENC424J600/624J600 Fast 100Mbps Ethernet PICtail Plus I/O pins
%ENC100_COMMENTS%	#define ENC100_MDIX_TRIS				(TRISB4)
%ENC100_COMMENTS%	#define ENC100_MDIX_IO					(LATB4)
%ENC100_COMMENTS%	#define ENC100_POR_TRIS					(TRISB5)
%ENC100_COMMENTS%	#define ENC100_POR_IO					(LATB5)
%ENC100_COMMENTS%	#define ENC100_INT_TRIS					(TRISB2)
%ENC100_COMMENTS%	#define ENC100_INT_IO					(RB2)
%ENC100_COMMENTS%
%ENC100_COMMENTS%	// ENC424J600/624J600 SPI pinout
%ENC100_COMMENTS%	#define ENC100_CS_TRIS					(TRISB3)
%ENC100_COMMENTS%	#define ENC100_CS_IO					(LATB3)
%ENC100_COMMENTS%	#define ENC100_SO_WR_B0SEL_EN_TRIS		(TRISC4)	// NOTE: SO is ENC624J600 Serial Out, which needs to connect to the PIC SDI pin for SPI mode
%ENC100_COMMENTS%	#define ENC100_SO_WR_B0SEL_EN_IO		(RC4)
%ENC100_COMMENTS%	#define ENC100_SI_RD_RW_TRIS			(TRISC5)	// NOTE: SI is ENC624J600 Serial In, which needs to connect to the PIC SDO pin for SPI mode
%ENC100_COMMENTS%	#define ENC100_SI_RD_RW_IO				(LATC5)
%ENC100_COMMENTS%	#define ENC100_SCK_AL_TRIS				(TRISC3)
%ENC100_COMMENTS%	#define ENC100_SCK_AL_IO				(RC3)		// NOTE: This must be the PORT, not the LATch like it is for the PSP interface.
%ENC100_COMMENTS%
%ENC100_COMMENTS%	// ENC424J600/624J600 SPI SFR register selection (controls which SPI 
%ENC100_COMMENTS%	// peripheral to use on PICs with multiple SPI peripherals).
%ENC100_COMMENTS%//	#define ENC100_ISR_ENABLE		(INT2IE)
%ENC100_COMMENTS%//	#define ENC100_ISR_FLAG			(INT2IF)
%ENC100_COMMENTS%//	#define ENC100_ISR_POLARITY		(INTEDG2)
%ENC100_COMMENTS%//	#define ENC100_ISR_PRIORITY		(INT2IP)
%ENC100_COMMENTS%	#define ENC100_SPI_ENABLE		(ENC100_SPISTATbits.SPIEN)
%ENC100_COMMENTS%	#define ENC100_SPI_IF			(PIR1bits.SSP1IF)
%ENC100_COMMENTS%	#define ENC100_SSPBUF			(SSP1BUF)
%ENC100_COMMENTS%	#define ENC100_SPISTAT			(SSP1STAT)
%ENC100_COMMENTS%	#define ENC100_SPISTATbits		(SSP1STATbits)
%ENC100_COMMENTS%	#define ENC100_SPICON1			(SSP1CON1)
%ENC100_COMMENTS%	#define ENC100_SPICON1bits		(SSP1CON1bits)
%ENC100_COMMENTS%	#define ENC100_SPICON2			(SSP1CON2)

%EEPROM_COMMENTS%	// 25LC256 I/O pins
%EEPROM_COMMENTS%	#define EEPROM_CS_TRIS		(TRISD7)
%EEPROM_COMMENTS%	#define EEPROM_CS_IO		(LATD7)
%EEPROM_COMMENTS%	#define EEPROM_SCK_TRIS		(TRISC3)
%EEPROM_COMMENTS%	#define EEPROM_SDI_TRIS		(TRISC4)
%EEPROM_COMMENTS%	#define EEPROM_SDO_TRIS		(TRISC5)
%EEPROM_COMMENTS%	#define EEPROM_SPI_IF		(SSP1IF)
%EEPROM_COMMENTS%	#define EEPROM_SSPBUF		(SSP1BUF)
%EEPROM_COMMENTS%	#define EEPROM_SPICON1		(SSP1CON1)
%EEPROM_COMMENTS%	#define EEPROM_SPICON1bits	(SSP1CON1bits)
%EEPROM_COMMENTS%	#define EEPROM_SPICON2		(SSP1CON2)
%EEPROM_COMMENTS%	#define EEPROM_SPISTAT		(SSP1STAT)
%EEPROM_COMMENTS%	#define EEPROM_SPISTATbits	(SSP1STATbits)

	// LCD I/O pins
	#define LCD_DATA_TRIS		(TRISE)
	#define LCD_DATA_IO			(LATE)
	#define LCD_RD_WR_TRIS		(TRISH1)
	#define LCD_RD_WR_IO		(LATH1)
	#define LCD_RS_TRIS			(TRISH2)
	#define LCD_RS_IO			(LATH2)
	#define LCD_E_TRIS			(TRISH0)
	#define LCD_E_IO			(LATH0)

	// Serial Flash/SRAM/UART PICtail
//	#define SPIRAM_CS_TRIS			(TRISB5)
//	#define SPIRAM_CS_IO			(LATB5)
//	#define SPIRAM_SCK_TRIS			(TRISC3)
//	#define SPIRAM_SDI_TRIS			(TRISC4)
//	#define SPIRAM_SDO_TRIS			(TRISC5)
//	#define SPIRAM_SPI_IF			(SSPIF)
//	#define SPIRAM_SSPBUF			(SSP1BUF)
//	#define SPIRAM_SPICON1			(SSP1CON1)
//	#define SPIRAM_SPICON1bits		(SSP1CON1bits)
//	#define SPIRAM_SPICON2			(SSP1CON2)
//	#define SPIRAM_SPISTAT			(SSP1STAT)
//	#define SPIRAM_SPISTATbits		(SSP1STATbits)
//
//	// NOTE: You must also set the SPI_FLASH_SST/SPI_FLASH_SPANSION, 
//	//       SPI_FLASH_SECTOR_SIZE, and SPI_FLASH_PAGE_SIZE macros in 
//	//       SPIFlash.h to match your particular Flash memory chip!!!
//	#define SPIFLASH_CS_TRIS		(TRISB4)
//	#define SPIFLASH_CS_IO			(LATB4)
//	#define SPIFLASH_SCK_TRIS		(TRISC3)
//	#define SPIFLASH_SDI_TRIS		(TRISC4)
//	#define SPIFLASH_SDI_IO			(RC4)
//	#define SPIFLASH_SDO_TRIS		(TRISC5)
//	#define SPIFLASH_SPI_IF			(SSPIF)
//	#define SPIFLASH_SSPBUF			(SSP1BUF)
//	#define SPIFLASH_SPICON1		(SSP1CON1)
//	#define SPIFLASH_SPICON1bits	(SSP1CON1bits)
//	#define SPIFLASH_SPICON2		(SSP1CON2)
//	#define SPIFLASH_SPISTAT		(SSP1STAT)
//	#define SPIFLASH_SPISTATbits	(SSP1STATbits)

	// Register name fixup for certain processors
	#define SPBRGH				SPBRGH1
	#define TXSTAbits			TXSTA1bits
	#define RCSTAbits			RCSTA1bits
	#define BAUDCONbits			BAUDCON1bits
	#define GO					GODONE

#elif defined(INTERNET_RADIO) // Internet Radio board
// Internet Radio board (05-60142)
	// I/O pins
	#define LED0_TRIS			(TRISCbits.TRISC2)
	#define LED0_IO				(LATCbits.LATC2)
	#define LED1_TRIS			(PRODL)				// No LED1 on this board
	#define LED1_IO				(PRODL)
	#define LED2_TRIS			(PRODL)				// No LED2 on this board
	#define LED2_IO				(PRODL)
	#define LED3_TRIS			(PRODL)				// No LED3 on this board
	#define LED3_IO				(PRODL)
	#define LED4_TRIS			(PRODL)				// No LED4 on this board
	#define LED4_IO				(PRODL)
	#define LED5_TRIS			(PRODL)				// No LED5 on this board
	#define LED5_IO				(PRODL)
	#define LED6_TRIS			(PRODL)				// No LED6 on this board
	#define LED6_IO				(PRODL)
	#define LED7_TRIS			(PRODL)				// No LED7 on this board
	#define LED7_IO				(PRODL)
	#define LED_GET()			(LED0_IO)
	#define LED_PUT(a)			(LED0_IO = (a))

	#define BUTTON0_TRIS		(TRISBbits.TRISB5)
	#define	BUTTON0_IO			(PORTBbits.RB5)
	#define BUTTON1_TRIS		(TRISFbits.TRISF1)
	#define	BUTTON1_IO			(PORTFbits.RF1)
	#define BUTTON2_TRIS		(TRISBbits.TRISB4)
	#define	BUTTON2_IO			(PORTBbits.RB4)
	#define BUTTON3_TRIS		(PRODL)				// No BUTTON3 on this board
	#define	BUTTON3_IO			(PRODL)

	// Serial SRAM
	#define SPIRAM_CS_TRIS			(TRISEbits.TRISE4)
	#define SPIRAM_CS_IO			(LATEbits.LATE4)
	#define SPIRAM_SCK_TRIS			(TRISCbits.TRISC3)
	#define SPIRAM_SDI_TRIS			(TRISCbits.TRISC4)
	#define SPIRAM_SDO_TRIS			(TRISCbits.TRISC5)
	#define SPIRAM_SPI_IF			(PIR1bits.SSPIF)
	#define SPIRAM_SSPBUF			(SSP1BUF)
	#define SPIRAM_SPICON1			(SSP1CON1)
	#define SPIRAM_SPICON1bits		(SSP1CON1bits)
	#define SPIRAM_SPICON2			(SSP1CON2)
	#define SPIRAM_SPISTAT			(SSP1STAT)
	#define SPIRAM_SPISTATbits		(SSP1STATbits)
	#define SPIRAM2_CS_TRIS			(TRISEbits.TRISE5)
	#define SPIRAM2_CS_IO			(LATEbits.LATE5)
	#define SPIRAM2_SCK_TRIS		(TRISCbits.TRISC3)
	#define SPIRAM2_SDI_TRIS		(TRISCbits.TRISC4)
	#define SPIRAM2_SDO_TRIS		(TRISCbits.TRISC5)
	#define SPIRAM2_SPI_IF			(PIR1bits.SSPIF)
	#define SPIRAM2_SSPBUF			(SSP1BUF)
	#define SPIRAM2_SPICON1			(SSP1CON1)
	#define SPIRAM2_SPICON1bits		(SSP1CON1bits)
	#define SPIRAM2_SPICON2			(SSP1CON2)
	#define SPIRAM2_SPISTAT			(SSP1STAT)
	#define SPIRAM2_SPISTATbits		(SSP1STATbits)
	
    // VLSI VS1011/VS1053 audio encoder/decoder and DAC
	#define MP3_DREQ_TRIS			(TRISBbits.TRISB0)	// Data Request
	#define MP3_DREQ_IO 			(PORTBbits.RB0)		
	#define MP3_XRESET_TRIS			(TRISDbits.TRISD0)	// Reset, active low
	#define MP3_XRESET_IO			(LATDbits.LATD0)
	#define MP3_XDCS_TRIS			(TRISBbits.TRISB1)	// Data Chip Select
	#define MP3_XDCS_IO				(LATBbits.LATB1)
	#define MP3_XCS_TRIS			(TRISBbits.TRISB2)	// Control Chip Select
	#define MP3_XCS_IO				(LATBbits.LATB2)
	#define MP3_SCK_TRIS			(TRISCbits.TRISC3)
	#define MP3_SDI_TRIS			(TRISCbits.TRISC4)
	#define MP3_SDO_TRIS			(TRISCbits.TRISC5)
	#define MP3_SPI_IF				(PIR1bits.SSP1IF)
	#define MP3_SSPBUF				(SSP1BUF)
	#define MP3_SPICON1				(SSP1CON1)
	#define MP3_SPICON1bits			(SSP1CON1bits)
	#define MP3_SPICON2				(SSP1CON2)
	#define MP3_SPISTAT				(SSP1STAT)
	#define MP3_SPISTATbits			(SSP1STATbits)
	
    // OLED Display
    #define oledWR                  (PORTAbits.RA3)
    #define oledWR_TRIS             (TRISAbits.TRISA3)
    #define oledRD                  (PORTAbits.RA4)
    #define oledRD_TRIS             (TRISAbits.TRISA4)
    #define oledCS                  (PORTAbits.RA5)
    #define oledCS_TRIS             (TRISAbits.TRISA5)
    #define oledRESET               (PORTDbits.RD1)
    #define oledRESET_TRIS          (TRISDbits.TRISD1)
    #define oledD_C                 (PORTGbits.RG4)
    #define oledD_C_TRIS            (TRISGbits.TRISG4)


#elif defined(PIC32_STARTER_KIT)
// PIC32 (General Purpose) Starter Kit (02-02002) with PIC32MX360F512L processor, 
// PIC32 USB Starter Board (02-02030) with PIC32MX460F512L processor, or
// PIC32 USB Starter Kit II (02-02148) with PIC32MX795F512L processor +
// PIC32 I/O Expansion Board (05-02029) +
// Ethernet PICtail Plus, Fast 100Mbps Ethernet PICtail Plus, or 
// ZeroG ZG2100M 802.11 WiFi PICtail Plus

	// Specify which SPI to use for the ENC28J60 or ENC624J600.  SPI1 is 
	// the topmost slot with pin 1 on it.  SPI2 is the middle slot 
	// starting on pin 33.
	#define ENC_IN_SPI1
	//#define ENC_IN_SPI2

	// Note that SPI1 cannot be used when using the PIC32 USB Starter 
	// Board or PIC32 USB Starter Kit II due to the USB peripheral pins 
	// mapping on top of the ordinary SPI1 pinout.  
	#if defined(ENC_IN_SPI1) && (defined(__32MX460F512L__) || defined(__32MX795F512L__))
		#undef ENC_IN_SPI1
		#define ENC_IN_SPI2
	#endif

    
    // Hardware mappings
    #define LED0_TRIS			(TRISDbits.TRISD0)	// Ref LED1
	#define LED0_IO				(LATDbits.LATD0)
	#define LED1_TRIS			(TRISDbits.TRISD1)	// Ref LED2
	#define LED1_IO				(LATDbits.LATD1)	
	#define LED2_TRIS			(TRISDbits.TRISD2)	// Ref LED3
	#define LED2_IO				(LATDbits.LATD2)
	#define LED3_TRIS			(LED2_TRIS)			// No such LED
	#define LED3_IO				(LATDbits.LATD6)
	#define LED4_TRIS			(LED2_TRIS)			// No such LED
	#define LED4_IO				(LATDbits.LATD6)
	#define LED5_TRIS			(LED2_TRIS)			// No such LED
	#define LED5_IO				(LATDbits.LATD6)
	#define LED6_TRIS			(LED2_TRIS)			// No such LED
	#define LED6_IO				(LATDbits.LATD6)
	#define LED7_TRIS			(LED2_TRIS)			// No such LED
	#define LED7_IO				(LATDbits.LATD6)
    
	#define LED_GET()			((BYTE)LATD & 0x07)
	#define LED_PUT(a)			do{LATD = (LATD & 0xFFF8) | ((a)&0x07);}while(0)

    #define BUTTON0_TRIS		(TRISDbits.TRISD6)	// Ref SW1
	#define	BUTTON0_IO			(PORTDbits.RD6)
	#define BUTTON1_TRIS		(TRISDbits.TRISD7)	// Ref SW2
	#define	BUTTON1_IO			(PORTDbits.RD7)
	#define BUTTON2_TRIS		(TRISDbits.TRISD13)	// Ref SW3
	#define	BUTTON2_IO			(PORTDbits.RD13)
	#define BUTTON3_TRIS		(TRISDbits.TRISD13)	// No BUTTON3 on this board
	#define	BUTTON3_IO			(1)
    
    // UART configuration (not too important since we don't have a UART 
    // connector attached normally, but needed to compile if the STACK_USE_UART 
    // or STACK_USE_UART2TCP_BRIDGE features are enabled.
    #define UARTTX_TRIS			(TRISFbits.TRISF3)
    #define UARTRX_TRIS			(TRISFbits.TRISF2)
    
    
%ENC28J60_COMMENTS%	// ENC28J60 I/O pins
%ENC28J60_COMMENTS%	#if defined ENC_IN_SPI1
%ENC28J60_COMMENTS%		#define ENC_CS_TRIS			(TRISDbits.TRISD14)	// Comment this line out if you are using the ENC424J600/624J600, ZeroG ZG2100, or other network controller.
%ENC28J60_COMMENTS%		#define ENC_CS_IO			(PORTDbits.RD14)
%ENC28J60_COMMENTS%		//#define ENC_RST_TRIS		(TRISDbits.TRISD15)	// Not connected by default.  It is okay to leave this pin completely unconnected, in which case this macro should simply be left undefined.
%ENC28J60_COMMENTS%		//#define ENC_RST_IO		(PORTDbits.RD15)
%ENC28J60_COMMENTS%
%ENC28J60_COMMENTS%		// SPI SCK, SDI, SDO pins are automatically controlled by the 
%ENC28J60_COMMENTS%		#define ENC_SPI_IF			(IFS0bits.SPI1RXIF)
%ENC28J60_COMMENTS%		#define ENC_SSPBUF			(SPI1BUF)
%ENC28J60_COMMENTS%		#define ENC_SPICON1			(SPI1CON)
%ENC28J60_COMMENTS%		#define ENC_SPICON1bits		(SPI1CONbits)
%ENC28J60_COMMENTS%		#define ENC_SPIBRG			(SPI1BRG)
%ENC28J60_COMMENTS%		#define ENC_SPISTATbits		(SPI1STATbits)
%ENC28J60_COMMENTS%	#elif defined ENC_IN_SPI2
%ENC28J60_COMMENTS%		#define ENC_CS_TRIS			(TRISFbits.TRISF12) // Comment this line out if you are using the ENC424J600/624J600, ZeroG ZG2100, or other network controller.
%ENC28J60_COMMENTS%		#define ENC_CS_IO			(PORTFbits.RF12)
%ENC28J60_COMMENTS%		//#define ENC_RST_TRIS		(TRISFbits.TRISF13)	// Not connected by default
%ENC28J60_COMMENTS%		//#define ENC_RST_IO		(PORTFbits.RF13)
%ENC28J60_COMMENTS%
%ENC28J60_COMMENTS%		// SPI SCK, SDI, SDO pins are automatically controlled by the 
%ENC28J60_COMMENTS%		// PIC32 SPI module 
%ENC28J60_COMMENTS%		#define ENC_SPI_IF			(IFS1bits.SPI2RXIF)
%ENC28J60_COMMENTS%		#define ENC_SSPBUF			(SPI2BUF)
%ENC28J60_COMMENTS%		#define ENC_SPICON1			(SPI2CON)
%ENC28J60_COMMENTS%		#define ENC_SPISTATbits		(SPI2STATbits)
%ENC28J60_COMMENTS%		#define ENC_SPICON1bits		(SPI2CONbits)
%ENC28J60_COMMENTS%		#define ENC_SPIBRG			(SPI2BRG)
%ENC28J60_COMMENTS%	#endif
    


%ENC100_COMMENTS%	// ENC624J600 Interface Configuration
%ENC100_COMMENTS%	// Comment out ENC100_INTERFACE_MODE if you don't have an ENC624J600 or 
%ENC100_COMMENTS%	// ENC424J600.  Otherwise, choose the correct setting for the interface you 
%ENC100_COMMENTS%	// are using.  Legal values are:
%ENC100_COMMENTS%	//  - Commented out: No ENC424J600/624J600 present or used.  All other 
%ENC100_COMMENTS%	//                   ENC100_* macros are ignored.
%ENC100_COMMENTS%	//	- 0: SPI mode using CS, SCK, SI, and SO pins
%ENC100_COMMENTS%	//  - 1: 8-bit demultiplexed PSP Mode 1 with RD and WR pins
%ENC100_COMMENTS%	//  - 2: 8-bit demultiplexed PSP Mode 2 with R/Wbar and EN pins
%ENC100_COMMENTS%	//  - 3: 16-bit demultiplexed PSP Mode 3 with RD, WRL, and WRH pins
%ENC100_COMMENTS%	//  - 4: 16-bit demultiplexed PSP Mode 4 with R/Wbar, B0SEL, and B1SEL pins
%ENC100_COMMENTS%	//  - 5: 8-bit multiplexed PSP Mode 5 with RD and WR pins
%ENC100_COMMENTS%	//  - 6: 8-bit multiplexed PSP Mode 6 with R/Wbar and EN pins
%ENC100_COMMENTS%	//  - 9: 16-bit multiplexed PSP Mode 9 with AL, RD, WRL, and WRH pins
%ENC100_COMMENTS%	//  - 10: 16-bit multiplexed PSP Mode 10 with AL, R/Wbar, B0SEL, and B1SEL 
%ENC100_COMMENTS%	//        pins
%ENC100_COMMENTS%	#define ENC100_INTERFACE_MODE			%ENC100_INTERFACE_MODE%
%ENC100_COMMENTS%
%ENC100_COMMENTS%	// If using a parallel interface, direct RAM addressing can be used (if all 
%ENC100_COMMENTS%	// addresses wires are connected), or a reduced number of pins can be used 
%ENC100_COMMENTS%	// for indirect addressing.  If using an SPI interface or PSP Mode 9 or 10 
%ENC100_COMMENTS%	// (multiplexed 16-bit modes), which require all address lines to always be 
%ENC100_COMMENTS%	// connected, then this option is ignored. Comment out or uncomment this 
%ENC100_COMMENTS%	// macro to match your hardware connections.
%ENC100_COMMENTS%	#define ENC100_PSP_USE_INDIRECT_RAM_ADDRESSING
%ENC100_COMMENTS%
%ENC100_COMMENTS%	// ENC424J600/624J600 parallel indirect address remapping macro function.
%ENC100_COMMENTS%	// This section translates SFR and RAM addresses presented to the 
%ENC100_COMMENTS%	// ReadMemory() and WriteMemory() APIs in ENCX24J600.c to the actual 
%ENC100_COMMENTS%	// addresses that must be presented on the parallel interface.  This macro 
%ENC100_COMMENTS%	// must be modified to match your hardware if you are using an indirect PSP 
%ENC100_COMMENTS%	// addressing mode (ENC100_PSP_USE_INDIRECT_RAM_ADDRESSING is defined) and 
%ENC100_COMMENTS%	// have some of your address lines tied off to Vdd.  If you are using the 
%ENC100_COMMENTS%	// SPI interface, then this section can be ignored or deleted.
%ENC100_COMMENTS%	#if (ENC100_INTERFACE_MODE == 1) || (ENC100_INTERFACE_MODE == 2) || (ENC100_INTERFACE_MODE == 5) || (ENC100_INTERFACE_MODE == 6) // 8-bit PSP
%ENC100_COMMENTS%		#define ENC100_TRANSLATE_TO_PIN_ADDR(a)		((((a)&0x0100)<<6) | ((a)&0x00FF))
%ENC100_COMMENTS%	#elif (ENC100_INTERFACE_MODE == 3) || (ENC100_INTERFACE_MODE == 4) // 16-bit PSP
%ENC100_COMMENTS%		#define ENC100_TRANSLATE_TO_PIN_ADDR(a)		(a)
%ENC100_COMMENTS%	#endif
%ENC100_COMMENTS%
%ENC100_COMMENTS%	// Auto-crossover pins on Fast 100Mbps Ethernet PICtail/PICtail Plus.  If 
%ENC100_COMMENTS%	// your circuit doesn't have such a feature, delete these two defines.
%ENC100_COMMENTS%	#define ENC100_MDIX_TRIS				(TRISBbits.TRISB3)
%ENC100_COMMENTS%	#define ENC100_MDIX_IO					(LATBbits.LATB3)
%ENC100_COMMENTS%
%ENC100_COMMENTS%	// ENC624J600 I/O control and status pins
%ENC100_COMMENTS%	// If a pin is not required for your selected ENC100_INTERFACE_MODE 
%ENC100_COMMENTS%	// interface selection (ex: WRH/B1SEL for PSP modes 1, 2, 5, and 6), then 
%ENC100_COMMENTS%	// you can ignore, delete, or put anything for the pin definition.  Also, 
%ENC100_COMMENTS%	// the INT and POR pins are entirely optional.  If not connected, comment 
%ENC100_COMMENTS%	// them out.
%ENC100_COMMENTS%	#define ENC100_INT_TRIS					(TRISEbits.TRISE9)		// INT signal is optional and currently unused in the Microchip TCP/IP Stack.  Leave this pin disconnected and comment out this pin definition if you don't want it.
%ENC100_COMMENTS%	#define ENC100_INT_IO					(PORTEbits.RE9)
%ENC100_COMMENTS%	#if (ENC100_INTERFACE_MODE >= 1)	// Parallel mode
%ENC100_COMMENTS%		// PSP control signal pinout
%ENC100_COMMENTS%		#define ENC100_CS_TRIS					(TRISAbits.TRISA5)	// CS is optional in PSP mode.  If you are not sharing the parallel bus with another device, tie CS to Vdd and comment out this pin definition.
%ENC100_COMMENTS%		#define ENC100_CS_IO					(LATAbits.LATA5)
%ENC100_COMMENTS%		#define ENC100_POR_TRIS					(TRISCbits.TRISC1)	// POR signal is optional.  If your application doesn't have a power disconnect feature, comment out this pin definition.
%ENC100_COMMENTS%		#define ENC100_POR_IO					(LATCbits.LATC1)
%ENC100_COMMENTS%		#define ENC100_SO_WR_B0SEL_EN_TRIS		(TRISDbits.TRISD4)
%ENC100_COMMENTS%		#define ENC100_SO_WR_B0SEL_EN_IO		(LATDbits.LATD4)
%ENC100_COMMENTS%		#define ENC100_SI_RD_RW_TRIS			(TRISDbits.TRISD5)
%ENC100_COMMENTS%		#define ENC100_SI_RD_RW_IO				(LATDbits.LATD5)
%ENC100_COMMENTS%		#define ENC100_SCK_AL_TRIS				(TRISBbits.TRISB15)
%ENC100_COMMENTS%		#define ENC100_SCK_AL_IO				(LATBbits.LATB15)
%ENC100_COMMENTS%	#else	
%ENC100_COMMENTS%		// SPI pinout
%ENC100_COMMENTS%    	#if defined ENC_IN_SPI1
%ENC100_COMMENTS%        	#define ENC100_CS_TRIS					(TRISDbits.TRISD14)	// CS is mandatory when using the SPI interface
%ENC100_COMMENTS%        	#define ENC100_CS_IO					(LATDbits.LATD14)
%ENC100_COMMENTS%        	#define ENC100_POR_TRIS					(TRISDbits.TRISD15)	// POR signal is optional.  If your application doesn't have a power disconnect feature, comment out this pin definition.
%ENC100_COMMENTS%        	#define ENC100_POR_IO					(LATDbits.LATD15)
%ENC100_COMMENTS%        	#define ENC100_SO_WR_B0SEL_EN_TRIS		(TRISFbits.TRISF7)	// SO is ENCX24J600 Serial Out, which needs to connect to the PIC SDI pin for SPI mode
%ENC100_COMMENTS%        	#define ENC100_SO_WR_B0SEL_EN_IO		(PORTFbits.RF7)
%ENC100_COMMENTS%        	#define ENC100_SI_RD_RW_TRIS			(TRISFbits.TRISF8)	// SI is ENCX24J600 Serial In, which needs to connect to the PIC SDO pin for SPI mode
%ENC100_COMMENTS%        	#define ENC100_SI_RD_RW_IO				(LATFbits.LATF8)
%ENC100_COMMENTS%        	#define ENC100_SCK_AL_TRIS				(TRISFbits.TRISF6)
%ENC100_COMMENTS%        #elif defined ENC_IN_SPI2
%ENC100_COMMENTS%            #define ENC100_CS_TRIS					(TRISFbits.TRISF12)
%ENC100_COMMENTS%    		#define ENC100_CS_IO					(LATFbits.LATF12)
%ENC100_COMMENTS%    		#define ENC100_POR_TRIS					(TRISFbits.TRISF13)
%ENC100_COMMENTS%    		#define ENC100_POR_IO					(LATFbits.LATF13)
%ENC100_COMMENTS%    		#define ENC100_SO_WR_B0SEL_EN_TRIS		(TRISGbits.TRISG7)	// NOTE: SO is ENC624J600 Serial Out, which needs to connect to the PIC SDI pin for SPI mode
%ENC100_COMMENTS%    		#define ENC100_SO_WR_B0SEL_EN_IO		(PORTGbits.RG7)
%ENC100_COMMENTS%    		#define ENC100_SI_RD_RW_TRIS			(TRISGbits.TRISG8)	// NOTE: SI is ENC624J600 Serial In, which needs to connect to the PIC SDO pin for SPI mode
%ENC100_COMMENTS%    		#define ENC100_SI_RD_RW_IO				(LATGbits.LATG8)
%ENC100_COMMENTS%    		#define ENC100_SCK_AL_TRIS				(TRISGbits.TRISG6)
%ENC100_COMMENTS%    		#define ENC100_SCK_AL_IO				(PORTGbits.RG6)		// NOTE: This must be the PORT, not the LATch like it is for the PSP interface.
%ENC100_COMMENTS%        #endif
%ENC100_COMMENTS%    #endif
%ENC100_COMMENTS%
%ENC100_COMMENTS%
%ENC100_COMMENTS%	// ENC624J600 SPI SFR register selection (controls which SPI peripheral to 
%ENC100_COMMENTS%	// use on PICs with multiple SPI peripherals).  If a parallel interface is 
%ENC100_COMMENTS%	// used (ENC100_INTERFACE_MODE is >= 1), then the SPI is not used and this 
%ENC100_COMMENTS%	// section can be ignored or deleted.
%ENC100_COMMENTS%	#if defined ENC_IN_SPI1
%ENC100_COMMENTS%		#define ENC100_ISR_ENABLE		(IEC0bits.INT2IE)
%ENC100_COMMENTS%		#define ENC100_ISR_FLAG			(IFS0bits.INT2IF)
%ENC100_COMMENTS%		#define ENC100_ISR_POLARITY		(INTCONbits.INT2EP)	
%ENC100_COMMENTS%		#define ENC100_ISR_PRIORITY		(IPC2bits.INT2IP)	
%ENC100_COMMENTS%		#define ENC100_SPI_ENABLE		(ENC100_SPICON1bits.ON)
%ENC100_COMMENTS%		#define ENC100_SPI_IF			(IFS0bits.SPI1RXIF)
%ENC100_COMMENTS%		#define ENC100_SSPBUF			(SPI1BUF)
%ENC100_COMMENTS%		#define ENC100_SPICON1			(SPI1CON)
%ENC100_COMMENTS%		#define ENC100_SPISTATbits		(SPI1STATbits)
%ENC100_COMMENTS%		#define ENC100_SPICON1bits		(SPI1CONbits)
%ENC100_COMMENTS%		#define ENC100_SPIBRG			(SPI1BRG)
%ENC100_COMMENTS%	#elif defined ENC_IN_SPI2
%ENC100_COMMENTS%		#define ENC100_ISR_ENABLE		(IEC0bits.INT4IE)
%ENC100_COMMENTS%		#define ENC100_ISR_FLAG			(IFS0bits.INT4IF)
%ENC100_COMMENTS%		#define ENC100_ISR_POLARITY		(INTCONbits.INT4EP)	
%ENC100_COMMENTS%		#define ENC100_ISR_PRIORITY		(IPC2bits.INT4IP)	
%ENC100_COMMENTS%		#define ENC100_SPI_ENABLE		(ENC100_SPICON1bits.ON)
%ENC100_COMMENTS%		#define ENC100_SPI_IF			(IFS1bits.SPI2RXIF)
%ENC100_COMMENTS%		#define ENC100_SSPBUF			(SPI2BUF)
%ENC100_COMMENTS%		#define ENC100_SPICON1			(SPI2CON)
%ENC100_COMMENTS%		#define ENC100_SPISTATbits		(SPI2STATbits)
%ENC100_COMMENTS%		#define ENC100_SPICON1bits		(SPI2CONbits)
%ENC100_COMMENTS%		#define ENC100_SPIBRG			(SPI2BRG)
%ENC100_COMMENTS%	#endif
 

%ZG2100_COMMENTS%	//----------------------------
%ZG2100_COMMENTS%	// ZeroG ZG2100M WiFi I/O pins
%ZG2100_COMMENTS%	//----------------------------
%ZG2100_COMMENTS%	// If you have a ZeroG ZG2100M WiFi PICtail, you must uncomment one of 
%ZG2100_COMMENTS%	// these two lines to use it.  SPI1 is the top-most slot while SPI2 corresponds to 
%ZG2100_COMMENTS%	// insertion of the PICtail into the middle of the side edge connector slot.
%ZG2100_COMMENTS%
%ZG2100_COMMENTS%	%ZG2100_IN_SPI1%#define ZG2100_IN_SPI1
%ZG2100_COMMENTS%	%ZG2100_IN_SPI2%#define ZG2100_IN_SPI2
%ZG2100_COMMENTS%	#if defined( ZG2100_IN_SPI1 )
%ZG2100_COMMENTS%		// ZG2100 in SPI1 slot
%ZG2100_COMMENTS%		#define ZG_CS_TRIS			(TRISBbits.TRISB2)
%ZG2100_COMMENTS%    	#define ZG_CS_IO			(LATBbits.LATB2)
%ZG2100_COMMENTS%    	#define ZG_SDI_TRIS			(TRISFbits.TRISF7)
%ZG2100_COMMENTS%    	#define ZG_SCK_TRIS			(TRISFbits.TRISF6)
%ZG2100_COMMENTS%    	#define ZG_SDO_TRIS			(TRISFbits.TRISF8)
%ZG2100_COMMENTS%      	#define ZG_RST_TRIS			(TRISFbits.TRISF0)	
%ZG2100_COMMENTS%    	#define ZG_RST_IO			(LATFbits.LATF0)  
%ZG2100_COMMENTS%    	#define ZG_EINT_TRIS		(TRISEbits.TRISE8)  // INT1  
%ZG2100_COMMENTS%    	#define ZG_EINT_IO			(PORTEbits.RE8)     
%ZG2100_COMMENTS%    	#define XCEN33_TRIS		    (TRISFbits.TRISF1)  
%ZG2100_COMMENTS%    	#define	XCEN33_IO			(PORTFbits.RF1)
%ZG2100_COMMENTS%    	#if defined( __C30__ )
%ZG2100_COMMENTS%           #define ZG_EINT_EDGE		(INTCON2bits.INT1EP)
%ZG2100_COMMENTS%        	#define ZG_EINT_IE			(IEC1bits.INT1IE)
%ZG2100_COMMENTS%        	#define ZG_EINT_IF			(IFS1bits.INT1IF)
%ZG2100_COMMENTS%    	#elif defined( __PIC32MX__ )
%ZG2100_COMMENTS%           #define ZG_EINT_EDGE		(INTCONbits.INT1EP)
%ZG2100_COMMENTS%        	#define ZG_EINT_IE			(IEC0bits.INT1IE)
%ZG2100_COMMENTS%        	#define ZG_EINT_IF			(IFS0bits.INT1IF)
%ZG2100_COMMENTS%        	#define ZG_EINT_IE_CLEAR    IEC0CLR
%ZG2100_COMMENTS%        	#define ZG_EINT_IF_CLEAR    IFS0CLR
%ZG2100_COMMENTS%        	#define ZG_EINT_IE_SET      IEC0SET
%ZG2100_COMMENTS%        	#define ZG_EINT_IF_SET      IFS0SET
%ZG2100_COMMENTS%        	#define ZG_EINT_BIT         0x00000080
%ZG2100_COMMENTS%        	#define ZG_EINT_IPCSET      IPC1SET
%ZG2100_COMMENTS%        	#define ZG_EINT_IPCCLR      IPC1CLR
%ZG2100_COMMENTS%        	#define ZG_EINT_IPC_MASK    0xFF000000
%ZG2100_COMMENTS%        	#define ZG_EINT_IPC_VALUE   0x0C000000
%ZG2100_COMMENTS%       #else
%ZG2100_COMMENTS%           #error Determine ZG2100 external interrupt
%ZG2100_COMMENTS%       #endif
%ZG2100_COMMENTS%
%ZG2100_COMMENTS%    	#define ZG_SSPBUF			(SPI1BUF)
%ZG2100_COMMENTS%    	#define ZG_SPISTAT			(SPI1STAT)
%ZG2100_COMMENTS%    	#define ZG_SPISTATbits		(SPI1STATbits)
%ZG2100_COMMENTS%    	#if defined( __C30__ )
%ZG2100_COMMENTS%        	#define ZG_SPICON1			(SPI1CON1)
%ZG2100_COMMENTS%        	#define ZG_SPICON1bits		(SPI1CON1bits)
%ZG2100_COMMENTS%        	#define ZG_SPICON2			(SPI1CON2)
%ZG2100_COMMENTS%        	#define ZG_SPI_IE			(IEC0bits.SPI1IE)
%ZG2100_COMMENTS%    //    	#define ZG_SPI_IP			(IPC2bits.SPI1IP)
%ZG2100_COMMENTS%        	#define ZG_SPI_IF			(IFS0bits.SPI1IF)
%ZG2100_COMMENTS%       #elif defined( __PIC32MX__ )
%ZG2100_COMMENTS%        	#define ZG_SPICON1			(SPI1CON)
%ZG2100_COMMENTS%        	#define ZG_SPICON1bits		(SPI1CONbits)
%ZG2100_COMMENTS%           #define ZG_SPI_IE_CLEAR     IEC0CLR
%ZG2100_COMMENTS%           #define ZG_SPI_IF_CLEAR     IFS0CLR
%ZG2100_COMMENTS%           #define ZG_SPI_INT_BITS     0x03800000
%ZG2100_COMMENTS%    		#define ZG_SPI_BRG		    (SPI1BRG)
%ZG2100_COMMENTS%           #define ZG_MAX_SPI_FREQ     (10000000ul)	// Hz
%ZG2100_COMMENTS%       #else
%ZG2100_COMMENTS%           #error Determine ZG2100 SPI information
%ZG2100_COMMENTS%		#endif
%ZG2100_COMMENTS%        
%ZG2100_COMMENTS%	#elif defined( ZG2100_IN_SPI2 )
%ZG2100_COMMENTS%       // ZG2100 in SPI2 slot
%ZG2100_COMMENTS%       #define ZG_CS_TRIS			(TRISGbits.TRISG9)
%ZG2100_COMMENTS%    	#define ZG_CS_IO			(LATGbits.LATG9)
%ZG2100_COMMENTS%    	#define ZG_SDI_TRIS			(TRISGbits.TRISG7)
%ZG2100_COMMENTS%    	#define ZG_SCK_TRIS			(TRISGbits.TRISG6)
%ZG2100_COMMENTS%    	#define ZG_SDO_TRIS			(TRISGbits.TRISG8)
%ZG2100_COMMENTS%      	#define ZG_RST_TRIS			(TRISGbits.TRISG0)	
%ZG2100_COMMENTS%    	#define ZG_RST_IO			(LATGbits.LATG0)  
%ZG2100_COMMENTS%    	#define ZG_EINT_TRIS		(TRISAbits.TRISA14) // INT3 
%ZG2100_COMMENTS%    	#define ZG_EINT_IO			(PORTAbits.RA14)     
%ZG2100_COMMENTS%    	#define XCEN33_TRIS		    (TRISGbits.TRISG1)  
%ZG2100_COMMENTS%    	#define	XCEN33_IO			(PORTGbits.RG1)
%ZG2100_COMMENTS%    	#if defined( __C30__ )
%ZG2100_COMMENTS%           #define ZG_EINT_EDGE		(INTCON2bits.INT3EP)
%ZG2100_COMMENTS%        	#define ZG_EINT_IE			(IEC3bits.INT3IE)
%ZG2100_COMMENTS%        	#define ZG_EINT_IF			(IFS3bits.INT3IF)
%ZG2100_COMMENTS%    	#elif defined( __PIC32MX__ )
%ZG2100_COMMENTS%           #define ZG_EINT_EDGE		(INTCONbits.INT3EP)
%ZG2100_COMMENTS%        	#define ZG_EINT_IE			(IEC0bits.INT3IE)
%ZG2100_COMMENTS%        	#define ZG_EINT_IF			(IFS0bits.INT3IF)
%ZG2100_COMMENTS%        	#define ZG_EINT_IE_CLEAR    IEC0CLR
%ZG2100_COMMENTS%        	#define ZG_EINT_IF_CLEAR    IFS0CLR
%ZG2100_COMMENTS%        	#define ZG_EINT_IE_SET      IEC0SET
%ZG2100_COMMENTS%        	#define ZG_EINT_IF_SET      IFS0SET
%ZG2100_COMMENTS%        	#define ZG_EINT_BIT         0x00008000
%ZG2100_COMMENTS%        	#define ZG_EINT_IPCSET      IPC3SET
%ZG2100_COMMENTS%        	#define ZG_EINT_IPCCLR      IPC3CLR
%ZG2100_COMMENTS%        	#define ZG_EINT_IPC_MASK    0xFF000000
%ZG2100_COMMENTS%        	#define ZG_EINT_IPC_VALUE   0x0C000000
%ZG2100_COMMENTS%       #else
%ZG2100_COMMENTS%			#error Determine ZG2100 external interrupt
%ZG2100_COMMENTS%       #endif
%ZG2100_COMMENTS%        
%ZG2100_COMMENTS%    	#define ZG_SSPBUF			(SPI2BUF)
%ZG2100_COMMENTS%    	#define ZG_SPISTAT			(SPI2STAT)
%ZG2100_COMMENTS%    	#define ZG_SPISTATbits		(SPI2STATbits)
%ZG2100_COMMENTS%    	#if defined( __C30__ )
%ZG2100_COMMENTS%        	#define ZG_SPICON1			(SPI2CON1)
%ZG2100_COMMENTS%        	#define ZG_SPICON1bits		(SPI2CON1bits)
%ZG2100_COMMENTS%        	#define ZG_SPICON2			(SPI2CON2)
%ZG2100_COMMENTS%        	#define ZG_SPI_IE			(IEC2bits.SPI2IE)
%ZG2100_COMMENTS%    //    	#define ZG_SPI_IP			(IPC8bits.SPI2IP)
%ZG2100_COMMENTS%        	#define ZG_SPI_IF			(IFS2bits.SPI2IF)
%ZG2100_COMMENTS%       #elif defined( __PIC32MX__ )
%ZG2100_COMMENTS%        	#define ZG_SPICON1			(SPI2CON)
%ZG2100_COMMENTS%        	#define ZG_SPICON1bits		(SPI2CONbits)
%ZG2100_COMMENTS%           #define ZG_SPI_IE_CLEAR     IEC1CLR
%ZG2100_COMMENTS%           #define ZG_SPI_IF_CLEAR     IFS1CLR
%ZG2100_COMMENTS%           #define ZG_SPI_INT_BITS     0x000000e0
%ZG2100_COMMENTS%    		#define ZG_SPI_BRG		    (SPI2BRG)
%ZG2100_COMMENTS%           #define ZG_MAX_SPI_FREQ     (10000000ul)	// Hz
%ZG2100_COMMENTS%       #else
%ZG2100_COMMENTS%			#error Determine ZG2100 SPI information
%ZG2100_COMMENTS%		#endif
%ZG2100_COMMENTS%	#endif


#elif defined(PIC32_ETH_STARTER_KIT)
// PIC32 Ethernet Starter Kit (04-02146) with PIC32MX795F512L processor and National DP83848 10/100 PHY
	// External SMSC PHY configuration
	#define	PHY_RMII				// external PHY runs in RMII mode
	#define	PHY_CONFIG_ALTERNATE	// alternate configuration used
	#define	PHY_ADDRESS			0x1	// the address of the National DP83848 PHY

	// I/O pins configuration
	#define BUTTON1_TRIS		(TRISDbits.TRISD6)
	#define	BUTTON1_IO			(PORTDbits.RD6)
	#define BUTTON2_TRIS		(TRISDbits.TRISD7)
	#define	BUTTON2_IO			(PORTDbits.RD7)
	#define BUTTON3_TRIS		(TRISDbits.TRISD13)
	#define	BUTTON3_IO			(PORTDbits.RD13)

	#define BUTTON0_TRIS		BUTTON1_TRIS		// no button 0. map to 1
	#define	BUTTON0_IO			BUTTON1_IO

	#define LED0_TRIS			(TRISDbits.TRISD0)
	#define LED0_IO				(LATDbits.LATD0)
	#define LED1_TRIS			(TRISDbits.TRISD1)
	#define LED1_IO				(LATDbits.LATD1)
	#define LED2_TRIS			(TRISDbits.TRISD2)
	#define LED2_IO				(LATDbits.LATD2)
	// no other LED's
	#define LED3_TRIS			LED2_TRIS
	#define LED3_IO				LED2_IO
	#define LED4_TRIS			LED2_TRIS
	#define LED4_IO				LED2_IO
	#define LED5_TRIS			LED2_TRIS
	#define LED5_IO				LED2_IO
	#define LED6_TRIS			LED2_TRIS
	#define LED6_IO				LED2_IO
	#define LED7_TRIS			LED2_TRIS
	#define LED7_IO				LED2_IO

	#define LED_GET()			(*((volatile unsigned char*)(&LATD)))
	#define LED_PUT(a)			(*((volatile unsigned char*)(&LATD)) = (a))

	// Note, it is not possible to use a ZeroG ZG2100M 802.11 WiFi PICtail 
	// Plus card with this starter kit.  The required interrupt signal, among 
	// possibly other I/O pins aren't available on the Starter Kit board.  

#elif defined(YOUR_BOARD)
// Define your own board hardware profile here

#else
	#error "Hardware profile not defined.  See available profiles in HardwareProfile.h and modify or create one."
#endif


#if defined(__18CXX)	// PIC18
	// UART mapping functions for consistent API names across 8-bit and 16 or 
	// 32 bit compilers.  For simplicity, everything will use "UART" instead 
	// of USART/EUSART/etc.
	#define BusyUART()				BusyUSART()
	#define CloseUART()				CloseUSART()
	#define ConfigIntUART(a)		ConfigIntUSART(a)
	#define DataRdyUART()			DataRdyUSART()
	#define OpenUART(a,b,c)			OpenUSART(a,b,c)
	#define ReadUART()				ReadUSART()
	#define WriteUART(a)			WriteUSART(a)
	#define getsUART(a,b,c)			getsUSART(b,a)
	#define putsUART(a)				putsUSART(a)
	#define getcUART()				ReadUSART()
	#define putcUART(a)				WriteUSART(a)
	#define putrsUART(a)			putrsUSART((far rom char*)a)

#else	 // PIC24F, PIC24H, dsPIC30, dsPIC33, PIC32
	// Some A/D converter registers on dsPIC30s are named slightly differently 
	// on other procesors, so we need to rename them.
	#if defined(__dsPIC30F__)
		#define ADC1BUF0			ADCBUF0
		#define AD1CHS				ADCHS
		#define	AD1CON1				ADCON1
		#define AD1CON2				ADCON2
		#define AD1CON3				ADCON3
		#define AD1PCFGbits			ADPCFGbits
		#define AD1CSSL				ADCSSL
		#define AD1IF				ADIF
		#define AD1IE				ADIE
		#define _ADC1Interrupt		_ADCInterrupt
	#endif

	// Select which UART the STACK_USE_UART and STACK_USE_UART2TCP_BRIDGE 
	// options will use.  You can change these to U1BRG, U1MODE, etc. if you 
	// want to use the UART1 module instead of UART2.
	#define UBRG					U2BRG
	#define UMODE					U2MODE
	#define USTA					U2STA
	#define BusyUART()				BusyUART2()
	#define CloseUART()				CloseUART2()
	#define ConfigIntUART(a)		ConfigIntUART2(a)
	#define DataRdyUART()			DataRdyUART2()
	#define OpenUART(a,b,c)			OpenUART2(a,b,c)
	#define ReadUART()				ReadUART2()
	#define WriteUART(a)			WriteUART2(a)
	#define getsUART(a,b,c)			getsUART2(a,b,c)
	#if defined(__C32__)
		#define putsUART(a)			putsUART2(a)
	#else
		#define putsUART(a)			putsUART2((unsigned int*)a)
	#endif
	#define getcUART()				getcUART2()
	#define putcUART(a)				do{while(BusyUART()); WriteUART(a); while(BusyUART()); }while(0)
	#define putrsUART(a)			putrsUART2(a)
#endif


#endif
