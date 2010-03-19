/*
	util untuk cek apakah nomer atau bukan
	dicopy dari sumber.c
	
	21 Jan 2010
	furkan jadid
	daun biru engineering
	
	3 Feb 2010
	proses_passwd ditambahkan
	
*/
static char passin[16];
static int  passc;

int proses_passwd(char *c)
{
	//printf("%s(): c=%c\r\n", __FUNCTION__, (unsigned char) *c );
	printf("*");	
	if ( *c == 0x0D || *c == 0x0A )
	{
		//printf("dicocokkan %s !\r\n", passin);
		passc = 0;
				
		if (strncmp(passin, "diesel", 6) == 0)
		{
			printf("%s(): OK\r\n", __FUNCTION__);
			passc = 0;
			memset(passin, 0, sizeof (passin));
			return 1;
		}
		else
		{
			printf(" Passwd salah !\r\n");
			memset(passin, 0, sizeof (passin));
			return 0;
		}
	}
	else
		passin[ passc ] = (unsigned char) *c;
	
	passc++;
	if (passc > sizeof (passin))
	{
		passc = 0;
		printf("%s(): kepanjangan !\r\n", __FUNCTION__);
		return 0;
	}
	
	return 0;
}

int cek_nomer_valid(char *arg, int maks)
{
	int ss;
	
	ss = baca_kanal(arg);
	
	if (ss > 0 && ss < (maks+1))
	{
		return ss;	
	}	
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

void garis_bawah(void)
{
	printf("-----------------------------------------------------\r\n");	
}

void garis_bawah2(void)
{
	printf("=====================================================\r\n");	
}

void judul(char *s)
{
	garis_bawah2();
	printf(s);
	garis_bawah2();
}
