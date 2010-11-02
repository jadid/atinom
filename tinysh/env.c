/*

	environtment & baca tulisnya
	
	furkan jadid
	daun biru engineering
	
	12 Desember 2008

	18 Maret 2010
		Ubah ke akses pakai pointer kepada memory di flash

*/


//#include "FreeRTOS.h"
//#include "task.h"

#include "enviro.h"
#include "../monita/monita_uip.h"

static int simpan_env( struct t_env *pgr);

static int uname(int argc, char **argv)	{
	int i;
	struct t_env *p_env;
	int sumb;
	//unsigned char str_env[24];
	int y=0;
	
	p_env = (char *) ALMT_ENV;
	p_env = pvPortMalloc(sizeof (struct t_env) );
	
	if (p_env == NULL)	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		return -1;
	}
	
	printf(" Ukuran %d !\r\n", (sizeof (struct t_env)));
	if (argc == 1)	{
		judul(" Lingkungan Setting\r\n");
		printf(" ID  : Nama       : Status : Keterangan : Jum Data : &Memory\r\n");
		garis_bawah();
		printf(" IP : %d.%d.%d.%d\r\n", p_env.IP0,p_env.IP1,p_env.IP2.p_env.IP3);

	}
	
}

//static tinysh_cmd_t uname_cmd={0,"uname","menampilkan konfigurasi env","[] nomer", uname,0,0,0};

static int simpan_env( struct t_env *pgr)
{
	printf(" Save struct GROUP ke flash ..");
	if(prepare_flash(SEKTOR_ENV, SEKTOR_ENV)) return -1;
	printf("..");
	
	if(hapus_flash(SEKTOR_ENV, SEKTOR_ENV)) return -1;
	printf("..");
	
	if(prepare_flash(SEKTOR_ENV, SEKTOR_ENV)) return -1;
	printf("..");
	
	if(tulis_flash(ALMT_ENV, (unsigned short *) pgr, (sizeof (struct t_group)))) return -1;
	
	printf(".. OK\r\n");
	return 0;
}
//*/
