/*
	berkaitan dengan titik
	12 jan 09
	furkan jadid
	daun biru engineering 
	* 
	
	setiap mesin dialokasikan 70 titik
	sehingga jika ada 10 mesin, diperlukan 
	sekitar 10 * 70 * 2 byte = 1400 byte untuk struknya
	dan 10 * 70 * 4 byte (float) = 2800 byte untuk datanya
	
	*/

#include "../monita/monita_uip.h"
extern struct t_mesin mesin[];
extern struct t_titik titik[TIAP_MESIN * JML_MESIN];

char *f_out(float ff);
							  
void cek_titik(unsigned char no_mes)
{
	int i;
	int pasnya;
	
	printf(" No. Sbr. Knl. Data     : Keterangan\r\n");
	garis_bawah();
	
	pasnya = TIAP_MESIN * (no_mes - 1);
	//titik[pasnya].data = 0.1234567;
	//titik[pasnya+1].data = 12345.67;
	
	printf("%3d. %3d. %3d. %9s: Charge air pressure kiri\r\n", \ 			
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %9s: Charge air pressure kanan\r\n", \			
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %9s: Charge air temperature kiri\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %9s: Charge air temperature kanan\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	
	printf("%3d. %3d. %3d. %9s: Jacket water inlet pressure 1\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %9s: Jacket water inlet pressure 2\r\n", \		
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %9s: Jacket water outlet pressure 1\r\n", \	
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %9s: Jacket water outlet pressure 2\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %9s: Jacket water inlet temperature 1\r\n", \	
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %9s: Jacket water inlet temperature 2\r\n", 	\
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %9s: Jacket water outlet temperature 1\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %9s: Jacket water outlet temperature 2\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));

	printf("%3d. %3d. %3d. %9s: Lub oil inlet pressure 1\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %9s: Lub oil inlet pressure 2\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %9s: Lub oil outlet pressure 1\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));	
	printf("%3d. %3d. %3d. %9s: Lub oil outlet pressure 2\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %9s: Lub oil inlet temperature 1\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %9s: Lub oil inlet temperature 2\r\n", 	\
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %9s: Lub oil outlet temperature 1\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %9s: Lub oil outlet temperature 2\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));

	for (i=0; i<20; i++)
	{
		printf("%3d. %3d. %3d. %9s: Exhaust temperature %d\r\n",  \
			pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data), (i+1));
	}
	
	for(i=0; i<5; i++)
	{
		printf("%3d. %3d. %3d. %9s: --\r\n", \
			pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	}
	
	printf("%3d. %3d. %3d. %9s: Engine rpm \r\n" , 	\
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %9s: Turbo rpm 1\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %9s: Turbo rpm 2\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %9s: Fuel inlet counter\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %9s: Fuel outlet counter\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %9s: Power (kW)\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %9s: Energi (kWh)\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %9s: Voltage (V)\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));
	printf("%3d. %3d. %3d. %9s: Ampere (A)\r\n", \
		pasnya+1, titik[pasnya].ID_sumber, titik[pasnya].kanal, f_out(titik[pasnya++].data));	
		
	printf("cek = %d\r\n", pasnya);
	
	
	
}							 

static tinysh_cmd_t cek_titik_cmd={0,"cek_titik","--","[args]",
                              cek_titik,0,0,0};


char *f_out(float ff)
{
	char tek[32];
	sprintf(tek, "%5.3f", ff);	
	
	return tek;	
}


void set_titik(int argc, char **argv)
{
	unsigned char buf[24];
	int sumb;
	int kanal;
	int ttk;
	
	if (argc < 4) 
	{
		printf(" ERR: argument kurang !\r\n");
		return;	
	}
	
	printf(" set_titik %s dipanggil\r\n", argv[1]);
	garis_bawah();
  	display_args(argc,argv);
	
	sprintf(buf, "%s", argv[1]);	
	ttk = cek_nomer_sumber(buf, (TIAP_MESIN * JML_MESIN));
	if (ttk > 0)
	{
		sprintf(buf, "%s", argv[2]);	
		sumb = cek_nomer_sumber(buf, JML_SUMBER);
		if (sumb > 0)
		{
			sprintf(buf, "%s", argv[3]);	
			kanal = cek_nomer_sumber(buf, JML_KANAL);
			if (kanal > 0)
			{
				titik[ttk - 1].ID_sumber = sumb;
				titik[ttk - 1].kanal = kanal;
			}
		} 
	}
	return;
}							 

static tinysh_cmd_t set_titik_cmd={0,"set_titik","titik sumber kanal","[args]",
                              set_titik,0,0,0};


void save_titik(void)
{
	printf("Save struct titik ke flash ..");
	if(prepare_flash(SEKTOR_TITIK, SEKTOR_TITIK)) return;
	printf("..");
	
	if(hapus_flash(SEKTOR_TITIK, SEKTOR_TITIK)) return;
	printf("..");
	
	if(prepare_flash(SEKTOR_TITIK, SEKTOR_TITIK)) return;
	printf("..");
	
	if(tulis_flash(ALMT_TITIK, (unsigned short *) &titik, sizeof (titik))) return;
	printf(".. OK\r\n");
}


static tinysh_cmd_t save_titik_cmd={0,"save_titik","--","[args]",
                              save_titik,0,0,0};

void read_titik(void)
{
	taskENTER_CRITICAL();
	memcpy((char *)&titik, (char *) ALMT_TITIK, sizeof (titik));
	taskEXIT_CRITICAL();		
}
