//      cywusb.c
//      
//      Copyright 2010 afrendy <afrendy@pavilion>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#ifndef __CYWUSB__
#define __CYWUSB__

#ifdef PAKAI_CYWUSB

#include "CYWUSB693x.h"

//#define PAKAI_REMOTE_RELAY

#define CYWM_LED	BIT(25)
#define CYWM_nRESET	BIT(19)
#define CYWM_nSS	BIT(21)
#define CYWM_nPD	BIT(22)

#define PCSSP0		BIT(21)
#define DSS			0x07		// data 8 bit
	
#define SSP_data	SSP0DR
	
#define SSP0TNF	0x02
#define SSP0BSY	0x10
#define SSP0RNE	0x04

unsigned char x [50];

void init_gpio_cywusb(void) {
	FIO1DIR = FIO1DIR | CYWM_LED | CYWM_nRESET | CYWM_nPD | CYWM_nSS;
}

void init_cywusb(void) {
	FIO1SET = CYWM_nRESET;		// pin 1
	FIO1SET = CYWM_nPD;			// pin 4
}

void mati_cywusb(void) {
	FIO1CLR = CYWM_nRESET;
	FIO1CLR = CYWM_nPD;
}

void LED_cywusb_on() {
	FIO1SET = CYWM_LED;
}

void LED_cywusb_off() {
	FIO1CLR = CYWM_LED;
}

void init_modul_cywusb() {
	
	init_gpio_cywusb();
	init_SSP0();
	init_cywusb();
	//printf("Init cywusb\r\n ");
	//konfig_WUSB('b');
}

void reboot_wusb() {
	mati_cywusb();
	vTaskDelay(10);
	init_cywusb();
	vTaskDelay(10);
	konfig_WUSB('b');
	vTaskDelay(100);
	//if(flag)
		lihatKonfig();
}

void init_SSP0(void) {		// sbg SPI
	// 1
	PCONP 	 |= PCSSP0;
	
	// 2		// Set clock SCK Cywusb min 475 ns ~ 500 ns ~ f= 2Mhz
	PCLKSEL1 &= ~(BIT(10) | BIT(11));		// reset dulu saja	P_CLK=CCLK/4
	PCLKSEL1 |= BIT(10);	// =0 << CCLK/1
	SSP0CPSR  = 20;				// CPSDVSR = 2   PCLK/(20)	// genap 2 < CPSDVSR < 254		// 60/(80*3)
	
	// 3
	PINSEL3 &= ~( BIT(8) | BIT(9) | BIT(14) | BIT(15) | BIT(16) | BIT(17) );	// reset dulu
	PINSEL3 |= (BIT(8) | BIT(9));		// set SCK0
	PINSEL3 |= (BIT(14) | BIT(15));		// set MISO0
	PINSEL3 |= (BIT(16) | BIT(17));		// set MOSI0
	// PINMODE pull up resistor
	
	// 4
	
	
	// 5
	SSP0CR0 = 0x00;		// reset dulu
	SSP0CR0 = 0x0200 | 0x07;			// 	PCLK / (CPSDVSR × [SCR+1]). = CCLK/(20*3) = 60/60 Mhz = 1Mhz, 1us
	//SSP0CR0 = 0x0200 | 0x07;			// 	PCLK / (CPSDVSR × [SCR+1]). = CCLK/(20*5) = 60/60 Mhz = 1Mhz, 1us
	//SSP0CR0 |= DSS;			// DSS:7 = 8 bit, FRF:0 = SPI CPOL=0 CPHA=0
	SSP0CR1 = 0x02;		// LBM normal, enable SSP, SSP sudah master
	SSP0IMSC = 0x00;
	// x
	// data ada di SSP0DR 16 bit
}

/*
void SPI_Write(uint8_t byte) {
	SPDR = byte;				// Send SPI byte
	while(!(SPSR & (1<<SPIF)));	// Wait for SPI transmission complete
}
//*/

void kirim_SSP0(unsigned char t) {
	//SSP0DR = t;
	while ( (SSP0SR & (SSP0TNF|SSP0BSY)) != SSP0TNF );
	SSP0DR = t;
	
	while ( (SSP0SR & (SSP0BSY|SSP0RNE)) != SSP0RNE );
	/* Whenever a byte is written, MISO FIFO counter increments, Clear FIFO 
	on MISO. Otherwise, when SSP0Receive() is called, previous data byte
	is left in the FIFO. */
	t = SSP0DR;
	return;
}

