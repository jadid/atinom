/*

	menampilkan info2 scrolling di LCD
	
	14 Nov 2007
	furkan jadid, daun biru enginering
	
	Jan 2009
	Mulai diport ke FreeRTOS
	
	20 Feb 2009
	diberi menu system 
	
	20 Jan 2010
	Model tampilan baru (LIPI Malingping)
	
	*/

#include <stdio.h>
#include "../../monita/monita_uip.h"
#include "../../tinysh/enviro.h"

#ifdef PAKAI_RTC
#include "../../../system/rtc.h"
#endif


extern struct t_sumber sumber[];
//extern struct t_mesin	mesin[];
extern struct t_titik	titik[];
extern struct t_status status[];
extern struct t_data_hitung data_hitung[];
extern unsigned char daytime[32];

unsigned short x;
unsigned short y;	


#define menu_kiri 		8
#define menu_kanan		65
#define menu_tinggi		20
#define menu_jarak_atas 6
#define menu_top			80		// 40
#define menu_antara		3
//#define menu_besar_tinggi	200
#define menu_besar_tinggi	225
#define menu_besar_kanan	318
#define menu_besar_top	22			// 30
#define menu_besar_bot	238
#define judul_top	7

#define mesin_kiri	20		// 70
#define mesin_top	8
#define mesin_width	100		// 50

void menu_monita(unsigned char p);
void menu_pilih(unsigned char p, unsigned char mesin, unsigned char flag);
void kotak_bolong(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2);
void menu_charge(unsigned int mes);
void menu_pelumas(unsigned int mes);
void menu_jacket(unsigned int mes);
void menu_exhaust(unsigned int mes);
void menu_generator(unsigned int mes);
void menu_bbakar(unsigned int mes, unsigned int nomer_mesin);
void menu_setting(unsigned int ttk);
void menu_titik(unsigned int ttk, unsigned char flag);
void menu_system(void);
static void menu_log(void);

unsigned char isi_sumber[20];	//sambungan yang aktif

static unsigned int enable_log;
#include "debug_layar.c"

extern unsigned char tek[];

void init_scroll(void)
{
	x = 20;
	y = 200;
}
	
void print_scroll(char *s)
{
	y = y+10;
	if (y > 238) init_scroll();
} 

void kotak_pesan(void)
{
	kotak3d(0, 198, 318, 238);
	update_lcd(); 
}

void tabel_data(void)
{
	kotak3d(0, 25, 318, 196);
	teks_layar(5, 28, "Titik");
	teks_layar(50, 28, "Data");
	//garis medatar
	move_ke(1, 37); line_ke(318, 37); 
   	move_ke(45, 26); line_ke(45, 195);
}

void menu_monita(unsigned char p)
{  
   	if (enable_log == 1)
	{
		cls_layar();
		enable_log = 0;
		update_lcd_layer2();
	}
   	
/*   	
	teks_layar(mesin_kiri, mesin_top, mesin[0].nama);	
	teks_layar(mesin_kiri+(mesin_width), mesin_top, mesin[1].nama);
	teks_layar(mesin_kiri+(mesin_width*2), mesin_top, mesin[2].nama);	
	teks_layar(mesin_kiri+(mesin_width*3), mesin_top, mesin[3].nama);
	teks_layar(mesin_kiri+(mesin_width*4), mesin_top, mesin[4].nama);	
//*/
}

void kotak_bolong(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2)
{
	/*
	move_ke(x1,y1);
   	line_ke(x1,y2);
   	line_ke(x2,y2);
   	move_ke(x2,y1);
   	line_ke(x1,y1);
   
   	//kotak besar
   	move_ke(x2, y1); line_ke(x2, menu_besar_top); 
   	//move_ke(x2, menu_besar_top);
   	move_ke(menu_besar_kanan, menu_besar_top); line_ke(menu_besar_kanan, menu_besar_tinggi);
   	line_ke(x2, menu_besar_tinggi); line_ke(x2, y2);
   */
   
   //garis kesan 3d
   /*
   move_ke(x1+1,y2+1);
   line_ke(x2+1,y2+1);
   
   line_ke(x2+1, 201);
   line_ke(319, 201);
   */
}

/* 	
	p adalah group, misalnya charge air dll, 
	p ini berdasarkan tombol naik turun
	
	mesin adalah unitnya, bedasarkan tombol kanan
 	
 	flag : jika diakses dari menu lebih dalam 
 	
 	*/
	
