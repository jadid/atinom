/*
	program untuk cek pulsa, kirim sms balik ke no peminta
	sebagai bagian dari 
	solusi Monita Performance Monitoring System

*/


#ifdef PAKAI_SMS

#ifndef __SYSTEM_SMS__
#define __SYSTEM_SMS__

#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"

#include "../monita/monita_uip.h" 


#define PULSA_SIMPATI "*888#"

#define CTRL_Z	26	/* END of TEXT lihat ascii code http://en.wikipedia.org/wiki/ASCII */

volatile char str_sms[128];
char pengirim[20];
int konter_sms=30000;

#define nPESAN	10
int no_pesan[nPESAN], jml=0;
char sPesan[nPESAN];
char sipPesan[nPESAN][20];

int cek_pulsa_exe();
int kirim_sms_exe();

int baca_sms_semua();
int hapus_sms_exe(int argc, char **argv);
int baca_sms_exe(int argc, char **argv);

static tinysh_cmd_t cek_pulsa_cmd={ 0,"cek_pulsa","cek pulsa","", cek_pulsa_exe, 0, 0, 0 };
static tinysh_cmd_t kirim_sms_cmd={ 0,"kirim_sms","kirim sms","[nomer] [pesan]", kirim_sms_exe, 0, 0, 0 };
static tinysh_cmd_t hapus_sms_cmd={ 0,"hapus_sms","hapus sms","[nomer] ", hapus_sms_exe, 0, 0, 0 };
static tinysh_cmd_t baca_sms_cmd={ 0,"baca_sms","baca sms","[nomer]", baca_sms_exe, 0, 0, 0 };
static tinysh_cmd_t baca_sms_semua_cmd={ 0,"baca_semua_sms","baca semua sms","[nomer]", baca_sms_semua, 0, 0, 0 };

void flush_modem() {
	int i;
	int loop;
	
	for (i=0; i<50; i++)
		#ifdef PAKAI_SERIAL_1
			if (PAKAI_SMS==1) {
				ser1_getchar(1, &loop, 20 );
			}
		#endif
		#ifdef PAKAI_SERIAL_2
			if (PAKAI_SMS==2) {
				ser2_getchar(1, &loop, 20 );
			}
		#endif
		#ifdef PAKAI_SERIAL_3
			if (PAKAI_SMS==3)	{
				ser3_getchar(1, &loop, 20 );
			}
		#endif
}