unsigned char ambil_SSP0(void) {	
	SSP0DR = 0xFF;
	/* Wait until the Busy bit is cleared */
	while ( (SSP0SR & (SSP0BSY|SSP0RNE)) != SSP0RNE );
	
	return (unsigned char) SSP0DR;
}

void CYWM_WriteReg(unsigned char which, unsigned char data)	{
	//low(PORTC, wCS);		// CYWM_nSS
	FIO1CLR = CYWM_nSS;
	kirim_SSP0(REG_WRITE | which);
	kirim_SSP0(data);
	FIO1SET = CYWM_nSS;
	//high(PORTC, wCS);
	//_delay_ms(1);
}

unsigned char CYWM_ReadReg(unsigned char which) {
	unsigned char hasil;
	//low(PORTC, wCS);
	FIO1CLR = CYWM_nSS;
	kirim_SSP0(which);
	hasil = ambil_SSP0();
	//kirim_SSP0(which);
	FIO1SET = CYWM_nSS;
	//high(PORTC, wCS);
	return hasil;
}

void konfig_WUSB(char tipe) {
	unsigned char data;
	int flag, oz;

	//buang_watchdog();
	flag=0;
	for (oz=0; oz<25; oz++) {
		if (flag == 0) {
			data = CYWM_ReadReg( REG_ID );
			if (data == 0x07) {
				flag = 1;
				printf("REG_ID: %x oz: %d!\n\r", data, oz);
				continue;
			}
		}
	}
	if (!flag) {
		//sprintf(x, "REG_ID: %x: GAGAL!\n\r", data);
		printf("REG_ID: %x: GAGAL!\n\r", data);
		//return;
	}

	//buang_watchdog();
	portENTER_CRITICAL();
	CYWM_WriteReg( REG_CLOCK_MANUAL, 0x41 );		// Must be written with 0x41 after reset for correct operation
	CYWM_WriteReg( REG_CLOCK_ENABLE, 0x41 );		// Must be written with 0x41 after reset for correct operation
	CYWM_WriteReg( REG_SERDES_CTL, 0x03 | 0x08 );	// Enable SERDES
	CYWM_WriteReg( REG_TX_VALID, 0xFF );			// Set all SERDES bits valid for TX
	CYWM_WriteReg( REG_VCO_CAL, 0xC0 );				// Set VCO adjust to -5/+5
	CYWM_WriteReg( REG_ANALOG_CTL, 0x04 );			// Enable PA Control Output
	//CYWM_WriteReg( REG_PWR_CTL, 0x80 );				// Conserve power (must set REG_ANALOG_CTL, bit 6=1 to enable writes)
	CYWM_WriteReg( REG_CHANNEL, 42 );				// Use channel 42
	CYWM_WriteReg( REG_PA, 0x07 );					// Set maximum transmit power
	CYWM_WriteReg( REG_RX_INT_EN, 0x03 );			// Enable EOF and FULL interrupts for channel A
	if (tipe=='b')		// b = baca
		CYWM_WriteReg( REG_CONTROL, 0x80 );				// Enable RX saja
	else {				// t = tulis		k = kirim
		CYWM_WriteReg( REG_CONTROL, 0x40 );				// Enable TX saja
	}
	portEXIT_CRITICAL();
}

void lihatKonfig() {
	//buang_watchdog();
	printf("Konfig Wireless "); 
	#ifdef ATAS
		printf("KINCIR ATAS");
	#else
		printf("BUNKER BAWAH");
	#endif
	printf("\r\n--------------------------\r\n");

	printf("Reg ID         : 0x%x\r\n", CYWM_ReadReg(REG_ID));
    printf("Clock Manual   : 0x%x\r\n", CYWM_ReadReg(REG_CLOCK_MANUAL));
    printf("Clock Enable   : 0x%x\r\n", CYWM_ReadReg(REG_CLOCK_ENABLE));
    printf("Valid RSSI     : %d  [RSSI max: 31]\r\n", CYWM_ReadReg(REG_RSSI)&0x1F);
    printf("VCO Cal        : 0x%x\r\n", CYWM_ReadReg(REG_VCO_CAL));     // 0x80, 0xC0
    printf("Analog Control : 0x%x\r\n", CYWM_ReadReg(REG_ANALOG_CTL));
    printf("Kanal          : %.3f GHz\r\n", ((float) CYWM_ReadReg(REG_CHANNEL)+2000)/1000.000); 
    printf("PA Bias        : 0x%x\r\n", CYWM_ReadReg(REG_PA));  
    printf("Kontrol RX TX  : 0x%x   mode ", CYWM_ReadReg(REG_CONTROL));
    
    if (CYWM_ReadReg(REG_CONTROL)==0x80) {
		printf("terima [Receiver]\r\n"); 
	} else {
		printf("kirim [Transmitter]\r\n"); 
	}
}



