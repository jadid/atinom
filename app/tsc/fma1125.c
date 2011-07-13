/*
	FMA1125 Touch controller
	implementasi untuk babelan dan blackfin
	
	menggunakan bitbang i2c
	
	furkan jadid
	5 April 2010
	daun biru engineering
	
*/

#include "FreeRTOS.h"
#include "fma1125.h"
//#include "hardware.h"

#include "../../tinysh/enviro.h"

#define ADDR_FMA1	0x68

#ifdef PAKAI_TSC

#define TSC_FILTER_EN    0x01   // Mode selection register values
#define TSC_APIS_1       0x02   // Mode 1 / only strongest sensor output is signaled
#define TSC_APIS_2       0x04   // Mode 2 / reports all outputs that exceeds pre-defined thresholds
#define TSC_APIS_3       0x08   // Mode 3 / reports two strongest outputs - for multitouch applications

//#define VERSI_BARITO	36

//#ifdef VERSI_BARITO
#if 0
const unsigned char tsc_init_data[] = {   // This only contains the writable TSC registers.
	30, // 0x00: PA0 Alpha, 30
	#if ( VERSI_BARITO == 36)
	30, // 0x01: PA1 Alpha
	#else
	4, // 0x01: PA1 Alpha
	#endif
	30, // 0x02: PA2 Alpha
	30, // 0x03: PA3 Alpha
	30, // 0x04: PA4 Alpha
	30, // 0x05: PA5 Alpha		//10 mei 2011, 30
	#if ( VERSI_BARITO == 36)
	30, // 0x06: PA6 Alpha
	#else
	31, // 0x06: PA6 Alpha
	#endif
	30, // 0x07: PA7 Alpha, 30
	20, // 0x08: Reference Delay
	10,  // 0x09: Beta, 4
	39, // 0x0A: AIC Wait Time
	5,  // 0x0B: PA0 Strength Threshold, 30
	5,  // 0x0C: PA1 Strength Threshold
	5,  // 0x0D: PA2 Strength Threshold
	5,  // 0x0E: PA3 Strength Threshold
	5,  // 0x0F: PA4 Strength Threshold
	5,  // 0x10: PA5 Strength Threshold
	#if ( VERSI_BARITO == 36)
	5,  // 0x11: PA6 Strength Threshold, 5
	#else
	99,  // 0x11: PA6 Strength Threshold, 5
	#endif
	5,  // 0x12: PA7 Strength Threshold
	TSC_APIS_1,  // 0x13: Feature
	100,  // 0x14: Integration Time, 128
	0x1f, // 0x15: IDLE State Enter Time
	0x20, // 0x16: Control 1
	0x01, // 0x17: Control 2, falling edge
	0x00, // 0x18: PA Data Out
	0x00, // 0x19: GPIO Data Out
	0x00, // 0x1A: PA Direction (0 = OUTPUT, 1 = INPUT)
	0x00, // 0x1B: GPIO Direction (0 = OUTPUT, 1 = INPUT)
	0x80, // 0x1C: PA Configuration (0 = TOUCH INPUT, 1 = GPIO)
	0x00, // 0x1D: GPIO Configuration (0 = Direct Touch Output, 1 = GPIO)
	0x20, // 0x1E: Calibration Interval
	0xfe, // 0x1F: GINT Interrupt Mask (1 = MASKED)
	0xff, // 0x20: GINT Interrupt Pending Clear
	0x00, // 0x21: PA EINT Enable
	0x00, // 0x22: GPIO EINT Enable
	0x05, // 0x23: Filter Period
	0x04, // 0x24: Filter Threshold
	0x00, // 0x25: Control 3
	0x7F, // 0x26: GINT Interrupt Edge Enable (7E)
	0x00, // 0x27: GPIO Input Bouncing Canceling Period
	0xFF, // 0x28: Register Check, tadinya 0x00
	#if ( VERSI_BARITO == 36)
	0x06, // 0x29: PA0_R_SEL~PA3_ R_SEL
	0xBD, // 0x2A: PA4_R_SEL~PA7_ R_SEL // 10 mei 2011, B9
	0x03, // 0x2B: REF_R_SEL
	#else
	0x3F, // 0x29: PA0_R_SEL~PA3_ R_SEL
	0x00, // 0x2A: PA4_R_SEL~PA7_ R_SEL
	0x01, // 0x2B: REF_R_SEL
	#endif
	0x80, // 0x2C: Beta Disable
	0x00, // 0x2D: GPIO0~GPIO1 Dimming Unit Period
	0x00, // 0x2E: GPIO2~GPIO3 Dimming Unit Period
	0x00, // 0x2F: PA0~PA1 Dimming Unit Period
	0x00, // 0x30: PA2~PA3 Dimming Unit Period
	0x00, // 0x31: GPIO0 Dimming Control
	0x00, // 0x32: GPIO1 Dimming Control
	0x00, // 0x33: GPIO2 Dimming Control
	0x00, // 0x34: GPIO3 Dimming Control
	0x00, // 0x35: PA0 Dimming Control
	0x00, // 0x36: PA1 Dimming Control
	0x00, // 0x37: PA2 Dimming Control
	0x00, // 0x38: PA3 Dimming Control
	0x00, // 0x39: GPIO0~GPIO3 Dimming Mode
	0x00, // 0x3A: PA0~PA3 Dimming Mode
	0x00, // 0x3B: Dimming Start
	0x00  // 0x3C: Dimming Enable
};
#endif