int kirim_sms_ascii(char * dest, char * isiSMS) {	
	// set kirim sms modem ascii
	char pesan[128];
	
	vTaskDelay(100);
	flush_modem();
	strcpy(pesan, isiSMS);
	//printf("Isi Pesan: %s\r\n", str_sms);
	sprintf((char *) str_sms, "AT+CMGF=1\r\n");		//printf("cmd: %s", str_sms);
	serX_putstring(PAKAI_GSM_FTP, str_sms);
	strcpy((char *) str_sms, "");
	baca_serial(str_sms, 20, 100);
	//printf("isi: %s\r\n", str_sms);
	if (strncmp((char *) str_sms, "AT+CMGF", 7) == 0)	{
		//strcpy(str_sms, "");
		baca_serial(str_sms, 20, 100);
	}
	//printf("isi2: %s\r\n", str_sms);
	if (strncmp((char *) str_sms, "OK", 2) != 0)	{
		printf("GAGAL kirim modem ascii\r\n");
		return -1;
	}
	vTaskDelay(10);
	
	// no tujuan SMS
	if (dest[0]=='\"')	{
		sprintf(str_sms, "AT+CMGW=%s\r\n", dest);		
	} else {
		sprintf(str_sms, "AT+CMGW=\"%s\"\r\n", dest);
	}
	//printf("cmd: %s", str_sms);
	serX_putstring(PAKAI_GSM_FTP, str_sms);
	strcpy(str_sms, "");
	baca_serial(str_sms, 128, 100);
	printf("respon: %s \r\n", str_sms);
	
	if (strncmp(str_sms, "AT+CMGW", 7) == 0)	{
		str_sms[0]='\0';
		baca_serial(str_sms, 128, 100);
		//printf("isinya: %s\r\n", str_sms);
	}
	if (strncmp(str_sms, ">", 1) != 0)	{
		printf("GAGAL text sms\r\n");
		return -1;
	}
	
	// isi Pesan SMS
	strcpy(str_sms, pesan);				printf("isi SMS: %s\r\n", str_sms);
	//sprintf(str_sms, "%s", pesan);		printf("isi SMS: %s\r\n", str_sms);
	serX_putstring(PAKAI_GSM_FTP, str_sms);
	
	sprintf(str_sms, "%c", CTRL_Z);		//printf("___kirim CTRL-Z___\r\n");
	serX_putstring(PAKAI_GSM_FTP, str_sms);
	
	//strcpy(str_sms, "");
	str_sms[0]='\0';
	baca_serial(str_sms, 120, 100);
	//printf("isi: %s\r\n", str_sms);
	
	//strcpy(str_sms, "");
	str_sms[0]='\0';
	baca_serial(str_sms, 120, 100);
	//printf("respon: %s\r\n", str_sms);

	if (strncmp(str_sms, "+CMGW", 5) != 0)	{
		printf("GAGAL dapat antrian\r\n");
		return -1;
	}
	
	vTaskDelay(10);
	//return 0;
	
	int antri=0;
	char *piM;
  	
  	piM=strstr(str_sms,":");
  	if (piM!=NULL) {
		antri = atoi(piM+1);
		printf("antri: %s: %d : %d\r\n", piM, atoi(piM+1), antri);
	}
	
	strcpy(str_sms, "");
	baca_serial(str_sms, 120, 50);
	//printf("CMGW OK?: %s\r\n", str_sms);

	// Kirimkan SMS !!!
	if (antri>0) {
		sprintf(str_sms, "AT+CMSS=%d\r\n", antri);		printf("cmd: %s", str_sms);
		serX_putstring(PAKAI_GSM_FTP, str_sms);
		
		strcpy(str_sms, "");
		baca_serial(str_sms, 120, 20);
		printf("1. isi: %s\r\n", str_sms);

		
		//if (strncmp(str_sms, "AT+CMSS", 7) == 0) {
			strcpy(str_sms, "");
			baca_serial(str_sms, 120, 20);
			printf("1.hasil: %s\r\n", str_sms);
		//}
		
		//if (strncmp(str_sms, "+CMSS", 5) == 0) {
			strcpy(str_sms, "");
			baca_serial(str_sms, 120, 20);
			printf("2.hasil: %s\r\n", str_sms);
		//}
			strcpy(str_sms, "");
			baca_serial(str_sms, 120, 20);
			printf("3.hasil: %s\r\n", str_sms);
			
		
			strcpy(str_sms, "");
			baca_serial(str_sms, 120, 20);
			printf("4.hasil: %s\r\n", str_sms);
			
		/*
		if (strncmp(str_sms, "OK", 2) != 0) {
			printf("GAGAL kirim sms !! \r\n"); 
			return -1;
		}
		//*/
		//printf("siap hapus pesan!! \r\n");
		//hapus_sms(antri);
	}
	
	status_modem = 0;
	return 1;
}



int kirim_sms_exe() {
	if (status_modem==1)	{		// modem sibuk
		printf("___modem sibuk___\r\n");
		return 0;		
	}
	status_modem=1;
	
	flush_modem();
	printf("kirim sms\r\n");
	sprintf(str_sms,"test modem %d", konter_sms++);
	kirim_sms_ascii("02192254186", str_sms);
	status_modem=0;
	
	return 1;
}