void wireless_putc(char data) {
    // Transmit data
	//_delay_us(10);
	while (!(CYWM_ReadReg( REG_TX_INT_STAT ) & 0x01));
	CYWM_WriteReg( REG_TX_INT_EN, 0x01 );			// Enable EMPTY interrupt
	CYWM_WriteReg( REG_TX_DATA, data);
}

void wireless_puts(char *data) {
	int len, count;

	len = strlen(data);
	for (count = 0; count < len; count++)
		wireless_putc(*(data+count));
}

int wusbc=0;
char wusbin[16];

void cek_wusb(void) {
	unsigned char tes;
	tes = CYWM_ReadReg( REG_RX_INT_STAT );
	if (tes & 0x01 || tes & 0x02) { // FULL A || EOF A
		CYWM_WriteReg( REG_RX_INT_EN, 0x03 );			// Enable EOF and FULL interrupts for channel A
		if (tes & 0x08) { // Valid A
			tes = CYWM_ReadReg( REG_RX_DATA_A );
			//wusb_in(tes);
			printf("%d",tes);
		}
		//data = CYWM_ReadReg( REG_RX_VALID_A );
	}
	
}

void wusb_in(unsigned char c) {
	if(c=='\n' || c=='\r') {
		// eksekusi !!
		printf('\n');

		if (wusbc>0) {
			wusbin[wusbc] = '\0';
			/*
			#ifndef ATAS
				if (strncmp(wusbin, "w_rpm", 5)==0) {
					paket_rpm(wusbin);
				} else
			#endif
				{
					aksinya(wusbin);
				}
			//*/
		}
		wusbc = 0;
		wusbin[0] = '\0';

		#ifdef DEBUG
			printf("isi: %s\r\n", wusbin); 
		#endif
		//sprintf(x, "Wireless$ "); 	transmitString(x);
	} else {
		printf('%c',c);
		//printf("%c",*c);
		wusbin[ wusbc ] = c;
		wusbc++;
	}
}

void paket_rpm(char * data) {
	char *pch;
	pch=strstr(data," ");
	if (pch!=NULL) {
		printf("RPM kincir: %d\r\n", atoi(pch+1));
	}
}

static int kirim_wusb(int argc, char **argv) {
	if (argc < 2)	{
		printf(" ERR:  argument kurang !\r\n");
		printf(" coba: wusb help \r\n");
		
		return 0;
	} else {
		if (strcmp(argv[1], "help") == 0)	{
			printf(" Perintah untuk menampilkan setting file !\r\n");
			printf("    wusb help  : untuk menampilkan ini.\r\n");
		} else {
			unsigned char rssi = CYWM_ReadReg(REG_RSSI);
			if ( (rssi&0x20)  ) {		// && (rssi&0x1F)>10
				CYWM_WriteReg( REG_CONTROL, 0x40 );		// mode kirim
				sprintf(x, "wusb %s", argv[1]);
				wireless_puts(x); wireless_putc('\n');
				printf("Kirim wusb: %s\r\n",argv[1]);
				CYWM_WriteReg( REG_CONTROL, 0x80 );		// mode terima
			}
		}	
	}
}

#ifdef PAKAI_REMOTE_RELAY
void motor_mati(void) {
	PORTB &= ~(_BV(RELAY1));
	PORTB &= ~(_BV(RELAY2));
	__asm__ __volatile__ ("nop");
	_delay_ms(10);
}

void motor_Relay2(void) {
	motor_mati();
	PORTB |= _BV(RELAY2);
	__asm__ __volatile__ ("nop");
	_delay_ms(1);
}

void motor_Relay1(void) {
	motor_mati();
	PORTB |= _BV(RELAY1);
	__asm__ __volatile__ ("nop");
	_delay_ms(1);
}
#endif

#endif

#endif