#if 1
const unsigned char tsc_init_data[] = {   // This only contains the writable TSC registers.
	30, // 0x00: PA0 Alpha		// 30
	30, // 0x01: PA1 Alpha
	30, // 0x02: PA2 Alpha
	30, // 0x03: PA3 Alpha
	30, // 0x04: PA4 Alpha
	1, // 0x05: PA5 Alpha
	30, // 0x06: PA6 Alpha
	30, // 0x07: PA7 Alpha
	20, // 0x08: Reference Delay	90
	10,  // 0x09: Beta
	39, // 0x0A: AIC Wait Time		// 0x27
	5,  // 0x0B: PA0 Strength Threshold: 1/10 ss/d 1/20 Int time		// 30
	5,  // 0x0C: PA1 Strength Threshold
	5,  // 0x0D: PA2 Strength Threshold
	5,  // 0x0E: PA3 Strength Threshold
	5,  // 0x0F: PA4 Strength Threshold
	5,  // 0x10: PA5 Strength Threshold
	5,  // 0x11: PA6 Strength Threshold
	5,  // 0x12: PA7 Strength Threshold
	(TSC_APIS_1),  // 0x13: Feature		TSC_APIS_1		// TSC_APIS_2 + TSC_FILTER_EN
	100,  // 0x14: Integration Time			// 128
	0x1f, // 0x15: IDLE State Enter Time
	0x20, // 0x16: Control 1
	0x01, // 0x17: Control 2
	0x00, // 0x18: PA Data Out
	0x00, // 0x19: GPIO Data Out
	0x00, // 0x1A: PA Direction (0 = OUTPUT, 1 = INPUT)
	0x00, // 0x1B: GPIO Direction (0 = OUTPUT, 1 = INPUT)
	0x80, // 0x1C: PA Configuration (0 = TOUCH INPUT, 1 = GPIO) : as input PA
	0x00, // 0x1D: GPIO Configuration (0 = Direct Touch Output, 1 = GPIO)
	0x20, // 0x1E: Calibration Interval
	0xfe, // 0x1F: GINT Interrupt Mask (1 = MASKED)
	0xff, // 0x20: GINT Interrupt Pending Clear
	0x00, // 0x21: PA EINT Enable : 0x00: disable interrupt
	0x00, // 0x22: GPIO EINT Enable
	0x05, // 0x23: Filter Period
	0x04, // 0x24: Filter Threshold
	0x00, // 0x25: Control 3
	0x7f, // 0x26: GINT Interrupt Edge Enable
	0x00, // 0x27: GPIO Input Bouncing Canceling Period
	0xFF, // 0x28: Register Check, tadinya 0x00
	175, // 0x29: PA0_R_SEL~PA3_ R_SEL		//			10 pF
	0xFF, // 0x2A: PA4_R_SEL~PA7_ R_SEL		//			10 pF
	0x03, // 0x2B: REF_R_SEL
	0x80, // 0x2C: Beta Disable
	0x00, // 0x2D: GPIO0~GPIO1 Dimming Unit Period
	0x00, // 0x2E: GPIO2~GPIO3 Dimming Unit Period
	0x00, // 0x2F: PA0~PA1 Dimming Unit Period
	0x00, // 0x30: PA2~PA3 Dimming Unit Period
	0x00, // 0x31: GPIO0 Dimming Control
	0x00, // 0x32: GPIO1 Dimming Control
	0x00, // 0x33: GPIO2 Dimming Control
	0x00, // 0x34: GPIO3 Dimming Control
	0x00, // 0x35: PA0 Dimming Control
	0x00, // 0x36: PA1 Dimming Control
	0x00, // 0x37: PA2 Dimming Control
	0x00, // 0x38: PA3 Dimming Control
	0x00, // 0x39: GPIO0~GPIO3 Dimming Mode
	0x00, // 0x3A: PA0~PA3 Dimming Mode
	0x00, // 0x3B: Dimming Start
	0x00  // 0x3C: Dimming Enable
};
#endif


