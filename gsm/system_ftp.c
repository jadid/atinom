#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"

#include "../monita/monita_uip.h" 

#ifdef PAKAI_GSM_FTP

#ifndef __SYSTEM_FTP__
#define __SYSTEM_FTP__

#define CTRL_ETX	0x03	/* END of TEXT lihat ascii code http://en.wikipedia.org/wiki/ASCII */
#define CTRL_DLE	0x10	/* Data link escape, untuk memagari jika ada karakter ETX yang ikut dikirim */

//#define modemBebas()	flush_modem(); \
//						vTaskDelay(10);

#define DEBUG_FTP

unsigned int files=0;
unsigned int file_sudah=0;
unsigned int file_sukses=0;

int gsm_ftp()	{
	int fff=0;
	
	if (status_modem==1)	{		// modem sibuk
		printf("___modem sibuk___\r\n");
		return;		
	}
	
	status_modem = 1;
	
	//tanpa_cmd_balik();
	fff = koneksi_on();
	printf("flag FTP: %d\r\n", fff);
	//fff=90;
	if (fff!=90) {
		printf("Koneksi GPRS gagal !!!\r\n");
		koneksi_off(fff);
		status_modem = 0;
		return 0;
	}
	
	files=0;	file_sudah=0;	file_sukses=0;
	
	vTaskDelay(50);
	cari_berkas("J-3", "kirim_ftp");
	
	koneksi_off(fff);
	status_modem = 0;
	printf(" File = %d, dikirim %d, sudah dikirim %d\r\n", files, file_sukses, file_sudah); 
	printf("modem longgar, status modem: %d\r\n", status_modem);
	return 1;
}

void cek_ftp() {
	int fff=0;
	if (status_modem==1)	{		// modem sibuk
		printf("___modem sibuk___\r\n");
		return;		
	}
	
	status_modem = 1;
	//tanpa_cmd_balik();
	fff=koneksi_on();
	
	if (fff==90) {
		printf("_________Konek ftp_________ %d\r\n", fff);
	} else {
		printf("Gagal konek\r\n");
	}

	koneksi_off(fff);
	status_modem = 0;
}

int koneksi_on()	{
	int flag=-1,oz;
	
	#ifdef DEBUG_FTP
	printf("________cek_awal dulu________\r\n");
	#endif
	cek_awal();
	
	vTaskDelay(100);
	flag = 20;
	#ifdef DEBUG_FTP
	printf("________set_wipcfg_on________\r\n");
	#endif
	if (set_wipcfg_on()==0)	{
		flag = 1;
	}
	if (flag==20)	return flag;
	
	vTaskDelay(1000);
	flag = 30;
	#ifdef DEBUG_FTP
	printf("________set_wipbr________\r\n");
	#endif
	if (set_wipbr()==0)	{
		flag = 1;
	}
	if (flag==30)	return flag;
	
	vTaskDelay(50);
	flag = 40;
	#ifdef DEBUG_FTP
	printf("________set_wipbr_apn________\r\n");
	#endif
	if (set_wipbr_apn()==0)	{
		flag = 1;
	}
	if (flag==50)	return flag;
	
	vTaskDelay(50);
	flag = 50;
	#ifdef DEBUG_FTP
	printf("________set_wipbr_user________\r\n");
	#endif
	if (set_wipbr_user()==0)	{
		flag = 1;
	}
	if (flag==50)	return flag;
	
	vTaskDelay(50);
	flag = 60;
	#ifdef DEBUG_FTP
	printf("________set_wipbr_password________\r\n");
	#endif
	if (set_wipbr_passwd()==0) {
		flag = 1;	
	}
	if (flag==60)	return flag;
	
	vTaskDelay(100);
	flag = 70;
	for (oz=0; oz<10; oz++) {
		if (flag == 70) {
			#ifdef DEBUG_FTP
			printf("________KONEK GPRS________\r\n");
			#endif
			if (set_gprs_on()==0)	{
				flag = 1;	
				continue;
			}
			vTaskDelay(10);
			cek_awal();
		}
	}
	if (flag==70)	return flag;
	
	vTaskDelay(100);
	flag = 80;
	for (oz=0; oz<3; oz++) {
		if (flag == 80) {
			#ifdef DEBUG_FTP
			printf("________KONEK FTP________\r\n");
			#endif
			if (set_ftp_on()==0) {
				flag = 90;
				continue;
			}
			vTaskDelay(10);
			cek_awal();
		}
	}

	return flag;
}

