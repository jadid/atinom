/*
	proses PM_server
	
	1 Des 2009, furkan jadid
	daun biru engineering
	
	11 Jun 2011, Afrendy Bayu
*/

#include "FreeRTOS.h"
#include "hardware.h"

//#define LIAT
//#define LIAT_TX
//#define LIAT_RX
//#define CEK_PM
//#define TIMEOUT

#ifdef AMBIL_PM

#ifndef __PM_SERVER__
#define __PM_SERVER__

#include <math.h>
#include <float.h>



#include "monita/monita_uip.h"
#include "modbus/mbcrc.h"
#include "modbus/low_mod.h"

int ambil_pmnya(char no, char alamat, char tipe, char sequen);

char pm_sukses=0;
struct d_pmod pmod;
int lenpmod = sizeof(pmod);

//static	int urut_PM710 = 0;
//static	int konting2 = 0;
//static	int loop;
unsigned char buf_rx[64];
extern float  data_f[];

#ifdef PAKAI_PM



//#include "modbus/low_mod.c"

int jmlPM=0,k=0;

unsigned char jml_sequen(char tipe) {
	if 		(tipe==PM710) return 6;			// PM710
	#ifdef TIPE_PM810
	else if (tipe==PM810) return 8;			// PM810
	#endif
	#ifdef TIPE_MICOM_M300
	else if (tipe==MICOM_M300) return 5;	// MICOM M300 <--- nilai return disesuaikan
	#endif
	#ifdef TIPE_MICOM_P127
	else if (tipe==MICOM_P127) return 7; 	// P_127
	#endif
	#ifdef TIPE_ION8600
	else if (tipe==ION8600) return 5; 		// ION8600
	#endif
	#ifdef TIPE_A2000
	else if (tipe==A2000) return 10; 		// A2000
	#endif
	#ifdef TIPE_TFX_ULTRA
	else if (tipe==TFX_ULTRA) return 1; 	// tfx = 20
	#endif
}

int sedot_pm() {
	int ap;
	struct t_sumber *p_sbrq;
	p_sbrq = (char *) ALMT_SUMBER;
	
	struct t_env *env3;
	
	unsigned char tipe, alamatClient;
	
	#ifdef LIAT
	printf("%5d. sedot: %d/%d\r\n", jmlPM, k+1, JML_SUMBER);
	#endif
	
	jmlPM++;		// kok ini kudu ada ????
	if ( (k<JML_SUMBER) && (alamatClient>0) ) {
		#ifdef TIPE_TFX_ULTRA
			if (p_sbrq[k].tipe==TFX_ULTRA)	{	// 20
				iCountTFX++;
				nFlowTFXlama = data_f[];
				if (iCountTFX==env3->intReset)	{
					if (data_f[]<env3->intTole)	{
						iCountTFX=0;
						
						
					}
				}
			}
		#endif
		
		
		
		if (p_sbrq[k].status==1)		// 1
		{
			tipe = p_sbrq[k].tipe;
			alamatClient = p_sbrq[k].alamat;
			
			#ifdef LIAT
			printf("sumber: %d aktif\r\n", alamatClient);
			#endif
			
			ap = ambil_pmnya(k, alamatClient, tipe, jml_sequen(tipe));
			
			//printf("ap: %d _____ k: %d __ d0: %.2f -- %.2f\r\n", ap, k, data_f[3], asli_PM710[k].kw);
			if (ap==1) {
				portENTER_CRITICAL();
				memcpy( (char *) &data_f[k*PER_SUMBER], (char *) &asli_PM710[k], (PER_SUMBER*sizeof (float)) );
				portEXIT_CRITICAL();
			}
			
			
			
			#if 0
			printf("data : %d : %.2f - %.2f\r\n", k*JML_SUMBER+0, asli_PM710[k].kwh,   data_f[k*JML_SUMBER+0]);
			printf("data : %d : %.2f - %.2f\r\n", k*JML_SUMBER+2, asli_PM710[k].kvarh, data_f[k*JML_SUMBER+2]);
			printf("data : %d : %.2f - %.2f\r\n", k*JML_SUMBER+3, asli_PM710[k].kw,    data_f[k*JML_SUMBER+3]);
			printf("ap: %d _____ k: %d __ d0: %.2f -- %.2f\r\n", ap, k, data_f[3], asli_PM710[k].kw);
			printf("------------------------------\r\n");
			#endif
		} 
		#if 0
		else {
			printf("sumber: %d mati\r\n", k);
		}
		#endif
	}
	
	k++;
	if (k>(JML_SUMBER-1)) {
		k=0;
	}
	return 0;
	//data_pm();
}

