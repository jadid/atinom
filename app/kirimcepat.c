	
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#ifndef __KIRIMCEPAT__
#define __KIRIMCEPAT__

#include "../../app/monita/monita_uip.h"
#include "../../modul/tinysh/enviro.h"


xTaskHandle hdl_kirimcepat;

extern float data_f[];

#ifdef HITUNG_ENERGI
void hitung_energinya()	{
	float tempf;
	
	struct t_env *penq;
	penq = (char *) ALMT_ENV;
	//printf("hitung energi\r\n");
	//if ()	{
		tempf = (data_f[8]/3600) + data_f[9];		// dalam VAh
		data_f[9] = (float) (tempf * penq->kalib[9].m) + penq->kalib[9].C;
		//printf("8: %.4f, 9: %.4f, tmpf: %.4f = d[10]: %.4f\r\n", data_f[7], data_f[8], tempf, data_f[9]);
		simpan_ke_mem_rtc();
		
	//}
}
#endif

portTASK_FUNCTION(kirimcepat, pvParameters )	{
  	vTaskDelay(560);

  	unsigned int qw=0, lb=0;
  	int dlama=0;
  	char ser2[100];
  	
  	struct t_env *penv;
	penv = (char *) ALMT_ENV;
	
	#ifdef PAKAI_SHELL
		printf(" Monita : Kirim cepat init !!\r\n");
  	#endif
  	
  	
  	vTaskDelay(50);
  	for(;;) {
		#ifdef PAKAI_TIMER_2
			if (flagT2)	{
				lb++;
				//printf("cout >> %d/%d\r\n", lb, penv->intSer);
				//baca_cron();
				flagT2 = 0;
				
				#ifdef KIRIM_KE_SER_2
					#ifdef UNTUK_MONITA_KAPAL
					if (penv->statusSerClient)	{
						if (lb == penv->intSer)	{
							statKirimSer = 1;
							lb = 0;
						}					
					}
					#endif
				#endif
			}
		#endif
		
		#ifdef KIRIM_KE_SER_2
		if (statKirimSer==1)	{
			qw++;
			printf("kirim ser 2 : %d\r\n", qw);
			
			#ifdef UNTUK_MONITA_KAPAL
			sprintf(ser2, "%.1f,%.1f\r\n", data_f[0], data_f[2]);	
			vTaskDelay(1);
			printf("%s\r\n", ser2);
			ser2_putstring(ser2);

			//cek_ngisi_minyak(dlama);
			//dlama = data_f[2];
			vTaskDelay(50);
			#endif

			//cek_PM(5);
			statKirimSer = 0;
		}
		#endif
		
		vTaskDelay(1);
	}
}



int nKirimCepat=10;

void init_kirimcepat(void)	{
	#ifdef BOARD_TAMPILAN
		xTaskCreate( kirimcepat, "kirimcepat_task", (configMINIMAL_STACK_SIZE * nKirimCepat), NULL, tskIDLE_PRIORITY+8, ( xTaskHandle * ) &hdl_kirimcepat);
	#else
		xTaskCreate( kirimcepat, "kirimcepat_task", (configMINIMAL_STACK_SIZE * nKirimCepat), NULL, tskIDLE_PRIORITY+3, ( xTaskHandle * ) &hdl_kirimcepat);
	#endif
}

#endif
