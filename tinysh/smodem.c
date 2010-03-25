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

#include "../gsm/system_ftp.c";

int fd = 0;
char buf[128];


//#define read(a, b, c) serX_getchar(PAKAI_GSM_FTP, a, b, 1000);	\ 
//				tendang_wdog();

#define read(a, b, c) ser2_getchar(a, b, 1000);	\
		tendang_wdog();
	
void cetak_tulisan(char *isi) {
	printf("%s", isi);
}

void baca_hasil() {
	baca_serial(buf, 12, 10);
	printf("%s\r\n",buf);
}


int baca_serial(char *buf, int len, int timeout)
{
	int res;
	int c;
	int tout=0;
	int masuk = 0;
	
	while(len--)
	{
		res = read( fd, &c, 1);
		if (res != 0)
		{
			if ( (char) c == 0x0A || (char) c == 0x0D )
			{
				if (masuk > 0) 
				{	
					/* supaya buffer lama tidak ikut di printout */
					buf[masuk] = 0;
					return 0;
				}
			}
			else
			{
				buf[ masuk ] = (char ) c;								
				masuk++;
				
				#if (DEBUG == 1)
				printf(" %s(): res=%d : msk=%02d : 0x%02X : %c\r\n", __FUNCTION__, res, masuk, (char) c, (char) c);
				#endif
				/*
				if ( (char) c == 0x0A && masuk > 2)
				{
					if (buf[ masuk - 2 ] == 0x0D)
					return 0;
				}*/
			}
		}
		else
		{
			printf(" %s(): %d :timeout\r\n", __FUNCTION__, tout);
			len++;
			tout++;
			if ( tout > timeout) 
				return 0;
				//return -1;
		}
	}
}