int proses_pm (char no, char alamatPM, char tipe, char urut_PM710)	{
	unsigned int jum_balik;
	unsigned char *st;
	int timeout=0, i;
	
	#ifdef TIPE_PM710
	if (tipe==0) {
		#ifdef LIAT
		printf("\r\n___710 ....");
		#endif
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
		}
	}
	#endif
	
	#ifdef TIPE_PM810
	if (tipe==PM810) {					// 1
	#ifdef LIAT
	printf("___810 ....\r\n");
	#endif
		if (urut_PM710==0)    {
   	    	jum_balik = get_PM710(alamatPM, reg_satuan_810, 5);		// 
   	    } else if (urut_PM710==1)		{
     		jum_balik = get_PM710(alamatPM, meter_current_810, 5);  //Current A, B, C, N, - , & ave
   		} else if (urut_PM710==2)		{
     	    jum_balik = get_PM710(alamatPM, meter_voltage_810, 9);  //Voltage A-B, B-C, C-A, L-L, A-N, B-N, C-N , L-N
   		} else if (urut_PM710==3)		{
     	    jum_balik = get_PM710(alamatPM, meter_power_810, 12); // kwA, kwB, kwC, kW, kvarA, kvarB, kvarC, kvar, kvaA, kvaB, kvaC, kva
   			#ifdef LIAT
   			//printf("jml balik POwer urut 3: %d \r\n", jum_balik);
   			#endif
   		} else if (urut_PM710==4)		{
     	    jum_balik = get_PM710(alamatPM, meter_faktor_810, 4);   //pfA, pfB, pfC, pf
   			#ifdef LIAT
   			//printf("jml balik PF urut 4: %d \r\n", jum_balik);
   			#endif
   		} else if (urut_PM710==5)		{
     	    jum_balik = get_PM710(alamatPM, reg_frek_810, 1); //Hz
     	    #ifdef LIAT
     	    //printf("jml balik frek urut 5: %d \r\n", jum_balik);
     	    #endif
   		} else if (urut_PM710==6)		{
   		   jum_balik = get_PM710(alamatPM, meter_energi2_810, 8);
   		} else if (urut_PM710==7)		{
   		   jum_balik = get_PM710(alamatPM, meter_energi_vah_810, 8);
   		}
	}
	#endif

	#ifdef TIPE_MICOM_M300
		if (tipe==MICOM_M300) {			// 2
		#ifdef LIAT
		printf("___MICOM M300.. edit disini ....");
		#endif
			if (urut_PM710==0)    {
				jum_balik = get_M300(alamatPM, meter_voltage_micom, 12);		// 
			} else if (urut_PM710==1)		{
				jum_balik = get_M300(alamatPM, meter_current_micom, 8);  //
			} else if (urut_PM710==2)		{
				jum_balik = get_M300(alamatPM, meter_frek_micom, 2);  //
			} else if (urut_PM710==3)		{
				jum_balik = get_M300(alamatPM, meter_power_micom, 7); //
			}
			else if (urut_PM710==4)			{
				jum_balik = get_M300(alamatPM, meter_energi_micom, 6);
			} else {
				#ifdef LIAT
				//printf("jml balik POwer urut 3: %d \r\n", jum_balik);
				#endif
			}
		}
	#endif

	#ifdef TIPE_MICOM_P127
		if (tipe==MICOM_P127) {			// 3
		#ifdef LIAT
		//printf("___MICOM P127 .. edit disini ....\n");
		#endif
			if (urut_PM710==0)    {
				jum_balik = get_P127(alamatPM, meter_vrms_p127, 6);		// 
			} else if (urut_PM710==1)	{	
				jum_balik = get_P127(alamatPM, meter_power_p127, 4);// 
			} else if (urut_PM710==2)	{
				jum_balik = get_P127(alamatPM, meter_current_p127, 8);  //
			} else if (urut_PM710==3)	{
				jum_balik = get_P127(alamatPM, meter_frek_p127, 2);  //
			} else if (urut_PM710==4)	{
				jum_balik = get_P127(alamatPM, meter_energi_p127, 6); //meter_power_p127, 7); //
			} else if (urut_PM710==5)	{
				jum_balik = get_P127(alamatPM, meter_pf_p127, 2);
			} else if (urut_PM710==6)	{
				jum_balik = get_P127(alamatPM, meter_kva_p127, 4); 
			} 
			/*else if (urut_PM710==7)	
			{
				jum_balik = get_P127(alamatPM, meter_kva_p127, 4); 
			} */
			else
			{
				#ifdef LIAT
				//printf("jml balik POwer urut 3: %d \r\n", jum_balik);
				#endif
			}
		}
	#endif
	
	#ifdef TIPE_TFX_ULTRA
	if (tipe==TFX_ULTRA) 	{
		#ifdef LIAT
		printf("___TFX_ULTRA__proses_pm\n");
		#endif
			if (urut_PM710==0)    {
				jum_balik = get_tfx(alamatPM, tfx_integer, 7);		// 
			} 
			else if (urut_PM710==1)		{	
				jum_balik = get_tfx(alamatPM, tfx_single_precision, 7);// 
			} 
			else if (urut_PM710==2)		{
				jum_balik = get_tfx(alamatPM, tfx_double_precision, 7);  //
			} 
			else	{
				#ifdef LIAT
				printf("Jml balik Power urut 20: %d \r\n", jum_balik);
				#endif
			}
		}
	#endif

	#ifdef LIAT_TX
	printf("___Minta ke PM -%d : %d : \r\n", urut_PM710, sizeof(pmod));
	#endif
	
	st = (char *) &pmod;
	FIO0SET = TXDE;		// on	---> bisa kirim
	FIO0CLR = RXDE;
	for (i=0; i< lenpmod; i++)	{
		#ifdef LIAT_TX
		printf("%02hX ", *st);
		#endif
		serX_putchar(PAKAI_PM, st++, TUNGGU_PM_TX);
	}
	
	#ifdef LIAT_TX
	printf("\r\n");
	printf("  Terima data .. \r\n");
	#endif
	
	i=0;
	#if 1
	while(1)	{
		#if (PAKAI_PM == 1) 
			if (ser1_getchar(1, &buf_rx[i], TUNGGU_PM_RX ) == pdTRUE)
		#elif (PAKAI_PM == 2) 			
			if (ser2_getchar(1, &buf_rx[i], TUNGGU_PM_RX ) == pdTRUE)
		#elif (PAKAI_PM == 3)
			if (ser3_getchar(1, &buf_rx[i], TUNGGU_PM_RX ) == pdTRUE)
		#endif 

		{
			#ifdef LIAT_RX
			printf("%02hX ",buf_rx[i]);
			#endif
	
			i++;
			pm_sukses = 1;
			#ifdef PAKAI_MAX485
				if (i == jum_balik+lenpmod) break;
			#else
				if (i == jum_balik) break;
			#endif
		}	else	{
			timeout++;
			if (timeout > 20)	{
				#if defined (LIAT_RX) || defined(TIMEOUT)
				printf("%s(): alamat %d : timeout: %d\r\n", __FUNCTION__, alamatPM, urut_PM710);
				#endif
				
				pm_sukses=0;
				break;
			}
		}
		
	}
	#endif
	
	#ifdef LIAT
	printf("\r\n");
	#endif
	
	vTaskDelay(1);
	FIO0SET = RXDE;
	if (pm_sukses) {
		return i;
	} else {
		return 0;
	}
}