int cari_pengirim(char *nomor, char *sms) {
	char * pchs, *pstrs;
	int awals=0, akhirs=strlen(sms);
	pchs=strstr(sms,",");
	if (pchs!=NULL)	{
		pchs=strstr(pchs+1,",");
		awals = pchs-sms+1;
		if (pchs!=NULL)	{
			awals = pchs-sms+1;
		}
  	}
  	
  	pstrs=strstr(pchs+1,",");
  	//printf("1. awal: %d, akhir:%d, %s\r\n", awals, akhirs, pstrs);
  	
	if (pchs!=NULL)	{
  		akhirs = pstrs-sms+1;
  	}
  	strcpy(nomor, pchs+1);
  	nomor[akhirs-awals-1]='\0';
  	//printf("2. awal: %d, akhir: %d\r\n", awals, akhirs);
  	//printf("hasil: %s\r\n", sms);
}

void toLower(char *hsl, char *sbesar) {
	int n = strlen(sbesar);
	int q;

	for (q=0; q<n; q++)
		hsl[q] = (sbesar[q]>='A' && sbesar[q]<='Z')?('a'+sbesar[q]-'A'):sbesar[q];
	hsl[n]='\0';
}

int cari_index(char * spesan) {
	char sangka[5];
  	char * nopaw, * nopak;
  	int noaw=0, noak=0;
  	nopaw=strstr(spesan,":")+2;
  	if (nopaw!=NULL) {
  		noaw = nopaw-spesan;
		nopak=strstr(nopaw,",");
		if (nopak!=NULL)
			noak = nopak-spesan;
  	}
  	
  	strncpy(sangka, nopaw, noak-noaw);
  	sangka[noak-noaw] = '\0';
  	//printf("nilai: %d\r\n", atoi(sangka));
  	return (atoi(sangka));
}

int baca_sms_semua() {
	char hasilx[255];
	char hasilb[255];
	//int no_pesan[50], jml=0, yy;
	int yy,zz;
	char fff=0;
	
	//flush_modem();
	cek_awal();
	vTaskDelay(50);

	sprintf(hasilx, "AT+CMGL=\"ALL\"\r\n");		//printf("cmd: %s", hasilx);	
	serX_putstring(PAKAI_GSM_FTP, hasilx);

	while (1) {
		baca_serial(hasilx, 250, 10);
		vTaskDelay(50);
		printf("___isi: %s\r\n", hasilx);
		if (strncmp(hasilx, "+CMGL", 5) == 0) {
			fff = 0;	
		}
		
		if ((strncmp(hasilx, "OK", 2)==0) || (strncmp(hasilx, "ok", 2)==0) || (strncmp(hasilx, "+WIND", 5)==0) || (strncmp(hasilx, "ERR", 3)==0)) {
			break;
		} 
		
		if (fff==1) {
			toLower(hasilb, hasilx);
			printf("hasilb : __%s__, jml: %d\r\n", hasilb, jml);
			//sPesan[jml-1]='x';
			if (strncmp(hasilb, "pulsa", 5)==0)		sPesan[jml-1]='p';
			else if (strncmp(hasilb, "monita", 6)==0)	sPesan[jml-1]='m';
			else if (strncmp(hasilb, "info", 4)==0)		sPesan[jml-1]='i';
			printf("sPesan: %c, index: %d, pengirim: %s, noP: %d, jml: %d\r\n", sPesan[jml-1], yy, sipPesan[jml-1], no_pesan[jml-1], jml-1);
			
			strcpy(hasilb, "");		//strcpy(hasilx, "");
			//fff=0;
		}

		if (strncmp(hasilx, "+CMGL", 5) == 0) {
			yy = cari_index(hasilx);
			cari_pengirim(sipPesan[jml], hasilx);
			no_pesan[jml] = yy;
			//printf("CMGLnya: %s, index: %d, pengirim: %s, noP: %d, jml: %d\r\n", hasilx, yy, sipPesan[jml], no_pesan[jml], jml);
			
			jml++;
			fff=1;
			strcpy(hasilx, "");
		} 
	}
	//printf("jml: %d\r\n", jml);

	if (jml>0) {
		for (yy=0; yy<jml; yy++) {
			//printf("index: %d\r\n", no_pesan[yy]);
			hapus_sms(no_pesan[yy]);
		}
		//jml=0;
	}
	return jml;
}

