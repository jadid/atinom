
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#ifndef __AMBILCEPAT__
#define __AMBILCEPAT__


xTaskHandle hdl_ambilcepat;

portTASK_FUNCTION(ambilcepat, pvParameters )	{
  	vTaskDelay(500);
	
  	printf(" Monita : Ambil cepat init !!\r\n");
  	#ifdef PAKAI_GPS
		int hasil_gpsnya;
		awal_gps();
  	#endif
  	
  	#ifdef PAKAI_ADC
		extern unsigned char status_adcnya; 
  	#endif
  	
  	vTaskDelay(100);
  	for(;;) {
		vTaskDelay(2);
		#ifdef PAKAI_GPS
			//if (serPollGPS())	
			{
				hasil_gpsnya = proses_gps();
				#ifdef DEBUG_GPSs
					info_gps(hasilGPS);
				#endif
			}
		#endif
		
		#ifdef PAKAI_ADC
			if (status_adcnya) {
				proses_data_adc();
				//printf("proses adc ... %d !!\r\n", status_adcnya);
				
				#ifdef BOARD_KOMON_420_SAJA
					hitung_datanya();
				#endif
				
				simpan_ke_data_f();
			}
		#endif
		
		#ifdef BOARD_KOMON_KONTER
			data_frek_rpm();
		#endif
	}
	
	#ifdef PAKAI_GPS
		deinit_gps();
	#endif
}

int nAmbilCepat=10;

#ifdef PAKAI_GPS
//	nAmbilCepat += 5;
#endif

void init_ambilcepat(void)	{
	xTaskCreate( ambilcepat, "ambilcepat_task", (configMINIMAL_STACK_SIZE * nAmbilCepat), NULL, tskIDLE_PRIORITY+3, ( xTaskHandle * ) &hdl_ambilcepat);
}

#endif

