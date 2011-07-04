/*********************************************************************
 *
 *	Medium Access Control (MAC) Layer for Microchip ENC624J600/424J600
 *  Module for Microchip TCP/IP Stack
 *	 -Provides access to ENC424J600/624J600 Ethernet controller
 *	 -Reference: ENC424J600/624J600 Data sheet (DS39935), IEEE 802.3 
 *    Standard
 *
 *********************************************************************
 * FileName:        ENCX24J600.c
 * Dependencies:    ENCX24J600.h
 *					MAC.h
 *					string.h
 *                  Helpers.h
 *					Delay.h
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.05 or higher
 *					Microchip C30 v3.12 or higher
 *					Microchip C18 v3.30 or higher
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
 * Author               Date   		Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder		12/07/07	Original
 ********************************************************************/



#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#if defined(PAKAI_ETH) && defined(PAKAI_ENCX24J600)

#include "../uip/uip.h"
#include "../uip/uip_arp.h"
#include "../uip/uipopt.h"

#include "../../tinysh/enviro.h"



//#include "TCPIP.h"
#include "../../hardware/hardware.h"
#include "ENCX24J600.h"
#include "StackTsk.h"


#define __ENCX24J600_C
#define PAKAI_INT_ENCX24J600		// untuk mengaktifkan interrupt yg datang
//#include "HardwareProfile.h"

int int_tris;		// apa ini ??
WORD ethRxPointer = 0;

// Make sure that this hardware profile has an ENC424J600/624J600 in it
#if defined(ENC100_INTERFACE_MODE)



/** D E F I N I T I O N S ****************************************************/
#define ENC100_MAX_SPI_FREQ		(14000000ul)	// Hz

// Configuration constants for diagnostics only.  If you don't define any of 
// these constants, auto-negotiation will be used (strongly recommended).
//#define ENC100_FORCE_10MBPS_HALF_DUPLEX
//#define ENC100_FORCE_10MBPS_FULL_DUPLEX
#define ENC100_FORCE_100MBPS_HALF_DUPLEX
//#define ENC100_FORCE_100MBPS_FULL_DUPLEX


// Pseudo Functions - these should not need changing unless porting to a new 
// processor type or speed
#define DelaySetupHold()		do{Nop();}while(0)
#if (GetInstructionClock() > 16000000ul)
	#undef DelaySetupHold
	#define DelaySetupHold()	do{Nop();Nop();}while(0)
#endif
#if (GetInstructionClock() > 32000000ul)
	#undef DelaySetupHold
	#define DelaySetupHold()	do{Nop();Nop();Nop();}while(0)
#endif
#if (GetInstructionClock() > 48000000ul)
	#undef DelaySetupHold
	#define DelaySetupHold()	do{Nop();Nop();Nop();Nop();}while(0)
#endif
#if (GetInstructionClock() > 64000000ul)
	#undef DelaySetupHold
	#define DelaySetupHold()	do{Nop();Nop();Nop();Nop();Nop();}while(0)
#endif

#if (ENC100_INTERFACE_MODE >= 1)	// Parallel mode
	#if defined(ENC100_CS_TRIS)
		#define AssertChipSelect()		do{ENC100_CS_IO = 1;}while(0)
		#define DeassertChipSelect()	do{ENC100_CS_IO = 0;}while(0)
	#else
		#define AssertChipSelect()
		#define DeassertChipSelect()
	#endif
#else						// SPI mode
	//mati sudah ada di *.h
	//#define AssertChipSelect()			do{ENC100_CS_IO = 0;}while(0)
	//#define DeassertChipSelect()		do{ENC100_CS_IO = 1;}while(0)

	#if defined (__18CXX)
	    #define ClearSPIDoneFlag()  do{ENC100_SPI_IF = 0;}while(0)
	    #define WaitForDataByte()   do{while(!ENC100_SPI_IF); ENC100_SPI_IF = 0;}while(0)
	#elif defined(__C30__)
	    #define ClearSPIDoneFlag()
	    static inline __attribute__((__always_inline__)) void WaitForDataByte(void)
	    {
	        while ((ENC100_SPISTATbits.SPITBF == 1) || (ENC100_SPISTATbits.SPIRBF == 0));
	    }
	#elif defined( __PIC32MX__ )
	    #define ClearSPIDoneFlag()
	    static inline __attribute__((__always_inline__)) void WaitForDataByte(void)
	    {
	        while (!ENC100_SPISTATbits.SPITBE || !ENC100_SPISTATbits.SPIRBF);
	    }
	#else
	    //#error Determine SPI flag mechanism
	#endif
#endif


// Binary constant identifiers for ReadMemoryWindow() and WriteMemoryWindow() 
// functions
#define UDA_WINDOW		(0x1)
#define GP_WINDOW		(0x2)
#define RX_WINDOW		(0x4)


#define ETHER_IP		((WORD)0x00)
#define ETHER_ARP		((WORD)0x06)


// A header appended at the start of all RX frames by the hardware
typedef struct  __attribute__((aligned(2), packed))
{
    WORD			NextPacketPointer;
    RXSTATUS		StatusVector;

    MAC_ADDR        DestMACAddr;
    MAC_ADDR        SourceMACAddr;
    WORD_VAL        Type;
} ENC100_PREAMBLE;

APP_CONFIG AppConfig;

BYTE awalkahRx=0;
WORD nextPaketRx;
BYTE awalkahTx=0;
WORD nextPaketTx;


// Internal MAC level variables and flags.
static BYTE vCurrentBank;
static WORD wCurrentPacketPointer;
static WORD wNextPacketPointer;
static struct
{
	unsigned char bWasDiscarded:1;		// Status tracking bit indicating if the last received packet has been discarded via MACDiscardRx() or not.
	unsigned char PoweredDown:1;		// Local cached bit indicating CRYPTEN == ETHEN == 0 and PSLEEP == 1
	unsigned char CryptoEnabled:1;		// Local cached bit indicating CRYPTEN == 1
} ENC100Flags;


// Internal MAC level functions
void ENC100DumpState(void);
int SendSystemReset(void);
static void ToggleCRYPTEN(void);
static WORD ReadReg(WORD wAddress);
static void WriteReg(WORD wAddress, WORD wValue);
static void BFSReg(WORD wAddress, WORD wBitMask);
static void BFCReg(WORD wAddress, WORD wBitMask);
static void ReadMemoryWindow(BYTE vWindow, BYTE *vData, WORD wLength);
static void WriteMemoryWindow(BYTE vWindow, BYTE *vData, WORD wLength);
#if ENC100_INTERFACE_MODE == 0
	static void Execute0(BYTE vOpcode);
	//static BYTE Execute1(BYTE vOpcode, BYTE wDataIn);	// Not currently used
	static WORD Execute2(BYTE vOpcode, WORD wDataIn);
	static DWORD Execute3(BYTE vOpcode, DWORD dwDataIn);
	static void ReadN(BYTE vOpcode, BYTE* vData, WORD wDataLen);
	static void WriteN(BYTE vOpcode, BYTE* vData, WORD wDataLen);
#else
	static void ReadMemory(WORD wAddress, BYTE *vData, WORD wLength);
	static void WriteMemory(WORD wAddress, BYTE *vData, WORD wLength);
#endif


/******************************************************************************
 * Function:        void ENC100Init(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        MACInit sets up the PIC's SPI module and all the 
 *					registers in the ENCX24J600 so that normal operation can 
 *					begin.
 *
 * Note:            None
 *****************************************************************************/
//char led=0;
unsigned int cek_paket(void)
{
	#if 1
	if (FIO_CEK_PAKET & INT_ENC)		// default high
	{
		return 0;
	}
	else
	{
		return 1;						// aktif low
	}
	#endif
	
	/*
	 * 13 feb 09, bukan karena ini paket sering ilang 
	*/
	
	#if 0
	encBankSelect (BANK1);
	if (encReadEthReg (EPKTCNT)) return 1;
	else return 0;
	#endif
	
}

int Enc624Init() {
	init_enc_port();
	spiInit ();
	
	DeassertChipSelect();
	ENCX24J600_Reset();
	//vTaskDelay (100 / portTICK_RATE_MS);
	vTaskDelay(2);
	ENCX24J600_Unreset ();
	//vTaskDelay (100 / portTICK_RATE_MS);
	vTaskDelay(2);
	//printf("clk eth: %d\r\n", GetCLKOUT());
	return Enc64MACInit();
	
}

//#define DEBUG_ETHRX

// 00:04:A3:10:49:71
int Enc624Terima(void) {
	#ifdef DEBUG_ETHRX
	BYTE a[32];
	int i,j,k,l;
	WORD w;
	#endif
	
	int nPaket=0, lenPaket=0;
	WORD almtSkrg=0;
	BYTE aP[8];
	int fff;
	
	if (!MACIsLinked()) {
		//printf("tak nyambung %d..", jmlTuris++);
		return 0;
	}

	if(!(ReadReg(EIR) & EIR_PKTIF)) {
		//printf("EIR_PKTIF ...");
		return 0;
	}
	
	nPaket = (BYTE) ReadReg(ESTAT);		//printf("\r\njml paket: %d, flag: %d\r\n", nPaket, (ReadReg(EIR) & EIR_PKTIF));

	if (!nPaket)	{
	    return 0;
	}

	if (awalkahRx==0) {
		wCurrentPacketPointer = almtSkrg = RXSTART;		//almtSkrg = ReadReg(ERXST);	printf("\r\n  ERXST   = 0x%04hX", almtSkrg);
	} else {
		wCurrentPacketPointer = almtSkrg = nextPaketRx;
	}
	WriteReg(ERXRDPT, wCurrentPacketPointer);
	
	ReadMemory(almtSkrg, aP, 8);
	wNextPacketPointer = nextPaketRx = (aP[1]<<8)+aP[0];
	lenPaket  	= (aP[3]<<8)+aP[2];
	
	#ifdef DEBUG_ETHRX
	printf("\r\nbaca: 0x%04Xh, Next: 0x%04Xh, lenPaket: 0x%04Xh = %d\r\n", almtSkrg, nextPaketRx, lenPaket, lenPaket);
	printf("Isi 8: %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx \r\n", \ 
		aP[0], aP[1], aP[2], aP[3], aP[4], aP[5], aP[6], aP[7]);
	printf("___paket_______%d\r\n", lenPaket);
	#endif
	
	if (nextPaketRx > ENC100_RAM_SIZE)	{
    	awalkahRx = 0;
    	printf("paket overload .... !!!\r\n");
	}
	
	// perbaiki read memory ketika sampe 0x5FFFh
	if (nextPaketRx < almtSkrg) {
		fff = 0x5fff - almtSkrg - 8;
		ReadMemory(almtSkrg+8, &uip_buf[0], fff);
		ReadMemory(RXSTART, &uip_buf[fff], nextPaketRx);
	} else {
		ReadMemory(almtSkrg+8, &uip_buf[0], lenPaket);
	}
	
	#ifdef DEBUG_ETHRX
		k = (0xFFF0 & almtSkrg);		// 0x066E, 0x0660 nP: 
		l = almtSkrg-k;
		
		for(j = k; j <= ((nextPaketRx<=0x5FFF)?nextPaketRx:RXSTART); j += 16)	{
			
			ReadMemory(j, a, 16);
			
			for(i = 0; i < 16; i++)	{
				if((a[i] >= 0x20) && (a[i] <= 0x7E))
					a[i+16] = a[i];
				else
					a[i+16] = '.';
			}
			
			printf("\r\n%04hx  %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx  %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx   %hhc%hhc%hhc%hhc%hhc%hhc%hhc%hhc %hhc%hhc%hhc%hhc%hhc%hhc%hhc%hhc", \
				j, a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11], a[12], a[13], a[14], a[15], \
				a[16], a[17], a[18], a[19], a[20], a[21], a[22], a[23], a[24], a[25], a[26], a[27], a[28], a[29], a[30], a[31]);
		}
		printf("\r\n");
	
	//ReadMemory(nextPaket-8, a, 8);
	//printf("Akhir: %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx", \
	//		a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8]);
	//printf("\r\n\r\n");
	#endif

	//MACDiscardRx();		// malah error pake ini

	WriteReg(ERXTAIL, nextPaketRx-2);	
	BFSReg(ECON1, ECON1_PKTDEC);
	awalkahRx=1;
	
	return (lenPaket-4);
}

#define DEBUG_ETHTX

void Enc624Kirim() {
	WORD lenPaket = uip_len;
	int header = 54;

	#ifdef DEBUG_ETHTX
	BYTE a[32];
	int i,j,k,l;
	
	printf("____KIRIM____ h:%d, len: %d !!\r\n", header, lenPaket);
	for (i=0; i<header; i++) {
		if (i%16==0) { 
			printf("\r\n");
		} else if (i%8==0) {
			printf("  "); 
		}
		printf("%02x ", uip_buf[i]);
	}
	printf("\r\n");
	#endif
	
	if (!MACIsLinked())		return 0;
	
	while (!MACIsTxReady());
	
	//printf("__MACIsTxReady__\r\n");
	WriteMemory(TXSTART, &uip_buf [0], 54);
	
	if (uip_len > 54)	{
		uip_len -= 54;
		//MACPutArray(uip_appdata, uip_len);
		WriteMemory(TXSTART+54, uip_appdata, uip_len);
	}
	
	WriteReg(ETXLEN, TXSTART+lenPaket+1);
	
	#ifdef DEBUG_ETHTX
		for(j = TXSTART; j <=lenPaket; j += 16)	{
			ReadMemory(j, a, 16);
			for(i=0; i<16; i++)	{
				if((a[i] >= 0x20) && (a[i] <= 0x7E))
					a[i+16] = a[i];
				else
					a[i+16] = '.';
			}
			
			printf("\r\n%04hx  %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx  %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx   %hhc%hhc%hhc%hhc%hhc%hhc%hhc%hhc %hhc%hhc%hhc%hhc%hhc%hhc%hhc%hhc", \
				j, a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11], a[12], a[13], a[14], a[15], \
				a[16], a[17], a[18], a[19], a[20], a[21], a[22], a[23], a[24], a[25], a[26], a[27], a[28], a[29], a[30], a[31]);
		}
		printf("\r\n");
	#endif
	
	MACFlush();
}

