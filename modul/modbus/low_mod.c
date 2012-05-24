/*
 * banyak dicopy dari versi windows
 * yang dikembangkan sejak Meulaboh - NAD
 */

//#define 	JUMLAH_PM		1
//#define		TIPE_PM710

//#define CEK_PM

#include 	"FreeRTOS.h"


#ifdef AMBIL_PM

#include 	"low_mod.h"
#include 	"mbcrc.h"
#include	"monita/monita_uip.h"
//#include 	"mbcrc.c"

//unsigned short jum_PM710_masuk;
unsigned short reg_flag;
//unsigned short urut_PM710;    //urutan pengambilan data
//unsigned short jum_balik;
//extern unsigned char addr_PM710;
//extern unsigned char addr_KTA;

//struct d_PM710 data_PM710[JML_SUMBER];
//struct f_PM710 asli_PM710[JML_SUMBER];
//struct t_kontrol_PM kontrol_PM[JUMLAH_PM];



/*
   query data dari register PM710 & juga dimasukkan ukuran data yang akan diquery
   di query secara periodik untuk mendapatkan data yang diinginkan

   return adalah jumlah byte yang seharusnya diterima
*/
//unsigned char addr_PM710=2;		//2

#ifdef TIPE_PM710
unsigned int get_PM710(int alamatPM, unsigned short reg, unsigned char uk)	{
   unsigned short dcrc;
   int i;

	//addr_PM710=2;
   reg_flag = reg;

   //if (reg != reg_kwh)
   reg = reg - 1;

   //pmod.addr = (unsigned char)   addr_PM710;
   pmod.addr = (unsigned char)   alamatPM;
   pmod.command = (unsigned char) command_baca;
   pmod.reg_hi = (unsigned char) ((reg & 0xFF00) >> 8);
   pmod.reg_lo = (unsigned char) (reg & 0x00FF);
   pmod.jum_hi = (unsigned char) ((uk & 0xFF00) >> 8);
   pmod.jum_lo = (unsigned char) (uk & 0x00FF);

   dcrc = usMBCRC16((unsigned char *) &pmod, sizeof (pmod)-2, 0);
   pmod.crc_lo = (unsigned char) ((dcrc & 0xFF00) >> 8);
   pmod.crc_hi = (unsigned char) (dcrc & 0x00FF);

   	return (1 + 1 + 1 + (uk * 2) + 2);	// slave address, function, bytecount, data, crc
}
#endif

#ifdef TIPE_MICOM_M300
unsigned int get_M300(int alamatPM, unsigned short reg, unsigned char uk)	{
   unsigned short dcrc;
   int i;
	
   reg_flag = reg;

   //if (reg != reg_kwh)
   //reg = reg - 1;

   //pmod.addr = (unsigned char)   addr_PM710;
   pmod.addr = (unsigned char)   alamatPM;
   pmod.command = (unsigned char) command_baca_micom;
   pmod.reg_hi = (unsigned char) ((reg & 0xFF00) >> 8);
   pmod.reg_lo = (unsigned char) (reg & 0x00FF);
   pmod.jum_hi = (unsigned char) ((uk & 0xFF00) >> 8);
   pmod.jum_lo = (unsigned char) (uk & 0x00FF);

   dcrc = usMBCRC16((unsigned char *) &pmod, sizeof (pmod)-2, 0);
   pmod.crc_lo = (unsigned char) ((dcrc & 0xFF00) >> 8);
   pmod.crc_hi = (unsigned char) (dcrc & 0x00FF);

	#ifdef LIAT   
	printf("Kirim: %0.2X %0.2X %0.2X %0.2X %0.2X %0.2X %0.2X %0.2X  \n",
			pmod.addr,
			pmod.command,
			pmod.reg_hi,
			pmod.reg_lo,
			pmod.jum_hi,
			pmod.jum_lo,
			pmod.crc_hi,
			pmod.crc_lo);
	
	#endif		
			
   	return (1 + 1 + 1 + (uk * 2) + 2);	// slave address, function, bytecount, data, crc
}
#endif

#ifdef TIPE_MICOM_P127
unsigned int get_P127(int alamatPM, unsigned short reg, unsigned char uk)	{
   unsigned short dcrc;
   int i;
	
   reg_flag = reg;

   //if (reg != reg_kwh)
   //reg = reg - 1;

   //pmod.addr = (unsigned char)   addr_PM710;
   pmod.addr = (unsigned char)   alamatPM;
   pmod.command = (unsigned char) comand_baca_127;
   pmod.reg_hi = (unsigned char) ((reg & 0xFF00) >> 8);
   pmod.reg_lo = (unsigned char) (reg & 0x00FF);
   pmod.jum_hi = (unsigned char) ((uk & 0xFF00) >> 8);
   pmod.jum_lo = (unsigned char) (uk & 0x00FF);

   dcrc = usMBCRC16((unsigned char *) &pmod, sizeof (pmod)-2, 0);
   pmod.crc_lo = (unsigned char) ((dcrc & 0xFF00) >> 8);
   pmod.crc_hi = (unsigned char) (dcrc & 0x00FF);

	#ifdef LIAT   
	printf("Kirim: %0.2X %0.2X %0.2X %0.2X %0.2X %0.2X %0.2X %0.2X  \n",
			pmod.addr,
			pmod.command,
			pmod.reg_hi,
			pmod.reg_lo,
			pmod.jum_hi,
			pmod.jum_lo,
			pmod.crc_hi,
			pmod.crc_lo);
	
	#endif		
			
   	return (1 + 1 + 1 + (uk * 2) + 2);	// slave address, function, bytecount, data, crc
}
#endif

#ifdef TIPE_ION8600
unsigned int get_ION8600(int alamatPM, unsigned short reg, unsigned char uk)	{
   unsigned short dcrc;
   int i;
	
   reg_flag = reg;

   //if (reg != reg_kwh)
   reg = reg - 1;

   //pmod.addr = (unsigned char)   addr_PM710;
   pmod.addr = (unsigned char)   alamatPM;
   pmod.command = (unsigned char) comand_baca_ION8600;
   pmod.reg_hi = (unsigned char) ((reg & 0xFF00) >> 8);
   pmod.reg_lo = (unsigned char) (reg & 0x00FF);
   pmod.jum_hi = (unsigned char) ((uk & 0xFF00) >> 8);
   pmod.jum_lo = (unsigned char) (uk & 0x00FF);

   dcrc = usMBCRC16((unsigned char *) &pmod, sizeof (pmod)-2, 0);
   pmod.crc_lo = (unsigned char) ((dcrc & 0xFF00) >> 8);
   pmod.crc_hi = (unsigned char) (dcrc & 0x00FF);

	#ifdef LIAT   
	printf("Kirim: %0.2X %0.2X %0.2X %0.2X %0.2X %0.2X %0.2X %0.2X  \n",
			pmod.addr,
			pmod.command,
			pmod.reg_hi,
			pmod.reg_lo,
			pmod.jum_hi,
			pmod.jum_lo,
			pmod.crc_hi,
			pmod.crc_lo);
	
	#endif		
			
   	return (1 + 1 + 1 + (uk * 2) + 2);	// slave address, function, bytecount, data, crc
}
#endif

#ifdef TIPE_A2000
unsigned int get_A2000(int alamatPM, unsigned short reg, unsigned char uk)	{
   unsigned short dcrc;
   int i;
	
	//printf("[%s]: alamatPM: %d, register: %d (0x%0.4X), ukuran: %d\n",__FUNCTION__, alamatPM,reg,reg, uk);
   reg_flag = reg;

   //if (reg != reg_kwh)
   //reg = reg - 1;

   //pmod.addr = (unsigned char)   addr_PM710;
   pmod.addr = (unsigned char)   alamatPM;
   pmod.command = (unsigned char) comand_baca_A2000;
   pmod.reg_hi = (unsigned char) ((reg & 0xFF00) >> 8);
   pmod.reg_lo = (unsigned char) (reg & 0x00FF);
   pmod.jum_hi = (unsigned char) ((uk & 0xFF00) >> 8);
   pmod.jum_lo = (unsigned char) (uk & 0x00FF);

   dcrc = usMBCRC16((unsigned char *) &pmod, sizeof (pmod)-2, 0);
   pmod.crc_lo = (unsigned char) ((dcrc & 0xFF00) >> 8);
   pmod.crc_hi = (unsigned char) (dcrc & 0x00FF);

	#ifdef LIAT   
	printf("Kirim: %0.2X %0.2X %0.2X %0.2X %0.2X %0.2X %0.2X %0.2X  \n",
			pmod.addr,
			pmod.command,
			pmod.reg_hi,
			pmod.reg_lo,
			pmod.jum_hi,
			pmod.jum_lo,
			pmod.crc_hi,
			pmod.crc_lo);
	
	#endif		
			
   	return (1 + 1 + 1 + (uk * 2) + 2);	// slave address, function, bytecount, data, crc
}
#endif

#ifdef TIPE_TFX_ULTRA
unsigned int get_tfx(int alamatPM, unsigned short reg, unsigned char uk)	{
   unsigned short dcrc;
   int i;
	
	#ifdef LIAT
	printf("[%s]: Alamat Modbus: %d, register: %d (0x%0.4X), ukuran: %d\n",__FUNCTION__, alamatPM,reg,reg, uk);
	#endif
	reg_flag = reg;

   //if (reg != reg_kwh)
   reg = reg - 1;

   //pmod.addr = (unsigned char)   addr_PM710;
   pmod.addr = (unsigned char)   alamatPM;
   pmod.command = (unsigned char) comand_baca_tfx;
   pmod.reg_hi = (unsigned char) ((reg & 0xFF00) >> 8);
   pmod.reg_lo = (unsigned char) (reg & 0x00FF);
   pmod.jum_hi = (unsigned char) ((uk & 0xFF00) >> 8);
   pmod.jum_lo = (unsigned char) (uk & 0x00FF);

   dcrc = usMBCRC16((unsigned char *) &pmod, sizeof (pmod)-2, 0);
   pmod.crc_lo = (unsigned char) ((dcrc & 0xFF00) >> 8);
   pmod.crc_hi = (unsigned char) (dcrc & 0x00FF);

	//#define LIAT
	#ifdef LIAT   
	printf("Kirim: %0.2X %0.2X %0.2X %0.2X %0.2X %0.2X %0.2X %0.2X  \n",
			pmod.addr,
			pmod.command,
			pmod.reg_hi,
			pmod.reg_lo,
			pmod.jum_hi,
			pmod.jum_lo,
			pmod.crc_hi,
			pmod.crc_lo);
	
	#endif		
			
   	return (1 + 1 + 1 + (uk * 2) + 2);	// slave address, function, bytecount, data, crc
}
#endif

//#define DEBUG_MODBUS_ELEMEN

void modbus_rtu(unsigned char *q, unsigned char almx, unsigned char cmd, int regx, int jmlx)	{
	unsigned char *stx, *x;
	unsigned char npaket, ww;
	unsigned char tipe = 1;
	
	paket_modbus_rtu(&x, almx, cmd, regx, jmlx);	
	npaket = minta_modbus(&x, jmlx);
	printf("\r\nnPaket: %d\r\n", npaket);
	
	//stx = x;
	//printf("%02x %02x %02x %02x %02x %02x %02x %02x\r\n", \
	//	stx[0], stx[1], stx[2], stx[3], stx[4], stx[5], stx[6], stx[7]);
	//printf("%02x %02x %02x %02x %02x %02x\r\n", \
	//	x[HD+0], x[HD+1], x[HD+2], x[HD+3], x[HD+4], x[HD+5], x[HD+6], x[HD+7]);
	
	#if 1
	if (1)	{
		stx = x;
		//printf("data: %02x %02x %02x %02x %02x %02x\r\n", stx[0], stx[1], stx[2], stx[3], stx[4], stx[5]);
		
		//stx = &x+HD;
		//printf("data: %02x %02x %02x %02x %02x %02x\r\n", stx[0], stx[1], stx[2], stx[3], stx[4], stx[5]);
		
		proses_modbus(&stx, almx, npaket, INTEGER);
	} else {
		printf("data salah\r\n");
	}
	#endif
	/*
	//stx = &x[HD];
	for (ww=0; ww<(npaket-HD); ww++)	{
		printf("%02x ", *stx++);
	}
	printf("\r\n");
	//*/
}

void proses_modbus(unsigned char *d, unsigned char almx, int lenx, unsigned char tipe)	{
	int nint;
	float nfloat;
	
	printf("data: %02x %02x %02x %02x %02x %02x %02x %02x\r\n", \
		d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7]);
	printf("data: %02x %02x %02x %02x %02x %02x %02x %02x\r\n", \
		d[HD+0], d[HD+1], d[HD+2], d[HD+3], d[HD+4], d[HD+5], d[HD+6], d[HD+7]);
	/*
	if (tipe == INTEGER)	{
		printf("data: %02x %02x %02x %02x %02x %02x\r\n", d[0], d[1], d[2], d[3], d[4], d[5]);
		nint = ((0x00FF & d[3])<<8)+(d[4] & 0x00FF);
		printf("Almt: %d, data : %d - %02x %02x\r\n", almx, nint, d[3], d[4]);
	} else {
		printf("tidak terdeteksi !!\r\n");
	}
	//*/
}

void paket_modbus_rtu(unsigned char *x, unsigned char almt, unsigned char cmd, int regx, int jmlx)	{
	#ifdef DEBUG_MODBUS_ELEMEN
		printf("%s() --> almtx: %d, cmd: %d, reg: %d, jml: %d\r\n", __FUNCTION__, almt, cmd, regx, jmlx);
	#endif
	
	struct d_pmod pmodx;
	unsigned short dcrcx;
	unsigned char *y;
	//int lenpmodx = 8;

	pmodx.addr    = (unsigned char) (almt);
	pmodx.command = (unsigned char) cmd;
	pmodx.reg_hi  = (unsigned char) ((regx & 0xFF00) >> 8);
	pmodx.reg_lo  = (unsigned char) (regx & 0x00FF);
	pmodx.jum_hi  = (unsigned char) ((jmlx & 0xFF00) >> 8);
	pmodx.jum_lo  = (unsigned char) (jmlx & 0x00FF);

	dcrcx = usMBCRC16((unsigned char *) &pmodx, sizeof (pmodx)-2, 0);
	pmodx.crc_lo = (unsigned char) ((dcrcx & 0xFF00) >> 8);
	pmodx.crc_hi = (unsigned char) (dcrcx & 0x00FF);
	
	#ifdef DEBUG_MODBUS_ELEMEN
		printf("addr: %02x, cmd: %02x, reg: %02x-%02x, jml: %02x-%02x, crc: %02x-%02x\r\n", \
			pmodx.addr, pmodx.command, pmodx.reg_hi, pmodx.reg_lo, pmodx.jum_hi, pmodx.jum_lo, \
			pmodx.crc_hi, pmodx.crc_lo);
	#endif
	
	y = &pmodx;
	for (dcrcx=0; dcrcx<HD; dcrcx++)	{
		#ifdef DEBUG_MODBUS_ELEMEN
			printf("%02x ", y[dcrcx]);
		#endif
		x[dcrcx] = y[dcrcx];
	}
	//#ifdef DEBUG_MODBUS_ELEMEN
	#if 1
		printf("\r\n");
		printf("X : %02x %02x %02x %02x %02x %02x %02x %02x\r\n", x[0], x[1], x[2], x[3], x[4], x[5], x[6], x[7]);
	#endif
}


