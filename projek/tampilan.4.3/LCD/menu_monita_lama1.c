/*

	menampilkan info2 scrolling di LCD
	
	14 Nov 2007
	furkan jadid, daun biru enginering
	
	Jan 2009
	Mulai diport ke FreeRTOS
	
	20 Feb 2009
	diberi menu system 
	
	*/

#include <stdio.h>
#include "../monita/monita_uip.h"
#include "../tinysh/enviro.h"

char keter[100][25] __attribute__ ((section (".lokasi_font"))) = \
	{	"Charge air P kiri", \
		"Charge air P kanan", \
		"Charge air T kiri", \
		"Charge air T kanan", \
		"JW inlet P 1", \
		"JW inlet P 2", \
		"JW outlet P 1", \
		"JW outlet P 2", \
		"JW inlet T 1", \
		"JW inlet T 2", /* 10 */ \
		"JW outlet T 1", \
		"JW outlet T 2", \
		"LO inlet P 1", \
		"LO inlet P 2", \
		"LO outlet P 1", \
		"LO outlet P 2", \
		"LO inlet T 1", \
		"LO inlet T 2", \
		"LO outlet T 1", \
		"LO outlet T 2", /* 20 */ \
		"Exhaust T #1", \
		"Exhaust T #2", \
		"Exhaust T #3", \
		"Exhaust T #4", \
		"Exhaust T #5", \
		"Exhaust T #6", \
		"Exhaust T #7", \
		"Exhaust T #8", \
		"Exhaust T #9", \
		"Exhaust T #10", \
		"Exhaust T #11", \
		"Exhaust T #12", \
		"Exhaust T #13", \
		"Exhaust T #14", \
		"Exhaust T #15", \
		"Exhaust T #16", \
		"Exhaust T #17", \
		"Exhaust T #18", \
		"Exhaust T #19", \
		"Exhaust T #20", /* 40 */\
		"--", \
		"--", \
		"--", \
		"--", \
		"--", \
		"Engine RPM", \
		"Turbo RPM 1", \
		"Turbo RPM 2", \
		"Fuel inlet counter", \
		"Fuel outlet counter", /* 50 */\
		"Power (kW)", \
		"Energi (kWh)", \
		"Voltage (V)", \
		"Ampere (A)", \
		"frekuensi", \
		"--", \
		"--", \
		"--", \
		"--", \
		"--", /* 60 */\
		"--", \
		"--", \
		"--", \
		"--", \
		"--", \
		"--", \
		"--", \
		"--", \
		"--", \
		"--", /* 70 */\
		"--", \
		"--", \
		"--", \
		"--", \
		"--", \
		"--", \
		"--", \
		"--", \
		"--", \
		"--", /* 80 */\
		"--", \
		"--", \
		"--", \
		"--", \
		"--", \
		"--", \
		"--", \
		"--", \
		"--", \
		"--", /* 90 */\
		"--", \
		"--", \
		"--", \
		"--", \
		"--", \
		"--", \
		"--", \
		"--", \
		"--", \
		"--", /* 100 */\
		};

extern struct t_sumber sumber[];
extern struct t_mesin	mesin[];
extern struct t_titik	titik[];
extern struct t_status status[];
extern struct t_data_hitung data_hitung[];
extern unsigned char daytime[32];

unsigned short x;
unsigned short y;	

//extern struct d2_data loc_data;
//extern struct t_temp_net temp_net;
//extern struct d_masing masing[];

#define menu_kiri 		8
#define menu_kanan		65
#define menu_tinggi		20
#define menu_jarak_atas 6
#define menu_top			40
#define menu_antara		3
//#define menu_besar_tinggi	200
#define menu_besar_tinggi	225
#define menu_besar_kanan	318
#define menu_besar_top	20