int Enc64MACInit(void)
{
	volatile portTickType xTicks;
	WORD w;
	struct uip_eth_addr mac;

	#if defined(ENC100_INT_TRIS)	// Interrupt output from ENCx24J600
		ENC100_INT_TRIS = 1;
	#endif
	#if defined(ENC100_MDIX_TRIS)	// MDIX control pin from PIC
		ENC100_MDIX_TRIS = 0;
		ENC100_MDIX_IO = 0;
	#endif

	#if (ENC100_INTERFACE_MODE >= 1)	// Parallel mode
		#if defined(ENC100_CS_TRIS)
			DeassertChipSelect();
			ENC100_CS_TRIS = 0;
		#endif
		ENC100_SO_WR_B0SEL_EN_IO = 0;
		ENC100_SO_WR_B0SEL_EN_TRIS = 0;
		ENC100_SI_RD_RW_IO = 0;
		ENC100_SI_RD_RW_TRIS = 0;
		#if ENC100_INTERFACE_MODE >= 5	// If PSP address bus is multiplexed, then set Address Latch TRIS bit to output
			ENC100_SCK_AL_IO = 0;
			ENC100_SCK_AL_TRIS = 0;
		#endif

		#if !defined(ENC100_BIT_BANG_PMP)
			// PMCON register
			PMCONbits.PMPEN = 1;
			#if ENC100_INTERFACE_MODE >= 5	// Multiplexed mode
				#if defined(__PIC32MX__) && ((ENC100_INTERFACE_MODE == 9) || (ENC100_INTERFACE_MODE == 10))
					// PIC32s can use 16-bit data bus with address multiplexing that doesn't clobber Explorer 16 UART2 or other functions
					PMCONbits.ADRMUX = 0x3;	// 16-bit data + full address muxing
				#else
					PMCONbits.ADRMUX = 0x1;	// 8-bit data + partial address muxing
				#endif
			#else
				PMCONbits.ADRMUX = 0x0;	// Fully demultiplexed address
			#endif
			PMCONbits.PTWREN = 1;	// Enable PMWR strobe
			PMCONbits.PTRDEN = 1;	// Enable PMRD strobe
			PMCONbits.ALP = 1;		// AL strobe is active high polarity
			PMCONbits.WRSP = 1;		// PMWR strobe is active high polarity
			PMCONbits.RDSP = 1;		// PMRD strobe is active high polarity

			// PMMODE register
			PMMODEbits.INCM = 0x0;	// No address increment
			#if defined(__PIC32MX__) && ((ENC100_INTERFACE_MODE == 3) || (ENC100_INTERFACE_MODE == 4) || (ENC100_INTERFACE_MODE == 9) || (ENC100_INTERFACE_MODE == 10))
				PMMODEbits.MODE16 = 1;	// 16-bit mode
			#else
				PMMODEbits.MODE16 = 0;	// 8-bit mode
			#endif
			#if (ENC100_INTERFACE_MODE == 1) || (ENC100_INTERFACE_MODE == 3) || (ENC100_INTERFACE_MODE == 5) || (ENC100_INTERFACE_MODE == 9)	// READ, WRITE strobes
				PMMODEbits.MODE = 0x2;	// Master mode 2: PMRD & PMWR, not PMRD/PMWR & PMENB
			#else	// READ/WRITE, ENABLE strobes
				PMMODEbits.MODE = 0x1;
			#endif
			PMMODEbits.WAITB = 0x0;	// 1Tcy wait for set up time and address latch time
			// Calculate the minimum number of wait states to ensure a 75ns or longer pulse width for READ and WRITE strobes
			#if defined(__PIC32MX__)	// PIC32 is offset by one (i.e. 0 = one wait state)
				#define OPTIMAL_PMP_WAIT_STATES ((BYTE)((double)GetPeripheralClock()*0.000000075))
			#else
				#define OPTIMAL_PMP_WAIT_STATES ((BYTE)((double)GetPeripheralClock()*0.000000075 + 0.9999))
			#endif
			PMMODEbits.WAITM = OPTIMAL_PMP_WAIT_STATES;
			PMMODEbits.WAITE = 0x0;	// 1Tcy wait for hold time

			// PMAEN address enables register			
			#if (ENC100_INTERFACE_MODE == 1) || (ENC100_INTERFACE_MODE == 2) // 8-bit demux
				PMAEN = ENC100_TRANSLATE_TO_PIN_ADDR(0x7FFF);
			#elif (ENC100_INTERFACE_MODE == 3) || (ENC100_INTERFACE_MODE == 4)	// 16-bit demux
				PMAEN = ENC100_TRANSLATE_TO_PIN_ADDR(0x3FFF);
				#if !defined(__PIC32MX__)
					#error 16-bit PMP mode is only available on PIC32.  Use ENC100_INTERFACE_MODE of 1, 2, 5, or 6 (8-bit) instead.
				#endif
			#elif (ENC100_INTERFACE_MODE == 5) || (ENC100_INTERFACE_MODE == 6)	// 8-bit mux
				PMAEN = ENC100_TRANSLATE_TO_PIN_ADDR(0x7F00) | 0x0001;
			#elif (ENC100_INTERFACE_MODE == 9) || (ENC100_INTERFACE_MODE == 10)	// 16-bit mux
				PMAEN = 0x0001; // Connect PMA<0> to AL
				#if !defined(__PIC32MX__)
					#error 16-bit PMP mode is only available on PIC32.  Use ENC100_INTERFACE_MODE of 1, 2, 5, or 6 (8-bit) instead.
				#endif
			#endif

			// Use CMOS schmitt trigger input buffers
			#if defined(__PIC32MX__)
				PMCONbits.PMPTTL = 0;	
			#else
				PADCFG1bits.PMPTTL = 0;
			#endif
		#else
			ENC100_INIT_PSP_BIT_BANG();
		#endif
	#else	// Use SPI interface
		vCurrentBank = 0;			// Needed for SPI only

		// Set up the SPI module on the PIC for communications with the ENCX24J600
		DeassertChipSelect();
		//ENC100_CS_TRIS = 0;			// IRQ mati dulu
		

	    // Set up SPI pins on PIC18s
		#if defined(__18CXX)
		{
			ENC100_SCK_AL_TRIS = 0;
			ENC100_SI_RD_RW_TRIS = 0;
			ENC100_SO_WR_B0SEL_EN_TRIS = 1;

			ENC100_SPICON1 = 0x20;		// SSPEN bit is set, SPI in master mode, FOSC/4, 
										//   IDLE state is low level
			ENC100_SPI_IF = 0;
			ENC100_SPISTATbits.CKE = 1;	// Transmit data on rising edge of clock
			ENC100_SPISTATbits.SMP = 0;	// Input sampled at middle of data output time
		}
		#elif defined(__C30__)
		{
		    ENC100_SPISTAT = 0;    		// clear SPI

			// Ensure SPI doesn't exceed processor limit
			#if (defined(__PIC24F__) || defined(__PIC24H__) || defined(__dsPIC33F__)) && ENC100_MAX_SPI_FREQ > 8000000
				#undef ENC100_MAX_SPI_FREQ
				#define ENC100_MAX_SPI_FREQ	8000000
			#endif
			#if defined(__dsPIC30F__) && ENC100_MAX_SPI_FREQ > 10000000
				#undef ENC100_MAX_SPI_FREQ
				#define ENC100_MAX_SPI_FREQ	10000000
			#endif
		    
		    // Calculate optimal primary and secondary prescalers to make an SPI clock no faster than ENC100_MAX_SPI_FREQ
		    #if GetPeripheralClock()/8/64 > ENC100_MAX_SPI_FREQ
		    	#warning Minimum possible SPI frequency is faster than ENC100_MAX_SPI_FREQ
		    #endif
	    	#define OPTIMAL_PRESCALE		((((~((GetPeripheralClock()/64+ENC100_MAX_SPI_FREQ-1)/ENC100_MAX_SPI_FREQ - 1)) & 0x7)<<2) | 0x0)
		    #if GetPeripheralClock()/8/16 <= ENC100_MAX_SPI_FREQ
		    	#undef OPTIMAL_PRESCALE
		    	#define OPTIMAL_PRESCALE	((((~((GetPeripheralClock()/16+ENC100_MAX_SPI_FREQ-1)/ENC100_MAX_SPI_FREQ - 1)) & 0x7)<<2) | 0x1)
		    #endif
		    #if GetPeripheralClock()/8/4 <= ENC100_MAX_SPI_FREQ
		    	#undef OPTIMAL_PRESCALE
	    		#define OPTIMAL_PRESCALE	((((~((GetPeripheralClock()/4+ENC100_MAX_SPI_FREQ-1)/ENC100_MAX_SPI_FREQ - 1)) & 0x7)<<2) | 0x2)
		    #endif
		    #if GetPeripheralClock()/8/1 <= ENC100_MAX_SPI_FREQ
		    	#undef OPTIMAL_PRESCALE
		    	#define OPTIMAL_PRESCALE	((((~((GetPeripheralClock()/1+ENC100_MAX_SPI_FREQ-1)/ENC100_MAX_SPI_FREQ - 1)) & 0x7)<<2) | 0x3)
		    #endif

			// Work around 1:1, 1:1 SPI errata on early PIC24FJ128GA010, PIC24HJ256GP610, dsPIC33FJ256GP710, etc. silicon where SCK stops running
			#if (defined(__PIC24F__) || defined(__PIC24H__) || defined(__dsPIC33F__)) && (OPTIMAL_PRESCALE == 0x1F)
				#undef OPTIMAL_PRESCALE
				#define OPTIMAL_PRESCALE	0x1B
			#endif

			ENC100_SPICON1 = OPTIMAL_PRESCALE;
		    ENC100_SPICON2 = 0;
		    ENC100_SPICON1bits.CKE = 1;
		    ENC100_SPICON1bits.MSTEN = 1;
		    ENC100_SPISTATbits.SPIEN = 1;
		}
		#elif defined(__C32__)
		{
			ENC100_SPIBRG = (GetPeripheralClock()-1ul)/2ul/ENC100_MAX_SPI_FREQ;
			ENC100_SPICON1bits.SMP = 1;	// Delay SDI input sampling (PIC perspective) by 1/2 SPI clock
		    ENC100_SPICON1bits.CKE = 1;
		    ENC100_SPICON1bits.MSTEN = 1;
			ENC100_SPICON1bits.ON = 1;
		}
		#endif
	#endif


	// Perform a reliable reset
	SendSystemReset();
	//printf("SendSystemReset sukses !!\r\n");

	// Initialize RX tracking variables and other control state flags
	wNextPacketPointer = RXSTART;
	wCurrentPacketPointer = 0x0000;
	ENC100Flags.bWasDiscarded = 1;
	ENC100Flags.PoweredDown = 0;
	ENC100Flags.CryptoEnabled = 0;

	// Set up TX/RX/UDA buffer addresses
	WriteReg(ETXST, TXSTART);
	WriteReg(ERXST, RXSTART);
	WriteReg(ERXTAIL, ENC100_RAM_SIZE-2);
	WriteReg(EUDAST, ENC100_RAM_SIZE);
	WriteReg(EUDAND, ENC100_RAM_SIZE+1);
	
	//printf("\r\nTXSTART: 0x%04Xh\r\n", TXSTART);
	//printf("RXSTART: 0x%04Xh, RXSIZE: 0x%04Xh, RXSTOP: 0x%04Xh\r\n", RXSTART, RXSIZE, RXSTOP);
	#if 0
	printf("\r\n");
	printf("ERXFCON: 0x%04x\r\n", ReadReg(ERXFCON));
	//printf("EPMO: 0x%04x\r\n", ReadReg(EPMO));
	//printf("EPMM1: 0x%04x\r\n", ReadReg(EPMM1));
	#endif
	// default
	WriteReg(EPMO, 0x0000);
	WriteReg(EPMCS, 0x5BFC);
	WriteReg(EPMM1, 0x0FC0);
	
	BFSReg(ERXFCON, ERXFCON_CRCEN | ERXFCON_RUNTEN | ERXFCON_UCEN | ERXFCON_BCEN | ERXFCON_PMEN0);	//  | ERXFCON_MCEN  | ERXFCON_MPEN
	//				CRC	valid ??	paket>64byte	 unicast ??		
	//BFCReg(ERXFCON, ERXFCON_BCEN | ERXFCON_NOTPM);	//  
	//BFCReg(ERXFCON, ERXFCON_UCEN);	// 
	
	#if 0
	//printf("\r\n");
	//printf("ERXFCON: 0x%04x\r\n", ReadReg(ERXFCON));
	printf("EPMCS: 0x%04x\r\n", ReadReg(EPMCS));
	printf("EPMM1: 0x%04x\r\n", ReadReg(EPMM1));
	#endif
	// set Filter
	
//	BFCReg(ERXFCON, ERXFCON_BCEN);
	// ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_PMEN3 | ERXFCON_PMEN0
	
	// Use ENCx24J600 preprogrammed MAC address, if AppConfig is not already set
	// mati dulu saja
	//*
	vTaskDelay(1);
	if(((AppConfig.MyMACAddr.v[0] == 0x00u) && (AppConfig.MyMACAddr.v[1] == 0x04u) && (AppConfig.MyMACAddr.v[2] == 0xA3u) && (AppConfig.MyMACAddr.v[3] == 0x00u) && (AppConfig.MyMACAddr.v[4] == 0x00u) && (AppConfig.MyMACAddr.v[5] == 0x00u)) ||
		((AppConfig.MyMACAddr.v[0] | AppConfig.MyMACAddr.v[1] | AppConfig.MyMACAddr.v[2] | AppConfig.MyMACAddr.v[3] | AppConfig.MyMACAddr.v[4] | AppConfig.MyMACAddr.v[5]) == 0x00u))
	{
		w = ReadReg(MAADR1);
		uip_ethaddr.addr [0] = AppConfig.MyMACAddr.v[0] = mac.addr[0] = ((BYTE*)&w)[0];
		uip_ethaddr.addr [1] = AppConfig.MyMACAddr.v[1] = mac.addr[1] = ((BYTE*)&w)[1];
		w = ReadReg(MAADR2);
		uip_ethaddr.addr [2] = AppConfig.MyMACAddr.v[2] = mac.addr[2] = ((BYTE*)&w)[0];
		uip_ethaddr.addr [3] = AppConfig.MyMACAddr.v[3] = mac.addr[3] = ((BYTE*)&w)[1];
		w = ReadReg(MAADR3);
		uip_ethaddr.addr [4] = AppConfig.MyMACAddr.v[4] = mac.addr[4] = ((BYTE*)&w)[0];
		uip_ethaddr.addr [5] = AppConfig.MyMACAddr.v[5] = mac.addr[5] = ((BYTE*)&w)[1];
		printf("Embd MAC: %02hX:%02hX:%02hX:%02hX:%02hX:%02hX", \
			AppConfig.MyMACAddr.v[0], AppConfig.MyMACAddr.v[1], AppConfig.MyMACAddr.v[2], \
			AppConfig.MyMACAddr.v[3], AppConfig.MyMACAddr.v[4], AppConfig.MyMACAddr.v[5]);			
	}
	else
	{	
		// override MAC. nanti dulu saja
		((BYTE*)&w)[0] = AppConfig.MyMACAddr.v[0] = UIP_ETHADDR0;
		((BYTE*)&w)[1] = AppConfig.MyMACAddr.v[1] = UIP_ETHADDR1;
		WriteReg(MAADR1, w);
		((BYTE*)&w)[0] = AppConfig.MyMACAddr.v[2] = UIP_ETHADDR2;
		((BYTE*)&w)[1] = AppConfig.MyMACAddr.v[3] = UIP_ETHADDR3;
		WriteReg(MAADR2, w);
		((BYTE*)&w)[0] = AppConfig.MyMACAddr.v[4] = UIP_ETHADDR4;
		((BYTE*)&w)[1] = AppConfig.MyMACAddr.v[5] = UIP_ETHADDR5;		// yg ini harus di bikin random
		WriteReg(MAADR3, w);
		printf("tulis MAC: %02hX:%02hX:%02hX:%02hX:%02hX:%02hX\r\n", \
			AppConfig.MyMACAddr.v[0], AppConfig.MyMACAddr.v[1], AppConfig.MyMACAddr.v[2], \
			AppConfig.MyMACAddr.v[3], AppConfig.MyMACAddr.v[4], AppConfig.MyMACAddr.v[5]);
	}
	//*/
	
	uip_setethaddr (mac);
	
	// Set PHY Auto-negotiation to support 10BaseT Half duplex, 
	// 10BaseT Full duplex, 100BaseTX Half Duplex, 100BaseTX Full Duplex,
	// and symmetric PAUSE capability
	WritePHYReg(PHANA, PHANA_ADPAUS0 | PHANA_AD10FD | PHANA_AD10 | PHANA_AD100FD | PHANA_AD100 | PHANA_ADIEEE0);
	
	// Force operating mode, for debugging only.  If none of these statements 
	// are executed, auto-negotiation/parallel detection is used which will 
	// always select the proper mode.
	#if defined(ENC100_FORCE_10MBPS_HALF_DUPLEX)
		WritePHYReg(PHCON1, 0x0000);
	#elif defined(ENC100_FORCE_10MBPS_FULL_DUPLEX)
		WritePHYReg(PHCON1, PHCON1_PFULDPX);
	#elif defined(ENC100_FORCE_100MBPS_HALF_DUPLEX)
		WritePHYReg(PHCON1, PHCON1_SPD100);
	#elif defined(ENC100_FORCE_100MBPS_FULL_DUPLEX)
		WritePHYReg(PHCON1, PHCON1_SPD100 | PHCON1_PFULDPX);
	#endif
	
	#if 0
	printf("\r\n\r\nERXFCON: 0x%04x\r\n", ReadReg(ERXFCON));
	printf("MACON1 : 0x%04x\r\n", ReadPHYReg(MACON1));
	#endif
	// Enable RX packet reception
	BFSReg(ECON1, ECON1_RXEN);
	
	
	#ifdef PAKAI_INT_ENCX24J600
		WriteReg(EIE, EIE_INTIE | EIE_LINKIE | EIE_PKTIE | EIE_TXABTIE | EIE_RXABTIE);
		//WriteReg(EIE, EIE_INTIE | EIE_LINKIE);		
	#endif
	
	awalkahRx=0;
	vTaskDelay(10);
	
	return 1;
}//end MACInit


/******************************************************************************
 * Function:        BOOL MACIsLinked(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TRUE: An Ethernet link is established
 *					FALSE: Ethernet link is not established
 *
 * Side Effects:    None
 *
 * Overview:        Returns the ESTAT<PHYLNK> bit.
 *
 * Note:            None
 *****************************************************************************/
BOOL MACIsLinked(void)
{
	return (ReadReg(ESTAT) & ESTAT_PHYLNK) != 0u;
}

/******************************************************************************
 * Function:        BOOL MACIsTxReady(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TRUE: If no Ethernet transmission is in progress
 *					FALSE: If a previous transmission was started, and it has 
 *						   not completed yet.  While FALSE, the data in the 
 *						   transmit buffer and the TXST/TXLEN pointers must not
 *						   be changed.
 *
 * Side Effects:    None
 *
 * Overview:        Returns the ECON1<TXRTS> bit
 *
 * Note:            None
 *****************************************************************************/
BOOL MACIsTxReady(void)
{
	return !(ReadReg(ECON1) & ECON1_TXRTS);
}


/******************************************************************************
 * Function:        void MACDiscardRx(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Marks the last received packet (obtained using 
 *					MACGetHeader())as being processed and frees the buffer 
 *					memory associated with it
 *
 * Note:            Is is safe to call this function multiple times between 
 *					MACGetHeader() calls.  Extra packets won't be thrown away 
 *					until MACGetHeader() makes it available.
 *****************************************************************************/
void MACDiscardRx(void)
{
	WORD wNewRXTail;

	// Make sure the current packet was not already discarded
	if(ENC100Flags.bWasDiscarded)
		return;
	ENC100Flags.bWasDiscarded = 1;
	
	// Decrement the next packet pointer before writing it into 
	// the ERXRDPT registers. RX buffer wrapping must be taken into account if the 
	// NextPacketLocation is precisely RXSTART.
	wNewRXTail = wNextPacketPointer - 2;
	if(wNextPacketPointer == RXSTART)
		wNewRXTail = ENC100_RAM_SIZE - 2;

	// Decrement the RX packet counter register, EPKTCNT
	BFSReg(ECON1, ECON1_PKTDEC);

	// Move the receive read pointer to unwrite-protect the memory used by the 
	// last packet.  The writing order is important: set the low byte first, 
	// high byte last (handled automatically in WriteReg()).
	WriteReg(ERXTAIL, wNewRXTail);
}


/******************************************************************************
 * Function:        WORD MACGetFreeRxSize(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          A WORD estimate of how much RX buffer space is free at 
 *					the present time.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 *****************************************************************************/
WORD MACGetFreeRxSize(void)
{
	WORD wHeadPtr;

	wHeadPtr = ReadReg(ERXHEAD);
	
	// Calculate the difference between the pointers, taking care to account 
	// for buffer wrapping conditions
	if(wHeadPtr > wCurrentPacketPointer)
		return (RXSTOP - RXSTART) - (wHeadPtr - wCurrentPacketPointer);
		
	return wCurrentPacketPointer - wHeadPtr - 2;
}


/******************************************************************************
 * Function:        BOOL MACGetHeader(MAC_ADDR *remote, BYTE* type)
 *
 * PreCondition:    None
 *
 * Input:           *remote: Location to store the Source MAC address of the 
 *							 received frame.
 *					*type: Location of a BYTE to store the constant 
 *						   MAC_UNKNOWN, ETHER_IP, or ETHER_ARP, representing 
 *						   the contents of the Ethernet type field.
 *
 * Output:          TRUE: If a packet was waiting in the RX buffer.  The 
 *						  remote, and type values are updated.
 *					FALSE: If a packet was not pending.  remote and type are 
 *						   not changed.
 *
 * Side Effects:    Last packet is discarded if MACDiscardRx() hasn't already
 *					been called.
 *
 * Overview:        None
 *
 * Note:            None
 *****************************************************************************/
