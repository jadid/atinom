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

#include "FreeRTOS.h"
#include "hardware.h"
#include "monita/monita_uip.h"

#ifndef PM710H
#define PM710H

//struct d_PM710 data_PM710[2];
//struct f_PM710 asli_PM710[2];

//struct d_PM710 data_PM710[JML_SUMBER];
//struct f_PM710 asli_PM710[JML_SUMBER];

#ifdef PAKAI_KTA
unsigned short 	wind_speed;
unsigned short 	wind_dir;
float 			f_wind_speed;

unsigned short wind_satuan;
unsigned short wind_speed_tr;		// retransmision value
unsigned short wind_dir_tr;
#endif

extern unsigned char buf_rx[];
#define buf	buf_rx
extern struct d_pmod pmod;

#define TUNGGU_PM_TX	50
#define TUNGGU_PM_RX	80

#define MOD_SERVER
#define T10_des2		0.01
#define T11_des3		0.001
#define T12_des4		0.0001


#ifdef PAKAI_MAX485
	#define HD	sizeof(pmod)
	//#define HD	0
#else
	#define HD	0
#endif

//#define addr_PM710      2
#define command_baca    0x03	//4
#define command_baca_micom 0x04

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

#if 0
#ifdef TIPE_MICOM_M300
	#define info_micom				30001		//
	#define meter_voltage_micom		30454		// 11 data: VA, VB, VC, VAB, VBC, VCA, IA, IB, IC, IN, frek
	#define meter_power_each_micom	30483		//  9 data: kwA, kwB, kwC, kvarA, kvarB, kvarC, kvaA, kvaB, kvaC
	#define meter_power_micom		30501		//  4 data: Tot P, Q, S, pf 
	#define meter_energi_micom		30508		//    data: wh, varh
#endif	// TIPE_MICOM_M300
#endif

#ifdef TIPE_MICOM_M300

	#define comand_baca_micom 0x04
	#define model_number 1
	#define meter_voltage_micom 454
	#define meter_current_micom 466
	#define meter_frek_micom 474

	#define meter_power_micom     501
	#define meter_pf_micom    507

	#define meter_energi_micom   508 

	//pembacaan skaal VT/CT
	#define reg_ct	42	//4044
	#define reg_vt	44	//4042

#endif //TIPE_MICOM_300

#ifdef TIPE_MICOM_P127

	#define comand_baca_127 0x03
	#define model_number_p127 1
	#define meter_vrms_p127 0x0080
	#define meter_angle_p127 0x00B2	//Digunakan untuk menghitung VAB dari VA-N 
	#define meter_current_p127 0x0030
	#define meter_frek_p127 0x003E

	#define meter_power_p127     0x009E
	#define meter_pf_p127    0x00A2

	#define meter_energi_p127   0x0061 
	#define meter_kva_p127 0x00AB

	//pembacaan skaal VT/CT
	#define reg_ct	42	
	#define reg_vt	44	

#endif //TIPE_MICOM_P127

#ifdef TIPE_ION8600
	#define comand_baca_ION8600 0x03

	#define meter_current_ION8600    150		// 40150 s/d 40158 =  8 register
	#define meter_voltage_ION8600    166		// 40166 s/d 40187 = 11 register
	#define meter_power_ION8600      198		// 1140
	#define meter_faktor_ION8600     262		// 1160
	#define meter_energi_ION8600     230    	// 1700
	#define reg_frek_ION8600         159	 	// 1180 x
	//#define meter_energi_vah_ION8600 1725    	// 1724

	//#define reg_kwh_ION8600   4000     // 2 register
	#define reg_satuan_ION8600      3210		//3209 x
#endif //TIPE_ION8600

//Maret 2012
#ifdef TIPE_A2000
	#define comand_baca_A2000 0x03
	
	#define meter_phase_voltage_A2000    0000	 // 0x0000 s/d 0x0005 = 6 register
	#define meter_delta_voltage_A2000    0x0100	 // 0x0100 s/d 0x0105 = 6 register
	#define meter_phase_current_A2000    0x0200	 // 0x0200 s/d 0x0205 = 6 register
	
	#define meter_active_power_A2000     0x0400	 // 0x0400 s/d 0x0407 = 8 register
	#define meter_reactive_power_A2000   0x0500	 // 0x0500 s/d 0x0507 = 8 register
	#define meter_apparent_power_A2000   0x0600	 // 0x0500 s/d 0x0507 = 8 register
	
	#define meter_frek_A2000         	 0x0f00	 // 0x0F00 			  = 1 register
	
	#define meter_netral_current_A2000  0x0D00	 // 0x0D00 s/d 0x0D03 = 4 register	
	
	#define meter_faktor_A2000     		0x0700	 // 0x0700 s/d 0x0707 = 8 register
	
	#define meter_energi_A2000     		0x0800 	//  0x0800 s/d 0x080F = 16 register
	
	//#define reg_satuan_A2000      3210		//3209 x
#endif //TIPE_A2000

#ifdef TIPE_TFX_ULTRA
	#define comand_baca_tfx 0x03
	
	#define tfx_integer				0x0064	 // 0x40100 s/d 0x40113  
	#define tfx_single_precision	0x0066	 // 0x40200 s/d 0x40213 
	#define tfx_double_precision	0x0068	 // 0x40200 s/d 0x40327 
	
	/*
	#define tfx_signal_strength    	0x40100	 // 0x0000 s/d 0x0005 = 2 register
	#define tfx_flowrate    		0x40102	 // 0x0100 s/d 0x0105 = 2 register
	#define tfx_net_totalizer    	0x40104	 // 0x0200 s/d 0x0205 = 2 register
	
	#define tfx_positive_totalizer	0x40106	 // 0x0400 s/d 0x0407 = 8 register
	#define tfx_negative_totalizer  0x40108	 // 0x0500 s/d 0x0507 = 8 register
	#define tfx_temperature_1   	0x40110	 // 0x0500 s/d 0x0507 = 8 register
	#define tfx_temperature_2       0x40112	 // 0x0F00 			  = 1 register
	*/
#endif //TIPE_TFX_ULTRA


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
   
   unsigned short kwh_receive;
   unsigned short kvarh_receive;
   unsigned short kvah_receive;

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

	//Parameter tambahan untuk KWH meter
   //#ifdef TIPE_ION8600
   float kwh_receive;
   float kvarh_receive;
   float kvah_receive;
   //#endif

   float pfA;
   float pfB;
   float pfC;

} ;

struct d_PM710 data_PM710[JML_SUMBER];
struct f_PM710 asli_PM710[JML_SUMBER];

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

#ifdef TIPE_MICOM_M300
unsigned int get_M300(int alamatPM, unsigned short reg, unsigned char uk);
#endif

#ifdef TIPE_MICOM_P127
unsigned int get_P127(int alamatPM, unsigned short reg, unsigned char uk);
#endif

#ifdef TIPE_ION8600
unsigned int get_ION8600(int alamatPM, unsigned short reg, unsigned char uk);
#endif

#ifdef TIPE_A2000
unsigned int get_A2000	(int alamatPM, unsigned short reg, unsigned char uk);
#endif

#ifdef TIPE_TFX_ULTRA
unsigned int get_tfx	(int alamatPM, unsigned short reg, unsigned char uk);
#endif
//---------------------------------------------------------------------------

void paket_modbus_rtu(unsigned char * x, unsigned char almt, unsigned char cmd, int regx, int jmlx);

#endif