int baca_sms(int indexnya) {
	char hasilx[128];
	flush_modem();
	strcpy(str_sms, "");
	vTaskDelay(5);
	sprintf(hasilx, "AT+CMGR=%d\r\n", indexnya);		//printf("cmd: %s", hasilx);	
	serX_putstring(PAKAI_GSM_FTP, hasilx);
	
	baca_serial(hasilx, 128, 5);
	//printf("isi: %s\r\n", hasilx);
	if (strncmp(hasilx, "AT+CMGR", 7) == 0)	{
		strcpy(hasilx, "");
		baca_serial(hasilx, 120, 5);
	}
	
	if (strncmp(hasilx, "+CMGR", 5) == 0)	{
		//printf("RESPON: %s\r\n", hasilx);
		cari_pengirim(hasilx, hasilx);
		//printf("Pengirim: %s\r\n", hasilx);
		strcpy(pengirim, hasilx);
		baca_serial(hasilx, 128, 50);
	}
	//strcpy(str_sms, "");
	//printf("isi SMS: %s\r\n", hasilx);
	if (strncmp(hasilx, "OK", 2) == 0) {		// tidak ada SMS
		return 0;
	} else if (strncmp(hasilx, "ERROR", 5) == 0) {
		printf("Ko ERROR !!\r\n");
	}
	strcpy(str_sms, "");
	/*
	while (strncmp(hasilx, "OK", 2) != 0) {
		strcat(str_sms,hasilx);
		strcat(str_sms,"\n");
		baca_serial(hasilx, 120, 50);
	}
	//*/
	strcat(str_sms,hasilx);
	printf("PESAN: %s\r\n", str_sms);
	if (strncmp(hasilx, "OK", 2) == 0) {		// tidak ada SMS
		return 0;
	}
	baca_serial(hasilx, 120, 50);
	printf("akhir: %s\r\n", hasilx);
}

int baca_sms_exe(int argc, char **argv) {
	int no;
	char nourut[10];
	if (status_modem==1)	{		// modem sibuk
		printf("___modem sibuk___\r\n");
		return 0;		
	}
	
	if (argc != 2) {
		printf("Format salah !!\r\n");
		printf("perintah: baca_sms [index]\r\n");
		return -1;
	}
	status_modem = 1;
	
	sprintf(nourut, "%s", argv[1]);	
	no = cek_nomer_valid(nourut, 1000);
	//printf("BACA sms ke %d\r\n", no);
	baca_sms(no);
	
	toLower(str_sms, str_sms);
	if (strncmp(str_sms, "pulsa",5)==0) {
		vTaskDelay(500);
		cek_pulsa();
		printf("SMS %s, %s\r\n", pengirim, str_sms);
		vTaskDelay(500);
		kirim_sms_ascii(pengirim, str_sms);
	}
	if (strncmp(str_sms, "monita",5)==0) {
		
	}
	status_modem = 0;
}

void kirim_sisa_pulsa_exe(char * dest_sms, int mode_pulsa) {
	if (status_modem==1)	{		// modem sibuk
		printf("___modem sibuk___\r\n");
		return 0;		
	}

	status_modem = 1;
	printf("%s\r\n", __FUNCTION__);
	kirim_sisa_pulsa(dest_sms, mode_pulsa);
	status_modem = 0;
}

