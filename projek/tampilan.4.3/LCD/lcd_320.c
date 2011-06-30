/*

	1 Des 09, furkan jadid
	operasi pada buffer di jaga oleh mutex
*/
#include "FreeRTOS.h"
#include "semphr.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "LCD/font.c"

#if (PAKAI_FONT_KOMIK == 1)
#include "LCD/komik.c"
#endif

#include "LCD/font16.h"

extern xSemaphoreHandle lcd_sem;

#define CSRW				0x46
#define AWAL_LAYER_SATU		1600
#define GRAFIK_LAYER_DUA	12000
#define GRAFIK_LAYER_TIGA	22000

#define BACKLIT		BIT(20)	// PF15, P1.20

#ifdef TAMPILAN_LPC
#define WR		BIT(23)	// P0
#define RD		BIT(22)	// P0
#define AMS0	BIT(11) // P2
#define A1		BIT(19)	// P0

#define DAT8	(BIT(24) | BIT(25) | BIT(26) | BIT(27) | BIT(28) | BIT(29) | BIT(30) | BIT(31))	
#endif

/* Board Tampilan_lipi versi 4.3 Mar 2010 */
#ifdef TAMPILAN_LPC_43
#define TAMPILAN_LPC_4
#endif

/* Board Tampilan LPC 20 Jan 09 */
#ifdef TAMPILAN_LPC_4
#define WR		BIT(4)	// P1
#define RD		BIT(1)	// P1
#define AMS0	BIT(8)  // P1
#define A1		BIT(0)	// P1

#define DAT8	(BIT(24) | BIT(25) | BIT(26) | BIT(27) | BIT(28) | BIT(29) | BIT(30) | BIT(31))	
#endif

#define 	BESAR_LAYAR		9600
#define 	JML_Q_LCD		5

struct t_layar {
	unsigned char buf[BESAR_LAYAR];
	unsigned char flag;		
	unsigned char teks[256];
	unsigned short pos_x;
	unsigned short pos_y;
	
} layar __attribute__ ((section (".eth_test")));


static unsigned konvert_y[8] __attribute__ ((section (".lokasi_font"))) = {128, 64, 32, 16, 8, 4, 2, 1} ;
static int x_awal;
static int y_awal;

#if 0
void tes_high(void)
{
	FIO0SET |= WR;
	FIO2SET |= AMS0;
	FIO0SET |= A1;
	FIO0SET |= RD;
	FIO1PIN3 = 0xFF;	
}

void tes_low(void)
{
	FIO0CLR |= WR;
	FIO2CLR |= AMS0;
	FIO0CLR |= A1;
	FIO0CLR |= RD;
	FIO1PIN3 = 0x00;	
}
#endif

void init_port_lcd(void)
{
	//backlit
	FIO1DIR |= BACKLIT;
	FIO1CLR |= BACKLIT;
	
	#ifdef TAMPILAN_LPC
	// WR & RD
	FIO0DIR |= (WR | RD | A1);
	FIO0SET |= (WR | RD | A1);
	
	sambel
	// AMS0
	FIO2DIR = AMS0;
	FIO2SET = AMS0;
	#endif
	
	#ifdef TAMPILAN_LPC_4
	FIO1DIR  |=  (WR | RD | A1 | AMS0);
	FIO1MASK &= ~(WR | RD | A1 | AMS0);
	
	FIO1SET |= (WR | RD | A1 | AMS0);
	#endif
	
	// data 8 bit
	FIO1DIR |= DAT8;
	FIO1MASK &= ~DAT8;
	
}

#ifdef TAMPILAN_LPC
void tulis_param(unsigned char param)
{
	//AD
	FIO0CLR = A1;
	//CS
	FIO2CLR = AMS0;
	//WR
	FIO0CLR = WR;
	//data
	FIO1PIN3 = (unsigned char) param;
	// UN-WR & UN-CS
	FIO0SET = WR | AMS0;
}

void tulis_command(unsigned char param)
{
	//AD
	FIO0SET = A1;
	//CS
	FIO2CLR = AMS0;
	//WR
	FIO0CLR = WR;
	//data
	FIO1PIN3 = (unsigned char) param;
	// UN-WR & UN-CS
	FIO0SET = WR | AMS0;	
}
#endif