int koneksi_off(int fff) {
	vTaskDelay(100);
	switch (fff) {
		case 90:
			goto TUTUP_FTP;
			break;
		case 80:
			goto TUTUP_GPRS;
			break;
		default:
			goto TUTUP_WIPCFG;
			break;
	}
	
	TUTUP_FTP:
	#ifdef DEBUG_FTP
	printf("________PUTUS FTP________\r\n");
	#endif
	set_ftp_off();
	
	vTaskDelay(100);
	TUTUP_GPRS:
	#ifdef DEBUG_FTP
	printf("________PUTUS GPRS________\r\n");
	#endif
	set_gprs_off();
		
	vTaskDelay(300);	
	TUTUP_WIPCFG:
	#ifdef DEBUG_FTP
	printf("________set_wipcfg_off________\r\n");
	#endif
	set_wipcfg_off();
}

int cek_AT() {
	char cmd_ftp[100];
	int i;
	
	sprintf(cmd_ftp, "AT\r\n");
	serX_putstring(PAKAI_GSM_FTP, cmd_ftp);
	baca_serial(cmd_ftp, 100, 10);
	
	#ifdef DEBUG_FTP
	printf("respon: %s\r\n", cmd_ftp);
	#endif
	if (strncmp(cmd_ftp,"AT",2)==0) {
		baca_serial(cmd_ftp, 10, 10);
		#ifdef DEBUG_FTP
		printf("respon1: %s\r\n", cmd_ftp);
		#endif
	}
	/*
	while(1) {
		strcpy(cmd_ftp, "");
		baca_serial(cmd_ftp, 100, 10);
		if (strlen(cmd_ftp)>0 && strncmp(cmd_ftp,"OK", 2)==0)	{
			#ifdef DEBUG_FTP
			printf("%d. respon1: %s\r\n", i+1, cmd_ftp);
			#endif
			break;
		}
		i++;
		if (i>10)	break;
	}
	//*/
	//*
	if (strncmp(cmd_ftp,"OK",2)==0) {
		#ifdef DEBUG_FTP
		printf("%s OK !!\r\n", __FUNCTION__);
		#endif
		return 0;
	}
	//*/
}

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

int tanpa_cmd_balik() {
	char cmd_ftp[20];

	sprintf(cmd_ftp, "ATE0\r\n");
	serX_putstring(PAKAI_GSM_FTP, cmd_ftp);
	baca_serial(cmd_ftp, 20, 10);
	#ifdef DEBUG_FTP
	printf("cmd: %s\r\n", cmd_ftp);
	#endif
	if (strncmp(cmd_ftp,"OK",2)==0)	{
		printf("CMD tanpa balik!!\r\n");
	}
}

int tanpa_wind() {
	char cmd_ftp[20];

	sprintf(cmd_ftp, "AT+WIND=0\r\n");
	serX_putstring(PAKAI_GSM_FTP, cmd_ftp);
	baca_serial(cmd_ftp, 20, 10);
	#ifdef DEBUG_FTP
	printf("cmd: %s\r\n", cmd_ftp);
	#endif
	if (strncmp(cmd_ftp,"OK",2)==0)	{
		printf("CMD TANPA WIND!!\r\n");
	}
}

