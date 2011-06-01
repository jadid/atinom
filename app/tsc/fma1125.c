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

#define ADDR_FMA1	0x68


#define TSC_FILTER_EN    0x01   // Mode selection register values
#define TSC_APIS_1       0x02   // Mode 1 / only strongest sensor output is signaled
#define TSC_APIS_2       0x04   // Mode 2 / reports all outputs that exceeds pre-defined thresholds
#define TSC_APIS_3       0x08   // Mode 3 / reports two strongest outputs - for multitouch applications

const unsigned char tsc_init_data[] = {   // This only contains the writable TSC registers.
	8, // 0x00: PA0 Alpha		// 30
	8, // 0x01: PA1 Alpha
	8, // 0x02: PA2 Alpha
	8, // 0x03: PA3 Alpha
	8, // 0x04: PA4 Alpha
	8, // 0x05: PA5 Alpha
	8, // 0x06: PA6 Alpha
	8, // 0x07: PA7 Alpha
	50, // 0x08: Reference Delay	90
	4,  // 0x09: Beta
	39, // 0x0A: AIC Wait Time		// 0x27
	1,  // 0x0B: PA0 Strength Threshold: 1/10 ss/d 1/20 Int time		// 30
	1,  // 0x0C: PA1 Strength Threshold
	1,  // 0x0D: PA2 Strength Threshold
	1,  // 0x0E: PA3 Strength Threshold
	1,  // 0x0F: PA4 Strength Threshold
	1,  // 0x10: PA5 Strength Threshold
	1,  // 0x11: PA6 Strength Threshold
	1,  // 0x12: PA7 Strength Threshold
	(TSC_APIS_2 + TSC_FILTER_EN),  // 0x13: Feature		TSC_APIS_1
	15,  // 0x14: Integration Time			// 128
	0x1f, // 0x15: IDLE State Enter Time
	0x20, // 0x16: Control 1
	0x00, // 0x17: Control 2
	0x00, // 0x18: PA Data Out
	0x00, // 0x19: GPIO Data Out
	0x00, // 0x1A: PA Direction (0 = OUTPUT, 1 = INPUT)
	0x00, // 0x1B: GPIO Direction (0 = OUTPUT, 1 = INPUT)
	0x00, // 0x1C: PA Configuration (0 = TOUCH INPUT, 1 = GPIO) : as input PA
	0x00, // 0x1D: GPIO Configuration (0 = Direct Touch Output, 1 = GPIO)
	0x20, // 0x1E: Calibration Interval
	0xff, // 0x1F: GINT Interrupt Mask (1 = MASKED)
	0xff, // 0x20: GINT Interrupt Pending Clear
	0x00, // 0x21: PA EINT Enable : 0x00: disable interrupt
	0x00, // 0x22: GPIO EINT Enable
	0x05, // 0x23: Filter Period
	0x04, // 0x24: Filter Threshold
	0x00, // 0x25: Control 3
	0x7e, // 0x26: GINT Interrupt Edge Enable
	0x00, // 0x27: GPIO Input Bouncing Canceling Period
	0xFF, // 0x28: Register Check, tadinya 0x00
	0x00, // 0x29: PA0_R_SEL~PA3_ R_SEL		//			10 pF
	0x00, // 0x2A: PA4_R_SEL~PA7_ R_SEL		//			10 pF
	0x00, // 0x2B: REF_R_SEL
	0x00, // 0x2C: Beta Disable
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
	vTaskDelay(350);
	
	td=test_device( ADDR_FMA1 );
	printf("test device: %d\r\n", td);
	if (td) {
		return -1;
	}
	else
	{		
		// reset register check
		// supaya INT tidak terus generate clock
		i2c_set_register( ADDR_FMA1, TSC_REGISTER_CHECK, 0xFF );		// 0x28
		printf("Register check ... done !!\r\n");
		vTaskDelay(10);
		
		for (i=0; i<61; i++) 
		{
			if (i2c_set_register( ADDR_FMA1, i, tsc_init_data[i]))		/*set TSC registers*/
			{
				return -2;
			}
		}
		printf("Init Register TSC ... done !!\r\n");
		vTaskDelay(50);
		if( i2c_set_register( ADDR_FMA1, TSC_WARM_RESET, 0x00))       // isue warm reset, do not wait for ACK
		{
			return -3;
		}
		printf("Warm Reset ... done !!\r\n");
		vTaskDelay(200);	// 200ms
		
		#if 1
		if( i2c_set_register( ADDR_FMA1, TSC_PA_EINT_ENABLE, 0xFF))
		{
			return -4;
		}
			printf(".... else enable EINT !!\r\n");
		#endif
		
		return 0;
	}
}

//eint i2c_read_register(char addr, char reg, char *val);

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

char read_key(void)
{ 
	char d = 'A';
	
	if ( i2c_read_register( ADDR_FMA1, 0, &d))
	{
		return 0;
	}
	return d;
}
