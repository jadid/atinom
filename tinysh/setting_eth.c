/*
	berkaiatan dengan sumber ip address 
	afrendy bayu
	daun biru engineering

	24 Sept 2010
	Setting data id via webserver
*/

#include "../monita/monita_uip.h"

#ifndef __SETTING_ETH__
#define __SETTING_ETH__

#ifdef PAKAI_PM	
extern char * judulnya_pm[];	
#endif
			  
void cek_konfig(int argc, char **argv)	{
	int i=0,j=0;
	struct t_setting *konfig;
	int jmlData = (sizeof(data_f)/sizeof(float));
	konfig = (char *) ALMT_KONFIG;
	
	//#ifdef PAKAI_PM
	int t=0;
	unsigned char str_eth[24];
	
	struct t_sumber *p_sbr;
	p_sbr = (char *) ALMT_SUMBER;

	if (argc < 3) {
		printf("Perintah salah!!!\r\n");
		printf("Gunakan : \r\n");
		printf("cek_konfig alamat [alamat client Modbus] <---- khusus client Modbus\r\n");
		printf("cek_konfig no     [urutan modul Sumber]\r\n");
		return;
	} else {
		if (strcmp(argv[1], "alamat") == 0)	{			// khusus modul MODBUS
			sprintf(str_eth, "%s", argv[2]);	

			t = cek_nomer_sumber(str_eth, 250);
			if (t > 0)	{
				for(j=0; j<JML_SUMBER; j++) {
					if (((int)p_sbr[j].alamat)==t) {
						break;
					}
				}

				#ifdef PAKAI_PM
				if (p_sbr[j].tipe==0) {		// modul Power Meter
					if (j==JML_SUMBER) {
						printf("Alamat PM tidak ditemukan\r\n");
						return;
					}
					
					if (p_sbr[j].status==0) {
						printf("PM Alamat %d tidak aktif\r\n", j);
						return;
					}
				}
				#endif
				
			} else	{
				printf("Alamat tidak benar !!!\r\n");
				return;
			}	
		} else if (strcmp(argv[1], "no") == 0)	{
			sprintf(str_eth, "%s", argv[2]);	

			j = cek_nomer_sumber(str_eth, JML_SUMBER);
			if (j > 0)	{
				printf(" no sumber: %d\r\n", j);
			} else	{
				return;
			}	
		}
	}
	printf(" alamat: %d, index: %d\r\n", t, j);
	//garis_bawah2();
	printf("Konfigurasi Sumber ke-%d: %s, Alamat: %d\r\n", j, p_sbr[j].nama, p_sbr[j].alamat);
	//garis_bawah2();

	//#endif
	printf("  No.    ID      Nama             Status\r\n");
	int w=1,z=0;
	for (i=0; i<45; i++)
		printf("-");
	printf("\r\n");
	
	#ifdef BOARD_KOMON_KONTER
	struct t_env *env2;
	env2 = (char *) ALMT_ENV;
	
	if (env2->IP3 == p_sbr[j-1].IP3) {
		printf("Modul Konter !!\r\n");
		for (i=0; i<KANALNYA; i++)	{
			#ifndef BOARD_KOMON_KONTER_3_1 
			if (i>6)
			#endif 
			{
				z++;
				printf(" (%2d): %4d : %-18s %2d : ", z, konfig[i*2].id, "Frek/RPM kanal", i+1);
				if (konfig[i*2].status == 0)
					printf("%-16s","Tidak Aktif");
				else if (konfig[i*2].status == 1)
					printf("%-16s","Aktif / Normal");
				else if (konfig[i*2].status == 2)
					printf("%-16s","TimeOut");
				else
					printf("%-16s"," ");
				printf("\r\n");
				
				z++;
				printf(" (%2d): %4d : %-18s %2d : ", z, konfig[i*2+1].id, "Pulsa konter kanal", i+1);
				if (konfig[i*2+1].status == 0)
					printf("%-16s","Tidak Aktif");
				else if (konfig[i*2+1].status == 1)
					printf("%-16s","Aktif / Normal");
				else if (konfig[i*2+1].status == 2)
					printf("%-16s","TimeOut");
				else
					printf("%-16s"," ");
				printf("\r\n");
			}
		}
	} else {
		j--;
		for (i=0; i<PER_SUMBER; i++)	{
			//ganti_karakter(ket, konfig[i].ket);
			if (p_sbr[j].tipe==100) 							// modul monita
				printf(" (%2d): %4d : %-16s : ", ((j*PER_SUMBER)+i+1), konfig[j*PER_SUMBER+i].id, konfig[j*PER_SUMBER+i].ket);
			else if (p_sbr[j].tipe==0 || p_sbr[j].tipe==1)		// modbus
				printf(" (%2d): %4d : %-16s : ", ((j*PER_SUMBER)+i+1), konfig[j*PER_SUMBER+i].id, judulnya_pm[i]);
			
			// status //
			if (konfig[j*PER_SUMBER+i].status == 0)				printf("%-16s","Tidak Aktif");
			else if (konfig[j*PER_SUMBER+i].status == 1)		printf("%-16s","Aktif / Normal");
			else if (konfig[j*PER_SUMBER+i].status == 2)		printf("%-16s","TimeOut");
			else												printf("%-16s"," ");
			printf("\r\n");
		}
	}
	#endif
	
	#ifdef BOARD_KOMON_420_SABANG
	if (t>0) {
		#ifdef PAKAI_PM
			if (p_sbr[j-1].tipe==0 || p_sbr[j-1].tipe==1 || p_sbr[j-1].tipe==100) {	// Power Meter 0:PM710, 1:PM810
				for (i=0; i<PER_SUMBER; i++)	{
					printf(" (%2d): %4d : %-16s : ", ((j*PER_SUMBER)+i+1), konfig[j*PER_SUMBER+i].id, judulnya_pm[i]);
					// status //
					if (konfig[j*PER_SUMBER+i].status == 0)
						printf("%-16s","Tidak Aktif");
					else if (konfig[j*PER_SUMBER+i].status == 1)
						printf("%-16s","Aktif / Normal");
					else if (konfig[j*PER_SUMBER+i].status == 2)
						printf("%-16s","TimeOut");
					else
						printf("%-16s"," ");
					printf("\r\n");
				}
			}
		#endif
	} else  {//if (t==0) {
		if (p_sbr[j-1].alamat==0) {		// Modul Monita
			for (i=0; i<20; i++)	{
				printf(" (%2d): %4d : %-16s : ", (((j-1)*PER_SUMBER)+i+1), konfig[(j-1)*PER_SUMBER+i].id, konfig[(j-1)*PER_SUMBER+i].ket );			// rata kiri
				
				// status //
				if (konfig[(j-1)*PER_SUMBER+i].status == 0)
					printf("%-16s","Tidak Aktif");
				else if (konfig[(j-1)*PER_SUMBER+i].status == 1)
					printf("%-16s","Aktif / Normal");
				else if (konfig[(j-1)*PER_SUMBER+i].status == 2)
					printf("%-16s","TimeOut");
				else
					printf("%-16s"," ");
				printf("\r\n");
			}
		} else if (p_sbr[j-1].alamat>0) {	// Modul Modbus
			#ifdef PAKAI_PM
			if (p_sbr[j-1].tipe==0 || p_sbr[j-1].tipe==1) {	// Power Meter
				for (i=0; i<PER_SUMBER; i++)	{
					printf(" (%2d): %4d : %-16s : ", (((j-1)*PER_SUMBER)+i+1), konfig[(j-1)*PER_SUMBER+i].id, judulnya_pm[i]);
					// status //
					if (konfig[(j-1)*PER_SUMBER+i].status == 0)
						printf("%-16s","Tidak Aktif");
					else if (konfig[(j-1)*PER_SUMBER+i].status == 1)
						printf("%-16s","Aktif / Normal");
					else if (konfig[(j-1)*PER_SUMBER+i].status == 2)
						printf("%-16s","TimeOut");
					else
						printf("%-16s"," ");
					printf("\r\n");
				}
			}
			#endif
		}
	}
	#endif
}
//
							 
