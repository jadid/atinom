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
	fff = koneksi_on();

	//fff=90;
	if (fff!=90) {
		printf("Koneksi GPRS gagal !!!\r\n");
		koneksi_off(fff);
		status_modem = 0;
		return 0;
	}
	
	files=0;	file_sudah=0;	file_sukses=0;
	
	vTaskDelay(50);
	cek_awal();
	
	vTaskDelay(50);
	//cari_berkas("J-3", "kirim_ftp");
	
	koneksi_off(fff);
	status_modem = 0;
	return 1;
}

void cek_ftp() {
	int fff=0;
	if (status_modem==1)	{		// modem sibuk
		printf("___modem sibuk___\r\n");
		return;		
	}
	
	status_modem = 1;
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
	int flag=-1;
	
	#ifdef DEBUG_FTP
	printf("________cek_awal dulu________\r\n");
	#endif
	cek_awal();
	
	vTaskDelay(300);
	flag = 20;
	#ifdef DEBUG_FTP
	printf("________set_wipcfg_on________\r\n");
	#endif
	if (set_wipcfg_on()==0)	{
		flag = 1;
	}
	
	vTaskDelay(1000);
	flag = 30;
	#ifdef DEBUG_FTP
	printf("________set_wipbr________\r\n");
	#endif
	if (set_wipbr()==0)	{
		flag = 1;
	}
	
	vTaskDelay(100);
	flag = 40;
	#ifdef DEBUG_FTP
	printf("________set_wipbr_apn________\r\n");
	#endif
	if (set_wipbr_apn()==0)	{
		flag = 1;
	}
	
	vTaskDelay(100);
	flag = 50;
	#ifdef DEBUG_FTP
	printf("________set_wipbr_user________\r\n");
	#endif
	if (set_wipbr_user()==0)	{
		flag = 1;
	}
	
	vTaskDelay(100);
	flag = 60;
	#ifdef DEBUG_FTP
	printf("________set_wipbr_password________\r\n");
	#endif
	if (set_wipbr_passwd()==0) {
		flag = 1;	
	}
	
	flag = 70;
	vTaskDelay(100);
	#ifdef DEBUG_FTP
	printf("________KONEK GPRS________\r\n");
	#endif
	if (set_gprs_on()==0)	{
		flag = 1;	
	}
	
	vTaskDelay(100);
	flag = 80;
	#ifdef DEBUG_FTP
	printf("________KONEK FTP________\r\n");
	#endif
	if (set_ftp_on()==0) {
		flag = 90;
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
	char cmd_ftp[20];
	
	sprintf(cmd_ftp, "AT\r\n");
	serX_putstring(PAKAI_GSM_FTP, cmd_ftp);
	baca_serial(cmd_ftp, 10, 10);
	
	#ifdef DEBUG_FTP
	printf("respon: %s\r\n", cmd_ftp);
	#endif
	if (strncmp(cmd_ftp,"AT",2)==0) {
		baca_serial(cmd_ftp, 10, 10);
		#ifdef DEBUG_FTP
		printf("respon1: %s\r\n", cmd_ftp);
		#endif
	}
	
	if (strncmp(cmd_ftp,"OK",2)==0) {
		#ifdef DEBUG_FTP
		printf("%s OK !!\r\n", __FUNCTION__);
		#endif
		return 0;
	}
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
	return 10;
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
		
	} else {
		printf("%s : AT+WIPBR=1,6 UNKNOWN %s\r\n", __FUNCTION__, cmd_ftp);
		
		strcpy(cmd_ftp, "");
		baca_serial(cmd_ftp, 20, 10);
		#ifdef DEBUG_FTP
		printf("respon1: %s\r\n", cmd_ftp);
		#endif
	}
	return -1;
}

