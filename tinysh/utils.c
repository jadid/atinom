/*
	util untuk cek apakah nomer atau bukan
	dicopy dari sumber.c
	
	21 Jan 2010
	furkan jadid
	daun biru engineering
	
	3 Feb 2010
	proses_passwd ditambahkan
	
*/

#ifndef __UTILS__
#define __UTILS__

static char passin[16];
static int  passc;

#define DIGANTI 5		// jml kata2 yg harus direplace : \\r, \\n di fungsi ganti_kata
/*
void flush_modem() {
	int i;
	int loop;
	
	for (i=0; i<100; i++)
		#ifdef PAKAI_SERIAL_1
			if (PAKAI_SMS==1)
				ser1_getchar(1, &loop, 20 );
		#endif
		#ifdef PAKAI_SERIAL_2
			if (PAKAI_SMS==2)
				ser2_getchar(1, &loop, 20 );
		#endif
		#ifdef PAKAI_SERIAL_3
			if (PAKAI_SMS==3)
				ser3_getchar(1, &loop, 20 );
		#endif
}
//*/
#define JML_STR_SMS	19

int data_titik_ukur(char * nilai, int pilih) {
	int i, jml=0, jm;
	char strSMS[128];
	char nilaif[20];
	
	struct t_simpan_file *ts;
	struct t_dt_set *dt;
	ts = (char *) ALMT_SFILE;
	dt = (char *) ALMT_DT_SET;
	
	strcpy(strSMS,"");
	for(i=0; i<(JML_SUMBER*PER_SUMBER); i++)	{
		jm = ts->no_data[i];
		if (jm != 0) {
			jml++;
			printf("%2d. %-15s : %10.2f : %s\r\n",jml, dt[jm-1].nama, data_f[jm-1], dt[jm-1].satuan);
			if (pilih == 1) {					// info
				if (jml<=JML_STR_SMS)	strcat(strSMS, dt[jm-1].nama);
				//else return jml;
			} else if (pilih == 2) {			// data
				if (jml<=JML_STR_SMS)	{				
					sprintf(nilaif, "%.2f", data_f[jm-1]);
					strcat(strSMS, nilaif);
				} 
				//else return jml;
			} else if (pilih == 3) {			// satuan
				if (jml<JML_STR_SMS)	strcat(strSMS, dt[jm-1].satuan);
				//else return jml;
			}
			if (pilih>=1 && pilih<=3)
				strcat(strSMS, "\n");
		}
	}
	strcpy(nilai, strSMS);
	//printf("%s\r\n", strSMS);
	return jml;
}

int proses_passwd(char *c)	{
	//printf("%s(): c=%c\r\n", __FUNCTION__, (unsigned char) *c );
		
	if ( *c == 0x0D || *c == 0x0A )			// enter !!
	{
		//printf("dicocokkan %s !\r\n", passin);
		passc = 0;
				
		if (strncmp(passin, "diesel", 6) == 0)
		{
			printf("\r\n%s(): OK\r\n", __FUNCTION__);
			passc = 0;
			memset(passin, 0, sizeof (passin));
			return 1;
		}
		else
		{
			printf(" Passwd salah\t!\r\n");
			memset(passin, 0, sizeof (passin));
			return 0;
		}
	} else if (*c==8 || *c==127) {
		if(passc>0)
        {
          printf("\b \b");
          passc--;
          passin[passc]=0;
        }
	}
	else {
		printf("*");
		//printf("%c",*c);
		passin[ passc ] = (unsigned char) *c;
		passc++;
	}
	
	if (passc > sizeof (passin))	{
		passc = 0;
		printf("%s(): kepanjangan !\r\n", __FUNCTION__);
		return 0;
	}
	
	return 0;
}

