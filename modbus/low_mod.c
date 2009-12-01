/*
 * banyak dicopy dari versi windows
 * yang dikembangkan sejak Meulaboh - NAD
 */

#define 	JUMLAH_PM		1
#define		TIPE_PM710

#include 	"low_mod.h"
#include 	"mbcrc.h"
#include 	"mbcrc.c"

//unsigned short jum_PM710_masuk;
unsigned short reg_flag;
//unsigned short urut_PM710;    //urutan pengambilan data
//unsigned short jum_balik;
//extern unsigned char addr_PM710;
//extern unsigned char addr_KTA;

struct d_PM710 data_PM710[JUMLAH_PM];
struct f_PM710 asli_PM710[JUMLAH_PM];
//struct t_kontrol_PM kontrol_PM[JUMLAH_PM];

unsigned short 	wind_speed;
unsigned short 	wind_dir;
float 			f_wind_speed;

unsigned short wind_satuan;
unsigned short wind_speed_tr;		// retransmision value
unsigned short wind_dir_tr;

#define buf	buf_rx
extern struct d_pmod pmod;

#define MOD_SERVER


/*
   query data dari register PM710 & juga dimasukkan ukuran data yang akan diquery
   di query secara periodik untuk mendapatkan data yang diinginkan

   return adalah jumlah byte yang seharusnya diterima
*/
unsigned char addr_PM710=2;		//2

unsigned int get_PM710(unsigned short reg, unsigned char uk)
{
   unsigned short dcrc;
   int i;

	addr_PM710=2;
   reg_flag = reg;

   if (reg != reg_kwh)
   reg = reg - 1;

   pmod.addr = (unsigned char)   addr_PM710;
   pmod.command = (unsigned char) command_baca;
   pmod.reg_hi = (unsigned char) ((reg & 0xFF00) >> 8);
   pmod.reg_lo = (unsigned char) (reg & 0x00FF);
   pmod.jum_hi = (unsigned char) ((uk & 0xFF00) >> 8);
   pmod.jum_lo = (unsigned char) (uk & 0x00FF);;

   dcrc = usMBCRC16((unsigned char *) &pmod, sizeof (pmod)-2);
   pmod.crc_lo = (unsigned char) ((dcrc & 0xFF00) >> 8);
   pmod.crc_hi = (unsigned char) (dcrc & 0x00FF);;

   	return (1 + 1 + 1 + (uk * 2) + 2);
}