// ambil seluruh data tiap 1 PM. Looping berdasarkan Jml request PM (urut_PM710)
int ambil_pmnya(char no, char alamat, char tipe, char sequen) {
	unsigned char i=0;
	int hasil_pm=0,gg=0;
	unsigned char *ph;
	unsigned short mbhcrc;
	unsigned char lo, hi;
		
	#ifdef LIAT
	printf("Ambil data Power Meter ke-%d - %d sequen\r\n", no+1, sequen);
	#endif

	//FIO0CLR = RXDE;		// mode brutal
	while(i<sequen) {
		vTaskDelay(20);			// MIN: 2, -delay10=5PM- -delay4=8PM-
		#ifdef LIAT
		printf("%s() almt %d, k: %d, i: %d\r\n", __FUNCTION__, alamat, no, i);
		#endif

		hasil_pm = proses_pm(no, alamat, tipe, i);		// i: PM810: 8 request (0-7), k:
		
		#ifdef LIAT
		printf("hasil proses_pm: %d\r\n", hasil_pm);
		#endif
		
		if (hasil_pm==0) {
			return 0;
		}
		#ifdef LIAT_RX
			printf("\r\nbalasan: %d\r\n", hasil_pm);
			#if 0
				ph = &buf_rx[HD];
				for(gg=0;gg<hasil_pm-lenpmod-2; gg++) {
					printf("%02X ", *ph++);
				}
				printf("\r\n");
			#endif
		#endif

		ph = &buf_rx[HD];
		mbhcrc = usMBCRC16((unsigned char *) ph, (hasil_pm-lenpmod-2), 1);
		hi = ((mbhcrc&0xFF00)>>8);
		lo = (mbhcrc&0xFF);
		
		#ifdef LIAT_RX
			printf("crc: %04X: %02X %02X\r\n", mbhcrc, hi, lo);
			printf("\r\n-------------------------\r\n");
		#endif
		
		if (hi==buf_rx[hasil_pm-1] && lo==buf_rx[hasil_pm-2]) {
			pm_sukses=1;
			#ifdef LIAT_RX
			printf(" pm_sukses=1: %d \r\n", i);
			#endif
		} else {
			pm_sukses=0;
			#ifdef LIAT_RX
			printf(" pm_sukses=0: %d ", i);
			#endif
			return 0;
		}
		
		if 		  (tipe==PM710 && pm_sukses==1)	{		// PM710 		= 0
		#ifdef TIPE_PM710
			portENTER_CRITICAL();
			taruh_data_710(no, i);
			portEXIT_CRITICAL();
		#endif
		#ifdef TIPE_PM810
		} else if (tipe==PM810 && pm_sukses==1) {		// PM810 		= 1
			portENTER_CRITICAL();
			taruh_data_810(no, i);
			portEXIT_CRITICAL();
		#endif
		#ifdef TIPE_MICOM_M300
		} else if (tipe==MICOM_M300 && pm_sukses==1) {	// MICOM_M300	= 2
			portENTER_CRITICAL();
			taruh_data_301(no, i);
			portEXIT_CRITICAL();
		#endif
		#ifdef TIPE_MICOM_P127
		} else if (tipe==MICOM_P127 && pm_sukses==1) {	// MICOM_P127	= 3
			portENTER_CRITICAL();
			//taruh_data_127(no, i);
			taruh_data_P127(no, i);
			portEXIT_CRITICAL();
		#endif
		#ifdef TIPE_ION8600
		} else if (tipe==ION8600 && pm_sukses==1) {		// ION8600		= 4
			portENTER_CRITICAL();
			taruh_data_ION8600(no, i);
			portEXIT_CRITICAL();
		#endif
		#ifdef TIPE_A2000
		} else if (tipe==A2000 && pm_sukses==1) {		// A2000		= 5
			portENTER_CRITICAL();
			taruh_data_A2000(no, i);
			portEXIT_CRITICAL();
		#endif
		#ifdef TIPE_TFX_ULTRA
		} else if (tipe==TFX_ULTRA && pm_sukses==1) {	// TFX_ULTRA	= 20
			portENTER_CRITICAL();
			taruh_data_tfx(no, i);
			portEXIT_CRITICAL();
		#endif
		}
		i++;
	}
	return 1;
}