int set_wipcfg_on(void) {	
	char cmd_ftp[128];

	sprintf(cmd_ftp, "AT+WIPCFG=1\r\n");
	serX_putstring(PAKAI_GSM_FTP, cmd_ftp);
	baca_serial(cmd_ftp, 20, 10);
	
	#ifdef DEBUG_FTP
	printf("cmd: %s\r\n", cmd_ftp);
	#endif
	if (strncmp(cmd_ftp,"AT+WIPCFG",9)==0) {
		baca_serial(cmd_ftp, 20, 10);
		#ifdef DEBUG_FTP
		printf("respon1: %s\r\n", cmd_ftp);
		#endif
	}
	
	if (strncmp(cmd_ftp,"OK",2)==0) {
		strcpy(cmd_ftp,"");
		baca_serial(cmd_ftp, 20, 10);
		#ifdef DEBUG_FTP
		printf("%s OK !! %s\r\n", __FUNCTION__, cmd_ftp);
		#endif
		return 0;
	}
	
	vTaskDelay(100);
	if ( (strncmp(cmd_ftp,"ERROR",5)==0) || (strncmp(cmd_ftp,"+CME", 4)==0) ) {
		//modemBebas();
		
		sprintf(cmd_ftp, "AT+WIPCFG=0\r\n");
		serX_putstring(PAKAI_GSM_FTP, cmd_ftp);
		baca_serial(cmd_ftp, 50, 5);
		
		#ifdef DEBUG_FTP
		printf("cmd: %s\r\n", cmd_ftp);
		#endif
		if (strncmp(cmd_ftp,"AT+WIPCFG",9)==0) {
			baca_serial(cmd_ftp, 20, 10);
			#ifdef DEBUG_FTP
			printf("respon1: %s\r\n", cmd_ftp);
			#endif
		}
		
		if (strncmp(cmd_ftp,"OK",2)==0) {
			baca_serial(cmd_ftp, 20, 10);
			#ifdef DEBUG_FTP
			printf("respon2: %s\r\n", cmd_ftp);
			#endif
			
			//modemBebas();
			
			sprintf(cmd_ftp, "AT+WIPCFG=1\r\n");
			serX_putstring(PAKAI_GSM_FTP, cmd_ftp);
			baca_serial(cmd_ftp, 50, 5);
			
			#ifdef DEBUG_FTP
			printf("cmd: %s\r\n", cmd_ftp);
			#endif
			if (strncmp(cmd_ftp,"AT+WIPCFG",9)==0) {
				baca_serial(cmd_ftp, 20, 10);
				#ifdef DEBUG_FTP
				printf("respon3: %s\r\n", cmd_ftp);
				#endif
			}
			
			if (strncmp(cmd_ftp,"OK",2)==0) {
				#ifdef DEBUG_FTP
				printf("%s OK\r\n", __FUNCTION__);
				#endif
				return 0;
			}
			if ( (strncmp(cmd_ftp,"ERROR",5)==0) || (strncmp(cmd_ftp,"+CME", 4)==0) ) {
				#ifdef DEBUG_FTP
				printf("AT+WIPCFG=1 GAGAL, TETEP ERROR\r\n");
				#endif
				
				return -1;
			}
		}
	} else {
		printf("%s UNKNWON : %s\r\n", __FUNCTION__, cmd_ftp);
		return -1;
	}
}

int set_wipcfg_off(void) {	
	char cmd_ftp[128];

	sprintf(cmd_ftp, "AT+WIPCFG=0\r\n");		printf("cmd: %s", cmd_ftp);
	serX_putstring(PAKAI_GSM_FTP, cmd_ftp);
	baca_serial(cmd_ftp, 50, 5);
	
	#ifdef DEBUG_FTP
	printf("respon: %s : %d\r\n", cmd_ftp, strlen(cmd_ftp));
	#endif
	if (strncmp(cmd_ftp,"AT+WIPCFG",9)==0) {
		baca_serial(cmd_ftp, 20, 10);
		#ifdef DEBUG_FTP
		printf("respon1: %s\r\n", cmd_ftp);
		#endif
	}
	
	if (strncmp(cmd_ftp,"OK",2)==0) {
		//baca_serial(cmd_ftp, 20, 10);
		#ifdef DEBUG_FTP
		printf("SIP %s sudah OK!!\r\n", __FUNCTION__);
		#endif
		return 0;
	}

	if ( (strncmp((char *) cmd_ftp,"ERROR",5)==0) || (strncmp((char *) cmd_ftp,"+CME", 4)==0) ) {
		printf("%s : AT+WIPCFG=0 GAGAL\r\n", __FUNCTION__);
		
	} else {
		printf("%s : AT+WIPCFG=0 UNKNOWN %s\r\n", __FUNCTION__, cmd_ftp);
		
		strcpy(cmd_ftp, "");
		baca_serial(cmd_ftp, 20, 10);
		#ifdef DEBUG_FTP
		printf("respon1: %s\r\n", cmd_ftp);
		#endif
	}
	return -1;
}

