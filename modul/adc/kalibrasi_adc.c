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

#if 0
char kalib_adc() {
	int itungadc=0;
	
	printf("Start calibrating ADC");
	set_calibrated(0);
	vTaskDelay(200);
	
	printf("ID adc: 0x%02x\r\n", cek_adc_id());
	
	reset_adc();
	vTaskDelay(100);
	printf("cek iocon: 0x%02x", cek_iocon());
	set_iocon(0x00);
	printf(" .... cek iocon: 0x%02x\r\n", cek_iocon());
	vTaskDelay(1);
	
	printf("cek filter: 0x%02x", cek_filter());
	set_filter(rate_7708);
	printf(" .... cek filter: 0x%02x\r\n", cek_filter());
	vTaskDelay(1);
	
	printf("cek adccon: 0x%02x", cek_adccon());
	set_adccon(0x00 | 0x08 | 0x07);
	printf(" .... cek adccon: 0x%02x\r\n", cek_adccon());
	vTaskDelay(1);
	
	printf("cek mode: 0x%02x", cek_mode());
	set_mode(0x14);
	printf(" .... cek mode: 0x%02x\r\n", cek_mode());
	
	do {
		printf(" %d.... cek mode: 0x%02x\r\n", itungadc++, cek_mode());
		vTaskDelay(100);
	} while(itungadc<100);
	return 0;
}
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
	unsigned char cek,nloop=0;
	//unsigned int mask;
	
	printf("Start calibrating ADC");

	stop_adc();				// set_mode(1);
	set_calibrated(0);
	vTaskDelay(200);
	//printf("\r\nstlh stop, mode: 0x%02x, status: 0x%02x\r\n", cek_mode(), cek_status());
	//printf("cek test1: 0x%02x, test2: 0x%02x\r\n", cek_test1adc(), cek_test2adc());
	reset_adc();			// bersihkan mem
	vTaskDelay(200);
	//printf("stlh reset, mode: 0x%02x, status: 0x%02x\r\n", cek_mode(), cek_status());
	//printf("cek tes1: 0x%02x, test2: 0x%02x\r\n", cek_test1adc(), cek_test2adc());
	//cek ID
	printf(" id = 0x%X \n", cek_adc_id());
	//printf(" mode: 0x%02x\r\n", cek_mode());
	vTaskDelay(10);
	set_iocon(0x00);
	//printf(" mode: 0x%02x, iocon: 0x%02x\r\n", cek_mode(), cek_iocon());
	er = 0;
	ada_adc_1 = false;
	
	//*
	t=0;
	for (t=0; t<10; t++)
	{
		temp_char = (unsigned char) ((er << 4) + 15);		// 0x0F
	   	if (er == 8)
			temp_char = (unsigned char) ((14 << 4) + 15);
	   	if (er == 9)
			temp_char = (unsigned char) ((15 << 4) + 15);
	   
	  	loop_ulang = 0;
	   	printf("   Kanal %2d : ", (1+t));
	   	
	   	//printf(" mode: 0x%02x\r\n", cek_mode());
	   	kalib_ulang:
	   	loop_ulang++;
		set_adccon(temp_char);	// 0x0F: UNIPOLAR | 2.56 V		// no ___1___.
		vTaskDelay(1);
		//printf(" mode: 0x%02x, adccon: 0x%02x, status: 0x%02x\r\n", cek_mode(), cek_adccon(), cek_status());
		//printf("temp_char: %02x\r\n", temp_char);
		// kalibrasi zero scale + CHCON //
		set_mode(4 + 16);		// no ___2___  : 0x14 : CHOP enabled, NEG to 0V, REFIN 0-2.56V, CHCON:10 ch,
		vTaskDelay(1);
		//cek = cek_adccon();		// 0x0F
		//printf("   mode: 0x%02x, adccon: 0x%02x, status: 0x%02x\r\n", cek_mode(), cek_adccon(), cek_status());
		//if (cek != temp_char) printf(" !%d ", cek); 
		//serial_puts("tidak cocok");
		
		//vTaskDelay(1);
		loop = 0;
		
		
		while ((cek=cek_mode() & 0x7) != 0x1)		// cari nilai 0x01, agar keluar dari loop
		{
			vTaskDelay(10);
			
			loop++;
			if (loop > 150) 
			{
				if (loop_ulang < 6)
				{
					printf(" recalib %d, cek: 0x%02x ", ++nloop, cek);
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
