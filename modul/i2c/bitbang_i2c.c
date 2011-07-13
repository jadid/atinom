/*
	bit bang I2C
	
	1 April 2010
	
	furkan jadid
	daun biru engineering
	
	dicopy dari http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=77455

*/

//#include <avr/io.h>
//#define F_CPU 16000000UL
//#include <util/delay.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "hardware.h"

#ifdef PAKAI_I2C

#if 0
#define GPIO_SCL	BIT(15)		// P0.15 (SCK)
#define GPIO_SDA	BIT(18)		// P0.18 (MOSI)
#define GPIO_INT	BIT(17)		// P0.17 (MISO)
#define DUMMY		BIT(26)		// led
#endif

#if 0
#define DDR 	DDRC
#define PORT 	PORTC
#define PIN  	PINC
//
#define SDA 1
#define SCL 0

#define SCL_set() (PORT |= _BV(SCL))
#define SCL_clr() (PORT &= ~_BV(SCL))
#define SCL_out() (DDR  |= _BV(SCL))

#define SDA_set() (PORT |= _BV(SDA))
#define SDA_clr() (PORT &= ~_BV(SDA))
#define SDA_read() (PIN  & _BV(SDA))
#define SDA_out() (DDR  |= _BV(SDA))
#define SDA_in()  (DDR  &= ~_BV(SDA))
#endif

// SCL_set diganti dengan input saja
// karena sudah ada R pull up
// untuk mengantisipasi clock stretching
// tetapi jika mau SCL_clr, harus
// dibuat output dulu

#if 0
//#define SCL_set() 	(FIO0SET = GPIO_SCL)
#define SCL_set() 	(FIO0DIR &= ~GPIO_SCL)
#define SCL_clr() 	(FIO0CLR = GPIO_SCL)
#define SCL_out() 	(FIO0DIR |= GPIO_SCL)
#define SCL_in()	(FIO0DIR &= ~GPIO_SCL)
#define SCL_read() 	(FIO0PIN & GPIO_SCL)

#define SDA_set() 	(FIO0SET = GPIO_SDA)
#define SDA_clr() 	(FIO0CLR = GPIO_SDA)
#define SDA_out() 	(FIO0DIR |= GPIO_SDA)
#define SDA_in() 	(FIO0DIR &= ~GPIO_SDA)
#define SDA_read() 	(FIO0PIN & GPIO_SDA)
#endif

static int togle_int;
int i2c_read_register(char addr, char reg, char *val);

#define outc(c) xSerialPutChar(	0, (char ) c)
#define out(s) vSerialPutString(0, s)

/*
void init_gpio_i2c(void)
{	
		FIO0DIR &= ~GPIO_INT;		
		togle_int = 0;
}
//*/