void cek_coil(unsigned char almx, int regx, int jmlx)	{
	struct d_pmod pmodx;
	unsigned char *stx, *x;	
	char i;

	//printf("  %s() ---> alm: %d, reg: %d, jml: %d\r\n", __FUNCTION__, almx, regx, jmlx);
	modbus_rtu(&x, almx, 0x01, regx, jmlx);
	
	
	#if 0
	stx = (char *) &pmodx;
	for (i=0; i< sizeof(pmodx); i++)	{
		printf("%02hX ", *stx++);
		//serX_putchar(PAKAI_PM, st++, TUNGGU_PM_TX);
	}
	printf("\r\n");

	paket_modbus_rtu(&pm, almx, 0x01, regx, jmlx);

	stx = (char *) &pm;
	for (i=0; i<8; i++)	{
		printf("%02hX ", *stx++);
		//serX_putchar(PAKAI_PM, st++, TUNGGU_PM_TX);
	}
	printf("\r\n");
	#endif

   	return ( 3+ jmlx + 2 );	// slave address, function, bytecount, data, crc
}

void cek_holding(unsigned char almx, int regx, int jmlx)	{
	unsigned char *stx, *x;	
	char i;

	//printf("  %s() ---> alm: %d, reg: %d, jml: %d\r\n", __FUNCTION__, almx, regx, jmlx);
	modbus_rtu(&x, almx, 0x03, regx, jmlx);
	//paket_modbus_rtu(&x, almx, 0x03, regx, jmlx);

	#if 0
	stx = &x;
	for (i=0; i<8; i++)	{
		//#ifdef DEBUG_MODBUS_ELEMEN
		printf("%02x ", stx[i]);
		//#endif
		serX_putchar(PAKAI_PM, stx[i], TUNGGU_PM_TX);
	}
	printf("\r\n");
	#endif
	
	//minta_modbus(&x, jmlx);
	
	//printf("%02x %02x %02x %02x\r\n", *pm, *(pm+1), *(pm+2), *(pm+3));

   	return (3 + jmlx + 2);	// slave address, function, bytecount, data, crc
}