#ifdef TAMPILAN_LPC_4
void tulis_param(unsigned char param)
{
	//AD
	FIO1CLR = A1 | AMS0;
	//CS
	//FIO1CLR = AMS0;
	//WR
	FIO1CLR = WR;
	//data
	FIO1PIN3 = (unsigned char) param;
	// UN-WR & UN-CS
	FIO1SET = WR | AMS0;
}

void tulis_command(unsigned char param)
{
	//AD
	FIO1SET = A1;
	//CS
	FIO1CLR = AMS0;
	//WR
	FIO1CLR = WR;
	//data
	FIO1PIN3 = (unsigned char) param;
	// UN-WR & UN-CS
	FIO1SET = WR | AMS0;	
}
#endif


void init_lcd(void)
{	   
	int j;
	//init LCD dengan 8pixel per karakter
	tulis_command(0x40); //delay_ms(5);  //init command	
		tulis_param(0x30);// P1. HW setup
		
		/*
			internal CG ROM, single panel drive */
			
		 
		tulis_param(0x87);// P2. Char width dalam pixel <== DIBUAT 8 pixel per karakter supaya mudah grafiknya
								// kalau sudah jadi baru dikonversi ke 6 pixel per karakter lagi
		
		tulis_param(0x07);// P3. char height+1 in pixel
		//tulis_param(16);
		tulis_param(40);// P4. 	--> 320 /8	CR
		tulis_param(80);// P5. TC/R
		tulis_param(240);  //P6. L/F   baris per frame 
		//tulis_param(80);
		tulis_param(40);//P7. APL
		tulis_param(00);//P8. APH 
	
	dele(100);	
	//vTaskDelay(5);
	
	tulis_command(0x44); // SCROLL
		tulis_param((char) (AWAL_LAYER_SATU & 0xFF));		//	--> awal grafik 0x640 (1600)		4
		tulis_param((char) (AWAL_LAYER_SATU >> 8));	//	
		tulis_param(240); 								
		
		tulis_param((char) 	(GRAFIK_LAYER_DUA & 0xFF));//	7
		tulis_param((char)	(GRAFIK_LAYER_DUA >> 8));//	8		
		tulis_param(240); // 									6
		
		tulis_param((char) (GRAFIK_LAYER_TIGA & 0xFF));//	7
		tulis_param((char) (GRAFIK_LAYER_TIGA >> 8));//	8
		//tulis_param((char) grafik_kedua);//	7
		//tulis_param((char)(grafik_kedua >> 8));//	8
		tulis_param(0x00);//	9
		tulis_param(0x00);//	10
	
	dele(100);
	//vTaskDelay(5);
	
	
	//HDOT SCR			
	tulis_command(0x5A);//delay_ms(5);
		tulis_param(0x00);//delay_ms(100);
	

	//OVERLAY	
	tulis_command(0x5B);//delay_ms(5);
		//tulis_param(12);
		tulis_param(0x1F);		// simple animation / grey / depth
		
	//DISP ON OFF
	tulis_command(0x58); //off		
		/* layer 1 on, layer 2 on, layer 3 off */
		tulis_param(0x10 + 0x04);
		/* layer 1 on, layer 2 on, layer 3 flash 16 Hz */
		//tulis_param(0x04 + 0x10 + 0xC0);
		/* layer 1 on, layer 2 flash 2 detik, layer 3 flash 16 Hz */
		//tulis_param(0x20 + 0x04 + 0xC0);
			
	tulis_command(0x59);	//display on
		tulis_param(0x10 + 0x04);
		 
   //csr form		--> set cursor type
   tulis_command(0x5D);
   		tulis_param(0x04);	//blink 2 HZ
    	tulis_param(0x86);
		
		
	tulis_command(0x4c);	//direction of cursor movement --> right

	// bersihkan layer 2
	tulis_command(CSRW);		//posisi tulisan coba di SAD1
   		tulis_param((char) (GRAFIK_LAYER_DUA & 0xFF));
    	tulis_param((char) (GRAFIK_LAYER_DUA >>8));
   
   	tulis_command(0x42); 
   		for (j=0; j<9600; j++)
   	  		tulis_param(0x00);
   	
   	// bersihkan layer 3
	tulis_command(CSRW);		//posisi tulisan coba di SAD1
   		tulis_param((char) (GRAFIK_LAYER_TIGA & 0xFF));
    	tulis_param((char) (GRAFIK_LAYER_TIGA >>8));
   
   	tulis_command(0x42); 
   		for (j=0; j<9600; j++)
   	  		tulis_param(0x00);
   	

	tulis_command(CSRW);		//posisi tulisan coba di SAD1
   		tulis_param((char) AWAL_LAYER_SATU);
    	tulis_param((char)(AWAL_LAYER_SATU >>8));
   
   	tulis_command(0x42); 
   	for (j=0; j<9600; j++)
   	  	tulis_param(0x11);
   	//tulis_param('A');
   /*		
   tulis_command(0x42);
   for(j=0;j<1200;j++)
   {
   		tulis_param(0x20);
		//tulis_param('A');
   }*/
   
   
	tulis_command( CSRW );
   		tulis_param(0x40);
    	tulis_param(0x06);
   
   	tulis_command(0x42); 
   	for(j=0;j<9600;j++)
   	{
   		tulis_param(0x01);
	}
	
	x_awal = 0;
	y_awal = 0;
	
	memset(layar.buf, 0, BESAR_LAYAR);
	memset(layar.teks, 0, 256);
	layar.flag = 0;
	
	return;
}

