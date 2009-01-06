/*
 * baca tulis flash
 *
 */


#include "enviro.h"



unsigned int command[5]; // For Command Table
unsigned int result[2]; 	// For Result Table

IAP iap_entry;



char* ftostr(float value, int places );

int Write_to_Flash(unsigned char Start_Sector, unsigned End_Sector, int No_of_Bytes, unsigned long Mem_Addr);

#ifdef TES_BACA_TULIS
int tes_tulis_flash()
{
	unsigned long i,j;
	unsigned char tek[32];

	serial_puts("tes tulis flash ..\n");
	iap_entry = (IAP)IAP_LOCATION;// Set Function Pointer

	serial_puts("prepare ");
	command[0] = 50; 	// Prepare sector(s) for a Erase Operation
	command[1] = 24;	// start sector
	command[2] = 24;	// end sector

	taskENTER_CRITICAL();
	iap_entry(command,result);
	taskEXIT_CRITICAL();

	sprintf(tek, "%d %d\n", result[0], result[1]);
	serial_puts(tek);

	serial_puts("..");
	command[0] = 52; 	// Hapus dulu
	command[1] = 24;	// start sector
	command[2] = 24;	// end sector
	command[3] = 60000;	// PCLK

	taskENTER_CRITICAL();
	iap_entry(command,result);
	taskEXIT_CRITICAL();

			if (result[0]) {
				serial_puts("hapus error !\n");
				return;
			}
	serial_puts("prepare ");
	command[0] = 50; 	// Prepare sector(s) for a Erase Operation
	command[1] = 24;	// start sector
	command[2] = 24;	// end sector

	taskENTER_CRITICAL();
	iap_entry(command,result);
	taskEXIT_CRITICAL();

	sprintf(tek, "%d %d\n", result[0], result[1]);
	serial_puts(tek);

	for (i=0; i<32; i++)
		tek[i] = ('0'+i);

	serial_puts("ram to flash ");
	command[0] = 51;
	command[1] = 0x007A000;	// tujuan flash (seuai dengan sektor flash)
	command[2] = &tek[0];		// source ram
	command[3] = 256;
	command[4] = 60000;// PCLK = 60000 KHz

	taskENTER_CRITICAL();
	iap_entry(command,result);
	taskEXIT_CRITICAL();

	sprintf(tek, "%d %d\n", result[0], result[1]);
	serial_puts(tek);

}

int tes_baca_flash()
{
	unsigned long i,j;
	unsigned char tek[32];
	unsigned char *p;
	unsigned char c;

	p = (unsigned char *) 0x7A000;

	serial_puts("tes BACA flash ..\n");
	//iap_entry = (IAP)IAP_LOCATION;// Set Function Pointer

	for (i=0; i<32; i++)
	{
		c = *p++;

		sprintf(tek, " %c", c);
		serial_puts(tek);

	}
}
#endif




/*
char* ftostr(float value, int places )
{
    static buffer[32] ;
    int whole ;
    int fraction ;
    char sign[2] = "" ;

    if( value < 0 )
    {
        value = -value ;
        sign[0] = '-' ;
        sign[1] = '\0' ;
    }

    whole = (int)value ;
    fraction = (int)((value - whole) * pow(10.0f,places) + 0.5f) ;
    sprintf( buffer, "%s%d.%*.*d", sign, whole, places, places, fraction ) ;

    return buffer ;
}
*/

