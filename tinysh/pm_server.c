/*
	proses PM_sever
	
	1 Des 2009, furkan jadid
	daun biru engineering
	
*/



#include <math.h>
#include <float.h>

#ifdef AMBIL_PM
#define TUNGGU_PM_TX	100
#define TUNGGU_PM_RX	100
#define  LIAT

#include "../monita/monita_uip.h"
#include "../modbus/low_mod.h"


#ifdef PAKAI_KTA
	unsigned char addr_KTA;
//unsigned short get_KTA(unsigned short reg, unsigned char uk);
#endif


struct d_pmod pmod;

xTaskHandle hdl_proses_pm;
//static	int urut_PM710 = 0;
static	int konting2 = 0;
static	int loop;
static 	char buf_rx[64];

#ifdef PAKAI_PM

float satuan_kwh[JML_SUMBER];
float satuan_kw[JML_SUMBER];
float satuan_volt[JML_SUMBER];
float satuan_volt2[JML_SUMBER];
float satuan_amp[JML_SUMBER];
float satuan_amp2[JML_SUMBER];
struct t_kontrol_PM kontrol_PM[JML_SUMBER];
#include "../modbus/low_mod.c"


#define TXDE	BIT(24)

static void proses_pm (int no, int alamatPM, int urut_PM710)	{
	unsigned int jum_balik;
	int i,j, k;
	char *st;
	int timeout=0;
	
	struct t_sumber *pmx;
	pmx = (char *) ALMT_SUMBER;

	konting2++;
	k=0;
	
	//printf("%s() no: %d, almt %d, urut_PM710: %d\r\n", __FUNCTION__, no, alamatPM, urut_PM710);
	
	#ifdef TIPE_PM710
	if (urut_PM710 == 0)	{
	   jum_balik = get_PM710(alamatPM, reg_satuan_710, 4);
	} else if (urut_PM710 == 1)	{
	   jum_balik = get_PM710(alamatPM, reg_kva_710, 7);  //kVA, kVAR, PF, volt L-L, L-N, A, Hz		// 4010
	} else if (urut_PM710 == 2)	{
	   jum_balik = get_PM710(alamatPM, reg_ampA_710, 4); //ampA, B, C & N								// 4020
	} else if (urut_PM710 == 3) {
	   jum_balik = get_PM710(alamatPM, reg_voltA_C_710, 6); //voltA_B, VB_C, VA_C, VA_N, VB_N, VC_N, P_A, P_B, P_C	// 4030
	} else if (urut_PM710 == 4)	{
	   jum_balik = get_PM710(alamatPM, reg_kwh_710, 6);  //kWh, kVAh, & kVArh		// 4000
	} else if (urut_PM710 == 5)	{
	   jum_balik = get_PM710(alamatPM, reg_kwA_710, 9);  //kW_A, kW_B, kW_C, kva_A, kva_B, kva_C, kVAr_A, kVAr_B, kVAr_C		// 4000
/*
	} else if (urut_PM710 == 5) {
		#ifdef PAKAI_KTA
		jum_balik = get_KTA(alamatPM, reg_KTA, 9);			// 30, 9
		#endif
	} else if (urut_PM710 == 6) {
		#ifdef PAKAI_KTA
		jum_balik = get_KTA(alamatPM, reg_satuan_KTA, 6);
		#endif
//*/
	}
	#endif
	
	#ifdef TIPE_PM810
	#ifdef LIAT
	printf("\r\n___810 ....");
	#endif
		if (urut_PM710==0)    {
   	    	jum_balik = get_PM710(alamatPM, reg_satuan_810, 5);		// 
   	    } else if (urut_PM710==1)		{
     		jum_balik = get_PM710(alamatPM, meter_current_810, 5);  //Current A, B, C, N, - , & ave
   		} else if (urut_PM710==2)		{
     	    jum_balik = get_PM710(alamatPM, meter_voltage_810, 9);  //Voltage A-B, B-C, C-A, L-L, A-N, B-N, C-N , L-N
   		} else if (urut_PM710==3)		{
     	    jum_balik = get_PM710(alamatPM, meter_power_810, 12); // kwA, kwB, kwC, kW, kvarA, kvarB, kvarC, kvar, kvaA, kvaB, kvaC, kva
   		} else if (urut_PM710==4)		{
     	    jum_balik = get_PM710(alamatPM, meter_faktor_810, 4);   //pfA, pfB, pfC, pf
   		} else if (urut_PM710==5)		{
     	    jum_balik = get_PM710(alamatPM, reg_frek_810, 1); //Hz
   		} else if (urut_PM710==6)		{
   		   jum_balik = get_PM710(alamatPM, meter_energi2_810, 8);
   		} else if (urut_PM710==7)		{
   		   jum_balik = get_PM710(alamatPM, meter_energi_vah_810, 8);
   		}

	#endif
	
	#ifdef LIAT
	printf("___Minta ke PM -%d : %d : \r\n", urut_PM710, sizeof(pmod));
	#endif
	
	st = (char *) &pmod;
	FIO0SET = TXDE;		// on	---> bisa kirim
	for (i=0; i< sizeof(pmod); i++)	{
		#ifdef LIAT
		printf("%02hX ", *st);
		#endif
		serX_putchar(PAKAI_PM, st++, TUNGGU_PM_TX);
	}
	//FIO0CLR = TXDE;		// on	---> bisa kirim, kasih delay klo mau pake ini !!!!
	
	#ifdef LIAT
	printf("\r\n");
	#endif
	
	/*
	#ifdef LIAT
	st = (char *) &pmod;
	for (i=0; i< sizeof(pmod); i++)	{
		printf("%2X ", (unsigned char) *st++);
	}
	#endif
	//*/
	
	i=0;

	while(1)	{
		#if (PAKAI_PM == 1) 
			if (ser1_getchar(1, &buf_rx[i], TUNGGU_PM_RX ) == pdTRUE)
		#elif (PAKAI_PM == 2) 			
			if (ser2_getchar(1, &buf_rx[i], TUNGGU_PM_RX ) == pdTRUE)
		#elif (PAKAI_PM == 3)
			if (ser3_getchar(1, &buf_rx[i], TUNGGU_PM_RX ) == pdTRUE)
		#endif 

		{
			#ifdef LIAT
			printf("%02hX ",buf_rx[i]);
			#endif
	
			i++;
			#ifdef PAKAI_MAX485
				if (i == jum_balik+sizeof(pmod)) break;
			#else
				if (i == jum_balik) break;
			#endif

		}	else	{
			timeout++;
			if (timeout > 20)	{
				#ifdef LIAT
				printf("%s(): alamat %d : timeout: %d\r\n", __FUNCTION__, alamatPM, urut_PM710);
				#endif
				break;
			}
		}
	}
	
	/*
	if (urut_PM710==5) {
		printf("Nilai KTA   : ");
		for (i=0; i<jum_balik; i++) {
			printf("%02hhX ", buf_rx[i]);	
		}
		printf("\r\n");
	}
	if (urut_PM710==6) {
		printf("Satuan  KTA : ");
		for (i=0; i<jum_balik; i++) {
			printf("%02hhX ", buf_rx[i]);	
		}
		printf("\r\n");
	}
	//*/
	if (pmx[no].tipe==0)	{	// 710
	#ifdef TIPE_PM710
		taruh_data_710(no, urut_PM710);
	#endif
	} else if (pmx[no].tipe==1) {
	#ifdef TIPE_PM810
		taruh_data_810(no, urut_PM710);
	#endif
	}
	/*
	// simpan data //
	for (i=0; i<JML_SUMBER; i++) {
		if (sumber[i].status == 1 && sumber[i].alamat > 0) {
			//printf("masuk data SUMBER: %d\r\n",i+1);
			memcpy( (char *) &data_f[i*PER_SUMBER], (char *) &asli_PM710[0], (PER_SUMBER*sizeof (float)) );
			
		}
	}
	//*/
	
/*
	//urut_PM710++;
	//if (urut_PM710 > 6) urut_PM710 = 0;
	urut_PM710++;
	#ifdef PAKAI_KTA
	if (urut_PM710 > 6) 
	#else
	if (urut_PM710 > 4) 
	#endif
	urut_PM710 = 0;
//*/
}