//*
static tinysh_cmd_t cek_konfig_cmd={0,"cek_konfig","menampilkan konfig modul","[args]", cek_konfig,0,0,0};
//*/
void set_konfig(int argc, char **argv)		{
	unsigned char str_eth[24];
	int sumb;
	unsigned int ret_ip;
	int stat;
	int jmlData = (sizeof(data_f)/sizeof(float));
	
	judul(" Setting Konfig\r\n");
	
	if (argc < 4) 	{
		if (argc > 1)	{
			if (strcmp(argv[1], "help") == 0)	{
				printf(" Setting konfig data\r\n");
				garis_bawah();
				printf(" argument : id, ket, status, help\r\n");
				printf("   help   : printout keterangan ini\r\n");
				printf("   default : load sumber dengan default ip & setting\r\n");
				printf(" \r\n");
				printf("   id : memberikan alamat id pada sumber titik tertentu\r\n");
				printf("     Power Meter tidak dipengaruhi setting ini\r\n");
				printf("     misalnya : $ set_konfig 4 id 19\r\n");
				printf("     artinya set setting nomer 4 dengan id 19\r\n");
				printf(" \r\n");
				printf("   ket : memberikan ket pada titik tertentu\r\n");
				printf("     misalnya : $ set_konfig 4 ket RTD_SWD#2\r\n");
				printf("     artinya sumber nomer 4 diberi ket RTD_SWD#2 (maks 10 karakter)\r\n");	
				printf(" \r\n");
				printf("   status : set pada sumber tertentu\r\n");
				printf("     status 0 = tidak aktif, 1 = aktif\r\n");
				printf("     aktif artinya sumber akan dikirim terus menerus !\r\n");			
				printf("     misalnya : $ set_konfig 4 status 1\r\n");
				printf("     artinya set nomer 4 supaya aktif\r\n");	
				printf(" \r\n");	
				return;
			} 
			else if (strcmp(argv[1], "default") == 0)
			{
				printf(" Set konfig dengan data default !\n");
				set_awal_konfig();
				
				return;
			}	
		}
		printf(" ERR: argument kurang !\r\n");
		printf(" coba set_konfig help \r\n");
		return;	
	}
	
  	display_args(argc,argv);
	

	struct t_setting *p_sbr;
	printf("Jml Data : %d\r\n", jmlData);
	p_sbr = pvPortMalloc( jmlData * sizeof (struct t_setting) );
	
	if (p_sbr == NULL)	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		return;
	}
	printf(" %s(): Mallok ok di %X\r\n", __FUNCTION__, p_sbr);	
	memcpy((char *) p_sbr, (char *) ALMT_KONFIG, (jmlData * sizeof (struct t_setting)));
	
	
	if (strcmp(argv[2], "id") == 0)		{
		sprintf(str_eth, "%s", argv[1]);	
		
		sumb = cek_nomer_sumber(str_eth, jmlData);
		if (sumb >= 0)	{
			printf(" sumber = %d\r\n", sumb);
			sprintf(str_eth, "%s", argv[3]);	
			stat = cek_nomer_sumber(str_eth, 32765);
			
			p_sbr[sumb-1].id = stat;
			printf("ID = %d\r\n", p_sbr[sumb-1].id);
			
		}
		else	{
			vPortFree( p_sbr );
			return;
		}	
	}
	else if (strcmp(argv[2], "nama") == 0)		{
		//printf("masuk sini\r\n");
		sprintf(str_eth, "%s", argv[1]);	
		sumb = cek_nomer_sumber(str_eth, jmlData);
		if (sumb > 0)		{
			printf(" sumber = %d : ", sumb);
			
			if (strlen(argv[3]) > 15)	{
				printf("\n ERR: nama terlalu panjang !\r\n");
				vPortFree( p_sbr );

				return;
			}
			sprintf(p_sbr[sumb-1].ket, argv[3]);
			printf(" Nama : %s\r\n", p_sbr[sumb-1].ket); 
		}
		else	{
			vPortFree( p_sbr );
			return;
		}	
	}
	else if (strcmp(argv[2], "status") == 0)	{
		printf(" set status konfig\r\n");

		sprintf(str_eth, "%s", argv[1]);	
		sumb = cek_nomer_sumber(str_eth, jmlData);
		if (sumb > 0)	{
			printf(" sumber = %d : ", sumb);
			
			/* 0 tidak dipakai, 1 dipakai / diaktifkan , 5 daytime */
			sprintf(str_eth, "%s", argv[3]);	
			
			if (( argv[3][0] == '1') || (argv[3][0] == '0')) {
				p_sbr[sumb-1].status = (argv[3][0] - '0');
			} 
			else if (strcmp(argv[3], "aktif")==0 || strcmp(argv[3], "hidup")==0) {
				p_sbr[sumb-1].status = 1;
			} else if (strcmp(argv[3], "mati")==0)  {
				p_sbr[sumb-1].status = 0;
			} else {
				p_sbr[sumb-1].status = 0;
			}

		}	else {
			vPortFree( p_sbr );
			return;
		}	
	}
	else	{
		printf(" ERR: perintah tidak benar !\r\n");
		printf(" coba set_sumber help \r\n");
		
		vPortFree( p_sbr );			
		return;	
	}
	
	// SEMUA TRUE dan sampai disini
	if (simpan_konfignya( p_sbr ) < 0)		{
		vPortFree( p_sbr );
		return;
	}
	vPortFree( p_sbr );
}			 