#define MWID	12

void menu_pilih(unsigned char p, unsigned char grop, unsigned char flag)
{
	int lebar;
	int len;
	int i;
	
	/*
	if (grop == 0)
		sprintf(tek, "Kompresor");
	if (grop == 1)
		sprintf(tek, "Turbin");
	if (grop == 2)
		sprintf(tek, "Sel Surya");
	if (grop == 4)
		sprintf(tek, "Penduduk");
	if (grop == 5)
		sprintf(tek, "Baterai");
	//*/
	struct t_group *pg;
	pg = (char *) ALMT_GROUP;
	len = strlen( pg[ grop ].nama );
	
	if ((len <= 0) || ( len > 10 ))
	{
		sprintf(tek, "Default !");
	}
	else
		sprintf(tek, "%s", pg[ grop ].nama);
	
	if (len <= 0) len = 1;
	lebar = (len+1) * 14;	// 12 adalah lebar font 
	lebar = 150;
	
	/* print keterangan di bagian bawah */
	teks_layar(menu_kiri + 3, menu_besar_bot - 9, pg[ grop ].ket);
	
	// garis2 tebal 2 pixel
	move_ke(menu_besar_kanan, menu_besar_top);
	line_ke(menu_besar_kanan, menu_besar_bot);
	
	move_ke(menu_besar_kanan+1, menu_besar_top);
	line_ke(menu_besar_kanan+1, menu_besar_bot);
	
	line_ke(menu_kiri, menu_besar_bot);
	move_ke(menu_kiri, menu_besar_bot+1);
	line_ke(menu_besar_kanan+1, menu_besar_bot+1);
	
	move_ke(menu_kiri, menu_besar_bot);
	line_ke(menu_kiri, menu_besar_top);
	move_ke(menu_kiri-1, menu_besar_bot+1);
	line_ke(menu_kiri-1, menu_besar_top);
	
	/* penutup nama group yang berpindah2 sesuai pencetan */
	line_ke(mesin_kiri + ( MWID * grop ), menu_besar_top );
	move_ke(menu_kiri-1, menu_besar_top - 1);
	line_ke(mesin_kiri + ( MWID * grop ), menu_besar_top - 1);
			
	line_ke(mesin_kiri + ( MWID * grop ), 2);
	move_ke(mesin_kiri + ( MWID * grop ) - 1, menu_besar_top - 1);
	line_ke(mesin_kiri + ( MWID * grop ) - 1, 2);
	
	line_ke(mesin_kiri + ( MWID * grop) + lebar, 2);
	move_ke(mesin_kiri + ( MWID * grop) - 1, 1);
	line_ke(mesin_kiri + ( MWID * grop) + lebar, 1);
	
	line_ke(mesin_kiri + ( MWID * grop) + lebar, menu_besar_top);
	move_ke(mesin_kiri + ( MWID * grop) + lebar - 1, 1);
	line_ke(mesin_kiri + ( MWID * grop) + lebar - 1, menu_besar_top);
	
	line_ke(menu_besar_kanan, menu_besar_top);
	move_ke(mesin_kiri + ( MWID * grop) + lebar - 1, menu_besar_top - 1);
	line_ke(menu_besar_kanan, menu_besar_top - 1);
	
	/* tutup nama group yang bawah */
	move_ke(mesin_kiri + ( MWID * grop ), menu_besar_top );
	line_ke(mesin_kiri + ( MWID * grop ), menu_besar_top + 7);
	move_ke(mesin_kiri + ( MWID * grop ) - 1, menu_besar_top );
	line_ke(mesin_kiri + ( MWID * grop ) - 1, menu_besar_top + 7);
	
	line_ke(mesin_kiri + ( MWID * grop ) + lebar, menu_besar_top + 7);
	move_ke(mesin_kiri + ( MWID * grop ) - 1, menu_besar_top + 6);
	line_ke(mesin_kiri + ( MWID * grop ) + lebar, menu_besar_top + 6);
	
	line_ke(mesin_kiri + ( MWID * grop ) + lebar, menu_besar_top );
	move_ke(mesin_kiri + ( MWID * grop ) + lebar - 1, menu_besar_top );
	line_ke(mesin_kiri + ( MWID * grop ) + lebar - 1, menu_besar_top + 7);
	
	/* bikin semacam tab book kanan */
	for (i = grop ; i<9; i++)
	{
		move_ke(mesin_kiri + ( MWID * i ) + lebar + 3, 2 );
		line_ke(mesin_kiri + ( MWID * (i + 1)) + lebar, 2 );
		line_ke(mesin_kiri + ( MWID * (i + 1)) + lebar, menu_besar_top - 4 );
	}
	/* tab book kiri */
	for (i = 0; i< grop ; i ++)
	{
		move_ke(mesin_kiri + ( MWID * (i+1) ) - 3, 2 );
		line_ke(mesin_kiri + ( MWID * i), 2 );
		line_ke(mesin_kiri + ( MWID * i), menu_besar_top - 4 );
	}
	
	teks_arial(mesin_kiri + 16 + ( MWID * grop ), judul_top, tek);
	
	
	/* JAM ditampilkan */
	#ifdef PAKAI_RTC
	RTCTime wt;
	
	p_rtc_get_time( &wt );
	sprintf(tek," %02d:%02d  %02d-%02d-%d", wt.RTC_Hour, wt.RTC_Min, \
		wt.RTC_Mday, wt.RTC_Mon, wt.RTC_Year);
		
	teks_layar(menu_kiri + 197, menu_besar_bot - 9, tek);
	
	#endif
}