// ambil seluruh data tiap 1 PM. Looping berdasarkan Jml request PM (urut_PM710)
void ambil_pm(int k) {
	struct t_sumber *pmx;
	pmx = (char *) ALMT_SUMBER;
	int i=0;
	
	int req;
	if (pmx[k].tipe==0) {				// PM710
		req = 6;
	} else if (pmx[k].tipe==1) {		// PM810
		req = 8;
	}
		
	
	//printf("Ambil data Power Meter ke-%d / %d\r\n", k+1, req);
	//while(i<JML_REQ_PM) {
	while(i<req) {
		vTaskDelay(2);			// MIN: 2
		//printf("%s() almt %d, k: %d, i: %d\r\n", __FUNCTION__, pmx[k].alamat, k, i);
		proses_pm(k, pmx[k].alamat, i);		// i: PM810: 8 request (0-7), k: 
		i++;
	}
}

#endif

portTASK_FUNCTION( pm_task, pvParameters )	{
	int i=0,j=0,k=0;
	int z=0;
	int alamatClient=0;
	//urut_PM710 = 0;
	konting2 = 0;
	#ifdef PAKAI_KTA
	addr_KTA = 1;
	#endif
	int muternya=0;
	
	vTaskDelay(300);
	
	printf("Ambil data Power Meter init !\r\n");
	// flush RX
	for (i=0; i<100; i++)
		#ifdef PAKAI_SERIAL_1
			ser1_getchar(1, &loop, 20 );
		#endif
		#ifdef PAKAI_SERIAL_2
			ser2_getchar(1, &loop, 20 );
		#endif
		#ifdef PAKAI_SERIAL_3
			ser3_getchar(1, &loop, 20 );
		#endif

	for(i=0; i<(sizeof(data_f)/sizeof(float)); i++) {
		data_f[i] = 0.00;
	}
	serX_putstring(3, "Testing dari serial 3 ...Testing dari serial 3 ...2x...");	
	struct t_sumber *pmx;
	pmx = (char *) ALMT_SUMBER;
	
	vTaskDelay(1000);
	for (;;)	{
		vTaskDelay(20);
		alamatClient = (int) pmx[k].alamat;
		
		if ( (k<JML_SUMBER) && (alamatClient>0) ) {
			//printf("k: %d, alamat: %d\r\n", k, alamatClient);
			if (pmx[k].status==1) {
				#ifdef PAKAI_PM
				if (pmx[k].tipe==0 || pmx[k].tipe==1) {		// 0: ambil power meter
					ambil_pm(k);
				}
				#endif

				#ifdef PAKAI_KTA
				
				#endif
			}
			
			portENTER_CRITICAL();
			memcpy( (char *) &data_f[k*PER_SUMBER], (char *) &asli_PM710[k], (PER_SUMBER*sizeof (float)) );
			portEXIT_CRITICAL();
		}
		
		k++;
		if (k>(JML_SUMBER-1)) {
			k=0;
		}
		
		#ifdef LIAT
		if (loop==100) {
			for (z=0; z<JML_SUMBER; z++) {
				//printf("frek %d: %f, asli: %f\r\n", z, data_f[z*PER_SUMBER+10], asli_PM710[z].frek);
			}
			loop=0;	
		}
		#endif 
		
		loop++;
		//printf("masih muter\r\n");
	}
		
}