//*
tinysh_cmd_t set_konfig_cmd={0,"set_konfig","set konfig untuk beberapa hal","help id ket status",
                              set_konfig,0,0,0};
/*
void read_konfig(void)	{
	struct t_setting *setting;
	
	setting = (char *) ALMT_KONFIG;
	printf("%s(): OK\r\n", __FUNCTION__);		
}
//*/
void set_awal_konfig(void)	{
	int i;	
	struct t_setting *p_stg;
	int jmlData = (sizeof(data_f)/sizeof(float));
	//judul(" Set Sumber ke Default\r\n");
	//printf("%s(), Jml data: %d\r\n", __FUNCTION__, jmlData);
	p_stg = pvPortMalloc( jmlData * sizeof (struct t_setting) );
	
	if (p_stg == NULL)	{
		printf("%s(): Err allok memory gagal !\r\n");
		return;
	}
	
	for (i=0; i<(PER_SUMBER*JML_SUMBER); i++)	{
		sprintf(p_stg[i].ket, "-");
		p_stg[i].id = 0;		/* default alamat = 0 : board Monita, PM = 1 s/d 247 / stack */
		p_stg[i].status = 0;
	}
	
	/*
	for(i=0; i<3; i++) {
		printf("id: %d, status: %d, ket: %s\r\n", p_stg[i].id, (int) p_stg[i].status, p_stg[i].ket);
	}
	//*/
	
	if (simpan_konfignya( p_stg ) < 0)		{
		vPortFree( p_stg );
		return;
	}
	vPortFree( p_stg );
	
	/*
	struct t_setting *konfig;
	konfig = (char *) ALMT_KONFIG;
	
	for(i=0; i<3; i++) {
		printf("id: %d, status: %d, ket: %s\r\n", konfig[i].id, (int) konfig[i].status, konfig[i].ket);
	}
	//*/
}

 int simpan_konfignya( struct t_setting *p_stg)	{
	printf(" Save struct KONFIG ke flash ..");
	if(prepare_flash(SEKTOR_KONFIG, SEKTOR_KONFIG)) return -1;
	printf("..");
	
	if(hapus_flash(SEKTOR_KONFIG, SEKTOR_KONFIG)) return -1;
	printf("..");
	
	if(prepare_flash(SEKTOR_KONFIG, SEKTOR_KONFIG)) return -1;
	printf("..");
	
	if(tulis_flash(ALMT_KONFIG, (unsigned short *) p_stg, (sizeof (struct t_setting) * (sizeof(data_f)/sizeof(float))))) return -1;
	
	printf(".. OK\r\n");
	return 0;
}

#endif