BOOL MACGetHeader(MAC_ADDR *remote, BYTE* type)
{
	ENC100_PREAMBLE header;

	// Test if at least one packet has been received and is waiting
	if(!(ReadReg(EIR) & EIR_PKTIF))
	{
		// No packets are waiting.  See if we are unlinked and need to process 
		// auto crossover timers.
		#if defined(ENC100_MDIX_TRIS)
		{
			static WORD wMDIXTimer = 0;
			
			// See if it is time to swap MDI mode
			if((WORD)(wMDIXTimer - (WORD)TickGetDiv256()) > (WORD)(2u*TICK_SECOND/256u))
			{
				// If we are linked, then we should simply reset the timer to a 
				// max of 1.3 seconds and do nothing
				if(MACIsLinked())
				{
					wMDIXTimer = (WORD)TickGetDiv256() + (WORD)(13u*TICK_SECOND/256u/10u);
				}
				else
				{
					// We are unlinked and the MDI-X timer expired, lets swap MDI mode
					ENC100_MDIX_IO ^= 1;
					
					// Set up new timer to expire sometime randomly between 1.0
					// seconds and 1.55 seconds in the future.  This needs to 
					// have randomness so that if you plug two similar devices 
					// into each other, you don't have a lock-step switching 
					// problem in which both devices swap MDI mode 
					// simultaneously and never link up.
					wMDIXTimer = (WORD)TickGetDiv256() + (WORD)(TICK_SECOND/256u) + (WORD)(55ul*TICK_SECOND/256ul/100ul*(DWORD)rand()/(DWORD)RAND_MAX);

					//// Restart auto-negotiation
					//WritePHYReg(PHCON1, PHCON1_ANEN | PHCON1_RENEG);
				}
			}
		}
		#endif

		return FALSE;
	}

	// Discard the last packet, if the user application hasn't done so already
	if(!ENC100Flags.bWasDiscarded)
	{
		MACDiscardRx();

		// Test again if at least one packet has been received and is waiting
		if(!(ReadReg(EIR) & EIR_PKTIF))
			return FALSE;
	}

	// Set the RX Read Pointer to the beginning of the next unprocessed packet
	wCurrentPacketPointer = wNextPacketPointer;
	WriteReg(ERXRDPT, wCurrentPacketPointer);

	// Obtain the MAC header from the Ethernet buffer
	ReadMemoryWindow(RX_WINDOW, (BYTE*)&header, sizeof(header));

	// The EtherType field, like most items transmitted on the Ethernet medium
	// are in big endian.
	
	// mati dulu
	//header.Type.Val = swaps(header.Type.Val);

	// Validate the data returned from the ENC624J600 Family device.  Random 
	// data corruption, such as if a single SPI/PSP bit error occurs while 
	// communicating or a momentary power glitch could cause this to occur 
	// in rare circumstances.  Also, certain hardware bugs such as violations 
	// of the absolute maximum electrical specs can cause this.  For example,
	// if an MCU with a high slew rate were to access the interface, parasitic 
	// inductance in the traces could cause excessive voltage undershoot.  
	// If the voltage goes too far below ground, the ENCx24J600's internal
	// ESD structure may activate and disrupt the communication.  To prevent 
	// this, ensure that you have a clean board layout and consider adding 
	// resistors in series with the MCU output pins to limit the slew rate 
	// of signals going to the ENCx24J600. 100 Ohm resistors is a good value 
	// to start testing with.
	if(header.NextPacketPointer > RXSTOP || ((BYTE_VAL*)(&header.NextPacketPointer))->bits.b0 ||
	   header.StatusVector.bits.Zero || header.StatusVector.bits.ZeroH || 
	   header.StatusVector.bits.CRCError ||
	   header.StatusVector.bits.ByteCount > 1522u ||
	   !header.StatusVector.bits.ReceiveOk)
	{
		//ENC100DumpState();
		
		// mati dulu
		portNOP();
		//Nop();
		vTaskDelay(1);
		
		//Reset();
	}

	// Save the location where the hardware will write the next packet to
	wNextPacketPointer = header.NextPacketPointer;

	// Return the Ethernet frame's Source MAC address field to the caller
	// This parameter is useful for replying to requests without requiring an 
	// ARP cycle.
    memcpy((void*)remote->v, (void*)header.SourceMACAddr.v, sizeof(*remote));

	// Return a simplified version of the EtherType field to the caller
    *type = MAC_UNKNOWN;
    if( (header.Type.v[1] == 0x08u) && 
    	((header.Type.v[0] == (BYTE)ETHER_IP) || (header.Type.v[0] == (BYTE)ETHER_ARP)) )
    {
    	*type = header.Type.v[0];
    }

    // Mark this packet as discardable
    ENC100Flags.bWasDiscarded = 0;
	return TRUE;
}


/******************************************************************************
 * Function:        void MACPutHeader(MAC_ADDR *remote, BYTE type, WORD dataLen)
 *
 * PreCondition:    MACIsTxReady() must return TRUE.
 *
 * Input:           *remote: Pointer to memory which contains the destination
 * 							 MAC address (6 bytes)
 *					type: The constant ETHER_ARP or ETHER_IP, defining which 
 *						  value to write into the Ethernet header's type field.
 *					dataLen: Length of the Ethernet data payload
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            Because of the dataLen parameter, it is probably 
 *					advantagous to call this function immediately before 
 *					transmitting a packet rather than initially when the 
 *					packet is first created.  The order in which the packet
 *					is constructed (header first or data first) is not 
 *					important.
 *****************************************************************************/
void MACPutHeader(MAC_ADDR *remote, BYTE type, WORD dataLen)
{
	WORD wEthernetType;

	wEthernetType = 0x08 | ((type == MAC_IP) ? (ETHER_IP<<8) : (ETHER_ARP<<8));
	
	// Set the Window Write Pointer to the beginning of the transmit buffer
	WriteReg(EGPWRPT, TXSTART);
	WriteReg(ETXLEN, dataLen + sizeof(ETHER_HEADER));

	// Write the Ethernet destination MAC address, our source MAC address, 
	// and the Ethernet Type field.
    WriteMemoryWindow(GP_WINDOW, (BYTE*)remote, sizeof(*remote));
    
    // mati dulu
    //WriteMemoryWindow(GP_WINDOW, (BYTE*)&AppConfig.MyMACAddr, 6);
    WriteMemoryWindow(GP_WINDOW, (BYTE*)&wEthernetType, 2);
}



/******************************************************************************
 * Function:        void MACFlush(void)
 *
 * PreCondition:    A packet has been created by calling MACPut() and 
 *					MACPutHeader().
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        MACFlush causes the current TX packet to be sent out on 
 *					the Ethernet medium.  The hardware MAC will take control
 *					and handle CRC generation, collision retransmission and 
 *					other details.
 *
 * Note:			After transmission completes (MACIsTxReady() returns TRUE), 
 *					the packet can be modified and transmitted again by calling 
 *					MACFlush() again.  Until MACPutHeader() or MACPut() is 
 *					called (in the TX data area), the data in the TX buffer 
 *					will not be corrupted.
 *****************************************************************************/
void MACFlush(void)
{
	WORD w;

	// Check to see if the duplex status has changed.  This can 
	// change if the user unplugs the cable and plugs it into a 
	// different node.  Auto-negotiation will automatically set 
	// the duplex in the PHY, but we must also update the MAC 
	// inter-packet gap timing and duplex state to match.
	if(ReadReg(EIR) & EIR_LINKIF)
	{
		BFCReg(EIR, EIR_LINKIF);

		// Update MAC duplex settings to match PHY duplex setting
		w = ReadReg(MACON2);
		if(ReadReg(ESTAT) & ESTAT_PHYDPX)
		{
			// Switching to full duplex
			WriteReg(MABBIPG, 0x15);
			w |= MACON2_FULDPX;
		}
		else
		{
			// Switching to half duplex
			WriteReg(MABBIPG, 0x12);
			w &= ~MACON2_FULDPX;
		}
		WriteReg(MACON2, w);
	}


	// Start the transmission, but only if we are linked.  Supressing 
	// transmissing when unlinked is necessary to avoid stalling the TX engine 
	// if we are in PHY energy detect power down mode and no link is present.  
	// A stalled TX engine won't do any harm in itself, but will cause the 
	// MACIsTXReady() function to continuously return FALSE, which will 
	// ultimately stall the Microchip TCP/IP stack since there is blocking code 
	// elsewhere in other files that expect the TX engine to always self-free 
	// itself very quickly.
	if(ReadReg(ESTAT) & ESTAT_PHYLNK) {
		BFSReg(ECON1, ECON1_TXRTS);
		#ifdef DEBUG_ETHTX
		printf("________________________harus__dikirim____________________ !!!\r\n");
		#endif
	}
}


/******************************************************************************
 * Function:        void MACSetReadPtrInRx(WORD offset)
 *
 * PreCondition:    A packet has been obtained by calling MACGetHeader() and 
 *					getting a TRUE result.
 *
 * Input:           offset: WORD specifying how many bytes beyond the Ethernet 
 *							header's type field to relocate the SPI read 
 *							pointer.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        SPI read pointer are updated.  All calls to 
 *					MACGet() and MACGetArray() will use these new values.
 *
 * Note:			RXSTOP must be statically defined as being > RXSTART for 
 *					this function to work correctly.  In other words, do not 
 *					define an RX buffer which spans the 0x1FFF->0x0000 memory
 *					boundary.
 *****************************************************************************/
void MACSetReadPtrInRx(WORD offset)
{
	WORD wReadPtr;

	// Determine the address of the beginning of the entire packet
	// and adjust the address to the desired location
	wReadPtr = wCurrentPacketPointer + sizeof(ENC100_PREAMBLE) + offset;
	
	// Since the receive buffer is circular, adjust if a wraparound is needed
	if(wReadPtr > RXSTOP)
		wReadPtr -= RXSIZE;
	
	// Set the RX Window Read pointer to the new calculated value
	WriteReg(ERXRDPT, wReadPtr);
}


/******************************************************************************
 * Function:        PTR_BASE MACSetWritePtr(PTR_BASE address)
 *
 * PreCondition:    None
 *
 * Input:           Address: Address to seek to
 *
 * Output:          WORD: Old EWRPT location
 *
 * Side Effects:    None
 *
 * Overview:        SPI write pointer is updated.  All calls to 
 *					MACPut() and MACPutArray() will use this new value.
 *
 * Note:			None
 *****************************************************************************/
PTR_BASE MACSetWritePtr(PTR_BASE address)
{
	WORD wOldWritePtr;

	wOldWritePtr = ReadReg(EGPWRPT);

	// Set the TX Write Pointer to the new calculated value
	WriteReg(EGPWRPT, address);

	return wOldWritePtr;
}


/******************************************************************************
 * Function:        PTR_BASE MACSetReadPtr(PTR_BASE Address)
 *
 * PreCondition:    None
 *
 * Input:           Address: Address to seek to
 *
 * Output:          WORD: Old ERDPT value
 *
 * Side Effects:    None
 *
 * Overview:        SPI write pointer is updated.  All calls to 
 *					MACPut() and MACPutArray() will use this new value.
 *
 * Note:			None
 *****************************************************************************/
PTR_BASE MACSetReadPtr(PTR_BASE address)
{
	WORD wOldReadPtr;

	wOldReadPtr = ReadReg(ERXRDPT);

	// Set the RX Read Pointer to the new calculated value
	WriteReg(ERXRDPT, address);

	return wOldReadPtr;
}


/******************************************************************************
 * Function:        WORD MACCalcRxChecksum(WORD offset, WORD len)
 *
 * PreCondition:    None
 *
 * Input:           offset	- Number of bytes beyond the beginning of the 
 *							Ethernet data (first byte after the type field) 
 *							where the checksum should begin
 *					len		- Total number of bytes to include in the checksum
 *
 * Output:          16-bit checksum as defined by RFC 793.
 *
 * Side Effects:    None
 *
 * Overview:        This function performs a checksum calculation in the MAC
 *                  buffer itself
 *
 * Note:            None
 *****************************************************************************/
WORD MACCalcRxChecksum(WORD offset, WORD len)
{
	WORD wStartAddress;

	// Add the offset requested by firmware plus the Ethernet header
	wStartAddress = wCurrentPacketPointer + sizeof(ENC100_PREAMBLE) + offset;
	if(wStartAddress > RXSTOP)
		wStartAddress -= RXSIZE;

	// If in power down, temporarily bring power up so we can use the DMA
	if(ENC100Flags.PoweredDown)
		BFSReg(ECON2, ECON2_ETHEN);

	// Calculate the checksum using the ENCX24J600 DMA engine		
	while(ReadReg(ECON1) & ECON1_DMAST);
	WriteReg(EDMAST, wStartAddress);
	WriteReg(EDMALEN, len);
	BFCReg(ECON1, ECON1_DMACPY | ECON1_DMANOCS | ECON1_DMACSSD);
	BFSReg(ECON1, ECON1_DMAST);
	while(ReadReg(ECON1) & ECON1_DMAST);

	// Restore power down state, if applicable
	if(ENC100Flags.PoweredDown)
		BFCReg(ECON2, ECON2_ETHEN);

	return ReadReg(EDMACS);
}

/******************************************************************************
 * Function:        WORD CalcIPBufferChecksum(WORD len)
 *
 * PreCondition:    Read buffer pointer set to starting of checksum data
 *
 * Input:           len: Total number of bytes to calculate the checksum over. 
 *						 The first byte included in the checksum is the byte 
 *						 pointed to by ERDPT, which is updated by calls to 
 *						 MACSetReadPtr(), MACGet(), MACGetArray(), 
 *						 MACGetHeader(), etc.
 *
 * Output:          16-bit checksum as defined by RFC 793
 *
 * Side Effects:    None
 *
 * Overview:        This function performs a checksum calculation in the MAC
 *                  buffer itself.  The ENCx24J600 has a hardware DMA module 
 *					which can calculate the checksum faster than software, so 
 *					this function replaces the CaclIPBufferChecksum() function 
 *					defined in the helpers.c file.  Through the use of 
 *					preprocessor defines, this replacement is automatic.
 *
 * Note:            This function works either in the RX buffer area or the TX
 *					buffer area.  No validation is done on the len parameter.
 *****************************************************************************/
WORD CalcIPBufferChecksum(WORD len)
{
	// If in power down, temporarily bring power up so we can use the DMA
	if(ENC100Flags.PoweredDown)
		BFSReg(ECON2, ECON2_ETHEN);

	// Calculate the checksum using the ENCX24J600 DMA engine
	while(ReadReg(ECON1) & ECON1_DMAST);
	WriteReg(EDMAST, ReadReg(ERXRDPT));
	WriteReg(EDMALEN, len);
	BFCReg(ECON1, ECON1_DMACPY | ECON1_DMANOCS | ECON1_DMACSSD);
	BFSReg(ECON1, ECON1_DMAST);
	while(ReadReg(ECON1) & ECON1_DMAST);

	// Restore power down state, if applicable
	if(ENC100Flags.PoweredDown)
		BFCReg(ECON2, ECON2_ETHEN);

	return ReadReg(EDMACS);
}


/*****************************************************************************
  Function:
	void MACMemCopyAsync(PTR_BASE destAddr, PTR_BASE sourceAddr, WORD len)

  Summary:
	Asynchronously copies data from one address to another within the 24KB 
	Ethernet memory.

  Description:
	Asynchronously copies data from one address to another within the 24KB 
	Ethernet memory using the ENCX24J600 hardware DMA engine (very fast).  
	Overlapped memory regions are allowed (with restrictions).  The addresses 
	do not have to be aligned.

  Precondition:
	SPI bus must be initialized (done in MACInit()).

  Parameters:
	destAddr - Destination address in the Ethernet memory to copy to.  If 
		(PTR_BASE)-1 is specified, the current EGPWRPT value will be used 
		instead.
	sourceAddr - Source address to read from.  If (PTR_BASE)-1 is specified, 
		the current EGPRDPT value will be used instead.
	len - Number of bytes to copy

  Returns:
	None

  Remarks:
  	Call MACIsMemCopyDone() to see when the transfer is complete.
  	
	Copying to a destination region that overlaps with the source address 
	is supported only if the destination start address is at a lower memory 
	address (closer to 0x0000) than the source pointer.  However, if they do 
	overlap there must be at least 2 bytes of non-overlap to ensure correct 
	results due to hardware DMA requirements.  For example, destAddr = 0; 
	sourceAddr = 1; is illegal while destAddr = 0; sourceAddr = 2; is fine.
  
 	If a prior transfer is already in progress prior to calling this function, 
 	this function will block until it can start this transfer.

 	If (PTR_BASE)-1 is used for the sourceAddr or destAddr parameters, 
 	then that pointer will get updated with the next address after the read or 
 	write.
 *****************************************************************************/
void MACMemCopyAsync(PTR_BASE destAddr, PTR_BASE sourceAddr, WORD len)
{
	WORD wNewReadPtr;

	// Decode destination and source addresses
	if(destAddr == (PTR_BASE)-1)
	{
		destAddr = ReadReg(EGPWRPT);
		WriteReg(EGPWRPT, destAddr + len);
	}
	if(sourceAddr == (PTR_BASE)-1)
	{
		sourceAddr = ReadReg(ERXRDPT);
		wNewReadPtr = sourceAddr + len;
		if(wNewReadPtr > RXSTOP)
			wNewReadPtr -= RXSIZE;
		WriteReg(ERXRDPT, wNewReadPtr);
	}

	// If in power down, temporarily bring power up so we can use the DMA
	if(ENC100Flags.PoweredDown)
		BFSReg(ECON2, ECON2_ETHEN);

	// Start the copy using the DMA 
	while(ReadReg(ECON1) & ECON1_DMAST);
	WriteReg(EDMAST, sourceAddr);
	WriteReg(EDMADST, destAddr);
	WriteReg(EDMALEN, len);
	BFSReg(ECON1, ECON1_DMAST | ECON1_DMACPY);
}

BOOL MACIsMemCopyDone(void)
{
	BOOL bDone;
	
	bDone = !(ReadReg(ECON1) & ECON1_DMAST);
	
	// Restore power down state, if applicable
	if(bDone && ENC100Flags.PoweredDown)
		BFCReg(ECON2, ECON2_ETHEN);

	return bDone;
}


/******************************************************************************
 * Function:        BYTE MACGet()
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 * 					ERDPT must point to the place to read from.
 *
 * Input:           None
 *
 * Output:          Byte read from the ENCx24J600's RAM
 *
 * Side Effects:    None
 *
 * Overview:        MACGet returns the byte pointed to by ERDPT and 
 *					increments ERDPT so MACGet() can be called again.  The 
 *					increment will follow the receive buffer wrapping boundary.
 *
 * Note:            None
 *****************************************************************************/
BYTE MACGet()
{
	BYTE i;
	
	ReadMemoryWindow(RX_WINDOW, &i, 1);
	return i;
}//end MACGet