void kirim_sisa_pulsa(char * dest_sms, int mode_pulsa) {
	cek_pulsa();
	vTaskDelay(100);
	
	if (mode_pulsa==0)
		kirim_sms_ascii(dest_sms, str_sms);
	if (mode_pulsa==1)
		kirim_sms_ascii("02192254186", str_sms);
	if (mode_pulsa==2) {
		//printf("str_sms: %s\r\n", str_sms);
		vTaskDelay(100);
		kirim_sms_ascii("081908870878", str_sms);
		//kirim_sms_ascii("08170504365", str_sms);
		vTaskDelay(100);
		kirim_sms_ascii("08118888623", str_sms);
		vTaskDelay(100);
		kirim_sms_ascii("02192254186", str_sms);
	}
	if (mode_pulsa==3) {
		int duit=duit_pulsa(str_sms);
		char dpls[120];
		//printf("masuk mode 3: duit: %d, str_sms: %s\r\n", duit, str_sms);
		if (duit<10000)	{
			strcpy(dpls, "Sisa pulsa: Rp.");
			strcat(dpls, str_sms);
			strcat(dpls, "Harap diisi yak!!");

			//kirim_sms_ascii("081908870878", str_sms);
			//kirim_sms_ascii("08170504365", dpls);
			//vTaskDelay(100);
			//kirim_sms_ascii("08118888623", str_sms);
			//vTaskDelay(100);
			kirim_sms_ascii("02192254186", dpls);
		}
	}
	
}

int sms_cron() {
	int no, jmlSMS;
	char nourut[10];
	if (status_modem==1)	{		// modem sibuk
		printf("___modem sibuk___\r\n");
		return 0;		
	}

	status_modem = 1;
	/*
	sprintf(nourut, "%s", argv[1]);	
	no = cek_nomer_valid(nourut, 1000);
	//printf("BACA sms ke %d\r\n", no);
	//*/
	jmlSMS = baca_sms_semua();
	//printf("jml: %d, %s\r\n", jml, __FUNCTION__);

	if (jmlSMS>0) {
		for(no=0; no<jmlSMS; no++) {
			printf("loop %d. sPesan: %c sip: %s, no_pesan: %d\r\n", no+1, sPesan[no], sipPesan[no], no_pesan[no]);
			// aksinya
			//*
			if (sPesan[no]=='p') kirim_sisa_pulsa(sipPesan[no], 0);
			if (sPesan[no]=='m') {		}
			if (sPesan[no]=='i') {		}
			
			sPesan[no]='x';
			hapus_sms(no_pesan[no]);
			//*/
		}
	}
	
	
	
	jml=0;
	status_modem = 0;
}
/*
int baca_sms_cron() {
	int no;
	char nourut[10];
	if (status_modem==1)	{		// modem sibuk
		printf("___modem sibuk___\r\n");
		return 0;		
	}
	status_modem = 1;
	printf("masuk %s\r\n", __FUNCTION__);
	cek_pulsa();
	kirim_sms_ascii("081908870878", str_sms);
	//vTaskDelay(100);
	kirim_sms_ascii("0219254186", str_sms);
	//vTaskDelay(100);
	//kirim_sms_ascii("08118888623", str_sms);
	status_modem = 0;
}
//*/

int hapus_sms(int no) {
	flush_modem();
	strcpy(str_sms,"");
	sprintf(str_sms, "AT+CMGD=%d\r\n", no);	//printf("cmd: %s\r\n", str_sms);
	serX_putstring(PAKAI_GSM_FTP, str_sms);
	
	baca_serial(str_sms, 20, 50);
	//printf("1.hasil: %s\r\n", str_sms);
	
	if (strncmp(str_sms, "AT+CMGD", 7) == 0)	{
		strcpy(str_sms, "");
		baca_serial(str_sms, 20, 50);
		//printf("2.hasil: %s\r\n", str_sms);
	}
	
	if (strncmp(str_sms, "OK", 2) == 0)	{
		printf("SMS no %d dihapus\r\n", no);
	} else {
		return -1;
	}
}

int hapus_sms_exe(int argc, char **argv) {
	char nourut[10];
	int no;
	if (argc != 2) {
		printf("Format salah\r\n");
		printf("perintah: hapus_sms [no]\r\n");
		return -1;
	} else {
		sprintf(nourut, "%s", argv[1]);	
		no = cek_nomer_valid(nourut, 1000);
		//printf("menghapus sms no %d\r\n", no);
		if (no>0) hapus_sms(no);
		return 1;
	}
}