void set_coil(unsigned char almx, int regx, int jmlx)	{
	unsigned char *stx, *x;	
	char i;

	modbus_rtu(&x, almx, 0x05, regx, jmlx);
   	return (3 + jmlx + 2);	// slave address, function, bytecount, data, crc
}

#if 0
void data_pm() {
	printf("Cek data PM\r\n");
	extern char * judulnya_pm[];
	printf("%-15s : %f\r\n", judulnya_pm[0], data_PM710[0].kwh);
	printf("%-15s : %f\r\n", judulnya_pm[1], data_PM710[0].kvah);
	printf("%-15s : %f\r\n", judulnya_pm[2], data_PM710[0].kvarh);
	printf("%-15s : %f\r\n", judulnya_pm[3], data_PM710[0].kw);
	printf("%-15s : %f\r\n", judulnya_pm[4], data_PM710[0].kva);
	printf("%-15s : %f\r\n", judulnya_pm[5], data_PM710[0].kvar);
	printf("%-15s : %f\r\n", judulnya_pm[6], data_PM710[0].pf);
	printf("%-15s : %f\r\n", judulnya_pm[7], data_PM710[0].volt1);
	printf("%-15s : %f\r\n", judulnya_pm[8], data_PM710[0].volt2);
	printf("%-15s : %f\r\n", judulnya_pm[9], data_PM710[0].amp);
	printf("%-15s : %f\r\n", judulnya_pm[10], data_PM710[0].frek);
	printf("%-15s : %f\r\n", judulnya_pm[11], data_PM710[0].ampA);
	printf("%-15s : %f\r\n", judulnya_pm[12], data_PM710[0].ampB);
	printf("%-15s : %f\r\n", judulnya_pm[13], data_PM710[0].ampC);
	printf("%-15s : %f\r\n", judulnya_pm[14], data_PM710[0].ampN);
	printf("%-15s : %f\r\n", judulnya_pm[15], data_PM710[0].voltA_B);
	printf("%-15s : %f\r\n", judulnya_pm[16], data_PM710[0].voltB_C);
	printf("%-15s : %f\r\n", judulnya_pm[17], data_PM710[0].voltA_C);
	printf("%-15s : %f\r\n", judulnya_pm[18], data_PM710[0].voltA_N);
	printf("%-15s : %f\r\n", judulnya_pm[19], data_PM710[0].voltB_N);
	printf("%-15s : %f\r\n", judulnya_pm[20], data_PM710[0].voltC_N);
	printf("%-15s : %f\r\n", judulnya_pm[21], data_PM710[0].kwA);
	printf("%-15s : %f\r\n", judulnya_pm[22], data_PM710[0].kwB);
	printf("%-15s : %f\r\n", judulnya_pm[23], data_PM710[0].kwC);
	printf("%-15s : %f\r\n", judulnya_pm[24], data_PM710[0].kvaA);
	printf("%-15s : %f\r\n", judulnya_pm[25], data_PM710[0].kvaB);
	printf("%-15s : %f\r\n", judulnya_pm[26], data_PM710[0].kvaC);
	printf("%-15s : %f\r\n", judulnya_pm[27], data_PM710[0].kvarA);
	printf("%-15s : %f\r\n", judulnya_pm[28], data_PM710[0].kvarB);
	printf("%-15s : %f\r\n", judulnya_pm[29], data_PM710[0].kvarC);
	printf("%-15s : %f\r\n", judulnya_pm[30], data_PM710[0].pfA);
	printf("%-15s : %f\r\n", judulnya_pm[31], data_PM710[0].pfB);
	printf("%-15s : %f\r\n", judulnya_pm[32], data_PM710[0].pfC);
}
#endif
#endif

#endif

#endif
