/*
	setenv ipaddr
	12 Des 2008
	furkan jadid
	 
	diStandardkan cara penyimpan data 
	Sekali set langsung tersimpan ke ROM
	1 Okt 2010 -- Afrendy Bayu
	
	*/

#include <stdio.h>
#include "enviro.h"

#define pengali	10

//extern struct t_env env2;

void display_args(int argc, char **argv);
unsigned int baca_ip(char *ipne);
unsigned char dec_to_bin(char CC);
int baca_kanal(char *c);


unsigned char char_to_bin(unsigned char x)
{
	return (x - '0');	
}

static void setenv_fnt(int argc, char **argv)
{
  	if (argc < 3)	 
	{
		if (argc > 1)
		{
			if (strcmp(argv[1], "help") == 0)
			{
				printf(" Setting environment utama untuk board / modul\r\n");
				garis_bawah();
				printf(" argument : help, default, ipaddr, gateway, nama\r\n");
				printf("   help   : printout keterangan ini\r\n");
				printf("   default : load default environment setting dari daun biru\r\n");
				printf(" \r\n");
				printf("   ipaddr : memberikan alamat IP\r\n");
				printf("     misalnya $ set_env ipaddr 192.168.1.124\r\n");
				printf(" \r\n");
				printf("   gateway : memberikan alamat IP gateway\r\n");
				printf("     misalnya $ set_env gateway 192.168.1.13\r\n");
				printf(" \r\n");
				printf("   nama : memberikan nama pada board\r\n");
				printf("     misalnya $ set_env nama BOARD_RTD_MAK_#3\r\n");
				printf(" \r\n");
				#ifdef PAKAI_WEBCLIENT
				printf("   webclient : aktivasi pengiriman data via webclient\r\n");
				printf("     misalnya $ set_env webclient [0|mati|1|aktif]\r\n");
				printf(" \r\n");
				printf("   file : tujuan file pengiriman data via webclient\r\n");
				printf("     misalnya $ set_env file /monita3/monita_loket.php\r\n");
				printf(" \r\n");
				printf("   burst : metode pengiriman data via webclient\r\n");
				printf("     mode burst sebaiknya digunakan jika mengirim beberapa data titik ukur\r\n");
				printf("	 dari beberapa sumber modul. Jml total data terkirim <20 titik.");
				printf("     misalnya $ set_env burst [0|mati|1|aktif]\r\n");
				printf(" \r\n");
				#endif
				return;
			} 
			else if (strcmp(argv[1], "default") == 0)			{
				printf(" set environment dengan data default !\n");
				//set_default_ip();
				//set_dafault_env_lain();
				set_env_default();
				return;
			}
			else if (strcmp(argv[1], "reset") == 0)			{
				printf(" set environment dengan data default !\n");
				set_env_default();
				vTaskDelay(2);

				#ifdef BANYAK_SUMBER
					#ifdef CENDOL
					set_awal_konfig();
					#endif
					
					set_awal_sumber();
					set_data_default();
				#endif
				
				#ifdef PAKAI_FILE_SIMPAN
					//set_file_default();
				#endif
				
				return;
			}	
		}
		printf(" ERR: argument kurang !\r\n");
		printf(" coba set_env help \r\n");
		
		return;	
	}
	
	printf(" setenv %s dipanggil\r\n", argv[1]);
	garis_bawah();
  	display_args(argc,argv);
	
	struct t_env *p_sbr;
	p_sbr = pvPortMalloc( sizeof (struct t_env) );
	memcpy((char *) p_sbr, (char *) ALMT_ENV, (sizeof (struct t_env)));
	
	if (p_sbr == NULL) {
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		return -1;
	}
	
	unsigned char str_ip_iddr[24];
	unsigned int ret_ip;
	
  	if (strcmp(argv[1], "ipaddr") == 0)  	{
		printf(" set IP address ");  

		sprintf(str_ip_iddr, "%s", argv[2]);
		
		ret_ip = baca_ip(str_ip_iddr);
		
		p_sbr->IP0 = (unsigned char)(ret_ip >> 24);
		p_sbr->IP1 = (unsigned char)(ret_ip >> 16);
		p_sbr->IP2 = (unsigned char)(ret_ip >> 8);
		p_sbr->IP3 = (unsigned char)(ret_ip);
		
		printf("%d.%d.%d.%d\r\n", p_sbr->IP0, p_sbr->IP1, p_sbr->IP2, p_sbr->IP3);
	}
	else if (strcmp(argv[1], "gateway") == 0)  	{
		printf(" set GATEWAY address ");  

		sprintf(str_ip_iddr, "%s", argv[2]);
		
		ret_ip = baca_ip(str_ip_iddr);
		
		p_sbr->GW0 = (unsigned char)(ret_ip >> 24);
		p_sbr->GW1 = (unsigned char)(ret_ip >> 16);
		p_sbr->GW2 = (unsigned char)(ret_ip >> 8);
		p_sbr->GW3 = (unsigned char)(ret_ip);
		
		printf("%d.%d.%d.%d\r\n", p_sbr->GW0, p_sbr->GW1, p_sbr->GW2, p_sbr->GW3);
	}
	else if (strcmp(argv[1], "nama") == 0)	{
		printf(" set nama_board\r\n");
		//memset(env2.nama_board, 0, sizeof (env2.nama_board));
		
		if (strlen(argv[2]) > sizeof (p_sbr->nama_board))		{
			printf("Nama board terlalu panjang !");
			vPortFree( p_sbr );
			return;	
		}
		else	{
			sprintf(p_sbr->nama_board, "%s", argv[2]);
			printf(" Nama board : %s\n", p_sbr->nama_board);
		}
	}	
	else if (strcmp(argv[1], "SN") == 0)	{
		printf(" set no seri \r\n");
		//memset(env2.SN, 0, sizeof (env2.SN));
		if (strlen(argv[2]) > sizeof (p_sbr->SN))
		{
			printf("SN terlalu panjang !");
			vPortFree( p_sbr );
			return;	
		}
		else
		{
			sprintf(p_sbr->SN, "%s", argv[2]);
			printf(" SN : %s\n", p_sbr->SN);
		}
	}
	#ifdef PAKAI_WEBCLIENT
	else if (strcmp(argv[1], "file") == 0)	{
		printf(" set file");
		//memset(env2.SN, 0, sizeof (env2.SN));
		if (strlen(argv[2]) > sizeof (p_sbr->berkas)) 	{
			printf(", Nama terlalu panjang !");
			vPortFree( p_sbr );
			return;	
		}
		else {
			sprintf(p_sbr->berkas, "%s", argv[2]);
			printf(", Nama : %s\n", p_sbr->berkas);
		}
	}
	else if (strcmp(argv[1], "webclient") == 0)	{
		printf(" set status webclient\r\n");
  		if (( argv[2][0] == '1') || (argv[2][0] == '0')) {
			p_sbr->statusWebClient = (argv[2][0] - '0');
		} 
		else if (strcmp(argv[2], "aktif")==0 || strcmp(argv[2], "hidup")==0) {
			p_sbr->statusWebClient = 1;
		} else if (strcmp(argv[2], "mati")==0)  {
			p_sbr->statusWebClient = 0;
		} else {
			p_sbr->statusWebClient = 0;
		}
		printf(" Status webclient : %s\r\n", (p_sbr->statusWebClient==1)?"aktif":"mati");
	}
	else if (strcmp(argv[1], "burst") == 0)	{
		printf(" set status burst\r\n");
  		if (( argv[2][0] == '1') || (argv[2][0] == '0')) {
			p_sbr->burst = (argv[2][0] - '0');
		} 
		else if (strcmp(argv[2], "aktif")==0 || strcmp(argv[2], "hidup")==0) {
			p_sbr->burst = 1;
		} else if (strcmp(argv[2], "mati")==0)  {
			p_sbr->burst = 0;
		} else {
			p_sbr->burst = 0;
		}
		printf(" Status burst : %s\r\n", (p_sbr->burst==1)?"aktif":"mati");
	}
	#endif
	else if (strcmp(argv[1], "kanal") == 0)	{
		int kanal;
		
		printf(" set param kanal \r\n");

		sprintf(str_ip_iddr, "%s", argv[2]);
		
		kanal = baca_kanal(str_ip_iddr);
		
		if (kanal > 0 && kanal < 21)
		{
			printf(" kanal terbaca = %d\r\n", kanal);
			
			if (strcmp(argv[3], "ket") == 0)	{
				printf(" keterangan kanal :");
				if (strlen(argv[4]) > p_sbr->kalib[kanal-1].ket)
				{
					printf(" terlalu panjang\n");
					vPortFree( p_sbr );
					return;	
				}
				sprintf(p_sbr->kalib[kanal-1].ket, "%s", argv[4]);
				printf(" %s\n", p_sbr->kalib[kanal-1].ket); 	
			}
		}
		else if (kanal == 0 || kanal > 20) {
			vPortFree( p_sbr );
			printf(" kanal %d diluar range !\r\n");
		} else {
			vPortFree( p_sbr );
			printf(" format kanal salah !\r\n");
		}
	}
	
	if (simpan_env( p_sbr ) < 0) {
		vPortFree( p_sbr );
		//return -1;
		return;
	}
	vPortFree( p_sbr );
		
	return;
  
}