int cek_pulsa_exe()	{
	if (status_modem==1)	{		// modem sibuk
		printf("___modem sibuk___\r\n");
		return 0;		
	}
	status_modem=1;
	
	printf("Cek Pulsa\r\n");
	cek_pulsa();

	status_modem=0;
	
	return 1;
}

int duit_pulsa(char * dpls) {
	int a1=0, b2=strlen(dpls);
	char * pd1, *pd2;
	
	pd1=strstr(dpls,".");
	if (pd1!=NULL)	{
  		a1 = pd1-dpls+1;
  	}
  	
  	pd2=strstr(pd1+1,".");
	if (pd2!=NULL)	{
  		b2 = pd2-dpls+20;
  	}
  	strcpy(dpls, pd1+1);
  	dpls[a1-b2-1]='\0';
	//duit = atoi(dpls);
	
	return atoi(dpls);
}


int isiSMSnya(char *hasil, char *str) {
	char * pch, *pstr;
	int awal=0, akhir=strlen(str);
	pch=strstr(str,"\"");
	if (pch!=NULL)	{
  		awal = pch-str+1;
  	}
  	/*
  	pstr=strstr(pch+1,"\"");
	if (pch!=NULL)	{
		
  		akhir = pstr-str+1;
  	}
  	//*/
  	pstr=strstr(pch+1,"f");
	if (pstr!=NULL)	{
  		akhir = pstr-str+20;
  	}
  	//printf("awal: %d, akhir: %d\r\n", awal, akhir);
  	strcpy(hasil, pch+1);
  	hasil[akhir-awal-1]='\0';
  	//printf("hasil: %s\r\n", hasil);
}

int cek_pulsa(void)	{	
	char strpls[128];
	flush_modem();

	sprintf(strpls, "AT+CUSD=1,%s\r\n", PULSA_SIMPATI);
	//serX_putstring(PAKAI_SMS, "AT+CUSD=1,*888#\r\n");
	serX_putstring(PAKAI_SMS, strpls);
	baca_serial(strpls, 20, 5);
	
	//printf("isi: %s\r\n", str_sms);
	if (strncmp(strpls, "+CUSD: 4", 8) == 0)	{
		//printf("Respon: +CUSD: 4\r\n");
		strcpy(strpls, "AT+CUSD=1\r\n");
		serX_putstring(PAKAI_SMS, strpls);
		
		strcpy(strpls, "");
		baca_serial(strpls, 20, 5);
		
		if (strncmp(strpls, "AT+CUSD", 7) == 0)	{
			strcpy(strpls, "");
			baca_serial(strpls, 20, 5);
		}
		
		if (strncmp(strpls, "OK", 2) == 0)	{
			//printf("CUSD: OK !!\r\n");
			//printf("AT+CUSD=1,*888#\r\n");
			sprintf(strpls, "AT+CUSD=1,%s\r\n", PULSA_SIMPATI);
			serX_putstring(PAKAI_SMS, strpls);
			baca_serial(strpls, 20, 5);
		}
		//printf("RESPON: %s\r\n", str_sms);
	}
	
	if (strncmp(strpls, "AT+CUSD", 7) == 0)	{
		baca_serial(strpls, 128, 50);
		//printf(".......AT+CUSD=1,*888# \r\n");
	}
	
	if (strncmp(strpls, "OK", 2) == 0)	{
		baca_serial(strpls, 128, 50);
		//printf("AT+CUSD OK!!\r\n");
		//printf("isi pulsa: %s\r\n", str_sms);
		//isiSMSnya(strpls, strpls);		// ambil nilai pulsa dan kadaluarsa saja
		//printf("PULSA: %s\r\n", str_sms);
		isiSMSnya(strpls, strpls);
		strcpy(str_sms, strpls);
		printf("PULSA: %s\r\n", str_sms);
		return 1;
	}
}

#endif

#endif /* PAKAI_SMS */