int i2c_set_register(char addr, char reg, char val);

int test_device(char addr)
{
	char c;
	c = addr << 1;
	i2c_start();
	if ( i2c_write(c) )
	{
		i2c_stop();
		return -1;
	}
	i2c_stop();
	return 0;	
}

int setup_fma(void)
{
	unsigned char i;
	int td=0;
	vTaskDelay(150);
	
	td=test_device( ADDR_FMA1 );
	#ifdef DEBUG_TSC
	printf("test device: %d\r\n", td);
	#endif
	if (td) {
		return -1;
	}
	
	struct t_env *p_env2;
	p_env2 = (char *) ALMT_ENV;
	//printf("k1: %d, k2: %d\r\n", p_env2->k1, p_env2->k2);
	
	//else
	{		
		// reset register check
		// supaya INT tidak terus generate clock
		i2c_set_register( ADDR_FMA1, TSC_REGISTER_CHECK, 0xFF );		// 0x28
		#ifdef DEBUG_TSC
		printf("Register check ... done !!\r\n");
		#endif
		vTaskDelay(5);
		
		for (i=0; i<61; i++) 
		{
			if (i==41) {
				if (i2c_set_register( ADDR_FMA1, i, p_env2->k1))
					return -2;
			} else if (i==42) {
				if (i2c_set_register( ADDR_FMA1, i, p_env2->k2))
					return -2;
			} else if (i2c_set_register( ADDR_FMA1, i, tsc_init_data[i]))		/*set TSC registers*/
			{
				return -2;
			}
		}
		#ifdef DEBUG_TSC
		printf("Init Register TSC ... done !!\r\n");
		#endif
		vTaskDelay(10);
		if( i2c_set_register( ADDR_FMA1, TSC_WARM_RESET, 0x00))       // isue warm reset, do not wait for ACK
		{
			return -3;
		}
		#ifdef DEBUG_TSC
		printf("Warm Reset ... done !!\r\n");
		#endif
		vTaskDelay(100);	// 200ms
		
		#if 1
		if( i2c_set_register( ADDR_FMA1, TSC_PA_EINT_ENABLE, 0xFF))
		{
			return -4;
		}
		#ifdef DEBUG_TSC
			printf(".... else enable EINT !!\r\n");
		#endif
		#endif
		
		return 0;
	}
}

//eint i2c_read_register(char addr, char reg, char *val);
unsigned char baca_tsc() {
	unsigned char qq;
	i2c_read_register(ADDR_FMA1, TSC_TOUCH_BYTE, &qq);
	//printf("0x%02X: TOUCH_BYTE: %d\r\n", TSC_TOUCH_BYTE, qq);
	return qq;
}