inline void pixel(int xt,int yt, int pilih)
{
   /* x adalah pojok kiri atas mulai dari nol - 320 (alamat memory 0 --> 40)
   	y adalah juga pojok kiri atas mulai dari nol - 240 (alamat memory 0 --> 240)*/

	div_t hasil;
	unsigned short offsetku;

	hasil = div (xt,8);
			
	offsetku = hasil.quot + (yt*40);
	if (pilih==1)
		layar.buf[offsetku] = layar.buf[offsetku] ^ konvert_y[hasil.rem];
	else
		layar.buf[offsetku] = layar.buf[offsetku] | konvert_y[hasil.rem];
	
}


void move_ke(short x1,short y1)
{
   x_awal=x1;
   y_awal=y1;
}


void line_ke(short x,short y, short pilihnya)
{

	int dy;  
   int dx;
   int m;
   
   dy = (y - y_awal);
   dx = (x - x_awal);

   if (dx == 0)
   {
   	if (dy>0)
      {//garis lurus kebawah
      	for (m = 0; m<=dy ; m++)
      	{
         	pixel(x_awal, m+y_awal, pilihnya);
      	}
      }
      else  //garis lurus ke atas
      {
      	for (m = 0; m >=dy ; m--)
      	{
         	pixel(x_awal, m+y_awal, pilihnya);
      	}
      }
   }
   else if ( abs(dy/dx) < 1)      //kemiringan biasa
   {

   	if (dx>0)	//dari kiri ke kanan
   	{		
		for (m = 0; m <= dx; m++)
   		{
				pixel(m + x_awal, y_awal + ((int)((dy*m)/dx)), pilihnya);
   		}
   	}
   	else if (dx<0)    // arah dari kanan ke kiri
   	{
   		for (m = 0; m >= dx; m--)
   		{
      		pixel(m + x_awal, y_awal + ((int)((dy*m)/dx)), pilihnya);
   		}
   	}
   }
   else       //kemiringan tajam
   {
   	if (dy>0)
   	{
   		for (m = 0; m <= dy; m++)
   		{
            pixel(x_awal + ((int)((dx*m)/dy)), m+y_awal, pilihnya);
   		}
   	}
   	else if (dy<0)    // arah dari bawah ke atas kanan tajam
   	{
   		for (m = 0; m >= dy; m--)
   		{
      		pixel(x_awal + ((int)((dx*m)/dy)), m+y_awal, pilihnya);
   		}
   	}
   }
   move_ke(x,y);
} 


/* fungsi kotak */
void kotak(int x1, int y1, int x2, int y2)
{
	move_ke(x1,y1);
   line_ke(x1,y2,0);
   line_ke(x2,y2,0);
   line_ke(x2,y1,0);
   line_ke(x1,y1,0);
}

