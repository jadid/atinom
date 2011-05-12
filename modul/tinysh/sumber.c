/*
	berkaiatan dengan sumber ip address 
	12 Jan 09
	furkan jadid
	daun biru engineering
	
	27 Feb 09, jadid
	setting nomer alamat belum dimassukkan
	ini akan menimbulkan kesulitan misalnya untuk 1 IP dengan 
	banyak data, misalnya server power meter.
	
	cara menggunakan :
	set_sumber alamat 2 1
	
	konsekuensi pada saat reques data, alamat ini harus
	disertakan, juga saat menerima data. alamat harus dicek ... huh
	
	3 Feb 2010, furkan jadid
	Untuk menghemat pemakaian RAM
	sumber diakses pakai pointer kepada memory di flash
	
	18 Maret 2010
	Ubah perintah sumber ke format standart : kommand no_x parameter data
*/

//#include <regex.h>
//#include "../monita/monita_uip.h"
#include "../../app/monita/monita_uip.h"

#ifndef __SUMBER__
#define __SUMBER__

struct t_sumber *sumber;
//int cek_nomer_sumber(char *arg, int maks);
//void set_awal_sumber(void);
//static int simpan_sumber( struct t_sumber *pgr);
							  
void cek_sumber(void)
{
	int i;
	struct t_sumber *sumber;
	
	sumber = (char *) ALMT_SUMBER;
	printf("  No :    Nama    :      IPaddr     : Stack : Almt :   Tipe   :    Status\r\n");

	for (i=0; i<67; i++)
		printf("-");
	
	printf("\r\n");
	
	
	for (i=0; i<JML_SUMBER; i++)
	{
		printf(" (%2d): %-10s : ", (i+1), sumber[i].nama);			// rata kiri
		printf("%3d.%3d.%3d.%3d : ", sumber[i].IP0, sumber[i].IP1, sumber[i].IP2, sumber[i].IP3);
		
		/* alamat */
		printf("%5d : %4d : %3d ", sumber[i].stack, sumber[i].alamat, sumber[i].tipe);
		
		/* tipe */
		if (sumber[i].tipe==0 && sumber[i].alamat>0) {
			printf("%-4s :","710");
		} else if(sumber[i].tipe==1 && sumber[i].alamat>0) {
			printf("%-4s :","810");
		} else if(sumber[i].tipe==2 && sumber[i].alamat>0) {
			printf("%-4s :","KTA");
		} else {
			printf("%-4s :","-");
		}
		
		/* status */
		if (sumber[i].status == 0)
			printf(" %-16s","Tidak Aktif");
		else if (sumber[i].status == 1)
			printf(" %-16s","Aktif / Normal");
		else if (sumber[i].status == 2)
			printf(" %-16s","TimeOut");
		else if (sumber[i].status == 5)
			printf(" %-16s","Aktif / Daytime server");
		else
			printf(" %-16s"," ");
		
		if (sumber[i].status == 1) {
			if (sumber[i].alamat>0) {
				printf("%s\r\n","Power Meter");
			} else if (sumber[i].alamat==0) {
				printf("%s\r\n","Modul Monita");
			} else {
				printf("%s\r\n","");
			}		
		} else {
			printf("%s\r\n","");
		}
	}
	
	
}							 

static tinysh_cmd_t cek_sumber_cmd={0,"cek_sumber","menampilkan konfigurasi sumber","[args]",
                              cek_sumber,0,0,0};

