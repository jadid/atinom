/*
	1 des 09
	furkan jadid
	daun biru engineering
	
	semacam ncurse ?
	*/

#include <stdarg.h>
#include <stdio.h>

static int x1;
static int y1;
extern unsigned char tek[];	

void goto_xy(int x, int y)
{
	x1 = x*8;
	y1 = y*8;	
}

void printw(const char *fmt, ...)
{
	va_list args;
   	uint i;

	{
   		va_start (args, fmt);

	   	/* For this to work, printbuffer must be larger than
		* anything we ever want to print.
		*/
	   	i = vsprintf ( tek , fmt, args);
	   	va_end (args);
	   	
		/*
	   	if (debug_line > 25)
	   	{
	   		cls_layar();
	   		debug_line = 0;
	   		update_lcd();
	   	}*/
	   	
	   	//teks_layar( pinggir , (debug_line*8) + 9, tek);
		
		if ( x1 < 310)
			teks_layar( x1, y1, tek);
		
		i = strlen(tek);
		x1 += (i * 6);
   	}
   	return;
}
