	

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "hardware.h"

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

char sping[256];
int np;
char flag_ms;
//int alamat_slave;
//void parsing_ping(int ch);


#ifdef PAKAI_PM
void sedot_mod(int ch);
void pr_mod(unsigned char *jud, unsigned char *x, int jml);
//struct d_pmod resp_modx;
struct	st_mod_slave p_mod_sl;
#endif

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
  	char flag_nos, sumbers=0;
  	int aa = 0, na = 0, mm, nn=0;
  	char cM[2];
  	
  	ayokirim = 0;	
  	if (penv->intKirim==1)			dKirim = 990;
	else							dKirim = (penv->intKirim-1)*1000+990;
	printf("Periode Kirim : tiapKirim = %d\r\n", dKirim);
  	
  	#ifdef WEBCLIENT_DATA
		statwc = 0;
  	#endif
  	
  	noawal = 0;
  	nos = 0;
  	
  	vTaskDelay(50);
  	vTaskDelay(5000);
  	//char ch[2];
	int ch;
	np = 0;
	flag_ms = 0;
	//alamat_slave = 5;
  	
  	#if 0
  	for(;;) {
		//#if 1
		#ifdef PAKAI_MODBUS_SLAVE
		FIO0CLR = TXDE;
		FIO0CLR = RXDE;
		if (ser3_getchar(1, &ch, 40) == pdTRUE)	  {		// 100
			flag_ms = 1;
			//printf("%02x ", (char) ch);
			sedot_mod(ch);
		} else {
			//printf("&");
			if (flag_ms==1)	{
				//printf("\r\n--------------------------------- CMD datang\r\n");
				proses_mod_cmd();
			}
			flag_ms = 0;
			np=0;
		}
		#endif
  	}
  	#endif
  	
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
		
		
		//printf("%d", statwc);
		
			//printf("++++");
			if (flag_nos==AKTIF)	{		// persumber, data dikirim > 12, flag_nos aktif
				ngitung++;
				//if ( (cKirim>0) && (ngitung%205) )	{
				if ( (cKirim>0) && (ngitung%505) )	{
					ayokirim |= 1;
					//printf("ngitung: %d, tambahan %d !!!!\r\n", ngitung, ayokirim);
				}
			}

			//printf("cKirim %d, dKirim: %d, ayoKirim: %d\r\n", cKirim, dKirim, ayokirim);
			#if 0
			if (penv->statusWebClient == 1)	
			{
				if ( (cKirim>=dKirim) || (ayokirim & 1) ) 	{
					//printf("cKirim %d, dKirim: %d, ayoKirim: %d\r\n", cKirim, dKirim, ayokirim);
					ayokirim &= (~1);
					
					
					
					if ( (nos==wRELAY) || (p_sbr[nos].status==AKTIF))	{
						
						jmlData=kirimModul(0, nos, noawal, iList, dList);
						//printf("____nos: %d   jmlData : %d, susun_kirim : %s\r\n", nos, jmlData, iList);
					}
					#if 1
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
					#endif
					//printf("----------------------sumber: %d, jml: %d, noawal: %d, %s\r\n", nos, jmlData, noawal, iList);
					
					flag_nos = AKTIF;
					if (jmlData<12)	{			// cari data aktif di sumber berikutnya
						
						nos++;
						//printf("nos: %d -->", nos);
						//jmlData = 0;
						//noawal = nos*PER_SUMBER;
						noawal = 0;
						//printf("nowawal: %d\r\n", noawal);
					}
					
					if (nos>JML_SUMBER) {
						nos=0;
						flag_nos = MATI;
						noawal = 0;
						//printf("================ ++++++++++++ reset ke sumber 0\r\n");
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
			} else {
				nos=0;
			}
			#endif
		

		#if 0
		FIO0SET = TXDE;
		FIO0CLR = RXDE;
		vTaskDelay(1000);
		ser3_putstring("serial 3 cout\r\n");
		//printf("data0: %.1f,2: %.1f,3: %.1f\r\n", data_f[0], data_f[2], data_f[3]);
		#endif

		#if 0
		#ifdef PAKAI_MODBUS_SLAVE
			//printf("kirim ke serial\r\n");
			if (cKirim==500)	{
				//ser3_putstring("1234567890123");
				cKirim=0;
			}
			if (ser3_getchar(1, &ch[0], 5 ) == pdTRUE)	  {
				printf("%02x ", ch[0]);
				//tinysh_char_in((unsigned char)c);
			}
		#endif
		#endif

		#ifdef PAKAI_SENSOR_JARAK
		//#if 0
		//FIO0CLR = TXDE;		// on	---> bisa kirim
		FIO0CLR = RXDE;
		
		
		if (ser3_getchar(1, &ch, 30) == pdTRUE)	  {
			//printf("%c", ch);
			//parsing_ping(ch);
		}
		//FIO0SET  = BIT(5);	// TX kirim
		FIO0SET  = TXDE;	// TX kirim
		#endif


		cKirim++;
	}
}