#define mesin_kiri	70
#define mesin_top		8
#define mesin_width	50

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
	//teks_layar4(x, y, "                                          ");
   	//teks_layar4(x, y, s);
	
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
   	
   	//waktu
	sprintf(tek, "%s", daytime);
	//teks_layar(menu_kanan+menu_kiri-4, 11, s);
   	teks_layar(175, 232, tek);
   	//teks_layar(180, 232, "harusnya waktu");
   
   	teks_layar(menu_kiri, menu_top, "Charge");
	teks_layar(menu_kiri, menu_top+8, "Air");
	
	teks_layar(menu_kiri, menu_top+menu_tinggi+menu_antara, 	"Jacket");
	teks_layar(menu_kiri, menu_top+menu_tinggi+8+menu_antara,  "water");
	
	teks_layar(menu_kiri, menu_top+2*(menu_tinggi+menu_antara), 	 "Minyak");
	teks_layar(menu_kiri, menu_top+2*(menu_tinggi+menu_antara)+8, "Pelumas");
	
	teks_layar(menu_kiri, menu_top+3*(menu_tinggi+menu_antara), 	 "Temp.");
	teks_layar(menu_kiri, menu_top+3*(menu_tinggi+menu_antara)+8, "Exhaust");
	
	teks_layar(menu_kiri, menu_top+4*(menu_tinggi+menu_antara), 	 "Generator");
	teks_layar(menu_kiri, menu_top+4*(menu_tinggi+menu_antara)+8, "Load");
	
	teks_layar(menu_kiri, menu_top+5*(menu_tinggi+menu_antara), 	 "Bahan");
	teks_layar(menu_kiri, menu_top+5*(menu_tinggi+menu_antara)+8, "Bakar");
	
	// setting
	if (p == 6)
		teks_layar(menu_kiri, menu_top+6*(menu_tinggi+menu_antara)+3, "Setting");
	
	else if (p == 7)
		teks_layar(menu_kiri, menu_top+6*(menu_tinggi+menu_antara)+5, "Sumber");
	
	else if (p == 8)
		teks_layar(menu_kiri, menu_top+6*(menu_tinggi+menu_antara)+7, "Titik");
	
	else if (p == 9)
		teks_layar(menu_kiri, menu_top+6*(menu_tinggi+menu_antara)+8, "System");
	
	else if (p == 10)
	{
		teks_layar(menu_kiri, menu_top+6*(menu_tinggi+menu_antara)+9, "Log...");
		
		init_debug_layar(72);
		enable_log = 1;
	}
	//teks_layar(menu_kiri, menu_top+7*(menu_tinggi+menu_antara)+8, "---");
	
	
	// hard coded nama mesin
	/*
	teks_layar(mesin_kiri, mesin_top, "DEUTZ");	
	teks_layar(mesin_kiri+(mesin_width), mesin_top, "SWD #1");
	teks_layar(mesin_kiri+(mesin_width*2), mesin_top, "SWD #2");	
	teks_layar(mesin_kiri+(mesin_width*3), mesin_top, "MAK");
	teks_layar(mesin_kiri+(mesin_width*4), mesin_top, "MIRRLS");	
	*/
	teks_layar(mesin_kiri, mesin_top, mesin[0].nama);	
	teks_layar(mesin_kiri+(mesin_width), mesin_top, mesin[1].nama);
	teks_layar(mesin_kiri+(mesin_width*2), mesin_top, mesin[2].nama);	
	teks_layar(mesin_kiri+(mesin_width*3), mesin_top, mesin[3].nama);
	teks_layar(mesin_kiri+(mesin_width*4), mesin_top, mesin[4].nama);	
	
}