#define DATA_KIRI	20
#define DATA_ATAS	35
#define DATA_KIRI_KOMIK	DATA_KIRI
#define DATA_ATAS_KOMIK DATA_ATAS
#define DATA_TINGGI	18

extern float data_f[];

void cetak_data( unsigned char grop, int idx )
{
	sprintf(tek, "nomer %d", idx);
	
	//teks_layar( DATA_KIRI, DATA_ATAS + (8 * idx), tek);
	teks_komik( DATA_KIRI_KOMIK, DATA_ATAS_KOMIK + ( 14 * idx ), tek);
}

void menu_group(unsigned char p, unsigned char grop)
{
	int i;
	struct t_group *p_gr;
	struct t_dt_set *p_dt;
	int jml=0;
	int temp;
	
	/*
	move_ke( DATA_KIRI, 36);
	line_ke( menu_besar_kanan - 10, 36);
	move_ke( DATA_KIRI, 37);
	line_ke( menu_besar_kanan - 10, 37);
	*/
	
	/*
	for (i=0; i<7; i++)
	{
		cetak_data( grop, i );
	}*/
	p_gr = (char *) ALMT_GROUP;
	p_dt = (char *) ALMT_DT_SET;
	
	teks_layar ( DATA_KIRI_KOMIK, DATA_ATAS_KOMIK, "Nama" );
	teks_layar ( DATA_KIRI_KOMIK + 120, DATA_ATAS_KOMIK, "Data" );
	teks_layar ( DATA_KIRI_KOMIK + 180, DATA_ATAS_KOMIK, "Satuan" );
	teks_layar ( DATA_KIRI_KOMIK + 220, DATA_ATAS_KOMIK, "Alarm H / HH" );
//*	
	for (i=0; i<40; i++)
	{
		temp = p_gr[grop].no_data[i];
		if (  temp != 0 )
		{
			jml++;
			//printf("nilai temp: %d\r\n", temp);
			//temp--;		// array dimulai dari nol
			
			//sprintf(tek, "nomer data %d, %s", temp, p_dt[ temp - 1)].nama );
			sprintf(tek, "%s", p_dt[ temp - 1].nama );
			teks_komik( DATA_KIRI_KOMIK, DATA_ATAS_KOMIK + ( DATA_TINGGI * jml ), tek);  // 14 terlalu mepet
			
			//data_f[ temp - 1 ] = (float) ((rand() % 100) * 0.10);
			if ((temp-1)< JML_SUMBER*PER_SUMBER) {
				sprintf(tek, "%.2f", data_f[ temp - 1] );
			} else {
				sprintf(tek, "%s", (int) data_f[temp-1]?"Aktif":"Mati" );
			}
			teks_komik( DATA_KIRI_KOMIK + 115, DATA_ATAS_KOMIK + ( DATA_TINGGI * jml ), tek); 
			
			if ((temp-1)< JML_SUMBER*PER_SUMBER) {
				sprintf(tek, "(%s)", p_dt[ temp - 1].satuan );
				teks_layar( DATA_KIRI_KOMIK + 180, 8 + DATA_ATAS_KOMIK + ( DATA_TINGGI * jml ), tek);
				
				// Batas ALARM //
				//sprintf(tek, "%2.2f", p_dt[ temp - 1].alarm_L );
				sprintf(tek, "%2.1f", p_dt[ temp - 1].alarm_H );
				teks_layar( DATA_KIRI_KOMIK + 220, 8 + DATA_ATAS_KOMIK + ( DATA_TINGGI * jml ), tek);
				
				//sprintf(tek, "%2.2f", p_dt[ temp - 1].alarm_H );
				sprintf(tek, "%2.1f", p_dt[ temp - 1].alarm_HH );
				teks_layar( DATA_KIRI_KOMIK + 255, 8 + DATA_ATAS_KOMIK + ( DATA_TINGGI * jml ), tek);
				//teks_komik( DATA_KIRI_KOMIK + 240, DATA_ATAS_KOMIK + ( DATA_TINGGI * jml ), tek);
			}
			if (jml > 8) break;
		}
	}
//*/
}

