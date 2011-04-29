#include "FreeRTOS.h"
#include "lpc23xx.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

void setup_hardware(void);

#ifdef BOARD_KOMON_420_SABANG_2_3
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

	#ifdef PAKAI_ETH
		#define CS_ENC	BIT(16)	/* P0 */
		#define INT_ENC	BIT(13) /* P2 */
		
		#define ENCX24J600_Unreset()  FIO0SET = CS_ENC

		#define FIO_CEK_PAKET		FIO2PIN

		#define init_enc_port()		FIO2DIR = FIO2DIR & ~(INT_ENC); \
									FIO0DIR = FIO0DIR | CS_ENC;
									
		#define ENCX24J600_Reset()    FIO0CLR = CS_ENC
		#define ENCX24J600_Unreset()  FIO0SET = CS_ENC
		
		#define AssertChipSelect()   FIO0CLR = CS_ENC  // P0.16		// ENC28J60_Select()
		#define DeassertChipSelect() FIO0SET = CS_ENC				// ENC28J60_Deselect()
	#endif

#endif
