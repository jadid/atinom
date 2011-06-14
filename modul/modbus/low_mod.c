/*
 * banyak dicopy dari versi windows
 * yang dikembangkan sejak Meulaboh - NAD
 */

//#define 	JUMLAH_PM		1
//#define		TIPE_PM710

#include 	"FreeRTOS.h"


#ifdef AMBIL_PM

#include 	"low_mod.h"
#include 	"mbcrc.h"
#include 	"mbcrc.c"

//unsigned short jum_PM710_masuk;
unsigned short reg_flag;
//unsigned short urut_PM710;    //urutan pengambilan data
//unsigned short jum_balik;
//extern unsigned char addr_PM710;
//extern unsigned char addr_KTA;

struct d_PM710 data_PM710[JML_SUMBER];
struct f_PM710 asli_PM710[JML_SUMBER];
//struct t_kontrol_PM kontrol_PM[JUMLAH_PM];

#ifdef PAKAI_KTA
unsigned short 	wind_speed;
unsigned short 	wind_dir;
float 			f_wind_speed;

unsigned short wind_satuan;
unsigned short wind_speed_tr;		// retransmision value
unsigned short wind_dir_tr;
#endif


#define buf	buf_rx
extern struct d_pmod pmod;

#define MOD_SERVER


/*
   query data dari register PM710 & juga dimasukkan ukuran data yang akan diquery
   di query secara periodik untuk mendapatkan data yang diinginkan

   return adalah jumlah byte yang seharusnya diterima
*/
//unsigned char addr_PM710=2;		//2

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
#ifdef PAKAI_MAX485
	#define HD	sizeof(pmod)
	//#define HD	0
#else
	#define HD	0
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
			if (data_PM710[pm_dibaca].pfA == 0x8000) asli_PM710[pm_dibaca].pfA = 1.00;
			else
			{
			   if (data_PM710[pm_dibaca].pfA > 0x8000)
					data_PM710[pm_dibaca].pfA = data_PM710[pm_dibaca].pfA + 0x8000;

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
			   if (data_PM710[pm_dibaca].pfB > 0x8000)
					data_PM710[pm_dibaca].pfB = data_PM710[pm_dibaca].pfB + 0x8000;

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
			   if (data_PM710[pm_dibaca].pfC > 0x8000)
					data_PM710[pm_dibaca].pfC = data_PM710[pm_dibaca].pfC + 0x8000;
			   
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
			   if (data_PM710[pm_dibaca].pf > 0x8000)
					data_PM710[pm_dibaca].pf = data_PM710[pm_dibaca].pf + 0x8000;

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
	#define T10_des2		0.01
	#define T11_des3		0.001
	#define T12_des4		0.0001
	
	void taruh_data_301(int pm_dibaca, int urut) {
		if (urut==0) {
			data_PM710[pm_dibaca].voltA_N = buf[3+HD];
			data_PM710[pm_dibaca].voltA_N = (data_PM710[pm_dibaca].voltA_N << 8) + buf[4+HD];		
			if (data_PM710[pm_dibaca].voltA_N == 32768)
				asli_PM710[pm_dibaca].voltA_N = 0;
			else
				asli_PM710[pm_dibaca].voltA_N = data_PM710[pm_dibaca].voltA_N * satuan_amp[pm_dibaca];
			#ifdef CEK_PM
			printf("ampA: %d - %.2f, satuan amp: %f\r\n", data_PM710[pm_dibaca].voltA_N, asli_PM710[pm_dibaca].voltA_N, satuan_amp[pm_dibaca]);
			#endif
		}
		else if (urut==1) {
			
		}
	}
#endif

//---------------------------------------------------------------------------
#pragma package(smart_init)


#endif