unsigned short cek_PM()
{
	unsigned short dcrc;
   	unsigned char *p;
   	int i;

	//reg_flag = reg;

	pmod.addr = (unsigned char)   addr_PM710;
	pmod.command = (unsigned char) 0x03;		// command untuk cek ID
	pmod.reg_hi = (unsigned char) 0x40;
	pmod.reg_lo = (unsigned char) 0x00;
	pmod.jum_hi = (unsigned char) ((5 & 0xFF00) >> 8);
	pmod.jum_lo = (unsigned char) (5 & 0x00FF);;

	dcrc = usMBCRC16((unsigned char *) &pmod, sizeof (pmod)-2);
	pmod.crc_lo = (unsigned char) ((dcrc & 0xFF00) >> 8);
	pmod.crc_hi = (unsigned char) (dcrc & 0x00FF);;

	return 7;	// respon harusnya 7 byte
	//return (1 + 1 + 1 + (uk * 2) + 2);		
}
#if 1
//void taruh_data(int no_slave, int urt, char *buf)
void taruh_data(int no_slave, int urt)
{
	short satuan_t;
	int i;
	/*
	for (i=0; i<255; i++)
	{
		buf[i] = buf[i] & 0x00FF;
	} */
	
	if (urt == 0)     //satuan2
	{
		// amper
		satuan_t = buf[3];
		satuan_t = (satuan_t << 8) | buf[4];

		if (satuan_t == (short) -4) satuan_amp = 0.0001;
		else if (satuan_t == -3) satuan_amp = 0.001;
		else if (satuan_t == -2) satuan_amp = 0.01;
		else if (satuan_t == -1) satuan_amp = 0.1;
		else if (satuan_t == 0) satuan_amp = 1.0;
		else if (satuan_t == 1) satuan_amp = 10.0;
		else if (satuan_t == 2) satuan_amp = 100.0;
		else if (satuan_t == 3) satuan_amp = 1000.0;
		else if (satuan_t == 4) satuan_amp = 10000.0;
		
		//move_ke(80, 12);
        //printw("sat amp  : %d : %X", satuan_t, (short) satuan_t);

		//volt
		satuan_t = buf[5];
		satuan_t = (satuan_t << 8) | buf[6];

		if (satuan_t == -4) satuan_volt = 0.0001;
		else if (satuan_t == -3) satuan_volt = 0.001;
		else if (satuan_t == -2) satuan_volt = 0.01;
		else if (satuan_t == -1) satuan_volt = 0.1;
		else if (satuan_t == 0) satuan_volt = 1.0;
		else if (satuan_t == 1) satuan_volt = 10.0;
		else if (satuan_t == 2) satuan_volt = 100.0;
		else if (satuan_t == 3) satuan_volt = 1000.0;
		else if (satuan_t == 4) satuan_volt = 10000.0;

		//power
		satuan_t = buf[7];
		satuan_t = (satuan_t << 8) | buf[8];

		if (satuan_t == -4) satuan_kw = 0.0001;
		else if (satuan_t == -3) satuan_kw = 0.001;
		else if (satuan_t == -2) satuan_kw = 0.01;
		else if (satuan_t == -1) satuan_kw = 0.1;
		else if (satuan_t == 0) satuan_kw = 1.0;
		else if (satuan_t == 1) satuan_kw = 10.0;
		else if (satuan_t == 2) satuan_kw = 100.0;
		else if (satuan_t == 3) satuan_kw = 1000.0;
		else if (satuan_t == 4) satuan_kw = 10000.0;

		//energy
		satuan_t = buf[9];
		satuan_t = (satuan_t << 8) | buf[10];

		if (satuan_t == -4) satuan_kwh = 0.0001;
		else if (satuan_t == -3) satuan_kwh = 0.001;
		else if (satuan_t == -2) satuan_kwh = 0.01;
		else if (satuan_t == -1) satuan_kwh = 0.1;
		else if (satuan_t == 0) satuan_kwh = 1.0;
		else if (satuan_t == 1) satuan_kwh = 10.0;
		else if (satuan_t == 2) satuan_kwh = 100.0;
		else if (satuan_t == 3) satuan_kwh = 1000.0;
		else if (satuan_t == 4) satuan_kwh = 10000.0;

		//urut_PM710 = 0;
	}
	else if (urt == 1) // kva, kvar, PF, volt1, volt2, amper
	{
		data_PM710[no_slave].kva = buf[3];
		data_PM710[no_slave].kva = (data_PM710[no_slave].kva << 8) | buf[4];
		asli_PM710[no_slave].kva = data_PM710[no_slave].kva * satuan_kw;

		data_PM710[no_slave].kvar = buf[5];
		data_PM710[no_slave].kvar = (data_PM710[no_slave].kvar << 8) | buf[6];
		asli_PM710[no_slave].kvar = data_PM710[no_slave].kvar * satuan_kw;

		data_PM710[no_slave].pf = buf[7];
		data_PM710[no_slave].pf = (data_PM710[no_slave].pf << 8) | buf[8];
		asli_PM710[no_slave].pf = data_PM710[no_slave].pf * 0.0001;

		data_PM710[no_slave].volt1 = buf[9];
		data_PM710[no_slave].volt1 = (data_PM710[no_slave].volt1 << 8) | buf[10];
		if (data_PM710[no_slave].volt1 == 32768)
			asli_PM710[no_slave].volt1 = 0;
		else
			asli_PM710[no_slave].volt1 = data_PM710[no_slave].volt1 * satuan_volt;
		
		//move_ke(80, 20);
        //printw("9=%X, 10 = %X, jadi = %d", buf[9], (unsigned char) buf[10], data_PM710[no_slave].volt1);

		data_PM710[no_slave].volt2 = buf[11];
		data_PM710[no_slave].volt2 = (data_PM710[no_slave].volt2 << 8) | buf[12];
		if (data_PM710[no_slave].volt2 == 32768)
			asli_PM710[no_slave].volt2 = 0;
		else
			asli_PM710[no_slave].volt2 = data_PM710[no_slave].volt2 * satuan_volt;

		data_PM710[no_slave].amp = buf[13];
		data_PM710[no_slave].amp = (data_PM710[no_slave].amp << 8) | buf[14];
		if (data_PM710[no_slave].amp  == 32768)
			asli_PM710[no_slave].amp = 0;
		else
			asli_PM710[no_slave].amp = data_PM710[no_slave].amp * satuan_amp;

		data_PM710[no_slave].frek = buf[15];
		data_PM710[no_slave].frek = (data_PM710[no_slave].frek << 8) | buf[16];
		if (data_PM710[no_slave].frek == 32768)
			asli_PM710[no_slave].frek = 0;
		else
			asli_PM710[no_slave].frek = data_PM710[no_slave].frek * 0.01;

		//urut_PM710 = 2;
	}
	else if (urt == 2)
	{
		// ampA, B, C & N
		data_PM710[no_slave].ampA = buf[3];
		data_PM710[no_slave].ampA = (data_PM710[no_slave].ampA << 8) | buf[4];
		if (data_PM710[no_slave].ampA == 32768)
			asli_PM710[no_slave].ampA = 0;
		else
			asli_PM710[no_slave].ampA = data_PM710[no_slave].ampA * satuan_amp;

		data_PM710[no_slave].ampB = buf[5];
		data_PM710[no_slave].ampB = (data_PM710[no_slave].ampB << 8) | buf[6];
		if (data_PM710[no_slave].ampB == 32768)
			asli_PM710[no_slave].ampB = 0;
		else
			asli_PM710[no_slave].ampB = data_PM710[no_slave].ampB * satuan_amp;

		data_PM710[no_slave].ampC = buf[7];
		data_PM710[no_slave].ampC = (data_PM710[no_slave].ampC << 8) | buf[8];
		if (data_PM710[no_slave].ampC == 32768)
			asli_PM710[no_slave].ampC = 0;
		else
			asli_PM710[no_slave].ampC = data_PM710[no_slave].ampC * satuan_amp;

		data_PM710[no_slave].ampN = buf[9];
		data_PM710[no_slave].ampN = (data_PM710[no_slave].ampN << 8) | buf[10];
		if (data_PM710[no_slave].ampN == 32768)
			asli_PM710[no_slave].ampN = 0;
		else
			asli_PM710[no_slave].ampN = data_PM710[no_slave].ampN * satuan_amp;

		//urut_PM710 = 3;
	}
	else if (urt == 3)
	{
		//voltA_B, B_C, A_C, A_N, B_N & C_N
		data_PM710[no_slave].voltA_B = buf[3];
		data_PM710[no_slave].voltA_B = (data_PM710[no_slave].voltA_B << 8) | buf[4];
		if (data_PM710[no_slave].voltA_B == 32768)
			asli_PM710[no_slave].voltA_B = 0;
		else
			asli_PM710[no_slave].voltA_B = data_PM710[no_slave].voltA_B * satuan_volt;

		data_PM710[no_slave].voltB_C = buf[5];
		data_PM710[no_slave].voltB_C = (data_PM710[no_slave].voltB_C << 8) | buf[6];
		if (data_PM710[no_slave].voltB_C == 32768)
			asli_PM710[no_slave].voltB_C = 0;
		else
			asli_PM710[no_slave].voltB_C = data_PM710[no_slave].voltB_C * satuan_volt;

		data_PM710[no_slave].voltA_C = buf[7];
		data_PM710[no_slave].voltA_C = (data_PM710[no_slave].voltA_C << 8) | buf[8];
		if (data_PM710[no_slave].voltA_C == 32768)
			asli_PM710[no_slave].voltA_C = 0;
		else
			asli_PM710[no_slave].voltA_C = data_PM710[no_slave].voltA_C * satuan_volt;

		data_PM710[no_slave].voltA_N = buf[9];
		data_PM710[no_slave].voltA_N = (data_PM710[no_slave].voltA_N << 8) | buf[10];
		if (data_PM710[no_slave].voltA_N == 32768)
			asli_PM710[no_slave].voltA_N = 0;
		else
			asli_PM710[no_slave].voltA_N = data_PM710[no_slave].voltA_N * satuan_volt;

		data_PM710[no_slave].voltB_N = buf[11];
		data_PM710[no_slave].voltB_N = (data_PM710[no_slave].voltB_N << 8) | buf[12];
		if (data_PM710[no_slave].voltB_N == 32768)
			asli_PM710[no_slave].voltB_N = 0;
		else
			asli_PM710[no_slave].voltB_N = data_PM710[no_slave].voltB_N * satuan_volt;

		data_PM710[no_slave].voltC_N = buf[9];
		data_PM710[no_slave].voltC_N = (data_PM710[no_slave].voltC_N << 8) | buf[10];
		if (data_PM710[no_slave].voltC_N == 32768)
			asli_PM710[no_slave].voltC_N = 0;
		else
			asli_PM710[no_slave].voltC_N = data_PM710[no_slave].voltC_N * satuan_volt;

		//urut_PM710 = 0;
	}
	else if (urt == 4)
	{
		//memcpy((char *) &asli_PM710[no_slave].frek, &buf[2], 4);
		
		// kWh
		data_PM710[no_slave].kwh = buf[5];
		data_PM710[no_slave].kwh = (data_PM710[no_slave].kwh << 8) | buf[6];
		data_PM710[no_slave].kwh = (data_PM710[no_slave].kwh << 8) | buf[3];
		data_PM710[no_slave].kwh = (data_PM710[no_slave].kwh << 8) | buf[4];
		asli_PM710[no_slave].kwh = data_PM710[no_slave].kwh * satuan_kwh;

		// kVAh
		data_PM710[no_slave].kvah = buf[9];
		data_PM710[no_slave].kvah  = (data_PM710[no_slave].kvah  << 8) | buf[10];
		data_PM710[no_slave].kvah  = (data_PM710[no_slave].kvah  << 8) | buf[7];
		data_PM710[no_slave].kvah  = (data_PM710[no_slave].kvah  << 8) | buf[8];
		asli_PM710[no_slave].kvah = data_PM710[no_slave].kvah * satuan_kwh;

		// kVArh
		data_PM710[no_slave].kvarh = buf[11];
		data_PM710[no_slave].kvarh  = (data_PM710[no_slave].kvarh  << 8) | buf[12];
		asli_PM710[no_slave].kvarh = data_PM710[no_slave].kvarh * satuan_kwh;

		// kW, ini harusnya salah posisi
		data_PM710[no_slave].kw = buf[13];
		data_PM710[no_slave].kw  = (data_PM710[no_slave].kw  << 8) | buf[14];
		if (data_PM710[no_slave].kw  == 32768)
			asli_PM710[no_slave].kw = 0;
		else
			asli_PM710[no_slave].kw = data_PM710[no_slave].kw * satuan_kw;
	
		//urut_PM710 = 1;
	}
	else if (urt == 5)
	{
		// data angin
		wind_speed = buf[3];
		wind_speed = (wind_speed << 8) | buf[4];
		
		/* ubah ke m/s dari MPH */
		f_wind_speed = wind_speed * 0.44704;
		
		wind_dir = buf[5];
		wind_dir = (wind_dir << 8) | buf[6];	
		
		//asli_PM710[no_slave].ampB = wind_speed;
		//sli_PM710[no_slave].ampC = wind_dir;
	}
	else if (urt == 6)
	{
		// satuan angin
		wind_satuan = buf[3];
		wind_satuan = (wind_satuan << 8) | buf[4];
		
		// retransmision value
		wind_speed_tr = buf[9];
		wind_speed_tr = (wind_speed_tr << 8) | buf[10];
		
		wind_dir_tr = buf[11];
		wind_dir_tr = (wind_dir_tr << 8) | buf[12];
		
		//asli_PM710[no_slave].voltB_C = wind_satuan;
	}
	
}

/*
// 22 Jan 09
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
   pmod.jum_lo = (unsigned char) (uk & 0x00FF);;

   dcrc = usMBCRC16((unsigned char *) &pmod, sizeof (pmod)-2);
   pmod.crc_lo = (unsigned char) ((dcrc & 0xFF00) >> 8);
   pmod.crc_hi = (unsigned char) (dcrc & 0x00FF);;

   	return (1 + 1 + 1 + (uk * 2) + 2);
}*/
#endif

//---------------------------------------------------------------------------
#pragma package(smart_init)