int set_wipbr_apn(void) {		// AT+WIPBR=2,6,11,"internet"
	char cmd_ftp[100];
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
		baca_serial(cmd_ftp, 50, 10);
		#ifdef DEBUG_FTP
		printf("%s OK !! %s\r\n", __FUNCTION__, cmd_ftp);
		#endif
		return 0;
	}
	
	if ( (strncmp((char *) cmd_ftp,"ERROR",5)==0) || (strncmp((char *) cmd_ftp,"+CME", 4)==0) ) {
		printf("%s : AT+WIPBR=2,6,11,\"apn\" GAGAL\r\n", __FUNCTION__);
		
	} else {
		printf("%s : AT+WIPBR=2,6,11,\"apn\" UNKNOWN %s\r\n", __FUNCTION__, cmd_ftp);
		
		strcpy(cmd_ftp, "");
		baca_serial(cmd_ftp, 20, 10);
		#ifdef DEBUG_FTP
		printf("respon1: %s\r\n", cmd_ftp);
		#endif
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
		
	} else {
		printf("%s : AT+WIPBR=2,6,0,\"user\" UNKNOWN %s\r\n", __FUNCTION__, cmd_ftp);
		
		strcpy(cmd_ftp, "");
		baca_serial(cmd_ftp, 20, 10);
		#ifdef DEBUG_FTP
		printf("respon1: %s\r\n", cmd_ftp);
		#endif
	}
}

int set_wipbr_passwd(void) {
	char cmd_ftp[100];
	struct t_gsm_ftp *p_dt;
	p_dt = (char *) ALMT_GSM_FTP;
	
	sprintf(cmd_ftp, "AT+WIPBR=2,6,1,\"%s\"\r\n", p_dt->gprs_passwd);	//printf("cmd: %s", cmd_ftp);
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
}

int set_gprs_on(void) {
	char cmd_ftp[50];
	struct t_gsm_ftp *p_dt;
	p_dt = (char *) ALMT_GSM_FTP;
	
	sprintf(cmd_ftp, "AT+WIPBR=4,6,0\r\n");
	serX_putstring(PAKAI_GSM_FTP, cmd_ftp);
	baca_serial(cmd_ftp, 20, 100);
	
	#ifdef DEBUG_FTP
	printf("cmd: %s\r\n", cmd_ftp);
	#endif
	
	if (strncmp(cmd_ftp,"AT+WIPBR",8)==0) {
		baca_serial(cmd_ftp, 50, 100);
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
	baca_serial(cmd_ftp, 100, 100);
	
	#ifdef DEBUG_FTP
	printf("cmd: %s\r\n", cmd_ftp);
	#endif
	
	if (strncmp(cmd_ftp,"AT+WIPCREATE",12)==0) {
		baca_serial(cmd_ftp, 100, 100);
		#ifdef DEBUG_FTP
		printf("respon1: %s\r\n", cmd_ftp);
		#endif
	}
	
	if (strncmp(cmd_ftp,"OK",2)==0) {
		strcpy(cmd_ftp,"");
		baca_serial(cmd_ftp, 100, 100);
		#ifdef DEBUG_FTP
		printf("%s OK !! %s\r\n", __FUNCTION__, cmd_ftp);
		#endif

		return 0;
	}
	if ( (strncmp((char *) cmd_ftp,"ERROR",5)==0) || (strncmp((char *) cmd_ftp,"+CME", 4)==0) ) {
		printf("%s : AT+WIPCREATE=4,1,\"ftp\",\"port\",\"user\",\"pwd\" GAGAL\r\n", __FUNCTION__);
	} else {
		printf("%s : AT+WIPCREATE=4,1,\"ftp\",\"port\",\"user\",\"pwd\" UNKNOWN %s\r\n", __FUNCTION__, cmd_ftp);
		
		strcpy(cmd_ftp, "");
		baca_serial(cmd_ftp, 80, 10);
		#ifdef DEBUG_FTP
		printf("respon1: %s\r\n", cmd_ftp);
		#endif
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

#endif
#endif