void kotak3d(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2)
{
	move_ke(x1,y1);
   line_ke(x1,y2, 0);
   line_ke(x2,y2, 0);
   line_ke(x2,y1, 0);
   line_ke(x1,y1, 0);
   move_ke(x1+1,y2+1);
   line_ke(x2+1,y2+1, 0);
   line_ke(x2+1,y1+1, 0);
}

void update_hard_lcd(void)
{
	int t;
	
	if (layar.flag == 0)
	{
   		portENTER_CRITICAL();
		tulis_command( CSRW );
   		tulis_param((char) (AWAL_LAYER_SATU & 0xFF));
		tulis_param((char) (AWAL_LAYER_SATU >> 8));
   		
  		tulis_command(0x42);
  		portEXIT_CRITICAL();
  		
		for (t=0; t<9600; t++)
  		{
  			tulis_param(layar.buf[t]);  		
  		}
	}
	else if (layar.flag == 1)
	{
		teks_h_hard(layar.pos_x, layar.pos_y, layar.teks);
		layar.flag = 0;
	}
	else if (layar.flag == 23)
	{
		// bersihkan layer 2
		portENTER_CRITICAL();
		tulis_command(CSRW);		//posisi tulisan coba di SAD1
   			tulis_param((char) (GRAFIK_LAYER_DUA & 0xFF));
    		tulis_param((char) (GRAFIK_LAYER_DUA >>8));
   
   		tulis_command(0x42); 
   		portEXIT_CRITICAL();
   		
   		for (t=0; t<9600; t++)
  		{
  			tulis_param(layar.buf[t]);  		
  		}
	}
	else if (layar.flag == 24)
	{
		// update layer 3 dari buffer layar 
		portENTER_CRITICAL();
		tulis_command(CSRW);		//posisi tulisan coba di SAD1
   			tulis_param((char) (GRAFIK_LAYER_TIGA & 0xFF));
    		tulis_param((char) (GRAFIK_LAYER_TIGA >>8));
   
   		tulis_command(0x42); 
   		portEXIT_CRITICAL();
   		
   		for (t=0; t<9600; t++)
  		{
  			tulis_param(layar.buf[t]);  		
  		}
	}
}

void update_lcd(void)
{
	layar.flag = 0;
	xSemaphoreGive( lcd_sem );	
}

void update_lcd_layer2(void)
{
	layar.flag = 23;
	xSemaphoreGive( lcd_sem );		// supaya segera dijadwalkan untuk update_hard_lcd
	vTaskDelay(10);	
}

void update_lcd_layer3(void)
{
	layar.flag = 24;
	xSemaphoreGive( lcd_sem );		// supaya segera dijadwalkan untuk update_hard_lcd
	vTaskDelay(10);	
}

