
#include "monita/monita_uip.h"

#define JML_ADC		2

#ifdef UNTUK_UNSRI
	#define JML_ADC		2
#endif

#define ADC_CLKDIV	(20 << 8)
#define ADC_PDN		BIT(21)
#define ADC_START	BIT(24)
#define ADC_0		BIT(0)
#define ADC_1		BIT(1)
#define ADC_2		BIT(2)
#define ADC_3		BIT(3)
#define ADC_4		BIT(4)
#define ADC_5		BIT(5)
#define ADC_6		BIT(6)
#define ADC_7		BIT(7)


#define ADC_DONE	BIT(31)
#define ADC_BURST	BIT(16)

#define koef_iir_K	0.03125		// 1/32
//#define koef_iir_K	0.0078125		// 1/128
#define koef_iir_K	0.125		// 1/8
#define koef_iir_K2	0.15

struct t_adc_ori {
	unsigned int cur_kanal;
	unsigned int count;
	unsigned short data[JML_ADC];
	unsigned short data_lama[JML_ADC];
	unsigned short data_masuk[JML_ADC];
	//float flt_data[JML_ADC];
};

struct t_adc_ori adco;

void init_adc_ori(void);



#if defined(BOARD_KOMON_KONTER)
	int proses_data_adc_ori(char kanalx);
#endif
