/*
	idle hook
	barangkali perlu
	
	15 april 2009
	furkan jadid, daun biru engineering
	
	*/
#include <stdarg.h>

#ifdef LOGGING
static char buffer_log[1024];
static char buf_temp[64];
unsigned int pos_log = 0;
#endif

extern unsigned int loop_idle;
	
void vApplicationIdleHook( void )
{
	loop_idle++;	
}


#ifdef LOGGING
   
int error_log (const char *fmt, ...)
{
   va_list args;
   unsigned int i;
   unsigned int len;
   
   va_start (args, fmt);

   /* For this to work, printbuffer must be larger than
    * anything we ever want to print.
    */
   i = vsprintf (buf_temp, fmt, args);
   va_end (args);

	len = strlen(buf_temp);
	if (pos_log + len > 1024)		// akan full 
	{
		pos_log = 0;
		
	}
   /* Print the string */
   vSerialPutString(1, printbuffer);
   return 0;
} 
#endif
