/*
	Afrendy Bayu
	13 Juli 2011

*/


#include "FreeRTOS.h"

#ifndef __SHELL_MODBUS_RTU__
#define __SHELL_MODBUS_RTU__

#ifdef PAKAI_MODBUS_RTU

#include "monita/monita_uip.h"
#include "modbus/mbcrc.h"
#include "modbus/low_mod.h"
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


#if 0
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


void cek_coil(unsigned char almx, int regx, int jmlx)	{
	struct d_pmod pmodx;
	unsigned char *stx, *x;	
	char i;

	//printf("  %s() ---> alm: %d, reg: %d, jml: %d\r\n", __FUNCTION__, almx, regx, jmlx);
	modbus_rtu(&x, almx, 0x01, regx, jmlx);
	
	
	#if 0
	stx = (char *) &pmodx;
	for (i=0; i< sizeof(pmodx); i++)	{
		printf("%02hX ", *stx++);
		//serX_putchar(PAKAI_PM, st++, TUNGGU_PM_TX);
	}
	printf("\r\n");

	paket_modbus_rtu(&pm, almx, 0x01, regx, jmlx);

	stx = (char *) &pm;
	for (i=0; i<8; i++)	{
		printf("%02hX ", *stx++);
		//serX_putchar(PAKAI_PM, st++, TUNGGU_PM_TX);
	}
	printf("\r\n");
	#endif

   	return ( 3+ jmlx + 2 );	// slave address, function, bytecount, data, crc
}

void cek_holding(unsigned char almx, int regx, int jmlx)	{
	unsigned char *stx, *x;	
	char i;

	//printf("  %s() ---> alm: %d, reg: %d, jml: %d\r\n", __FUNCTION__, almx, regx, jmlx);
	modbus_rtu(&x, almx, 0x03, regx, jmlx);
	//paket_modbus_rtu(&x, almx, 0x03, regx, jmlx);

	#if 0
	stx = &x;
	for (i=0; i<8; i++)	{
		//#ifdef DEBUG_MODBUS_ELEMEN
		printf("%02x ", stx[i]);
		//#endif
		serX_putchar(PAKAI_PM, stx[i], TUNGGU_PM_TX);
	}
	printf("\r\n");
	#endif
	
	//minta_modbus(&x, jmlx);
	
	//printf("%02x %02x %02x %02x\r\n", *pm, *(pm+1), *(pm+2), *(pm+3));

   	return (3 + jmlx + 2);	// slave address, function, bytecount, data, crc
}

void set_coil(unsigned char almx, int regx, int jmlx)	{
	unsigned char *stx, *x;	
	char i;

	modbus_rtu(&x, almx, 0x05, regx, jmlx);
   	return (3 + jmlx + 2);	// slave address, function, bytecount, data, crc
}


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

#endif
