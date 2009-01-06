/*

	menampilkan info2 scrolling di LCD
	
	14 Nov 2007
	
	furkan jadid, daun biru enginering
	
	*/

#include <stdio.h>
#include "../monita/monita_uip.h"

extern struct t_sumber sumber[];
extern struct t_mesin	mesin[];
extern struct t_titik	titik[];

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

void menu_monita(void);
void menu_pilih(unsigned char p, unsigned char mesin);
void kotak_bolong(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2);
void menu_charge(unsigned char mes);
void menu_pelumas(unsigned char mes);
void menu_jacket(unsigned char mes);
void menu_exhaust(unsigned char mes);
void menu_generator(unsigned char mes);


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
	//move_ke(0, 25); line_ke(319, 25);
	//line_ke(319, 196); line_ke(0, 196);
}

void menu_monita(void)
{
	char s[32];
	//sprintf(s,"%s", loc_data.nama);
	
   //teks_arial(230, 2, "SWD #1");
   //teks_arial(230, 2, s);
   
   //waktu
	//sprintf(s, "%s", temp_net.waktu);
	//teks_layar(menu_kanan+menu_kiri-4, 11, s);
   //teks_layar(180, 232, s);
   teks_layar(180, 232, "harusnya waktu");
   
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
	
	// setting
	teks_layar(menu_kiri, menu_top+6*(menu_tinggi+menu_antara)+3, "Setting");
	//teks_layar(menu_kiri, menu_top+7*(menu_tinggi+menu_antara)+8, "---");
	
	/*
	teks_layar(mesin_kiri, mesin_top, "Unit #1");	
	teks_layar(mesin_kiri+(mesin_width), mesin_top, "Unit #2");
	teks_layar(mesin_kiri+(mesin_width*2), mesin_top, "Unit #3");	
	teks_layar(mesin_kiri+(mesin_width*3), mesin_top, "Unit #4");
	teks_layar(mesin_kiri+(mesin_width*4), mesin_top, "Unit #5");	
	*/
	
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
	
	
	// testing
	//teks_arial(20, 220, "Sedang di test .. !");
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

//void menu_pilih(unsigned char p)
/* p adalah group, misalnya charge air dll, mesin adalah unitnya) */
void menu_pilih(unsigned char p, unsigned char mesin)
{
	kotak_bolong(2, 40-menu_jarak_atas+(p*(menu_tinggi+menu_antara)), menu_kanan, 40+menu_tinggi+(p*(menu_tinggi+menu_antara)));
	
	//kontak mesin
	move_ke(menu_kanan, menu_besar_top);
	line_ke(mesin_kiri-5 + (mesin_width*mesin), menu_besar_top); line_ke(mesin_kiri-5+(mesin_width*mesin), 2);
	line_ke(mesin_kiri-5 + (mesin_width*(mesin+1)), 2); 
	line_ke(mesin_kiri-5 + (mesin_width*(mesin+1)), menu_besar_top);
	line_ke(menu_besar_kanan, menu_besar_top); 
	
	if (mesin == 0)
		teks_arial(menu_besar_kanan-22, menu_besar_tinggi-18, "#1");
	if (mesin == 1)
		teks_arial(menu_besar_kanan-22, menu_besar_tinggi-18, "#2");
	if (mesin == 2)
		teks_arial(menu_besar_kanan-22, menu_besar_tinggi-18, "#3");
	if (mesin == 3)
		teks_arial(menu_besar_kanan-22, menu_besar_tinggi-18, "#4");
	if (mesin == 4)
		teks_arial(menu_besar_kanan-22, menu_besar_tinggi-18, "#5");
	
	mesin = mesin-1;
	if (p == 0) menu_charge(mesin);	
	else if (p == 1) menu_jacket(mesin);
	else if (p == 2) menu_pelumas(mesin);
	else if (p == 3) menu_exhaust(mesin);
	else if (p == 4) menu_generator(mesin);
	
	else if (p == 6) menu_setting();
}

void menu_charge(unsigned char mes)
{
	char s[128];
	float f1;
	float f2;
	
	f1 = 0.21;
	f2 = 43.19;
	
	//teks_arial(180, 22, "Charge Air"); /* posisi kanan atas */
	teks_arial(menu_kanan+menu_kiri+4, menu_besar_tinggi-18, "Charge Air");
	/*
	sprintf(s, "Pl = %3.2f bar", masing[mes].CA_P_L);
	teks_arial(100, 50, s); 
	
	sprintf(s, "Tl = %3.2f C", masing[mes].CA_T_L);
	teks_arial(100, 70, s); 

	sprintf(s, "Pr = %3.2f bar", masing[mes].CA_P_R);
	teks_arial(100, 100, s); 
	
	sprintf(s, "Tr = %3.2f C", masing[mes].CA_T_R);
	teks_arial(100, 120, s); 
	*/
}

void menu_jacket(unsigned char mes)
{
	char s[128];
	float f1;
	float f2;
	
	f1 = 2.35;
	f2 = 93.19;
	
	//teks_arial(180, 22, "Charge Air"); /* posisi kanan atas */
	teks_arial(menu_kanan+menu_kiri+4, menu_besar_tinggi-18, "Jacket Water");
	
	/*
	teks_layar(92, 41, "Inlet");
	sprintf(s, "P = %3.2f bar", masing[mes].JW_P_in);
	teks_arial(100, 50, s); 
	
	sprintf(s, "T = %3.2f C", masing[mes].JW_T_in);
	teks_arial(100, 70, s); 
	
	teks_layar(92, 91, "Outlet");
	sprintf(s, "P = %3.2f bar", masing[mes].JW_P_out);
	teks_arial(100, 100, s); 
	
	sprintf(s, "T = %3.2f C", masing[mes].JW_T_out);
	teks_arial(100, 120, s); 
	*/
}

void menu_pelumas(unsigned char mes)
{
	char s[128];
	float f1;
	float f2;
	
	f1 = 4.56;
	f2 = 93.19;
	
	//teks_arial(180, 22, "Charge Air"); /* posisi kanan atas */
	teks_arial(menu_kanan+menu_kiri+4, menu_besar_tinggi-18, "Pelumas");
	
	teks_layar(92, 41, "Inlet");
	/*
	sprintf(s, "P = %3.2f bar", masing[mes].LO_P_in);
	teks_arial(100, 50, s); 
	
	sprintf(s, "T = %3.2f C", masing[mes].LO_T_in);
	teks_arial(100, 70, s); 

	teks_layar(92, 91, "Outlet");
	sprintf(s, "P = %3.2f bar", masing[mes].LO_P_out);
	teks_arial(100, 100, s); 
	
	sprintf(s, "T = %3.2f C", masing[mes].LO_T_out);
	teks_arial(100, 120, s);
	*/ 
}

void menu_exhaust(unsigned char mes)
{
	char s[128];
	char i;
	float f1;
	float f2;
	
	f1 = 4.56;
	f2 = 93.19;
	
	//teks_arial(180, 22, "Charge Air"); /* posisi kanan atas */
	teks_arial(menu_kanan+menu_kiri+4, menu_besar_tinggi-18, "Exhaust");
	
	/*
	for (i=1; i<9; i++)
	{
		sprintf(s, "%d = %3.0f", i, masing[mes].exhaust[i]);
		teks_arial(85, 4+(i*19), s); 
	}
	
	for (i=9; i<17; i++)
	{
		if (i==9)
	   sprintf(s, "%d  = %3.0f", i, masing[mes].exhaust[i]);
		else
		sprintf(s, "%d = %3.0f", i, masing[mes].exhaust[i]);
		
	   teks_arial(207, 4+((i-8)*19), s); 
	}
	*/
	//sprintf(s, "T = %3.2f C", f2);
	//teks_arial(100, 70, s); 
}

void menu_generator(unsigned char mes)
{
	char s[128];
	float f1;
	float f2;
	
	f1 = 4.56;
	f2 = 93.19;
	
	//teks_arial(180, 22, "Charge Air"); /* posisi kanan atas */
	teks_arial(menu_kanan+menu_kiri+4, menu_besar_tinggi-18, "Generator");
	/*
	sprintf(s, "kW  = %3.2f", masing[mes].kw);
	teks_arial(85, 50, s); 
	
	//sprintf(s, "n = %3.2f rpm", masing[mes].rpm);
	sprintf(s, "kWh = %8.0f ", masing[mes].kwh);
	teks_arial(85, 70, s); 
	
	sprintf(s, "f   = %3.2f Hz", masing[mes].frek);
	teks_arial(85, 90, s); 

	sprintf(s, "pf  = %3.2f ", masing[mes].pf);
	teks_arial(85, 110, s); 
	
	sprintf(s, "Amp = %3.2f ", masing[mes].amper);
	teks_arial(85, 130, s);
	
	sprintf(s, "Volt= %3.2f ", masing[mes].volt);
	teks_arial(85, 150, s);

	sprintf(s, "rpm = %3.2f ", masing[mes].rpm);
	teks_arial(85, 170, s);
	*/
}

void menu_setting(void)
{
	int i;
	char s[128];
	
	teks_layar(85, 40, "Sumber Data");
	for (i=0; i<10; i++)
	{
		teks_layar(85, 50 + (i*9), sumber[i].nama);
		
		// print out IP
		sprintf(s,"%d.%d.%d.%d", sumber[i].IP0, sumber[i].IP1, sumber[i].IP2, sumber[i].IP3);
		teks_layar(140, 50 + (i*9), s);
		
		// status
		if (sumber[i].status == 0)
			teks_layar(250, 50 + (i*9), "Null");
		if (sumber[i].status == 1)
			teks_layar(250, 50 + (i*9), "Normal");
		if (sumber[i].status == 2)
			teks_layar(250, 50 + (i*9), "TimeOut");	
	}	
	
}