int cek_nomer_valid(char *arg, int maks)	{
	int ss;
	
	ss = baca_kanal(arg);
	
	if (ss > 0 && ss < (maks+1))		{
		return ss;	
	}	
	else if (ss == 0)		{
		return 0;	
	}
	else if (ss > maks)		{
		printf("\r\n ERR: %d diluar range !\r\n", ss);
		return -1;
	}
	else	{
		printf("\r\n ERR: format salah !\r\n");
		return -2;
	}
}

int cek_nomer_sumber(char *arg, int maks)	{
	unsigned char str_utils[24];
	int ss;
	
	sprintf(str_utils, "%s", arg);
	ss = baca_kanal(str_utils);
	
	if (ss > 0 && ss < (maks+1))	{
		return ss;	
	}	
	//else if (ss == 0 || ss > maks)
	else if (ss == 0)	{
		return 0;	
	}
	else if (ss > maks)	{
		printf("\r\n ERR: %d diluar range !\r\n", ss);
		return -1;
	}
	else	{
		printf("\r\n ERR: format salah !\r\n");
		return -2;
	}
}

void garis_bawah(void)	{
	printf("-----------------------------------------------------\r\n");	
}

void garis_bawah2()	{
	printf("=====================================================\r\n");	
}

void judul(char *s)	{
	garis_bawah2();
	printf(s);
	garis_bawah2();
}

int ganti_kata(char *dest, char *src) {
	//printf ("kalimat: %s, p: %d\r\n",src, strlen(src));
	char *asli[] = {"\\r", "\\n", "s/", "s\%",  "s%%"};
	char *pengganti[] = {"\r", "\n", "", "", ""};
	char * pch;

	int i=0, j=0, k=0;
	for (j=0; src[j] != '\0'; ++j) {
		dest[i] = src[j];
		//*
		for (k=0; k<DIGANTI; k++) {
			pch = strstr (dest,asli[k]);
			
			if (pch) {
				strncpy (pch,pengganti[k],strlen(pengganti[k]));
				strcat(dest, pengganti[k]);
				//printf ("ada pch\r\n");
				i += strlen(pengganti[k])-strlen(asli[k]);
			}
		}
		//*/	
		//printf ("kalimat: %s, p: %d\r\n",dest, strlen(dest));
		++i;
	}
	dest[i] = '\0';
}

#ifdef PAKAI_PM
	char * judulnya_pm[] __attribute__ ((section (".lokasi_font"))) = {
		"kWh",				// 0 //
		"kVAh",
		"kVArh",
		"kW",
		"kVA",		
		"kVAr",
		"Power Faktor",
		"Volt1",
		"Volt2",
		"Arus",
		"Frek",				// 10 //
		"Arus R",
		"Arus S",
		"Arus T",
		"Arus Netral",
		"Tegangan Vab",
		"Tegangan Vbc",
		"Tegangan Vac",
		"Tegangan Van",
		"Tegangan Vbn",
		"Tegangan Vcn",		// 20 //
		"Daya kW R",
		"Daya kW S",
		"Daya kW T",
		"Daya kVA R",
		"Daya kVA S",
		"Daya kVA T",
		"Daya kVAr R",
		"Daya kVAr S",
		"Daya kVAr T",
		"Power Faktor R",	// 30 //
		"Power Faktor S",
		"Power Faktor T"
	};
	
	char * satuannya_pm[] __attribute__ ((section (".lokasi_font"))) = {
		"kWh",		// 0 //
		"kVAh",
		"kVArh",
		"Watt",
		"kVA",		
		"kVAr",
		" ",
		"Volt",
		"Volt",
		"Ampere",
		"Hz",		// 10 //
		"Ampere",
		"Ampere",
		"Ampere",
		"Ampere",
		"Volt",
		"Volt",
		"Volt",
		"Volt",
		"Volt",
		"Volt",		// 20 //
		"kW",
		"kW",
		"kW",
		"kVA",
		"kVA",
		"kVA",
		"kVAr",
		"kVAr",
		"kVAr",
		" ",			// 30 //
		" ",
		" "
	};
#endif

#endif