int int_berganti(void)
{
	char a;
	
	//if ( FIO0PIN & GPIO_INT )
	if (I2C_INT_PIN & GPIO_INT )
	{
		//return 1;
		// cek keypad yang ditekan
		out("KEY \r\n");
					if (i2c_read_register(0x68, 0x68, &a))
					{
						out("\r\n Read failed !\r\n");
					}
					else
					{
						out(" Read OK =");
						a = a + '0';
						outc((char) a);
						out(" \r\n");
					}
	}
	else
		return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////
void delay(void)
{
	//_delay_us(10);
	int i;
	int y;
	
	// untuk arm 60 MHz
	// loop 6000 sekitar 750 uS
	// 100 sekitar 15 uS
	// 50 sekitar 7.5 uS
	
	for (i=0; i<50; i++)		// 15 uS
		//y = (FIO0PIN & DUMMY); 
		y = (I2C_PIN & DUMMY); 
}
////////////////////////////////////////////////////////////////////////////////////////////
int i2c_write(unsigned char data)
{
	char index;
	int tot;
	
	delay();
	for(index=0; index<8; index++)
	{
		if(data & 0x80)
			SDA_in();	// akan high, karena pull UP
		else
		{
			SDA_out();	// harusnya langsung HIGH
			//SDA_clr();
		}		
		delay();
		SCL_set();	// SCL_ jadi in dan otomatis HIGH jika tidak streching
		#if 1
		/* cek jika clock strecting */
		tot = 0;
		while ( !SCL_read() )
		{
			delay();
			#ifdef DEBUG_TSC
			outc('s');
			#endif
			tot++;
			if (tot > 100) return -1;
		}		
		delay();
		#endif
		delay();
		SCL_out(); // otomatis going LOW
		delay();
		
		data<<=1;
	}	
	SDA_in();	// akan LOW karena PULL UP, jika tidak ACK
	delay();
	SCL_set();	// SCL jadi in, seharusnya high oleh R pullup
	#if 1
	/* cek jika clock strecting */
	tot = 0;
	while ( !SCL_read() )
	{
			delay();
			#ifdef DEBUG_TSC
			outc('s');
			#endif
			tot++;
			if (tot > 100) return -1;
	}		
	delay();
	#endif
	delay();
	if ( SDA_read() )	// masih high
	{
		SCL_out();	// SCL LOW
		delay();
		return -1;	// tidak ada ack
	}
	else
	{
		SCL_out();	// SCL LOW
		delay();
		return 0;
	}	
}
///////////////////////////////////////////////////////////////////////////////////////////
int i2c_read(unsigned char *pdata, int ack)
{
	SDA_in();
	#ifdef DEBUG_TSC
	outc('Z');
	#endif
	unsigned char data=0x00;
	char index;
	int tot = 0;

	SCL_out(); // LOW
	delay();
	
	for(index=0; index<8; index++)
	{		
		SCL_set(); // IN, HIGH
		#if 1
		/* cek jika clock strecting */
		tot = 0;
		while ( !SCL_read() )
		{
			delay();
			#ifdef DEBUG_TSC
			outc('s');
			#endif
			tot++;
			if (tot > 100) return -1;
		}
		delay();
		#endif
		
		delay();
		data<<=1;		
		if(SDA_read())
		{
			data++;
			#ifdef DEBUG_TSC
			outc('-');
			#endif
		}
		else {
			#ifdef DEBUG_TSC
			outc('x');
			#endif
		}
			
		delay();
		SCL_out();	// LOW, 
		delay();
	}
		
	if (ack)
	{
		SDA_out();
		SDA_clr();
	}
	else
	{
		SDA_in();	// akan high
		//SDA_set();
	}	
	delay();
	SCL_in(); // IN, HIGH
	delay();
	SCL_out(); // LOW
	delay();
	
	*pdata = data;
		
	return 0;
	
}
///////////////////////////////////////////////////////////////////////////////////////////
void i2c_start(void)
{
	int tot;
	
	SCL_set();	// jadi in
	#if 1
	/* cek jika clock strecting */
	tot = 0;
	while ( !SCL_read() )
	{
			delay();
			#ifdef DEBUG_TSC
			outc('a');
			#endif
			tot++;
			if (tot > 100) return -1;
	}		
	delay();
	#endif
		
	SDA_out();
	SDA_set();
	delay();
	SCL_set();	// in
	delay();
	SDA_clr();
	delay();
	SCL_out();
	SCL_clr();
	delay();
}
///////////////////////////////////////////////////////////////////////////////////////////
void i2c_stop(void)
{
	int tot;
	SDA_out();
	delay();
	
	SCL_set();	// jadi in, akan H jika tidak streching
	#if 1
	/* cek jika clock strecting */
	tot = 0;
	while ( !SCL_read() )
	{
			delay();
			#ifdef DEBUG_TSC
			outc('b');
			#endif
			tot++;
			if (tot > 100) return -1;
	}		
	delay();
	#endif
	
	SCL_set();
	delay();
	SDA_set();
	delay();
	
	SDA_in();
} 


int i2c_set_register(char addr, char reg, char val)
{
	char c;
	c = addr << 1;
	
	i2c_start();
	if ( i2c_write(c) )
	{
		i2c_stop();
		return -1;
	}
	if ( i2c_write(reg) )
	{
		i2c_stop();
		return -2;
	}
	if ( i2c_write(val) )
	{
		i2c_stop();
		return -3;
	}
	i2c_stop();
	return 0;
}

int i2c_read_register(char addr, char reg, char *val)
{
	char c;
	//char data;
	int i;
	
	c = addr << 1;
	/* write untuk addres reg */
	i2c_start();
	if ( i2c_write(c) )
	{
		#ifdef DEBUG_TSC
		outc('a');
		#endif
		i2c_stop();
		return -1;
	}
	if ( i2c_write(reg) )
	{
		#ifdef DEBUG_TSC
		outc('B');
		#endif
		i2c_stop();
		return -2;
	}

	c |= 1;			/* read */
	i2c_start();
	if ( i2c_write(c) )
	{
		#ifdef DEBUG_TSC
		outc('C');
		#endif
		i2c_stop();
		return -3;
	}
	if ( i2c_read( val , 0))
	{
		//i2c_stop();
		//return -4;
		#ifdef DEBUG_TSC
		outc('D');
		#endif
	}
	i2c_stop();
	#ifdef DEBUG_TSC
	outc('E');
	#endif
	
	//*val = data;
	return 0;
}

#endif