void teks_layar(unsigned short x, unsigned short y, char *pc)
{
   int offsetku;
   unsigned int font_loc;
   unsigned int z, i;
   unsigned char font[9];
   unsigned char font_h[9];
 
   div_t hasil;
	/* x adalah jarak dari kiri 0 - 320 pixel
   		y adalah jarak dari atas 0 - 240 pixel juga
      jadi data harus digeser sejauh y kebawah kemudian di or kan dengan
      data yang sudah ada */
   
   for (z = 0 ; z < 50 ; z++)
   {
   	hasil = div(x,8);	
		offsetku = hasil.quot + (y*40);
   	
		if (pc[z]==NULL) break;  	//untuk mengecek panjangnya string pc
   		font_loc = (unsigned int) (pc[z] - 32) * 5;
	
		//ambil dulu satu set fontnya
		for (i=0; i<5; i++)				//5 karena ukuran fontnya adalah 5 x7 
		{
			font[i] = font57[font_loc+i];
		}
		//putar kekanan	
		font_h[0] = ((font[4] & 1) << 3) + ((font[3] & 1) << 4) + ((font[2] & 1) << 5) + ((font[1] & 1) << 6) + ((font[0] & 1) << 7);
		font_h[1] = ((font[4] & 2) << 2) + ((font[3] & 2) << 3) + ((font[2] & 2) << 4) + ((font[1] & 2) << 5) + ((font[0] & 2) << 6);
		font_h[2] = ((font[4] & 4) << 1) + ((font[3] & 4) << 2) + ((font[2] & 4) << 3) + ((font[1] & 4) << 4) + ((font[0] & 4) << 5);
		font_h[3] = ((font[4] & 8) )     + ((font[3] & 8) << 1) + ((font[2] & 8) << 2) + ((font[1] & 8) << 3) + ((font[0] & 8) << 4);
		font_h[4] = ((font[4] & 16) >> 1)+ ((font[3] & 16) ) + ((font[2] & 16) << 1) + ((font[1] & 16) << 2) + ((font[0] & 16) << 3);
		font_h[5] = ((font[4] & 32) >> 2)+ ((font[3] & 32) >> 1 ) + ((font[2] & 32)) + ((font[1] & 32) << 1) + ((font[0] & 32) << 2);
		font_h[6] = ((font[4] & 64) >> 3)+ ((font[3] & 64) >> 2 ) + ((font[2] & 64 ) >> 1) + ((font[1] & 64)) + ((font[0] & 64) << 1);
		
		/*
   	//teks selalu hitam
   	for (i=0; i<7; i++)		// 7 kebawah
   	{
   		layar[offsetku]= layar[offsetku] | (font_h[i] >> hasil.rem);
			if (hasil.rem != 0)
			layar[offsetku+1]= layar[offsetku+1] | (font_h[i] << (8-hasil.rem));
			offsetku = offsetku+40;
   	} */
   	
   	for (i=0; i<7; i++)		// 7 kebawah
   	{
   		layar.buf[offsetku]= layar.buf[offsetku] ^ (font_h[i] >> hasil.rem);
			if (hasil.rem != 0)
			layar.buf[offsetku+1]= layar.buf[offsetku+1] ^ (font_h[i] << (8-hasil.rem));
			offsetku = offsetku+40;
   	}
	  x = x+6;			//lebar font yang dipakai --> tinggal ganti ini jika mau lebih lebar jaraknya
   }
}

#if (PAKAI_FONT_KOMIK == 1)
//	ditambahkan untuk mendapat ukuran teks yang lebih besar 
//	font didapat dari external display lib font
// jam 2.29, 26 februari 2006
void teks_komik(unsigned short x, unsigned short y, char *pc)
{
	int offsetku;
   unsigned int font_loc;
   unsigned int z, i;
   
 
   div_t hasil;
   
   hasil = div(x,8);	
	offsetku = hasil.quot + (y*40);
   
   for (z = 0 ; z < 50 ; z++)
   {
   	
		//offset2 = offsetku;		
		if (pc[z]==NULL) break;  	//untuk mengecek panjangnya string pc

   	font_loc = (unsigned int) (pc[z] - 32);
   	
   	for (i=0; i<16; i++)
   	{
   		layar.buf[offsetku+(i*40)]= layar.buf[offsetku+(i*40)] | (big[font_loc][i] >> hasil.rem );
   		layar.buf[offsetku+(i*40)+1]= layar.buf[offsetku+(i*40)+1] | (big[font_loc][i] << (8 - hasil.rem) );
   		
   		layar.buf[offsetku+(i*40)-1]= layar.buf[offsetku+(i*40)-1] | ((big[font_loc][i] >> 8) >> hasil.rem);
         layar.buf[offsetku+(i*40)]= layar.buf[offsetku+(i*40)] | ((big[font_loc][i] >> 8) << (8 -hasil.rem)); 		
   	}
      offsetku++;
   }
}
#endif

// menulis langsung directly ke layar
void teks_h(unsigned short x, unsigned short y, char *pc)
{
	layar.pos_x = x;
	layar.pos_y = y;

	sprintf(layar.teks,"%s",pc);
	
	layar.flag = 1;
	xSemaphoreGive( lcd_sem );		// supaya segera dijadwalkan untuk update_hard_lcd
	vTaskDelay(50);		// 50
}