int set_wipbr(void) {
	char cmd_ftp[100];
	
	sprintf(cmd_ftp, "AT+WIPBR=1,6\r\n");
	serX_putstring(PAKAI_GSM_FTP, cmd_ftp);
	baca_serial(cmd_ftp, 20, 10);
	
	#ifdef DEBUG_FTP
	printf("cmd: %s\r\n", cmd_ftp);
	#endif
	
	if (strncmp(cmd_ftp,"AT+WIPBR",8)==0) {
		baca_serial(cmd_ftp, 20, 10);
		#ifdef DEBUG_FTP
		printf("respon1: %s\r\n", cmd_ftp);
		#endif
	}
	
	if (strncmp(cmd_ftp,"OK",2)==0) {
		strcpy(cmd_ftp,"");
		baca_serial(cmd_ftp, 20, 10);
		#ifdef DEBUG_FTP
		printf("%s OK !! %s\r\n", __FUNCTION__, cmd_ftp);
		#endif
		return 0;
	}
	
	if ( (strncmp((char *) cmd_ftp,"ERROR",5)==0) || (strncmp((char *) cmd_ftp,"+CME", 4)==0) ) {
		printf("%s : AT+WIPBR=1,6 GAGAL\r\n", __FUNCTION__);
		return -1;
	} else {
		printf("%s : AT+WIPBR=1,6 UNKNOWN %s\r\n", __FUNCTION__, cmd_ftp);
		
		strcpy(cmd_ftp, "");
		baca_serial(cmd_ftp, 20, 10);
		#ifdef DEBUG_FTP
		printf("respon1: %s\r\n", cmd_ftp);
		#endif
		return -1;
	}
	return -1;
}

int set_wipbr_apn(void) {		// AT+WIPBR=2,6,11,"internet"
	char cmd_ftp[60];
	struct t_gsm_ftp *p_dt;
	p_dt = (char *) ALMT_GSM_FTP;
	
	sprintf(cmd_ftp, "AT+WIPBR=2,6,11,\"%s\"\r\n", p_dt->gprs_apn1);	//printf("cmd: %s", cmd_ftp);
	serX_putstring(PAKAI_GSM_FTP, cmd_ftp);
	baca_serial(cmd_ftp, 50, 20);
	
	#ifdef DEBUG_FTP
	printf("cmd: %s\r\n", cmd_ftp);
	#endif
	
	if (strncmp(cmd_ftp,"AT+WIPBR",8)==0) {
		baca_serial(cmd_ftp, 50, 20);
		#ifdef DEBUG_FTP
		printf("respon1: %s\r\n", cmd_ftp);
		#endif
	}
	
	if (strncmp(cmd_ftp,"OK",2)==0) {
		strcpy(cmd_ftp,"");
		baca_serial(cmd_ftp, 50, 20);
		#ifdef DEBUG_FTP
		printf("%s OK !! %s\r\n", __FUNCTION__, cmd_ftp);
		#endif
		return 0;
	}
	
	if ( (strncmp((char *) cmd_ftp,"ERROR",5)==0) || (strncmp((char *) cmd_ftp,"+CME", 4)==0) ) {
		printf("%s : AT+WIPBR=2,6,11,\"apn\" GAGAL\r\n", __FUNCTION__);
		return -1;
	} else {
		printf("%s : AT+WIPBR=2,6,11,\"apn\" UNKNOWN %s\r\n", __FUNCTION__, cmd_ftp);
		
		strcpy(cmd_ftp, "");
		baca_serial(cmd_ftp, 20, 10);
		#ifdef DEBUG_FTP
		printf("respon1: %s\r\n", cmd_ftp);
		#endif
		return -1;
	}
}

int set_wipbr_user(void) {
	char cmd_ftp[100];
	struct t_gsm_ftp *p_dt;
	p_dt = (char *) ALMT_GSM_FTP;
	
	sprintf(cmd_ftp, "AT+WIPBR=2,6,0,\"%s\"\r\n", p_dt->gprs_user);		//printf("cmd: %s", cmd_ftp);
	serX_putstring(PAKAI_GSM_FTP, cmd_ftp);
	baca_serial(cmd_ftp, 50, 10);
	
	#ifdef DEBUG_FTP
	printf("cmd: %s\r\n", cmd_ftp);
	#endif
	
	if (strncmp(cmd_ftp,"AT+WIPBR",8)==0) {
		baca_serial(cmd_ftp, 50, 10);
		#ifdef DEBUG_FTP
		printf("respon1: %s\r\n", cmd_ftp);
		#endif
	}
	
	if (strncmp(cmd_ftp,"OK",2)==0) {
		strcpy(cmd_ftp,"");
		baca_serial(cmd_ftp, 50, 10);
		#ifdef DEBUG_FTP
		printf("%s OK !! %s\r\n", __FUNCTION__, cmd_ftp);
		#endif
		return 0;
	}
	
	if ( (strncmp((char *) cmd_ftp,"ERROR",5)==0) || (strncmp((char *) cmd_ftp,"+CME", 4)==0) ) {
		printf("%s : AT+WIPBR=2,6,0,\"user\" GAGAL\r\n", __FUNCTION__);
		return -1;
	} else {
		printf("%s : AT+WIPBR=2,6,0,\"user\" UNKNOWN %s\r\n", __FUNCTION__, cmd_ftp);
		
		strcpy(cmd_ftp, "");
		baca_serial(cmd_ftp, 20, 10);
		#ifdef DEBUG_FTP
		printf("respon1: %s\r\n", cmd_ftp);
		#endif
		return -1;
	}
}

