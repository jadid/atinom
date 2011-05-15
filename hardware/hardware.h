#include "FreeRTOS.h"
#include "lpc23xx.h"
#include "task.h"


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
		#define port_cs_ad7708		BIT(6)	/* P0 */
		#define port_rdy_ad7708		BIT(11)	/* P2 */
		
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
#endif


#ifdef BOARD_KOMON_420_SABANG
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
	
	#ifdef PAKAI_ADC
		#define port_cs_ad7708		BIT(17)	/* P1 */
		#define port_rdy_ad7708		BIT(11)	/* P2 */
		
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
