/*
	19 sept 2008
	dipindah dari adc_monita kesini
	
	15 April 2009
	porting ke freeRTOS & LPC2368
	
	*/
	
#include "FreeRTOS.h"

//extern unsigned int kanal_aktif;

#define false	0
#define true	1
#ifdef PAKAI_ADC

#if defined(BOARD_KOMON_420_SABANG_2_3) || defined(BOARD_KOMON_420_SABANG)
	#include "../../hardware/hardware.h"
#endif

char kalibrasi_adc1(int fdy)
{
	unsigned char er;
	char ada_adc_1;
	int t;
	int loop_ulang;
	int loop;
	//int loop_full_calib;
	
	unsigned char temp_char;
	unsigned char cek;
	//unsigned int mask;
	
	printf("Start calibrating ADC");

	stop_adc();				// set_mode(1);
	set_calibrated(0);
	vTaskDelay(200);
	reset_adc();
	vTaskDelay(200);
	
	//cek ID
	cek = cek_adc_id();	
	printf(" id = 0x%X \n", cek);
	
	vTaskDelay(10);
	set_iocon(0x00);
	er = 0;
	ada_adc_1 = false;
	
	//return 0;
	//*
	for (t=0; t<10; t++)
	{
		temp_char = (unsigned char) ((er << 4) + 15);		// 0x0F
	   	if (er == 8)
			temp_char = (unsigned char) ((14 << 4) + 15);
	   	if (er == 9)
			temp_char = (unsigned char) ((15 << 4) + 15);
	   
	  	loop_ulang = 0;
	   	printf("   Kanal %2d : ", (1+t));
	   	
	   	kalib_ulang:
	   	loop_ulang++;
		set_adccon(temp_char);	// 0x0F: UNIPOLAR | 2.56 V
		//printf("temp_char: %02x\r\n", temp_char);
		// kalibrasi zero scale + CHCON //
		set_mode(4 + 16);		// 0x14		: CHOP enabled, NEG to 0V, REFIN 0-2.56V, CHCON:10 ch,
		vTaskDelay(1);
		cek = cek_adccon();
		//printf("   adccon: %02x\r\n", cek);
		if (cek != temp_char) printf(" !%d ", cek); 
		//serial_puts("tidak cocok");
		
		vTaskDelay(1);
		loop = 0;
		
		
		while ((cek_mode() & 0x3) != 0x1)		// cari nilai 0x01
		{
			vTaskDelay(10);
			loop++;
			if (loop > 50) 
			{
				if (loop_ulang < 6)
				{
					printf(" recalib ");
				   	goto kalib_ulang;
				}

				printf(".... GAGAL !!!\r\n");
				ada_adc_1 = false;
				break;
			}
		}
		
		if (loop < 50) 
		{	
		   	
			printf(" try = %d (OK)\n", loop);
			ada_adc_1 = true;
		}

		//if (ada_adc_1 == false) 		{
		//	printf("tidak terdapat ADC");
		//	return 1;
		//}

		er++;
	}
	//*/
	if (ada_adc_1 == true)
	{
		set_filter((unsigned char) rate_7708);
	   	
	   	printf(" ADC iocon %d", cek_iocon());
		printf("; adccon %d", cek_adccon());
		printf("; mode %d\r\n", cek_mode());
	
		set_iocon(0x00);
		
		set_calibrated(1);
		return 1;
	}
	else
	{
		set_calibrated(0);
		printf("__ADC tidak dikenali__\r\n");
		return 0;
	}
}
#endif
