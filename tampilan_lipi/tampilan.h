/*
	header tampilan
	furkan jadid
	21 jan 2009
	daun biru engineering
	
	*/
	
#include "FreeRTOS.h"
	
#ifdef TAMPILAN_LPC
#define PF10	BIT(15)		// PF1
#define PF14	BIT(19)		// PF1
#define FIO_KEYPAD FIO1PIN
#endif

#ifdef TAMPILAN_LPC_4
#define PF14	BIT(0)			// P2, 0
#define FIO_KEYPAD FIO2PIN
#endif

#ifdef TAMPILAN_LPC_43
#define PF14	BIT(12)			// P2, 12
#define FIO_KEYPAD FIO2PIN
#endif

#define PF11	BIT(16)
#define PF12	BIT(17)
#define PF13	BIT(18)

#define KEY_DAT	(PF11 | PF12 | PF13)

#define ATAS		0
#define BAWAH	131072
#define KANAN	65536
#define OK		196608
#define CANCEL	262144