void kotak_bolong(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2)
{
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
void menu_pilih(unsigned char p, unsigned char mesin, unsigned char flag)
{
	unsigned int ttk;
	
	if (p < 6)
		kotak_bolong(2, 40-menu_jarak_atas+(p*(menu_tinggi+menu_antara)), menu_kanan, 40+menu_tinggi+(p*(menu_tinggi+menu_antara)));
	else
		kotak_bolong(2, 40-menu_jarak_atas+(6*(menu_tinggi+menu_antara))+(p-6)*2, menu_kanan, 40+menu_tinggi+(6*(menu_tinggi+menu_antara))+(p-6)*2);
	
	//kontak mesin
	move_ke(menu_kanan, menu_besar_top);
	line_ke(mesin_kiri-5 + (mesin_width*mesin), menu_besar_top); line_ke(mesin_kiri-5+(mesin_width*mesin), 2);
	line_ke(mesin_kiri-5 + (mesin_width*(mesin+1)), 2); 
	line_ke(mesin_kiri-5 + (mesin_width*(mesin+1)), menu_besar_top);
	line_ke(menu_besar_kanan, menu_besar_top); 
	
	if (mesin == 0 && p != 8)
		teks_arial(menu_besar_kanan-22, menu_besar_tinggi-18, "#1");
	if (mesin == 1 && p != 8)
		teks_arial(menu_besar_kanan-22, menu_besar_tinggi-18, "#2");
	if (mesin == 2 && p != 8)
		teks_arial(menu_besar_kanan-22, menu_besar_tinggi-18, "#3");
	if (mesin == 3 && p != 8)
		teks_arial(menu_besar_kanan-22, menu_besar_tinggi-18, "#4");
	if (mesin == 4 && p != 8)
		teks_arial(menu_besar_kanan-22, menu_besar_tinggi-18, "#5");
	
	ttk = TIAP_MESIN * mesin;
	
	if (p == 0) menu_charge(ttk);	
	else if (p == 1) menu_jacket(ttk);
	else if (p == 2) menu_pelumas(ttk);
	else if (p == 3) menu_exhaust(ttk);
	else if (p == 4) menu_generator(ttk);
	else if (p == 5) menu_bbakar(ttk, mesin);
	
	else if (p == 6) menu_setting(ttk);
	else if (p == 7) menu_sumber(ttk);
	else if (p == 8) menu_titik(ttk, flag);
	else if (p == 9) menu_system();
	else if (p == 10) menu_log();
	
}

void menu_charge(unsigned int mes)
{
	//char s[128];
	float f1;
	float f2;
	
	f1 = 0.21;
	f2 = 43.19;
	
	//teks_arial(180, 22, "Charge Air"); /* posisi kanan atas */
	teks_arial(menu_kanan+menu_kiri+4, menu_besar_tinggi-18, "Charge Air");

	sprintf(tek, "Pl = %3.2f bar", titik[mes + OFFSET_CAP_L].data);
	teks_arial(100, 50, tek); 
	
	sprintf(tek, "Tl = %3.2f C", titik[mes + OFFSET_CAT_L].data);
	teks_arial(100, 70, tek); 

	sprintf(tek, "Pr = %3.2f bar", titik[mes + OFFSET_CAP_R].data);
	teks_arial(100, 100, tek); 
	
	sprintf(tek, "Tr = %3.2f C", titik[mes + OFFSET_CAT_R].data);
	teks_arial(100, 120, tek); 
	
}

void menu_jacket(unsigned int mes)
{
	//char s[128];
	float f1;
	float f2;
	
	f1 = 2.35;
	f2 = 93.19;
	
	//teks_arial(180, 22, "Charge Air"); /* posisi kanan atas */
	teks_arial(menu_kanan+menu_kiri+4, menu_besar_tinggi-18, "Jacket Water");
	
	
	teks_layar(92, 41, "Inlet");
	sprintf(tek, "P = %3.2f bar", titik[mes + OFFSET_JWP_IN1].data);
	teks_arial(100, 50, tek); 
	
	sprintf(tek, "T = %3.2f C", titik[mes + OFFSET_JWT_IN1].data);
	teks_arial(100, 70, tek); 
	
	teks_layar(92, 91, "Outlet");
	sprintf(tek, "P = %3.2f bar", titik[mes + OFFSET_JWP_OUT1].data);
	teks_arial(100, 100, tek); 
	
	sprintf(tek, "T = %3.2f C", titik[mes + OFFSET_JWT_OUT1].data);
	teks_arial(100, 120, tek); 
	
}

void menu_pelumas(unsigned int mes)
{
	//char s[128];
	float f1;
	float f2;
	
	f1 = 4.56;
	f2 = 93.19;
	
	//teks_arial(180, 22, "Charge Air"); /* posisi kanan atas */
	teks_arial(menu_kanan+menu_kiri+4, menu_besar_tinggi-18, "Pelumas");
	
	teks_layar(92, 41, "Inlet");
	
	sprintf(tek, "P = %3.2f bar", titik[mes + OFFSET_LOP_IN1].data);
	teks_arial(100, 50, tek); 
	
	sprintf(tek, "T = %3.2f C", titik[mes + OFFSET_LOT_IN1].data);
	teks_arial(100, 70, tek); 

	teks_layar(92, 91, "Outlet");
	sprintf(tek, "P = %3.2f bar", titik[ mes + OFFSET_LOP_OUT1 ].data);
	teks_arial(100, 100, tek); 
	
	sprintf(tek, "T = %3.2f C", titik[ mes + OFFSET_LOT_OUT1 ].data);
	teks_arial(100, 120, tek);
	 
}

void menu_exhaust(unsigned int mes)
{
	//char s[128];
	char i;
	float f1;
	float f2;
	
	f1 = 4.56;
	f2 = 93.19;
	
	//teks_arial(180, 22, "Charge Air"); /* posisi kanan atas */
	teks_arial(menu_kanan+menu_kiri+4, menu_besar_tinggi-18, "Exhaust");
	
	
	for (i=1; i<9; i++)
	{
		sprintf(tek, "%d = %3.0f", i, titik[mes + OFFSET_EXHT1 + i - 1].data);
		teks_arial(85, 4+(i*19), tek); 
	}
	
	for (i=9; i<17; i++)
	{
		if (i==9)
	   sprintf(tek, "%d  = %3.0f", i, titik[mes + OFFSET_EXHT9].data);
		else
		sprintf(tek, "%d = %3.0f", i, titik[mes + OFFSET_EXHT1 + i - 1].data);
		
	   teks_arial(207, 4+((i-8)*19), tek); 
	}
	
	//sprintf(s, "T = %3.2f C", f2);
	//teks_arial(100, 70, s); 
}

void menu_generator(unsigned int mes)
{
	//char s[128];
	float f1;
	float f2;
	
	f1 = 4.56;
	f2 = 93.19;
	
	//teks_arial(180, 22, "Charge Air"); /* posisi kanan atas */
	teks_arial(menu_kanan+menu_kiri+4, menu_besar_tinggi-18, "Generator");
	
	sprintf(tek, "kW  = %3.2f", titik[mes + OFFSET_KW].data);
	teks_arial(85, 50, tek); 
	
	//sprintf(s, "n = %3.2f rpm", masing[mes].rpm);
	sprintf(tek, "kWh = %.0f ", titik[mes + OFFSET_KWH].data);
	teks_arial(85, 70, tek); 
	
	sprintf(tek, "f   = %3.2f Hz", titik[mes + OFFSET_FREK].data);
	teks_arial(85, 90, tek); 

	//sprintf(s, "pf  = %3.2f ", masing[mes].pf);
	//teks_arial(85, 110, s); 
	
	sprintf(tek, "Amp = %3.2f ", titik[mes + OFFSET_AMP].data);
	teks_arial(85, 130, tek);
	
	sprintf(tek, "Volt= %3.2f ", titik[mes + OFFSET_VOLT].data);
	teks_arial(85, 150, tek);

	sprintf(tek, "rpm = %3.2f ", titik[mes + OFFSET_GEN_RPM].data);
	teks_arial(85, 170, tek);
	
	//titik[mes].data);
}

void menu_bbakar(unsigned int mes, unsigned int nomer_mesin)
{
	float f1=0;
	float f2=0;
	
	teks_arial(menu_kanan+menu_kiri+4, menu_besar_tinggi-18, "Bahan Bakar");
	
	sprintf(tek, "In  = %4.0f", titik[mes + OFFSET_BB_IN].data);
	teks_arial(85, 50, tek); 
	
	sprintf(tek, "Out = %4.0f ", titik[mes + OFFSET_BB_OUT].data);
	teks_arial(85, 70, tek); 
	
	teks_layar(77, 91, "Per menit");
	sprintf(tek, "In  = %4.0f", data_hitung[ nomer_mesin ].bb_in);
	teks_arial(85, 100, tek); 
	
	sprintf(tek, "Out = %4.0f", data_hitung[ nomer_mesin ].bb_out);
	teks_arial(85, 120, tek);
	
	sprintf(tek, "kWh = %4.0f", data_hitung[ nomer_mesin ].kwh);
	teks_arial(85, 140, tek);
	
	sprintf(tek, "sfc = %4.0f", data_hitung[ nomer_mesin ].sfc);
	teks_arial(85, 160, tek);
	
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
	
		for (i=0; i<15; i++)
		{
			sprintf(tek, "%s", keter[i]);	
			teks_layar(200, 38 + (9 * i), tek);
		}
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
		
		for (i=0; i<15; i++)
		{
			sprintf(tek, "%s", keter[i+plus]);	
			teks_layar(200, 38 + (9 * i), tek);
		}		
	}
	
	teks_layar(68, 216, "Tekan OK & panah untuk scroll ke bawah");
}

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

static void menu_log(void)
{


}


