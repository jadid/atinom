#include "FreeRTOS.h"
#include "lpc23xx.h"
#include "task.h"


void setup_hardware(void);
void dele(int dd);

#ifdef BOARD_KOMON_420_SABANG_2_3
	#define uC	"NXP LPC 2387"
	
	#ifdef PAKAI_LED_UTAMA
		#define LED_UTAMA	BIT(27)
		
		#define setup_led_utama()	do {	\
										FIO0DIR = LED_UTAMA;	\
										FIO0CLR = LED_UTAMA;	\
									} while(0)
	#endif

	#ifdef PAKAI_SHELL
		#define BAUD_RATE_SHELL	( ( unsigned portLONG ) 115200 )
	#endif

	#ifdef PAKAI_MODBUS_RTU
		#define TXDE	BIT(24)
		#define RXDE	BIT(23)
	#endif
	
	#ifdef PAKAI_SERIAL_2_P0
		#define setup_serial2_P0()	do 	{	\
										PCONP |= BIT(24);	\
										PCLKSEL1 &= ~(BIT(16) | BIT(17));	\
										PCLKSEL1 |= BIT(16);	\
										PINSEL0 |= (BIT(20) | BIT(22));	\
									} while(0)
	#endif
	
	#ifdef PAKAI_SERIAL_3_P0
		#define setup_serial3_P0()	do 	{	\
										PINSEL1 &= ~(BIT(18) | BIT(19) | BIT(20) | BIT(21));		\
										PINSEL1 |= (BIT(18) | BIT(19));								\
										PINSEL1 |= (BIT(20) | BIT(21));								\
										PINSEL1 &= ~(BIT(16) | BIT(17));							\
										FIO0DIR |= TXDE;					\
										FIO0SET = TXDE;						\
										FIO0DIR |= RXDE;					\
										FIO0SET  = RXDE;					\
									} while(0)
	#endif
	
	#ifdef PAKAI_ETH
		#define CS_ENC					BIT(16)	/* P0 */
		#define INT_ENC					BIT(13) /* P2 */
		
		#define ENCX24J600_Unreset()  	FIO0SET = CS_ENC

		#define FIO_CEK_PAKET			FIO2PIN

		#define init_enc_port()		do {	\
										FIO2DIR = FIO2DIR & ~(INT_ENC); \
										FIO0DIR = FIO0DIR | CS_ENC;	\
									} while (0);
									
		#define ENCX24J600_Reset()		FIO0CLR = CS_ENC
		#define ENCX24J600_Unreset()	FIO0SET = CS_ENC
		
		#define AssertChipSelect()		FIO0CLR = CS_ENC  // P0.16		// ENC28J60_Select()
		#define DeassertChipSelect()	FIO0SET = CS_ENC				// ENC28J60_Deselect()
	#endif
	
	#ifdef PAKAI_ADC
		#define port_cs_ad7708		BIT(6)	// P0.6  //
		#define port_rdy_ad7708		BIT(11)	// P2.11 //
	
		#define ad7708_select()		FIO0CLR = port_cs_ad7708
		#define ad7708_deselect()	FIO0SET = port_cs_ad7708
		
		//#define rate_7708 	25			// 55 data per detik
		#define rate_7708		71			// 20 data per detik

		#define	UNIPOLAR		0x08		// nilai positif saja
		//#define range_RTD		(4 | UNIPOLAR)	// 0 - 320 mV

		#define range_RTD		(5 | UNIPOLAR)	// 0 - 640 mV
		#define range_420		(7 | UNIPOLAR)	// 0 - 2.5 V

		//#define range_adc		13				// 0 - 640 mV
			
		//#define faktor_pengali_RTD		0.320
		#define faktor_pengali_RTD		0.640
		#define faktor_pengali_420		2.5
		
		#define AD7708_LPC_KOMON
		
		#define setup_adc()		do {	\
										FIO0DIR = FIO0DIR | port_cs_ad7708;	\
										FIO2DIR = FIO2DIR & ~port_rdy_ad7708;	\
										uncs_ad7708();	\
									} while(0);
	#endif

	#ifdef PAKAI_TIMER2
		#define setup_timer2()		do	{	\
										PCONP |= BIT(22);		\
										PCLKSEL1 &= ~(BIT(12) | BIT(13));	\
										PCLKSEL1 |= BIT(12);				\
									} while(0)
	#endif
#endif


