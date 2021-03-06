/*
   Seunebok Meulaboh
   11 Desember 2007

   komunikasi dengan Scheneider PM710

   furkan jadid
   daun biru engineering

   ketelitian sementara menggunakan default pabrik

   8 januari 2008
   di edit untuk PM810f.h
   
   */




//---------------------------------------------------------------------------
#ifndef PM710H
#define PM710H

//#define addr_PM710      2
#define command_baca    0x03	//4

#ifdef PAKAI_KTA
#define reg_KTA			1
#define reg_satuan_KTA			30
#endif
//#define TIPE_PM810

#ifdef TIPE_PM710
//#define JML_REQ_PM	6
#define reg_test_710	7006
//#define reg_kwh   1000     // 2 register		//harusnya 4000
#define reg_kwh_710   4001     // 2 register		//harusnya 4000
#define reg_kvah_710  4002     // 2 register
#define reg_kvarh_710  4004    // 2 reactive energy consumption
#define reg_kw_710    4006     // total real power
#define reg_kva_710   4007     // total apparent power
#define reg_kvar_710  4008     // total reactive power
#define reg_pf_710    4009     // total power faktor
#define reg_volt1_710 4010     // voltage L-L, ave of 3 phase
#define reg_volt2_710  4011    // voltage L-N, ave of 3 phase
#define reg_amp_710   4012     // average 3 phase
#define reg_frek_710  4013
#define reg_ampA_710  4020
#define meter_current_710 reg_ampA_710
#define reg_voltA_C_710  4030
#define reg_kwA_710  4036

#define reg_frek_f_710	1000

#define reg_satuan_710 	4105
//#define reg_satuan 	1
#define reg_speed_710 	7006

#define meter_voltage_710	4010
#define meter_voltage2_710	4011
#define meter_power_710		4006
#define meter_faktor_710	4009
#define meter_energi2_710	4000
#define meter_energi_vah_710	4002

#endif // tipe_PM710

#ifdef TIPE_PM810
//#define JML_REQ_PM	8
#define meter_current_810    1100		// 1100
#define meter_voltage_810    1120		// 1120
#define meter_power_810      1140		// 1140
#define meter_faktor_810     1160		// 1160
#define meter_energi2_810    1701    	// 1700
#define reg_frek_810         1180	 	// 1180 x
#define meter_energi_vah_810 1725    	// 1724

//#define reg_kwh_810   4000     // 2 register

#define reg_satuan_810      3210		//3209 x

#endif //TIPE_PM810

struct d_pmod {
      unsigned char addr;
      unsigned char command;
      unsigned char reg_hi;
      unsigned char reg_lo;
      unsigned char jum_hi;
      unsigned char jum_lo;
      unsigned char crc_hi;
      unsigned char crc_lo;
};


struct d_PM710 {
   unsigned int kwh;		// 41
   unsigned int kvah;
   unsigned int kvarh;		// 43
   unsigned short kw;
   unsigned short kva;		// 45
   unsigned short kvar;
   unsigned short pf;		// 47
   unsigned short volt1;   // volt rata2 fasa - fasa
   unsigned short volt2;   // volt fasa - netral
   unsigned short amp;		// 50
   unsigned short frek;

   unsigned short ampA;
   unsigned short ampB;		//  53. wind_speed
   unsigned short ampC;		//  54. wind_dir
   unsigned short ampN;
   
   unsigned short voltA_B;		// 56
   unsigned short voltB_C;		// 57
   unsigned short voltA_C;	// 58
   unsigned short voltA_N;	// 59
   unsigned short voltB_N;	// 60
   unsigned short voltC_N;

   unsigned short kwA;
   unsigned short kwB;
   unsigned short kwC;

   unsigned short kvaA;
   unsigned short kvaB;
   unsigned short kvaC;

   unsigned short kvarA;
   unsigned short kvarB;
   unsigned short kvarC;

   unsigned short pfA;
   unsigned short pfB;
   unsigned short pfC;
} ;

/*
struct f_PM710 {
   double kwh;
   double kvah;
   double kvarh;
   double kw;
   double kva;
   double kvar;
   double pf;
   double volt1;
   double volt2;
   double amp;
   double frek;

   double ampA;   //current instaneus phase A
   double ampB;
   double ampC;
   double ampN;   // current instantaneus netral

   double voltA_B;
   double voltB_C;
   double voltA_C;
   double voltA_N;
   double voltB_N;
   double voltC_N;

   double kwA;
   double kwB;
   double kwC;

   double kvaA;
   double kvaB;
   double kvaC;

   double kvarA;
   double kvarB;
   double kvarC;

   double pfA;
   double pfB;
   double pfC;

} ;
*/

struct f_PM710 {
   float kwh;		// 0
   float kvah;
   float kvarh;
   float kw;
   float kva;		
   float kvar;
   float pf;
   float volt1;
   float volt2;
   float amp;
   float frek;		// 10

   float ampA;   // current instaneus phase A
   float ampB;	 // masukkan data angin speed (12)
   float ampC;	 // masukkan data angin direction
   float ampN;   // current instantaneus netral

   float voltA_B;
   float voltB_C;	// satuan angin
   float voltA_C;
   float voltA_N;
   float voltB_N;
   float voltC_N;		//20

   float kwA;
   float kwB;
   float kwC;

   float kvaA;
   float kvaB;
   float kvaC;

   float kvarA;
   float kvarB;
   float kvarC;

   float pfA;
   float pfB;
   float pfC;

} ;


//17 januari 2008
struct dpm_eth {
     char mon[8];             // id bahwa data monita
     unsigned int nomer;      // urutan pengiriman data
     unsigned char flag;      // belum di definisikan
     unsigned char alamat;    // alamat stacking board (address power meter)
     char buf[232];           // data (biasanya float dimasukkan)
};

struct t_kontrol_PM {
     unsigned char alamat;      // alamat PM
     unsigned char konek;       // tersambung atau tidak
     unsigned char baru;        // data baru atau tidak
};

unsigned int get_PM710(int alamatPM, unsigned short reg, unsigned char uk);

//---------------------------------------------------------------------------
#endif