/******************************************************************************
 * Function:        WORD MACGetArray(BYTE *val, WORD len)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 * 					ERDPT must point to the place to read from.
 *
 * Input:           *val: Pointer to storage location
 *					len:  Number of bytes to read from the data buffer.
 *
 * Output:          Byte(s) of data read from the data buffer.
 *
 * Side Effects:    None
 *
 * Overview:        Burst reads several sequential bytes from the data buffer 
 *					and places them into local memory.  With SPI burst support, 
 *					it performs much faster than multiple MACGet() calls.
 *					ERDPT is incremented after each byte, following the same 
 *					rules as MACGet().
 *
 * Note:            None
 *****************************************************************************/
WORD MACGetArray(BYTE *val, WORD len)
{
	WORD wNewReadPtr;
	
	if(val)
	{
		ReadMemoryWindow(RX_WINDOW, val, len);
	}
	else
	{
		wNewReadPtr = ReadReg(ERXRDPT) + len;
		if(wNewReadPtr > RXSTOP)
			wNewReadPtr -= RXSIZE;
		WriteReg(ERXRDPT, wNewReadPtr);
	}
		
	return len;
}//end MACGetArray


/******************************************************************************
 * Function:        void MACPut(BYTE val)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 * 					EWRPT must point to the location to begin writing.
 *
 * Input:           Byte to write into the ENCx24J600 buffer memory
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        MACPut outputs the Write Buffer Memory opcode/constant 
 *					(8 bits) and data to write (8 bits) over the SPI.  
 *					EWRPT is incremented after the write.
 *
 * Note:            None
 *****************************************************************************/
void MACPut(BYTE val)
{
	WriteMemoryWindow(GP_WINDOW, &val, 1);
}//end MACPut


/******************************************************************************
 * Function:        void MACPutArray(BYTE *val, WORD len)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 * 					EWRPT must point to the location to begin writing.
 *
 * Input:           *val: Pointer to source of bytes to copy.
 *					len:  Number of bytes to write to the data buffer.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        MACPutArray writes several sequential bytes to the 
 *					ENCx24J600 RAM.  It performs faster than multiple MACPut()
 *					calls.  EWRPT is incremented by len.
 *
 * Note:            None
 *****************************************************************************/
void MACPutArray(BYTE *val, WORD len)
{
	WriteMemoryWindow(GP_WINDOW, val, len);
}//end MACPutArray


#if defined(__18CXX)
/******************************************************************************
 * Function:        void MACPutROMArray(ROM BYTE *val, WORD len)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 * 					EWRPT must point to the location to begin writing.
 *
 * Input:           *val: Pointer to source of bytes to copy.
 *					len:  Number of bytes to write to the data buffer.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        MACPutArray writes several sequential bytes to the 
 *					ENCx24J600 RAM.  It performs faster than multiple MACPut()
 *					calls.  EWRPT is incremented by len.
 *
 * Note:            None
 *****************************************************************************/
void MACPutROMArray(ROM BYTE *val, WORD len)
{
	WORD wChunkLen;
	BYTE vBuffer[12];

	while(len)
	{
		wChunkLen = sizeof(vBuffer);
		if(len < wChunkLen)
			wChunkLen = len;

		memcpypgm2ram((void*)vBuffer, (ROM void*)val, wChunkLen);
		WriteMemoryWindow(GP_WINDOW, vBuffer, wChunkLen);

		len -= wChunkLen;
		val += wChunkLen;
	}
}//end MACPutROMArray
#endif


/******************************************************************************
 * Function:        void MACPowerDown(void)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        MACPowerDown puts the ENCx24J600 in low power sleep mode. In
 *					sleep mode, no packets can be transmitted or received.  
 *					All MAC and PHY registers should not be accessed.
 *
 *					To exit power down, call MACPowerUp().
 *
 * Note:            If a packet is being transmitted while this function is 
 * 					called, this function will block until it is it complete.
 *					If anything is being received, it will be completed.
 *****************************************************************************/
#ifdef PAKAI_ETH_CRYPT
void MACPowerDown(void)
{
	// Disable packet reception
	BFCReg(ECON1, ECON1_RXEN);

	// Make sure any last packet which was in-progress when RXEN was cleared 
	// is completed
	while(ReadReg(ESTAT) & ESTAT_RXBUSY);

	// If a packet is being transmitted, the DMA is operating, the Modular 
	// Exponentiation or AES engine is running, wait for it to finish
	while(ReadReg(ECON1) & (ECON1_TXRTS | ECON1_DMAST | ECON1_MODEXST | ECON1_AESST));
	
	// Enter sleep mode
	ENC100Flags.PoweredDown = 1;
	ENC100Flags.CryptoEnabled = 0;
	if(ReadReg(EIR) & EIR_CRYPTEN)				// Turn off ModdEx/AES clock
		ToggleCRYPTEN();
	WritePHYReg(PHCON1, PHCON1_PSLEEP);			// Turn off the PHY
	BFCReg(ECON2, ECON2_ETHEN | ECON2_STRCH);	// Turn off general internal clocks and LED stretching so they immediately turn off and don't get stuck on
}//end MACPowerDown
#endif

/******************************************************************************
 * Function:        void MACEDPowerDown(void)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        MACEDPowerDown puts the ENCx24J600 PHY in Energy Detect 
 *					mode. In this mode, the PHY will passively listen for link 
 *					pulses and automatically link up, if detected. This can be 
 *					detected via the MACIsLinkned() function.  This power state 
 *					will save power only when an Ethernet cable is unattached.
 *
 *					To exit energy detect power down, call MACPowerUp().
 *
 * Note:            The ENCx24J600 is fully operational when in energy detect 
 *					mode.  If a Ethernet link is detected or already established,
 *					full TX/RX activity will work as normal and no power saving 
 *					will occur.
 *****************************************************************************/
#ifdef PAKAI_ETH_CRYPT
void MACEDPowerDown(void)
{
	// Put the PHY into energy detect mode
	WritePHYReg(PHCON2, PHCON2_EDPWRDN);		

	// Go into general power down
	ENC100Flags.CryptoEnabled = 0;
	if(ReadReg(EIR) & EIR_CRYPTEN)				// Turn off ModdEx/AES clock
		ToggleCRYPTEN();
	ENC100Flags.PoweredDown = 1;
	BFCReg(ECON2, ECON2_ETHEN | ECON2_STRCH);	
}//end MACEDPowerDown
#endif
/******************************************************************************
 * Function:        void MACPowerUp(void)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        MACPowerUp returns the ENCx24J600 back to normal operation
 *					after a previous call to MACPowerDown().  Calling this 
 *					function when already powered up will have no effect.
 *
 * Note:            If a link partner is present, it will take 10s of 
 *					milliseconds before a new link will be established after
 *					waking up.  While not linked, packets which are 
 *					transmitted will most likely be lost.  MACIsLinked() can 
 *					be called to determine if a link is established.
 *****************************************************************************/
void MACPowerUp(void)		// ora perlu karena selalu ON
{	
	// Start up general clocks and reenable LED stretching
	ENC100Flags.PoweredDown = 0;
	BFSReg(ECON2, ECON2_ETHEN | ECON2_STRCH);
	
	// Power PHY back up and put in auto-negotation mode to reestablish a link
	if(ReadPHYReg(PHCON1) & PHCON1_PSLEEP)
		WritePHYReg(PHCON1, PHCON1_ANEN);
	
	// Disable energy detect PHY power down
	WritePHYReg(PHCON2, 0x0000);

	// Ensure the PLL and PHY are ready
	while(!(ReadReg(ESTAT) & (ESTAT_PHYRDY | ESTAT_CLKRDY)));
	
	// Enable packet reception
	BFSReg(ECON1, ECON1_RXEN);
}//end MACPowerUp


/******************************************************************************
 * Function:        void SetCLKOUT(BYTE NewConfig)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           NewConfig - 0x00: CLKOUT disabled (pin driven low)
 *								0x01: Divide by 1 (25 MHz)
 *								0x02: Divide by 2 (12.5 MHz)
 *								0x03: Divide by 3 (8.333333 MHz)
 *								0x04: Divide by 4 (6.25 MHz, POR default)
 *								0x05: Divide by 8 (3.125 MHz)
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Writes the value of NewConfig into the ECOCON register.  
 *					The CLKOUT pin will beginning outputting the new frequency
 *					immediately.
 *
 * Note:            
 *****************************************************************************/
#ifdef PAKAI_ETH_CKLOUT
void SetCLKOUT(BYTE NewConfig)		// gak perlu, ini buat debug 
{	
	WORD w;
	
	w = ReadReg(ECON2) & 0xF0FF;	
	((BYTE*)&w)[1] |= (NewConfig & 0x0F);
	WriteReg(ECON2, w);
}//end SetCLKOUT
#endif

/******************************************************************************
 * Function:        BYTE GetCLKOUT(void)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           None
 *
 * Output:          BYTE - 0x00: CLKOUT disabled (pin driven low)
 *						   0x01: Divide by 1 (25 MHz)
 *						   0x02: Divide by 2 (12.5 MHz)
 *						   0x03: Divide by 3 (8.333333 MHz)
 *						   0x04: Divide by 4 (6.25 MHz, POR default)
 *						   0x05: Divide by 8 (3.125 MHz)
 *						   0x06: Reserved
 *						   0x07: Reserved
 *
 * Side Effects:    None
 *
 * Overview:        Returns the current value of the ECOCON register.
 *
 * Note:            None
 *****************************************************************************/
#ifdef PAKAI_ETH_CKLOUT
BYTE GetCLKOUT(void)				// gak perlu karena SetCLKOUT tidak dipake
{	
	WORD w;

	w = ReadReg(ECON2) & 0xF0FF;	

	return ((BYTE*)&w)[1] & 0x0F;
}//end GetCLKOUT
#endif

/******************************************************************************
 * Function:        void SetRXHashTableEntry(MAC_ADDR DestMACAddr)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           DestMACAddr: 6 byte group destination MAC address to allow 
 *								 through the Hash Table Filter
 *
 * Output:          Sets the appropriate bit in the EHT* registers to allow 
 *					packets sent to DestMACAddr to be received if the Hash 
 *					Table receive filter is enabled
 *
 * Side Effects:    None
 *
 * Overview:        Calculates a CRC-32 using polynomial 0x4C11DB7 and then, 
 *					using bits 28:23 of the CRC, sets the appropriate bit in 
 *					the EHT* registers
 *
 * Note:            This code is commented out to save code space on systems 
 *					that do not need this function.  Change the "#if 0" line 
 *					to "#if 1" to uncomment it.
 *****************************************************************************/
#if 0
void SetRXHashTableEntry(MAC_ADDR DestMACAddr)			// sudah dinggak perlukan
{
	DWORD_VAL CRC = {0xFFFFFFFF};
	WORD HTRegister;
	BYTE i, j;

	// Calculate a CRC-32 over the 6 byte MAC address 
	// using polynomial 0x4C11DB7
	for(i = 0; i < sizeof(MAC_ADDR); i++)
	{
		BYTE  crcnext;
	
		// shift in 8 bits
		for(j = 0; j < 8; j++)
		{
			crcnext = 0;
			if(((BYTE_VAL*)&(CRC.v[3]))->bits.b7)
				crcnext = 1;
			crcnext ^= (((BYTE_VAL*)&DestMACAddr.v[i])->bits.b0);
	
			CRC.Val <<= 1;
			if(crcnext)
				CRC.Val ^= 0x4C11DB7;
			// next bit
			DestMACAddr.v[i] >>= 1;
		}
	}
	
	// CRC-32 calculated, now extract bits 28:23
	// Bits 25:23 define where within the Hash Table byte the bit needs to be set
	// Bits 28:26 define which of the 8 Hash Table bytes that bits 25:23 apply to
	i = CRC.v[3] & 0x1F;
	HTRegister = (i >> 2) + EHT1;
	i = (i << 1) & 0x06;
	((BYTE_VAL*)&i)->bits.b0 = ((BYTE_VAL*)&CRC.v[2])->bits.b7;
	
	// Set the proper bit in the Hash Table
	BFSReg(HTRegister, 1<<i);
}
#endif

/******************************************************************************
 * Function:        static void SendSystemReset(void)
 *
 * PreCondition:    SPI or PSP bus must be initialized (done in MACInit()).
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        SendSystemReset reliably resets the Ethernet controller.  
 *					It resets all register contents (except for COCON bits of 
 *					ECON2) and returns the device to the power on default state.
 *					This function should be called instead of directly 
 *					attempting to perform a reset via the ECON2<ETHRST> bit.  
 *					If using the PSP, SendSystemReset also does basic checks to 
 *					look for unsoldered pins or solder bridges on the PSP pins.
 *
 * Note:            This function is a blocking function and will lock up the 
 *					application if a non-recoverable problem is present.  
 *					Possible non-recoverable problems include:
 *						- SPI module not configured correctly
 *						- PMP module not configured correctly
 *						- HardwareProfile pins not defined correctly
 *						- Solder bridge on SPI/PSP/PMP lines
 *						- Unsoldered pins on SPI/PSP/PMP lines
 *						- 25MHz Ethernet oscillator not running
 *						- Vdd lower than ENCX24J600 operating range
 *						- I/O levels out of range (for example if the PIC is at 
 *						  2V without level shifting)
 *						- Bus contention on SPI/PSP/PMP lines with other slaves
 *						- One or more Vdd or Vss pins are not connected.
 *****************************************************************************/
int SendSystemReset(void)
{
	// Power cycle the ENCx24J600 device, if any sort of POR pin is defined
	int f=0, g=0;
	//printf("%s() !!!\r\n", __FUNCTION__);
	#if defined(ENC100_POR_IO)
		ENC100_POR_IO = 0;
		ENC100_POR_TRIS = 0;
		DelayMs(2);
		ENC100_POR_IO = 1;
		vCurrentBank = 0;
		Delay10us(40);
	#endif
	
	// Perform a reset via the SPI/PSP interface
	do
	{
		// Set and clear a few bits that clears themselves upon reset.  
		// If EUDAST cannot be written to and your code gets stuck in this 
		// loop, you have a hardware problem of some sort (SPI or PMP not 
		// initialized correctly, I/O pins aren't connected or are 
		// shorted to something, power isn't available, etc.)
		do
		{
			WriteReg(EUDAST, 0x1234);
			f++;
		} while ((ReadReg(EUDAST) != 0x1234u) && (f<=15) );
		if (f>=15)		return 0;
		//printf("baca EUDAST %d: 0x1234h\r\n", f);

		// Issue a reset and wait for it to complete
		BFSReg(ECON2, ECON2_ETHRST);
		vCurrentBank = 0;
		while((ReadReg(ESTAT) & (ESTAT_CLKRDY | ESTAT_RSTDONE | ESTAT_PHYRDY)) != (ESTAT_CLKRDY | ESTAT_RSTDONE | ESTAT_PHYRDY));
		//Delay10us(3);
		vTaskDelay(1);

		// Check to see if the reset operation was successful by 
		// checking if EUDAST went back to its reset default.  This test 
		// should always pass, but certain special conditions might make 
		// this test fail, such as a PSP pin shorted to logic high.
		//g++;
	} while(ReadReg(EUDAST) != 0x0000u);
	//printf("baca EUDAST %d: 0x0000u\r\n", g);
	
	// Really ensure reset is done and give some time for power to be stable
	//DelayMs(1);
	vTaskDelay(1);


	// If using PSP, verify all address and data lines are working
	#if (ENC100_INTERFACE_MODE >= 1) && defined(ENC100_PSP_USE_INDIRECT_RAM_ADDRESSING)
	{
		BYTE i;
		WORD wTestWriteData, wTestReadData;
		
		// If we have a PSP interface, but are using indirect addressing, we 
		// can't use the ReadMemory()/WriteMemory() functions directly to access 
		// RAM.  Instead, lets do a simple test to verify all data lines only.
		
		// Check marching zero data pattern, then marching one data pattern
		wTestWriteData = 0xFFFE;
		for(i = 0; i < 34u; i++)
		{
			WriteReg(EHT1, wTestWriteData);
			wTestReadData = ReadReg(EHT1);
			
			// See if the data matches.  If your application gets stuck here, 
			// it means you have a hardware failure.  Check all of your PSP 
			// address and data lines.
			if(wTestWriteData != wTestReadData)
				while(1);
			
			// March the data over left by one bit
			wTestWriteData <<= 1;
			if(i == 16u)
				wTestWriteData = 0x0001;
			else if(i < 16u)
				wTestWriteData |= 0x0001;
		}	
	}
	#elif (ENC100_INTERFACE_MODE >= 1) // Parallel direct addressing
	{
		WORD w;
		WORD wTestWriteData, wTestReadData;
		
		// Initialize RAM contents with a random pattern and read back to verify
		// This step is critical if using a PSP interface since some functionality 
		// may appear to work while a solder bridge or disconnect will cause 
		// certain memory ranges to fail.

		// Generate and write random pattern
		srand(600);
		for(w = 0; w < ENC100_RAM_SIZE; w += sizeof(wTestWriteData))
		{
			wTestWriteData = rand() + rand();
			WriteMemory(w, (BYTE*)&wTestWriteData, sizeof(wTestWriteData));
		}
		
		// Read back and verify random pattern
		srand(600);
		for(w = 0; w < ENC100_RAM_SIZE; w += sizeof(wTestWriteData))
		{
			wTestWriteData = rand() + rand();
			ReadMemory(w, (BYTE*)&wTestReadData, sizeof(wTestReadData));
			
			// See if the data matches.  If your application gets stuck here, 
			// it means you have a hardware failure.  Check all of your PSP 
			// address and data lines.
			if(wTestWriteData != wTestReadData)
				while(1);
		}
	}
	#endif
	return 1;
}//end SendSystemReset


//#if ENC100_INTERFACE_MODE	// WriteMemory() is not currently needed in SPI mode
#if 1
/******************************************************************************
 * Function:        void WriteMemory(WORD wAddress, BYTE *vData, WORD wLength)
 *
 * PreCondition:    None
 *
 * Input:           wAddress: ENCX24J600 RAM or SFR address to write to
 *					*vData: Pointer to local PIC RAM which needs to be written 
 *							to the ENCX24J600
 *					wLength: Number of bytes to copy from vData to wAddress
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Copys 0 or more bytes to the ENCX24J600 RAM
 *
 * Note:            Can be used to access SFRs and ESFRs when using PSP
 *****************************************************************************/
