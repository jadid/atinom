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
#define reg_KTA			1
#define reg_satuan_KTA			30

//#define TIPE_PM810

#ifdef TIPE_PM710

#define reg_test	7006
#define reg_kwh   4000     // 2 register
#define reg_kvah  4002     // 2 register
#define reg_kvarh  4004    // 2 reactive energy consumption
#define reg_kw    4006     // total real power
#define reg_kva   4007     // total apparent power
#define reg_kvar  4008     // total reactive power
#define reg_pf    4009     // total power faktor
#define reg_volt1 4010     // voltage L-L, ave of 3 phase
#define reg_volt2  4011    // voltage L-N, ave of 3 phase
#define reg_amp   4012     // average 3 phase
#define reg_frek  4013
#define reg_ampA  4020
#define meter_current reg_ampA
#define reg_voltA_C  4030

#define reg_frek_f	1000

#define reg_satuan 	4105
//#define reg_satuan 	1
#define reg_speed 	7006

#define meter_voltage	4010
#define meter_voltage2	4011
#define meter_power		4006
#define meter_faktor	4009
#define meter_energi2	4000
#define meter_energi_vah	4002

#endif // tipe_PM710

#ifdef TIPE_PM810

#define meter_current   1100
#define meter_voltage   1120
#define meter_power     1140
#define meter_faktor    1160
#define meter_energi2    1701    //harusnya 1700
#define reg_frek        1180
#define meter_energi_vah 1725    //harusnya 1724

#define reg_kwh   4000     // 2 register

#define reg_satuan      3209

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
   unsigned int kwh;
   unsigned int kvah;
   unsigned int kvarh;
   unsigned short kw;
   unsigned short kva;
   unsigned short kvar;
   unsigned short pf;
   unsigned short volt1;   // volt rata2 fasa - fasa
   unsigned short volt2;   // volt fasa - netral
   unsigned short amp;
   unsigned short frek;

   unsigned short ampA;
   unsigned short ampB;
   unsigned short ampC;
   unsigned short ampN;

   unsigned short voltA_B;
   unsigned short voltB_C;
   unsigned short voltA_C;
   unsigned short voltA_N;
   unsigned short voltB_N;
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
/*
struct dpm_eth {
   unsigned int nomer;
   unsigned short flag;	   //10 - 20 (data 10 kanal & 20 kanal), 30 untuk data power meter
   unsigned char alamat;
   char buf[232];
}; */

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

unsigned int get_PM710(unsigned short reg, unsigned char uk);

//---------------------------------------------------------------------------
#endif
