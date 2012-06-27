
#include "FreeRTOS.h"
#include "monita/monita_uip.h"


#ifdef PAKAI_MODEM_GSM

	#define nRespM 			200
	#define JML_MEM_SMS		10

	void init_mem_sms();
	int membaca_sms();
	int ambil_sms();
	int aksi_sms();
	int menghapus_sms(int idx);
	void kirimCmdModem(char *psCmdM,char * psRespM);
	int cek_AT_cmd();
	

#endif