void WriteMemory(WORD wAddress, BYTE *vData, WORD wLength)
{
	BOOL bISREnabled;

	if(wLength == 0u)
		return;

	#if defined(ENC100_ISR_ENABLE) && defined(ENC100_INT_TRIS)
		bISREnabled = ENC100_ISR_ENABLE;
		ENC100_ISR_ENABLE = 0;
	#endif

	#if !defined(ENC100_BIT_BANG_PMP) && ((ENC100_INTERFACE_MODE == 1) || (ENC100_INTERFACE_MODE == 2) || (ENC100_INTERFACE_MODE == 5) || (ENC100_INTERFACE_MODE == 6))
	{
		PMCONbits.PMPEN = 1;
		AssertChipSelect();
		while(wLength--)
		{
			PMADDR = wAddress++;	// NOTE: Performance might be improvable if you use PMP address increment
			while(PMMODEbits.BUSY);
			PMDIN1 = *vData++;
		}
		while(PMMODEbits.BUSY);
		DeassertChipSelect();
		PMCONbits.PMPEN = 0;
	}
	#elif !defined(ENC100_BIT_BANG_PMP) && ((ENC100_INTERFACE_MODE == 3) || (ENC100_INTERFACE_MODE == 4) || (ENC100_INTERFACE_MODE == 9) || (ENC100_INTERFACE_MODE == 10))
	{
		WORD wWORDAddress;
		volatile WORD wDummy;
		
		wWORDAddress = wAddress>>1;
		PMCONbits.PMPEN = 1;
		AssertChipSelect();
		if(wAddress & 0x1)		
		{
			PMADDR = wWORDAddress++;
			wDummy = PMDIN1;		// Can't write to a single byte address by itself, so we will do a read-modify-write
			wLength--;
			((BYTE*)&wDummy)[1] = *vData++;
			while(PMMODEbits.BUSY);
			((BYTE*)&wDummy)[0] = ((BYTE*)&PMDIN1)[0];
			while(PMMODEbits.BUSY);
			PMDIN1 = wDummy;
		}
		while(wLength >= 2u)
		{
			PMADDR = wWORDAddress++;	// NOTE: Performance might be improvable if you use PMP address increment
			wLength -= 2;
			((BYTE*)&wDummy)[0] = *vData++;
			((BYTE*)&wDummy)[1] = *vData++;
			while(PMMODEbits.BUSY);
			PMDIN1 = wDummy;
		}
		if(wLength)
		{
			PMADDR = wWORDAddress;
			wDummy = PMDIN1;		// Can't write to a single byte address by itself, so we will do a read-modify-write
			((BYTE*)&wDummy)[0] = *vData++;
			while(PMMODEbits.BUSY);
			((BYTE*)&wDummy)[1] = ((BYTE*)&PMDIN1)[1];
			while(PMMODEbits.BUSY);
			PMDIN1 = wDummy;
		}		
		while(PMMODEbits.BUSY);
		DeassertChipSelect();
		PMCONbits.PMPEN = 0;
	}
	#elif ENC100_INTERFACE_MODE == 1
	{
		ENC100_SET_ADDR_TRIS_OUT();
		ENC100_SET_AD_TRIS_OUT();
		AssertChipSelect();
		while(wLength--)
		{
			ENC100_SET_ADDR_IO(wAddress++);
			ENC100_SET_AD_IO(*vData++);
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			ENC100_SO_WR_B0SEL_EN_IO = 0;
		}
		DeassertChipSelect();
	}
	#elif ENC100_INTERFACE_MODE == 2
	{
		ENC100_SET_ADDR_TRIS_OUT();
		ENC100_SET_AD_TRIS_OUT();
		AssertChipSelect();
		ENC100_SI_RD_RW_IO = 0;
		while(wLength--)
		{
			ENC100_SET_ADDR_IO(wAddress++);
			ENC100_SET_AD_IO(*vData++);
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			ENC100_SO_WR_B0SEL_EN_IO = 0;
		}
		DeassertChipSelect();
	}
	#elif ENC100_INTERFACE_MODE == 3
	{
		WORD wData;
		
		if(wLength == 0u)
			return;
		
		ENC100_SET_AD_TRIS_OUT();
		AssertChipSelect();
		if(wAddress & 0x1)		// Write high byte to odd address, if not WORD aligned
		{
			ENC100_SET_ADDR_IO(wAddress>>1);
			((BYTE*)&wData)[1] = *vData++;
			ENC100_SET_AD_IO(wData);
			ENC100_WRH_B1SEL_IO = 1;
			ENC100_WRH_B1SEL_IO = 0;
			wAddress++;
			wLength--;
		}
		while(wLength >= 2u)		// Write all possible WORDs
		{
			ENC100_SET_ADDR_IO(wAddress>>1);
			((BYTE*)&wData)[0] = *vData++;
			((BYTE*)&wData)[1] = *vData++;
			ENC100_SET_AD_IO(wData);
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			ENC100_WRH_B1SEL_IO = 1;
			ENC100_SO_WR_B0SEL_EN_IO = 0;
			ENC100_WRH_B1SEL_IO = 0;
			wAddress += 2;
			wLength -= 2;
		}
		if(wLength)				// Write final byte to low byte address, if needed
		{
			ENC100_SET_ADDR_IO(wAddress>>1);
			((BYTE*)&wData)[0] = *vData++;
			ENC100_SET_AD_IO(wData);
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			ENC100_SO_WR_B0SEL_EN_IO = 0;
		}
		DeassertChipSelect();
	}
	#elif ENC100_INTERFACE_MODE == 4
	{
		WORD wData;
		
		if(wLength == 0u)
			return;
		
		ENC100_SET_AD_TRIS_OUT();
		AssertChipSelect();
		ENC100_SI_RD_RW_IO = 0;
		if(wAddress & 0x1)		// Write high byte to odd address, if not WORD aligned
		{
			ENC100_SET_ADDR_IO(wAddress>>1);
			((BYTE*)&wData)[1] = *vData++;
			ENC100_SET_AD_IO(wData);
			ENC100_WRH_B1SEL_IO = 1;
			ENC100_WRH_B1SEL_IO = 0;
			wAddress++;
			wLength--;
		}
		while(wLength >= 2u)		// Write all possible WORDs
		{
			ENC100_SET_ADDR_IO(wAddress>>1);
			((BYTE*)&wData)[0] = *vData++;
			((BYTE*)&wData)[1] = *vData++;
			ENC100_SET_AD_IO(wData);
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			ENC100_WRH_B1SEL_IO = 1;
			ENC100_SO_WR_B0SEL_EN_IO = 0;
			ENC100_WRH_B1SEL_IO = 0;
			wAddress += 2;
			wLength -= 2;
		}
		if(wLength)				// Write final byte to low byte address, if needed
		{
			ENC100_SET_ADDR_IO(wAddress>>1);
			((BYTE*)&wData)[0] = *vData++;
			ENC100_SET_AD_IO(wData);
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			ENC100_SO_WR_B0SEL_EN_IO = 0;
		}
		DeassertChipSelect();
	}
	#elif ENC100_INTERFACE_MODE == 5
	{
		ENC100_SET_AD_TRIS_OUT;
		AssertChipSelect();
		while(wLength--)
		{
			ENC100_SET_AD_IO(wAddress);
			ENC100_SCK_AL_IO = 1;
			ENC100_SCK_AL_IO = 0;
			ENC100_SET_AD_IO(*vData++);
			wAddress++;
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			ENC100_SO_WR_B0SEL_EN_IO = 0;
		}
		DeassertChipSelect();
	}
	#elif ENC100_INTERFACE_MODE == 6
	{
		ENC100_SET_AD_TRIS_OUT;
		AssertChipSelect();
		ENC100_SI_RD_RW_IO = 0;
		while(wLength--)
		{
			ENC100_SET_AD_IO(wAddress);
			ENC100_SCK_AL_IO = 1;
			ENC100_SCK_AL_IO = 0;
			ENC100_SET_AD_IO(*vData++);
			wAddress++;
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			ENC100_SO_WR_B0SEL_EN_IO = 0;
		}
		DeassertChipSelect();
	}
	#elif ENC100_INTERFACE_MODE == 9
	{
		WORD wData;
		
		if(wLength == 0u)
			return;
		
		ENC100_SET_AD_TRIS_OUT;
		AssertChipSelect();
		if(wAddress & 0x1)		// Write high byte to odd address, if not WORD aligned
		{
			ENC100_SET_AD_IO(wAddress>>1);
			ENC100_SCK_AL_IO = 1;
			DelaySetupHold();
			ENC100_SCK_AL_IO = 0;
			DelaySetupHold();
			((BYTE*)&wData)[1] = *vData++;
			ENC100_SET_AD_IO(wData);
			DelaySetupHold();
			ENC100_WRH_B1SEL_IO = 1;
			wAddress++;
			wLength--;
			DelaySetupHold();
			ENC100_WRH_B1SEL_IO = 0;
		}
		while(wLength >= 2u)		// Write all possible WORDs
		{
			ENC100_SET_AD_IO(wAddress>>1);
			ENC100_SCK_AL_IO = 1;
			DelaySetupHold();
			ENC100_SCK_AL_IO = 0;
			DelaySetupHold();
			((BYTE*)&wData)[0] = *vData++;
			((BYTE*)&wData)[1] = *vData++;
			ENC100_SET_AD_IO(wData);
			DelaySetupHold();
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			ENC100_WRH_B1SEL_IO = 1;
			wAddress += 2;
			wLength -= 2;
			DelaySetupHold();
			ENC100_SO_WR_B0SEL_EN_IO = 0;
			ENC100_WRH_B1SEL_IO = 0;
		}
		if(wLength)				// Write final byte to low byte address, if needed
		{
			ENC100_SET_AD_IO(wAddress>>1);
			ENC100_SCK_AL_IO = 1;
			DelaySetupHold();
			ENC100_SCK_AL_IO = 0;
			DelaySetupHold();
			((BYTE*)&wData)[0] = *vData++;
			ENC100_SET_AD_IO(wData);
			DelaySetupHold();
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			DelaySetupHold();
			ENC100_SO_WR_B0SEL_EN_IO = 0;
		}
		DeassertChipSelect();
	}
	#elif ENC100_INTERFACE_MODE == 10
	{
		WORD wData;
		
		if(wLength == 0u)
			return;
		
		ENC100_SET_AD_TRIS_OUT;
		AssertChipSelect();
		ENC100_SI_RD_RW_IO = 0;
		if(wAddress & 0x1)		// Write high byte to odd address, if not WORD aligned
		{
			ENC100_SET_AD_IO(wAddress>>1);
			ENC100_SCK_AL_IO = 1;
			DelaySetupHold();
			ENC100_SCK_AL_IO = 0;
			DelaySetupHold();
			((BYTE*)&wData)[1] = *vData++;
			ENC100_SET_AD_IO(wData);
			DelaySetupHold();
			ENC100_WRH_B1SEL_IO = 1;
			wAddress++;
			wLength--;
			DelaySetupHold();
			ENC100_WRH_B1SEL_IO = 0;
		}
		while(wLength >= 2u)		// Write all possible WORDs
		{
			ENC100_SET_AD_IO(wAddress>>1);
			ENC100_SCK_AL_IO = 1;
			DelaySetupHold();
			ENC100_SCK_AL_IO = 0;
			DelaySetupHold();
			((BYTE*)&wData)[0] = *vData++;
			((BYTE*)&wData)[1] = *vData++;
			ENC100_SET_AD_IO(wData);
			DelaySetupHold();
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			ENC100_WRH_B1SEL_IO = 1;
			wAddress += 2;
			wLength -= 2;
			DelaySetupHold();
			ENC100_SO_WR_B0SEL_EN_IO = 0;
			ENC100_WRH_B1SEL_IO = 0;
		}
		if(wLength)				// Write final byte to low byte address, if needed
		{
			ENC100_SET_AD_IO(wAddress>>1);
			ENC100_SCK_AL_IO = 1;
			DelaySetupHold();
			ENC100_SCK_AL_IO = 0;
			DelaySetupHold();
			((BYTE*)&wData)[0] = *vData++;
			ENC100_SET_AD_IO(wData);
			DelaySetupHold();
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			DelaySetupHold();
			ENC100_SO_WR_B0SEL_EN_IO = 0;
		}
		DeassertChipSelect();
	}
	#else	// SPI mode
	{
		WORD w;

		// Use RX window
		w = Execute2(RRXWRPT, 0x0000);
		Execute2(WRXWRPT, wAddress);
		WriteN(WBMRX, vData, wLength);
		Execute2(WRXWRPT, w);
	}
	#endif

	#if defined(ENC100_ISR_ENABLE) && defined(ENC100_INT_TRIS)
		ENC100_ISR_ENABLE = bISREnabled;
	#endif
}
#endif


/******************************************************************************
 * Function:        void WriteMemoryWindow(BYTE vWindow, BYTE *vData, WORD wLength)
 *
 * PreCondition:    None
 *
 * Input:           vWindow: UDA_WINDOW, GP_WINDOW, or RX_WINDOW corresponding 
 *							 to the window register to write to
 *					*vData: Pointer to local PIC RAM which contains the 
 *							source data
 *					wLength: Number of bytes to copy from vData to window
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Copys 0 or more bytes from CPU RAM to the ENCX24J600 
 *					Family RAM using one of the UDA, TX, or RX write window 
 *					pointers.  This pointer is incremented by the number of 
 *					bytes writen.
 *
 * Note:            None
 *****************************************************************************/
 void WriteMemoryWindow(BYTE vWindow, BYTE *vData, WORD wLength)
{
	BOOL bISREnabled;

	#if (ENC100_INTERFACE_MODE >= 1)	// Parallel mode
		WORD wAddress;
	#endif

	// If we are in the middle of a SPI/PSP read/write operation, an interrupt 
	// cannot be immediately processed (which would also require SPI/PSP 
	// read/write operations).  Therefore, we must disable the Ethernet 
	// interrupt temporarily to ensure this illegal reentrancy doesn't occur.
	#if defined(ENC100_ISR_ENABLE) && defined(ENC100_INT_TRIS)
		bISREnabled = ENC100_ISR_ENABLE;
		ENC100_ISR_ENABLE = 0;
	#endif

	// Decode the vWindow handle to the litteral SFR address to read/write from/to
	#if (ENC100_INTERFACE_MODE == 1) || (ENC100_INTERFACE_MODE == 2) || (ENC100_INTERFACE_MODE == 5) || (ENC100_INTERFACE_MODE == 6) || defined(ENC100_BIT_BANG_PMP)
		wAddress = EUDADATA;
		if(vWindow & GP_WINDOW)
			wAddress = EGPDATA;
		if(vWindow & RX_WINDOW)
			wAddress = ERXDATA;
	#elif (ENC100_INTERFACE_MODE == 3) || (ENC100_INTERFACE_MODE == 4) || (ENC100_INTERFACE_MODE == 9) || (ENC100_INTERFACE_MODE == 10)
		wAddress = (EUDADATA>>1);
		if(vWindow & GP_WINDOW)
			wAddress = (EGPDATA>>1);
		if(vWindow & RX_WINDOW)
			wAddress = (ERXDATA>>1);
	#endif

	#if (ENC100_INTERFACE_MODE >= 1) && !defined(ENC100_BIT_BANG_PMP)	// PMP under hardware control
	{
		PMCONbits.PMPEN = 1;
		AssertChipSelect();
		PMADDR = wAddress;
		while(wLength--)
		{
			while(PMMODEbits.BUSY);
			PMDIN1 = *vData++;
		}
		while(PMMODEbits.BUSY);
		DeassertChipSelect();
		PMCONbits.PMPEN = 0;
	}
	#elif ENC100_INTERFACE_MODE == 1	// Bit bang PMP
	{
		ENC100_SET_ADDR_TRIS_OUT();
		ENC100_SET_AD_TRIS_OUT();
		AssertChipSelect();
		ENC100_SET_ADDR_IO(wAddress);
		while(wLength--)
		{
			ENC100_SET_AD_IO(*vData++);
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			ENC100_SO_WR_B0SEL_EN_IO = 0;
		}
		DeassertChipSelect();
	}
	#elif ENC100_INTERFACE_MODE == 2		// Bit bang PMP
	{
		ENC100_SET_ADDR_TRIS_OUT();
		ENC100_SET_AD_TRIS_OUT();
		AssertChipSelect();
		ENC100_SI_RD_RW_IO = 0;
		ENC100_SET_ADDR_IO(wAddress);
		while(wLength--)
		{
			ENC100_SET_AD_IO(*vData++);
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			ENC100_SO_WR_B0SEL_EN_IO = 0;
		}
		DeassertChipSelect();
	}
	#elif ENC100_INTERFACE_MODE == 3		// Bit bang PMP
	{
		ENC100_SET_AD_TRIS_OUT();
		AssertChipSelect();
		ENC100_SET_ADDR_IO(wAddress>>1);
		while(wLength--)
		{
			ENC100_SET_AD_IO(*vData++);
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			ENC100_SO_WR_B0SEL_EN_IO = 0;
		}
		DeassertChipSelect();
	}
	#elif ENC100_INTERFACE_MODE == 4		// Bit bang PMP
	{
		ENC100_SET_AD_TRIS_OUT();
		AssertChipSelect();
		ENC100_SET_ADDR_IO(wAddress>>1);
		while(wLength--)
		{
			ENC100_SET_AD_IO(*vData++);
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			ENC100_SO_WR_B0SEL_EN_IO = 0;
		}
		DeassertChipSelect();
	}
	#elif ENC100_INTERFACE_MODE == 5		// Bit bang PMP
	{
		ENC100_SET_AD_TRIS_OUT;
		AssertChipSelect();
		ENC100_SET_AD_IO(wAddress);
		ENC100_SCK_AL_IO = 1;
		ENC100_SCK_AL_IO = 0;
		while(wLength--)
		{
			ENC100_SET_AD_IO(*vData++);
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			ENC100_SO_WR_B0SEL_EN_IO = 0;
		}
		DeassertChipSelect();
	}
	#elif ENC100_INTERFACE_MODE == 6		// Bit bang PMP
	{
		ENC100_SET_AD_TRIS_OUT;
		AssertChipSelect();
		ENC100_SI_RD_RW_IO = 0;
		ENC100_SET_AD_IO(wAddress);
		ENC100_SCK_AL_IO = 1;
		ENC100_SCK_AL_IO = 0;
		while(wLength--)
		{
			ENC100_SET_AD_IO(*vData++);
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			ENC100_SO_WR_B0SEL_EN_IO = 0;
		}
		DeassertChipSelect();
	}
	#elif ENC100_INTERFACE_MODE == 9		// Bit bang PMP
	{
		ENC100_SET_AD_TRIS_OUT;
		AssertChipSelect();
		ENC100_SET_AD_IO(wAddress>>1);
		DelaySetupHold();
		ENC100_SCK_AL_IO = 1;
		DelaySetupHold();
		ENC100_SCK_AL_IO = 0;
		DelaySetupHold();
		while(wLength--)
		{
			ENC100_SET_AD_IO(*vData++);
			DelaySetupHold();
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			DelaySetupHold();
			ENC100_SO_WR_B0SEL_EN_IO = 0;
		}
		DeassertChipSelect();
	}
	#elif ENC100_INTERFACE_MODE == 10		// Bit bang PMP
	{
		ENC100_SET_AD_TRIS_OUT;
		AssertChipSelect();
		ENC100_SI_RD_RW_IO = 0;
		ENC100_SET_AD_IO(wAddress>>1);
		DelaySetupHold();
		ENC100_SCK_AL_IO = 1;
		DelaySetupHold();
		ENC100_SCK_AL_IO = 0;
		DelaySetupHold();
		while(wLength--)
		{
			ENC100_SET_AD_IO(*vData++);
			DelaySetupHold();
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			DelaySetupHold();
			ENC100_SO_WR_B0SEL_EN_IO = 0;
		}
		DeassertChipSelect();
	}
	#else	// SPI mode
	{
		BYTE vOpcode;

		vOpcode = WBMUDA;
		if(vWindow & GP_WINDOW)
			vOpcode = WBMGP;
		if(vWindow & RX_WINDOW)
			vOpcode = WBMRX;

		WriteN(vOpcode, vData, wLength);
	}
	#endif

	#if defined(ENC100_ISR_ENABLE) && defined(ENC100_INT_TRIS)
		ENC100_ISR_ENABLE = bISREnabled;
	#endif
}


