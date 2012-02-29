

#ifdef PAKAI_MODEM_SERIAL

#ifndef __SYSTEM_SMS__
#define __SYSTEM_SMS__

#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"

int cek_awal(void) {
	char cmd_ftp[60];
	int i=0, flag=2;
	
	//while(1) {
	for(i=0; i<20; i++) {
		vTaskDelay(50);
		strcpy(cmd_ftp,"");
		baca_serial(cmd_ftp, 10, 20);
		
		#ifdef DEBUG_FTP
		printf("respon%d: %s : %d %s\r\n", i+1, cmd_ftp, strlen(cmd_ftp), __FUNCTION__);
		#endif

		if ((strncmp(cmd_ftp,"+WIND",5)!=0) && (strlen(cmd_ftp)==0))	{	// sudah tidak +WIND
			flag--;
			if (flag==0)		break;
		}
	}
	/*
	sprintf(cmd_ftp, "ATE0\r\n");
	serX_putstring(PAKAI_GSM_FTP, cmd_ftp);
	baca_serial(cmd_ftp, 10, 20);
	//*/
	return 10;
}

#endif
#endif
