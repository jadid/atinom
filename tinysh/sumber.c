/*
	berkaiatan dengan sumber ip address 
	12 Jan 09
	furkan jadid
	daun biru engineering
	
	*/
	
#include "../monita/monita_uip.h"
extern struct t_sumber sumber[JML_SUMBER];
int cek_nomer_sumber(char *arg, int maks);
void set_awal_sumber(void);
							  
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
			printf("Tidak Aktif\r\n");
		else if (sumber[i].status == 1)
			printf("Aktif / Normal\r\n");
		else if (sumber[i].status == 2)
			printf("TimeOut\r\n");
		
		else
			printf("\r\n");
	}
	
}							 

static tinysh_cmd_t cek_sumber_cmd={0,"cek_sumber","--","[args]",
                              cek_sumber,0,0,0};

void set_sumber(int argc, char **argv)
{
	unsigned char buf[24];
	int sumb;
	unsigned int ret_ip;
	int stat;
	
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
			else if (strcmp(argv[1], "default") == 0)
			{
				set_awal_sumber();
				
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
	else if (strcmp(argv[1], "status") == 0)
	{
		sprintf(buf, "%s", argv[2]);	
		sumb = cek_nomer_sumber(buf, 20);
		if (sumb > 0)		
		{
			printf(" sumber = %d : ", sumb);
			
			// 0 tidak dipakai, 1 dipakai / diaktifkan , 2 nyambung & aktif, 3 timeout
			sprintf(buf, "%s", argv[3]);	
			stat = cek_nomer_sumber(buf, 1);
			
			if (stat >=0)
			{
				sumber[sumb-1].status = stat;
				printf("%d.%d.%d.%d : ", sumber[sumb-1].IP0, sumber[sumb-1].IP1, sumber[sumb-1].IP2, sumber[sumb-1].IP3); 
				if (stat == 0) printf("Tidak diaktifkan\r\n");
				else if (stat == 1) printf("Diaktifkan\r\n");	
			}
		}
		else return;	
	}
}							 

static tinysh_cmd_t set_sumber_cmd={0,"set_sumber","help ipaddr nama status default","[args]",
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
	//else if (ss == 0 || ss > maks)
	else if (ss == 0)
	{
		return 0;	
	}
	else if (ss > maks)
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

void save_sumber(void)
{
	printf("Save struct sumber ke flash ..");
	if(prepare_flash(SEKTOR_SUMBER, SEKTOR_SUMBER)) return;
	printf("..");
	
	if(hapus_flash(SEKTOR_SUMBER, SEKTOR_SUMBER)) return;
	printf("..");
	
	if(prepare_flash(SEKTOR_SUMBER, SEKTOR_SUMBER)) return;
	printf("..");
	
	if(tulis_flash(ALMT_SUMBER, (unsigned short *) &sumber, sizeof (sumber))) return;
	printf(".. OK\r\n");
	
}

static tinysh_cmd_t save_sumber_cmd={0,"save_sumber","--","[args]",
                              save_sumber,0,0,0};


void read_sumber(void)
{
	taskENTER_CRITICAL();
	memcpy((char *)&sumber, (char *) ALMT_SUMBER, sizeof (sumber));
	taskEXIT_CRITICAL();		
}

void set_awal_sumber(void)
{
	int i;
	
	for (i=0; i<JML_SUMBER; i++)
	{
		sprintf(sumber[i].nama, "-");
		sumber[i].ID_sumber = i;
		sumber[i].status = 0;	
		
		sumber[i].IP0 = 192;
		sumber[i].IP1 = 168;
		sumber[i].IP2 = 1;
		sumber[i].IP3 = 255;
		
	}	
	
	/* testing
	sumber[2].status = 2;
	sumber[2].IP0 = 192;
	sumber[2].IP1 = 168;
	sumber[2].IP2 = 1;
	sumber[2].IP3 = 251;
	*/
}