//#if ENC100_INTERFACE_MODE	// ReadMemory() is not currently needed in SPI mode
#if 1
/******************************************************************************
 * Function:        void ReadMemory(WORD wAddress, BYTE *vData, WORD wLength)
 *
 * PreCondition:    None
 *
 * Input:           wAddress: ENCX24J600 RAM or SFR address to read from
 *					*vData: Pointer to local PIC RAM which will be written 
 *							with data from the ENCX24J600.
 *					wLength: Number of bytes to copy from wAddress to vData
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Copys 0 or more bytes from the ENCX24J600 RAM
 *
 * Note:            Can be used to access SFRs and ESFRs when using PSP
 *****************************************************************************/
void ReadMemory(WORD wAddress, BYTE *vData, WORD wLength)
{
	BOOL bISREnabled;
	
	if(wLength == 0u)
		return;

	#if defined(ENC100_ISR_ENABLE) && defined(ENC100_INT_TRIS)
		bISREnabled = ENC100_ISR_ENABLE;
		ENC100_ISR_ENABLE = 0;
	#endif

	#if !defined(ENC100_BIT_BANG_PMP) && ((ENC100_INTERFACE_MODE == 1) || (ENC100_INTERFACE_MODE == 2) || (ENC100_INTERFACE_MODE == 5) || (ENC100_INTERFACE_MODE == 6))
	{
		volatile BYTE vDummy;

		PMCONbits.PMPEN = 1;
		AssertChipSelect();
		PMADDR = wAddress++;
		vDummy = PMDIN1;
		while(--wLength)
		{
			PMADDR = wAddress++;
			while(PMMODEbits.BUSY);
			*vData++ = PMDIN1;
		}
		while(PMMODEbits.BUSY);
		DeassertChipSelect();
		*vData = PMDIN1;
		PMCONbits.PMPEN = 0;
	}
	#elif !defined(ENC100_BIT_BANG_PMP) && ((ENC100_INTERFACE_MODE == 3) || (ENC100_INTERFACE_MODE == 4) || (ENC100_INTERFACE_MODE == 9) || (ENC100_INTERFACE_MODE == 10))
	{
		WORD wWORDAddress;
		volatile WORD wDummy;
	
		wWORDAddress = wAddress>>1;
		PMCONbits.PMPEN = 1;
		AssertChipSelect();
		PMADDR = wWORDAddress++;
		wDummy = PMDIN1;
		if(wAddress & 0x1)
		{
			PMADDR = wWORDAddress++;
			wLength--;
			while(PMMODEbits.BUSY);
			*vData++ = ((BYTE*)&PMDIN1)[1];
		}
		while(wLength >= 2)
		{
			PMADDR = wWORDAddress++;
			wLength -= 2;
			while(PMMODEbits.BUSY);
			wDummy = PMDIN1;
			*vData++ = ((BYTE*)&wDummy)[0];
			*vData++ = ((BYTE*)&wDummy)[1];
		}
		if(wLength)
		{
			while(PMMODEbits.BUSY);
			*vData = ((BYTE*)&PMDIN1)[0];
		}
		DeassertChipSelect();
		PMCONbits.PMPEN = 0;
	}
	#elif ENC100_INTERFACE_MODE == 1
	{
		ENC100_SET_ADDR_TRIS_OUT();
		ENC100_SET_AD_TRIS_IN();
		AssertChipSelect();
		while(wLength--)
		{
			ENC100_SET_ADDR_IO(wAddress);
			ENC100_SI_RD_RW_IO = 1;
			wAddress++;
			DelaySetupHold();
			*vData++ = ENC100_GET_AD_IO();
			ENC100_SI_RD_RW_IO = 0;
		}
		DeassertChipSelect();
	}
	#elif ENC100_INTERFACE_MODE == 2
	{
		ENC100_SET_ADDR_TRIS_OUT();
		ENC100_SET_AD_TRIS_IN();
		AssertChipSelect();
		ENC100_SI_RD_RW_IO = 1;
		while(wLength--)
		{
			ENC100_SET_ADDR_IO(wAddress);
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			wAddress++;
			DelaySetupHold();
			*vData++ = ENC100_GET_AD_IO();
			ENC100_SO_WR_B0SEL_EN_IO = 0;
		}
		DeassertChipSelect();
	}
	#elif ENC100_INTERFACE_MODE == 3
	{
		if(wLength == 0u)
			return;
	
		ENC100_SET_AD_TRIS_IN();
		AssertChipSelect();
		if(wAddress & 0x1)		// Read from high byte if not WORD aligned
		{
			ENC100_SET_ADDR_IO(wAddress>>1);
			ENC100_SI_RD_RW_IO = 1;
			DelaySetupHold();
			wAddress++;
			wLength--;
			*vData++ = ENC100_GET_AD_IOH();
			ENC100_SI_RD_RW_IO = 0;
		}
		while(wLength >= 2u)	// Read all necessary WORDs
		{
			ENC100_SET_ADDR_IO(wAddress>>1);
			ENC100_SI_RD_RW_IO = 1;
			DelaySetupHold();
			wLength -= 2;
			wAddress += 2;
			*vData++ = ENC100_GET_AD_IOL();
			*vData++ = ENC100_GET_AD_IOH();
			ENC100_SI_RD_RW_IO = 0;			
		}
		if(wLength)				// Read final low byte, if needed
		{
			ENC100_SET_ADDR_IO(wAddress>>1);
			ENC100_SI_RD_RW_IO = 1;
			DelaySetupHold();
			*vData = ENC100_GET_AD_IOL();
			ENC100_SI_RD_RW_IO = 0;			
		}
		DeassertChipSelect();
	}
	#elif ENC100_INTERFACE_MODE == 4
	{
		if(wLength == 0u)
			return;
	
		ENC100_SET_AD_TRIS_IN();
		AssertChipSelect();
		ENC100_SI_RD_RW_IO = 1;
		if(wAddress & 0x1)		// Read from high byte if not WORD aligned
		{
			ENC100_SET_ADDR_IO(wAddress>>1);
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			DelaySetupHold();
			wAddress++;
			wLength--;
			*vData++ = ENC100_GET_AD_IOH();
			ENC100_SO_WR_B0SEL_EN_IO = 0;
		}
		while(wLength >= 2u)	// Read all necessary WORDs
		{
			ENC100_SET_ADDR_IO(wAddress>>1);
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			DelaySetupHold();
			wLength -= 2;
			wAddress += 2;
			*vData++ = ENC100_GET_AD_IOL();
			*vData++ = ENC100_GET_AD_IOH();
			ENC100_SO_WR_B0SEL_EN_IO = 0;
		}
		if(wLength)				// Read final low byte, if needed
		{
			ENC100_SET_ADDR_IO(wAddress>>1);
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			DelaySetupHold();
			*vData = ENC100_GET_AD_IOL();
			ENC100_SO_WR_B0SEL_EN_IO = 0;
		}

		DeassertChipSelect();
	}	
	#elif ENC100_INTERFACE_MODE == 5
	{
		AssertChipSelect();
		while(wLength--)
		{
			ENC100_SET_AD_TRIS_OUT;
			ENC100_SET_AD_IO(wAddress);
			ENC100_SCK_AL_IO = 1;
			ENC100_SCK_AL_IO = 0;
			ENC100_SET_AD_TRIS_IN;
			ENC100_SI_RD_RW_IO = 1;
			DelaySetupHold();
			wAddress++;
			*vData++ = ENC100_GET_AD_IO();
			ENC100_SI_RD_RW_IO = 0;		
		}
		DeassertChipSelect();
	}	
	#elif ENC100_INTERFACE_MODE == 6
	{
		AssertChipSelect();
		ENC100_SI_RD_RW_IO = 1;
		while(wLength--)
		{
			ENC100_SET_AD_TRIS_OUT;
			ENC100_SET_AD_IO(wAddress);
			ENC100_SCK_AL_IO = 1;
			ENC100_SCK_AL_IO = 0;
			ENC100_SET_AD_TRIS_IN;
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			DelaySetupHold();
			wAddress++;
			*vData++ = ENC100_GET_AD_IO();
			ENC100_SO_WR_B0SEL_EN_IO = 0;		
		}
		DeassertChipSelect();
	}	
	#elif ENC100_INTERFACE_MODE == 9
	{
		if(wLength == 0u)
			return;
	
		AssertChipSelect();
		if(wAddress & 0x1)		// Read from high byte if not WORD aligned
		{
			ENC100_SET_AD_TRIS_OUT;
			ENC100_SET_AD_IO(wAddress>>1);
			ENC100_SCK_AL_IO = 1;
			DelaySetupHold();
			ENC100_SCK_AL_IO = 0;
			DelaySetupHold();
			ENC100_SET_AD_TRIS_IN;
			ENC100_SI_RD_RW_IO = 1;
			DelaySetupHold();
			wAddress++;
			wLength--;
			*vData++ = ((BYTE*)&ENC100_GET_AD_IO())[1];
			ENC100_SI_RD_RW_IO = 0;			
		}
		while(wLength >= 2u)	// Read all necessary WORDs
		{
			ENC100_SET_AD_TRIS_OUT;
			ENC100_SET_AD_IO(wAddress>>1);
			ENC100_SCK_AL_IO = 1;
			DelaySetupHold();
			ENC100_SCK_AL_IO = 0;
			DelaySetupHold();
			ENC100_SET_AD_TRIS_IN;
			ENC100_SI_RD_RW_IO = 1;
			DelaySetupHold();
			wAddress += 2;
			wLength -= 2;
			*vData++ = ((BYTE*)&ENC100_GET_AD_IO())[0];
			*vData++ = ((BYTE*)&ENC100_GET_AD_IO())[1];
			ENC100_SI_RD_RW_IO = 0;		
		}
		if(wLength)				// Read final low byte, if needed
		{
			ENC100_SET_AD_TRIS_OUT;
			ENC100_SET_AD_IO(wAddress>>1);
			ENC100_SCK_AL_IO = 1;
			DelaySetupHold();
			ENC100_SCK_AL_IO = 0;
			DelaySetupHold();
			ENC100_SET_AD_TRIS_IN;
			ENC100_SI_RD_RW_IO = 1;
			DelaySetupHold();
			*vData++ = ((BYTE*)&ENC100_GET_AD_IO())[0];
			ENC100_SI_RD_RW_IO = 0;			
		}
		DeassertChipSelect();
	}	
	#elif ENC100_INTERFACE_MODE == 10
	{
		if(wLength == 0u)
			return;
	
		AssertChipSelect();
		ENC100_SI_RD_RW_IO = 1;
		if(wAddress & 0x1)		// Read from high byte if not WORD aligned
		{
			ENC100_SET_AD_TRIS_OUT;
			ENC100_SET_AD_IO(wAddress>>1);
			ENC100_SCK_AL_IO = 1;
			DelaySetupHold();
			ENC100_SCK_AL_IO = 0;
			DelaySetupHold();
			ENC100_SET_AD_TRIS_IN;
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			DelaySetupHold();
			wAddress++;
			wLength--;
			*vData++ = ((BYTE*)&ENC100_GET_AD_IO())[1];
			ENC100_SO_WR_B0SEL_EN_IO = 0;			
		}
		while(wLength >= 2u)	// Read all necessary WORDs
		{
			ENC100_SET_AD_TRIS_OUT;
			ENC100_SET_AD_IO(wAddress>>1);
			ENC100_SCK_AL_IO = 1;
			DelaySetupHold();
			ENC100_SCK_AL_IO = 0;
			DelaySetupHold();
			ENC100_SET_AD_TRIS_IN;
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			DelaySetupHold();
			wAddress += 2;
			wLength -= 2;
			*vData++ = ((BYTE*)&ENC100_GET_AD_IO())[0];
			*vData++ = ((BYTE*)&ENC100_GET_AD_IO())[1];
			ENC100_SO_WR_B0SEL_EN_IO = 0;		
		}
		if(wLength)				// Read final low byte, if needed
		{
			ENC100_SET_AD_TRIS_OUT;
			ENC100_SET_AD_IO(wAddress>>1);
			ENC100_SCK_AL_IO = 1;
			DelaySetupHold();
			ENC100_SCK_AL_IO = 0;
			DelaySetupHold();
			ENC100_SET_AD_TRIS_IN;
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			DelaySetupHold();
			*vData++ = ((BYTE*)&ENC100_GET_AD_IO())[0];
			ENC100_SO_WR_B0SEL_EN_IO = 0;			
		}
		DeassertChipSelect();
	}	
	#else // SPI mode
	{
		WORD w;

		// Use RX pointer
		w = Execute2(RRXRDPT, 0x0000);
		Execute2(WRXRDPT, wAddress);
		ReadN(RBMRX, vData, wLength);
		Execute2(WRXRDPT, w);
	}	
	#endif

	#if defined(ENC100_ISR_ENABLE) && defined(ENC100_INT_TRIS)
		ENC100_ISR_ENABLE = bISREnabled;
	#endif
}
#endif

/******************************************************************************
 * Function:        void ReadMemoryWindow(BYTE vWindow, BYTE *vData, WORD wLength)
 *
 * PreCondition:    None
 *
 * Input:           vWindow: UDA_WINDOW, GP_WINDOW, or RX_WINDOW corresponding 
 *							 to the window register to read from
 *					*vData: Pointer to local PIC RAM which will be written 
 *							with data from the ENC624J600 Family.
 *					wLength: Number of bytes to copy from window to vData
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Copys 0 or more bytes from the ENC624J600 Family RAM using 
 *					one of the UDA, TX, or RX read window pointers.  This 
 *					pointer is incremented by the number of bytes read.  
 *					However, if using a 16-bit parallel interface, the pointer 
 *					will be incremented by 1 extra if the length parameter is 
 *					odd to ensure 16-bit alignment.
 *
 * Note:            None
 *****************************************************************************/
