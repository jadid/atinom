/*
	Afrendy Bayu
	13 Juli 2011

*/


#include "FreeRTOS.h"

#ifndef __SHELL_MODBUS_RTU__
#define __SHELL_MODBUS_RTU__

#ifdef PAKAI_MODBUS_RTU

#include "../../app/powermeter/pm.c"

/*
void modbus_kitab(char *x)	{
	//printf("Salah asuhan, arg = 1 !!\r\n");
	if (strcmp(x, "cek_coil")==0)	{
		printf("  cek_coil [alamat] [register] [jml_data]\r\n");
		printf("    contoh : cek_coil 5 100 2\r\n");
		printf("    cek_coil alamat slave 5 pada register 100 sejumlah 2 byte\r\n");
	} else if (strcmp(x, "cek_holding")==0)	{
		printf("  cek_holding [alamat] [register] [jml_data]\r\n");
		printf("    contoh : cek_holding 5 100 2\r\n");
		printf("    cek_holding alamat slave 5 pada register 100 sejumlah 2 byte\r\n");
	} else if (strcmp(x, "set_coil")==0)	{
		printf("  set_coil [alamat] [register] [jml_data]\r\n");
		printf("    contoh : set_coil 5 100 2\r\n");
		printf("    set_coil alamat slave 5 pada register 100 sejumlah 2 byte\r\n");
}
//*/

void modbus_kitab(char *x)	{
	printf("  %s [alamat] [register] [jml_data]\r\n", x);
	printf("    contoh : %s 5 100 2\r\n", x);
	printf("    %s alamat slave 5 pada register 100 sejumlah 2 byte\r\n", x);
}

int cek_coil_sh(int argc, char **argv)	{
	//display_args(argc,argv);
	if ( (argc<4) || (argc>4) )	{	// cek_relay
		modbus_kitab(argv[0]);
		return 0;
	}
	
	int regx, jmlx;
	unsigned char almx;
	almx = (unsigned char) atoi(argv[1]);
	regx = atoi(argv[2]);
	jmlx = atoi(argv[3]);
	
	printf("  Cek coil register modbus\r\n");
	
	if ( (almx==0) || (almx>125) )	{
		printf(">>>>>>> alamat salah. Paksa ke alamat 1\r\n");
		almx = 1;
	}
	if (regx<0)	{
		printf(">>>>>>> Register salah. Paksa ke register 1\r\n");
		regx = 0;
	}
	if (jmlx<1)	{
		printf(">>>>>>> Tipe data salah. Paksa ke tipe 2\r\n");
		jmlx = 2;
	}
	cek_coil(almx, regx, jmlx);
}


static tinysh_cmd_t cek_coil_cmd={0,"cek_coil","menampilkan status coil modbus","[] nomer", cek_coil_sh,0,0,0};

int cek_holding_sh(int argc, char **argv)	{
	//display_args(argc,argv);
	if ( (argc<4) || (argc>4) )	{	// cek_relay
		modbus_kitab(argv[0]);
		return 0;
	}
	
	int regx, jmlx;
	unsigned char almx;
	almx = (unsigned char) atoi(argv[1]);
	regx = atoi(argv[2]);
	jmlx = atoi(argv[3]);
	
	printf("  Cek holding register modbus\r\n");
	if ( (almx==0) || (almx>125) )	{
		printf(">>>>>>> alamat salah. Paksa ke alamat 1\r\n");
		almx = 1;
	}
	if (regx<0)	{
		printf(">>>>>>> Register salah. Paksa ke register 1\r\n");
		regx = 0;
	}
	if (jmlx<1)	{
		printf(">>>>>>> Jml data salah. Paksa ke jmldata 2\r\n");
		jmlx = 2;
	}
	cek_holding(almx, regx, jmlx);
}


static tinysh_cmd_t cek_holding_cmd={0,"cek_holding", \
	"menampilkan status holding modbus","[alamat_slave] [register] [jml_data]", cek_holding_sh,0,0,0};


#ifdef UNTUK_MONITA_KAPAL
	void reset_total_TFX()	{
		set_coil(1, 0, 0xFF00);
		// 0xFF00 : coil ON
		// 0x0000 : coil OFF
	}
#endif

void set_coil_sh(int argc, char **argv) {
	if ( (argc<4) || (argc>4) )	{	// cek_relay
		modbus_kitab(argv[0]);
		return 0;
	}
	
	int regx, jmlx;
	unsigned char almx;
	almx = (unsigned char) atoi(argv[1]);
	regx = atoi(argv[2]);
	jmlx = atoi(argv[3]);
	
	printf("  Cek holding register modbus\r\n");
	if ( (almx==0) || (almx>125) )	{
		printf(">>>>>>> alamat salah. Paksa ke alamat 1\r\n");
		almx = 1;
	}
	if (regx<0)	{
		printf(">>>>>>> Register salah. Paksa ke register 1\r\n");
		regx = 0;
	}
	if (jmlx<1)	{
		printf(">>>>>>> Jml data salah. Paksa ke jmldata 2\r\n");
		jmlx = 2;
	}
	set_coil(almx, regx, jmlx);
}

static tinysh_cmd_t set_coil_cmd={0,"set_coil","setting coil modbus", "help default ",set_coil_sh,0,0,0};

#endif

#endif
