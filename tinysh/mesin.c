/*
	berkaitan dengan data2 mesin
	12 Jan 09
	furkan jadid
	daun biru engineering
	*/

#include "../monita/monita_uip.h"
extern struct t_mesin mesin[];

							  
void cek_mesin(int argc, char **argv)
{
	int i;
	unsigned char buf[24];
	int no_mesin;
	
	printf(" Mesin data");
	if (argc < 2) 
	{
		printf(" global\r\n");
		garis_bawah();
	
		for (i=0; i<10; i++)
		{
			printf(" (%2d): %10s\r\n", (i+1), mesin[i].nama);	
		}
		return;	
	}
	else
	{
		// cek titik dari nomer mesin yang dimasukkan
		sprintf(buf, "%s", argv[1]);	
		no_mesin = cek_nomer_sumber(buf, 10);
		if (no_mesin > 0)		
		{
			printf(" pada mesin = %d\r\n", no_mesin);
			
			cek_titik(no_mesin);
		}
		else return;
			
	}
	
}							 

static tinysh_cmd_t cek_mesin_cmd={0,"cek_mesin","-- nomer","[args]",
                              cek_mesin,0,0,0};


void set_mesin(int argc, char **argv)
{
	unsigned char buf[24];
	int sumb;
	unsigned int ret_ip;
	
	if (argc < 4) 
	{
		printf(" argument kurang !\r\n");
		return;	
	}
	
	printf(" set_mesin %s dipanggil\r\n", argv[1]);
	garis_bawah();
  	display_args(argc,argv);
	
	if (strcmp(argv[1], "nama") == 0)
	{
		sprintf(buf, "%s", argv[2]);	
		sumb = cek_nomer_sumber(buf, 10);
		if (sumb > 0)		
		{
			printf(" mesin = %d : ", sumb);
			
			if (strlen(argv[3]) > 10)
			{
				printf("nama terlalu panjang !\r\n");
				return;
			}
			sprintf(mesin[sumb-1].nama, argv[3]);
			printf(" Nama : %s\r\n", mesin[sumb-1].nama); 
		}
		else return;	
	}
}							 

static tinysh_cmd_t set_mesin_cmd={0,"set_mesin","nama","[args]",
                              set_mesin,0,0,0};