void ReadMemoryWindow(BYTE vWindow, BYTE *vData, WORD wLength)
{
	BOOL bISREnabled;
	
	#if (ENC100_INTERFACE_MODE >= 1)	// Parallel mode
		WORD wAddress;
	#endif

	if(wLength == 0u)
		return;

	#if defined(ENC100_ISR_ENABLE) && defined(ENC100_INT_TRIS)
		bISREnabled = ENC100_ISR_ENABLE;
		ENC100_ISR_ENABLE = 0;
	#endif
	
	#if (ENC100_INTERFACE_MODE == 1) || (ENC100_INTERFACE_MODE == 2) || (ENC100_INTERFACE_MODE == 5) || (ENC100_INTERFACE_MODE == 6) || defined(ENC100_BIT_BANG_PMP)
		wAddress = EUDADATA;
		if(vWindow & GP_WINDOW)
			wAddress = EGPDATA;
		if(vWindow & RX_WINDOW)
			wAddress = ERXDATA;
	#elif (ENC100_INTERFACE_MODE == 3) || (ENC100_INTERFACE_MODE == 4) || (ENC100_INTERFACE_MODE == 9) || (ENC100_INTERFACE_MODE == 10)
		wAddress = (EUDADATA>>1);
		if(vWindow & GP_WINDOW)
			wAddress = (EGPDATA>>1);
		if(vWindow & RX_WINDOW)
			wAddress = (ERXDATA>>1);
	#endif
	
	#if (ENC100_INTERFACE_MODE >= 1) && !defined(ENC100_BIT_BANG_PMP)
	{
		volatile BYTE vDummy;

		PMCONbits.PMPEN = 1;
		AssertChipSelect();
		PMADDR = wAddress;
		vDummy = PMDIN1;
		while(--wLength)
		{
			while(PMMODEbits.BUSY);
			*vData++ = PMDIN1;
		}
		while(PMMODEbits.BUSY);
		DeassertChipSelect();
		PMCONbits.PMPEN = 0;
		*vData = PMDIN1;
	}
	#elif ENC100_INTERFACE_MODE == 1
		ENC100_SET_ADDR_TRIS_OUT();
		ENC100_SET_AD_TRIS_IN();
		AssertChipSelect();
		ENC100_SET_ADDR_IO(wAddress);
		while(wLength--)
		{
			ENC100_SI_RD_RW_IO = 1;
			DelaySetupHold();
			*vData++ = ENC100_GET_AD_IO();
			ENC100_SI_RD_RW_IO = 0;			
		}
		DeassertChipSelect();
	#elif ENC100_INTERFACE_MODE == 2
		ENC100_SET_ADDR_TRIS_OUT();
		ENC100_SET_AD_TRIS_IN();
		AssertChipSelect();
		ENC100_SI_RD_RW_IO = 1;
		ENC100_SET_ADDR_IO(wAddress);
		while(wLength--)
		{
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			DelaySetupHold();
			*vData++ = ENC100_GET_AD_IO();
			ENC100_SO_WR_B0SEL_EN_IO = 0;
		}
		DeassertChipSelect();
	#elif ENC100_INTERFACE_MODE == 3
		ENC100_SET_AD_TRIS_IN();
		AssertChipSelect();
		ENC100_SET_ADDR_IO(wAddress>>1);
		while(wLength--)
		{
			ENC100_SI_RD_RW_IO = 1;
			DelaySetupHold();
			*vData++ = ENC100_GET_AD_IOL();
			ENC100_SI_RD_RW_IO = 0;	
		}
		DeassertChipSelect();
	#elif ENC100_INTERFACE_MODE == 4
		ENC100_SET_AD_TRIS_IN();
		AssertChipSelect();
		ENC100_SI_RD_RW_IO = 1;
		ENC100_SET_ADDR_IO(wAddress>>1);
		while(wLength--)
		{
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			DelaySetupHold();
			*vData++ = ENC100_GET_AD_IOL();
			ENC100_SO_WR_B0SEL_EN_IO = 0;
		}
		DeassertChipSelect();
	#elif ENC100_INTERFACE_MODE == 5
		AssertChipSelect();
		ENC100_SET_AD_TRIS_OUT;
		ENC100_SET_AD_IO(wAddress);
		ENC100_SCK_AL_IO = 1;
		ENC100_SCK_AL_IO = 0;
		ENC100_SET_AD_TRIS_IN;
		while(wLength--)
		{
			ENC100_SI_RD_RW_IO = 1;
			DelaySetupHold();
			*vData++ = ENC100_GET_AD_IO();
			ENC100_SI_RD_RW_IO = 0;			
		}
		DeassertChipSelect();
	#elif ENC100_INTERFACE_MODE == 6
		AssertChipSelect();
		ENC100_SI_RD_RW_IO = 1;
		ENC100_SET_AD_TRIS_OUT;
		ENC100_SET_AD_IO(wAddress);
		ENC100_SCK_AL_IO = 1;
		ENC100_SCK_AL_IO = 0;
		ENC100_SET_AD_TRIS_IN;
		while(wLength--)
		{
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			DelaySetupHold();
			*vData++ = ENC100_GET_AD_IO();
			ENC100_SO_WR_B0SEL_EN_IO = 0;			
		}
		DeassertChipSelect();
	#elif ENC100_INTERFACE_MODE == 9
		AssertChipSelect();
		ENC100_SET_AD_TRIS_OUT;
		ENC100_SET_AD_IO(wAddress>>1);
		ENC100_SCK_AL_IO = 1;
		DelaySetupHold();
		ENC100_SCK_AL_IO = 0;
		DelaySetupHold();
		ENC100_SET_AD_TRIS_IN;
		while(wLength--)
		{
			ENC100_SI_RD_RW_IO = 1;
			DelaySetupHold();
			*vData++ = ((BYTE*)&ENC100_GET_AD_IO())[0];
			ENC100_SI_RD_RW_IO = 0;			
		}
		DeassertChipSelect();
	#elif ENC100_INTERFACE_MODE == 10
		AssertChipSelect();
		ENC100_SI_RD_RW_IO = 1;
		ENC100_SET_AD_TRIS_OUT;
		ENC100_SET_AD_IO(wAddress>>1);
		ENC100_SCK_AL_IO = 1;
		DelaySetupHold();
		ENC100_SCK_AL_IO = 0;
		DelaySetupHold();
		ENC100_SET_AD_TRIS_IN;
		while(wLength--)
		{
			ENC100_SO_WR_B0SEL_EN_IO = 1;
			DelaySetupHold();
			*vData++ = ((BYTE*)&ENC100_GET_AD_IO())[0];
			ENC100_SO_WR_B0SEL_EN_IO = 0;			
		}
		DeassertChipSelect();
	#else // SPI mode
	{
		BYTE vOpcode;

		vOpcode = RBMUDA;
		if(vWindow & GP_WINDOW)
			vOpcode = RBMGP;
		if(vWindow & RX_WINDOW)
			vOpcode = RBMRX;

		ReadN(vOpcode, vData, wLength);
	}
	#endif

	#if defined(ENC100_ISR_ENABLE) && defined(ENC100_INT_TRIS)
		ENC100_ISR_ENABLE = bISREnabled;
	#endif
}



#if (ENC100_INTERFACE_MODE == 0)	// These are SPI only functions
/******************************************************************************
 * Function:        void Execute0(BYTE vOpcode)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Sends a single byte command with no parameters
 *
 * Note:            None
 *****************************************************************************/
static void Execute0(BYTE vOpcode)
{
	volatile BYTE vDummy;
	BOOL bISREnabled;

	#if defined(ENC100_ISR_ENABLE) && defined(ENC100_INT_TRIS)
		bISREnabled = ENC100_ISR_ENABLE;
		ENC100_ISR_ENABLE = 0;
	#endif

	AssertChipSelect();
	spiPut(vOpcode);
	/*
	ClearSPIDoneFlag();
	ENC100_SSPBUF = vOpcode;	// Send the command/opcode
	WaitForDataByte();
	vDummy = ENC100_SSPBUF;
	//*/
	
	
	DeassertChipSelect();

	#if defined(ENC100_ISR_ENABLE) && defined(ENC100_INT_TRIS)
		ENC100_ISR_ENABLE = bISREnabled;
	#endif
}//end Execute0

// This Execute1 function is not currently used
//static BYTE Execute1(BYTE vOpcode, BYTE vData)
//{
//	volatile BYTE vReturn;
//	BOOL bISREnabled;
//
//	#if defined(ENC100_ISR_ENABLE) && defined(ENC100_INT_TRIS)
//		bISREnabled = ENC100_ISR_ENABLE;
//		ENC100_ISR_ENABLE = 0;
//	#endif
//
//	AssertChipSelect();
//	ClearSPIDoneFlag();
//	ENC100_SSPBUF = vOpcode;	// Send the command/opcode
//	WaitForDataByte();
//	vReturn = ENC100_SSPBUF;
//	ENC100_SSPBUF = vData;		// Send the data
//	WaitForDataByte();
//	vReturn = ENC100_SSPBUF;
//	DeassertChipSelect();
//
//	#if defined(ENC100_ISR_ENABLE) && defined(ENC100_INT_TRIS)
//		ENC100_ISR_ENABLE = bISREnabled;
//	#endif
//
//	return vReturn;
//}//end Execute1

static WORD Execute2(BYTE vOpcode, WORD wData)
{
	volatile WORD wReturn;
	BOOL bISREnabled;

	#if defined(ENC100_ISR_ENABLE) && defined(ENC100_INT_TRIS)
		bISREnabled = ENC100_ISR_ENABLE;
		ENC100_ISR_ENABLE = 0;
	#endif

	AssertChipSelect();
	/*
	ClearSPIDoneFlag();
	ENC100_SSPBUF = vOpcode;			// Send the command/opcode
	WaitForDataByte();
	((BYTE*)&wReturn)[0] = ENC100_SSPBUF;
	ENC100_SSPBUF = ((BYTE*)&wData)[0];	// Send low byte of data
	WaitForDataByte();
	((BYTE*)&wReturn)[0] = ENC100_SSPBUF;
	ENC100_SSPBUF = ((BYTE*)&wData)[1];	// Send high byte of data
	WaitForDataByte();
	((BYTE*)&wReturn)[1] = ENC100_SSPBUF;
	//*/
	
	spiPut(vOpcode);
	((BYTE*)&wReturn)[0] = spiPut(((BYTE*)&wData)[0]);
	((BYTE*)&wReturn)[1] = spiPut(((BYTE*)&wData)[1]);
	
	
	DeassertChipSelect();

	#if defined(ENC100_ISR_ENABLE) && defined(ENC100_INT_TRIS)
		ENC100_ISR_ENABLE = bISREnabled;
	#endif

	return wReturn;
}//end Execute2

static DWORD Execute3(BYTE vOpcode, DWORD dwData)
{
	volatile DWORD dwReturn;
	BOOL bISREnabled;

	#if defined(ENC100_ISR_ENABLE) && defined(ENC100_INT_TRIS)
		bISREnabled = ENC100_ISR_ENABLE;
		ENC100_ISR_ENABLE = 0;
	#endif

	AssertChipSelect();
	/*
	ClearSPIDoneFlag();
	ENC100_SSPBUF = vOpcode;				// Send the command/opcode
	((BYTE*)&dwReturn)[3] = 0x00;
	WaitForDataByte();
	((BYTE*)&dwReturn)[0] = ENC100_SSPBUF;
	ENC100_SSPBUF = ((BYTE*)&dwData)[0];	// Send byte 0 of data
	WaitForDataByte();
	((BYTE*)&dwReturn)[0] = ENC100_SSPBUF;
	ENC100_SSPBUF = ((BYTE*)&dwData)[1];	// Send byte 1 of data
	WaitForDataByte();
	((BYTE*)&dwReturn)[1] = ENC100_SSPBUF;
	ENC100_SSPBUF = ((BYTE*)&dwData)[2];	// Send byte 2 of data
	WaitForDataByte();
	((BYTE*)&dwReturn)[2] = ENC100_SSPBUF;
	//*/
	
	((BYTE*)&dwReturn)[3] = 0x00;
	spiPut(vOpcode);
	((BYTE*)&dwReturn)[0] = spiPut(((BYTE*)&dwData)[0]);
	((BYTE*)&dwReturn)[1] = spiPut(((BYTE*)&dwData)[1]);
	((BYTE*)&dwReturn)[2] = spiPut(((BYTE*)&dwData)[2]);
	
	DeassertChipSelect();

	#if defined(ENC100_ISR_ENABLE) && defined(ENC100_INT_TRIS)
		ENC100_ISR_ENABLE = bISREnabled;
	#endif

	return dwReturn;
}//end Execute2

static void ReadN(BYTE vOpcode, BYTE* vData, WORD wDataLen)
{
	volatile BYTE vDummy;
	BOOL bISREnabled;

	#if defined(ENC100_ISR_ENABLE) && defined(ENC100_INT_TRIS)
		bISREnabled = ENC100_ISR_ENABLE;
		ENC100_ISR_ENABLE = 0;
	#endif

	AssertChipSelect();
	/*
	ClearSPIDoneFlag();
	ENC100_SSPBUF = vOpcode;				// Send the command/opcode
	WaitForDataByte();
	vDummy = ENC100_SSPBUF;
	while(wDataLen--)
	{
		ENC100_SSPBUF = 0x00;
		WaitForDataByte();
		*vData++ = ENC100_SSPBUF;
	}
	//*/
	int dd=0;
	spiPut(vOpcode);
	while(wDataLen--)
	{
		#ifdef DEBUG_ETH
		*vData = spiPut(0x00);
		
		if (dd==16) { 
			dd=0;
			printf("\r\n");
		}
		printf("%02x ", *vData++);
		dd++;
		printf("\r\n");
		//*vData = spiPut(0x00);
		//vData++;
		#else
			*vData++ = spiPut(0x00);
		#endif
	}
	
	DeassertChipSelect();

	

	#if defined(ENC100_ISR_ENABLE) && defined(ENC100_INT_TRIS)
		ENC100_ISR_ENABLE = bISREnabled;
	#endif
}

static void WriteN(BYTE vOpcode, BYTE* vData, WORD wDataLen)
{
	volatile BYTE vDummy;
	BOOL bISREnabled;

	#if defined(ENC100_ISR_ENABLE) && defined(ENC100_INT_TRIS)
		bISREnabled = ENC100_ISR_ENABLE;
		ENC100_ISR_ENABLE = 0;
	#endif

	AssertChipSelect();
	/*
	ClearSPIDoneFlag();
	ENC100_SSPBUF = vOpcode;				// Send the command/opcode
	WaitForDataByte();
	vDummy = ENC100_SSPBUF;

	while(wDataLen--)
	{
		ENC100_SSPBUF = *vData++;
		WaitForDataByte();
		vDummy = ENC100_SSPBUF;
	}
	//*/
	
	spiPut(vOpcode);
	while(wDataLen--)
	{
		vDummy = spiPut(*vData++);
	}
	
	DeassertChipSelect();

	#if defined(ENC100_ISR_ENABLE) && defined(ENC100_INT_TRIS)
		ENC100_ISR_ENABLE = bISREnabled;
	#endif
}

#endif

/******************************************************************************
 * Function:        WORD ReadReg(WORD wAddress)
 *
 * PreCondition:    SPI/PSP bus must be initialized (done in MACInit()).
 *
 * Input:           wAddress: Address of SFR register to read from.  
 *							  The LSb is ignored and treated as '0' always.
 *
 * Output:          WORD value of register contents
 *
 * Side Effects:    None
 *
 * Overview:        Selects the correct bank (if needed), and reads the 
 *					corresponding 16-bit register
 *
 * Note:            This routine cannot be used to read PHY registers.  
 *					Use the ReadPHYReg() function to read from PHY registers.
 *****************************************************************************/
static WORD ReadReg(WORD wAddress)
{
	#if (ENC100_INTERFACE_MODE >= 1)	// Parallel mode
	{
		WORD w;
		
		ReadMemory(wAddress, (BYTE*)&w, 2);
		return w;
	}
	#else	// SPI mode
	{
		WORD w;
		BYTE vBank;
		
		// See if we need to change register banks
		vBank = ((BYTE)wAddress) & 0xE0;
		if(vBank <= (0x3u<<5))
		{
			if(vBank != vCurrentBank)
			{
				if(vBank == (0x0u<<5))
					Execute0(B0SEL);
				else if(vBank == (0x1u<<5))
					Execute0(B1SEL);
				else if(vBank == (0x2u<<5))
					Execute0(B2SEL);
				else if(vBank == (0x3u<<5))
					Execute0(B3SEL);
					
				vCurrentBank = vBank;
			}
		
			w = Execute2(RCR | (wAddress & 0x1F), 0x0000);
		}
		else
		{
			DWORD dw = Execute3(RCRU, (DWORD)wAddress);
			((BYTE*)&w)[0] = ((BYTE*)&dw)[1];
			((BYTE*)&w)[1] = ((BYTE*)&dw)[2];
		}

		return w;
	}
	#endif
}//end ReadReg


/******************************************************************************
 * Function:        void WriteReg(WORD wAddress, WORD wValue)
 *
 * PreCondition:    SPI/PSP bus must be initialized (done in MACInit()).
 *
 * Input:           wAddress: Address of the SFR register to write to.  
 *					16-bit WORD to be written into the register.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Selects the correct bank (if using the SPI and needed), and 
 *					writes the corresponding 16-bit register with wValue.
 *
 * Note:            This routine cannot write to PHY registers.  Use the 
 *					WritePHYReg() function for writing to PHY registers.
 *****************************************************************************/
static void WriteReg(WORD wAddress, WORD wValue)
{
	#if (ENC100_INTERFACE_MODE >= 1)	// Parallel mode
	{
		WriteMemory(wAddress, (BYTE*)&wValue, 2);
	}
	#else	// SPI mode
	{
		BYTE vBank;
		
		// See if we need to change register banks
		vBank = ((BYTE)wAddress) & 0xE0;
		if(vBank <= (0x3u<<5))
		{
			if(vBank != vCurrentBank)
			{
				if(vBank == (0x0u<<5))
					Execute0(B0SEL);
				else if(vBank == (0x1u<<5))
					Execute0(B1SEL);
				else if(vBank == (0x2u<<5))
					Execute0(B2SEL);
				else if(vBank == (0x3u<<5))
					Execute0(B3SEL);
	
				vCurrentBank = vBank;
			}

			Execute2(WCR | (wAddress & 0x1F), wValue);
		}
		else
		{
			DWORD dw;
			((BYTE*)&dw)[0] = (BYTE)wAddress;
			((BYTE*)&dw)[1] = ((BYTE*)&wValue)[0];
			((BYTE*)&dw)[2] = ((BYTE*)&wValue)[1];
			Execute3(WCRU, dw);
		}
			
	}
	#endif
}//end WriteReg


/******************************************************************************
 * Function:        void BFSReg(WORD wAddress, WORD wBitMask)
 *					void BFCReg(WORD wAddress, WORD wBitMask)
 *
 * PreCondition:    None
 *
 * Input:           7 bit address of the register to write to.  
 *					16-bit WORD bitmask to set/clear in the register.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Sets/clears bits in Ethernet SFR registers
 *
 * Note:            These functions cannot be used to access ESFR registers.
 *****************************************************************************/
static void BFSReg(WORD wAddress, WORD wBitMask)
{
	#if (ENC100_INTERFACE_MODE >= 1)	// Parallel mode
	{
		WriteMemory(wAddress+SET_OFFSET, (BYTE*)&wBitMask, 2);
	}
	#else	// SPI mode
	{
		BYTE vBank;
		
		// See if we need to change register banks
		vBank = ((BYTE)wAddress) & 0xE0;
		if(vBank != vCurrentBank)
		{
			if(vBank == (0x0u<<5))
				Execute0(B0SEL);
			else if(vBank == (0x1u<<5))
				Execute0(B1SEL);
			else if(vBank == (0x2u<<5))
				Execute0(B2SEL);
			else if(vBank == (0x3u<<5))
				Execute0(B3SEL);

			vCurrentBank = vBank;
		}
	
		Execute2(BFS | (wAddress & 0x1F), wBitMask);
	}
	#endif
}//end BFSReg

static void BFCReg(WORD wAddress, WORD wBitMask)
{
	#if (ENC100_INTERFACE_MODE >= 1)	// Parallel mode
	{
		WriteMemory(wAddress+CLR_OFFSET, (BYTE*)&wBitMask, 2);
	}
	#else	// SPI mode
	{
		BYTE vBank;
		
		// See if we need to change register banks
		vBank = ((BYTE)wAddress) & 0xE0;
		if(vBank != vCurrentBank)
		{
			if(vBank == (0x0u<<5))
				Execute0(B0SEL);
			else if(vBank == (0x1u<<5))
				Execute0(B1SEL);
			else if(vBank == (0x2u<<5))
				Execute0(B2SEL);
			else if(vBank == (0x3u<<5))
				Execute0(B3SEL);

			vCurrentBank = vBank;
		}
	
		Execute2(BFC | (wAddress & 0x1F), wBitMask);
	}
	#endif
}//end BFCReg


