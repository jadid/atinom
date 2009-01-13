/*
	setenv ipaddr
	12 Des 2008
	furkan jadid
	*/

#include <stdio.h>
#include "enviro.h"

#define pengali	10

extern struct t_env env2;

void display_args(int argc, char **argv);
unsigned int baca_ip(char *ipne);
unsigned char dec_to_bin(char CC);
int baca_kanal(char *c);

void garis_bawah(void)
{
	printf("---------------------------------\r\n");	
}


unsigned char char_to_bin(unsigned char x)
{
	return (x - '0');	
}

static void setenv_fnt(int argc, char **argv)
{
  	if (argc < 2) 
	{
		printf(" argument kurang !\r\n");
		return;	
	}
	
	printf(" setenv %s dipanggil\r\n", argv[1]);
	garis_bawah();
  	display_args(argc,argv);
	
	unsigned char buf[24];
	unsigned int ret_ip;
	
  	if (strcmp(argv[1], "ipaddr") == 0)
  	{
		printf(" set IP address\r\n");  
		memset(buf, 0, sizeof (buf));
		sprintf(buf, "%s", argv[2]);
		
		ret_ip = baca_ip(buf);
		
		env2.IP0 = (unsigned char)(ret_ip >> 24);
		env2.IP1 = (unsigned char)(ret_ip >> 16);
		env2.IP2 = (unsigned char)(ret_ip >> 8);
		env2.IP3 = (unsigned char)(ret_ip);
		
		printf("%d.", env2.IP0);
		printf("%d.", env2.IP1);
		printf("%d.", env2.IP2);
		printf("%d\r\n", env2.IP3);
	}
	else if (strcmp(argv[1], "gateway") == 0)
  	{
		printf(" set GATEWAY address\r\n");  
		memset(buf, 0, sizeof (buf));
		sprintf(buf, "%s", argv[2]);
		
		ret_ip = baca_ip(buf);
		
		env2.GW0 = (unsigned char)(ret_ip >> 24);
		env2.GW1 = (unsigned char)(ret_ip >> 16);
		env2.GW2 = (unsigned char)(ret_ip >> 8);
		env2.GW3 = (unsigned char)(ret_ip);
		
		printf("%d.", env2.GW0);
		printf("%d.", env2.GW1);
		printf("%d.", env2.GW2);
		printf("%d\n", env2.GW3);
	}
	else if (strcmp(argv[1], "nama") == 0)
	{
		printf(" set nama_board\r\n");
		memset(env2.nama_board, 0, sizeof (env2.nama_board));
		if (strlen(argv[2]) > sizeof (env2.nama_board))
		{
			printf("Nama board terlalu panjang !");
			return;	
		}
		else
		{
			sprintf(env2.nama_board, "%s", argv[2]);
			printf(" Nama board : %s\n", env2.nama_board);
		}
	}
	else if (strcmp(argv[1], "kanal") == 0)
	{
		int kanal;
		
		printf(" set param kanal \r\n");
		memset(buf, 0, sizeof (buf));
		sprintf(buf, "%s", argv[2]);
		
		kanal = baca_kanal(buf);
		
		if (kanal > 0 && kanal < 21)
		{
			printf(" kanal terbaca = %d\r\n", kanal);
			
			if (strcmp(argv[3], "ket") == 0)
			{
				printf(" keterangan kanal :");
				if (strlen(argv[4]) > env2.kalib[kanal-1].ket)
				{
					printf(" terlalu panjang\n");
					return;	
				}
				sprintf(env2.kalib[kanal-1].ket, "%s", argv[4]);
				printf(" %s\n", env2.kalib[kanal-1].ket); 	
			}
		}
		else if (kanal == 0 || kanal > 20)
			printf(" kanal %d diluar range !\r\n");
		else
			printf(" format kanal salah !\r\n");
		
	}
	else
		printf(" perintah setenv %s belum ada !\r\n", argv[1]); 
		
	return;
  
}

static tinysh_cmd_t setenv_cmd={0,"setenv","set ip","[args]",
                              setenv_fnt,0,0,0};

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
	
	
	printf(" Hasil IP = 0x%X = ", hasil);
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