static tinysh_cmd_t setenv_cmd={0,"set_env","setting environment utama", \
	"ipaddr gateway nama kanal default help", setenv_fnt,0,0,0};

unsigned int baca_ip(char *ipne)
{
	unsigned char bin=0;
	unsigned int hasil;
	int i;
	
	hasil = 0x0000;
	
	while(*ipne)
	{
		if (((*ipne > '9') || (*ipne < '0')) && (*ipne != '.'))
		{
			//printf("\n Error format ip = %c", *ipne);
			//i = 0;
		}
		else
		{
			if (*ipne == '.')
			{
				if (hasil == 0x0000) 
				{
					hasil = (unsigned int)bin;
					//printf(" k ");
				}
				else
					hasil = (unsigned int)((hasil  << 8) + bin);
				
				bin = 0;
				//serial_puts("\n tik");
			}
			else
			{
				bin = (bin * pengali) + dec_to_bin((char) *ipne);	
				//printf("%d ", (unsigned char)bin);
			}	
		}
		ipne++;
	}
	
	hasil = (unsigned int)((hasil  << 8) + bin);
	
	
	//printf(" Hasil IP = 0x%X = ", hasil);
	//cek hasil
	/*
	for (i = 3; i >= 0; i--)
	{
		printf("%d ", (unsigned char)(hasil >> (8 * i)));
	}
	printf("\n");
	*/
	return ((unsigned int) hasil);	
}

unsigned char dec_to_bin(char CC)
{
	return ((unsigned char) (CC - '0'));
}

int baca_kanal(char *ipne)
{
	int ret = 0;
	
	while(*ipne)
	{
		if ((*ipne > '9') || (*ipne < '0'))
		{
			//printf("\n Error format kanal = %c", *ipne);
			return -1;
			//i = 0;
		}
		else
		{
			ret = (ret * pengali)  + dec_to_bin((char) *ipne);		
		}
		ipne++;
	}
	
	return ret;
}
