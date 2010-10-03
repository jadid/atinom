//
//  $Id: enc28j60.c 320 2008-11-08 05:05:26Z jcw $
//  $Revision: 320 $
//  $Author: jcw $
//  $Date: 2008-11-08 00:05:26 -0500 (Sat, 08 Nov 2008) $
//  $HeadURL: http://tinymicros.com/svn_public/arm/lpc2148_demo/trunk/uip/hardware/enc28j60.c $
//

/*****************************************************************************
//
// File Name    : 'enc38j60.c'
// Title        : Driver for the Microchip ENC28J60 Ethernet controller
// Author       : Bernard Debbasch - Copyright (C) 2005
// Created      : 2005.08.17
// Revised      : 2005.08.17
// Version      : 1.0
// Target MCU   : Philips LPC213x series
// Editor Tabs  : 2
//
//
//
// Copyright (C) 2005, Bernard Debbasch
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote
//    products derived from this software without specific prior
//    written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
 *******************************************************************************/

/*
 * 16 feb 09
 * organisasi memeory diperbaiki, RX ditaruh dibawah
 * biar tidak sering paket hilang ??
 * 
 * 27 feb 09
 * init_enc_port diperbaiki
 */

#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "../uip/uip.h"
#include "../uip/uip_arp.h"
#include "../uip/uipopt.h"
#include "enc28j60.h"

//
//  RX & TX memory organization
//
#define MAXFRAMESIZE    1518
#define RAMSIZE       0x2000
/*
#define TXSTART       0x0000
#define TXEND         0x0fff
#define RXSTART       0x1000
#define RXEND         0x1fff
#define RXSIZE        (RXSTOP - RXSTART + 1)
*/
#define TX_BUFFER_SIZE  2040 //4096
#define TXSTART			(RAMSIZE - (TX_BUFFER_SIZE + 8ul))
#define RXSTART			0x0000ul
#define	RXEND			((TXSTART -2ul) | 0x0001ul)
//#define RXSIZE			(RXSTOP-(RXSTART+1ul))

//
//  Chose one or the other
//
//#undef  HALF_DUPLEX
//#define FULL_DUPLEX
#define HALF_DUPLEX

#ifdef HALF_DUPLEX
#ifdef FULL_DUPLEX
#error "DUPLEX==> CHOOSE ONE OR THE OTHER"
#endif
#endif

//#define USE_INTERRUPTS

//
//  Local Prototypes
//
static u8_t encReadEthReg (u8_t address);
static u8_t encReadMacReg (u8_t address);
static void encWriteReg (u8_t address, u8_t data);
static void encWriteReg16 (u8_t address, u16_t data);
static void encBFSReg (u8_t address, u8_t data);
static void encBFCReg (u8_t address, u8_t data);
static void SendSystemCommand (void);
static void encBankSelect (u8_t bank);
static void encWritePHYReg (u8_t address, u16_t data);
static u16_t encReadPHYReg (u8_t address);
static void encMACwrite (u8_t data);
static void encMACwriteBulk (u8_t *buffer, u16_t length);
static u8_t encMACread (void);
static void encMACreadBulk (u8_t *buffer, u16_t length);

#include "../../tinysh/enviro.h"
//extern struct t_env env2;

//
//  Global variable
//
u16_t ethRxPointer = 0;
unsigned int error_ENC=0;

#ifdef GUNA_SEMA
xSemaphoreHandle xENC28J60Semaphore = NULL;
#endif

/******************************************************************************
 * Function:        void EthernetInit (void)
 * PreCondition:    SPI must be configured
 * Input:           None
 * Output:          None
 * Side Effects:    None
 *
 * Overview:        Initialize the ENC28J60
 * Note:            None
 *****************************************************************************/