int set_wipbr_passwd(void) {			// AT+WIPBR=2,6,1,"password"
	char cmd_ftp[100];
	struct t_gsm_ftp *p_dt;
	p_dt = (char *) ALMT_GSM_FTP;
	
	sprintf(cmd_ftp, "AT+WIPBR=2,6,1,\"%s\"\r\n", p_dt->gprs_passwd);	//printf("cmd: %s", cmd_ftp);
	serX_putstring(PAKAI_GSM_FTP, cmd_ftp);
	baca_serial(cmd_ftp, 50, 20);
	
	#ifdef DEBUG_FTP
	printf("cmd: %s\r\n", cmd_ftp);
	#endif
	
	if (strncmp(cmd_ftp,"AT+WIPBR",8)==0) {
		baca_serial(cmd_ftp, 50, 20);
		#ifdef DEBUG_FTP
		printf("respon1: %s\r\n", cmd_ftp);
		#endif
	}
	
	if (strncmp(cmd_ftp,"OK",2)==0) {
		strcpy(cmd_ftp,"");
		baca_serial(cmd_ftp, 50, 10);
		#ifdef DEBUG_FTP
		printf("%s OK !! %s\r\n", __FUNCTION__, cmd_ftp);
		#endif

		return 0;
	}
	
	if ( (strncmp((char *) cmd_ftp,"ERROR",5)==0) || (strncmp((char *) cmd_ftp,"+CME", 4)==0) ) {
		printf("%s : AT+WIPBR=2,6,1,\"user\" GAGAL\r\n", __FUNCTION__);
		return -1;
	} else {
		printf("%s : AT+WIPBR=2,6,1,\"user\" UNKNOWN %s\r\n", __FUNCTION__, cmd_ftp);
		
		strcpy(cmd_ftp, "");
		baca_serial(cmd_ftp, 20, 10);
		#ifdef DEBUG_FTP
		printf("respon1: %s\r\n", cmd_ftp);
		#endif
		return -1;
	}
}

int set_gprs_on(void) {
	char cmd_ftp[50];
	struct t_gsm_ftp *p_dt;
	p_dt = (char *) ALMT_GSM_FTP;
	
	sprintf(cmd_ftp, "AT+WIPBR=4,6,0\r\n");
	serX_putstring(PAKAI_GSM_FTP, cmd_ftp);
	baca_serial(cmd_ftp, 20, 200);
	
	#ifdef DEBUG_FTP
	printf("cmd: %s\r\n", cmd_ftp);
	#endif
	
	if (strncmp(cmd_ftp,"AT+WIPBR",8)==0) {
		baca_serial(cmd_ftp, 20, 200);
		#ifdef DEBUG_FTP
		printf("respon1: %s\r\n", cmd_ftp);
		#endif
	}
	
	if (strncmp(cmd_ftp,"OK",2)==0) {
		strcpy(cmd_ftp,"");
		baca_serial(cmd_ftp, 50, 10);
		#ifdef DEBUG_FTP
		printf("%s OK !! %s\r\n", __FUNCTION__, cmd_ftp);
		#endif

		return 0;
	}
	if ( (strncmp((char *) cmd_ftp,"ERROR",5)==0) || (strncmp((char *) cmd_ftp,"+CME", 4)==0) ) {
		printf("%s : AT+WIPBR=4,6,0 GAGAL\r\n", __FUNCTION__);
	} else {
		printf("%s : AT+WIPBR=4,6,0 UNKNOWN %s\r\n", __FUNCTION__, cmd_ftp);
		
		strcpy(cmd_ftp, "");
		baca_serial(cmd_ftp, 20, 10);
		#ifdef DEBUG_FTP
		printf("respon1: %s\r\n", cmd_ftp);
		#endif
	}
	return -1;
}

