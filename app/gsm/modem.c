
#include "FreeRTOS.h"
//#include "monita/monita_uip.h"
#include "modem.h"

char sCmdM[200];
char sRespM[200];

#define CMD_LIST_SMS	"AT+CMGL=\"ALL\"\r\n"
#define CMD_DEL_SMS		"AT+CMGD="

#ifdef PAKAI_MODEM_GSM
	struct t_modem {
		int	 ix;			// id sms
		char nomor[20];		// nomor pengirim sms
		char st;			// status sms
		char isi[170];
	};
	
	struct t_modem mPesan[JML_MEM_SMS];


	void init_mem_sms()	{
		int ax;
		for (ax=0; ax<JML_MEM_SMS; ax++)	{
			mPesan[ax].ix = 0;
		}
	}

	void perintah_modem(char cmd)	{
		extern char index_gsm;
		char hasil_modem=0;
		if (cmd==CEK_AT)
			hasil_modem = cek_AT_cmd();
		if (cmd==BACA_SMS)		
			hasil_modem = membaca_sms();
		if (cmd==HAPUS_SMS)		{
			hasil_modem = menghapus_sms(index_gsm);
			printf("hasil modem: %d\r\n", hasil_modem);
		}
			//mengirim_sms();
		
		if (hasil_modem)
			flagModem = 0;
	}

	int membaca_sms()	{
		//ambil_sms();
		if (ambil_sms())	
			aksi_sms();
		else {
			printf("Tidak Ada SMS !!\r\n");
		}
		return 1;
	}

	int ambil_sms()	{
		char ix, nn = 0;
		int *pi, *ps, id,  st;
		
		kirimCmdModem(CMD_LIST_SMS,sRespM);		// +CMGL
		while(strncmp(sRespM,"OK", 2)!=0)	{
			if ( (strncmp(sRespM,"\r\n", 2)!=0) || (strcmp(sRespM,"")!=0) )	{
				if (strncmp(sRespM, "+CMGL", 5)==0)	{
					//sta = st; ind = in;
					pi = &id;	ps = &st;
					cari_pengirim(sRespM, pi, &mPesan[nn].nomor, ps);
					//printf("status: %d, index: %d\r\n", st, id);
					mPesan[nn].st = (char) st;
					mPesan[nn].ix = (char) id;
					//printf("-----> nn: %d, ix: %d, nomor: %s, st: %d\r\n", 	\
					//	nn, mPesan[nn].ix, mPesan[nn].nomor, mPesan[nn].st);
					ix = 1;
				} else {
					ix = 0;
					strcpy(mPesan[nn].isi, sRespM);
					//printf("=========> isi: %s\r\n", 	mPesan[nn].isi);
					nn++;
					//printf("  SMS : %s\r\n", sRespM);
				}
			}
			kirimCmdModem("",sRespM);
		}
		return nn;
	}
	
	int aksi_sms()	{
		int al = 0;
		
		printf("_________Aksi SMS____________\r\n");
		for (al=0; al<JML_MEM_SMS; al++)	{
			if (mPesan[al].ix>0)	{
				printf("al: %d, isi [%d] dari %s : %s\r\n", \
					al, mPesan[al].ix, mPesan[al].nomor, mPesan[al].isi);
			}
		}
	}
	
	int mengirim_sms()	{
		kirimCmdModem(CMD_LIST_SMS,sRespM);		// +CMGL
	}
	
	int menghapus_sms(int idx)	{
		int xx=0;
		sprintf(sCmdM, "%s%d\r\n", CMD_DEL_SMS, idx);
		kirimCmdModem(sCmdM, sRespM);		// +CMGD
		while(strncmp(sRespM,"OK", 2)!=0)	{
			kirimCmdModem("",sRespM);
		}
		if (xx==0)
			return 1;
		else
			return xx;
	}
	