int enc28j60Init (void)
{
  volatile portTickType xTicks;
	struct t_env *envx;
	envx = (char *) ALMT_ENV;
  //
  //  If the current MAC address is 00:00:00:00:00:00, default to UIP_ETHADDR[0..5] values
  //

  if (!uip_ethaddr.addr[0] && !uip_ethaddr.addr[1] && !uip_ethaddr.addr[2] && !uip_ethaddr.addr[3] && !uip_ethaddr.addr[4] && !uip_ethaddr.addr[5])
  {
    	unsigned char ran_mac;
    	unsigned int sed;
    
    	/* 
    		init random seed 
			ambil dari watchdog clock    
    	*/
    	sed = WDTV;
    	printf("Random seed = %u, ", sed);
    	//sed = sed * env2.IP3;
    	sed = sed * envx->IP3;
    	
    	srand(sed);
    	
    	ran_mac = rand() % 255;
    	printf("ran mac = %X\r\n", ran_mac);
    
    	//struct uip_eth_addr mac = {{UIP_ETHADDR0, UIP_ETHADDR1, UIP_ETHADDR2, UIP_ETHADDR3, UIP_ETHADDR4, UIP_ETHADDR5}};
    	/*
    	struct uip_eth_addr mac = {{UIP_ETHADDR0, UIP_ETHADDR1, UIP_ETHADDR2, env2.IP3, UIP_ETHADDR4, env2.IP3+3}};
		printf("(%2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X)", 
			UIP_ETHADDR0, UIP_ETHADDR1, UIP_ETHADDR2, env2.IP3, UIP_ETHADDR4, env2.IP3+3);
		*/
		//struct uip_eth_addr mac = {{UIP_ETHADDR0, UIP_ETHADDR1, UIP_ETHADDR2, env2.IP3, ran_mac, env2.IP3+3}};
		struct uip_eth_addr mac = {{UIP_ETHADDR0, UIP_ETHADDR1, UIP_ETHADDR2, envx->IP3, ran_mac, envx->IP3+3}};
		//printf("(%2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X)", 
		//	UIP_ETHADDR0, UIP_ETHADDR1, UIP_ETHADDR2, env2.IP3, ran_mac, env2.IP3+3);
		//printf("(%2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X)", 
		//	UIP_ETHADDR0, UIP_ETHADDR1, UIP_ETHADDR2, envx->IP3, ran_mac, envx->IP3+3);
		uip_setethaddr (mac);
  }

#ifdef GUNA_SEMA
  //
  //  It'd probably be better if the EINT initialization code took a pointer to
  //  the semaphore, rather than just "knowing" about it, but it's hard to make
  //  that sort of thing generic, since an interrupt routine may not really
  //  want to use a semaphore, and instead use a queue or somesuch.
  //
  if (!xENC28J60Semaphore)
  {
    vSemaphoreCreateBinary (xENC28J60Semaphore);

    if (!xENC28J60Semaphore)
      return 0;
  }

  xSemaphoreTake (xENC28J60Semaphore, 0);
#endif
  //
  //  Set up the reset line.  This isn't done in the SPI code, because it
  //  doesn't need to be aware of the ENC28J60 reset.  Do it first so we
  //  can hold the part in reset before SPI initialization.
  //
  //PCB_PINSEL1 = (PCB_PINSEL1 & ~PCB_PINSEL1_P022_MASK) | PCB_PINSEL1_P022_GPIO;
  //GPIO0_FIODIR |= GPIO_IO_P22;
  ENC28J60_Reset ();

  // set INT_ENC sebagai input
  //FIO1DIR = FIO1DIR & ~(INT_ENC);
  init_enc_port();
  
  //
  //  Disable UART1 so we can use the RXD pin as EINT3
  //
  //uart1Deinit ();
  //eint3Init ();
  //init_enc_intterupt();
  spiInit ();

  //
  //  Now hold part in reset for 100ms
  //
  ENC28J60_Deselect ();
  ENC28J60_Reset ();
  vTaskDelay (100 / portTICK_RATE_MS);
  ENC28J60_Unreset ();
  vTaskDelay (100 / portTICK_RATE_MS);

  //
  //  Give the part 1 second for the PHY to become ready (CLKRDY == 1).  If it
  //  doesn't, return an error to the user.
  //
  //  Note that we also check that bit 3 is 0.  The data sheet says this is
  //  unimplemented and will return 0.  We use this as a sanity check for the
  //  ENC28J60 actually being present, because the MISO line typically floats
  //  high.  If we only checked the CLKRDY, it will likely return 1 for when no
  //  ENC28J60 is present.
  //
  xTicks = xTaskGetTickCount ();

  while (((encReadEthReg (ESTAT) & (ESTAT_UNIMP | ESTAT_CLKRDY)) != ESTAT_CLKRDY))
    if ((xTaskGetTickCount () - xTicks) > (1000 / portTICK_RATE_MS))
      return 0;

  //
  //  Send a Soft Reset to the chip
  //
  SendSystemCommand ();
  vTaskDelay (20 / portTICK_RATE_MS);

  encBankSelect (BANK0);

  ethRxPointer = RXSTART;             // Initialize the local RX pointer...
  encWriteReg16 (ERXSTL, RXSTART);    // ...and configure the base address */
  encWriteReg16 (ERXRDPTL, RXSTART);  // Read pointer is also at the base address for now
  encWriteReg16 (ERXNDL, RXEND);      // Configure the H/W with the end of memory location
  encWriteReg16 (ETXSTL, TXSTART);    // Configure the H/W with the TX base address

  encBankSelect (BANK1);
  //encWriteReg16 (ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_BCEN);
	// set filter
	encWriteReg16 (ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_PMEN);
	encWriteReg(EPMM0, 0x3f);
	encWriteReg(EPMM1, 0x30);
	encWriteReg(EPMCSL, 0xf9);
	encWriteReg(EPMCSH, 0xf7);


  encBankSelect (BANK2);
  encWriteReg (MACON2, 0);  // Remove all reset conditions

#ifdef HALF_DUPLEX
  encWriteReg (MACON1, MACON1_MARXEN);                    // Enable RX
  encWriteReg (MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN);  // Automatic padding, CRC generation
#else
  encWriteReg (MACON1, MACON1_TXPAUS | MACON1_RXPAUS | MACON1_MARXEN);    // Enable TX pause, RX pause and RX
  encWriteReg (MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FULDPX);  // Automatic padding, CRC generation, Full-Duplex
#endif

  encWriteReg16 (MAMXFLL, MAXFRAMESIZE);  // Set the maximum packet size which the controller will accept

  encWriteReg16 (MAIPGL, 0x0c12);         // Set inter-packet gap to 9.6us

  encWriteReg (MABBIPG, 0x12);            // Set back to back inter-packet gap

  encBankSelect (BANK3);

	#if 1
	printf("MAC address : ");
	printf("  (%2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X) ", \
		uip_ethaddr.addr [0], uip_ethaddr.addr [1],	\
		uip_ethaddr.addr [2], uip_ethaddr.addr [3], \
		uip_ethaddr.addr [4], uip_ethaddr.addr [5]);
		
	#endif

  encWriteReg (MAADR1, uip_ethaddr.addr [0]);
  encWriteReg (MAADR2, uip_ethaddr.addr [1]);
  encWriteReg (MAADR3, uip_ethaddr.addr [2]);
  encWriteReg (MAADR4, uip_ethaddr.addr [3]);
  encWriteReg (MAADR5, uip_ethaddr.addr [4]);
  encWriteReg (MAADR6, uip_ethaddr.addr [5]);

#ifdef HALF_DUPLEX
  encWritePHYReg (PHCON1, 0);
#else
  encWritePHYReg (PHCON1, PHCON1_PDPXMD);
#endif

  encWritePHYReg (PHCON2, PHCON2_HDLDIS);   // Disable half duplex loopback in PHY.

  encReadPHYReg (PHID1);          // Read a PHY register
  encBFSReg (ECON1, ECON1_RXEN);  // Enable packet reception

  // enable interrupt
  encBankSelect(BANK0);
  //encWriteReg(EIE, EIE_INTIE | EIE_PKTIE | EIE_TXIE | EIE_TXERIE | EIE_RXERIE);
  encWriteReg(EIE, EIE_INTIE | EIE_PKTIE | EIE_TXERIE | EIE_RXERIE | EIE_LINKIE);
  //encWriteReg(EIE, EIE_INTIE | EIE_PKTIE );

	//encWriteReg(EIE, 0x00);
	
  return 1;
}