int set_gprs_off(void) {
	char cmd_ftp[50];
	
	sprintf(cmd_ftp, "AT+WIPBR=5,6\r\n");
	serX_putstring(PAKAI_GSM_FTP, cmd_ftp);
	baca_serial(cmd_ftp, 20, 100);
	
	#ifdef DEBUG_FTP
	printf("cmd: %s\r\n", cmd_ftp);
	#endif
	
	if (strncmp(cmd_ftp,"AT+WIPBR",8)==0) {
		baca_serial(cmd_ftp, 20, 100);
		#ifdef DEBUG_FTP
		printf("respon1: %s\r\n", cmd_ftp);
		#endif
	}
	
	if (strncmp(cmd_ftp,"OK",2)==0) {
		strcpy(cmd_ftp,"");
		baca_serial(cmd_ftp, 20, 10);
		#ifdef DEBUG_FTP
		printf("%s OK !! %s\r\n", __FUNCTION__, cmd_ftp);
		#endif
		return 0;
	}
	
	if ( (strncmp((char *) cmd_ftp,"ERROR",5)==0) || (strncmp((char *) cmd_ftp,"+CME", 4)==0) ) {
		printf("%s : AT+WIPBR=5,6 GAGAL\r\n", __FUNCTION__);
	} else {
		printf("%s : AT+WIPBR=5,6 UNKNOWN %s\r\n", __FUNCTION__, cmd_ftp);
		
		strcpy(cmd_ftp, "");
		baca_serial(cmd_ftp, 20, 10);
		#ifdef DEBUG_FTP
		printf("respon1: %s\r\n", cmd_ftp);
		#endif
	}
	return -1;
}

int set_ftp_on(void) {
	char cmd_ftp[100];
	struct t_gsm_ftp *p_dt;
	p_dt = (char *) ALMT_GSM_FTP;
	
	sprintf(cmd_ftp, "AT+WIPCREATE=4,1,\"%s\",%d,\"%s\",\"%s\"\r\n", \
		p_dt->ftp_server, p_dt->ftp_port, p_dt->ftp_user, p_dt->ftp_passwd);
	//printf("cmd: %s", cmd_ftp);
	
	serX_putstring(PAKAI_GSM_FTP, cmd_ftp);
	baca_serial(cmd_ftp, 100, 1000);
	
	#ifdef DEBUG_FTP
	printf("cmd: %s\r\n", cmd_ftp);
	#endif
	
	if (strncmp(cmd_ftp,"AT+WIPCREATE",12)==0) {
		baca_serial(cmd_ftp, 100, 1000);
		#ifdef DEBUG_FTP
		printf("respon1: %s\r\n", cmd_ftp);
		#endif
	}
	
	if (strncmp(cmd_ftp,"OK",2)==0) {
		#ifdef DEBUG_FTP
		printf("%s OK !! %s\r\n", __FUNCTION__, cmd_ftp);
		#endif

		return 0;
	}
	if ( (strncmp((char *) cmd_ftp,"ERROR",5)==0) || (strncmp((char *) cmd_ftp,"+CME", 4)==0) ) {
		printf("%s : AT+WIPCREATE=4,1,\"ftp\",\"port\",\"user\",\"pwd\" GAGAL\r\n", __FUNCTION__);
		return -1;
	} else {
		printf("%s : AT+WIPCREATE=4,1,\"ftp\",\"port\",\"user\",\"pwd\" UNKNOWN %s\r\n", __FUNCTION__, cmd_ftp);
		
		strcpy(cmd_ftp, "");
		baca_serial(cmd_ftp, 80, 10);
		#ifdef DEBUG_FTP
		printf("respon1: %s\r\n", cmd_ftp);
		#endif
		return -1;
	}
	return -1;
}

int set_ftp_off(void) {
	char cmd_ftp[50];
	
	sprintf(cmd_ftp, "AT+WIPCLOSE=4,1\r\n");
	serX_putstring(PAKAI_GSM_FTP, cmd_ftp);
	baca_serial(cmd_ftp, 20, 100);
	
	#ifdef DEBUG_FTP
	printf("cmd: %s\r\n", cmd_ftp);
	#endif
	
	if (strncmp(cmd_ftp,"AT+WIPCLOSE",11)==0) {
		baca_serial(cmd_ftp, 20, 100);
		#ifdef DEBUG_FTP
		printf("respon1: %s\r\n", cmd_ftp);
		#endif
	}
	
	if (strncmp(cmd_ftp,"OK",2)==0) {
		strcpy(cmd_ftp,"");
		baca_serial(cmd_ftp, 20, 10);
		#ifdef DEBUG_FTP
		printf("%s OK !! %s\r\n", __FUNCTION__, cmd_ftp);
		#endif
		return 0;
	}
	
	if ( (strncmp((char *) cmd_ftp,"ERROR",5)==0) || (strncmp((char *) cmd_ftp,"+CME", 4)==0) ) {
		printf("%s : AT+WIPBR=5,6 GAGAL\r\n", __FUNCTION__);
	} else {
		printf("%s : AT+WIPBR=5,6 UNKNOWN %s\r\n", __FUNCTION__, cmd_ftp);
		
		strcpy(cmd_ftp, "");
		baca_serial(cmd_ftp, 20, 10);
		#ifdef DEBUG_FTP
		printf("respon1: %s\r\n", cmd_ftp);
		#endif
	}
	return -1;
}