// jangan panggil langsung fungsi ini
// harus dipanggil dari skedule LCD
void teks_h_hard(unsigned short x, unsigned short y, char *pc)
{
	int offsetku;
    int font_loc;
    int z, i;
    unsigned char font[9];
    unsigned char font_h[9];
 
    div_t hasil;
   
   
	/* x adalah jarak dari kiri 0 - 320 pixel
   		y adalah jarak dari atas 0 - 240 pixel juga
      jadi data harus digeser sejauh y kebawah kemudian di or kan dengan
      data yang sudah ada */
   

   
   /*
   tulis_command(0x46);
   offsetku = awal_grafik + offsetku;
  		tulis_param((char) offsetku);
  		tulis_param((char) (offsetku>>8));
  	
  	tulis_command(0x42);
   */
   
   for (z = 0 ; z < 50 ; z++)
   {
   		hasil = div(x,8);	
		offsetku = hasil.quot + (y*40);
   	
		if (pc[z]==NULL) break;  	//untuk mengecek panjangnya string pc
   		font_loc = (unsigned int) (pc[z] - 32) * 5;
	
		//ambil dulu satu set fontnya
		for (i=0; i<5; i++)				//5 karena ukuran fontnya adalah 5 x7 
		{
			font[i] = font57[font_loc+i];
		}
		//putar kekanan	
		font_h[0] = ((font[4] & 1) << 3) + ((font[3] & 1) << 4) + ((font[2] & 1) << 5) + ((font[1] & 1) << 6) + ((font[0] & 1) << 7);
		font_h[1] = ((font[4] & 2) << 2) + ((font[3] & 2) << 3) + ((font[2] & 2) << 4) + ((font[1] & 2) << 5) + ((font[0] & 2) << 6);
		font_h[2] = ((font[4] & 4) << 1) + ((font[3] & 4) << 2) + ((font[2] & 4) << 3) + ((font[1] & 4) << 4) + ((font[0] & 4) << 5);
		font_h[3] = ((font[4] & 8) )     + ((font[3] & 8) << 1) + ((font[2] & 8) << 2) + ((font[1] & 8) << 3) + ((font[0] & 8) << 4);
		font_h[4] = ((font[4] & 16) >> 1)+ ((font[3] & 16) ) + ((font[2] & 16) << 1) + ((font[1] & 16) << 2) + ((font[0] & 16) << 3);
		font_h[5] = ((font[4] & 32) >> 2)+ ((font[3] & 32) >> 1 ) + ((font[2] & 32)) + ((font[1] & 32) << 1) + ((font[0] & 32) << 2);
		font_h[6] = ((font[4] & 64) >> 3)+ ((font[3] & 64) >> 2 ) + ((font[2] & 64 ) >> 1) + ((font[1] & 64)) + ((font[0] & 64) << 1);		
   	
   	for (i=0; i<7; i++)		// 7 kebawah
   	{
   		//portENTER_CRITICAL();
		
		tulis_command( CSRW );
  			//tulis_param((char) (AWAL_LAYER_SATU + offsetku));	
  			//tulis_param((char) ((AWAL_LAYER_SATU + offsetku)>>8));
  			tulis_param((char) (GRAFIK_LAYER_DUA + offsetku));	
  			tulis_param((char) ((GRAFIK_LAYER_DUA + offsetku)>>8));
  			
  			tulis_command(0x42);
   		layar.buf[offsetku]= layar.buf[offsetku] ^ (font_h[i] >> hasil.rem);
   		tulis_param(layar.buf[offsetku]);
			if (hasil.rem != 0)
			{
				layar.buf[offsetku+1]= layar.buf[offsetku+1] ^ (font_h[i] << (8-hasil.rem));
				tulis_param(layar.buf[offsetku+1]);	
			}
			offsetku = offsetku+40;
   		
		//portEXIT_CRITICAL();
	}
	  x = x+6;			//lebar font yang dipakai --> tinggal ganti ini jika mau lebih lebar jaraknya
   }
}

unsigned short y_p=0;

void printf_lcd(const char *format, ...)
{
	va_list args;
	char printbuffer[128];

	va_start (args, format);

	/* For this to work, printbuffer must be larger than
	 * anything we ever want to print.
	 */
	vsprintf (printbuffer, format, args);
	va_end (args);

	/* Print the string */
	teks_h(5, y_p, printbuffer);
	y_p = y_p + 10;
}