void enc28j60Deinit (void)
{
  ENC28J60_Reset ();
  //eint3Deinit ();
  //uart1Init (0, 0);
}

//
//
//



void enc28j60Send (void)
{
  u16_t length;
  u16_t value;

	//portENTER_CRITICAL();
	
	
  length = uip_len;           // Save length for later

  encBankSelect (BANK0);
  encWriteReg16 (EWRPTL, TXSTART);  // Configure the H/W with the TX Start and End addresses
  //encMACwrite (0x00);               // Write the per packet option byte we use the default values set in chip earlier

	encMACwrite (0x0E); 
	
  encMACwriteBulk (&uip_buf [0], 54);   // Send 40 + 14 = 54 bytes of header

#ifdef ETHERNETDEBUG
  for (value = 0; value < 54; value++)
    printf ("%x ", uip_buf [value]);
  printf ("\n");
#endif

  //
  // Send the rest of the packet, the application data
  //
  if (uip_len > 54)
  {
    uip_len -= 54;
    encMACwriteBulk (uip_appdata, uip_len);
  }

  //
  //  Configure the H/W with the TX Start and End addresses
  //
  //encWriteReg16 (ETXSTL, TXSTART);  // Configure the H/W with the TX base address
  encWriteReg16 (ETXNDL, (TXSTART + length + 1)); // 1 untuk kontrol byte
  //value  = encReadEthReg (ETXNDL);
  //value += encReadEthReg (ETXNDH) << 8;

  //
  //  Enable packet Transmission
  //

  encBFCReg (EIR, EIR_TXIF);
  encBFSReg (ECON1, ECON1_TXRST);         // Errata for B5
  encBFCReg (ECON1, ECON1_TXRST);         // Errata for B5
  encBFCReg (EIR, EIR_TXERIF | EIR_TXIF); // Errata for B5

  encBFSReg (ECON1, ECON1_TXRTS);

  #if 0
  unsigned int temp;
  // dari gaya BF
  temp = encReadEthReg (EIR);
  
  if((temp & EIR_TXERIF) == 1)
  {
  		encBFCReg (EIR, EIR_TXERIF); 
		encBFSReg (ECON1, ECON1_TXRTS);
		encBFSReg (ECON1, ECON1_TXRTS);
  }
  
  encBFCReg (EIR, EIR_TXIF);
  encBFSReg (ECON1, ECON1_TXRTS);
  #endif
  //portEXIT_CRITICAL();
  
}