int upload_file(char *namafile) {
	char cmd_ftp[100];
	char rspn_ftp[100];
	int i=0;
	
	sprintf(cmd_ftp, "AT+WIPFILE=4,1,2,\"%s\"\r\n", namafile);		printf("cmd: %s", cmd_ftp);
	serX_putstring(PAKAI_GSM_FTP, cmd_ftp);
	
	baca_serial(cmd_ftp, 100, 500);
	vTaskDelay(100);
	//*
	if (strncmp(cmd_ftp,"AT+WIPFILE",10)==0) {
		baca_serial(cmd_ftp, 100, 500);
	}
	
	while(strncmp(cmd_ftp,"CONNECT",7)!=0) {
		strcpy(cmd_ftp, "");
		baca_serial(cmd_ftp, 100, 500);
		if (strlen(cmd_ftp)>0)	{
			#ifdef DEBUG_FTP
			printf("%d. respon1: %s\r\n", i+1, cmd_ftp);
			#endif
			break;
		}
		vTaskDelay(50);
		i++;
		if (i>10)	break;
	}
	//*/
	/*
	while(strncmp(cmd_ftp,"CONNECT",7)!=0) {
		strcpy(cmd_ftp, "");
		baca_serial(cmd_ftp, 100, 500);
		if (strlen(cmd_ftp)>0)	{
			#ifdef DEBUG_FTP
			printf("%d.respon2: %s\r\n", i+1, cmd_ftp);
			#endif
			break;
		}
		i++;
		if (i>10)	break;
	}	

	if (strncmp(cmd_ftp,"AT+WIPFILE",11)==0) {
		while(1) {
			strcpy(cmd_ftp, "");
			baca_serial(cmd_ftp, 100, 1000);
			if (strlen(cmd_ftp)>0)	{
				#ifdef DEBUG_FTP
				printf("respon3: %s\r\n", cmd_ftp);
				#endif
				break;
			}
			i++;
			if (i>10)	break;
		}
	}
	//*/
	if (strncmp(cmd_ftp,"CONNECT",7)==0) {
		strcpy(cmd_ftp,"");
		baca_serial(cmd_ftp, 100, 100);
		#ifdef DEBUG_FTP
		printf("%s OK !! %s\r\n", __FUNCTION__, cmd_ftp);
		#endif
		return 0;
	}
	
	
	if ( (strncmp((char *) cmd_ftp,"ERROR",5)==0) || (strncmp((char *) cmd_ftp,"+CME", 4)==0) ) {
		printf("%s : AT+WIPFILE=4,1,2,\"namafile\" GAGAL\r\n", __FUNCTION__);
		return 11;
	} else {
		printf("%s : AT+WIPFILE=4,1,2,\"namafile\" UNKNOWN %s\r\n", __FUNCTION__, cmd_ftp);
		strcpy(cmd_ftp, "");
		baca_serial(cmd_ftp, 100, 100);
		#ifdef DEBUG_FTP
		printf("respon1: %s\r\n", cmd_ftp);
		#endif
		return -1;
	}
	//
}

