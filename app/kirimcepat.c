	
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#ifndef __KIRIMCEPAT__
#define __KIRIMCEPAT__

#include "monita/monita_uip.h"
#include "tinysh/enviro.h"


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

int noawal;
#if 1
int kirimModulX(int burst, int sumber, int awal, char *il, char *dl) {
	char id[16], dt[16];
	int i=0,z=0;
	int jmlAktif=0;
		
	struct t_setting *konfig;
	konfig = (char *) ALMT_KONFIG;
		
	strcpy(il,"&il=");
	strcpy(dl,"&dl=");
	if (burst==1) {
		struct t_sumber *psbr;
		psbr = (char *) ALMT_SUMBER;
		
		for (z=0; z<JML_SUMBER; z++) {
			if (psbr[z].status==1) {
				for (i=0; i<PER_SUMBER; i++) {
					if (konfig[(PER_SUMBER*z)+i].status==1) {
						jmlAktif++;
						//if (i==0) {
						if (jmlAktif==1) {
							//strcat(id, konfig[(PER_SUMBER*z)+i].id);
							//strcat(dt, data_f[(PER_SUMBER*z)+i]);
							sprintf(id, "%d", konfig[(PER_SUMBER*z)+i].id);
							sprintf(dt, "%.2f", data_f[(PER_SUMBER*z)+i]);
							//printf("no: %d, id: %d, data: %d\r\n",(PER_SUMBER*z)+i,  konfig[(PER_SUMBER*z)+i].id, data_f[(PER_SUMBER*z)+i]);
						} else {
							//strcat(id, "~");
							//strcat(id, konfig[(PER_SUMBER*z)+i].id);
							//strcat(dt, "~");
							//strcat(dt, data_f[(PER_SUMBER*z)+i]);
							sprintf(id, "~%d", konfig[(PER_SUMBER*z)+i].id);
							sprintf(dt, "~%.2f", data_f[(PER_SUMBER*z)+i]);
							//printf("no: %d, id: %d, data: %d\r\n",(PER_SUMBER*z)+i,  konfig[(PER_SUMBER*z)+i].id, data_f[(PER_SUMBER*z)+i]);
						}
						
						strcat(il,id);
						strcat(dl,dt);
					}
				}
			}
		}
	} else {
		for (i=noawal; i<PER_SUMBER; i++) {
			noawal=i+1;
			if (konfig[PER_SUMBER*sumber+i].status) {
				jmlAktif++;
				//if (i==0) {
				if (jmlAktif==1) {
					sprintf(id, "%d", konfig[PER_SUMBER*sumber+i].id);
					sprintf(dt, "%.2f", data_f[PER_SUMBER*sumber+i]);
				} else {
					sprintf(id, "~%d", konfig[PER_SUMBER*sumber+i].id);
					sprintf(dt, "~%.2f", data_f[PER_SUMBER*sumber+i]);
				}
				strcat(il,id);
				strcat(dl,dt);
			} 
			if (jmlAktif==12)
				break; 
				
		}
	}
	//printf("no awal: %d\r\n", noawal);
	return jmlAktif;
}
#endif

portTASK_FUNCTION(kirimcepat, pvParameters )	{
  	vTaskDelay(1060);

  	unsigned int qw=0, lb=0;
  	int dlama=0;
  	char ser2[100];
  	char sedot[100];
  	
  	struct t_env *penv;
	penv = (char *) ALMT_ENV;
	
	#ifdef PAKAI_SHELL
		printf(" Monita : Kirim cepat init !!\r\n");
  	#endif
  	#ifdef PAKAI_RTC
  	extern unsigned char flagRTCc;
  	#endif
  	
  	#ifdef PAKAI_MODEM_GSM
		extern int	 nRespM;
		extern char sCmdM[];
		extern char sRespM[];
		extern char flagModem;
		flagModem = 0;
		status_modem = 0;
		init_mem_sms();
  	#endif
  	
  	int lk = 0;
  	int aa = 0, na = 0, mm, nn=0;
  	char cM[2];
  	
  	vTaskDelay(50);
  	vTaskDelay(5000);
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
			#ifdef PAKAI_MODEM_GSM
				if (flagModem)	{
					perintah_modem(flagModem);
				}
				#if 0
				if (lk>2000)	{
					lk = 0;
					
					strcpy(sCmdM, "AT\r\n");
					//printf("cmd : %s", sCmdM);
					
					kirimCmdModem(sCmdM, &sRespM);
					/*
					strcpy(sCmdM, "AT\r\n");
					printf("===> kirim : %s", sCmdM);
					ser2_putstring(sCmdM);
					nRespM = 2;
					nn=0;
					do {
						for (mm=0; mm<(nRespM+2); mm++)	{
							if (ser2_getchar(1, &cM[0], 100 ) == pdTRUE)		{
								//printf("%c", cM[0]);
								if (cM[0]==0x0A)	{
									//printf("-----0x0A---------------\r\n");
									break;
								} else if (cM[0]==0x0D)	{
									continue;
								} else {
									nn++;
									sRespM[mm] = cM[0];
								}
							}
						}
						sRespM[nn] = '\0';
						//printf("Respon : %s\r\n", sRespM);
					} while ( (strncmp(sRespM,sCmdM, nRespM)==0) || (strncmp(sRespM,"\r\n", 2)==0) );
					//*/
					printf(">>> Respon: %s \r\n", sRespM);
				}
				lk++;
				#endif
			#endif
		
		if (statKirimSer==1)	{
			qw++;
			#ifdef UNTUK_MONITA_KAPAL
			//printf("kirim ser 2 : %d\r\n", qw);
			sprintf(ser2, "%.1f,%.1f\r\n", data_f[0], data_f[2]);	
			vTaskDelay(1);
			//printf("%s\r\n", ser2);
			ser2_putstring(ser2);

			//cek_ngisi_minyak(dlama);
			//dlama = data_f[2];
			vTaskDelay(50);
			#endif

			//cek_PM(5);
			statKirimSer = 0;
		}
		#endif
		
		#ifdef PAKAI_KONTROL_RTC
			if (flagRTCc==1)	{
				lb++;
				#ifdef PAKAI_MODEM_GSM
				flagModem = 1;
				#endif
				//get_cal();
				//printf("counter : %d, tik: %ld\r\n", lb, xTaskGetTickCount());
				flagRTCc=99;
				#ifdef PAKAI_CRON
					baca_cron();
				#endif
			}
			if (flagRTCc==2)	{
				lb =lb+2;
				printf("alarm	 : %d\r\n", lb);
				flagRTCc=99;
			}
		#endif
		
		#if 1
		vTaskDelay(1);
		#endif
		
		#if 0
		vTaskDelay(1000);
		//printf("data0: %.1f,2: %.1f,3: %.1f\r\n", data_f[0], data_f[2], data_f[3]);
		#endif
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