void set_sumber(int argc, char **argv)
{
	unsigned char str_sumber[24];
	int sumb;
	unsigned int ret_ip;
	int stat;
	
	judul(" Setting Sumber\r\n");
	
	if (argc < 4) 
	{
		if (argc > 1)
		{
			if (strcmp(argv[1], "help") == 0)
			{
				printf(" Setting sumber data\r\n");
				garis_bawah();
				printf(" argument : help, default, ipaddr, nama, status, modul\r\n");
				printf("   help   : printout keterangan ini\r\n");
				printf("   default : load sumber dengan default ip & setting\r\n");
				printf(" \r\n");
				printf("   ipaddr : memberikan alamat IP pada sumber tertentu\r\n");
				printf("     Power Meter tidak dipengaruhi setting ini\r\n");
				printf("     misalnya : $ set_sumber 4 ipaddr 192.168.1.21\r\n");
				printf("     artinya set sumber nomer 4 dengan IP tersebut\r\n");
				printf(" \r\n");
				printf("   nama : memberikan nama pada sumber tertentu\r\n");
				printf("     misalnya : $ set_sumber 4 nama RTD_SWD#2\r\n");
				printf("     artinya sumber nomer 4 diberi nama RTD_SWD#2 (maks 10 karakter)\r\n");	
				printf(" \r\n");
				printf("   status : set pada sumber tertentu\r\n");
				printf("     status 0 = tidak aktif, 1 = aktif, 5 = daytime\r\n");
				printf("     aktif artinya sumber akan dicari terus menerus !\r\n");			
				printf("     misalnya : $ set_sumber 4 status 1\r\n");
				printf("     artinya sumber nomer 4 supaya aktif\r\n");	
				printf(" \r\n");
				printf("   alamat : set nomer alamat\r\n");
				printf("     hal ini berguna misalnya modul memiliki lebih dari 1 alamat\r\n");
				printf("     Alamat digunakan pada Power Meter. Modul monita yang dipanel tidak terpengaruh setting ini.\r\n");
				printf("     Tiap PM HARUS memiliki alamat yang berbeda.\r\n");
				printf("     misalnya : $ set_sumber 4 modul 1\r\n");
				printf("     artinya sumber nomer 4 pada modul nomer 1\r\n");	
				return;
			} 
			else if (strcmp(argv[1], "default") == 0)
			{
				printf(" Set sumber dengan data default !\n");
				set_awal_sumber();
				
				return;
			}	
		}
		printf(" ERR: argument kurang !\r\n");
		printf(" coba set_sumber help \r\n");
		return;	
	}
	
  	display_args(argc,argv);
	
	struct t_sumber *p_sbr;
	p_sbr = pvPortMalloc( JML_SUMBER * sizeof (struct t_sumber) );
	
	
	if (p_sbr == NULL)	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		return -1;
	}
	printf(" %s(): Mallok ok di %X\r\n", __FUNCTION__, p_sbr);	
	memcpy((char *) p_sbr, (char *) ALMT_SUMBER, (JML_SUMBER * sizeof (struct t_sumber)));
	
	
	if (strcmp(argv[2], "ipaddr") == 0)
	{
		sprintf(str_sumber, "%s", argv[1]);	
		sumb = cek_nomer_sumber(str_sumber, JML_SUMBER);
		if (sumb > 0)		{
			printf(" sumber = %d\r\n", sumb);
			sprintf(str_sumber, "%s", argv[3]);
		
			ret_ip = baca_ip(str_sumber);
			
			p_sbr[sumb-1].IP0 = (unsigned char) (ret_ip >> 24);
			p_sbr[sumb-1].IP1 = (unsigned char) (ret_ip >> 16);
			p_sbr[sumb-1].IP2 = (unsigned char) (ret_ip >> 8);
			p_sbr[sumb-1].IP3 = (unsigned char) (ret_ip);
			
			printf("IP = %d.%d.%d.%d\r\n", p_sbr[sumb-1].IP0, p_sbr[sumb-1].IP1, p_sbr[sumb-1].IP2, p_sbr[sumb-1].IP3);
			
		}
		else	{
			vPortFree( p_sbr );
			return;
		}	
	}
	else if (strcmp(argv[2], "nama") == 0)	{
		sprintf(str_sumber, "%s", argv[1]);	
		sumb = cek_nomer_sumber(str_sumber, JML_SUMBER);
		if (sumb > 0)	{
			printf(" sumber = %d : ", sumb);
			
			if (strlen(argv[3]) > 15)	{
				printf("\n ERR: nama terlalu panjang !\r\n");
				vPortFree( p_sbr );
				return;
			}
			sprintf(p_sbr[sumb-1].nama, argv[3]);
			printf(" Nama : %s\r\n", p_sbr[sumb-1].nama); 
		}
		else	{
			vPortFree( p_sbr );
			return;
		}	
	}
	else if (strcmp(argv[2], "status") == 0)
	{
		sprintf(str_sumber, "%s", argv[1]);	
		sumb = cek_nomer_sumber(str_sumber, JML_SUMBER);
		if (sumb > 0)		{
			printf(" sumber = %d : ", sumb);
			
			if ( strcmp(argv[3], "aktif")==0 || strcmp(argv[3], "hidup")==0 ) {
				p_sbr[sumb-1].status = 1;
			} else if ( strcmp(argv[3], "mati")==0 ) 	{
				p_sbr[sumb-1].status = 0;
			} else {
			/* 0 tidak dipakai, 1 dipakai / diaktifkan , 5 daytime */
				sprintf(str_sumber, "%s", argv[3]);	
				stat = cek_nomer_sumber(str_sumber, 5);
				
				if (stat >=0)	{
					p_sbr[sumb-1].status = stat;
					printf("%d.%d.%d.%d : ", p_sbr[sumb-1].IP0, p_sbr[sumb-1].IP1, p_sbr[sumb-1].IP2, p_sbr[sumb-1].IP3); 
					if (stat == 0) printf("Tidak diaktifkan\r\n");
					else if (stat == 1) printf("Diaktifkan\r\n");	
					else if (stat == 5) printf("Daytime\r\n");	
				}
			}
		} else 	{
			vPortFree( p_sbr );
			return;
		}	
	}
	else if (strcmp(argv[2], "tipe") == 0)
	{
		sprintf(str_sumber, "%s", argv[1]);	
		sumb = cek_nomer_sumber(str_sumber, JML_SUMBER);
		if (sumb > 0)		{
			printf(" sumber = %d : ", sumb);
			
			if ( strcmp(argv[3], "710")==0 || strcmp(argv[3], "0")==0) {
				p_sbr[sumb-1].tipe = 0;
			} else if ( strcmp(argv[3], "810")==0 || strcmp(argv[3], "1")==0) 	{
				p_sbr[sumb-1].tipe = 1;
			} else if ( strcmp(argv[3], "KTA")==0 || strcmp(argv[3], "2")==0) 	{
				p_sbr[sumb-1].tipe = 2;
			} else {
				p_sbr[sumb-1].tipe = 0;
			}
		} else 	{
			vPortFree( p_sbr );
			return;
		}	
	}
	else if (strcmp(argv[2], "alamat") == 0)	{
		sprintf(str_sumber, "%s", argv[1]);	
		sumb = cek_nomer_sumber(str_sumber, JML_SUMBER);
		if (sumb > 0)		{
			printf(" sumber = %d : ", sumb);
			
			sprintf(str_sumber, "%s", argv[3]);	
			stat = cek_nomer_sumber(str_sumber, 200);
			
			if (stat >=0)	{
				p_sbr[sumb-1].alamat = stat;
				//printf("%d.%d.%d.%d : ", p_sbr[sumb-1].IP0, p_sbr[sumb-1].IP1, p_sbr[sumb-1].IP2, p_sbr[sumb-1].IP3);
				printf("pd modul = %d\r\n", p_sbr[sumb-1].alamat);
			}
		}
		else {
			vPortFree( p_sbr );
			return;
		}
	}
	else if (strcmp(argv[2], "stack") == 0)
	{
		sprintf(str_sumber, "%s", argv[1]);	
		sumb = cek_nomer_sumber(str_sumber, JML_SUMBER);
		if (sumb > 0)		{
			printf(" sumber = %d : ", sumb);
			
			sprintf(str_sumber, "%s", argv[3]);	
			stat = cek_nomer_sumber(str_sumber, 10);
			
			if (stat >=0)	{
				p_sbr[sumb-1].stack = stat;
				//printf("%d.%d.%d.%d : ", p_sbr[sumb-1].IP0, p_sbr[sumb-1].IP1, p_sbr[sumb-1].IP2, p_sbr[sumb-1].IP3);
				printf("pd modul = %d\r\n", p_sbr[sumb-1].stack);
			}
		}
		else {
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
	if (simpan_sumber( p_sbr ) < 0)	{
		vPortFree( p_sbr );
		return -1;
	}
	vPortFree( p_sbr );
}							 

//static tinysh_cmd_t set_sumber_cmd={0,"set_sumber","help ipaddr nama status alamat default","[args]",
static tinysh_cmd_t set_sumber_cmd={0,"set_sumber","set sumber untuk beberapa hal","help ipaddr nama status alamat default",
                              set_sumber,0,0,0};

void save_sumber(void)
{
	/*
	printf("Save struct sumber ke flash ..");
	if(prepare_flash(SEKTOR_SUMBER, SEKTOR_SUMBER)) return;
	printf("..");
	
	if(hapus_flash(SEKTOR_SUMBER, SEKTOR_SUMBER)) return;
	printf("..");
	
	if(prepare_flash(SEKTOR_SUMBER, SEKTOR_SUMBER)) return;
	printf("..");
	
	if(tulis_flash(ALMT_SUMBER, (unsigned short *) &sumber, sizeof (sumber))) return;
	printf(".. OK\r\n");
	*/
}

static tinysh_cmd_t save_sumber_cmd={0,"save_sumber","menyimpan konfigurasi sumber ke flash","[args]",
                              save_sumber,0,0,0};


void read_sumber(void)
{
	struct t_sumber *sumber;
	
	sumber = (char *) ALMT_SUMBER;
	printf("%s(): OK\r\n", __FUNCTION__);		
}

void set_awal_sumber(void)
{
	int i;	
	struct t_sumber *p_sbr;
	
	//judul(" Set Sumber ke Default\r\n");
	
	p_sbr = pvPortMalloc( JML_SUMBER * sizeof (struct t_sumber) );
	if (p_sbr == NULL)	{
		printf("%s(): Err allok memory gagal !\r\n");
		return -1;
	}
	
	for (i=0; i<JML_SUMBER; i++)	{
		sprintf(p_sbr[i].nama, "-");
		p_sbr[i].alamat = 0;		/* default alamat = 0 : board Monita, PM = 1 s/d 247 / stack */
		p_sbr[i].status = 0;	
		//p_sbr[i].modul  = 0;	
		p_sbr[i].stack  = 1;
		p_sbr[i].tipe  = 100;
		
		p_sbr[i].IP0 = 192;
		p_sbr[i].IP1 = 168;
		p_sbr[i].IP2 = 1;
		p_sbr[i].IP3 = 254;
	}	
	
	if (simpan_sumber( p_sbr ) < 0)		{
		vPortFree( p_sbr );
		return -1;
	}
	vPortFree( p_sbr );
	
	/* testing
	sumber[2].status = 2;
	sumber[2].IP0 = 192;
	sumber[2].IP1 = 168;
	sumber[2].IP2 = 1;
	sumber[2].IP3 = 251;
	*/
}

int simpan_sumber( struct t_sumber *pgr)	{
	printf(" Save struct SUMBER ke flash ..");
	if(prepare_flash(SEKTOR_SUMBER, SEKTOR_SUMBER)) return -1;
	printf("..");
	
	if(hapus_flash(SEKTOR_SUMBER, SEKTOR_SUMBER)) return -1;
	printf("..");
	
	if(prepare_flash(SEKTOR_SUMBER, SEKTOR_SUMBER)) return -1;
	printf("..");
	
	if(tulis_flash(ALMT_SUMBER, (unsigned short *) pgr, (sizeof (struct t_sumber) * JML_SUMBER))) return -1;
	
	printf(".. OK\r\n");
	return 0;
}
#endif
