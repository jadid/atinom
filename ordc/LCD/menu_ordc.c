/*
	menu_ordc.c
	
	12 Juli 2009
	furkan jadid
	
	versi awal
	
	
*/
//MENU
#define t_menu		24			//tinggi menu
#define j_atas		15			//jarak menu dari atas LCD

#define t_menu_utama	30

void menu_ordc(unsigned int key)
{
	teks_arial(50, 50, "Backlit On/Off");
	//teks_arial(50, 80, "Recip Analyzer");
	teks_arial(50, 80, "Pilih Rute");
	//teks_arial(50, 110, "Rotating Analyzer");
	teks_arial(50, 110, "Lihat Data");	
	//teks_arial(50, 140, "Balancing");
	teks_arial(50, 140, "Komunikasi");
	teks_arial(50, 170, "Hal Lain");	


}

void menu_pilih_utama(unsigned char p, unsigned char mesin)
{
	kotak3d(30, (p*t_menu_utama) + 42, 290, ((p+1) * t_menu_utama) + 42); 	
}

extern float volt_bat;

void show_battery(void)
{
	unsigned char tek[64];
		
	hapus_layar(1, 230, 80, 239);
	sprintf(tek, "Bat %2.2f", (float) volt_bat);
	teks_h(1, 231, tek);
	 	
}