#ifdef PAKAI_PM
void pr_mod(unsigned char *jud, unsigned char *x, int jml)	{
	int i=0;
	printf("%s: %3d -->", jud, jml);
	for (i=0; i<jml; i++)	{
		printf(" %02x", x[i]);
	}
	printf("\r\n");
}

int cek_crc_mod(unsigned char *x)	{
	unsigned char lo, hi;
	unsigned short mbhcrc;
	mbhcrc = usMBCRC16((unsigned char *) x, 6, 0);
	hi = ((mbhcrc&0xFF00)>>8);	lo = (mbhcrc&0xFF);
	//printf(">>> %s hi: %02x, lo: %02x -->", __FUNCTION__, hi, lo);
	
	if (hi==sping[7] && lo==sping[6]) {
		//printf("SIP crc\r\n");
		return 1;
	}
	return 0;
}

int parsing_mod(unsigned char *x)	{
	struct t_env *p_env3;
	p_env3 = (char *) ALMT_ENV;
	
	p_mod_sl.almt = x[0];
	p_mod_sl.cmd  = x[1];
	p_mod_sl.reg  = (x[2] << 8) + x[3]; 
	//p_mod_sl.jml  = ((x[4]<<8)+x[5])/4;
	//p_mod_sl.jml  = ((x[4]<<8)+x[5])/2;		// pake QModMaster
	p_mod_sl.jml  = ((x[4]<<8)+x[5]);
	//printf("al: %d, cmd: %d, reg: %d, jml: %d\r\n", p_mod_sl.almt, p_mod_sl.cmd, p_mod_sl.reg, p_mod_sl.jml);
	
	if (p_mod_sl.almt==p_env3->almtSlave)	{		// untuk sendiri
		return 1;
	}
	return 0;
}

int respon_modbus()	{
	struct t_setting *p_kfg;
	p_kfg = (char *) ALMT_KONFIG;
	int i, n=0, ix;
	unsigned char dtmod[256];
	unsigned short mbhcrc;
	int jmlData = (sizeof(data_f)/sizeof(float));
	unsigned char *w;
	char fk=0;
	int iTemp;
	
	//printf("----> %s, reg: %d\r\n", __FUNCTION__, p_mod_sl.reg);
	for (i=0; i<jmlData; i++)	{
		if (p_mod_sl.reg == p_kfg[i].id)	{
			n = i;
			break;
		}
	}
	//printf("mulai id: %d, jml: %d, data: %f, sizedataf: %d\r\n", n, p_mod_sl.jml, data_f[n], sizeof(float));
	
	dtmod[0] = p_mod_sl.almt;
	dtmod[1] = p_mod_sl.cmd;
	dtmod[2] = p_mod_sl.jml*4;
	for (i=0; i<p_mod_sl.jml; i++)		{
		iTemp = (int) (data_f[n+i]*10);
		memcpy( (char *) &ix, (char *) &data_f[n+i], 4);
		
		#if 1		// versi RedLion
		dtmod[3+i*4] = (unsigned char) ( (ix>>24) & 0xff );		// 3
		dtmod[4+i*4] = (unsigned char) ( (ix>>16) & 0xff );		// 2
		dtmod[5+i*4] = (unsigned char) ( (ix>>8)  & 0xff );		// 1
		dtmod[6+i*4] = (unsigned char) (  ix & 0xff );				// 0
		#endif
		#if 0
		dtmod[6+i*4] = (unsigned char) ( (ix>>24) & 0xff );		// 3
		dtmod[5+i*4] = (unsigned char) ( (ix>>16) & 0xff );		// 2
		dtmod[4+i*4] = (unsigned char) ( (ix>>8)  & 0xff );		// 1
		dtmod[3+i*4] = (unsigned char) (  ix & 0xff );				// 0
		#endif
	}
	ix = 3+4*p_mod_sl.jml;
	
	#if 1
	mbhcrc = usMBCRC16((unsigned char *) dtmod, ix, 0);
	dtmod[ix+1] = ((mbhcrc&0xFF00)>>8);	dtmod[ix] = (mbhcrc&0xFF);
	ix += 2;
	#endif
	
	#if 0
	//printf(">>>>> BALAS: %d -->", ix);
	FIO0SET = TXDE;
	FIO0SET = RXDE;
	#endif
	
	w = (char *) &dtmod;
	for (i=0; i<ix; i++)	{
		//printf("%02hX ", *w);
		serX_putchar(3, w++, 80);			// 150
		//xSerialPutChar3(1, *w++, 150);
		
		//
	}
	//printf("\r\n");	
	vTaskDelay(80);
	return ix;

}