#ifdef BOARD_KOMON_420_SABANG
	#define uC	"NXP LPC 2387"
	
	#ifdef PAKAI_LED_UTAMA
		#define LED_UTAMA	BIT(27)
		
		#define setup_led_utama()	do {	\
										FIO0DIR = LED_UTAMA;	\
										FIO0CLR = LED_UTAMA;	\
									} while(0)
	#endif
	
	#ifdef PAKAI_SHELL
		#define BAUD_RATE_SHELL	( ( unsigned portLONG ) 115200 )
	#endif

	#ifdef PAKAI_SERIAL_2_P0
		#define setup_serial2_P0()	do 	{	\
										PCONP |= BIT(24);	\
										PCLKSEL1 &= ~(BIT(16) | BIT(17));	\
										PCLKSEL1 |= BIT(16);	\
										PINSEL0 |= (BIT(20) | BIT(22));	\
									} while(0)
	#endif
	
	#ifdef PAKAI_SERIAL_3_P0
		#define setup_serial3_P0()	do 	{	\
										PINSEL1 &= ~(BIT(18) | BIT(19) | BIT(20) | BIT(21));		\
										PINSEL1 |= (BIT(18) | BIT(19));								\
										PINSEL1 |= (BIT(20) | BIT(21));								\
										PINSEL1 &= ~(BIT(16) | BIT(17));							\
										FIO0DIR |= TXDE;					\
										FIO0SET = TXDE;						\
										FIO0DIR |= RXDE;					\
										FIO0SET  = RXDE;					\
									} while(0)
	#endif
	
	#ifdef PAKAI_MODBUS_RTU
		#define TXDE	BIT(24)
		#define RXDE	BIT(23)
	#endif
	
	#ifdef PAKAI_ADC
		#define port_cs_ad7708		BIT(17)	/* P1 */
		#define port_rdy_ad7708		BIT(11)	/* P2 */

		#define ad7708_select()		FIO1CLR = port_cs_ad7708
		#define ad7708_deselect()	FIO1SET = port_cs_ad7708
		
		//#define rate_7708 	25			// 55 data per detik
		#define rate_7708		71			// 20 data per detik

		#define	UNIPOLAR		0x08
		//#define range_RTD		(4 | UNIPOLAR)	// 0 - 320 mV

		#define range_RTD		(5 | UNIPOLAR)	// 0 - 640 mV
		#define range_420		(7 | UNIPOLAR)	// 0 - 2.5 V

		//#define range_adc		13				// 0 - 640 mV
			
		//#define faktor_pengali_RTD		0.320
		#define faktor_pengali_RTD		0.640
		#define faktor_pengali_420		2.5

		#define AD7708_LPC_KOMON
		
		#define setup_adc()		do {	\
									FIO1DIR = FIO1DIR | port_cs_ad7708;		\
									FIO2DIR = FIO2DIR & ~port_rdy_ad7708;	\
									uncs_ad7708();							\
								} while (0);
	#endif
	
#endif

#ifdef BOARD_TAMPILAN_4_2
	#ifdef PAKAI_LED_UTAMA
		#define LED_UTAMA	BIT(27)
		
		#define setup_led_utama()	do {	\
										FIO0DIR = LED_UTAMA;	\
										FIO0CLR = LED_UTAMA;	\
									} while(0)
	#endif
	
	#ifdef PAKAI_SHELL
		#define BAUD_RATE_SHELL	( ( unsigned portLONG ) 115200 )
	#endif
	
	#ifdef PAKAI_I2C
		#define ADDR_FMA1	0x68
	
		#ifdef I2C_GPIO
			#define GPIO_SCL	BIT(17)		// P1.17
			#define GPIO_SDA	BIT(18)		// P1.18
			#define GPIO_INT	BIT(0)		// P2.0
			#define DUMMY		BIT(27)		// led
			
			#define I2C_DIR		FIO1DIR
			#define I2C_CLR		FIO1CLR
			#define I2C_PIN		FIO1PIN
			#define I2C_SET		FIO1SET
			#define I2C_INT_DIR		FIO2DIR
			#define I2C_INT_PIN		FIO2PIN
		#endif
		
		#define setup_gpio_i2c()	(I2C_INT_DIR &= ~GPIO_INT)
		
		#define SCL_set() 	(I2C_DIR &= ~GPIO_SCL)
		#define SCL_clr() 	(I2C_CLR =   GPIO_SCL)
		#define SCL_out() 	(I2C_DIR |=  GPIO_SCL)
		#define SCL_in()	(I2C_DIR &= ~GPIO_SCL)
		#define SCL_read() 	(I2C_PIN &   GPIO_SCL)

		#define SDA_set() 	(I2C_SET =   GPIO_SDA)
		#define SDA_clr() 	(I2C_CLR =   GPIO_SDA)
		#define SDA_out() 	(I2C_DIR |=  GPIO_SDA)
		#define SDA_in() 	(I2C_DIR &= ~GPIO_SDA)
		#define SDA_read() 	(I2C_PIN &   GPIO_SDA)
	#endif
