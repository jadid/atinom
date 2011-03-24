/*
	selenoid driver
	
	11 Maret 2010
	furkan jadid, daun biru engineering
	
	Menggunakan ULN2803
	
	masih hanya available pada Tampilan versi 4.3
	
	*/
	
#include "FreeRTOS.h"
//#include "task.h"
#include "../monita/monita_uip.h"
	

	
#if (PAKAI_SELENOID == 1)



#define RLY_1	BIT(26)			/* P3 */
#define RLY_2	BIT(25)

#define RLY_3	BIT(23)			/* P1 */
#define RLY_4	BIT(21)	
#define RLY_5	BIT(15)

#define RLY_6	BIT(29)			/* P4 */
#define RLY_7	BIT(28)

#define RLY_8	BIT(7)			/* P2 */

void init_selenoid(void)	{
	FIO3DIR = FIO3DIR | RLY_1 | RLY_2;
	FIO1DIR = FIO1DIR | RLY_3 | RLY_4 | RLY_5;
	FIO4DIR = FIO4DIR | RLY_6 | RLY_7;
	FIO2DIR = FIO2DIR | RLY_8;	
}

void set_selenoid( unsigned int no )	{
	if (no>0 && no<=JML_RELAY) {		// 1-8 
		data_f[JML_SUMBER*PER_SUMBER+no-1] = 1;
	}
	
	if (no == 1)
		FIO3SET = RLY_1;
	else if (no == 2)
		FIO3SET = RLY_2;
	else if (no == 3)
		FIO1SET = RLY_3;
	else if (no == 4)
		FIO1SET = RLY_4;
	else if (no == 5)
		FIO1SET = RLY_5;
	else if (no == 6)
		FIO4SET = RLY_6;
	else if (no == 7)
		FIO4SET = RLY_7;
	else if (no == 8)
		FIO2SET = RLY_8;
	else
		printf("%s(): ERR tidak ada !\r\n", __FUNCTION__);
		
	
}

void unset_selenoid(unsigned int no )	{
	if (no>0 && no<=JML_RELAY) {
		data_f[JML_SUMBER*PER_SUMBER+no-1] = 0;
	}
	
	if (no == 1)
		FIO3CLR = RLY_1;
	else if (no == 2)
		FIO3CLR = RLY_2;
	else if (no == 3)
		FIO1CLR = RLY_3;
	else if (no == 4)
		FIO1CLR = RLY_4;
	else if (no == 5)
		FIO1CLR = RLY_5;
	else if (no == 6)
		FIO4CLR = RLY_6;
	else if (no == 7)
		FIO4CLR = RLY_7;
	else if (no == 8)
		FIO2CLR = RLY_8;
	else
		printf("%s(): ERR tidak ada !\r\n", __FUNCTION__);
		
	
}

void set_relay(int argc, char **argv) {
	int sumb=0;
	unsigned char str_selenoid[24];
	
	if (argc < 3) {
		printf("Perintah salah atau Parameter kurang !!\r\n");
		return;
	} else {
		sprintf(str_selenoid, "%s", argv[1]);			// no cron
		sumb = cek_nomer_valid(str_selenoid, JML_RELAY);
		if (sumb <= 0)	{
			printf("No relay salah !\r\n");
			return;	
		}
		
		sprintf(str_selenoid, "%s", argv[2]);
		if ( (strcmp(str_selenoid, "aktif") == 0) || (str_selenoid[0]=='1') || (strcmp(str_selenoid, "hidup") == 0) )	{
			set_selenoid(sumb);
		} else if ( (strcmp(str_selenoid, "mati")  == 0) || (str_selenoid[0]=='0') ) {
			unset_selenoid(sumb);
		}
	}
}



#endif
