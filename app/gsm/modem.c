
#include "FreeRTOS.h"
//#include "monita/monita_uip.h"
#include "modem.h"

char sCmdM[200];
char sRespM[200];

#define CMD_LIST_SMS	"AT+CMGL=\"ALL\"\r\n"
#define CMD_DEL_SMS		"AT+CMGD="
#define CMD_CEK_PULSA	"AT+CUSD=1,"
#define CMD_BACA_SMS	"AT+CMGR="
#define CMD_SMS_ASCII	"AT+CMGF=1\r\n"
#define CMD_SMS_PDU		"AT+CMGF=0\r\n"
#define CMD_KIRIM_SMS	"AT+CMGS="

#define CTRL_Z			26
#define HP_DEFAULT		"+6282114722505"

#define PULSA_SIMPATI	"*888#"
#define PULSA_AXIS		"*888#"

#define MAX_LOOP_MODEM	50
char * smsDibalas[] = {"pulsa", "data", "nama", "satuan"};

#ifdef PAKAI_MODEM_GSM
	struct t_modem {
		int	 ix;			// id sms
		char nomor[20];		// nomor pengirim sms
		char st;			// status sms
		char isi[170];
		char balas;
	};
	
	struct t_modem mPesan[MAX_MEM_SMS];

	void init_mem_sms()	{
		int ax;
		for (ax=0; ax<JML_MEM_SMS; ax++)	{
			mPesan[ax].ix = 0;
			mPesan[ax].balas = 0;
		}
	}

	void perintah_modem(char cmd)	{
		extern char index_gsm;
		char hasil_modem=0;
		
		//printf("%s() masuk... ", __FUNCTION__);
		
		if (cmd==CEK_AT)
			hasil_modem = cek_AT_cmd();
		if (cmd==BACA_SMS)	{
		//	printf("BACA SMS !  ");
			hasil_modem = membaca_sms();
		}
		if (cmd==HAPUS_SMS)
			hasil_modem = menghapus_sms(index_gsm);
		if (cmd==CEK_PULSA)
			hasil_modem = mengecek_pulsa();
		if (cmd==BACA_SMS_TUNGGAL)
			hasil_modem = ambil_sms_tunggal(index_gsm);
		if (cmd==KIRIM_SMS)	{
			printf("kirim sms ...\r\n");
			//hasil_modem = aksi_kirim_sms_ascii();
			hasil_modem = aksi_kirim_sms_ascii(HP_DEFAULT, "coba kirim dari sini jo ... !!\n");
		}
		//printf("*******\r\n");
		if (hasil_modem)
			flagModem = 0;
	}

	int membaca_sms()	{
		//ambil_sms();
		#if 1
		if (ambil_sms())	
			aksi_sms();
		else {
			printf("Tidak Ada SMS !!\r\n");
		}
		#endif
		return 1;
	}
	
	int mengecek_pulsa()	{
		printf("pulsa: %d\r\n",ambil_pulsa());;
		#if 0
		if (ambil_pulsa())	
			aksi_sms();
		else {
			printf("Tidak Ada balasan Pulsa !!\r\n");
		}
		#endif
		return 1;
	}

	int ambil_sms()	{
		char ix, nn = 0;
		int *pi, *ps, id,  st;
		
		kirimCmdModem(CMD_LIST_SMS,sRespM);		// +CMGL
		while(strncmp(sRespM,"OK", 2)!=0)	{
			if ( (strncmp(sRespM,"\r\n", 2)!=0) || (strcmp(sRespM,"")!=0) )	{
				if (strncmp(sRespM, "+CMGL", 5)==0)	{
					pi = &id;	ps = &st;
					//cari_pengirim(sRespM, pi, &mPesan[nn].nomor, ps);
					terjemah_sms(sRespM, pi, &mPesan[nn].nomor, ps);
					printf("status: %d, index: %d\r\n", st, id);			// di printf baru nilai bener !!
					mPesan[nn].st = (char) st;
					mPesan[nn].ix = (char) id;
					//printf("-----> nn: %d, st: %d, ix: %d, nomor: %s\r\n", 	\
					//	nn, mPesan[nn].ix, mPesan[nn].st, mPesan[nn].nomor);
					ix = 1;
				} else {
					ix = 0;
					toLower(mPesan[nn].isi, sRespM);
					//printf("=========> isi: %s\r\n", 	mPesan[nn].isi);
					nn++;
					//printf("  SMS : %s\r\n", sRespM);
				}
			}
			kirimCmdModem("",sRespM);
		}
		return nn;
	}
	
	int ambil_sms_tunggal(int idx)	{
		int xx=0, nn=0, fm=JML_MEM_SMS;
		int *pi, *ps, id,  st;
		pi = &id;	ps = &st;
		strcpy(sRespM, "");
		strcpy(mPesan[fm].isi,"");
		sprintf(sCmdM, "%s%d\r\n", CMD_BACA_SMS, idx);
		nn = kirimCmdModem(sCmdM, sRespM);		// +CMGD
		
		while ( (strncmp(sRespM,"+CMGR", 5)!=0) && (xx<MAX_LOOP_MODEM) )	{
			nn = kirimCmdModem("", sRespM);		// +CMGD
			xx++;
		}
		
		if (xx==MAX_LOOP_MODEM)		return 0;			// biasanya langsung, xx=0
		//printf("jml xx: %d, Resp: %s\r\n", xx, sRespM);

		// masuk ke +CMGR
		terjemah_sms(sRespM, pi, &mPesan[fm].nomor, ps);
		//printf("status: %d, index: %d\r\n", st, id);
		mPesan[fm].st = (char) st;
		mPesan[fm].ix = (char) id;
		
		#if 0
		nn = kirimCmdModem("", sRespM);
		strcpy(mPesan[fm].isi, sRespM);
		#endif
		
		#if 1
		xx = 0;
		while ( (strncmp(sRespM,"OK", 2)!=0) && (xx<MAX_LOOP_MODEM) )	{
			nn = kirimCmdModem("", sRespM);
			printf("nn: %d, %s\r\n", nn, sRespM);
			if (nn>2)	{		// "OK"
				//printf("nn: %d, %s\r\n", nn, sRespM);
				strcpy(mPesan[fm].isi, sRespM);
			}
			xx++;
		}
		#endif
		// masuk OK
		if (strncmp(sRespM,"OK", 2)==0)	{
			printf("_____Pesan: %s\r\n", mPesan[fm].isi);
			return 1;
		}
		return 0;
	}
	
	int aksi_sms()	{
		int al = 0;
		
		printf("_________Aksi SMS____________\r\n");
		for (al=0; al<JML_MEM_SMS; al++)	{
			if (mPesan[al].ix>0)	{
				printf("al: %d, isi [%d] [%d] dari %s : %s\r\n", \
					al, mPesan[al].ix, mPesan[al].st, mPesan[al].nomor, mPesan[al].isi);
				balas_sms(al);
			}
		}
	}
	
	void balas_sms(int idx)	{
		int nosms=0;
		printf("req: %s\r\n", mPesan[idx].isi);
		if (strcmp(mPesan[idx].isi, "monita pulsa")==0)	{
			nosms = ambil_pulsa();
			printf("no: %d, sender: %s, isi: %s\r\n", nosms, mPesan[idx].nomor, mPesan[nosms].isi);
			aksi_kirim_sms_ascii(mPesan[idx].nomor, mPesan[nosms].isi);
		}
	}
	
	int aksi_kirim_sms_ascii(char *no, char *isiee)	{
		char xx=0, nn = 0;
		
		// set sbg ASCII
		kirimCmdModem(CMD_SMS_ASCII,sRespM);		// mode ASCII
		while ( (strncmp(sRespM,"OK", 2)!=0) && (xx<MAX_LOOP_MODEM) )	{
			kirimCmdModem("",sRespM);
			xx++;
		}
		if (xx==MAX_LOOP_MODEM)		return 0;
		
		// kirim pesan
		xx=0;
		sprintf(sCmdM, "%s\"%s\"\r\n", CMD_KIRIM_SMS, no);
		//sprintf(sCmdM, "AT+CMGS=\"082114722505\"\r\n");
		printf("sCmdM: %s", sCmdM);
		kirimCmdModem(sCmdM,sRespM);		// mode ASCII
		while ( (strncmp(sRespM,">", 1)!=0) && (xx<MAX_LOOP_MODEM) )	{
			kirimCmdModem("",sRespM);
			xx++;
		}
		if (xx==MAX_LOOP_MODEM)		return 0;
		
		//sprintf(sCmdM, "%s%c", "kirim dari modem airlink\nbaris ke-2\r\nsetelah rn", CTRL_Z);
		sprintf(sCmdM, "%s%c", isiee, CTRL_Z);
		kirimCmdModem(sCmdM, sRespM);		// mode ASCII
		xx=0;
		while ( (strncmp(sRespM,"+CMGS", 5)!=0) && (xx<MAX_LOOP_MODEM) )	{
			kirimCmdModem("",sRespM);
			xx++;
		}
		if (xx==MAX_LOOP_MODEM)		return 0;
		
		// terjemah +CMGS
		if (strncmp(sRespM,"+CMGS", 5)==0)	{
			printf("terjemah +CMGS: %s\r\n", sRespM);
		}
		
		xx=0;
		do {
			kirimCmdModem("",sRespM);
			xx++;
		} while ( (strncmp(sRespM, "OK", 2)!=0) && (xx<MAX_LOOP_MODEM) );
		
		if (xx==MAX_LOOP_MODEM)		return 0;
		else						return 1;
	}
	
	void toLower(char *hsl, char *sbesar) {
		int n = strlen(sbesar);
		int q;

		for (q=0; q<n; q++)
			hsl[q] = (sbesar[q]>='A' && sbesar[q]<='Z')?('a'+sbesar[q]-'A'):sbesar[q];
		hsl[n]='\0';
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
	
	//int ambil_pulsa(int idx)	{
	int ambil_pulsa()	{
		printf("__________%s()____________\r\n", __FUNCTION__);
		int xx=0, nn=0;
		char isine[180];
		int *pi, *ps, id,  st;
		pi = &id;	ps = &st;
		sprintf(sCmdM, "%s%s\r\n", CMD_CEK_PULSA, PULSA_AXIS);
		kirimCmdModem(sCmdM, sRespM);		// +CMGD
		do	{
			kirimCmdModem("", sRespM);		// +CMGD
			xx++;
			if (strncmp(sRespM, "+CUSD", 5)==0)	{
				terjemah_sms(sRespM, pi, &isine, ps);
				while ((int)(mPesan[nn].ix)>0)	{
					//printf("nn: %d\r\n", mPesan[nn].ix);
					nn++;
				}
				mPesan[nn].ix	= id;
				mPesan[nn].st	= st;
				strcpy(mPesan[nn].isi, isine);
				
				printf("nn: %d, idx: %d, st: %d, isi: %s\r\n", nn, mPesan[nn].ix, mPesan[nn].st, mPesan[nn].isi);
				return nn;
			}
		} while (xx<MAX_LOOP_MODEM);
		return 0;
	}
	
	#if 0		// rusak
	int terjemah_sms(char *sms, int *idx, char *nomor, int *status) {
		char * pchs, * pcht;
		char ix[180];
		int nm=0, aw, ak, nX;
		int loop, flagS;
		if (strncmp(sms,"+CMGL",5)==0)	{
			flagS = 1;
			loop = 3;
		}
		if (strncmp(sms,"+CUSD",5)==0)	{
			flagS = 2;
			loop = 3;
		}
		if (strncmp(sms,"+CMGR",5)==0)	{
			flagS = 3;
			loop = 3;
		}

		pchs=strstr(sms,":")+2;
		do {
			if (pchs!=NULL) {
				aw = pchs-sms;
				//printf("pchs: %s\r\n", pchs);
				pcht=strstr(pchs+1,",");
				if (pcht!=NULL)	{
					ak = pcht-sms;
					//printf("awal: %d, awal: %d\r\n", aw, ak);
				} else {
					ak = strlen(sms);
				}
			} else {
				printf("mentok disini !!!\r\n");
			}

			if (nm==0)	{
				if (flagS==3)	{		// baca SMS tunggal
					*idx = 0;
					nm++;
					//strncpy(ix, pchs, ak-aw);
					//printf("lanjut sajjah .... %s\r\n", ix);
				} else {
					strncpy(ix, pchs, ak-aw);
					*idx = atoi (ix);
					if (flagS==2)	{		// cek PULSA
						if (*idx==4)	
							return 0;
					}
					//printf("id: %d\r\n", *idx);
				}
			}
			if (nm==1)	{
				strncpy(ix, pchs+1, ak-aw-1);
				ix[ak-aw-2] = '\0';
				//printf("ix: %s\r\n", ix);
				if ( (flagS==1) || (flagS==3) )	{
					if (strcmp(ix, "REC READ")==0)	{
						*status = 1;
						//printf("status %d !!!\r\n");
					} else if (strcmp(ix, "REC UNREAD")==0)	{
						*status = 2;
					} else {
						*status = 10;
					}
					//return 0;
					//printf("status:__%d\r\n", *status);
				}
				if (flagS==2)	{
					strcpy(nomor, ix);
				}
				//return 0;
			}
			if (nm==2)	{
				//printf("pchs: %s\r\n", pchs);
				//vTaskDelay(100);
				if ( (flagS==1) || (flagS==3) )	{
					strncpy(ix, pchs+1, ak-aw);
					//printf("st:__%s___\r\n", ix);
					ix[ak-aw-2] = '\0';
					//printf("st:__%s___\r\n", ix);
					//return;
					strcpy(nomor, ix);
					//printf("no:__%s___\r\n", nomor);
				}
				if (flagS==2)	{
					*status = atoi (pchs);
					//printf("st:__%d___\r\n", *status);
				}
			}
			nm++;
			pchs=strstr(pchs,",")+1;
		//} while (nm<3);
		} while (nm<loop);
		//printf("nomor: %s\r\n", nomor);
		return 1;
	}
	#endif
	#if 1
	int terjemah_sms(char *sms, int *idx, char *nomor, int *status) {
		char * pchs, * pcht;
		char ix[180];
		int nm=0, aw, ak, nX;
		int loop, flagS;
		if (strncmp(sms,"+CMGL",5)==0)	{		// list pesan
			flagS = 1;
			loop = 3;
		}
		if (strncmp(sms,"+CUSD",5)==0)	{		// pulsa
			flagS = 2;
			loop = 3;
		}
		if (strncmp(sms,"+CMGR",5)==0)	{		// baca sms tunggal
			flagS = 3;
			loop = 3;
		}

		pchs=strstr(sms,":")+2;
		do {
			if (pchs!=NULL) {
				aw = pchs-sms;
				//printf("pchs: %s\r\n", pchs);
				pcht=strstr(pchs+1,",");
				if (pcht!=NULL)	{
					ak = pcht-sms;
				//	printf("awal: %d, awal: %d\r\n", aw, ak);
				} else {
					ak = strlen(sms);
				}
			} else {
				printf("mentok disini !!!\r\n");
			}

			if (nm==0)	{		// index
				if (flagS==3)	{		// baca SMS tunggal
					*idx = 0;
					nm++;
				} else {
					strncpy(ix, pchs, ak-aw);
					*idx = atoi (ix);
					if (flagS==2)	{
						if (*idx==4)	
							return 0;
					}
				}
			}
			if (nm==1)	{		// READ/UNREAD
				strncpy(ix, pchs+1, ak-aw-1);
				if (flagS==3)		ix[ak-aw-1] = '\0';
				else				ix[ak-aw-2] = '\0';
				//printf("ix: %s\r\n", ix);
				if ( (flagS==1) || (flagS==3) )	{
					if (strcmp(ix, "REC READ")==0)	{
						*status = 1;
						//printf("status %d !!!\r\n");
					} else if (strcmp(ix, "REC UNREAD")==0)	{
						*status = 2;
					} else {
						*status = 10;
					}
					//printf("status:__%d\r\n", *status);
				}
				if (flagS==2)	{
					strcpy(nomor, ix);
				}
			}
			if (nm==2)	{
				//printf("pchs: %s\r\n", pchs);
				if ( (flagS==1) || (flagS==3) )	{
					strncpy(ix, pchs+1, ak-aw);
					//printf("st:__%s___\r\n", ix);
					ix[ak-aw-2] = '\0';
					//printf("st:__%s___\r\n", ix);
					//return;
					strcpy(nomor, ix);
				//printf("no:__%s___\r\n", nomor);
				}
				if (flagS==2)	{
					*status = atoi (pchs);
					//printf("st:__%d___\r\n", *status);
				}
			}
			nm++;
			pchs=strstr(pchs,",")+1;
		//} while (nm<3);
		} while (nm<loop);
		//printf("nomor: %s\r\n", nomor);
		return 1;
	}
	#endif

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

	int kirimCmdModem(char *psCmdM,char * psRespM)	{
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
				//printf(" +++++++ Respon : %s\r\n", sRespM);
				mm = sizeof (psCmdM)-2;
				mx++;
				if (mx>50)	break;			// escape from SATAN LOOP !!!
			} while ( (strncmp(psRespM, psCmdM, mm)==0) || (strncmp(psRespM,"\r\n", 2)==0) );
			//printf(">>>>>>>>>> Respon: %s \r\n", psRespM);
			return nn;
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