/******************************************************************************
 * Function:        WORD ReadPHYReg(BYTE Register)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           Address of the PHY register to read from.
 *
 * Output:          16 bits of data read from the PHY register.
 *
 * Side Effects:    None
 *
 * Overview:        ReadPHYReg performs an MII read operation.  While in 
 *					progress, it simply polls the MII BUSY bit wasting time 
 *					(25.6us).
 *
 * Note:            None
 *****************************************************************************/
WORD ReadPHYReg(BYTE Register)
{
	WORD wResult;

	// Set the right address and start the register read operation
	WriteReg(MIREGADR, 0x0100 | Register);
	WriteReg(MICMD, MICMD_MIIRD);

	// Loop to wait until the PHY register has been read through the MII
	// This requires 25.6us
	while(ReadReg(MISTAT) & MISTAT_BUSY);

	// Stop reading
	WriteReg(MICMD, 0x0000);
	
	// Obtain results and return
	wResult = ReadReg(MIRD);

	return wResult;
}//end ReadPHYReg

/******************************************************************************
 * Function:        WritePHYReg
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           Address of the PHY register to write to.
 *					16 bits of data to write to PHY register.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        WritePHYReg performs an MII write operation.  While in 
 *					progress, it simply polls the MII BUSY bit wasting time.
 *
 * Note:            None
 *****************************************************************************/
void WritePHYReg(BYTE Register, WORD Data)
{
	// Write the register address
	WriteReg(MIREGADR,  0x0100 | Register);
	
	// Write the data
	WriteReg(MIWR, Data);

	// Wait until the PHY register has been written
	while(ReadReg(MISTAT) & MISTAT_BUSY);
}//end WritePHYReg



/*****************************************************************************
  Function:
	static void ToggleCRYPTEN(void)

  Summary:
	Toggles the CRYPTEN (EIR<15>) cryptographic enable bit in an errata safe 
	way.

  Description:
	Due to an ENC424J600/624J600 Rev. A2 silicon errata, it is not possible to 
	set or clear the CRYPTEN bit (EIR<15>) by using the BFSReg() or BFCReg() 
	functions.  Instead a register write to the volatile EIR interrupt flags 
	register is required.  This function reads EIR, toggles the CRYPTEN state, 
	and writes the value back to EIR in a manner that does not destroy or 
	glitch the interrupt states.

  Precondition:
	None

  Parameters:
	None

  Returns:
	None
  ***************************************************************************/
#ifdef PAKAI_ETH_CRYPT
static void ToggleCRYPTEN(void)
{
	WORD wLinkStat;
	WORD wInterruptEnable;
	
	// About to do read-modify-write to interrupt flags - disable interrupts 
	// temporarily.
	wInterruptEnable = ReadReg(EIE);
	if(wInterruptEnable & EIE_INTIE)
		BFCReg(EIE, EIE_INTIE);
		
	// Get current link status so that we can regenerate LINKIF if it is missed
	wLinkStat = ReadReg(ESTAT);
	
	#if(ENC100_INTERFACE_MODE == 0)	// SPI
	{
		BYTE i;
		
		ReadN(RCR | EIRH, &i, 1);	// Read EIR high byte only
		i ^= 0x80;					// Modify - toggle CRYPTEN	
		WriteN(WCR | EIRH, &i, 1);	// Write EIR high byte only
	}
	#elif(ENC100_INTERFACE_MODE == 1) || (ENC100_INTERFACE_MODE == 2) || (ENC100_INTERFACE_MODE == 5) || (ENC100_INTERFACE_MODE == 6) 	// 8-bit PSP modes
	{
		BYTE i;
		
		ReadMemory(EIRH, &i, 1);	// Read EIR high byte only
		i ^= 0x80;					// Modify - toggle CRYPTEN	
		WriteMemory(EIRH, &i, 1);	// Write EIR high byte only
	}
	#else	// 16-bit PSP
	{
		WORD w;
		
		// Wait for any pending transmit and DMA operations to complete so that 
		// the interrupt flags get set appropriately
		while(ReadReg(ECON1) & (ECON1_DMAST | ECON1_TXRTS));

		// Turn on/off crypto enable
		ReadMemory(EIR, (BYTE*)&w, 2);	// Read EIR
		w ^= EIR_CRYPTEN;				// Modify - set CRYPTEN	
		WriteMemory(EIR, (BYTE*)&w, 2);	// Write EIR - has to be 16 bits
		
		// Regenerate packet counter full interrupt flag, PCFULIF, if it was 
		// missed (unlikley but possible) 
		if((ReadReg(ESTAT) & 0xFFu) == 0xFFu)
			BFSReg(EIR, EIR_PCFULIF);
	}
	#endif

	// Generate link change interrupt if the status has changed while we were 
	// doing the read-modify-write operation (unlikely but possible)
	if((ReadReg(ESTAT) ^ wLinkStat) & ESTAT_PHYLNK)
		BFSReg(EIR, EIR_LINKIF);
		
	// Restore interrupts
	if(wInterruptEnable & EIE_INTIE)
		BFSReg(EIE, EIE_INTIE);
}
#endif

/****************************************************************************
  Section:
	Global RSA Variables
  ***************************************************************************/
#if defined(STACK_USE_SSL_SERVER) || defined(STACK_USE_SSL_CLIENT)

static SM_RSA smRSA;					// State machine variable
static BYTE keyLength;					// Length of the input key
static BYTE* vOutput;					// Where output should be stored

extern ROM BYTE SSL_N[SSL_RSA_KEY_SIZE/8], SSL_D[SSL_RSA_KEY_SIZE/8];


/****************************************************************************
  Section:
	Function Implementations
  ***************************************************************************/

/*****************************************************************************
  Function:
	void RSAInit(void)

Deleted Only
  ***************************************************************************/
#ifdef PAKAI_ETH_RSA
void RSAInit(void)
{
	smRSA = SM_RSA_IDLE;
}
#endif
/*****************************************************************************
  Function:
	BOOL RSABeginUsage(RSA_OP op, BYTE vKeyByteLen)

Same
  ***************************************************************************/
#ifdef PAKAI_ETH_RSA
BOOL RSABeginUsage(RSA_OP op, BYTE vKeyByteLen)
{
	WORD oldPtr;
	
	if(smRSA != SM_RSA_IDLE)
		return FALSE;

	keyLength = vKeyByteLen;
	
	// ENCX24J600 modular exponentiation engine only supports 1024 bit, 768 bit, 
	// and 512 bit modulos.  Other values could cause the incorrect behavior so 
	// must be avoided.
	if(!((keyLength == 1024u/8u) || (keyLength == 768u/8u) || (keyLength == 512u/8u)))
		return FALSE;

	// Set up the state machine
	if(op == RSA_OP_ENCRYPT)
	{
		smRSA = SM_RSA_ENCRYPT_START;

		// Turn on Modular Exponentiation hardware engine
		if(!(ReadReg(EIR) & EIR_CRYPTEN))
			ToggleCRYPTEN();
	}
#if defined(STACK_USE_SSL_SERVER) || defined(STACK_USE_RSA_DECRYPT)
	else if(op == RSA_OP_DECRYPT)
	{
		smRSA = SM_RSA_DECRYPT_START;

		// Turn on Modular Exponentiation hardware engine
		if(!(ReadReg(EIR) & EIR_CRYPTEN))
			ToggleCRYPTEN();
			    
	    // Copy the decryption key and DMA into palce
	    oldPtr = MACSetWritePtr(BASE_CRYPTOB_ADDR);
	    MACPutROMArray(SSL_D, keyLength);
	    MACMemCopyAsync(ENC100_MODEX_E, BASE_CRYPTOB_ADDR, 128);
	    while(!MACIsMemCopyDone());
	    
		// Copy the modulus and DMA into palce
		MACSetWritePtr(BASE_CRYPTOB_ADDR);
	    MACPutROMArray(SSL_N, keyLength);
	    MACMemCopyAsync(ENC100_MODEX_M, BASE_CRYPTOB_ADDR, 128);
	    while(!MACIsMemCopyDone());
	    
		// Restore the write pointer
		MACSetWritePtr(oldPtr);
	}
#endif
	else
		return FALSE;

	return TRUE;
}
#endif
/*****************************************************************************
  Function:
	void RSAEndUsage(void)

Same
Need to clear MODEXST to cancel
  ***************************************************************************/
#ifdef PAKAI_ETH_RSA
void RSAEndUsage(void)
{
	smRSA = SM_RSA_IDLE;

	// Turn off Modular Exponentiation hardware engine to save power
	if(ReadReg(EIR) & EIR_CRYPTEN)
		ToggleCRYPTEN();
}
#endif
/*****************************************************************************
  Function:
	void RSASetData(BYTE* data, BYTE len, RSA_DATA_FORMAT format)

Replaced
  ***************************************************************************/
#ifdef PAKAI_ETH_RSA
void RSASetData(BYTE* data, BYTE len, RSA_DATA_FORMAT format)
{
	WORD oldPtr;
	BYTE i;
	
	// Little-endian input is not supported for ENC624 hardware crypto
	if(format == RSA_LITTLE_ENDIAN)
		while(1);
		
	// Set the write pointer to reserved crypto area
    oldPtr = MACSetWritePtr(BASE_CRYPTOB_ADDR);

	// Pad the input data according to PKCS #1 Block 2 if doing an encryption 
	// operation
	#if defined(STACK_USE_SSL_CLIENT) || defined(STACK_USE_RSA_ENCRYPT)
	{
		BYTE j;
		
		if(smRSA == SM_RSA_ENCRYPT_START)
		{
			if(len < keyLength-4)
			{
				MACPut(0x00);
				MACPut(0x02);
				for(i = len + 3; i < keyLength; i++)
				{
					do
					{
						j = RandomGet();
					} while(j == 0x00u);
					MACPut(j);
				}
				MACPut(0x00);
			}
		}
	}
	#endif
    
    // Left zero-pad the input for decrypt operations
    if(smRSA == SM_RSA_DECRYPT_START)
    {
	    for(i = len; i < keyLength; i++)
	    	MACPut(0x00);
	}
	
	// Copy data into ENCX24J600 and DMA into ModEx engine
    MACPutArray(data, len);
    MACMemCopyAsync(ENC100_MODEX_X, BASE_CRYPTOB_ADDR, 128);

	// Restore the write pointer
	MACSetWritePtr(oldPtr);
		
	// Decrypt operations happen in place
	if(smRSA == SM_RSA_DECRYPT_START)
		RSASetResult(data, format);

	// Wait for DMA engine to finish copying data into ModEx engine
    while(!MACIsMemCopyDone());
}
#endif
/*****************************************************************************
  Function:
	void RSASetResult(BYTE* data, RSA_DATA_FORMAT format)

Replaced
  ***************************************************************************/
#ifdef PAKAI_ETH_RSA
void RSASetResult(BYTE* data, RSA_DATA_FORMAT format)
{	
	// Little-endian input is not supported for ENC624 hardware crypto
	if(format == RSA_LITTLE_ENDIAN)
		while(1);
		
	vOutput = data;
}
#endif
/*****************************************************************************
  Function:
	void RSASetE(BYTE *data, BYTE len, RSA_DATA_FORMAT format)

Replace
  ***************************************************************************/
#ifdef PAKAI_ETH_RSA
#if defined(STACK_USE_RSA_ENCRYPT)
void RSASetE(BYTE* data, BYTE len, RSA_DATA_FORMAT format)
{
	WORD oldPtr;
	BYTE i;
	
	// Little-endian input is not supported for ENC624 hardware crypto
	if(format == RSA_LITTLE_ENDIAN)
		while(1);
		
	// Set the write pointer to reserved crypto area
    oldPtr = MACSetWritePtr(BASE_CRYPTOB_ADDR);
    
    // Zero-pad the input, copy data, and DMA into palce
    for(i = len; i < keyLength; i++)
    	MACPut(0x00);
    MACPutArray(data, len);
    MACMemCopyAsync(ENC100_MODEX_E, BASE_CRYPTOB_ADDR, 128);
    
	// Restore the write pointer
	MACSetWritePtr(oldPtr);

	// Wait for DMA copy to ModEx to complete
    while(!MACIsMemCopyDone());
}
#endif
#endif
/*****************************************************************************
  Function:
	void RSASetN(BYTE* data, RSA_DATA_FORMAT format)

Replace
  ***************************************************************************/
#ifdef PAKAI_ETH_RSA
#if defined(STACK_USE_RSA_ENCRYPT)
void RSASetN(BYTE* data, RSA_DATA_FORMAT format)
{
	WORD oldPtr;
	
	// Little-endian input is not supported for ENC624 hardware crypto
	if(format == RSA_LITTLE_ENDIAN)
		while(1);
		
	// Set the write pointer to reserved crypto area
    oldPtr = MACSetWritePtr(BASE_CRYPTOB_ADDR);
    
    // Copy data and DMA into palce
    MACPutArray(data, keyLength);
    MACMemCopyAsync(ENC100_MODEX_M, BASE_CRYPTOB_ADDR, 128);
    
	// Restore the write pointer
	MACSetWritePtr(oldPtr);

	// Wait for DMA copy to ModEx to complete
    while(!MACIsMemCopyDone());
}
#endif
#endif
/*****************************************************************************
  Function:
	RSA_STATUS RSAStep(void)

Replace
  ***************************************************************************/
#ifdef PAKAI_ETH_RSA
RSA_STATUS RSAStep(void)
{
	WORD oldPtr;
	
	switch(smRSA)
	{
		case SM_RSA_ENCRYPT_START:
		case SM_RSA_DECRYPT_START:
			// Set the correct key length
			BFCReg(ECON2, ECON2_MODLEN1 | ECON2_MODLEN0);
			if(keyLength == 1024u/8u)
				BFSReg(ECON2, ECON2_MODLEN1);				
			if(keyLength == 768u/8u)
				BFSReg(ECON2, ECON2_MODLEN0);
			
			// Start the engine
			BFSReg(ECON1, ECON1_MODEXST);
			
			// Advance state machine to SM_RSA_ENCRYPT.  With the ENCX24J600 
			// Modular Exponentiation engine, the finishing of decryption is 
			// identical to the finishing of encryption.
			smRSA = SM_RSA_ENCRYPT;
			break;
			
		case SM_RSA_ENCRYPT:	// Also used to finish decryption
			// Check if operation is complete
			if(ReadReg(ECON1) & ECON1_MODEXST)
				break;
				
			// Copy the data out
			MACMemCopyAsync(BASE_CRYPTOB_ADDR, ENC100_MODEX_Y, 128);
   			oldPtr = MACSetReadPtr(BASE_CRYPTOB_ADDR);
			while(!MACIsMemCopyDone());
			MACGetArray(vOutput, keyLength);
			MACSetReadPtr(oldPtr);

			// Indicate that we're finished
			smRSA = SM_RSA_DONE;
			return RSA_DONE;
		
		default:
			// Unknown state
			return RSA_DONE;
	}
	
	// Indicate that we're still working
	return RSA_WORKING;
	
}
#endif

/****************************************************************************
  Section:
	Fundamental RSA Operations
  ***************************************************************************/

/*****************************************************************************
  Function:
	static BOOL _RSAModExpROM(BIGINT* y, BIGINT* x, BIGINT_ROM* e, 
								BIGINT_ROM* n)

Not Needed
  ***************************************************************************/

/*****************************************************************************
  Function:
	static BOOL _RSAModExp(BIGINT* y, BIGINT* x, BIGINT* e, BIGINT* n)

Not Needed
  ***************************************************************************/

#endif



//* 
// A function potentially useful for debugging, but a waste of code otherwise
#ifdef TEST_ENC
void ENC100DumpState(void)
{
	WORD w;
	BYTE a[32];
	BYTE i;

	printf("\r\n  Current Packet Address = 0x%04hX", wCurrentPacketPointer);
	printf("\r\n  EIR     = 0x%04hX", ReadReg(EIR));
	printf("\r\n  ERXST   = 0x%04hX", ReadReg(ERXST));
	printf("\r\n  EUDAST  = 0x%04hX", ReadReg(EUDAST));
	printf("\r\n  EUDAND  = 0x%04hX", ReadReg(EUDAND));
	printf("\r\n  ERXTAIL = 0x%04hX", ReadReg(ERXTAIL));
	printf("\r\n  ERXHEAD = 0x%04hX", ReadReg(ERXHEAD));
	w = ReadReg(ESTAT);
	printf("\r\n  ESTAT   = 0x%04hX (Packet Count = %hhu)", w, (BYTE)w);
	printf("\r\n  ERXFCON = 0x%04hX", ReadReg(ERXFCON));
	printf("\r\n  MACON1  = 0x%04hX", ReadReg(MACON1));
	printf("\r\n  MACON2  = 0x%04hX", ReadReg(MACON2));
	printf("\r\n  ECON1   = 0x%04hX", ReadReg(ECON1));
	printf("\r\n  ECON2   = 0x%04hX", ReadReg(ECON2));
	printf("\r\n  ETXST   = 0x%04hX", ReadReg(ETXST));
	printf("\r\n  ETXLEN  = 0x%04hX", ReadReg(ETXLEN));
	printf("\r\n  EDMAST  = 0x%04hX", ReadReg(EDMAST));
	printf("\r\n  EDMALEN = 0x%04hX", ReadReg(EDMALEN));
	printf("\r\n  EDMADST = 0x%04hX", ReadReg(EDMADST));
	printf("\r\n  EDMACS  = 0x%04hX", ReadReg(EDMACS));

	for(w = 0; w < ENC100_RAM_SIZE; w += 16)
	{
		ReadMemory(w, a, 16);
		
		for(i = 0; i < 16u; i++)
		{
			if((a[i] >= 0x20) && (a[i] <= 0x7E))
				a[i+16] = a[i];
			else
				a[i+16] = '.';
		}
		
		printf("\r\n%04hx  %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx  %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx   %hhc%hhc%hhc%hhc%hhc%hhc%hhc%hhc %hhc%hhc%hhc%hhc%hhc%hhc%hhc%hhc", \
			w, a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11], a[12], a[13], a[14], a[15], \
			a[16], a[17], a[18], a[19], a[20], a[21], a[22], a[23], a[24], a[25], a[26], a[27], a[28], a[29], a[30], a[31]);
	}
	printf("\r\n\r\n");
}
#endif
//*/

#endif //#if defined(ENC100_INTERFACE_MODE)

#endif