void baca_register_tsc() {
	unsigned char qq;
	int ww, zz;
	
	//printf("\t%s\r\n", __FUNCTION__);
	i2c_read_register(ADDR_FMA1, TSC_TOUCH_BYTE, &qq);
	printf("0x%02X: TOUCH_BYTE: %d\r\n", TSC_TOUCH_BYTE, qq);
	
	for (ww=0; ww<8; ww++) 
	{
		i2c_read_register(ADDR_FMA1, (TSC_ALPHA_00+ww), &qq);		// TSC_ALPHA_00
		printf("0x%02X: ALPHA%d: %d   ", (TSC_ALPHA_00+ww), ww, qq);

		i2c_read_register(ADDR_FMA1, (TSC_STR_THRES_00+ww), &qq);
		printf("0x%02X: STR_THRES%d: %d   ", (TSC_STR_THRES_00+ww),  ww, qq);

		//*	
		i2c_read_register(ADDR_FMA1, (TSC_STRENGTH_00+ww), &qq); // TSC_STRENGTH_00   0x50
		printf("0x%02X: STRENGTH%d: %d   ", (TSC_STRENGTH_00+ww), ww, qq);

		i2c_read_register(ADDR_FMA1, (TSC_IMP_00+ww), &qq);	// SC_IMP_00   0x58
		printf("0x%02X: IMP%d: %d   ", (TSC_IMP_00+ww), ww, qq);
		
		i2c_read_register(ADDR_FMA1, (TSC_REF_IMP_00+ww), &qq);		// 0x60
		printf("0x%02X: REF_IMP%d: %d\r\n", (TSC_REF_IMP_00+ww), ww, qq);
	}
	printf("\r\n");
	//*/
}

static int loop_read;

char read_key(void)
{
	#if 0
	char d = 'A';
	
	if ( i2c_read_register( ADDR_FMA1, 0, &d))
	{
		return 0;
	}
	return d;
	#endif
	
	#if 1
	unsigned char d = 'A';
	int i;
	char tek[32];
	
	for (i=0; i<8; i++)
	{
		i2c_read_register( ADDR_FMA1, (0x50 + i), &d);
		//sprintf(tek, "%d, %d",i, d);
		sprintf(tek, "%d, 0x%X, %d",i, (i+0x50), d);

		#if (USE_EMU_TFT == 0)
		teks_layar( 20, (i*8) + 10, tek);
		#else
		teks_layar( 20, (i*16) + 10, tek);
		#endif
	}

	for (i=0; i<8; i++)
	{
		i2c_read_register( ADDR_FMA1, (0x58 + i), &d);
		sprintf(tek, "%d, 0x%X, %d",i, (0x58 + i), d);

		#if (USE_EMU_TFT == 0)
		teks_layar( 20, (i*8) + 80, tek);
		#else
		teks_layar( 20, (i*16) + 160, tek);
		#endif	
	}
	
	for (i=0; i<8; i++)
	{
		i2c_read_register( ADDR_FMA1, (0x60 + i), &d);
		sprintf(tek, "%d, 0x%X, %d",i, (0x60 + i), d);

		#if (USE_EMU_TFT == 0)
		teks_layar( 120, (i*8) + 80, tek);
		#else
		teks_layar( 240, (i*16) + 160, tek);
		#endif	
	}
	
	//if ( i2c_read_register( ADDR_FMA1, 0x68, &d))
	i2c_read_register( ADDR_FMA1, 0x68, &d);
	{
		loop_read++;
		
		//sprintf(tek, "key %d, tot = %d", d, tot_touch);
		sprintf(tek, "key %d, Loop %d", d, loop_read);

		#if (USE_EMU_TFT == 0)
		teks_layar( 20, 180, tek);
		#else
		teks_layar( 20, 360, tek);
		#endif
		//tot_touch = 0;
		return 0;
	}
	#endif
}

#endif