// menggunakan ukuran font 16x16
// 4 januari 2006, font aslinya terbalik
void teks_arial(unsigned short x, unsigned short y, char *pc)
{
	int offsetku;
   unsigned int font_loc;
   unsigned int z, i;
 
   div_t hasil;
   
   hasil = div(x,8);	
	offsetku = hasil.quot + (y*40);
   
   for (z = 0 ; z < 50 ; z++)
   {		
		if (pc[z]==NULL) break;  	//untuk mengecek panjangnya string pc

		hasil = div(x,8);	
		offsetku = hasil.quot + (y*40);
	
   	font_loc = (unsigned int) (pc[z] - 32);
   	
   	for (i=0; i<16; i++)
   	{
   		layar.buf[offsetku+(i*40)]= layar.buf[offsetku+(i*40)] | (arial[font_loc][16-i] >> hasil.rem );
   		layar.buf[offsetku+(i*40)+1]= layar.buf[offsetku+(i*40)+1] | (arial[font_loc][16-i] << (8 - hasil.rem) );
   		
   		layar.buf[offsetku+(i*40)-1]= layar.buf[offsetku+(i*40)-1] | ((arial[font_loc][16-i] >> 8) >> hasil.rem);
         layar.buf[offsetku+(i*40)]= layar.buf[offsetku+(i*40)] | ((arial[font_loc][16-i] >> 8) << (8 -hasil.rem)); 		
   	}
      //x = x+12;
       x = x+14;
   }
}

void cls_layar(void)
{
	int i;
	
	for (i=0; i<BESAR_LAYAR; i++)
		layar.buf[i] = 0;	
}


// membersihkan bagian tertentu dari ram, supaya ditulis bersih
void teks_clear(unsigned short x, unsigned short y, unsigned short pjg)
{
   	int offsetku;
   	unsigned int z, i;
	int f;
 	
   	div_t hasil;
   
	/* x adalah jarak dari kiri 0 - 320 pixel
   		y adalah jarak dari atas 0 - 240 pixel juga
      jadi data harus digeser sejauh y kebawah kemudian di or kan dengan
      data yang sudah ada */
   f = x;
   for (z = 0 ; z < pjg ; z++)
   {
   		hasil = div(f,8);	
		offsetku = hasil.quot + (y*40);
		
		for (i=0; i<7; i++)		// 7 kebawah
   		{   		
   			layar.buf[offsetku]= 0x00;
			
			if (hasil.rem != 0)
			{
			   layar.buf[offsetku+1]= 0x00;		
			}
			offsetku = offsetku+40;
   		}
	   	f = f+6;
   }
}

void hapus(unsigned short x, unsigned short y, unsigned short x2, unsigned short y2)
{
	//15 des 06, menghapus layar sesuai dengan koordinat
	int i;
	int j;
	div_t hasil;
	int offsetku;
   
   for (i=y; i<y2; i++)
   {
   	hasil = div(x,8);
   	offsetku = hasil.quot + (i*40);
   	for (j=x; j<x2;)
   	{
   		layar.buf[offsetku] = 0x00;
   	   //layar[offsetku] = layar[offsetku] | (~(0xFF >> hasil.rem));
   		//if (hasil.rem != 0)
			//layar[offsetku+1]= layar[offsetku+1] ^ (font_h[i] << (8-hasil.rem));
			
   		offsetku++;
   		j = j+8;
   	}	
   }	
}

void msg_box(char *msg)
{
   unsigned short cl,kiri;
   cl = strlen(msg);
   kiri = (unsigned short)(320 - (cl*6))/2;
   //bikin kotak ditengah
   hapus(kiri-20+1, 79, kiri+20+(cl*6) -1, 111);
   kotak3d(kiri-20, 81, kiri+20+(cl*6), 109);
   teks_layar(kiri-4+4, 90, msg);
}

void fill_layar(unsigned char fil)
{
	int i;
	
	for (i=0; i<BESAR_LAYAR; i++)
		layar.buf[i] = (unsigned char) fil;	
}
