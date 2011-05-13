/*
	low level SSP1
	6 juni 2008
	
	furkan jadid, daun biru engineering
	
	21 Juli 2009
	porting untuk tampilan
	menggunakan SSP1
	*/

#include "FreeRTOS.h"

//#define SPI_SPCCR	S0SPCCR
//#define SPI_SPCR	S0SPCR
//#define SPI_SPSR	S0SPSR
//#define SPI_SPDR	S0SPDR
#define SPI_SPIF	0x80		// bit 7 status
#define MSTR		0x20		// bit 5

#define port_cs_mmc	BIT(1)		// port 0.1

#define GPIO_IOSET	FIO0SET
#define GPIO_IOCLR	FIO0CLR
#define GPIO_IODIR	FIO0DIR

//SSP1
#define CPOL	0x40
#define SSP1TNF	0x02
#define SSP1BSY	0x10
#define SSP1RNE	0x04

//#define  init_ss1()	init_gpio_mmc()

void init_spi_mmc(char min);
void kirim_word_mmc(unsigned char t);
unsigned char ambil_word_mmc(void);
void tx_busy_mmc(void);
inline void rx_busy_mmc(void);
void spi_tx_mmc(unsigned char txd);
unsigned char spi_rx_mmc(void);


void init_gpio_mmc(void)
{
	PCONP |= BIT(10);
	PINSEL0 &= ~(BIT(2) | BIT(3));
	
	PINSEL0 &= ~(BIT(14) | BIT(15) | BIT(16) | BIT(17) | BIT(18) | BIT(19));
	PINSEL0 |= BIT(15) | BIT(17) | BIT(19);
	
	GPIO_IODIR |= port_cs_mmc;
	GPIO_IOSET  = port_cs_mmc;
}


void cs_mmc(void)
{	GPIO_IOCLR = port_cs_mmc;
}

void uncs_mmc(void)
{
	GPIO_IOSET = port_cs_mmc;
}

void init_spi_mmc(char min)	
{
	// jika min = 1, berarti awal konfigurasi mmc ( < 300 kHz )
	portENTER_CRITICAL();
	unsigned char p;
	int i;

	//disable dulu
	SSP1CR1 = 0x00;
   	
   	if (min == 1)
   	{
   		 /* enable clock to SSP0 for security reason. By default, it's enabled already */
  		//PCONP |= (1 << 21);
   		//SSP1CR0 = 0x6000 | 0x07;		// dapat sekitar 300 kHz
   		SSP1CR0 = 0x6000 | 0x07; // | BIT(6) | BIT(7);
   	}
   	else
   		//SSP1CR0 = 0x400 | 0x07;		// SCR = 0x400 dapat 5 Mhz, cocok karena 60 Mhz / (2 * (4+1))
   		SSP1CR0 = 0x200 | 0x07;// | BIT(6) | BIT(7);		// SCR = 0x200 dapat 10 Mhz, cocok karena 60 Mhz / (2 * (2+1))
   	
   	//SSP1CR0 = 0x0707;
   	// clock = PCLK / (CPSDVSR * (SCR + 1))
  
  	// CS manual
  	//PINSEL0 |= 0x000A8000;
  	
  	// CS otomatis
  	//PINSEL0 |= 0x000AA000;
  	
  	 /* SSPCPSR clock prescale register, master mode, minimum divisor is 0x02 */
  	//SSP1CPSR = 0x2;
	SSP1CPSR = 0x2;
	
	SSP1IMSC = 0x00;
	
	// clear fifo
	for (i=0; i<8; i++)
		p = SSP1DR;
	
	SSP1CR1 = 0x02; //enable 
	portEXIT_CRITICAL();
}

void kirim_word_mmc(unsigned char t)
{
	while ( (SSP1SR & (SSP1TNF|SSP1BSY)) != SSP1TNF );
	SSP1DR = t;
	
	while ( (SSP1SR & (SSP1BSY|SSP1RNE)) != SSP1RNE );
	/* Whenever a byte is written, MISO FIFO counter increments, Clear FIFO 
	on MISO. Otherwise, when SSP0Receive() is called, previous data byte
	is left in the FIFO. */
	t = SSP1DR;
	return;
}

unsigned char ambil_word_mmc(void)
{	
	SSP1DR = 0xFF;
	/* Wait until the Busy bit is cleared */
	while ( (SSP1SR & (SSP1BSY|SSP1RNE)) != SSP1RNE );
	
	return (unsigned char) SSP1DR;
}

/*
void kirim_word_mmc(unsigned char t)
{  	
  	tx_nempty_mmc();
  	spi_tx_mmc(t);				//langsung initiate transfer
   	tx_busy_mmc();
} 

unsigned char ambil_word_mmc(void)
{
	spi_tx_mmc(0xFF);		//dummy send
	//tx_busy_mmc();
	while ( !(SSP1SR & SSPSR_RNE) );
	return spi_rx_mmc();		
}
*/
/*
void tx_nempty_mmc(void)
{
	while (!(SSP1SR & SSP1TNF)); 
}

void tx_busy_mmc(void)
{
	while (!(SSP1SR & SSP1BSY)); 
}

void spi_tx_mmc(unsigned char txd)
{
	unsigned short pp;
	pp = txd;
	//SSP1DR = txd;
	SSP1DR = (unsigned short) pp;
}

unsigned char spi_rx_mmc(void)
{
	unsigned short rr;
	
	rr = SSP1DR;
	
	return (rr & 0xFF);
	//return SSP1DR;
}*/