#endif

#ifdef BOARD_TAMPILAN_4_3
	#define uC	"NXP LPC 2387"
	#define FIO_KEYPAD 			FIO2PIN
	
	
	#ifdef PAKAI_BACKLIT
		#define LED_PICKUP			BIT(14)
		#define BACKLIT				BIT(20)	// PF15, P1.20
	
		#define setup_backlit()	do {	\
										FIO1DIR  = FIO1DIR | LED_PICKUP | BACKLIT;		\
										FIO1MASK = FIO1MASK & ~(LED_PICKUP | BACKLIT);	\
								} while(0)
	#endif
	
	#define PF14				BIT(12)			// P2, 12
	#define PF11				BIT(16)
	#define PF12				BIT(17)
	#define PF13				BIT(18)
	
	#define KEY_DAT	(PF11 | PF12 | PF13)

	#define ATAS		0
	#define BAWAH	131072
	#define KANAN	65536
	#define OK		196608
	#define CANCEL	262144

	#ifdef PAKAI_LED_UTAMA
		#define LED_UTAMA	BIT(27)
		
		#define setup_led_utama()	do {	\
										FIO0DIR = LED_UTAMA;	\
										FIO0CLR = LED_UTAMA;	\
									} while(0)
	#endif
	
	#ifdef PAKAI_SHELL
		#define BAUD_RATE_SHELL	( ( unsigned portLONG ) 115200 )
	#endif
	
	#ifdef PAKAI_RELAY
		#define RLY_1	BIT(26)			/* P3 */
		#define RLY_2	BIT(25)

		#define RLY_3	BIT(23)			/* P1 */
		#define RLY_4	BIT(21)	
		#define RLY_5	BIT(15)

		#define RLY_6	BIT(29)			/* P4 */
		#define RLY_7	BIT(28)

		#define RLY_8	BIT(7)			/* P2 */
		
		#define cRelay1()		FIO3CLR = RLY_1
		#define cRelay2()		FIO3CLR = RLY_2
		#define cRelay3()		FIO1CLR = RLY_3
		#define cRelay4()		FIO1CLR = RLY_4
		#define cRelay5()		FIO1CLR = RLY_5
		#define cRelay6()		FIO4CLR = RLY_6
		#define cRelay7()		FIO4CLR = RLY_7
		#define cRelay8()		FIO2CLR = RLY_8
		
		#define sRelay1()		FIO3SET = RLY_1
		#define sRelay2()		FIO3SET = RLY_2
		#define sRelay3()		FIO1SET = RLY_3
		#define sRelay4()		FIO1SET = RLY_4
		#define sRelay5()		FIO1SET = RLY_5
		#define sRelay6()		FIO4SET = RLY_6
		#define sRelay7()		FIO4SET = RLY_7
		#define sRelay8()		FIO2SET = RLY_8

		#define setup_relay()		do {	\
										FIO3DIR = FIO3DIR | RLY_1 | RLY_2;				\
										FIO1DIR = FIO1DIR | RLY_3 | RLY_4 | RLY_5;		\
										FIO4DIR = FIO4DIR | RLY_6 | RLY_7;				\
										FIO2DIR = FIO2DIR | RLY_8;						\
									} while(0)
	#endif
	
	#ifdef PAKAI_I2C
		#ifdef I2C_GPIO
			#define GPIO_SCL	BIT(17)		// P1.17
			#define GPIO_SDA	BIT(18)		// P1.18
			#define GPIO_INT	BIT(0)		// P2.0
			#define DUMMY		BIT(27)		// led
			
			#define I2C_DIR		FIO1DIR
			#define I2C_CLR		FIO1CLR
			#define I2C_PIN		FIO1PIN
			#define I2C_SET		FIO1SET
			#define I2C_INT_DIR		FIO2DIR
			#define I2C_INT_PIN		FIO2PIN
		#endif
		
		#define setup_gpio_i2c()	(I2C_INT_DIR &= ~GPIO_INT)
		
		#define SCL_set() 	(I2C_DIR &= ~GPIO_SCL)
		#define SCL_clr() 	(I2C_CLR =   GPIO_SCL)
		#define SCL_out() 	(I2C_DIR |=  GPIO_SCL)
		#define SCL_in()	(I2C_DIR &= ~GPIO_SCL)
		#define SCL_read() 	(I2C_PIN &   GPIO_SCL)

		#define SDA_set() 	(I2C_SET =   GPIO_SDA)
		#define SDA_clr() 	(I2C_CLR =   GPIO_SDA)
		#define SDA_out() 	(I2C_DIR |=  GPIO_SDA)
		#define SDA_in() 	(I2C_DIR &= ~GPIO_SDA)
		#define SDA_read() 	(I2C_PIN &   GPIO_SDA)
	#endif
