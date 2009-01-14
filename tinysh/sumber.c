/*
	berkaiatan dengan sumber ip address 
	12 Jan 09
	furkan jadid
	daun biru engineering
	
	*/
	
#include "../monita/monita_uip.h"
extern struct t_sumber sumber[];
int cek_nomer_sumber(char *arg, int maks);

							  
void cek_sumber(void)
{
	int i;
	extern struct t_sumber sumber[];
	
	printf(" Sumber data\r\n");
	garis_bawah();
	
	for (i=0; i<20; i++)
	{
		printf(" (%2d): %10s : ", (i+1), sumber[i].nama);	
		printf("%d.%d.%d.%d : ", sumber[i].IP0, sumber[i].IP1, sumber[i].IP2, sumber[i].IP3);
		
		// cek status
		if (sumber[i].status == 0)
			printf("Null\r\n");
		if (sumber[i].status == 1)
			printf("Normal\r\n");
		if (sumber[i].status == 2)
			printf("TimeOut\r\n");
		
	}
	
}							 

static tinysh_cmd_t cek_sumber_cmd={0,"cek_sumber","--","[args]",
                              cek_sumber,0,0,0};

void set_sumber(int argc, char **argv)
{
	unsigned char buf[24];
	int sumb;
	unsigned int ret_ip;
	
	if (argc < 4) 
	{
		if (argc > 1)
		{
			if (strcmp(argv[1], "help") == 0)
			{
				printf("  ipaddr X aaa.bbb.ccc.ddd\r\n");
				printf("  : setting ip address sumber nomer X dengan ip abcd\r\n");
		
				printf("  nama X gedombreng\r\n");
				printf("  : setting nama sumber nomer X dengan nama gedombreng (maks 10 karakter)\r\n");	
			
				return;
			} 
		}
		printf(" ERR: argument kurang !\r\n");
		printf(" coba set_sumber help \r\n");
		return;	
	}
	
	printf(" set_sumber %s dipanggil\r\n", argv[1]);
	garis_bawah();
  	display_args(argc,argv);
	
	if (strcmp(argv[1], "ipaddr") == 0)
	{
		sprintf(buf, "%s", argv[2]);	
		sumb = cek_nomer_sumber(buf, 20);
		if (sumb > 0)		
		{
			printf(" sumber = %d\r\n", sumb);
			sprintf(buf, "%s", argv[3]);
		
			ret_ip = baca_ip(buf);
			
			sumber[sumb-1].IP0 = (unsigned char) (ret_ip >> 24);
			sumber[sumb-1].IP1 = (unsigned char) (ret_ip >> 16);
			sumber[sumb-1].IP2 = (unsigned char) (ret_ip >> 8);
			sumber[sumb-1].IP3 = (unsigned char) (ret_ip);
			
			printf("IP = %d.%d.%d.%d\r\n", sumber[sumb-1].IP0, sumber[sumb-1].IP1, sumber[sumb-1].IP2, sumber[sumb-1].IP3);
			
		}
		else
			return;
			
	}
	else if (strcmp(argv[1], "nama") == 0)
	{
		sprintf(buf, "%s", argv[2]);	
		sumb = cek_nomer_sumber(buf, 20);
		if (sumb > 0)		
		{
			printf(" sumber = %d : ", sumb);
			
			if (strlen(argv[3]) > 10)
			{
				printf("\n ERR: nama terlalu panjang !\r\n");
				return;
			}
			sprintf(sumber[sumb-1].nama, argv[3]);
			printf(" Nama : %s\r\n", sumber[sumb-1].nama); 
		}
		else return;	
	}
}							 

static tinysh_cmd_t set_sumber_cmd={0,"set_sumber","help ipaddr nama","[args]",
                              set_sumber,0,0,0};

int cek_nomer_sumber(char *arg, int maks)
{
	unsigned char buf[24];
	int ss;
	
	sprintf(buf, "%s", arg);
	ss = baca_kanal(buf);
	
	if (ss > 0 && ss < (maks+1))
	{
		return ss;	
	}	
	else if (ss == 0 || ss > maks)
	{
		printf("\r\n ERR: %d diluar range !\r\n", ss);
		return -1;
	}
	else
	{
		printf("\r\n ERR: format salah !\r\n");
		return -2;
	}
}