void menu_setting(unsigned int ttk)
{
	int i;
	int y;
	int tot;
	//char bb[32];

	
	//teks_layar(85, 40, "Sumber Data");
	teks_arial(menu_kanan+menu_kiri+4, menu_besar_tinggi-18, "Sumber Data");
	
	// printout sumber data berdasarkan nomer mesin
	tot = 0;
	
	#if 1
	for (i=0; i<JML_SUMBER; i++)
	{
		for (y= ttk ; y< ((ttk+1)+TIAP_MESIN) ; y++)
		{
			if (titik[y].ID_sumber == (i+1))
			{
				sprintf(tek, "%2d %s", (i+1), sumber[i].nama);
				teks_layar(70, 27 + (tot*9), tek);
		
				// print out IP
				sprintf(tek,"%d.%d.%d.%d", sumber[i].IP0, sumber[i].IP1, sumber[i].IP2, sumber[i].IP3);
				teks_layar(170, 27 + (tot*9), tek);	
				
				#ifdef BOARD_TAMPILAN
				// reply count
				sprintf(tek, "%d", status[i].reply);
				teks_layar(270, 27 + (tot*9), tek);
				#endif
				
				tot++;
				break;
			}	
		}
	}
	#endif
}

void menu_sumber(unsigned int ttk)
{
	int i;
	int jum_aktif=0;

	//teks_arial(menu_kanan+menu_kiri+4, menu_besar_tinggi-18, "Sumber Data");
	
	// tampilkan yang aktif saja !
	for (i=0; i<JML_SUMBER; i++)
	{
		#ifdef BOARD_TAMPILAN
		if (sumber[i].status == 1 || sumber[i].status == 5)
		{
			sprintf(tek, "%2d %s", (i+1), sumber[i].nama);
			teks_layar(72, 27 + (jum_aktif*9), tek);
		
			// print out IP
			sprintf(tek,"%d.%d.%d.%d", sumber[i].IP0, sumber[i].IP1, sumber[i].IP2, sumber[i].IP3);
			teks_layar(170, 27 + (jum_aktif*9), tek);
		
		/*
		// status
		if (sumber[i].status == 0)
			teks_layar(270, 30 + (i*9), "Null");
		if (sumber[i].status == 1)
			teks_layar(270, 30 + (i*9), "Normal");
		if (sumber[i].status == 2)
			teks_layar(270, 30 + (i*9), "TimeOut");
		*/
			// jumlah reply 
			sprintf(tek, "%d", status[i].reply);
			teks_layar(270, 27 + (jum_aktif*9), tek);
			
			/* nomer sumber yang aktif disimpan */
			isi_sumber[jum_aktif] = i;
			
			jum_aktif++;
		}	
		#endif
	}
	
	if (jum_aktif == 0)
	{
		msg_box("Tidak ada sumber data aktif !");	
	}
	
	teks_layar(68, 216, "Tekan OK untuk melihat raw data");
}