#endif


#ifdef BOARD_KOMON_KONTER
	#define uC	"NXP LPC 2387"
	
	#ifdef PAKAI_LED_UTAMA
		#define LED_UTAMA	BIT(27)
		
		#define setup_led_utama()	do {	\
										FIO0DIR = LED_UTAMA;	\
										FIO0CLR = LED_UTAMA;	\
									} while(0)
	#endif
	
	#ifdef PAKAI_SHELL
		#define BAUD_RATE_SHELL	( ( unsigned portLONG ) 115200 )
	#endif

	#ifdef PAKAI_SERIAL_2_P0
		#define setup_serial2_P0()	do 	{	\
										PCONP |= BIT(24);	\
										PCLKSEL1 &= ~(BIT(16) | BIT(17));	\
										PCLKSEL1 |= BIT(16);	\
										PINSEL0 |= (BIT(20) | BIT(22));	\
									} while(0)
	#endif
	
	#ifdef PAKAI_ETH
		#define CS_ENC	BIT(18)
		#define INT_ENC	BIT(17)

		#define ENC28J60_Select()   FIO1CLR = CS_ENC  // P1.18
		#define ENC28J60_Deselect() FIO1SET = CS_ENC

		// seharusnya tidak ada pin reset (sudah disambung ke VCC)
		#define ENC28J60_Reset()    FIO1CLR = CS_ENC
		#define ENC28J60_Unreset()  FIO1SET = CS_ENC

		#define FIO_CEK_PAKET		FIO1PIN
		#define init_enc_port()		do 	{	\
										FIO1DIR = FIO1DIR & ~(INT_ENC); \
										FIO1DIR = FIO1DIR | CS_ENC;	\
									} while(0)
	#endif
#endif

#ifdef BOARD_KOMON_KONTER_3_0
	#define uC	"NXP LPC 2387"
	
	#ifdef PAKAI_LED_UTAMA
		#define LED_UTAMA	BIT(27)
		
		#define setup_led_utama()	do {	\
										FIO0DIR = LED_UTAMA;	\
										FIO0CLR = LED_UTAMA;	\
									} while(0)
	#endif
	
	#ifdef PAKAI_SHELL
		#define BAUD_RATE_SHELL	( ( unsigned portLONG ) 115200 )
	#endif

	#ifdef PAKAI_SERIAL_2_P0
		#define setup_serial2_P0()	do 	{	\
										PCONP |= BIT(24);	\
										PCLKSEL1 &= ~(BIT(16) | BIT(17));	\
										PCLKSEL1 |= BIT(16);	\
										PINSEL0 |= (BIT(20) | BIT(22));	\
									} while(0)
	#endif
	
	#ifdef PAKAI_ETH
		#define CS_ENC	BIT(18)
		#define INT_ENC	BIT(17)

		#define ENC28J60_Select()   FIO1CLR = CS_ENC  // P1.18
		#define ENC28J60_Deselect() FIO1SET = CS_ENC

		// seharusnya tidak ada pin reset (sudah disambung ke VCC)
		#define ENC28J60_Reset()    FIO1CLR = CS_ENC
		#define ENC28J60_Unreset()  FIO1SET = CS_ENC

		#define FIO_CEK_PAKET		FIO1PIN
		#define init_enc_port()		do 	{	\
										FIO1DIR = FIO1DIR & ~(INT_ENC); \
										FIO1DIR = FIO1DIR | CS_ENC;	\
									} while(0)
	#endif
#endif


#ifdef BOARD_KOMON_KONTER_3_1


	#ifdef PAKAI_ETH
		#define CS_ENC	BIT(18)

		#define INT_ENC	BIT(13)


		#define ENC28J60_Select()   FIO1CLR = CS_ENC  // P1.18
		#define ENC28J60_Deselect() FIO1SET = CS_ENC

		// seharusnya tidak ada pin reset (sudah disambung ke VCC)
		#define ENC28J60_Reset()    FIO1CLR = CS_ENC
		#define ENC28J60_Unreset()  FIO1SET = CS_ENC

		#define FIO_CEK_PAKET		FIO2PIN

		#define init_enc_port()		FIO2DIR = FIO2DIR & ~(INT_ENC); \
									FIO1DIR = FIO1DIR | CS_ENC;
	#endif
#endif
