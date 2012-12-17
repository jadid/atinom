	
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

int noawal;
//extern char ayokirim;
int susun_kirim(int jmlData, char *iList, char *dList)	{
	struct t_env *penq;
	penq = (char *) ALMT_ENV;
	char angka[5];

	if (jmlData>0) {
		strcpy(datakeserver, "");
		//sprintf(ipdest, "%d.%d.%d.%d", envx->GW0, envx->GW1, envx->GW2, envx->GW3);
		//sprintf(ipdest, "%d.%d.%d.%d", envx->wIP0, envx->wIP1, envx->wIP2, envx->wIP3);
		//portENTER_CRITICAL();
		//sprintf(datakeserver, "%s?i=%s&p=diesel&j=%d&%s&%s", envx->berkas, envx->SN, jmlData, il, dl);
		strcat(datakeserver, "?i=");
		strcat(datakeserver, penq->SN);
		strcat(datakeserver, "&p=diesel&j=");
		sprintf(angka, "%d", jmlData);
		strcat(datakeserver, angka);
		#ifdef DEBUG_WEBCLIENT
			strcat(datakeserver, "&cc=");
			wc++;
			sprintf(countwc, "%d", wc);
			strcat(datakeserver, countwc);
		#endif
		strcat(datakeserver, iList);
		strcat(datakeserver, dList);
	}
}


portTASK_FUNCTION(kirimcepat, pvParameters )	{
  	vTaskDelay(1060);

  	unsigned int qw=0, lb=0;
  	int dlama=0;
  	char ser2[100];
  	char sedot[100];
  	
  	struct t_env *penv;
	penv = (char *) ALMT_ENV;
	
	struct t_sumber *p_sbr;
	p_sbr = (char *) ALMT_SUMBER;
	
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
  	
  	int lk = 0, cKirim=0, dKirim;
  	char iList[256], dList[256];
  	
  	int jmlData=0,nos=0, ngitung=0, selang=0, sumber=0;
  	char flag_nos;
  	int aa = 0, na = 0, mm, nn=0;
  	char cM[2];
  	
  	ayokirim = 0;	
  	if (penv->intKirim==1)			dKirim = 990;
	else							dKirim = (penv->intKirim-1)*1000+990;
	printf("Periode Kirim : tiapKirim = %d\r\n", dKirim);
  	
  	noawal = 0;
  	nos = 0;
  	
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
			//*
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
				//printf("alarm	 : %d\r\n", lb);
				flagRTCc=99;
			}
			//*/
		#endif
		
		#if 1
		vTaskDelay(1);
		#endif
		
		if (flag_nos==AKTIF)	{		// persumber, data dikirim > 12, flag_nos aktif
			ngitung++;
			if ( (cKirim>0) && (ngitung%205) )	{
				ayokirim |= 1;
				//printf("ngitung: %d, tambahan %d !!!!\r\n", ngitung, ayokirim);
			}
		}
		
		//printf("cKirim %d, dKirim: %d, ayoKirim: %d\r\n", cKirim, dKirim, ayokirim);
		if ( (cKirim==dKirim) || (ayokirim & 1) ) 	{
			ayokirim &= (~1);
			
			if ( (nos==wRELAY) || (p_sbr[nos].status==AKTIF))
				jmlData=kirimModul(0, nos, noawal, iList, dList);
			//printf("jmlData : %d, susun_kirim : %s\r\n", jmlData, iList);
			if (jmlData>0)	{
				susun_kirim(jmlData, iList, dList);
				//printf("susun_kirim : %s\r\n", iList);

				#ifdef WEBCLIENT_DATA
					ayokirim |= 2;
					//printf("ayokirim : %d\r\n", ayokirim);
				#endif

				#ifdef KIRIM_KE_SER_2
					ayokirim |= 4;
				#endif
			} 
			//printf("----------------------sumber: %d, jml: %d, noawal: %d, %s\r\n", nos, jmlData, noawal, iList);
			
			flag_nos = AKTIF;
			if (jmlData<12)	{
				nos++;
				noawal=0;
			}
			
			if (nos>JML_SUMBER) {
				nos=0;
				flag_nos = MATI;
			#ifdef PAKAI_RELAY
			} else if (nos==JML_SUMBER)	{
				flag_nos = AKTIF;
				nos = wRELAY;
				noawal = PER_SUMBER * JML_SUMBER;
				//printf("===========>  ini bagian relay : %d !!!\r\n", nos);
			#endif
			}
			
			selang++;
			if (selang>10)	{
				selang = 0;
				if (penv->intKirim==1)	dKirim = 990;
				else					dKirim = (penv->intKirim-1)*1000+990;
			}

			ngitung=0;
			cKirim = 0;
		}

		#if 0
		vTaskDelay(1000);
		//printf("data0: %.1f,2: %.1f,3: %.1f\r\n", data_f[0], data_f[2], data_f[3]);
		#endif

		cKirim++;
	}
}

int kirimModul(int burst, int sumber, int awal, char *il, char *dl) {
	char id[16], dt[16];
	int i=0,z=0, qq = PER_SUMBER*sumber;
	int jmlAktif=0;
		
	struct t_setting *konfig;
	konfig = (char *) ALMT_KONFIG;

	strcpy(il,"&il="); 	strcpy(dl,"&dl=");
	#ifdef PAKAI_RELAY
	if (sumber==wRELAY)		{
		strcpy(il,"&rl="); 	strcpy(dl,"&kl=");
		sumber = JML_SUMBER;
	}
	#endif
	
	if (burst==AKTIF) {
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
		//printf("-------> sumber: %d\r\n", sumber);
		if (sumber<JML_SUMBER)		{
			for (i=noawal; i<PER_SUMBER; i++) {
				noawal=i+1;
				if (konfig[qq+i].status==AKTIF) {
					jmlAktif++;
					//if (i==0) {
					if (jmlAktif==1) {
						sprintf(id, "%d", konfig[qq+i].id);
						sprintf(dt, "%.2f", data_f[qq+i]);
					} else {
						sprintf(id, "~%d", konfig[qq+i].id);
						sprintf(dt, "~%.2f", data_f[qq+i]);
					}
					strcat(il,id);
					strcat(dl,dt);
				}
				if (jmlAktif==12)
					break; 
			}
		#ifdef PAKAI_RELAY
		} else {
			for (i=noawal; i<noawal+JML_RELAY; i++) {
				if (konfig[i].status==AKTIF) {
					jmlAktif++;
					//if (i==0) {
					if (jmlAktif==1) {
						sprintf(id, "%d", konfig[i].id);
						sprintf(dt, "%.2f", data_f[i]);
					} else {
						sprintf(id, "~%d", konfig[i].id);
						sprintf(dt, "~%.2f", data_f[i]);
					}
					strcat(il,id);
					strcat(dl,dt);
				} 
			}
			//printf("il relay: %s, dl: %s\r\n", il, dl);
		#endif
		}
	}
	//printf("no awal: %d\r\n", noawal);
	return jmlAktif;
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
