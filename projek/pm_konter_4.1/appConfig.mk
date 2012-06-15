


//#define _printf	printf2


//#define UNTUK_UNSRI
//#define UNTUK_MONITA_KAPAL

#define PAKAI_SHELL
#define PAKAI_LED_UTAMA
//#define PAKAI_MODE_POWER

#define BANYAK_SUMBER
#define CENDOL
//#define PAKAI_RTC
//#define PAKAI_ADC_ORI
//#define PAKAI_PUSHBUTTON
#define HITUNG_RPM

//#define PAKAI_PUSHBUTTON
//#define PAKAI_RELAY

//#define PAKAI_TIMER_2		// CRON
#ifdef  PAKAI_TIMER_2
	//#define PAKAI_CRON
#endif

#ifdef PAKAI_MODE_POWER
	#define PAKAI_RTC
#endif

#ifdef PAKAI_RTC
	#define PAKAI_MEM_RTC
	#define PAKAI_KONTROL_RTC
	//#define TES_MEM_RTC
#endif

#define PAKAI_ETH
#ifdef PAKAI_ETH
//	#define PAKAI_ENC28J60		// bukan untuk modul sabang 2.3
	#define PAKAI_ENCX24J600
	#define PAKAI_ETH_POWERDOWN

//	#define PAKAI_MODBUSTCP
	#define SAMPURASUN_CLIENT
//	#define SAMPURASUN_SERVER
	#define PAKAI_WEBCLIENT
	#define PAKE_TELNETD
	#define PAKAI_HTTP
//	
	
	#ifdef PAKAI_WEBCLIENT
//		#define WEBCLIENT_DATA
		#ifdef PAKAI_GPS
			#define WEBCLIENT_GPS
		#endif
		
		//#define PAKAI_WEBCLIENT_INTERNET
		#ifdef PAKAI_WEBCLIENT_INTERNET
			#define PAKAI_RESOLV
		#endif
	#endif
#endif

#define PAKAI_SERIAL_3
#ifdef PAKAI_SERIAL_3
//	#define PAKAI_SERIAL_3_P0	115200		// TES
//	#define PAKAI_SERIAL_3_P0	38400		// max PM810: 38400
	#define PAKAI_SERIAL_3_P0	19200		// max MICOM: 19200
//	#define PAKAI_SERIAL_3_P0	9600		// max TFX  :  9600

	#define PAKAI_MODBUS
	#define PAKAI_MODBUS_RTU
	#define PAKAI_MAX485		1
	
	#define AMBIL_PM
	#define PAKAI_PM			3

	#ifdef AMBIL_PM
	
	#endif

	#ifdef PAKAI_PM
		#define TIPE_PM810
		//#define TIPE_PM710
		//#define TIPE_MICOM_M300
		//#define TIPE_MICOM_P127
		#define TIPE_TFX_ULTRA
	#endif
#endif

//#define PAKAI_SERIAL_2
#ifdef PAKAI_SERIAL_2
	#define PAKAI_SERIAL_2_P0		9600
	#define KIRIM_KE_SER_2
//	#define PAKAI_GPS				2
//	#define DEBUG_GPS
#endif
