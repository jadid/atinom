/*
	command line untuk setting m & C pada kanal
	untuk persamaan 
	
	y = mx + C
	
	15 april 2009
	furkan jadid, daun biru engineering
	
	sintak :
	
	set_kanal A B C
	
	A untuk nomer kanal 
	B untuk m
	C untuk C
	
	misalnya : set_kanal 3 0.34 -34
	
	*/

extern struct t_env env2;


void set_kanal(int argc, char **argv)
{
	unsigned char buf[24];
	unsigned int kanal;
	float m;
	float c;
	int ret;
	
	if (argc < 3) 
	{
		if (argc > 1)
		{
			if (strcmp(argv[1], "help") == 0)
			{
				printf(" setting faktor kalibrasi kanal dengan y = mx + c, misalnya : \r\n");
				printf(" set_kanal 3 1.34 -0.431\r\n");
				printf(" artinya : setting kanal 3 dengan m = 1.34 dan C = -0.431\r\n");
				printf(" \r\n");
				printf(" set_kanal juga bisa untuk setting keterangan, misalnya :\r\n");
				printf(" set_kanal 3 ket jacket_water_temp\r\n");
				printf(" artinya : setting kanal 3 dengan keterangan 'jacket_water_temp'\r\n");
				return;
			} 
			else if (strcmp(argv[1], "default") == 0)
			{
				printf("set kanal dengan data default !\n");
				set_dafault_kalib();
				
				return;
			}	
		}
		printf(" ERR: argument kurang !\r\n");
		printf(" coba set_kanal help \r\n");
		return;	
	}
	
	printf(" set_kanal untuk kanal %s dipanggil\r\n", argv[1]);
	garis_bawah();
  	display_args(argc,argv);
  	
  	sprintf(buf, "%s", argv[1]);
	ret = sscanf(buf, "%d", &kanal);
	
	if (kanal > 10 || ret == NULL) 
	{
		printf(" Err kanal !\r\n");
		return ;
	}
	
	if (strcmp(argv[2], "ket") == 0)
  	{
  		printf(" Setting keterangan kanal %d :\r\n", kanal);
  		sprintf(env2.kalib[kanal-1].ket, "%s", argv[3]);
  		printf(" %s", env2.kalib[kanal-1].ket);
  	}
  	else
  	{
		sprintf(buf, "%s", argv[2]);
		ret = sscanf(buf, "%f", &m);
	
		if (ret == NULL) 
		{
			printf(" Err m !\r\n"); 
			return ;
		}
	
		sprintf(buf, "%s", argv[3]);
		ret = sscanf(buf, "%f", &c);
	
		if (ret == NULL) 
		{
			printf(" Err C !\r\n"); 
			return ;
		}
	
		printf(" Seting kanal %d, m = %f, dan C = %f\r\n", kanal, m, c);
		env2.kalib[kanal - 1].m = m;
		env2.kalib[kanal - 1].C = c;
		
	}
}

static tinysh_cmd_t set_kanal_cmd={0,"set_kanal","setting kanal","help",
                              set_kanal,0,0,0};	
