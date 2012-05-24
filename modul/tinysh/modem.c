/*
	10 Maret 2010
	furkan jadid, daun biru engineering
	
	setting file untuk kontrol modem
	
	perintah2 :
	------------------------------
	
	FTP :
	set_modem_ftp default
	set_modem_ftp set		: program ftp dijalankan
	set_modem_ftp unset		: program ftp tidak dijalankan
	set_modem_ftp period 10	: periode untuk melakukan ftp (menit)
	
	set_modem_ftp user		nama_user
	set_modem_ftp passwd	isi_passwd
	set_modem_ftp server	www.daunbiru.com
	set_modem_ftp direktori data_angin
	
	cek_modem_ftp
	
	GSM :
	set_modem_gsm set
	set_modem_gsm unset
	
*/


#include "../../app/monita/monita_uip.h"
//#include "../gsm/system_ftp.c"
//#include "../gsm/kirim_ftp.c"

//int fd = 0;
//char serGSM[128];
	

//#define read(a, b, c) serX_getchar(PAKAI_GSM_FTP, a, b, 1000);	\ 
//				tendang_wdog();

#define read(a, b, c) ser2_getchar(a, b, 1000);	\
		tendang_wdog();




void baca_hasil() {
	char serGSM[24];
	int aaa;
	
	baca_serial(serGSM, 50, 20);
	printf(" MODEM: %s : %d\r\n",serGSM, strlen(serGSM));
		for (aaa=0; aaa<strlen(serGSM); aaa++)
			printf("%x ", serGSM[aaa]);
		printf("\r\n");
	//if (strncmp(serGSM,"AT",2)==0) {
		baca_serial(serGSM, 20, 5);
		printf(" MODEM: %s : %d\r\n",serGSM, strlen(serGSM));
		for (aaa=0; aaa<strlen(serGSM); aaa++)
			printf("%x ", serGSM[aaa]);
		printf("\r\n");
	//}
}

/*
int baca_serial(char *string_modem, int len, int timeout)
{
	int res;
	char c;
	int tout=0;
	int masuk = 0;	

	while(len--)	{
		//res = read( fd, &c, 1);
		res = ser2_getchar(1, &c, 100);
		if (res != 0)	{
			if ( (char) c == 0x0A || (char) c == 0x0D )		{
				if (masuk > 0) 	{	
					string_modem[masuk] = 0;
					return 0;
				}
			}
			else	{
				string_modem[ masuk ] = (char ) c;								
				masuk++;
				
				#if (DEBUG == 1)
				printf(" %s(): res=%d : msk=%02d : 0x%02X : %c\r\n", __FUNCTION__, res, masuk, (char) c, (char) c);
				#endif
			}
		}
		else		{
			//printf(" %s(): %d :timeout\r\n", __FUNCTION__, tout);
			len++;
			tout++;
			if ( tout > timeout) 
				return 0;
				//return -1;
		}
	}
}
//*/
