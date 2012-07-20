/*
	1 April 2010
	
	Furkan Jadid
	Daun Biru Engineering
	
	Test untuk touch pad
	
*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define BAUD_RATE	( ( unsigned portLONG ) 115200 )
#define LED_UTAMA	BIT(26)

static int tog;

void init_led_utama(void);
void init_task_serial(void);

#define out(s) vSerialPutString(0, s)
#define outc(c) xSerialPutChar(	0, (char ) c)

/*-----------------------------------------------------------*/
#define jalankan

int main( void )
{
	sysInit();

	FIO0DIR = LED_UTAMA;
	FIO0CLR = LED_UTAMA;	

	xSerialPortInitMinimal( BAUD_RATE, configMINIMAL_STACK_SIZE  );

#ifdef jalankan
	init_led_utama();
	init_task_serial();
	
	vTaskStartScheduler();

    /* Will only get here if there was insufficient memory to create the idle
    task. */
	return 0;
#endif
}

int int_berganti(void);
char read_key(void);

void togle_led_utama(void)
{
	char a;
	if (tog)
	{
		FIO0SET = LED_UTAMA;
		tog = 0;
		
		if (int_berganti() == 0)
		{
				//vSerialPutString(0, "NOL\r\n");
		}
		else
				vSerialPutString(0, "HIGH\r\n");
		
	}
	else
	{
		FIO0CLR = LED_UTAMA;
		tog = 1;		
	}
}

static portTASK_FUNCTION(task_led, pvParameters )
{
	tog = 0;
	//init_gpio_i2c();
	
	
	for (;;)
	{
		togle_led_utama();
		vTaskDelay(100);
	}
}
void init_led_utama(void)
{
	xTaskCreate(task_led, ( signed portCHAR * ) "Led2",  (configMINIMAL_STACK_SIZE * 2) ,\
		 NULL, tskIDLE_PRIORITY - 2, ( xTaskHandle * ) NULL );
}

void proses_c(char * c);

unsigned char i2c_read(unsigned char ACK);
void i2c_write(unsigned char data);
char sda_low();
int test_ping(void);

#define	TSC_WARM_RESET               0xFF



int toggle_scl(void);
int test_kirim(void);
int i2c_set_register(char addr, char reg, char val);
int i2c_read_register(char addr, char reg, char *val);

static portTASK_FUNCTION(task_serial, pvParameters )
{
	int c;
	vTaskDelay(100);
	vSerialPutString(0, "Testing FreeRTOS serial\r\n");
	char a;
	int i;
	
	init_gpio_i2c();
	/*
	vTaskDelay(100);
	if (I2CInit( 1 ) == 0) // master
	{
		out("Init I2c Error !\r\n");
	}		
	else
		out("Init I2C OK !\r\n");
	*/
	
	while(1)
    {
	  	if (xSerialGetChar(1, &c, 0xFFFF ) == pdTRUE)
	  	{
			vSerialPutString(0, "Tombol ditekan = ");
	  		xSerialPutChar(	0, (char ) c);
	  		vSerialPutString(0, " \r\n");
			
			if ( (char) c == 's')
			{
				out(" Set\r\n");
				/*
				if (i2c_set_register(0x68, 1, 8))
				{
					out(" NO ACK !!\r\n");
				}
				else
					out(" OK\r\n");
				*/
				
				if (setup_fma())
				{
					out(" NO ack !\r\n");
				}
				else
					out(" OK ack !\r\n");
					
				
			}
			else
			{
				out("====\r\n");
				for (i=0;i<16; i++)
				{
					if (i == 8) out("****\r\n");
					
					if (i2c_read_register(0x68, (0x50 + i), &a))
					{
						out(" Read failed !\r\n");
					}
					else
					{
						out(" Read OK =");
						a = a + '0';
						outc((char) a);
						out(" \r\n");
					}
				}
					
					out("KEY \r\n");
					if (i2c_read_register(0x68, 0x68, &a))
					{
						out(" Read failed !\r\n");
					}
					else
					{
						out(" Read OK =");
						a = a + '0';
						outc((char) a);
						out(" \r\n");
					}
					//a = read_key();
					//a = a + '0';
					//xSerialPutChar(	0, (char ) a);
			}
			/*
			for (i=0; i<1000000; i++)
			{
				if (toggle_scl() < 0) 
				{
					out("streching !\r\n");
					break;
				}
			}*/
			
			/*
			if (test_ping() == 1)
			{
				out("dapat ACK\r\n");
			}
			else
				out(" errr !\r\n");
			*/
			
			/*
			aa = 0x68;
			aa = (aa << 1);
			i2c_start();
			i2c_write(aa);
			i2c_write( TSC_WARM_RESET );
			i2c_stop();
			
			
			if ( sda_low() )
			{
				vSerialPutString(0, " ACK\r\n");
			}
			else
			vSerialPutString(0, " no resp \r\n");
			*/
	  	}
	}
}

void init_task_serial(void)
{
	xTaskCreate(task_serial, ( signed portCHAR * ) "SERR",  (configMINIMAL_STACK_SIZE * 10) ,\
		 NULL, tskIDLE_PRIORITY, ( xTaskHandle * ) NULL );
}
