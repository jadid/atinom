
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#ifndef __AMBILCEPAT__
#define __AMBILCEPAT__

#include "monita/monita_uip.h"
xTaskHandle hdl_ambilcepat;

#define TES_MEM_RTC

#ifdef PAKAI_MODEM_SERIAL
	
#endif

#ifdef BOARD_KOMON_KONTER_3_0
	#include "../modul/GPIO/gpio.h"
#endif

#ifdef BOARD_KOMON_KONTER_3_1
	#include "../modul/GPIO/gpio.h"
#endif


#ifdef DATA_RANDOM
	extern float data_f[];
#endif

#ifdef BOARD_KOMON_KONTER
	#include "../modul/tinysh/enviro.h"
	extern unsigned char status_konter[KANALNYA];
#endif


#ifdef PAKAI_PUSHBUTTON
	extern unsigned int debound[KANALNYA];
#endif

#ifdef PAKAI_PILIHAN_FLOW
	extern struct t2_konter konter;
#endif

portTASK_FUNCTION(ambilcepat, pvParameters )	{
  	vTaskDelay(500);
  	baca_env(0);	
  	
  	vTaskDelay(50);
  	unsigned int loopambil=0, lm=0, lb=0;
  	char ser2[50];
  	
	
	#ifdef PAKAI_SHELL
		printf(" Monita : Ambil cepat init !!\r\n");
  	#endif
  	
  	#if defined(PAKAI_I2C)
		#if defined (PAKAI_TSC)
			unsigned char st_tsc=0;
			char a;
			int i;
			int c=0;
			
			printf("mau setup fma ...\r\n");
			if (setup_fma())	printf(" NO ack !\r\n");
			else	{			
				printf("Init TSC OK ack !!!\r\n");
				st_tsc = 1;
			}
		#endif
  	#endif
  	
  	#ifdef PAKAI_CRON
		printf("Init CRON OK !!!\r\n");
  	#endif
  	
  	#ifdef PAKAI_GPS
		int hasil_gpsnya;
		awal_gps();
  	#endif
  	
  	#ifdef PAKAI_ADC
		extern unsigned char status_adcnya; 
  	#endif

  	#ifdef PAKAI_GPIO_DIMMER
		//init_remang();
  	#endif
  	
  	#ifdef PAKAI_PUSHBUTTON
		for (loopambil=0; loopambil<KANALNYA; loopambil++) {
			debound[0] = 0;
		}
	#endif
	
  	#ifdef PAKAI_PM
		int almtSumber=0;
		int sPM=0;
		
		#ifdef AMBIL_PM
			printf("    Init ambil PM ..-ambilcepat-..!!!\r\n");
			printf("      Kompatibel : \r\n");
			#ifdef TIPE_PM810
				printf("         - %s\r\n", TIPE_PM810);
			#endif
			#ifdef TIPE_PM710
				printf("         - %s\r\n", TIPE_PM710);
			#endif
			#ifdef TIPE_MICOM_M300 			
				printf("         - %s\r\n", TIPE_MICOM_M300);
			#endif
			#ifdef TIPE_ION8600
				printf("         - %s\r\n", TIPE_ION8600);
			#endif
			#ifdef TIPE_MICOM_P127
				printf("         - %s\r\n", TIPE_MICOM_P127);
			#endif
			#ifdef TIPE_A2000
				printf("         - %s\r\n", TIPE_A2000);
			#endif
			#ifdef TIPE_TFX_ULTRA
				printf("         - %s\r\n", TIPE_TFX_ULTRA);
			#endif
			vTaskDelay(2500);
		#endif
  	#endif
  	
  	#ifdef PAKAI_ADC_ORI
		printf("  ADC ori dipakai !!\r\n");
		init_adc_ori();
  	#endif
  	
	#ifdef BOARD_KOMON_KONTER
		#ifndef BLINK
		struct t_env *penv;
		penv = (char *) ALMT_ENV;
		
		baca_rtc_mem();
		
		for (loopambil=0; loopambil<KANALNYA; loopambil++)	{
			status_konter[loopambil] = penv->kalib[loopambil].status;
			setup_konter_onoff(loopambil, status_konter[loopambil]);
			//printf("status %d --> %d : %d\r\n", \
			//	loopambil+1, status_konter[loopambil], setup_konter_onoff(loopambil, status_konter[loopambil]));
			
		}
		loopambil=0;
		#endif
  	#endif

	#ifdef UNTUK_UNSRI
		//printf("*******************baca_mem_rtc*********************\r\n");
		baca_mem_rtc();
	#endif

  	#ifdef PAKAI_RTC
  	#if 1
		printf(" Init RTC ....%d\r\n", xTaskGetTickCount());
		flagRTCc = 5;
		init_rtc();
		vTaskDelay(10);
		start_rtc();
		
		
	#endif
  	#endif
  	
  	vTaskDelay(50);
  	for(;;) {		
		#ifdef DATA_RANDOM
			data_f[loopambil%10] = (float) ((rand() % 100));
			//printf("%d: data: %.1f\r\n", loopambil%10, data_f[loopambil%10]);
		#endif
		
		/*
		#ifdef PAKAI_SERIAL_2
			if (loopambil%1000==0) {			// 1 detik ambil data
				sprintf(ser2, "tes serial 2 : %d\r\n", lm);
				ser2_putstring(ser2);
			}
			
			
		#endif
		//*/
		
		loopambil++;
		#ifdef PAKAI_GPS
			if (serPollGPS())	
			{
				hasil_gpsnya = proses_gps();
				#if 0
					info_gps(hasilGPS);
				#endif
			}
		#endif

		#ifdef PAKAI_PM
			#ifdef AMBIL_PM			// AMBIL_PM
				sedot_pm();
			#endif
		#endif
		
		#ifdef PAKAI_ADC
			if (status_adcnya) {
				proses_data_adc();
				//printf("proses adc ... %d - %d !!\r\n", status_adcnya, loopambil);
				
				#ifdef BOARD_KOMON_420_SAJA
					hitung_datanya();
				#endif
				simpan_ke_data_f();
			}
		#endif
		
		//#ifdef UNTUK_UNSRI
		if (loopambil%500==0) {			// 1 detik ambil data
			//FIO0PIN ^= BIT(27);
			//hitung_energi();
			//printf("T2MR0: %d, T2TC: %d\r\n", T2MR0, T2TC);
		}
		//#endif
		
		/*
		#ifdef PAKAI_TIMER_2
			if (flagT2)	{
				//printf("lb: %d : %d\r\n", lb, iTim2);
				
				#ifdef PAKAI_CRON
				//if (loopambil%120==0)	//	60 detik
					baca_cron();		// tiap detik
				#endif
				
				
				flagT2 = 0;
				lb++;
			}
		#endif
		//*/
		
		#ifdef PAKAI_ADC_ORI
			//if (loopambil%2)	{
				proses_data_adc_ori(loopambil%2);		// 2 = JML_ADC
			//}
		#endif
		
		#ifdef BOARD_KOMON_KONTER
			#ifdef HITUNG_RPM
				if (loopambil%150==0) {				// 1.5 detik ambil data
					cek_input_onoff();				// ada di tinysh/rpm.c
					hitung_rpm();
				}
				data_frek_rpm();
				
				if (loopambil%1000==0) {		// 5x20 = 100
					//printf("____RPM 7: %f-%f\r\n", data_f[12],data_f[13]);
				}
				
			#endif
		#endif

		#ifdef BOARD_KOMON_KONTER_3_0
			#ifdef HITUNG_RPM
				if (loopambil%1000==0) {		// 5x20 = 100
					hitung_rpm();
					data_frek_rpm();
				}
			#endif
		#endif
	
		#ifdef PAKAI_MODEM_SERIAL
		
		#endif
	
		#ifdef PAKAI_I2C
			#if 0
			if (st_tsc) {
				if (loopambil%500==0) {	// 2*250: 500ms = 0.5 detik
					printf("__detik: %3d\r\n", c++);
					//baca_register_tsc();

					#if 1
					if (int_berganti() == 0) 
					{
						printf("disentuh\r\n");
					}
					else
						printf("HIGH\r\n");
						//vSerialPutString(0, "HIGH\r\n");
					#endif
				}
			}
			#endif
			#if 0
			if (st_tsc) {
				if (xSerialGetChar(1, &c, 0xFFFF ) == pdTRUE)
				{
					vSerialPutString(0, "Tombol ditekan = ");
					xSerialPutChar(	0, (char ) c);
					vSerialPutString(0, " \r\n");
					
					if ( (char) c == 's')
					{
						printf(" Set\r\n");
						/*
						if (i2c_set_register(0x68, 1, 8))
						{
							out(" NO ACK !!\r\n");
						}
						else
							out(" OK\r\n");
						*/
						
						if (setup_fma())
						{
							printf(" NO ack !\r\n");
						}
						else
							printf(" OK ack !\r\n");
							
						
					}
					else
					{
						printf("====\r\n");
						for (i=0;i<16; i++)
						{
							if (i == 8) printf("****\r\n");
							
							if (i2c_read_register(0x68, (0x50 + i), &a))
							{
								printf(" Read failed !\r\n");
							}
							else
							{
								printf(" Read OK =");
								a = a + '0';
								printf("%c", (char) a);
								printf(" \r\n");
							}
						}
							
							printf("KEY \r\n");
							if (i2c_read_register(0x68, 0x68, &a))
							{
								printf(" Read failed !\r\n");
							}
							else
							{
								printf(" Read OK =");
								a = a + '0';
								printf("%c", (char) a);
								printf(" \r\n");
							}
							//a = read_key();
							//a = a + '0';
							//xSerialPutChar(	0, (char ) a);
					}
				}
			}
			#endif

		#endif
		
		//printf("isi memRTC0: %d, loopambil: %d\r\n", MEM_RTC0, loopambil);
		if (loopambil>10000) {
			#ifdef TES_MEM_RTC
			//MEM_RTC0++;
			//MEM_RTC1++;
			//printf("   isi mem0: %d, mem1: %d, mem2: %d, mem3: %d    ", \
			//	*(&MEM_RTC0), *(&MEM_RTC0+1), *(&MEM_RTC0+2), *(&MEM_RTC0+3));
			//printf("   isi h1: %d : %d, --- h2: %d : %d\r\n", \
			//	konter.t_konter[0].onoff, konter.t_konter[0].hit, konter.t_konter[1].hit, konter.t_konter[1].hit2);
			#endif
			loopambil=0;
		}
		
		#ifdef PAKAI_GPIO_DIMMER
			//remangkan();
			/*
			loop_pwm++;
			
			#ifdef DEBUG_PWM_GPIO
			if (loop_pwm>500) {
				loop_pwm=0;
				blink_pwm=1-blink_pwm;
				if (blink_pwm) {
					FIO1SET = BIT(30);
				} else {
					FIO1CLR = BIT(30);
				}
			}
			#endif
			//*/
		#endif
		
		#if 0
		vTaskDelay(1000);
		#endif
		
		#if 1
		vTaskDelay(1);
		#endif
		
		lm++;
	}

	#ifdef PAKAI_GPS
		deinit_gps();
	#endif
}



#ifdef PAKAI_GPS
//	nAmbilCepat += 5;
#endif

void init_ambilcepat(void)	{
	int nAmbilCepat=10;
	#ifdef BOARD_TAMPILAN
		xTaskCreate( ambilcepat, "ambilcepat_task", (configMINIMAL_STACK_SIZE * nAmbilCepat), NULL, tskIDLE_PRIORITY+8, ( xTaskHandle * ) &hdl_ambilcepat);
	#else
		xTaskCreate( ambilcepat, "ambilcepat_task", (configMINIMAL_STACK_SIZE * nAmbilCepat), NULL, tskIDLE_PRIORITY+3, ( xTaskHandle * ) &hdl_ambilcepat);
	#endif
}

#endif