int proses_mod_cmd()	{
	int hsl=0;
	//pr_mod("\r\n>> BACA: ", sping, np);
	hsl = cek_crc_mod(sping);
	if (hsl==1)	{
		//printf(" > LULUS < !!!\r\n");
		if (parsing_mod(sping)==1)	{
			//printf("__PROSES DATA KITA !!\r\n");
			respon_modbus();
		}	
	}
}

void sedot_mod(int ch)	{
	unsigned char lo, hi;
	unsigned short mbhcrc;
	char x = (char) ch;
	int hsl=0, r;
	//char stmp[50];
	
	sping[np] = x;
	sping[np+1] = '\0';
	
	#if 0
	if (np==0)	{
		printf("m: ");
	}
	#endif
	
	np++;
	#if 0
	if (np>7)	{
		#if 1
		printf("#### : ");
		for (r=0; r<np; r++)	{
			printf("%02x ", sping[r]);
		}
		printf("\r\n");
		#endif

		hsl = cek_crc_mod(sping);
		if (hsl==1)	{
			//printf(" > LULUS < !!!\r\n");
			if (parsing_mod(sping)==1)	{
				//printf("__PROSES DATA KITA !!\r\n");
				respon_modbus();
			}
			
		}
		
		//printf("######## : %s--\r\n", sping);
		np=0;
	}
	#endif
}
#endif

#ifdef PAKAI_SENSOR_JARAK
void parsing_ping(int ch)	{
	char x = (char) ch;
	int hsl=0;
	char stmp[50];
	
	struct t_env *penv;
	penv = (char *) ALMT_ENV;
	
	if (np<50)	{
		if ( (x=='\r') || (x=='\n') )	{
			//printf("+++++++++ lengkap : %s\r\n", sping);
			if (strncmp(sping, "PING", 4)==0)	{
				sscanf(sping, "%s%d", stmp, &hsl);
				data_f[0] = (float) (penv->kalib[0].m * -hsl)+penv->kalib[0].C;
				data_f[1] = (float) (penv->kalib[0].m * hsl)+penv->kalib[0].C;
				//printf("hasil: %d : %f\r\n", hsl, data_f[0]);
				np = 0;
			}
			sping[np] = '\0';
		} else {
			//printf("  ---> kumpul ch: %c, np: %d\r\n", x, np);
			sping[np] = x;
			sping[np+1] = '\0';
			//printf("######## : %s\r\n", sping);
			np++;
		}
	} else {
		np = 0;
		strcpy(sping, "");
	}
	
}
#endif


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
		//printf("-------> sumber: %d --- qq: %d\r\n", sumber, qq);
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
		//xTaskCreate( kirimcepat, "kirimcepat_task", (configMINIMAL_STACK_SIZE * nKirimCepat), NULL, tskIDLE_PRIORITY+3, ( xTaskHandle * ) &hdl_kirimcepat);
		xTaskCreate( kirimcepat, "kirimcepat_task", (configMINIMAL_STACK_SIZE * nKirimCepat), NULL, tskIDLE_PRIORITY, ( xTaskHandle * ) &hdl_kirimcepat);
	#endif
}

#endif