void init_task_pm(void)
{
	xTaskCreate( pm_task, ( signed portCHAR * ) "PM_S", (configMINIMAL_STACK_SIZE * 10), \
		NULL, tskIDLE_PRIORITY + 1, &hdl_proses_pm );	
}

 

/*
#ifdef PAKAI_KTA
//unsigned short 	wind_dir;
//float 			f_wind_speed;

//unsigned short wind_satuan;
//unsigned short wind_speed_tr;		// retransmision value
//unsigned short wind_dir_tr;



unsigned short get_KTA(unsigned short reg, unsigned char uk) {
   unsigned short dcrc;
   int i;

   reg_flag = reg;

   if (reg != reg_kwh)
   reg = reg - 1;

   pmod.addr = (unsigned char)   addr_KTA;
   pmod.command = (unsigned char) command_baca;
   pmod.reg_hi = (unsigned char) ((reg & 0xFF00) >> 8);
   pmod.reg_lo = (unsigned char) (reg & 0x00FF);
   pmod.jum_hi = (unsigned char) ((uk & 0xFF00) >> 8);
   pmod.jum_lo = (unsigned char) (uk & 0x00FF);;

   dcrc = usMBCRC16((unsigned char *) &pmod, sizeof (pmod)-2);
   pmod.crc_lo = (unsigned char) ((dcrc & 0xFF00) >> 8);
   pmod.crc_hi = (unsigned char) (dcrc & 0x00FF);;

   	return (1 + 1 + 1 + (uk * 2) + 2);
}


#endif
//*/


#endif