unsigned char minta_modbus(unsigned char *data, int tipe)	{
	char ss, timeout=0, retx;
	unsigned char * ww, qq;
	int jml = 3+(tipe)+4;		// slave+cmd+jml+ data +crchi+crclo
	//int lenpmodx = 8;
	unsigned char bufmod[50];
	//printf("len: %d : %d\r\n", sizeof(struct d_pmod), strlen(struct d_pmod));
	
	FIO0SET = TXDE;		// on	---> bisa kirim
	FIO0CLR = RXDE;
	
	//#if 0
	#ifdef DEBUG_MODBUS_ELEMEN
	printf("%s() -->Kirim modbus ..\r\n", __FUNCTION__);

	printf("Data : %02x %02x %02x %02x %02x %02x %02x %02x\r\n", \
		data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
	ww = data;
	printf("ww : %02x %02x %02x %02x %02x %02x %02x %02x\r\n", \
		ww[0], ww[1], ww[2], ww[3], ww[4], ww[5], ww[6], ww[7]);
	#endif
	
	ww = data;
	for (ss=0; ss<HD; ss++)	{
		#ifdef DEBUG_MODBUS_ELEMEN
			printf("%02x ", *ww);
		#endif
		serX_putchar(PAKAI_PM, ww++, TUNGGU_PM_TX);
	}
	#ifdef DEBUG_MODBUS_ELEMEN
	printf("\r\n");
	#endif

	ss=0;
	#ifdef DEBUG_MODBUS_ELEMEN
	printf("terima ..\r\n");
	#endif
	while(1)	{
		#if (PAKAI_PM == 1) 
			if (ser1_getchar(1, &bufmod[ss], TUNGGU_PM_RX ) == pdTRUE)
		#elif (PAKAI_PM == 2) 			
			if (ser2_getchar(1, &bufmod[ss], TUNGGU_PM_RX ) == pdTRUE)
		#elif (PAKAI_PM == 3)
			if (ser3_getchar(1, &bufmod[ss], TUNGGU_PM_RX ) == pdTRUE)
		#endif 

		{
			//#ifdef DEBUG_MODBUS_ELEMEN
			printf("%02hX ",bufmod[ss]);
			//#endif
	
			ss++;
			retx = 1;
			#ifdef PAKAI_MAX485
				if (ss == jml+HD) break;
			#else
				if (ss == jml) break;
			#endif

		}	else	{
			timeout++;
			if (timeout > 10)	{
				#if defined (LIAT_RX) || defined(TIMEOUT)
				printf("%s(): alamat %d : timeout: %d\r\n", __FUNCTION__, alamatPM, urut_PM710);
				#endif
				
				retx=0;
				break;
			}
		}
		
	}
	
	#ifdef DEBUG_MODBUS_ELEMEN
	printf("\r\n -----> ada %d\r\n", ss);
	#endif
	
	for (qq=0; qq<ss; qq++)	{
		#ifdef DEBUG_MODBUS_ELEMEN
			printf("%02x ", bufmod[qq]);
		#endif
		data[qq] = bufmod[qq];
	}

	return ss;
}

unsigned short cek_PM(int alamatPM) {
	unsigned short dcrc;
   	unsigned char *p;
   	int i;

	//reg_flag = reg;

	pmod.addr = (unsigned char)   alamatPM;
	pmod.command = (unsigned char) 0x03;		// command untuk cek ID
	pmod.reg_hi = (unsigned char) 0x40;
	pmod.reg_lo = (unsigned char) 0x00;
	pmod.jum_hi = (unsigned char) ((5 & 0xFF00) >> 8);
	pmod.jum_lo = (unsigned char) (5 & 0x00FF);

	dcrc = usMBCRC16((unsigned char *) &pmod, sizeof (pmod)-2, 0);
	pmod.crc_lo = (unsigned char) ((dcrc & 0xFF00) >> 8);
	pmod.crc_hi = (unsigned char) (dcrc & 0x00FF);

	return 7;	// respon harusnya 7 byte
	//return (1 + 1 + 1 + (uk * 2) + 2);		
}

#if 0


//*
// 22 Jan 09
#ifdef PAKAI_KTA
unsigned short get_KTA(unsigned short reg, unsigned char uk)
{
   unsigned short dcrc;
   int i;

   reg_flag = reg;

   if (reg != reg_kwh)
   reg = reg - 1;

   pmod.addr = (unsigned char)   addr_KTA;
   pmod.command = (unsigned char) command_baca;
   pmod.reg_hi = (unsigned char) ((reg & 0xFF00) >> 8);
   pmod.reg_lo = (unsigned char) (reg & 0x00FF);
   pmod.jum_hi = (unsigned char) ((uk & 0xFF00) >> 8);
   pmod.jum_lo = (unsigned char) (uk & 0x00FF);

   dcrc = usMBCRC16((unsigned char *) &pmod, sizeof (pmod)-2);
   pmod.crc_lo = (unsigned char) ((dcrc & 0xFF00) >> 8);
   pmod.crc_hi = (unsigned char) (dcrc & 0x00FF);

   	return (1 + 1 + 1 + (uk * 2) + 2);
}
#endif

//*/
#endif


#ifdef TIPE_PM710
void taruh_data_710(int no_slave, int urt)	{
	short satuan_t;
	int i;
	/*
	for (i=0; i<255; i++)	{
		buf[i] = buf[i] & 0x00FF;
	} 
	//*/
	
	if (urt == 0)  {   //satuan2
		// amper
		satuan_t = buf[3+HD];
		satuan_t = (satuan_t << 8) | buf[4+HD];

		if (satuan_t == (short) -4) satuan_amp[no_slave] = 0.0001;
		else if (satuan_t == -3) satuan_amp[no_slave] = 0.001;
		else if (satuan_t == -2) satuan_amp[no_slave] = 0.01;
		else if (satuan_t == -1) satuan_amp[no_slave] = 0.1;
		else if (satuan_t == 0) satuan_amp[no_slave] = 1.0;
		else if (satuan_t == 1) satuan_amp[no_slave] = 10.0;
		else if (satuan_t == 2) satuan_amp[no_slave] = 100.0;
		else if (satuan_t == 3) satuan_amp[no_slave] = 1000.0;
		else if (satuan_t == 4) satuan_amp[no_slave] = 10000.0;
		
		#ifdef CEK_PM
		printf("satuan t: %d, satuan amp: %f\r\n", satuan_t, satuan_amp);
		#endif
        //printw("sat amp  : %d : %X", satuan_t, (short) satuan_t);

		//volt
		satuan_t = buf[5+HD];
		satuan_t = (satuan_t << 8) | buf[6+HD];

		if (satuan_t == -4) satuan_volt[no_slave] = 0.0001;
		else if (satuan_t == -3) satuan_volt[no_slave] = 0.001;
		else if (satuan_t == -2) satuan_volt[no_slave] = 0.01;
		else if (satuan_t == -1) satuan_volt[no_slave] = 0.1;
		else if (satuan_t == 0)  satuan_volt[no_slave] = 1.0;
		else if (satuan_t == 1)  satuan_volt[no_slave] = 10.0;
		else if (satuan_t == 2)  satuan_volt[no_slave] = 100.0;
		else if (satuan_t == 3)  satuan_volt[no_slave] = 1000.0;
		else if (satuan_t == 4)  satuan_volt[no_slave] = 10000.0;
		#ifdef CEK_PM
		printf("satuan t: %d, satuan volt: %f\r\n", satuan_t, satuan_volt);
		#endif
		//power
		satuan_t = buf[7+HD];
		satuan_t = (satuan_t << 8) | buf[8+HD];

		if (satuan_t == -4) satuan_kw[no_slave] = 0.0001;
		else if (satuan_t == -3) satuan_kw[no_slave] = 0.001;
		else if (satuan_t == -2) satuan_kw[no_slave] = 0.01;
		else if (satuan_t == -1) satuan_kw[no_slave] = 0.1;
		else if (satuan_t == 0)  satuan_kw[no_slave] = 1.0;
		else if (satuan_t == 1)  satuan_kw[no_slave] = 10.0;
		else if (satuan_t == 2)  satuan_kw[no_slave] = 100.0;
		else if (satuan_t == 3)  satuan_kw[no_slave] = 1000.0;
		else if (satuan_t == 4)  satuan_kw[no_slave] = 10000.0;
		#ifdef CEK_PM
		printf("satuan t: %d, satuan kw: %f\r\n", satuan_t, satuan_kw);
		#endif
		//energy
		satuan_t = buf[9+HD];
		satuan_t = (satuan_t << 8) | buf[10+HD];

		if (satuan_t == -4) satuan_kwh[no_slave] = 0.0001;
		else if (satuan_t == -3) satuan_kwh[no_slave] = 0.001;
		else if (satuan_t == -2) satuan_kwh[no_slave] = 0.01;
		else if (satuan_t == -1) satuan_kwh[no_slave] = 0.1;
		else if (satuan_t == 0) satuan_kwh[no_slave] = 1.0;
		else if (satuan_t == 1) satuan_kwh[no_slave] = 10.0;
		else if (satuan_t == 2) satuan_kwh[no_slave] = 100.0;
		else if (satuan_t == 3) satuan_kwh[no_slave] = 1000.0;
		else if (satuan_t == 4) satuan_kwh[no_slave] = 10000.0;
		#ifdef CEK_PM
		printf("satuan t: %d, satuan kwh: %f\r\n", satuan_t, satuan_kwh);
		#endif
		//urut_PM710 = 0;
	}
	else if (urt == 1) 	{		// kva, kvar, PF, volt1, volt2, amper
		data_PM710[no_slave].kva = buf[3+HD];
		data_PM710[no_slave].kva = (data_PM710[no_slave].kva << 8) | buf[4+HD];
		asli_PM710[no_slave].kva = data_PM710[no_slave].kva * satuan_kw[no_slave];

		data_PM710[no_slave].kvar = buf[5+HD];
		data_PM710[no_slave].kvar = (data_PM710[no_slave].kvar << 8) | buf[6+HD];
		asli_PM710[no_slave].kvar = (signed) data_PM710[no_slave].kvar * satuan_kw[no_slave];
		#ifdef CEK_PM
		printf("kvar asli: %.2f, kvar data: %d\r\n", asli_PM710[no_slave].kvar, (signed) data_PM710[no_slave].kvar);
		#endif
		data_PM710[no_slave].pf = buf[7+HD];
		data_PM710[no_slave].pf = (data_PM710[no_slave].pf << 8) | buf[8+HD];
		asli_PM710[no_slave].pf = data_PM710[no_slave].pf * 0.0001;

		data_PM710[no_slave].volt1 = buf[9+HD];
		data_PM710[no_slave].volt1 = (data_PM710[no_slave].volt1 << 8) | buf[10+HD];
		if (data_PM710[no_slave].volt1 == 32768)
			asli_PM710[no_slave].volt1 = 0;
		else
			asli_PM710[no_slave].volt1 = data_PM710[no_slave].volt1 * satuan_volt[no_slave];
		#ifdef CEK_PM
		printf("volt1 asli: %.2f, volt1 data: %d\r\n", asli_PM710[no_slave].volt1, (signed) data_PM710[no_slave].volt1);
		#endif
		
		data_PM710[no_slave].volt2 = buf[11+HD];
		data_PM710[no_slave].volt2 = (data_PM710[no_slave].volt2 << 8) | buf[12+HD];
		if (data_PM710[no_slave].volt2 == 32768)
			asli_PM710[no_slave].volt2 = 0;
		else
			asli_PM710[no_slave].volt2 = data_PM710[no_slave].volt2 * satuan_volt[no_slave];
		#ifdef CEK_PM
		printf("volt2 asli: %.2f, volt2 data: %d\r\n", asli_PM710[no_slave].volt2, (signed) data_PM710[no_slave].volt2);
		#endif

		data_PM710[no_slave].amp = buf[13+HD];
		data_PM710[no_slave].amp = (data_PM710[no_slave].amp << 8) | buf[14+HD];
		if (data_PM710[no_slave].amp  == 32768)
			asli_PM710[no_slave].amp = 0;
		else
			asli_PM710[no_slave].amp = data_PM710[no_slave].amp * satuan_amp[no_slave];
		#ifdef CEK_PM
		printf("amp asli: %.2f, data: %d\r\n", asli_PM710[no_slave].amp, (signed) data_PM710[no_slave].amp);
		#endif

		data_PM710[no_slave].frek = buf[15+HD];
		data_PM710[no_slave].frek = (data_PM710[no_slave].frek << 8) | buf[16+HD];
		if (data_PM710[no_slave].frek == 32768)
			asli_PM710[no_slave].frek = 0;
		else
			asli_PM710[no_slave].frek = data_PM710[no_slave].frek * 0.01;
		#ifdef CEK_PM
		printf("frek asli: %.2f, volt1 data: %d\r\n", asli_PM710[no_slave].frek, (signed) data_PM710[no_slave].frek);
		#endif

		//urut_PM710 = 2;
	}
	else if (urt == 2)	{		// ampA, B, C & N
		data_PM710[no_slave].ampA = buf[3+HD];
		data_PM710[no_slave].ampA = (data_PM710[no_slave].ampA << 8) | buf[4+HD];
		if (data_PM710[no_slave].ampA == 32768)
			asli_PM710[no_slave].ampA = 0;
		else
			asli_PM710[no_slave].ampA = data_PM710[no_slave].ampA * satuan_amp[no_slave];
		#ifdef CEK_PM
		printf("ampA asli: %.2f, ampA data: %d\r\n", asli_PM710[no_slave].ampA, (signed) data_PM710[no_slave].ampA);
		#endif
		
		data_PM710[no_slave].ampB = buf[5+HD];
		data_PM710[no_slave].ampB = (data_PM710[no_slave].ampB << 8) | buf[6+HD];
		if (data_PM710[no_slave].ampB == 32768)
			asli_PM710[no_slave].ampB = 0;
		else
			asli_PM710[no_slave].ampB = data_PM710[no_slave].ampB * satuan_amp[no_slave];
		#ifdef CEK_PM
		printf("ampB asli: %.2f, ampB data: %d\r\n", asli_PM710[no_slave].ampB, (signed) data_PM710[no_slave].ampB);
		#endif
		data_PM710[no_slave].ampC = buf[7+HD];
		data_PM710[no_slave].ampC = (data_PM710[no_slave].ampC << 8) | buf[8+HD];
		if (data_PM710[no_slave].ampC == 32768)
			asli_PM710[no_slave].ampC = 0;
		else
			asli_PM710[no_slave].ampC = data_PM710[no_slave].ampC * satuan_amp[no_slave];
		#ifdef CEK_PM
		printf("ampC asli: %.2f, ampC data: %d\r\n", asli_PM710[no_slave].ampC, (signed) data_PM710[no_slave].ampC);
		#endif
		
		data_PM710[no_slave].ampN = buf[9+HD];
		data_PM710[no_slave].ampN = (data_PM710[no_slave].ampN << 8) | buf[10+HD];
		if (data_PM710[no_slave].ampN == 32768)
			asli_PM710[no_slave].ampN = 0;
		else
			asli_PM710[no_slave].ampN = data_PM710[no_slave].ampN * satuan_amp[no_slave];
		#ifdef CEK_PM
		printf("ampN asli: %.2f, ampN data: %d\r\n", asli_PM710[no_slave].ampN, (signed) data_PM710[no_slave].ampN);
		#endif
		//urut_PM710 = 3;
	}
	else if (urt == 3)	{		// voltA_B, B_C, A_C, A_N, B_N, C_N
		//voltA_B, B_C, A_C  
		data_PM710[no_slave].voltA_B = buf[3+HD];
		data_PM710[no_slave].voltA_B = (data_PM710[no_slave].voltA_B << 8) | buf[4+HD];
		if (data_PM710[no_slave].voltA_B == 32768)
			asli_PM710[no_slave].voltA_B = 0;
		else
			asli_PM710[no_slave].voltA_B = data_PM710[no_slave].voltA_B * satuan_volt[no_slave];

		data_PM710[no_slave].voltB_C = buf[5+HD];
		data_PM710[no_slave].voltB_C = (data_PM710[no_slave].voltB_C << 8) | buf[6+HD];
		if (data_PM710[no_slave].voltB_C == 32768)
			asli_PM710[no_slave].voltB_C = 0;
		else
			asli_PM710[no_slave].voltB_C = data_PM710[no_slave].voltB_C * satuan_volt[no_slave];

		data_PM710[no_slave].voltA_C = buf[7+HD];
		data_PM710[no_slave].voltA_C = (data_PM710[no_slave].voltA_C << 8) | buf[8+HD];
		if (data_PM710[no_slave].voltA_C == 32768)
			asli_PM710[no_slave].voltA_C = 0;
		else
			asli_PM710[no_slave].voltA_C = data_PM710[no_slave].voltA_C * satuan_volt[no_slave];

		// Volt A_N, B_N & C_N
		data_PM710[no_slave].voltA_N = buf[9+HD];
		data_PM710[no_slave].voltA_N = (data_PM710[no_slave].voltA_N << 8) | buf[10+HD];
		if (data_PM710[no_slave].voltA_N == 32768)
			asli_PM710[no_slave].voltA_N = 0;
		else
			asli_PM710[no_slave].voltA_N = data_PM710[no_slave].voltA_N * satuan_volt[no_slave];

		data_PM710[no_slave].voltB_N = buf[11+HD];
		data_PM710[no_slave].voltB_N = (data_PM710[no_slave].voltB_N << 8) | buf[12+HD];
		if (data_PM710[no_slave].voltB_N == 32768)
			asli_PM710[no_slave].voltB_N = 0;
		else
			asli_PM710[no_slave].voltB_N = data_PM710[no_slave].voltB_N * satuan_volt[no_slave];

		data_PM710[no_slave].voltC_N = buf[13+HD];
		data_PM710[no_slave].voltC_N = (data_PM710[no_slave].voltC_N << 8) | buf[14+HD];
		if (data_PM710[no_slave].voltC_N == 32768)
			asli_PM710[no_slave].voltC_N = 0;
		else
			asli_PM710[no_slave].voltC_N = data_PM710[no_slave].voltC_N * satuan_volt[no_slave];
		//urut_PM710 = 3;
	}
	else if (urt == 4)	{		// kwh, kvah, kvarh, kw
		//memcpy((char *) &asli_PM710[no_slave].frek, &buf[2], 4);
		
		// kWh
		data_PM710[no_slave].kwh = buf[5+HD];
		data_PM710[no_slave].kwh = (data_PM710[no_slave].kwh << 8) | buf[6+HD];
		data_PM710[no_slave].kwh = (data_PM710[no_slave].kwh << 8) | buf[3+HD];
		data_PM710[no_slave].kwh = (data_PM710[no_slave].kwh << 8) | buf[4+HD];
		asli_PM710[no_slave].kwh = data_PM710[no_slave].kwh * satuan_kwh[no_slave];
		printf("____________-kWhnya: %.2f, data: %d, satuan: %f\r\n", asli_PM710[no_slave].kwh, data_PM710[no_slave].kwh, satuan_kwh);
		
		// kVAh
		data_PM710[no_slave].kvah = buf[9+HD];
		data_PM710[no_slave].kvah  = (data_PM710[no_slave].kvah  << 8) | buf[10+HD];
		data_PM710[no_slave].kvah  = (data_PM710[no_slave].kvah  << 8) | buf[7+HD];
		data_PM710[no_slave].kvah  = (data_PM710[no_slave].kvah  << 8) | buf[8+HD];
		asli_PM710[no_slave].kvah = data_PM710[no_slave].kvah * satuan_kwh[no_slave];

		// kVArh
		data_PM710[no_slave].kvarh = buf[11+HD];
		data_PM710[no_slave].kvarh  = (data_PM710[no_slave].kvarh  << 8) | buf[12+HD];
		if (data_PM710[no_slave].kvarh  == 32768)
			asli_PM710[no_slave].kvarh = 0;
		else
			asli_PM710[no_slave].kvarh = data_PM710[no_slave].kvarh * satuan_kwh[no_slave];

		// kW, ini harusnya salah posisi
		data_PM710[no_slave].kw = buf[13+HD];
		data_PM710[no_slave].kw  = (data_PM710[no_slave].kw  << 8) | buf[14+HD];
		if (data_PM710[no_slave].kw  == 32768)
			asli_PM710[no_slave].kw = 0;
		else
			asli_PM710[no_slave].kw = data_PM710[no_slave].kw * satuan_kw[no_slave];

		//urut_PM710 = 4;
	}
	else if (urt == 5)	{		// kW_A, kW_B, kW_C
		// kW_A, kW_B, kW_C
		data_PM710[no_slave].kwA = buf[3+HD];
		data_PM710[no_slave].kwA  = (data_PM710[no_slave].kwA  << 8) | buf[4+HD];
		if (data_PM710[no_slave].kwA  == 32768)
			asli_PM710[no_slave].kwA = 0;
		else
			asli_PM710[no_slave].kwA = data_PM710[no_slave].kwA * satuan_kw[no_slave];
			
		data_PM710[no_slave].kwB = buf[5+HD];
		data_PM710[no_slave].kwB  = (data_PM710[no_slave].kwB  << 8) | buf[6+HD];
		if (data_PM710[no_slave].kwB  == 32768)
			asli_PM710[no_slave].kwB = 0;
		else
			asli_PM710[no_slave].kwB = data_PM710[no_slave].kwB * satuan_kw[no_slave];
			
		data_PM710[no_slave].kwC = buf[7+HD];
		data_PM710[no_slave].kwC  = (data_PM710[no_slave].kwC  << 8) | buf[8+HD];
		if (data_PM710[no_slave].kwC  == 32768)
			asli_PM710[no_slave].kwC = 0;
		else
			asli_PM710[no_slave].kwC = data_PM710[no_slave].kwC * satuan_kw[no_slave];
		
		// kVA_A, kVA_B, kVA_C
		data_PM710[no_slave].kvaA = buf[9+HD];
		data_PM710[no_slave].kvaA  = (data_PM710[no_slave].kvaA  << 8) | buf[10+HD];
		if (data_PM710[no_slave].kvaA  == 32768)
			asli_PM710[no_slave].kvaA = 0;
		else
			asli_PM710[no_slave].kvaA = data_PM710[no_slave].kvaA * satuan_kw[no_slave];
			
		data_PM710[no_slave].kvaB = buf[11+HD];
		data_PM710[no_slave].kvaB  = (data_PM710[no_slave].kvaB  << 8) | buf[12+HD];
		if (data_PM710[no_slave].kvaB  == 32768)
			asli_PM710[no_slave].kvaB = 0;
		else
			asli_PM710[no_slave].kvaB = data_PM710[no_slave].kvaB * satuan_kw[no_slave];
			
		data_PM710[no_slave].kvaC = buf[13+HD];
		data_PM710[no_slave].kvaC  = (data_PM710[no_slave].kvaC  << 8) | buf[14+HD];
		if (data_PM710[no_slave].kvaC  == 32768)
			asli_PM710[no_slave].kvaC = 0;
		else
			asli_PM710[no_slave].kvaC = data_PM710[no_slave].kvaC * satuan_kw[no_slave];
			
		// kVA_A, kVA_B, kVA_C
		data_PM710[no_slave].kvarA = buf[15+HD];
		data_PM710[no_slave].kvarA  = (data_PM710[no_slave].kvarA  << 8) | buf[16+HD];
		if (data_PM710[no_slave].kvarA  == 32768)
			asli_PM710[no_slave].kvarA = 0;
		else
			asli_PM710[no_slave].kvarA = data_PM710[no_slave].kvarA * satuan_kw[no_slave];
			
		data_PM710[no_slave].kvarB = buf[17+HD];
		data_PM710[no_slave].kvarB  = (data_PM710[no_slave].kvarB  << 8) | buf[18+HD];
		if (data_PM710[no_slave].kvarB  == 32768)
			asli_PM710[no_slave].kvarB = 0;
		else
			asli_PM710[no_slave].kvarB = data_PM710[no_slave].kvarB * satuan_kw[no_slave];
			
		data_PM710[no_slave].kvarC = buf[19+HD];
		data_PM710[no_slave].kvarC  = (data_PM710[no_slave].kvarC  << 8) | buf[20+HD];
		if (data_PM710[no_slave].kvarC  == 32768)
			asli_PM710[no_slave].kvarC = 0;
		else
			asli_PM710[no_slave].kvarC = data_PM710[no_slave].kvarC * satuan_kw[no_slave];
		//urut_PM710 = 5;
	}
/*
	else if (urt == 5)
	{
		// data angin
		wind_speed = buf[3+HD];
		wind_speed = (wind_speed << 8) | buf[4+HD];
		
		// ubah ke m/s dari MPH //
		f_wind_speed = wind_speed * 0.44704;
		
		wind_dir = buf[5+HD];
		wind_dir = (wind_dir << 8) | buf[6+HD];	
		
		asli_PM710[no_slave].voltB_C = f_wind_speed;
		asli_PM710[no_slave].voltA_C = wind_dir;
	}
	else if (urt == 6)
	{
		// satuan angin
		wind_satuan = buf[3+HD];
		wind_satuan = (wind_satuan << 8) | buf[4+HD];
		
		// retransmision value
		wind_speed_tr = buf[9+HD];
		wind_speed_tr = (wind_speed_tr << 8) | buf[10+HD];
		
		wind_dir_tr = buf[11+HD];
		wind_dir_tr = (wind_dir_tr << 8) | buf[12+HD];
		
		//asli_PM710[no_slave].voltB_C = wind_satuan;
	}
//*/
}
#endif


#ifdef TIPE_PM810
	void taruh_data_810(int pm_dibaca, int urt) {
		short satuan_t;
		int i;
		unsigned int temp;
		unsigned int temp2;
		if (urt == 0)	{
			// amper
			satuan_t = buf[3+HD];
			satuan_t = (satuan_t << 8) + buf[4+HD];

			if 		(satuan_t == -4) satuan_amp[pm_dibaca] = 0.0001;
			else if (satuan_t == -3) satuan_amp[pm_dibaca] = 0.001;
			else if (satuan_t == -2) satuan_amp[pm_dibaca] = 0.01;
			else if (satuan_t == -1) satuan_amp[pm_dibaca] = 0.1;
			else if (satuan_t == 0) satuan_amp[pm_dibaca] = 1.0;
			else if (satuan_t == 1) satuan_amp[pm_dibaca] = 10.0;
			else if (satuan_t == 2) satuan_amp[pm_dibaca] = 100.0;
			else if (satuan_t == 3) satuan_amp[pm_dibaca] = 1000.0;
			else if (satuan_t == 4) satuan_amp[pm_dibaca] = 10000.0;
			#ifdef CEK_PM
			printf("__sat amp : %d_____%f\r\n", satuan_t, satuan_amp[pm_dibaca]);
			#endif
			//ampere2 (L-N)
			satuan_t = buf[5+HD];
			satuan_t = (satuan_t << 8) + buf[6+HD];

			if (satuan_t == -4) satuan_amp2[pm_dibaca] = 0.0001;
			else if (satuan_t == -3) satuan_amp2[pm_dibaca] = 0.001;
			else if (satuan_t == -2) satuan_amp2[pm_dibaca] = 0.01;
			else if (satuan_t == -1) satuan_amp2[pm_dibaca] = 0.1;
			else if (satuan_t == 0) satuan_amp2[pm_dibaca] = 1.0;
			else if (satuan_t == 1) satuan_amp2[pm_dibaca] = 10.0;
			else if (satuan_t == 2) satuan_amp2[pm_dibaca] = 100.0;
			else if (satuan_t == 3) satuan_amp2[pm_dibaca] = 1000.0;
			else if (satuan_t == 4) satuan_amp2[pm_dibaca] = 10000.0;
			#ifdef CEK_PM
			printf("__sat amp2 : %d_____%f\r\n", satuan_t, satuan_amp2[pm_dibaca]);
			#endif
			
			// buf[7] & [8] kosong

			// volt (L - L)
			satuan_t = buf[7+HD];
			satuan_t = (satuan_t << 8) + buf[8+HD];

			if (satuan_t == -4) satuan_volt[pm_dibaca] = 0.0001;
			else if (satuan_t == -3) satuan_volt[pm_dibaca] = 0.001;
			else if (satuan_t == -2) satuan_volt[pm_dibaca] = 0.01;
			else if (satuan_t == -1) satuan_volt[pm_dibaca] = 0.1;
			else if (satuan_t == 0) satuan_volt[pm_dibaca] = 1.0;
			else if (satuan_t == 1) satuan_volt[pm_dibaca] = 10.0;
			else if (satuan_t == 2) satuan_volt[pm_dibaca] = 100.0;
			else if (satuan_t == 3) satuan_volt[pm_dibaca] = 1000.0;
			else if (satuan_t == 4) satuan_volt[pm_dibaca] = 10000.0;
			#ifdef CEK_PM
			printf("__satuan_volt : %d_____%f\r\n", satuan_t, satuan_volt[pm_dibaca]);
			#endif
			
			// volt2 (L - N)
			satuan_t = buf[9+HD];
			satuan_t = (satuan_t << 8) + buf[10+HD];

			if (satuan_t == -4) satuan_volt2[pm_dibaca] = 0.0001;
			else if (satuan_t == -3) satuan_volt2[pm_dibaca] = 0.001;
			else if (satuan_t == -2) satuan_volt2[pm_dibaca] = 0.01;
			else if (satuan_t == -1) satuan_volt2[pm_dibaca] = 0.1;
			else if (satuan_t == 0) satuan_volt2[pm_dibaca] = 1.0;
			else if (satuan_t == 1) satuan_volt2[pm_dibaca] = 10.0;
			else if (satuan_t == 2) satuan_volt2[pm_dibaca] = 100.0;
			else if (satuan_t == 3) satuan_volt2[pm_dibaca] = 1000.0;
			else if (satuan_t == 4) satuan_volt2[pm_dibaca] = 10000.0;
			#ifdef CEK_PM
			printf("__sat volt2 : %d_____%f\r\n", satuan_t, satuan_volt2[pm_dibaca]);
			#endif
			//power
			satuan_t = buf[11+HD];
			satuan_t = (satuan_t << 8) + buf[12+HD];

			if (satuan_t == -4) satuan_kw[pm_dibaca] = 0.0001;
			else if (satuan_t == -3) satuan_kw[pm_dibaca] = 0.001;
			else if (satuan_t == -2) satuan_kw[pm_dibaca] = 0.01;
			else if (satuan_t == -1) satuan_kw[pm_dibaca] = 0.1;
			else if (satuan_t == 0) satuan_kw[pm_dibaca] = 1.0;
			else if (satuan_t == 1) satuan_kw[pm_dibaca] = 10.0;
			else if (satuan_t == 2) satuan_kw[pm_dibaca] = 100.0;
			else if (satuan_t == 3) satuan_kw[pm_dibaca] = 1000.0;
			else if (satuan_t == 4) satuan_kw[pm_dibaca] = 10000.0;
			#ifdef CEK_PM
			printf("__satuan_kw: %d_____%f\r\n", satuan_t, satuan_kw[pm_dibaca]);
			#endif
			//energy
			satuan_kwh[pm_dibaca] = 10.00;	
			//*
			#ifdef LIAT 
			printf("pm_dibaca: %d, amp: %.4f, amp2: %.4f, volt: %.4f, volt2: %.4f, kw: %f\r\n", \
				pm_dibaca, satuan_amp[pm_dibaca], satuan_amp2[pm_dibaca], satuan_volt[pm_dibaca], \
				satuan_volt2[pm_dibaca], satuan_kw[pm_dibaca]);
			#endif
			//*/
		}	
		else if (urt == 1)		{	//xx ampA, ampB, ampC, amp
			// current_metering
			data_PM710[pm_dibaca].ampA = buf[3+HD];
			data_PM710[pm_dibaca].ampA = (data_PM710[pm_dibaca].ampA << 8) + buf[4+HD];		
			if (data_PM710[pm_dibaca].ampA == 32768)
				asli_PM710[pm_dibaca].ampA = 0;
			else
				asli_PM710[pm_dibaca].ampA = data_PM710[pm_dibaca].ampA * satuan_amp[pm_dibaca];
			#ifdef CEK_PM
			printf("ampA: %d - %.2f, satuan amp: %f\r\n", data_PM710[pm_dibaca].ampA, asli_PM710[pm_dibaca].ampA, satuan_amp[pm_dibaca]);
			#endif
			
			// ampere B
			data_PM710[pm_dibaca].ampB = buf[5+HD];
			data_PM710[pm_dibaca].ampB = (data_PM710[pm_dibaca].ampB  << 8) + buf[6+HD];
			if (data_PM710[pm_dibaca].ampB == 32768)
				asli_PM710[pm_dibaca].ampB = 0;
			else
				asli_PM710[pm_dibaca].ampB = data_PM710[pm_dibaca].ampB * satuan_amp[pm_dibaca];
			#ifdef CEK_PM
			printf("ampB: %d - %.2f\r\n", data_PM710[pm_dibaca].ampB, asli_PM710[pm_dibaca].ampB);
			#endif
			// ampere C
			data_PM710[pm_dibaca].ampC = buf[7+HD];
			data_PM710[pm_dibaca].ampC = (data_PM710[pm_dibaca].ampC << 8) + buf[8+HD];
			if (data_PM710[pm_dibaca].ampC == 32768)
				asli_PM710[pm_dibaca].ampC = 0;
			else
				asli_PM710[pm_dibaca].ampC = data_PM710[pm_dibaca].ampC * satuan_amp[pm_dibaca];
			#ifdef CEK_PM
			printf("ampC: %d - %.2f\r\n", data_PM710[pm_dibaca].ampC, asli_PM710[pm_dibaca].ampC);
			#endif
			// ampere N
			data_PM710[pm_dibaca].ampN = buf[9+HD];
			data_PM710[pm_dibaca].ampN = (data_PM710[pm_dibaca].ampN << 8) + buf[10+HD];
			if (data_PM710[pm_dibaca].ampN == 32768)
				asli_PM710[pm_dibaca].ampN = 0;
			else
				asli_PM710[pm_dibaca].ampN = data_PM710[pm_dibaca].ampN * satuan_amp[pm_dibaca];
			#ifdef CEK_PM
			printf("ampN: %d - %.2f\r\n", data_PM710[pm_dibaca].ampN, asli_PM710[pm_dibaca].ampN);
			#endif
			// ampere average
			data_PM710[pm_dibaca].amp = buf[11+HD];
			data_PM710[pm_dibaca].amp = (data_PM710[pm_dibaca].amp << 8) + buf[12+HD];
			if (data_PM710[pm_dibaca].amp == 32768)
				asli_PM710[pm_dibaca].amp = 0;
			else
				asli_PM710[pm_dibaca].amp = data_PM710[pm_dibaca].amp * satuan_amp[pm_dibaca];
			#ifdef CEK_PM
			printf("amp : %d - %.2f\r\n", data_PM710[pm_dibaca].amp, asli_PM710[pm_dibaca].amp);
			#endif
		 }
		else if (urt == 2)		{	//xx voltA_B, B_C, A_C, A_N, B_N & C_N
			//voltA_B, B_C, A_C, A_N, B_N & C_N
			data_PM710[pm_dibaca].voltA_B = buf[3+HD];
			data_PM710[pm_dibaca].voltA_B = (data_PM710[pm_dibaca].voltA_B << 8) + buf[4+HD];
			if (data_PM710[pm_dibaca].voltA_B == 32768)
				asli_PM710[pm_dibaca].voltA_B = 0;
			else
				asli_PM710[pm_dibaca].voltA_B = data_PM710[pm_dibaca].voltA_B * satuan_volt[pm_dibaca];
			#ifdef CEK_PM
			printf("voltA_B: %d - %.2f\r\n", data_PM710[pm_dibaca].voltA_B, asli_PM710[pm_dibaca].voltA_B);
			#endif
			data_PM710[pm_dibaca].voltB_C = buf[5+HD];
			data_PM710[pm_dibaca].voltB_C = (data_PM710[pm_dibaca].voltB_C << 8) + buf[6+HD];
			if (data_PM710[pm_dibaca].voltB_C == 32768)
				asli_PM710[pm_dibaca].voltB_C = 0;
			else
				asli_PM710[pm_dibaca].voltB_C = data_PM710[pm_dibaca].voltB_C * satuan_volt[pm_dibaca];
			#ifdef CEK_PM
			printf("voltB_C: %d - %.2f\r\n", data_PM710[pm_dibaca].voltB_C, asli_PM710[pm_dibaca].voltB_C);
			#endif
			data_PM710[pm_dibaca].voltA_C = buf[7+HD];
			data_PM710[pm_dibaca].voltA_C = (data_PM710[pm_dibaca].voltA_C << 8) + buf[8+HD];
			if (data_PM710[pm_dibaca].voltA_C == 32768)
				asli_PM710[pm_dibaca].voltA_C = 0;
			else
				asli_PM710[pm_dibaca].voltA_C = data_PM710[pm_dibaca].voltA_C * satuan_volt[pm_dibaca];
			#ifdef CEK_PM
			printf("voltB_C: %d - %.2f, satuan_volt: %f\r\n", data_PM710[pm_dibaca].voltB_C, asli_PM710[pm_dibaca].voltA_C, satuan_volt[pm_dibaca]);
			#endif
			// L-L average
			data_PM710[pm_dibaca].volt1 = buf[9+HD];
			data_PM710[pm_dibaca].volt1 = (data_PM710[pm_dibaca].volt1 << 8) + buf[10+HD];
			if (data_PM710[pm_dibaca].volt1 == 32768)
				asli_PM710[pm_dibaca].volt1 = 0;
			else
				asli_PM710[pm_dibaca].volt1 = data_PM710[pm_dibaca].volt1 * satuan_volt[pm_dibaca];
			#ifdef CEK_PM
			printf("volt1  : %d - %.2f, satuan_volt: %f\r\n", data_PM710[pm_dibaca].volt1, asli_PM710[pm_dibaca].volt1, satuan_volt[pm_dibaca]);
			#endif
			data_PM710[pm_dibaca].voltA_N = buf[11+HD];
			data_PM710[pm_dibaca].voltA_N = (data_PM710[pm_dibaca].voltA_N << 8) + buf[12+HD];
			if (data_PM710[pm_dibaca].voltA_N == 32768)
				asli_PM710[pm_dibaca].voltA_N = 0;
			else
				asli_PM710[pm_dibaca].voltA_N = data_PM710[pm_dibaca].voltA_N * satuan_volt[pm_dibaca];
			#ifdef CEK_PM
			printf("voltAN : %d - %.2f\r\n", data_PM710[pm_dibaca].voltA_N, asli_PM710[pm_dibaca].voltA_N);
			#endif


			data_PM710[pm_dibaca].voltB_N = buf[13+HD];
			data_PM710[pm_dibaca].voltB_N = (data_PM710[pm_dibaca].voltB_N << 8) + buf[14+HD];
			if (data_PM710[pm_dibaca].voltB_N == 32768)
				asli_PM710[pm_dibaca].voltB_N = 0;
			else
				asli_PM710[pm_dibaca].voltB_N = data_PM710[pm_dibaca].voltB_N * satuan_volt[pm_dibaca];
			#ifdef CEK_PM
			printf("voltBN : %d - %.2f\r\n", data_PM710[pm_dibaca].voltB_N, asli_PM710[pm_dibaca].voltB_N);
			#endif


			data_PM710[pm_dibaca].voltC_N = buf[15+HD];
			data_PM710[pm_dibaca].voltC_N = (data_PM710[pm_dibaca].voltC_N << 8) + buf[16+HD];
			if (data_PM710[pm_dibaca].voltC_N == 32768)
				asli_PM710[pm_dibaca].voltC_N = 0;
			else
				asli_PM710[pm_dibaca].voltC_N = data_PM710[pm_dibaca].voltC_N * satuan_volt[pm_dibaca];
			#ifdef CEK_PM
			printf("voltCN : %d - %.2f\r\n", data_PM710[pm_dibaca].voltC_N, asli_PM710[pm_dibaca].voltC_N);
			#endif
			
			// buf[17] + buf[18] : Tegangan N-R

			// fasa - netral average
			data_PM710[pm_dibaca].volt2 = buf[19+HD];
			data_PM710[pm_dibaca].volt2 = (data_PM710[pm_dibaca].volt2 << 8) + buf[20+HD];
			if (data_PM710[pm_dibaca].volt2 == 32768)
				asli_PM710[pm_dibaca].volt2 = 0;
			else
				asli_PM710[pm_dibaca].volt2 = data_PM710[pm_dibaca].volt2 * satuan_volt[pm_dibaca];
			#ifdef CEK_PM
			printf("volt2  : %d - %.2f, satuan_volt: %f\r\n", data_PM710[pm_dibaca].volt2, asli_PM710[pm_dibaca].volt2, satuan_volt[pm_dibaca]);
			#endif
		 }
		else if (urt == 3)		 {	//xx kwA, kwB, kwC, kw dll
			// kwA, kwB, kwC, kw
			data_PM710[pm_dibaca].kwA = buf[3+HD];
			data_PM710[pm_dibaca].kwA = (data_PM710[pm_dibaca].kwA << 8) + buf[4+HD];
			if (data_PM710[pm_dibaca].kwA == 32768)
				asli_PM710[pm_dibaca].kwA = 0;
			else
				asli_PM710[pm_dibaca].kwA = data_PM710[pm_dibaca].kwA * satuan_kw[pm_dibaca];
			#ifdef CEK_PM
			printf("kwA: %d - %.2f, satuan kW: %f\r\n", data_PM710[pm_dibaca].kwA, asli_PM710[pm_dibaca].kwA, satuan_kw[pm_dibaca]);
			#endif
			data_PM710[pm_dibaca].kwB = buf[5+HD];
			data_PM710[pm_dibaca].kwB = (data_PM710[pm_dibaca].kwB << 8) + buf[6+HD];
			if (data_PM710[pm_dibaca].kwB == 32768)
				asli_PM710[pm_dibaca].kwB = 0;
			else
				asli_PM710[pm_dibaca].kwB = data_PM710[pm_dibaca].kwB * satuan_kw[pm_dibaca];
			#ifdef CEK_PM
			printf("kwB: %d - %.2f\r\n", data_PM710[pm_dibaca].kwB, asli_PM710[pm_dibaca].kwB);
			#endif
			data_PM710[pm_dibaca].kwC = buf[7+HD];
			data_PM710[pm_dibaca].kwC = (data_PM710[pm_dibaca].kwC << 8) + buf[8+HD];
			if (data_PM710[pm_dibaca].kwC == 32768)
				asli_PM710[pm_dibaca].kwC = 0;
			else
				asli_PM710[pm_dibaca].kwC = data_PM710[pm_dibaca].kwC * satuan_kw[pm_dibaca];
			#ifdef CEK_PM
			printf("kwC: %d - %.2f, satuan kW: %f\r\n", data_PM710[pm_dibaca].kwC, asli_PM710[pm_dibaca].kwC, satuan_kw[pm_dibaca]);
			#endif
			data_PM710[pm_dibaca].kw = buf[9+HD];
			data_PM710[pm_dibaca].kw = (data_PM710[pm_dibaca].kw << 8) + buf[10+HD];
			if (data_PM710[pm_dibaca].kw == 32768)
				asli_PM710[pm_dibaca].kw = 0;
			else
				asli_PM710[pm_dibaca].kw = data_PM710[pm_dibaca].kw * satuan_kw[pm_dibaca];
			#ifdef CEK_PM
			printf("kw : %d - %.2f, satuan kW: %f\r\n", data_PM710[pm_dibaca].kw, asli_PM710[pm_dibaca].kw, satuan_kw[pm_dibaca]);
			#endif
			// kvarA, kvarB, kvarC, kvar
			data_PM710[pm_dibaca].kvarA = buf[11+HD];
			data_PM710[pm_dibaca].kvarA = (data_PM710[pm_dibaca].kvarA << 8) + buf[12+HD];
			if (data_PM710[pm_dibaca].kvarA == 32768)
				asli_PM710[pm_dibaca].kvarA = 0;
			else
				asli_PM710[pm_dibaca].kvarA = (signed short) data_PM710[pm_dibaca].kvarA * satuan_kw[pm_dibaca];
			#ifdef CEK_PM
			printf("kvarA: %d - %.2f\r\n", data_PM710[pm_dibaca].kvarA, asli_PM710[pm_dibaca].kvarA);
			#endif
			
			data_PM710[pm_dibaca].kvarB = buf[13+HD];
			data_PM710[pm_dibaca].kvarB = (data_PM710[pm_dibaca].kvarB << 8) + buf[14+HD];
			if (data_PM710[pm_dibaca].kvarB == 32768)
				asli_PM710[pm_dibaca].kvarB = 0;
			else
				asli_PM710[pm_dibaca].kvarB = (signed short) data_PM710[pm_dibaca].kvarB * satuan_kw[pm_dibaca];
			#ifdef CEK_PM
			printf("kvarB: %d - %.2f\r\n", data_PM710[pm_dibaca].kvarB, asli_PM710[pm_dibaca].kvarB);
			#endif
			
			data_PM710[pm_dibaca].kvarC = buf[15+HD];
			data_PM710[pm_dibaca].kvarC = (data_PM710[pm_dibaca].kvarC << 8) + buf[16+HD];
			if (data_PM710[pm_dibaca].kvarC == 32768)
				asli_PM710[pm_dibaca].kvarC = 0;
			else
				asli_PM710[pm_dibaca].kvarC = (signed short) data_PM710[pm_dibaca].kvarC * satuan_kw[pm_dibaca];
			#ifdef CEK_PM
			printf("kvarC: %d - %.2f\r\n", data_PM710[pm_dibaca].kvarC, asli_PM710[pm_dibaca].kvarC);
			#endif
			
			data_PM710[pm_dibaca].kvar = buf[17+HD];
			data_PM710[pm_dibaca].kvar = (data_PM710[pm_dibaca].kvar << 8) + buf[18+HD];
			if (data_PM710[pm_dibaca].kvar == 32768)
				asli_PM710[pm_dibaca].kvar = 0;
			else
				asli_PM710[pm_dibaca].kvar = (signed short) data_PM710[pm_dibaca].kvar * satuan_kw[pm_dibaca];
			#ifdef CEK_PM
			printf("kvar : %d - %.2f\r\n", data_PM710[pm_dibaca].kvar, asli_PM710[pm_dibaca].kvar);
			#endif
			// kvaA, kvaB, kvaC, kva
			data_PM710[pm_dibaca].kvaA = buf[19+HD];
			data_PM710[pm_dibaca].kvaA = (data_PM710[pm_dibaca].kvaA << 8) + buf[20+HD];
			if (data_PM710[pm_dibaca].kvaA == 32768)
				asli_PM710[pm_dibaca].kvaA = 0;
			else
				asli_PM710[pm_dibaca].kvaA = data_PM710[pm_dibaca].kvaA * satuan_kw[pm_dibaca];
			#ifdef CEK_PM
			printf("kvarA: %d - %.2f\r\n", data_PM710[pm_dibaca].kvarA, asli_PM710[pm_dibaca].kvarA);
			#endif
			
			data_PM710[pm_dibaca].kvaB = buf[21+HD];
			data_PM710[pm_dibaca].kvaB = (data_PM710[pm_dibaca].kvaB << 8) + buf[22+HD];
			if (data_PM710[pm_dibaca].kvaB == 32768)
				asli_PM710[pm_dibaca].kvaB = 0;
			else
				asli_PM710[pm_dibaca].kvaB = data_PM710[pm_dibaca].kvaB * satuan_kw[pm_dibaca];
			#ifdef CEK_PM
			printf("kvarB: %d - %.2f\r\n", data_PM710[pm_dibaca].kvarB, asli_PM710[pm_dibaca].kvarB);
			#endif

			data_PM710[pm_dibaca].kvaC = buf[23+HD];
			data_PM710[pm_dibaca].kvaC = (data_PM710[pm_dibaca].kvaC << 8) + buf[24+HD];
			if (data_PM710[pm_dibaca].kvaC == 32768)
				asli_PM710[pm_dibaca].kvaC = 0;
			else
				asli_PM710[pm_dibaca].kvaC = data_PM710[pm_dibaca].kvaC * satuan_kw[pm_dibaca];
			#ifdef CEK_PM
			printf("kvarC: %d - %.2f\r\n", data_PM710[pm_dibaca].kvarC, asli_PM710[pm_dibaca].kvarC);
			#endif

			data_PM710[pm_dibaca].kva = buf[25+HD];
			data_PM710[pm_dibaca].kva = (data_PM710[pm_dibaca].kva << 8) + buf[26+HD];
			if (data_PM710[pm_dibaca].kva == 32768)
				asli_PM710[pm_dibaca].kva = 0;
			else
				asli_PM710[pm_dibaca].kva = data_PM710[pm_dibaca].kva * satuan_kw[pm_dibaca];
			#ifdef CEK_PM
			printf("kvar : %d - %.2f\r\n", data_PM710[pm_dibaca].kvar, asli_PM710[pm_dibaca].kvar);
			#endif

		 }
		else if (urt == 4)		 {	//xx pfA, pfB, pfC, pf
			//pfA, pfB, pfC, pf
			data_PM710[pm_dibaca].pfA = buf[3+HD];
			data_PM710[pm_dibaca].pfA = (data_PM710[pm_dibaca].pfA << 8) + buf[4+HD];
			if (data_PM710[pm_dibaca].pfA == 0x8000) 
				asli_PM710[pm_dibaca].pfA = 1.00;
			else
			{
				if (data_PM710[pm_dibaca].pfA > 0x8000) {
					data_PM710[pm_dibaca].pfA = (data_PM710[pm_dibaca].pfA + 0x8000);
					asli_PM710[pm_dibaca].pfA = data_PM710[pm_dibaca].pfA * -0.001;
				}
				else
					asli_PM710[pm_dibaca].pfA = data_PM710[pm_dibaca].pfA * 0.001;
			}
			#ifdef CEK_PM
			printf("pfA: %d - %.2f\r\n", data_PM710[pm_dibaca].pfA, asli_PM710[pm_dibaca].pfB);
			#endif
			data_PM710[pm_dibaca].pfB = buf[5+HD];
			data_PM710[pm_dibaca].pfB = (data_PM710[pm_dibaca].pfB << 8) + buf[6+HD];
			if (data_PM710[pm_dibaca].pfB == 0x8000) asli_PM710[pm_dibaca].pfB = 1.00;
			else
			{
				if (data_PM710[pm_dibaca].pfB > 0x8000) {
					data_PM710[pm_dibaca].pfB = data_PM710[pm_dibaca].pfB + 0x8000;
					asli_PM710[pm_dibaca].pfB = data_PM710[pm_dibaca].pfB * -0.001;
				}
				else 
					asli_PM710[pm_dibaca].pfB = data_PM710[pm_dibaca].pfB * 0.001;
			}
			#ifdef CEK_PM
			printf("pfB: %d - %.2f\r\n", data_PM710[pm_dibaca].pfB, asli_PM710[pm_dibaca].pfB);
			#endif

			data_PM710[pm_dibaca].pfC = buf[7+HD];
			data_PM710[pm_dibaca].pfC = (data_PM710[pm_dibaca].pfC << 8) + buf[8+HD];
			if (data_PM710[pm_dibaca].pfC == 0x8000) asli_PM710[pm_dibaca].pfC = 1.00;
			else
			{
			   if (data_PM710[pm_dibaca].pfC > 0x8000) {
					data_PM710[pm_dibaca].pfC = data_PM710[pm_dibaca].pfC + 0x8000;
					asli_PM710[pm_dibaca].pfC = data_PM710[pm_dibaca].pfC * -0.001;
			   }
			   else
					asli_PM710[pm_dibaca].pfC = data_PM710[pm_dibaca].pfC * 0.001;
			}
			#ifdef CEK_PM
			printf("pfC: %d - %.2f\r\n", data_PM710[pm_dibaca].pfC, asli_PM710[pm_dibaca].pfC);
			#endif

			data_PM710[pm_dibaca].pf = buf[9+HD];
			data_PM710[pm_dibaca].pf = (data_PM710[pm_dibaca].pf << 8) + buf[10+HD];
			if (data_PM710[pm_dibaca].pf == 0x8000) asli_PM710[pm_dibaca].pf = 1.00;
			else
			{
				if (data_PM710[pm_dibaca].pf > 0x8000) {
					data_PM710[pm_dibaca].pf = data_PM710[pm_dibaca].pf + 0x8000;
					asli_PM710[pm_dibaca].pf = data_PM710[pm_dibaca].pf * -0.001;
				} 
				else
					asli_PM710[pm_dibaca].pf = data_PM710[pm_dibaca].pf * 0.001;
			}
			#ifdef CEK_PM
			printf("pf : %d - %.2f\r\n", data_PM710[pm_dibaca].pf, asli_PM710[pm_dibaca].pf);
			#endif

		 }
		else if (urt == 5)		 {	//xx
			data_PM710[pm_dibaca].frek = buf[3+HD];
			data_PM710[pm_dibaca].frek = (data_PM710[pm_dibaca].frek << 8) + buf[4+HD];

			if (data_PM710[pm_dibaca].frek == 32768) data_PM710[pm_dibaca].frek = 0;
			asli_PM710[pm_dibaca].frek = data_PM710[pm_dibaca].frek * 0.01;

			#ifndef MOD_SERVER
			f = data_PM710[pm_dibaca].frek * 0.01;
			#endif
			#ifdef CEK_PM
			printf("frek: %d - %.2f\r\n", data_PM710[pm_dibaca].frek, asli_PM710[pm_dibaca].frek);
			#endif
		 }
		else if (urt == 6)		 {
			//ENERGI
			// kwh, kvarh
			temp2 = buf[7+HD];
			temp2 = (temp2 << 8) + buf[8+HD];
			temp2 = temp2 * 10000;

			temp = buf[5+HD];
			temp = (temp << 8) + buf[6+HD];
			temp = (temp+temp2) * 10000;

			data_PM710[pm_dibaca].kwh = buf[3+HD];
			data_PM710[pm_dibaca].kwh = (data_PM710[pm_dibaca].kwh << 8) + buf[4+HD];
			data_PM710[pm_dibaca].kwh = data_PM710[pm_dibaca].kwh + temp;
			asli_PM710[pm_dibaca].kwh = data_PM710[pm_dibaca].kwh * satuan_kwh[pm_dibaca];
			#ifdef CEK_PM
			printf("kWh: %d - %f\r\n", data_PM710[pm_dibaca].kwh, asli_PM710[pm_dibaca].kwh);
			#endif
			//
			temp2 = buf[15+HD];
			temp2 = (temp2 << 8) + buf[16+HD];
			temp2 = temp2 * 10000;

			temp = buf[13+HD];
			temp = (temp << 8) + buf[14+HD];
			temp = (temp+temp2) * 10000;

			data_PM710[pm_dibaca].kvarh = buf[11+HD];
			data_PM710[pm_dibaca].kvarh = (data_PM710[pm_dibaca].kvarh << 8) + buf[12+HD];
			data_PM710[pm_dibaca].kvarh = data_PM710[pm_dibaca].kvarh + temp;
			asli_PM710[pm_dibaca].kvarh = data_PM710[pm_dibaca].kvarh * satuan_kwh[pm_dibaca];
			#ifdef CEK_PM
			printf("kvarh: %d - %f\r\n", data_PM710[pm_dibaca].kvarh, asli_PM710[pm_dibaca].kvarh);
			#endif
		 }
		else if (urt == 7)		 {
			//ENERGI KVAH
			// kvah
			temp2 = buf[7+HD];
			temp2 = (temp2 << 8) + buf[8+HD];
			temp2 = temp2 * 10000;

			temp = buf[5+HD];
			temp = (temp << 8) + buf[6+HD];
			temp = (temp+temp2) * 10000;

			data_PM710[pm_dibaca].kvah = buf[3+HD];
			data_PM710[pm_dibaca].kvah = (data_PM710[pm_dibaca].kvah << 8) + buf[4+HD];
			data_PM710[pm_dibaca].kvah = data_PM710[pm_dibaca].kvah + temp;
			asli_PM710[pm_dibaca].kvah = data_PM710[pm_dibaca].kvah * satuan_kwh[pm_dibaca];
			#ifdef CEK_PM
			printf("kvah: %d - %f\r\n", data_PM710[pm_dibaca].kvah, asli_PM710[pm_dibaca].kvah);
			#endif
			
			//kontrol_PM[pm_dibaca].alamat = addr_PM710;
			kontrol_PM[pm_dibaca].alamat = pm_dibaca;
			kontrol_PM[pm_dibaca].konek = 1;             // tersambung
			kontrol_PM[pm_dibaca].baru = 1;              // data baru

			//cek jika Volt = 0, maka mesin mati
			if (asli_PM710[pm_dibaca].volt1 == 0)	{
			   kontrol_PM[pm_dibaca].baru = 0;           // supaya tidak dikirim ethernet
			}

			//NEXT PM
		 }
	}
#endif


#ifdef TIPE_MICOM_M300
	
void taruh_data_301(int pm_dibaca, int urut) 
{	
	#ifdef LIAT
	printf("taruh_data_301: urut:%d\n", urut); 
	printf("-- RESPONSE ANALYSIS---\n");
	#endif
	
	pm_dibaca=1;
   
	short satuan_t;
	int i;
	unsigned int temp;
   	unsigned int temp1,temp2,temp3,temp4,temp5,temp6;
    
    int shift;
    shift=8;
    int itemp;
   
    float ftemp;
    
	if(urut==0)	{
		#ifdef LIAT
		printf(">Voltage dalam Long\n");
		#endif
		
		for(i=0;i<6;i++)
		{
			int j=i*2;
			temp1=(buf[shift+3+j*2]<<8)+buf[shift+4+j*2];
			int k=j+1;
			temp2=(buf[shift+3+k*2]<<8)+buf[shift+4+k*2];
			
			temp3=(temp1<<16) + temp2;
			ftemp=2*((float) temp3)/10;
			
			#ifdef LIAT
			printf(" - Address 3%0.4d: (0x%0.4X)+(0x%0.4X)= (0x%0.4X) =>  %d => %0.3f =>\n",meter_voltage_micom+i*2, temp1, temp2,temp3,temp3,ftemp);
			#endif
			
			if(i==0)
			{
				asli_PM710[pm_dibaca].voltA_N = ftemp;
				data_PM710[pm_dibaca].voltA_N = ftemp;
			}
			if(i==1)
			{
				asli_PM710[pm_dibaca].voltB_N = ftemp;
				data_PM710[pm_dibaca].voltB_N = ftemp;
			}
			if(i==2)
			{
				asli_PM710[pm_dibaca].voltC_N = ftemp;
				data_PM710[pm_dibaca].voltC_N = ftemp;
			}
			if(i==3)
			{
				asli_PM710[pm_dibaca].voltA_B = ftemp;
				data_PM710[pm_dibaca].voltA_B = ftemp;
			}
			if(i==4)
			{
				asli_PM710[pm_dibaca].voltB_C = ftemp;
				data_PM710[pm_dibaca].voltB_C = ftemp;
			}
			if(i==5)
			{
				asli_PM710[pm_dibaca].voltA_C = ftemp;
				data_PM710[pm_dibaca].voltA_C = ftemp;
			}
		}
	}
	else if(urut==1)
	{
		#ifdef LIAT
		printf(">DATA CURRENT dalam Long\n");
		#endif
		for(i=0;i<4;i++)
		{
			int j=i*2;
			temp1=(buf[shift+3+j*2]<<8)+buf[shift+4+j*2];
			int k=j+1;
			temp2=(buf[shift+3+k*2]<<8)+buf[shift+4+k*2];
			
			temp3=(temp1<<16) + temp2;
			ftemp=4*((float) temp3)/500; //harusnya di scaled dengan data CT 
									//Untuk CT 100/5 -> 1000
									//Untuk CD 200/5 -> 500
			//printf(" - Address 3%0.4d: (0x%0.4X)+(0x%0.4X)= (0x%0.4X) =>  %d => %0.3f\n",meter_current_micom+i*2, temp1, temp2,temp3,temp3,ftemp);
			
			if(i==0)
			{
				asli_PM710[pm_dibaca].ampA = ftemp;
				data_PM710[pm_dibaca].ampA = ftemp;
			}
			if(i==1)
			{
				asli_PM710[pm_dibaca].ampB = ftemp;
				data_PM710[pm_dibaca].ampB = ftemp;
			}
			if(i==2)
			{
				asli_PM710[pm_dibaca].ampC = ftemp;
				data_PM710[pm_dibaca].ampC = ftemp;
			}
			if(i==3)
			{
				asli_PM710[pm_dibaca].ampN = ftemp;
				data_PM710[pm_dibaca].ampN = ftemp;
			}
		}
	}
	else if(urut==2)
	{
		temp1=(buf[shift+3]<< 8)+buf[shift+4];
		temp2=(buf[shift+5]<< 8)+buf[shift+6];
		
		ftemp=(float) temp2/1000;
		
		#ifdef LIAT
			printf(">DATA Frekuensi (3%0.4d)\n",meter_frek_micom);		
			printf(" Frekuensi: %0.3f\n",ftemp);
		#endif
		
		asli_PM710[pm_dibaca].frek = ftemp;
		data_PM710[pm_dibaca].frek = ftemp;
		
		#ifdef CEK_PM
			printf(" --->Frek : %d - %.2f : %f\r\n", data_PM710[pm_dibaca].frek, asli_PM710[pm_dibaca].frek, satuan_kw[pm_dibaca]);
		#endif

	}
	else if(urut==3)
	{
		int i,j;
		
		#ifdef LIAT
			printf(">DATA POWER \n");
		#endif
		
		for(i=0;i<3;i++)
		{
			j=i*2;
			temp1=(buf[shift+3+j*2]<<8)+buf[shift+4+j*2];
			int k=j+1;
			temp2=(buf[shift+3+k*2]<<8)+buf[shift+4+k*2];
			
			itemp=(int) ((temp1<<16) + temp2);
			ftemp=8*((float) itemp)/10000;
			
			#ifdef LIAT
			printf(" - Address 3%0.4d: (0x%0.4X)+(0x%0.4X)= (0x%0.4X) =>  %d => %0.3f\n",meter_power_micom+i*2, temp1, temp2,temp3,temp3,ftemp);
			#endif
			
			if(i==0)
			{
				
				asli_PM710[pm_dibaca].kw = ftemp;
				data_PM710[pm_dibaca].kw=ftemp;
				#ifdef CEK_PM
					printf(" --->KW : %d - %.2f, satuan kW: %f\r\n", data_PM710[pm_dibaca].kw, asli_PM710[pm_dibaca].kw, satuan_kw[pm_dibaca]);
				#endif
			}
			else if(i==1)
				asli_PM710[pm_dibaca].kvar = (int) ftemp;
			else if(i==2)
				asli_PM710[pm_dibaca].kva = (int) ftemp;
				
		}
		
		i=3;
		j=i*2;
		temp1=(buf[3+j*2]<<8)+buf[4+j*2];
			
		ftemp= ((float) temp1/1000);
		
		#ifdef LIAT
			printf("\n - Address 3%0.4d: (0x%0.4X)+(0x%0.4X)= (0x%0.4X) =>  %d => %0.3f\n",meter_power_micom+i*2, temp1, temp2,temp3,temp3,ftemp);
		#endif
		asli_PM710[pm_dibaca].pf = ftemp;
			
	}
	else if(urut==4)
	{
		#ifdef LIAT
		printf(">DATA ENERGY \n");
		#endif
		
		for(i=0;i<4;i++)
		{
			int j=i*2;
			temp1=(buf[shift+3+j*2]<<8)+buf[shift+4+j*2];
			int k=j+1;
			temp2=(buf[shift+3+k*2]<<8)+buf[shift+4+k*2];
			
			temp3=(temp1<<16) + temp2;
			ftemp=(float) temp3;///1000; //harusnya di scaled dengan data CT 
			ftemp=ftemp/125;
			
			#ifdef LIAT
			printf(" - Address 3%0.4d: (0x%0.4X)+(0x%0.4X)= (0x%0.4X) =>  %d => %0.3f\n",meter_energi_micom+i*2, temp1, temp2,temp3,temp3,ftemp);
			#endif
			
			if(i==0)
				asli_PM710[pm_dibaca].kvarh = ftemp;//3*ftemp/125;
			else if(i==1)
				asli_PM710[pm_dibaca].kwh =ftemp;// 3*ftemp/125; //Imported energy
			else if(i==2)
				asli_PM710[pm_dibaca].kvah =ftemp;// 3*ftemp/125;
			else if(i==3)
				asli_PM710[pm_dibaca].kvah =ftemp;// 3*ftemp/125;
		}	
			
	}
	/*
		else if (urut==10) { //VOLTAGE
		
			
			data_PM710[pm_dibaca].voltA_N = buf[3+HD];
			data_PM710[pm_dibaca].voltA_N = (data_PM710[pm_dibaca].voltA_N << 8) + buf[4+HD];		
			if (data_PM710[pm_dibaca].voltA_N == 32768)
				asli_PM710[pm_dibaca].voltA_N = 0;
			else
				asli_PM710[pm_dibaca].voltA_N = data_PM710[pm_dibaca].voltA_N * satuan_amp[pm_dibaca];
			#ifdef CEK_PM
			printf("ampA: %d - %.2f, satuan amp: %f\r\n", data_PM710[pm_dibaca].voltA_N, asli_PM710[pm_dibaca].voltA_N, satuan_amp[pm_dibaca]);
			#endif
		}*/
	else 
	{
		#ifdef LIAT
		printf(">DATA lain-lain (urut: %d \n",urut);
		#endif
			
		temp1=(buf[3]<< 8)+buf[4];
		temp2=(buf[5]<< 8)+buf[6];
		temp3=(buf[7]<< 8)+buf[8];
		temp4=(buf[9]<< 8)+buf[10];
			
		#ifdef LIAT
		printf(" - Data 1: %d (0x%0.4X)\n",temp1,temp1);
		printf(" - Data 2: %d (0x%0.4X)\n",temp2,temp2); //(buf[3]<< 8) + buf[4]
		
		printf(" - Data 3: %d (0x%0.4X)\n",temp3,temp3);
		printf(" - Data 4: %d (0x%0.4X)\n",temp4,temp4); //(buf[5]<< 8) + 	
		#endif
	}
}
#endif

#ifdef TIPE_MICOM_P127	
void taruh_data_P127(int pm_dibaca, int urut) {	
	#ifdef LIAT
	printf("taruh_data_P127: urut:%d\n-- RESPONSE ANALYSIS---\n", urut); 
	#endif
	
	short satuan_t;
	int i;
	unsigned int temp;
   	unsigned int temp1,temp2,temp3,temp4,temp5,temp6;
    
    int shift;
    shift=8;
    int itemp;
   
    float ftemp;
    
	if(urut==0)	{
		#ifdef LIAT
		printf(">Voltage dalam Long\n");
		#endif
		
		//Formatnya adalah F18A (P127 Modbus database, page 65)
		// F18A = Unsigned Long integer. 0 - 2E32
		for(i=0;i<3;i++)	{
			int j=i*2;
			temp1=(buf[shift+3+j*2]<<8)+buf[shift+4+j*2];
			int k=j+1;
			temp2=(buf[shift+3+k*2]<<8)+buf[shift+4+k*2];
			
			//temp3=(temp1<<16) + temp2;
			temp3=(temp2<<16) + temp1;
			
			ftemp=((float) temp3)/100;
			
			#ifdef LIAT
			printf(" - Address 3%0.4d: (0x%0.4X)+(0x%0.4X)= (0x%0.4X) =>  %d => %0.3f\n",meter_vrms_p127+i*2, temp1, temp2,temp3,temp3,ftemp);
			//printf(">Hitung Voltage antar Phase. Hitung dengan perkalian sudut fasanya\n");
			#endif
			
			if(i==0)	{
				asli_PM710[pm_dibaca].voltA_N = ftemp;
				data_PM710[pm_dibaca].voltA_N = ftemp;
				
				ftemp=asli_PM710[pm_dibaca].voltA_N + asli_PM710[pm_dibaca].voltA_N * 0.73205;
				asli_PM710[pm_dibaca].voltA_B = ftemp;
				data_PM710[pm_dibaca].voltA_B = ftemp;
			}
			if(i==1)	{
				asli_PM710[pm_dibaca].voltB_N = ftemp;
				data_PM710[pm_dibaca].voltB_N = ftemp;
				
				ftemp=asli_PM710[pm_dibaca].voltB_N +asli_PM710[pm_dibaca].voltC_N * 0.73205;
				asli_PM710[pm_dibaca].voltB_C = ftemp;
				data_PM710[pm_dibaca].voltB_C = ftemp;
			}
			if(i==2)	{
				asli_PM710[pm_dibaca].voltC_N = ftemp;
				data_PM710[pm_dibaca].voltC_N = ftemp;
				
				ftemp=(asli_PM710[pm_dibaca].voltA_N+asli_PM710[pm_dibaca].voltB_N+asli_PM710[pm_dibaca].voltC_N)/3;
				asli_PM710[pm_dibaca].volt2=ftemp;
				data_PM710[pm_dibaca].volt2=ftemp;
				
				ftemp=asli_PM710[pm_dibaca].voltC_N + asli_PM710[pm_dibaca].voltA_N * 0.73205;
				asli_PM710[pm_dibaca].voltA_C = ftemp;
				data_PM710[pm_dibaca].voltA_C = ftemp;
				
				ftemp=(asli_PM710[pm_dibaca].voltA_B+asli_PM710[pm_dibaca].voltB_C+asli_PM710[pm_dibaca].voltA_C)/3;
				asli_PM710[pm_dibaca].volt1 = ftemp;
				data_PM710[pm_dibaca].volt1 = ftemp;
			}
		}
	}
	else if (urut==1)	{
		int i,j;
		
		#ifdef LIAT
		//	printf(">DATA POWER \n");
		#endif
		
		for(i=0;i<2;i++)	{
			j=i*2;
			temp1=(buf[shift+3+j*2]<<8)+buf[shift+4+j*2];
			int k=j+1;
			temp2=(buf[shift+3+k*2]<<8)+buf[shift+4+k*2];
			
			//itemp=(int) ((temp1<<16) + temp2);
			itemp=(int) ((temp2<<16) + temp1);
			ftemp=((float) itemp)/100;
			
			#ifdef LIAT
			printf(" - Address %0.4X: (0x%0.4X)+(0x%0.4X)= (0x%0.4X) =>  %d => %0.3f\n",meter_power_p127+i*2, temp1, temp2,temp3,temp3,ftemp);
			#endif
			
			if(i==0)
			{
				
				asli_PM710[pm_dibaca].kw = ftemp;
				data_PM710[pm_dibaca].kw=ftemp;
				#ifdef CEK_PM
					printf(" --->KW : %d - %.2f, satuan kW: %f\r\n", data_PM710[pm_dibaca].kw, asli_PM710[pm_dibaca].kw, satuan_kw[pm_dibaca]);
				#endif
			}
			else if(i==1)
				asli_PM710[pm_dibaca].kvar = (int) ftemp;
		}	
	}
	else if(urut==2)	{
		#ifdef LIAT
		printf(">DATA CURRENT dalam Long\n");
		#endif
		for(i=0;i<4;i++)	{
			int j=i*2;
			temp1=(buf[shift+3+j*2]<<8)+buf[shift+4+j*2];
			int k=j+1;
			temp2=(buf[shift+3+k*2]<<8)+buf[shift+4+k*2];
			
			//temp3=(temp1<<16) + temp2;
			temp3=(temp2<<16) + temp1;
			ftemp=((float) temp3)/100; 
			
			#ifdef LIAT
			printf(" - Address %0.4d: (0x%0.4X)+(0x%0.4X)= (0x%0.4X) =>  %d => %0.3f\n",meter_current_p127+i*2, temp1, temp2,temp3,temp3,ftemp);
			#endif
			
			if(i==0)	{
				asli_PM710[pm_dibaca].ampA = ftemp;
				data_PM710[pm_dibaca].ampA = ftemp;
			}
			if(i==1)	{
				asli_PM710[pm_dibaca].ampB = ftemp;
				data_PM710[pm_dibaca].ampB = ftemp;
			}
			if(i==2)	{
				asli_PM710[pm_dibaca].ampC = ftemp;
				data_PM710[pm_dibaca].ampC = ftemp;
			}
			if(i==3)	{
				asli_PM710[pm_dibaca].ampN = ftemp;
				data_PM710[pm_dibaca].ampN = ftemp;
				
				ftemp=asli_PM710[pm_dibaca].ampA+asli_PM710[pm_dibaca].ampB+asli_PM710[pm_dibaca].ampC;
				asli_PM710[pm_dibaca].amp = ftemp/3;
				data_PM710[pm_dibaca].amp = ftemp/3;
			}
		}
	}
	else if(urut==3) 	{//FREKUENSI
		temp1=(buf[shift+3]<< 8)+buf[shift+4];
		temp2=(buf[shift+5]<< 8)+buf[shift+6];
		
		ftemp=(float) temp1/100;
		
		#ifdef LIAT
			printf(">DATA Frekuensi [0x%0.4X] : 0x%0.4X + 0x%0.4X \n",meter_frek_p127, temp1, temp2);		
			printf(" Frekuensi: %0.3f\n",ftemp);
		#endif
		
		asli_PM710[pm_dibaca].frek = ftemp;
		data_PM710[pm_dibaca].frek = ftemp;
		
		#ifdef CEK_PM
			printf(" --->Frek : %d - %.2f : %f\r\n", data_PM710[pm_dibaca].frek, asli_PM710[pm_dibaca].frek, satuan_kw[pm_dibaca]);
		#endif

	}
	else if(urut==4)	{
		#ifdef LIAT
		printf(">DATA ENERGY \n");
		#endif
		
		for(i=0;i<3;i++)	{
			int j=i*2;
			temp1=(buf[shift+3+j*2]<<8)+buf[shift+4+j*2];
			int k=j+1;
			temp2=(buf[shift+3+k*2]<<8)+buf[shift+4+k*2];
			
			temp3=(temp1<<16) + temp2;
			ftemp=(float) temp3;///1000; //harusnya di scaled dengan data CT 
			ftemp=ftemp/16;
			
			#ifdef LIAT
			printf(" - Address 3%0.4d: (0x%0.4X)+(0x%0.4X)= (0x%0.4X) =>  %d => %0.3f\n",meter_energi_micom+i*2, temp1, temp2,temp3,temp3,ftemp);
			#endif
			
			if(i==0)
				asli_PM710[pm_dibaca].kvarh = ftemp;//3*ftemp/125;
			else if(i==1)
				asli_PM710[pm_dibaca].kwh =ftemp;// 3*ftemp/125; //Imported energy
			else if(i==2)
				asli_PM710[pm_dibaca].kvah =ftemp;// 3*ftemp/125;
			//else if(i==3)
			//	asli_PM710[pm_dibaca].kvah =ftemp;// 3*ftemp/125;
		}	
		
	}
	else if(urut==5)	{
		int i,j;
		//Power Factor ngitungnya beda
		i=0;
		j=i*2;
		temp1=(short) (buf[shift+3+j*2]<<8)+buf[shift+4+j*2];
			
		ftemp= (short) temp1;
	
		asli_PM710[pm_dibaca].pf = ftemp/100;
		
		#ifdef LIAT
		printf(">Power Factor \n");
		printf(" - Address 3%0.4d: (0x%0.4X) =>  %d => %0.3f\n",meter_pf_p127+i*2, temp1, temp1, ftemp);
		#endif
	}
	else if (urut==6)	{
		int i,j;
		//KVA
		i=0;
		j=i*2;
		temp1=(buf[shift+3+j*2]<<8)+buf[shift+4+j*2];
		int k=j+1;
		temp2=(buf[shift+3+k*2]<<8)+buf[shift+4+k*2];
		
		//itemp=(int) ((temp1<<16) + temp2);
		temp3=(int) ((temp2<<16) + temp1);
		ftemp=((float) temp3)/100;
		
		asli_PM710[pm_dibaca].kva = (int) ftemp;
		
		#ifdef LIAT
		printf(">KVA \n");
		printf(" - Address %0.4X: (0x%0.4X)+(0x%0.4X)= (0x%0.4X) =>  %d => %0.3f\n",meter_kva_p127+i*2, temp1, temp2,temp3,temp3,ftemp);
		#endif		
		
	}
	/*
		else if (urut==10) { //VOLTAGE
		
			
			data_PM710[pm_dibaca].voltA_N = buf[3+HD];
			data_PM710[pm_dibaca].voltA_N = (data_PM710[pm_dibaca].voltA_N << 8) + buf[4+HD];		
			if (data_PM710[pm_dibaca].voltA_N == 32768)
				asli_PM710[pm_dibaca].voltA_N = 0;
			else
				asli_PM710[pm_dibaca].voltA_N = data_PM710[pm_dibaca].voltA_N * satuan_amp[pm_dibaca];
			#ifdef CEK_PM
			printf("ampA: %d - %.2f, satuan amp: %f\r\n", data_PM710[pm_dibaca].voltA_N, asli_PM710[pm_dibaca].voltA_N, satuan_amp[pm_dibaca]);
			#endif
		}*/
		/*else 
		{
			#ifdef LIAT
			printf(">DATA lain-lain (urut: %d \n",urut);
			#endif
			
			temp1=(buf[3]<< 8)+buf[4];
			temp2=(buf[5]<< 8)+buf[6];
			temp3=(buf[7]<< 8)+buf[8];
			temp4=(buf[9]<< 8)+buf[10];
			
			#ifdef LIAT
			printf(" - Data 1: %d (0x%0.4X)\n",temp1,temp1);
			printf(" - Data 2: %d (0x%0.4X)\n",temp2,temp2); //(buf[3]<< 8) + buf[4]
			
			printf(" - Data 3: %d (0x%0.4X)\n",temp3,temp3);
			printf(" - Data 4: %d (0x%0.4X)\n",temp4,temp4); //(buf[5]<< 8) + 	
			#endif
		}*/
		
	#ifdef CEK_PM
		printf(" -out >KW : data_pm710[%d]: %d - %.2f. \r\n", pm_dibaca, data_PM710[pm_dibaca].kw, asli_PM710[pm_dibaca].kw, satuan_kw[pm_dibaca]);
	#endif
		
}
#endif

#ifdef TIPE_ION8600
void taruh_data_ION8600(int pm_dibaca, int urut) {	
	#ifdef LIAT
	printf("taruh_data_ION8600: urut:%d\n-- RESPONSE ANALYSIS---\n", urut); 
	#endif
	
	short satuan_t;
	int i;
	unsigned int temp;
   	unsigned int temp1,temp2,temp3,temp4,temp5,temp6;
    
    int shift;
    shift=8;
    int itemp;
   
    float ftemp;
    
	if(urut==0)	{
		#ifdef LIAT
		printf(">Voltage dalam Long\n");
		#endif
		
		for(i=0;i<10;i++)		{
			int j=i*2;
			temp1=(buf[shift+3+j*2]<<8)+buf[shift+4+j*2];
			int k=j+1;
			temp2=(buf[shift+3+k*2]<<8)+buf[shift+4+k*2];
			
			//temp3=(temp1<<16) + temp2;
			temp3=(temp1<<16) + temp2;
			
			ftemp=((float) temp3);
			
			#ifdef LIAT
			printf(" - Address 4%0.4d: (0x%0.4X)+(0x%0.4X)= (0x%0.4X) =>  %d => %0.3f\n",meter_voltage_ION8600+i*2, temp1, temp2,temp3,temp3,ftemp);
			#endif
			
			if(i==0)	{
				asli_PM710[pm_dibaca].voltA_N = ftemp;
				data_PM710[pm_dibaca].voltA_N = ftemp;		
			} 
			if(i==1)	{
				asli_PM710[pm_dibaca].voltB_N = ftemp;
				data_PM710[pm_dibaca].voltB_N = ftemp;
			} 
			if(i==2)	{
				asli_PM710[pm_dibaca].voltC_N = ftemp;
				data_PM710[pm_dibaca].voltC_N = ftemp;
			} 
			if(i==3)	{
				asli_PM710[pm_dibaca].volt2 = ftemp;
				data_PM710[pm_dibaca].volt2 = ftemp;
			} 
			if(i==6)	{
				asli_PM710[pm_dibaca].voltA_B = ftemp;
				data_PM710[pm_dibaca].voltA_B = ftemp;
			} 
			if(i==7)	{
				asli_PM710[pm_dibaca].voltB_C = ftemp;
				data_PM710[pm_dibaca].voltB_C = ftemp;
			} 
			if(i==8)	{
				asli_PM710[pm_dibaca].voltA_C = ftemp;
				data_PM710[pm_dibaca].voltA_C = ftemp;
			} 
			if(i==9)	{
				asli_PM710[pm_dibaca].volt1 = ftemp;
				data_PM710[pm_dibaca].volt1 = ftemp;
			}
		}
	}// EOF urut 0: Voltage ION8600 
	else if (urut==1)	{ //Arus ION 8600
		int i,j;
		
		#ifdef LIAT
		//	printf(">DATA POWER \n");
		#endif
		
		for(i=0;i<10;i++)	{
			j=i;//*2;
			temp1=(buf[shift+3+j*2]<<8)+buf[shift+4+j*2];
			//int k=j+1;
			//temp2=(buf[shift+3+k*2]<<8)+buf[shift+4+k*2];
			
			//itemp=(int) ((temp1<<16) + temp2);
			//itemp=(int) ((temp2<<16) + temp1);
			ftemp=((float) temp1/10);
			
			#ifdef LIAT
			printf(" - Address 4%0.4d: (0x%0.4X)+(0x%0.4X)= (0x%0.4X) =>  %d => %0.3f\n",meter_current_ION8600+i*2, temp1, temp2,temp3,temp3,ftemp);
			#endif
			
			if(i==0)	{
				asli_PM710[pm_dibaca].ampA = ftemp;
				data_PM710[pm_dibaca].ampA = ftemp;		
			} 
			if(i==1)	{
				asli_PM710[pm_dibaca].ampB = ftemp;
				data_PM710[pm_dibaca].ampB = ftemp;
			} 
			if(i==2)	{
				asli_PM710[pm_dibaca].ampC = ftemp;
				data_PM710[pm_dibaca].ampC = ftemp;
			} 
			if(i==3)	{//Arus Netral
				asli_PM710[pm_dibaca].ampN = ftemp;
				data_PM710[pm_dibaca].ampN = ftemp;
			} 
			if(i==5)	{//Arus rata-rata
				asli_PM710[pm_dibaca].amp = ftemp;
				data_PM710[pm_dibaca].amp = ftemp;
			} 
			if(i==9)	{//Frekuensi
				asli_PM710[pm_dibaca].frek = ftemp;
				data_PM710[pm_dibaca].frek = ftemp;
			}
		}	
	} //EOF ARUS ion8600
	else if(urut==2)	{ //meter_power_ION8600
		#ifdef LIAT
		printf(">DATA power dalam Long\n");
		#endif
		for(i=0;i<15;i++)	{
			int j=i*2;
			temp1=(buf[shift+3+j*2]<<8)+buf[shift+4+j*2];
			int k=j+1;
			temp2=(buf[shift+3+k*2]<<8)+buf[shift+4+k*2];
			
			temp3=(int) (temp1<<16) + temp2;
			//temp3=(temp2<<16) + temp1;
			ftemp=((signed) temp3); 
			ftemp=ftemp*0.001;
			#ifdef LIAT
			printf(" - Address %0.4d: (0x%0.4X)+(0x%0.4X)= (0x%0.4X) =>  %d => %0.3f\n",meter_power_ION8600+i*2, temp1, temp2,temp3,temp3,ftemp);
			#endif
			
			if(i==0)	{
				asli_PM710[pm_dibaca].kwA = ftemp;
				data_PM710[pm_dibaca].kwA = ftemp;
			} if(i==1)	{
				asli_PM710[pm_dibaca].kwB = ftemp;
				data_PM710[pm_dibaca].kwB = ftemp;
			} if(i==2)	{
				asli_PM710[pm_dibaca].kwC = ftemp;
				data_PM710[pm_dibaca].kwC = ftemp;
			} if(i==3)	{
				asli_PM710[pm_dibaca].kw = ftemp;
				data_PM710[pm_dibaca].kw = ftemp;
			} if(i==5)	{
				asli_PM710[pm_dibaca].kvarA = ftemp;
				data_PM710[pm_dibaca].kvarA = ftemp;
			} if(i==6)	{
				asli_PM710[pm_dibaca].kvarB = ftemp;
				data_PM710[pm_dibaca].kvarB = ftemp;
			} if(i==7)	{
				asli_PM710[pm_dibaca].kvarC = ftemp;
				data_PM710[pm_dibaca].kvarC = ftemp;
			} if(i==8)	{
				asli_PM710[pm_dibaca].kvar = ftemp;
				data_PM710[pm_dibaca].kvar = ftemp;
			} if(i==10)	{
				asli_PM710[pm_dibaca].kvaA = ftemp;
				data_PM710[pm_dibaca].kvaA = ftemp;
			}
			if(i==11)
			{
				asli_PM710[pm_dibaca].kvaB = ftemp;
				data_PM710[pm_dibaca].kvaB = ftemp;
			}
			if(i==12)
			{
				asli_PM710[pm_dibaca].kvaC = ftemp;
				data_PM710[pm_dibaca].kvaC = ftemp;
			}
			if(i==13)
			{
				asli_PM710[pm_dibaca].kva = ftemp;
				data_PM710[pm_dibaca].kva = ftemp;
			
			}
		}
	}
	else if(urut==3) 	{//ENERGI
		#ifdef LIAT
		printf(">DATA ENERGI. pm_dibaca: [%d]\n", pm_dibaca);
		#endif
		for(i=0;i<6;i++)	{
			int j=i*2;
			temp1=(buf[shift+3+j*2]<<8)+buf[shift+4+j*2];
			int k=j+1;
			temp2=(buf[shift+3+k*2]<<8)+buf[shift+4+k*2];
			
			temp3=(int) (temp1<<16) + temp2;
			//temp3=(temp2<<16) + temp1;
			ftemp=((signed) temp3); 
			ftemp=ftemp;
			#ifdef LIAT
			printf(" - Address %0.4d: (0x%0.4X)+(0x%0.4X)= (0x%0.4X) =>  %d => %0.3f\n",meter_energi_ION8600+i*2, temp1, temp2,temp3,temp3,ftemp);
			#endif
			
			if (i==0)	{
				asli_PM710[pm_dibaca].kwh = ftemp;
				data_PM710[pm_dibaca].kwh = ftemp;
			}
			if (i==1)	{
				asli_PM710[pm_dibaca].kwh_receive = ftemp;
				data_PM710[pm_dibaca].kwh_receive = ftemp;
			}
			if (i==2)	{
				asli_PM710[pm_dibaca].kvarh = ftemp;
				data_PM710[pm_dibaca].kvarh = ftemp;
			}
			if (i==3)	{
				asli_PM710[pm_dibaca].kvarh_receive = ftemp;
				data_PM710[pm_dibaca].kvarh_receive = ftemp;
			}
			if (i==4)	{
				asli_PM710[pm_dibaca].kvah = ftemp;
				data_PM710[pm_dibaca].kvah = ftemp;
			}
			/*if(i==5)
			{
				asli_PM710[pm_dibaca].kvah_receive = ftemp;
				data_PM710[pm_dibaca].kvah_receive = ftemp;
			}*/
		}
		#ifdef CEK_PM
			printf(" --->KWH : %d - %.2f : %f\r\n", data_PM710[pm_dibaca].frek, asli_PM710[pm_dibaca].frek, satuan_kw[pm_dibaca]);
		#endif

	} //EOF ENERGY ION8600
	else if(urut==4)	{
		#ifdef LIAT
		printf(">POWER FAKTOR \n");
		#endif
		
		for(i=0;i<4;i++)
		{
			int j=i;
			temp1=(buf[shift+3+j*2]<<8)+buf[shift+4+j*2];
			//int k=j+1;
			//temp2=(buf[shift+3+k*2]<<8)+buf[shift+4+k*2];
			
			temp2=(signed short int) temp1;
			
			if(temp2 & 0x8000000)
			{
				printf ("-> 0x8000000 =>");
				temp3 =(int)( ~temp2|0xffffffffffffffff); 
			}
			else
				temp3=temp2;
			
			ftemp=((float) temp3)/10000;			
		
			#ifdef LIAT
			printf(" - Address 3%0.4d: (0x%0.4X) %d => %0.3f\n",meter_faktor_ION8600+i, temp1,temp3,ftemp);
			#endif
			
			if(i==0)
				asli_PM710[pm_dibaca].pfA = ftemp;
			else if(i==1)
				asli_PM710[pm_dibaca].pfB =ftemp;
			else if(i==2)
				asli_PM710[pm_dibaca].pfC =ftemp;
			else if(i==3)
				asli_PM710[pm_dibaca].pf =ftemp;
		}	
		
	}
		
		#ifdef CEK_PM
		printf(" -out >KW : data_pm710[%d]: %d - %.2f. \r\n", pm_dibaca, data_PM710[pm_dibaca].kw, asli_PM710[pm_dibaca].kw, satuan_kw[pm_dibaca]);
		#endif
		
}
#endif //TIPE ION8600

#ifdef TIPE_A2000	
void taruh_data_A2000(int pm_dibaca, int urut) {	
	#ifdef LIAT
	printf("taruh_data_A2000: urut:%d\n-- RESPONSE ANALYSIS---\n", urut); 
	#endif
	
	short satuan_t;
	int i;
	unsigned int temp;
   	unsigned int temp1,temp2,temp3,temp4,temp5,temp6;
    
    int shift;
    shift=8;
    int itemp;
   
    float ftemp;
    float SKALA_A2000=0.1;
    
	if 		(urut==0)	{ //Voltage Delta Phase
		#ifdef LIAT
		printf(">Voltage dalam Long\n");
		#endif
		
		for(i=0;i<3;i++)	{
			int j=i;//*2;
			temp1=(buf[shift+3+j*2]<<8)+buf[shift+4+j*2];
			//int k=j+1;
			//temp2=(buf[shift+3+k*2]<<8)+buf[shift+4+k*2];
			
			//temp3=(temp1<<16) + temp2;
			temp3=temp1;
			
			ftemp=((float) temp3) *SKALA_A2000;
			
			//#define LIAT
			#ifdef LIAT
			printf(" - Address 4%0.4d: (0x%0.4X)+(0x%0.4X)= (0x%0.4X) =>  %d => %0.3f\n",meter_voltage_ION8600+i*2, temp1, temp2,temp3,temp3,ftemp);
			#endif
			
			if (i==0)	{
				asli_PM710[pm_dibaca].voltA_B = ftemp;
				data_PM710[pm_dibaca].voltA_B = ftemp;
			}
			if (i==1)	{
				asli_PM710[pm_dibaca].voltB_C = ftemp;
				data_PM710[pm_dibaca].voltB_C = ftemp;
			}
			if (i==2)	{
				asli_PM710[pm_dibaca].voltA_C = ftemp;
				data_PM710[pm_dibaca].voltA_C = ftemp;
			}
			
		}
		//Hitung Rata-rata
		ftemp=asli_PM710[pm_dibaca].voltA_B+asli_PM710[pm_dibaca].voltB_C+asli_PM710[pm_dibaca].voltA_C;
		asli_PM710[pm_dibaca].volt2=ftemp/3;
		
	}// EOF urut 0: Voltage Delta Phase A2000 
	else if (urut==1)	{ //Voltae Phase A2000
		int i,j;
		
		#ifdef LIAT
		#endif
		
		for(i=0;i<3;i++)	{
			j=i;//*2;
			temp1=(buf[shift+3+j*2]<<8)+buf[shift+4+j*2];
			ftemp=((float) temp1/10);
			
			#ifdef LIAT
			printf(" - Address 4%0.4d: (0x%0.4X)+(0x%0.4X)= (0x%0.4X) =>  %d => %0.3f\n",meter_current_ION8600+i*2, temp1, temp2,temp3,temp3,ftemp);
			#endif
		
			if (i==0)		{
				asli_PM710[pm_dibaca].voltA_N = ftemp;
				data_PM710[pm_dibaca].voltA_N = ftemp;		
			}
			if (i==1)	{
				asli_PM710[pm_dibaca].voltB_N = ftemp;
				data_PM710[pm_dibaca].voltB_N = ftemp;
			}
			if (i==2)	{
				asli_PM710[pm_dibaca].voltC_N = ftemp;
				data_PM710[pm_dibaca].voltC_N = ftemp;
			}		
			//Hitung Rata-rata
			ftemp=asli_PM710[pm_dibaca].voltA_N+asli_PM710[pm_dibaca].voltB_N+asli_PM710[pm_dibaca].voltC_N;
			asli_PM710[pm_dibaca].volt1=ftemp/3;	
		}	
	} //EOF Voltae Phase A2000
	else if (urut==2)	{ //Voltae ARUS PHASE A2000
		int i,j;
		
		#ifdef LIAT
		#endif
		
		for(i=0;i<4;i++)	{
			j=i;//*2;
			temp1=(buf[shift+3+j*2]<<8)+buf[shift+4+j*2];
			ftemp=((float) temp1/10);
			
			#ifdef LIAT
			printf(" - Address 4%0.4d: (0x%0.4X)+(0x%0.4X)= (0x%0.4X) =>  %d => %0.3f\n",meter_current_ION8600+i*2, temp1, temp2,temp3,temp3,ftemp);
			#endif
		
			if (i==0)	{
				asli_PM710[pm_dibaca].ampA = ftemp;
				data_PM710[pm_dibaca].ampA = ftemp;		
			}
			if (i==1)	{
				asli_PM710[pm_dibaca].ampB = ftemp;
				data_PM710[pm_dibaca].ampB = ftemp;
			}
			if(i==2)	{
				asli_PM710[pm_dibaca].ampC = ftemp;
				data_PM710[pm_dibaca].ampC = ftemp;
			}		//Hitung Rata-rata
			ftemp=asli_PM710[pm_dibaca].ampA+asli_PM710[pm_dibaca].ampB+asli_PM710[pm_dibaca].ampC;
			asli_PM710[pm_dibaca].amp=ftemp/3;
		}		
	} //EOF Arus Phase A2000
	else if (urut==3)	{ //
		#ifdef LIAT
		printf(">DATA power dalam Long\n");
		#endif
		int i,j;
		for(i=0;i<4;i++)	{
			j=i;//*2;
			temp1=(buf[shift+3+j*2]<<8)+buf[shift+4+j*2];
			ftemp=((float) temp1/10);
			
			#ifdef LIAT
			printf(" - Address %0.4d: (0x%0.4X)+(0x%0.4X)= (0x%0.4X) =>  %d => %0.3f\n",meter_power_ION8600+i*2, temp1, temp2,temp3,temp3,ftemp);
			#endif
			
			if (i==0)	{
				asli_PM710[pm_dibaca].kwA = ftemp;
				data_PM710[pm_dibaca].kwA = ftemp;
			}
			if (i==1)	{
				asli_PM710[pm_dibaca].kwB = ftemp;
				data_PM710[pm_dibaca].kwB = ftemp;
			}
			if (i==2)	{
				asli_PM710[pm_dibaca].kwC = ftemp;
				data_PM710[pm_dibaca].kwC = ftemp;
			}
			if (i==3)	{
				asli_PM710[pm_dibaca].kw = ftemp;
				data_PM710[pm_dibaca].kw = ftemp;
			}
		}
	}
	else if (urut==4)	{ //ENERGI
		#ifdef LIAT
		printf(">DATA ENERGI. pm_dibaca: [%d]\n", pm_dibaca);
		#endif
		for(i=0;i<8;i++)	{
			int j=i*2;
			temp1=(buf[shift+3+j*2]<<8)+buf[shift+4+j*2];
			int k=j+1;
			temp2=(buf[shift+3+k*2]<<8)+buf[shift+4+k*2];
			
			temp3=(int) (temp1<<16) + temp2;
			//temp3=(temp2<<16) + temp1;
			ftemp=((signed) temp3); 
			ftemp=ftemp;
			
			#ifdef LIAT
			printf(" - Address %0.4d: (0x%0.4X)+(0x%0.4X)= (0x%0.4X) =>  %d => %0.3f\n",meter_energi_ION8600+i*2, temp1, temp2,temp3,temp3,ftemp);
			#endif
			
			if (i==3)	{
				asli_PM710[pm_dibaca].kwh = ftemp;
				data_PM710[pm_dibaca].kwh = ftemp;
			}
			if (i==7)	{
				asli_PM710[pm_dibaca].kvarh = ftemp;
				data_PM710[pm_dibaca].kvarh = ftemp;
			}
		}
		#ifdef CEK_PM
			printf(" --->KWH : %d - %.2f : %f\r\n", data_PM710[pm_dibaca].frek, asli_PM710[pm_dibaca].frek, satuan_kw[pm_dibaca]);
		#endif

	} //EOF ENERGY A2000
	else if (urut==5)	{
		#ifdef LIAT
		printf(">POWER FAKTOR \n");
		#endif
		
		for(i=0;i<4;i++)			{
			int j=i;
			temp1=(buf[shift+3+j*2]<<8)+buf[shift+4+j*2];
			ftemp=((float) temp1/100);
			
			//#define LIAT
			#ifdef LIAT
			printf(" - Address 3%0.4d: (0x%0.4X) %d => %0.3f\n",meter_faktor_A2000+i, temp1,temp3,ftemp);
			#endif
			
			if(i==0)
				asli_PM710[pm_dibaca].pfA = ftemp;
			else if(i==1)
				asli_PM710[pm_dibaca].pfB =ftemp;
			else if(i==2)
				asli_PM710[pm_dibaca].pfC =ftemp;
			else if(i==3)
				asli_PM710[pm_dibaca].pf =ftemp;
		}	
		
	}
	else if (urut==6)	{ //Apparent Power
		#ifdef LIAT
		printf(">DATA Apparent power dalam Long (KVA)\n");
		#endif
		int i,j;
		for(i=0;i<4;i++)
		{
			j=i;//*2;
			temp1=(buf[shift+3+j*2]<<8)+buf[shift+4+j*2];
			ftemp=((float) temp1/10);
			
			#ifdef LIAT
			printf(" - Address %0.4d: (0x%0.4X)+(0x%0.4X)= (0x%0.4X) =>  %d => %0.3f\n",meter_power_ION8600+i*2, temp1, temp2,temp3,temp3,ftemp);
			#endif
			
			if (i==0)	{
				asli_PM710[pm_dibaca].kvaA = ftemp;
				data_PM710[pm_dibaca].kvaA = ftemp;
			}
			if (i==1)	{
				asli_PM710[pm_dibaca].kvaB = ftemp;
				data_PM710[pm_dibaca].kvaB = ftemp;
			}
			if (i==2)	{
				asli_PM710[pm_dibaca].kvaC = ftemp;
				data_PM710[pm_dibaca].kvaC = ftemp;
			}
			if (i==3)	{
				asli_PM710[pm_dibaca].kva = ftemp;
				data_PM710[pm_dibaca].kva = ftemp;
			}
		}
	}
	else if (urut==7)	{ //Reactive Power
		#ifdef LIAT
		printf(">DATA Apparent power dalam Long (KVAR)\n");
		#endif
		int i,j;
		for(i=0;i<4;i++)	{
			j=i;//*2;
			temp1=(buf[shift+3+j*2]<<8)+buf[shift+4+j*2];
			ftemp=((float) temp1/10);
			
			#ifdef LIAT
			printf(" - Address %0.4d: (0x%0.4X)+(0x%0.4X)= (0x%0.4X) =>  %d => %0.3f\n",meter_power_ION8600+i*2, temp1, temp2,temp3,temp3,ftemp);
			#endif
			
			if (i==0)	{
				asli_PM710[pm_dibaca].kvarA = ftemp;
				data_PM710[pm_dibaca].kvarA = ftemp;
			}
			if (i==1)	{
				asli_PM710[pm_dibaca].kvarB = ftemp;
				data_PM710[pm_dibaca].kvarB = ftemp;
			}
			if (i==2)	{
				asli_PM710[pm_dibaca].kvarC = ftemp;
				data_PM710[pm_dibaca].kvarC = ftemp;
			}
			if (i==3)	{
				asli_PM710[pm_dibaca].kvar = ftemp;
				data_PM710[pm_dibaca].kvar = ftemp;
			}
		}
	}
	else if (urut==8)	{ //Frekuensi
		#ifdef LIAT
		printf(">DATA Frekuensi\n");
		#endif
		int i,j;
		for(i=0;i<1;i++)	{
			j=i;//*2;
			temp1=(buf[shift+3+j*2]<<8)+buf[shift+4+j*2];
			ftemp=((float) temp1/100);
			
			#ifdef LIAT
			printf(" - Address %0.4d: (0x%0.4X)+(0x%0.4X)= (0x%0.4X) =>  %d => %0.3f\n",meter_power_ION8600+i*2, temp1, temp2,temp3,temp3,ftemp);
			#endif
			
			if (i==0)	{
				asli_PM710[pm_dibaca].frek = ftemp;
				data_PM710[pm_dibaca].frek = ftemp;
			}
		}
	}
	else if (urut==9)	{ //Netral Ampere
		#ifdef LIAT
		printf(">DATA Arus Netral\n");
		#endif
		int i,j;
		for(i=0;i<1;i++)	{
			j=i;//*2;
			temp1=(buf[shift+3+j*2]<<8)+buf[shift+4+j*2];
			ftemp=((float) temp1/10);
			
			#ifdef LIAT
			printf(" - Address %0.4d: (0x%0.4X)+(0x%0.4X)= (0x%0.4X) =>  %d => %0.3f\n",meter_power_ION8600+i*2, temp1, temp2,temp3,temp3,ftemp);
			#endif
			
			if(i==0)	{
				asli_PM710[pm_dibaca].ampN = ftemp;
				data_PM710[pm_dibaca].ampN = ftemp;
			}
		}
	}
	#ifdef CEK_PM
	printf(" -out >KW : data_pm710[%d]: %d - %.2f. \r\n", pm_dibaca, data_PM710[pm_dibaca].kw, asli_PM710[pm_dibaca].kw, satuan_kw[pm_dibaca]);
	#endif
		
}
#endif //TIPE_A2000
//---------------------------------------------------------------------------

#ifdef TIPE_TFX_ULTRA	
void taruh_data_tfx(int pm_dibaca, int urut) {	
	#ifdef LIAT
	printf("taruh_data_tfx: urut:%d\n-- RESPONSE ANALYSIS---\n", urut); 
	#endif
	
	short satuan_t;
	int i;
	unsigned int temp;
   	unsigned int temp1,temp2,temp3,temp4,temp5,temp6;
    
    int shift;
    shift=8;
    int itemp;
   
    float ftemp;
    float SKALA_tfx=1;
    
	if (urut==0) {	//Integer Data
		#ifdef LIAT
		printf(">Long Integer Precision\n");
		#endif
		
		for(i=0;i<7;i++)	{
			int j=i*2;
			temp1=(buf[shift+3+j*2]<<8)+buf[shift+4+j*2];
			int k=j+1;
			temp2=(buf[shift+3+k*2]<<8)+buf[shift+4+k*2];
			
			//BIG ENDIAN
			temp3=(temp2<<16) + temp1;
			
			//LITTLE ENDIAN
			//temp3=(temp1<<16) + temp2;
			//temp3=temp1;
						
			if(temp1 & 0x8000)	{
				//printf("| minus =>");
				temp2= ~temp1 +1 ;
				temp3=0x0000FFFF & temp2;
				
				temp3 =temp3;
				ftemp=- ((float) temp3);
			}	else	{
				temp2=temp1;
				temp3=temp2;
				ftemp=((float) temp3);
			}	
			
			//#define LIAT
			#ifdef LIAT
			printf(" - Address 4%0.4d: (0x%0.4X)+(0x%0.4X)= (0x%0.4X) =>  %d => %0.3f\n",tfx_integer+i*2, temp1, temp2,temp3,temp3,ftemp);
			#endif
			
			if (i==0)	{
				asli_PM710[pm_dibaca].kwh = ftemp;
				//data_PM710[pm_dibaca].kwh = ftemp;
			}
			if (i==1)	{
				asli_PM710[pm_dibaca].kvah = ftemp;
				//data_PM710[pm_dibaca].kvah = ftemp;
			}
			if (i==2)	{
				asli_PM710[pm_dibaca].kvarh = ftemp;
				//data_PM710[pm_dibaca].kvarh = ftemp;
			}
			if(i==3)	{
				asli_PM710[pm_dibaca].kw = ftemp;
				//data_PM710[pm_dibaca].kw = ftemp;
			}
			if(i==4)	{
				asli_PM710[pm_dibaca].kva = ftemp;
				//data_PM710[pm_dibaca].kva = ftemp;
			}
			if(i==5)	{
				asli_PM710[pm_dibaca].kvar = ftemp;
				//data_PM710[pm_dibaca].kvar = ftemp;
			}
			if(i==6)	{
				asli_PM710[pm_dibaca].pf = ftemp;
				//data_PM710[pm_dibaca].pf = ftemp;
			}
		}
		
		//Hitung Rata-rata
		//ftemp=asli_PM710[pm_dibaca].voltA_B+asli_PM710[pm_dibaca].voltB_C+asli_PM710[pm_dibaca].voltA_C;
		//asli_PM710[pm_dibaca].volt2=ftemp/3;
		
	}// EOF urut 0: Integer Format
	if (urut==1) 	{ //Single Precission Format
		#ifdef LIAT
		printf(">Single Precision\n");
		#endif
		
		for(i=0;i<7;i++)	{
			int j=i*2;
			temp1=(buf[shift+3+j*2]<<8)+buf[shift+4+j*2];
			int k=j+1;
			temp2=(buf[shift+3+k*2]<<8)+buf[shift+4+k*2];
			
			//BIG ENDIAN
			temp3=(temp2<<16) + temp1;
			
			//LITTLE ENDIAN
			//temp3=(temp1<<16) + temp2;
			//temp3=temp1;
						
			if(temp1 & 0x8000)	{
				//printf("| minus =>");
				temp2= ~temp1 +1 ;
				temp3=0x0000FFFF & temp2;
				
				temp3 =temp3;
				ftemp=- ((float) temp3);
			} else	{
				temp2=temp1;
				temp3=temp2;
				ftemp=((float) temp3);
			}	
			
			//#define LIAT
			#ifdef LIAT
			printf(" - Address 4%0.4d: (0x%0.4X)+(0x%0.4X)= (0x%0.4X) =>  %d => %0.3f\n",tfx_integer+i*2, temp1, temp2,temp3,temp3,ftemp);
			#endif
			
			if (i==0)	{
				asli_PM710[pm_dibaca].voltA_B = ftemp;
				data_PM710[pm_dibaca].voltA_B = ftemp;
			}
			if (i==1)	{
				asli_PM710[pm_dibaca].voltB_C = ftemp;
				data_PM710[pm_dibaca].voltB_C = ftemp;
			}
			if(i==2)	{
				asli_PM710[pm_dibaca].voltA_C = ftemp;
				data_PM710[pm_dibaca].voltA_C = ftemp;
			}	
		}
		
		//Hitung Rata-rata
		ftemp=asli_PM710[pm_dibaca].voltA_B+asli_PM710[pm_dibaca].voltB_C+asli_PM710[pm_dibaca].voltA_C;
		asli_PM710[pm_dibaca].volt2=ftemp/3;
		
	}// EOF urut 1: Single Precission Format
	if (urut==2)	{ // Double Precission Format
		#ifdef LIAT
		printf("> Double Precission Format\n");
		#endif
		
		for(i=0;i<7;i++)	{
			int j=i*2;
			temp1=(buf[shift+3+j*2]<<8)+buf[shift+4+j*2];
			int k=j+1;
			temp2=(buf[shift+3+k*2]<<8)+buf[shift+4+k*2];
			
			//BIG ENDIAN
			temp3=(temp2<<16) + temp1;
			
			//LITTLE ENDIAN
			//temp3=(temp1<<16) + temp2;
			//temp3=temp1;
						
			if(temp1 & 0x8000)	{
				//printf("| minus =>");
				temp2= ~temp1 +1 ;
				temp3=0x0000FFFF & temp2;
				
				temp3 =temp3;
				ftemp=- ((float) temp3);
			} else {
				temp2=temp1;
				temp3=temp2;
				ftemp=((float) temp3);
			}	
			
			//#define LIAT
			#ifdef LIAT
			printf(" - Address 4%0.4d: (0x%0.4X)+(0x%0.4X)= (0x%0.4X) =>  %d => %0.3f\n",tfx_integer+i*2, temp1, temp2,temp3,temp3,ftemp);
			#endif
			
			if (i==0)	{
				asli_PM710[pm_dibaca].voltA_B = ftemp;
				data_PM710[pm_dibaca].voltA_B = ftemp;
			}
			if (i==1)	{
				asli_PM710[pm_dibaca].voltB_C = ftemp;
				data_PM710[pm_dibaca].voltB_C = ftemp;
			}
			if (i==2)	{
				asli_PM710[pm_dibaca].voltA_C = ftemp;
				data_PM710[pm_dibaca].voltA_C = ftemp;
			}	
		}
		
		//Hitung Rata-rata
		ftemp=asli_PM710[pm_dibaca].voltA_B+asli_PM710[pm_dibaca].voltB_C+asli_PM710[pm_dibaca].voltA_C;
		asli_PM710[pm_dibaca].volt2=ftemp/3;
		
	}// EOF urut 2: Double Precission Format
	#ifdef CEK_PM
	//printf(" -out >KW : data_pm710[%d]: %d - %.2f. \r\n", pm_dibaca, data_PM710[pm_dibaca].kw, asli_PM710[pm_dibaca].kw, satuan_kw[pm_dibaca]);
	#endif
		
}
#endif //TIPE_TFX_ULTRA

//---------------------------------------------------------------------------
#pragma package(smart_init)


#endif