int cari_pengirim(char *sms, int *idx, char *nomor, int *status) {
	char * pchs, * pcht;
	char ix[20];
	int nm=0, aw, ak, nX;
	
	pchs=strstr(sms,":")+2;
	do {
		if (pchs!=NULL) {
			aw = pchs-sms;
			//printf("pchs: %s\r\n", pchs);
			pcht=strstr(pchs+1,",");
			if (pcht!=NULL)	{
				ak = pcht-sms;
				//printf("awal: %d, awal: %d\r\n", aw, ak);
			}
		}

		if (nm==0)	{
			strncpy(ix, pchs, ak-aw);
			*idx = atoi (ix);
			//printf("idx:__%d___,   ", *idx);
		}
		if (nm==1)	{
			strncpy(ix, pchs+1, ak-aw-1);
			ix[ak-aw-2] = 0;
			//printf("ix: %s\r\n", ix);
			//strcpy(status, ix);
			#if 1
			//printf("status:__%s___%s__%d\r\n", ix, status, strlen(ix));
			if (strcmp(ix, "REC READ")==0)	{
				*status = 1;
				//printf("status %d !!!\r\n");
			} else if (strcmp(ix, "REC UNREAD")==0)	{
				*status = 2;
			} else {
				*status = 10;
			}
			//printf("status:__%d\r\n", *status);
			#endif
		}
		if (nm==2)	{
			//printf("pchs: %s\r\n", pchs);
			strncpy(ix, pchs+1, ak-aw);
			ix[ak-aw-2] = '\0';
			strcpy(nomor, ix);
			//printf("no:__%s___\r\n", nomor);
			//if (strcmp(nomor, "+6282114722505")==0)	{
			//	printf("benar\r\n");
			//}
		}
		nm++;
		pchs=strstr(pchs,",")+1;

	} while (nm<3);	
  	return 1;
}
	
	
	int cek_AT_cmd()	{
		kirimCmdModem("AT\r\n",sRespM);		// AT >> OK
		while (strncmp(sRespM,"OK", 2)!=0)	{
			if (strncmp(sRespM,"\r\n", 2)!=0)
				printf("Respon : %s\r\n", sRespM);					// sms ke-1;
			// proses data Respon;
			//kirimCmdModem("\r\n",sRespM);		// +CMGL

		}
		return 1;
	}
	
	
	void kirimCmdModem(char *psCmdM,char * psRespM)	{
		int mm;
		char cM[2];
		int nn = 0, mx=0;
		
		
		//printf("===> kirim : %s", psCmdM);
		//if (flagModem)	{
		//	flagModem = 0;		// Tanda modem SIBUK !!!
			ser2_putstring(psCmdM);
			do {
				for (mm=0; mm<nRespM; mm++)	{
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
				psRespM[nn] = '\0';
				//printf("Respon : %s\r\n", sRespM);
				mm = sizeof (psCmdM)-2;
				mx++;
				if (mx>50)	break;			// escape from SATAN LOOP !!!
			} while ( (strncmp(psRespM, psCmdM, mm)==0) || (strncmp(psRespM,"\r\n", 2)==0) );
			//printf(">>> Respon: %s \r\n", psRespM);
		//}
		//flagModem = 1;			// Tanda modem FREE !!!
	}

#endif





#ifdef PAKAI_MODEM_SERIAL

#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"

int cek_awal(void) {
	char cmd_ftp[60];
	int i=0, flag=2;
	
	//while(1) {
	for(i=0; i<20; i++) {
		vTaskDelay(50);
		strcpy(cmd_ftp,"");
		baca_serial(cmd_ftp, 10, 20);
		
		#ifdef DEBUG_FTP
		printf("respon%d: %s : %d %s\r\n", i+1, cmd_ftp, strlen(cmd_ftp), __FUNCTION__);
		#endif

		if ((strncmp(cmd_ftp,"+WIND",5)!=0) && (strlen(cmd_ftp)==0))	{	// sudah tidak +WIND
			flag--;
			if (flag==0)		break;
		}
	}
	/*
	sprintf(cmd_ftp, "ATE0\r\n");
	serX_putstring(PAKAI_GSM_FTP, cmd_ftp);
	baca_serial(cmd_ftp, 10, 20);
	//*/
	return 10;
}

#endif