//
//
//
u16_t enc28j60Receive (void)
{
  u16_t len = 0;
  u16_t u;
  int jum_pak;
  
	//portENTER_CRITICAL();
	
#ifndef USE_INTERRUPTS
  //
  //  Check if at least one packet has been received and is waiting
  //
  	if ((encReadEthReg (EIR) & EIR_PKTIF) == 0)
  	{
    	//printf("INT %X ENC, re init()\n", encReadEthReg(EIR));
		//portEXIT_CRITICAL();
		
		// re init ENC
		enc28j60Init();	
		return 0;
	}
	/*
	encBankSelect (BANK1);
	jum_pak = encReadEthReg (EPKTCNT);
	printf("\njp=%d ", jum_pak);
	*/
#else
  //
  //  Sanity check that at least one full packet is present (spurious interrupts?)
  //
  
  encBankSelect (BANK1);
	jum_pak = encReadEthReg (EPKTCNT);
	printf("jp=%d ", jum_pak);

  //if (!encReadEthReg (EPKTCNT))
  if (!jum_pak)
    return 0;
#endif

  //
  //  Set read Pointer
  //
  encBankSelect (BANK0);
  encWriteReg16 (ERDPTL, ethRxPointer);

  ethRxPointer = encMACread ();
  ethRxPointer += encMACread () << 8;

	//printf("N=%X ", ethRxPointer);

  //
  //  Sanity check
  //
  if (ethRxPointer > RXEND)
  {
    	ethRxPointer = 0;
		//printf("reset rx pointer!"); 
		
		//portEXIT_CRITICAL();
	//enc28j60Init ();
    return 0;
  }
  

  len = encMACread ();
  len += encMACread () << 8;

  //
  //  Read 2 more statuses that we ignore (why?)
  //
  encMACread ();
  encMACread ();

  //
  //  If the frame is too big to handle, throw it away
  //
  if (len > UIP_BUFSIZE)
  {
    for (u = 0; u < len; u++)
      encMACread ();

	//portEXIT_CRITICAL();
    return 0;
  }

  //
  //  Read the whole frame
  //
  encMACreadBulk (&uip_buf [0], len);

  //
  //  Clean up for next packet.  Set the read pointer to the start of RX packet to free up space used by the current frame
  //
  encBankSelect (BANK0);
  encWriteReg16 (ERXRDPTL, ethRxPointer-1);		// errata

  //
  //  Decrement EPKTCNT
  //
  encBFSReg (ECON2, ECON2_PKTDEC);

  //printf(":%d:", len);
  //
  //  Return the length - the 4 bytes of CRC (why?)
  //
  
  //portEXIT_CRITICAL();
  return (len - 4);
}

