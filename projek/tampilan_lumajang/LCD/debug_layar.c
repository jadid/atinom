/* 
	debug lewat layar
	4 Mei 2009
	
	furkan jadid
	daun biru engineering
	
	*/
/* fungsi untuk cetak debug ke layar */
#include <stdarg.h>
#include <stdio.h>

#define DEBUG_PLOT_KIRI			240
#define DEBUG_PLOT_KIRI_H		40

static unsigned int debug_line = 0;
static unsigned int pinggir = 0;

/* inisiasi line debug, dari atas & pinggir */
void init_debug_layar(unsigned int pgr)
{
	debug_line = 0;
	pinggir = pgr;
}

/* 	
	fungsi ini menampilkan teks di layar, turun 1 baris setiap dipanggil
	tetapi tetap mengandalkan update_lcd() untuk menampilkannnya
	di layar 
	
	*/
extern unsigned char tek[];	

void debug_out(const char *fmt, ...)
{
	va_list args;
   	uint i;

	if (enable_log == 1)
	{
   		va_start (args, fmt);

	   	/* For this to work, printbuffer must be larger than
		* anything we ever want to print.
		*/
	   	i = vsprintf ( tek , fmt, args);
	   	va_end (args);

	   	/* Print the string */
	   	debug_line++;
	   	
	   	if (debug_line > 25)
	   	{
	   		cls_layar();
	   		debug_line = 0;
	   		update_lcd();
	   	}
	   	
	   	teks_layar( pinggir , (debug_line*8) + 9, tek);
   	}
   	return;
}

#if 1
/* 	
	fungsi berikut langsung menuliskan ke layar, tanpa harus memanggil 
	update_lcd() 
	
	*/
void debug_out_h(const char *fmt, ...)
{
	va_list args;
   	uint i;

	if (enable_log == 1)
	{
	   	va_start (args, fmt);

	   	/* For this to work, printbuffer must be larger than
		* anything we ever want to print.
		*/
	   	i = vsprintf ( tek , fmt, args);
	   	va_end (args);

		teks_h( pinggir , (debug_line*8) + 25, tek );
	   	/* Print the string */
	   	debug_line++;
	   	if (debug_line > 24)
	   	{
	   		cls_layar();
	   		vTaskDelay(10);
	   		update_lcd_layer2();	   		
	   		debug_line = 0;
	   		//update_lcd();
	   		//vTaskDelay(20);
	   	}	   	
	   	//teks_h( pinggir , (debug_line*8) + 9, tek );
   	}
   	return;	
}
#endif