void menu_titik(unsigned int ttk, unsigned char flag)
{
	int i;
	int plus;
	
	/* informasi titik sesuai dengan mesin yang dipilih */
	teks_layar(72, 27, "No.   Sbr  Knl  Almt Keterangan");

	if (flag < 15)
	{
		for (i=0; i<15; i++)
		{
			sprintf(tek, "%3d    %2d   %2d   %d", ttk+i+1, titik[ttk+i].ID_sumber, \
				titik[ttk+i].kanal, titik[ttk+i].alamat);
			
			teks_layar(72, 38 + (9 * i), tek);
		}
		/*
		for (i=0; i<15; i++)
		{
			sprintf(tek, "%s", keter[i]);	
			teks_layar(200, 38 + (9 * i), tek);
		}*/
	}
	else
	{
		plus = flag - 14;
		for (i=0; i<15; i++)
		{
			sprintf(tek, "%3d    %2d   %2d   %d", ttk+i+1+plus, titik[ttk+i+plus].ID_sumber, \
				titik[ttk+i+plus].kanal, titik[ttk+i+plus].alamat);
			
			teks_layar(72, 38 + (9 * i), tek);
		}
		/*
		for (i=0; i<15; i++)
		{
			sprintf(tek, "%s", keter[i+plus]);	
			teks_layar(200, 38 + (9 * i), tek);
		}*/		
	}
	
	teks_layar(68, 216, "Tekan OK & panah untuk scroll ke bawah");
}

#if 0
extern xTaskHandle *hdl_shell;
extern xTaskHandle *hdl_lcd;
extern xTaskHandle *hdl_led;
extern xTaskHandle *hdl_tampilan;
extern xTaskHandle *hdl_ether;
extern struct t_env env2;

void menu_system(void)
{
	unsigned int sec;
	unsigned int menit;
	unsigned int jam;
	unsigned int hari;
	unsigned int tahun;
	unsigned char up[64];
	
	sprintf(tek, "Monita %s v %s", NAMA_BOARD, VERSI_KOMON);
	teks_layar(80, 35, tek);
	teks_layar(80, 45, "CPU : LPC2368 60 MHz FreeRTOS 5.1.1");
	teks_layar(80, 55, "daun biru Engineering, Des 2008");	
	sprintf(tek, "ARM-GCC %s", __VERSION__);
	teks_layar(80, 65, tek);
	sprintf(tek, "CC : %s : %s", __DATE__, __TIME__);
	teks_layar(80, 75, tek);
	
	
	uptime(&sec, &menit, &jam, &hari, &tahun);
	memset(up, 0, 64);
	
	sprintf(tek, "Up : ");
	strcat(up, tek);
	
	if (tahun !=0)
	{
		sprintf(tek, "%d thn ", tahun);	
		strcat(up, tek);
	}
	if (hari !=0)
	{
		sprintf(tek, "%d hari ", hari);	
		strcat(up, tek);	
	}
	if (jam !=0)
	{
		sprintf(tek, "%d jam ", jam);
		strcat(up, tek);		
	}
	if (menit !=0)
	{
		sprintf(tek, "%d mnt ", menit);	
		strcat(up, tek);	
	}
		
	sprintf(tek, "%d dtk", sec);
	strcat(up, tek);	
	
	teks_layar(80, 85, up);
	
	teks_layar(80, 100, "Free Stack");
	sprintf(tek, "Shell    : %d", uxTaskGetStackHighWaterMark(hdl_shell));
	teks_layar(80, 110, tek); 
	sprintf(tek, "Led      : %d", uxTaskGetStackHighWaterMark(hdl_led));
	teks_layar(80, 120, tek);
	sprintf(tek, "Tampilan : %d", uxTaskGetStackHighWaterMark(hdl_tampilan));
	teks_layar(80, 130, tek); 
	sprintf(tek, "LCD      : %d", uxTaskGetStackHighWaterMark(hdl_lcd));
	teks_layar(80, 140, tek);
	sprintf(tek, "Ethernet : %d", uxTaskGetStackHighWaterMark(hdl_ether));
	teks_layar(80, 150, tek);   
	
	/* informasi environtment, nama, ip, gateway dll */
	sprintf(tek, "Nama     : %s", env2.nama_board);
	teks_layar(80, 170, tek);   
	sprintf(tek, "IP       : %d.%d.%d.%d", env2.IP0, env2.IP1, env2.IP2, env2.IP3);
	teks_layar(80, 180, tek);   
	sprintf(tek, "Gateway  : %d.%d.%d.%d", env2.GW0, env2.GW1, env2.GW2, env2.GW3); 
	teks_layar(80, 190, tek);
}
#endif

static void menu_log(void)
{


}