//
//
//
signed portBASE_TYPE enc28j60WaitForData (portTickType delay)
{

	portBASE_TYPE semStat;

#ifdef USE_INTERRUPTS
  encBFCReg (EIR, EIR_PKTIF);
  encWriteReg (EIE, EIE_INTIE | EIE_PKTIE);
#endif

#ifdef GUNA_SEMA
  if ((semStat = xSemaphoreTake (xENC28J60Semaphore, delay)) == pdPASS)
  {
#ifdef USE_INTERRUPTS
    encBFCReg (EIE, EIE_INTIE);
#endif
  }

#endif
  return semStat;

}

unsigned int cek_paket(void)
{
	#if 1
	if (FIO_CEK_PAKET & INT_ENC)
	{
		return 0;
	}
	else
	{
		return 1;
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

/******************************************************************************
 * Function:        void SendSystemCommand (void)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    All values are lost
 *
 * Overview:        Send the SC (Reset) command to the device
 * Note:            None
 *****************************************************************************/
static void SendSystemCommand (void)
{
  ENC28J60_Select ();

  spiPut (SC);

  ENC28J60_Deselect ();
}

/******************************************************************************
 * Function:        u8_t encReadEthReg (u8_t address)
 * PreCondition:    None
 * Input:           address to read
 * Output:          value read from the register
 * Side Effects:    None
 *
 * Overview:        read the value at the address over the SPI bus
 * Note:            None
 *****************************************************************************/
static u8_t encReadEthReg (u8_t address)
{
  u8_t value;

  //
  //  Tcss (/CS setup time) is 50ns, Tcsh (/CS hold time) is 10ns ETH registers
  //  and memory buffer, 210ns MAC and MII registers.
  //
  ENC28J60_Select ();

  spiPut (RCR | address);
  value = spiPut (0x00);

  ENC28J60_Deselect ();

  return value;
}

/******************************************************************************
 * Function:        u8_t encReadMacReg (u8_t address)
 * PreCondition:    None
 * Input:           address to read
 * Output:          value read from the register
 * Side Effects:    None
 *
 * Overview:        read the value at the address over the SPI bus
 * Note:            None
 *****************************************************************************/
static u8_t encReadMacReg (u8_t address)
{
  u8_t value;

  ENC28J60_Select ();

  spiPut (RCR | address);
  spiPut (0x00);             /* Send a dummy byte */
  value = spiPut (0x00);

  ENC28J60_Deselect ();

  return value;
}

/******************************************************************************
 * Function:        void encWriteReg (u8_t address, u8_t data)
 * PreCondition:    Bank must point ot the right bank
 * Input:           address and data to write
 * Output:          None
 * Side Effects:    None
 *
 * Overview:        Send value at the address over the SPI bus
 * Note:            None
 *****************************************************************************/
static void encWriteReg (u8_t address, u8_t data)
{
  ENC28J60_Select ();

  spiPut (WCR | address);
  spiPut (data);

  ENC28J60_Deselect ();
}

/******************************************************************************
 * Function:        void encBFSReg (u8_t address, u8_t data)
 * PreCondition:    Bank must point ot the right bank
 * Input:           address and data field
 * Output:          None
 * Side Effects:    None
 *
 * Overview:        Set the data field in the address register
 * Note:            None
 *****************************************************************************/
static void encBFSReg (u8_t address, u8_t data)
{
  ENC28J60_Select ();

  spiPut (BFS | address);
  spiPut (data);

  ENC28J60_Deselect ();
}

/******************************************************************************
 * Function:        void encBFCReg (u8_t address, u8_t data)
 * PreCondition:    Bank must point ot the right bank
 * Input:           address and data fiels
 * Output:          None
 * Side Effects:    None
 *
 * Overview:        Clear the data field in the address register
 * Note:            None
 *****************************************************************************/
static void encBFCReg (u8_t address, u8_t data)
{
  ENC28J60_Select ();

  spiPut (BFC | address);
  spiPut (data);

  ENC28J60_Deselect ();
}

/******************************************************************************
 * Function:        void encWriteReg16 (u8_t address, u16_t data)
 * PreCondition:    Bank must point ot the right bank
 * Input:           address and data to write
 * Output:          None
 * Side Effects:    None
 *
 * Overview:        Send value at the address over the SPI bus
 * Note:            None
 *****************************************************************************/
static void encWriteReg16 (u8_t address, u16_t data)
{
  encWriteReg (address,   (u8_t) (data & 0xff));
  encWriteReg (address+1, (u8_t) (data >> 8));
}

/******************************************************************************
 * Function:        void encWritePHYReg (u8_t address, u16_t data)
 * PreCondition:    None
 * Input:           address and data to write
 * Output:          None
 * Side Effects:    None
 *
 * Overview:        Send value at the address over the SPI bus
 * Note:            None
 *****************************************************************************/
static void encWritePHYReg (u8_t address, u16_t data)
{
  encBankSelect (BANK2);

  encWriteReg (MIREGADR, address);
  encWriteReg16 (MIWRL, data);

  encBankSelect (BANK3);

  while ((encReadMacReg (MISTAT) & MISTAT_BUSY))
    ;
}

/******************************************************************************
 * Function:        u16_t encReadPHYReg (u8_t address)
 * PreCondition:    None
 * Input:           address to read from
 * Output:          data from a PHY register
 * Side Effects:    None
 *
 * Overview:        Read a value from a PHY register
 * Note:            None
 *****************************************************************************/
static u16_t encReadPHYReg (u8_t address)
{
  u16_t value;

  encBankSelect (BANK2);

  encWriteReg (MIREGADR, address);
  encWriteReg (MICMD, MICMD_MIIRD);

  encBankSelect (BANK3);

  while ((( encReadMacReg (MISTAT)) & MISTAT_BUSY))
    ;

  encBankSelect (BANK2);

  encWriteReg (MICMD, 0);

  value  = encReadMacReg (MIRDH) << 8;
  value += encReadMacReg (MIRDL);

  return value;
}

/******************************************************************************
 * Function:        void encBankSelect (u8_t bank)
 * PreCondition:    None
 * Input:           bank (0, 1, 2 or 3)
 * Output:          None
 * Side Effects:    None
 *
 * Overview:        Set the bank select
 * Note:            None
 *****************************************************************************/
static void  encBankSelect (u8_t bank)
{
  encBFCReg (ECON1, ECON1_BSEL1 | ECON1_BSEL0);
  encBFSReg (ECON1, bank);
}

/******************************************************************************
 * Function:        void encMACwrite (u8_t data)
 * PreCondition:    EWRPT must point to the location to be written to
 * Input:           data to write
 * Output:          None
 * Side Effects:    None
 *
 * Overview:        Write a byte in the MAC memory with the intention
 *                  of sending a frame later
 * Note:            None
 *****************************************************************************/
static void encMACwrite (u8_t data)
{
  ENC28J60_Select ();

  spiPut (WBM);
  spiPut (data);

  ENC28J60_Deselect ();
}

/******************************************************************************
 * Function:        void encMACwriteBulk (u8_t *buffer, u16_t length)
 * PreCondition:    EWRPT must point to the location to be written to
 * Input:           data to write and length of data
 * Output:          None
 * Side Effects:    None
 *
 * Overview:        Multi Write in the MAC memory with the intention
 *                  of sending a frame later
 * Note:            None
 *****************************************************************************/
static void encMACwriteBulk (u8_t *buffer, u16_t length)
{
  ENC28J60_Select ();

  spiPut (WBM);

  while (length--)
    spiPut (*buffer++);

  ENC28J60_Deselect ();
}

/******************************************************************************
 * Function:        u8_t data encMACread (void)
 * PreCondition:    ERDPT must point to the location to read from
 * Input:           None
 * Output:          Data read
 * Side Effects:    None
 *
 * Overview:        Read a byte from the MAC memory
 * Note:            None
 *****************************************************************************/
static u8_t encMACread (void)
{
  u8_t value;

  ENC28J60_Select ();

  spiPut (RBM);
  value = spiPut (0x00);

  ENC28J60_Deselect ();

  return value;
}

/******************************************************************************
 * Function:        encMACreadBulk (u8_t *buffer, u16_t length)
 * PreCondition:    ERDPT must point to the location to read from
 * Input:           Buffer to put the data and length of data to read
 * Output:          Data read in the buffer
 * Side Effects:    None
 *
 * Overview:        Read multiple bytes from the MAC memory
 * Note:            None
 *****************************************************************************/
static void encMACreadBulk (u8_t *buffer, u16_t length)
{
  ENC28J60_Select ();

  spiPut (RBM);

  while (length--)
    *buffer++ = spiPut  (0x00);

  ENC28J60_Deselect ();
}