int kirim_file_ke_ftp(char *abs_path, char *nf) {
	char posisifile[128];
	char namafile[32];
	
	int res, flag=1, oz=0, rspn=1;
	unsigned long int size, i,uf;
	FIL fd2;
	time_t timeval;
	
	strcpy(posisifile, abs_path);
	strcpy(namafile, nf);
	
	#ifdef DEBUG_FTP
	printf("___path: %s, nama: %s\r\n", posisifile, namafile);
	#endif
	if (res = f_open(&fd2, abs_path, FA_READ | FA_WRITE)) {
		printf("%s(): Buka file error %d !\r\n", __FUNCTION__, res);					
		return 0;
	}
	
	f_lseek( &fd2, fd2.fsize - 6 );
	f_read( &fd2, abs_path, 6, &res);
	
	#ifdef DEBUG_FTP
	//printf("___mo dikirim, CEK %s @@@, nf: %s\r\n", posisifile, namafile);
	#endif
	
	files++;
	if (strncmp( abs_path, "SENDED", 6) == 0)  {
		#ifdef DEBUG_FTP
		printf("file %s sudah dikirim !\r\n", posisifile);
		#endif
		file_sudah++;
	} else	{
		f_lseek( &fd2, 0);				// kembalikan pointer //
		flag=55;

		oz=0;
		while(1) {
			rspn = upload_file(namafile);
			#ifdef DEBUG_FTP
			//printf("respon upload : %d, oz: %d, flag: %d\r\n", rspn, oz, flag);
			#endif
			if (rspn==0)	{
				flag = 77;	
				#ifdef DEBUG_FTP
				printf("_____KELUAR loop, flag: %d\r\n", flag);
				#endif
				break;
			}
			vTaskDelay(10);
			//cek_awal();
			oz++;
			if (oz>20)	break;
		}

		if (flag==77)		{
			size = sizeof (abs_path);
			
			#ifdef DEBUG_FTP
			//printf("Sudah konek !!!...........Kirim data size: %d, res: %d!!!\r\n", size, res);
			#endif

			for (;;)	{
				f_read( &fd2, abs_path, size, &res);
				//printf("res: %d\r\n");
				portENTER_CRITICAL();
				for (i=0; i<res; i++)		{								
					//tulis_char( abs_path[i] );
					serX_putchar(PAKAI_GSM_FTP, &abs_path[i], 1000);
				}	
				portEXIT_CRITICAL();
				
				if ( res < size ) break; 
			}
			//printf("kirim data: %d byte\r\n", i);
			// untuk mengakhiri data ftp //
			vTaskDelay(100);
			oz=0;
			while(1) {
				if (oz>50)	{
					status_modem = 0;
					#ifdef PAKAI_SELENOID
					printf("__RESTART MODEM__");
					unset_selenoid(1);		// mati relay 1
					vTaskDelay(500);
					set_selenoid(1);		// hidup relay 1
					#endif
					break;
				}
				
				oz++;
				uf = send_etx(oz);
				if (uf == 0)	{
					flag = 88;
					break;
				}
				/*
				if (uf==11) {
					flag = 77;
					break;
				}
				//*/
				vTaskDelay(500);
				//cek_awal();
			}
			
			
			if (flag==77) {
				f_close( &fd2 );
				printf("Upload GAGAL parsial !!! tanpa ETX\r\n");
				oz=0;
				
				return 90;
			}		
			// tulis SENDED pada akhir file //
			sprintf(abs_path, "%s", ctime( &timeval ));	
			sprintf( &abs_path[24], "SENDED");	
			#ifdef DEBUG_FTP
			printf("TULIS %s \r\n", abs_path);
			#endif						
			f_write( &fd2, abs_path, strlen(abs_path), &res);
			file_sukses++;
			printf("FILE %s terkirim ke FTP\r\n", posisifile);
		} else	{
			printf("Upload %s file ERROR !\r\n", namafile);
			//break;
		}
	}
	f_close( &fd2 );
	vTaskDelay(100);		// kasih waktu buat proses yg lain.	
}

int send_etx(int a) {
	int i=0;
	char cmd_ftp[50];
	char ch = 0x03, *p;
	p = &ch;
	serX_putchar(PAKAI_GSM_FTP, p, 1000);	
	baca_serial(cmd_ftp, 20, 10);
	
	printf("cmd: %s, %s %d\r\n", cmd_ftp, __FUNCTION__, a);
	while(strncmp(cmd_ftp,"OK",2)!=0) {
		strcpy(cmd_ftp, "");
		baca_serial(cmd_ftp, 20, 10);
		if (strlen(cmd_ftp)>0)	{
			//#ifdef DEBUG_FTP
			printf("%d. respon: %s\r\n", i+1, cmd_ftp);
			//#endif
			break;
		}
		i++;
		if (i>10)	break;
	}
	
	
	#ifdef DEBUG_FTP
	printf("cmd_ftp: %s\r\n", cmd_ftp);
	#endif
	if (strncmp(cmd_ftp, "OK", 2) == 0) 	{
		printf(" %s(): OK\r\n", __FUNCTION__);
		return 0;
	} 	else	{
		printf(cmd_ftp);
		baca_serial(cmd_ftp, 20, 10);
		printf(" %s(): ERR ??, %s\r\n", __FUNCTION__, cmd_ftp);
		if (strncmp(cmd_ftp, "OK", 2) == 0)
			return 0;
		else
			return 11;
	}
}

#endif
#endif
