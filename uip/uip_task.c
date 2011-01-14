/********
 * 
 * 
 * 
 * 
 * 18 feb 2009
 * cek paket diperbaiki
 * jika ada error langsung reset enc saja
 * 
 ******/

#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"

#include "uip.h"
#include "uip_arp.h"

#include "../monita/monita_uip.h"
#include "../tinysh/enviro.h"

#ifdef PAKE_TELNETD
#include "apps/telnetd/telnetd.h"
#endif

#ifdef BOARD_KOMON_420_SAJA
#define BOARD_KOMON
#include "../adc/ad7708.h"
extern struct t_adc st_adc;
#endif

#ifdef BOARD_KOMON_A_RTD
#define BOARD_KOMON
#include "../adc/ad7708.h"
extern struct t_adc st_adc;
#endif

#ifdef BOARD_KOMON_B_THERMO
#define BOARD_KOMON
#include "../adc/ad7708.h"
extern struct t_adc st_adc;
#endif

//#define RT_CLOCK_SECOND   (configTICK_RATE_HZ)
#define RT_CLOCK_SECOND   ( configTICK_RATE_HZ / 10 )
#define uipARP_FREQUENCY  (20)
#define uipMAX_BLOCK_TIME (RT_CLOCK_SECOND / 4)
#define pucUIP_Buffer ((struct uip_eth_hdr *) &uip_buf [0])
#define RT_MENIT		(configTICK_RATE_HZ * 15)

#ifdef BOARD_KOMON
#define PAKE_HTTP
#endif

#ifdef BOARD_TAMPILAN
//#ifdef CARI_SUMBER
#define PAKE_HTTP
#endif

#ifdef PAKAI_HTTP
#define PAKE_HTTP
#endif

unsigned int paket_per_menit=0;
unsigned int paket_kita=0;
	

			
extern xTaskHandle hdl_ether;


static portTASK_FUNCTION( tunggu, pvParameters )
{
	struct t_env *envx;
	envx = (char *) ALMT_ENV;
	
	portBASE_TYPE xARPTimer;
	uip_ipaddr_t xIPAddr;
	int loop;
	int mul;
	unsigned int timer_menit=0;
	unsigned int loop_menit=0;	
	static volatile portTickType xStartTime, xCurrentTime;
	
	vTaskDelay(200);
	
	/* baca environtment (dapat IP dll dulu) */
	baca_env(0);
	printf("UIP : uip_init\r\n");

	uip_init ();
	
	//uip_ipaddr( xIPAddr, uipIP_ADDR0, uipIP_ADDR1, uipIP_ADDR2, uipIP_ADDR3 );
	uip_ipaddr( xIPAddr, envx->IP0, envx->IP1, envx->IP2, envx->IP3 );
	uip_sethostaddr( xIPAddr );

	printf("ARP : arp_init\r\n");
	uip_arp_init ();
	
	printf("Init ENC28J .. ");

	if (enc28j60Init() == 1)	{
		 printf(" .. ENC OK\r\n");
	}	else
		printf("ENC tidak respons !\r\n");

	#ifdef PAKE_HTTP
	printf("SIMPLE HTTP : init\r\n");
	httpd_init ();
	#endif
	
	#ifdef PAKE_TELNETD
	printf("SIMPLE TELNET : init\r\n");
    telnetd_init ();
	#endif

#ifdef BOARD_KOMON
    printf("MONITA : monita init\r\n");
//    monita_init();
#endif
	
#if (PAKAI_KONTROL == 1)
	printf("MONITA : monita kontrol init\r\n");
	kontrol_init();
#endif

//#ifdef BOARD_TAMPILAN
//#ifdef CARI_SUMBERNYA
#ifdef SAMPURASUN_SERVER
	printf("MONITA : sambungan_aktif init\r\n");
	sambungan_init();
	mul = 0;
#endif

#ifdef SAMPURASUN_CLIENT
    //printf("MONITA : monita init\r\n");
    printf("Monita : sampurasun client init !\r\n");
    monita_init();
#endif

#ifdef PAKAI_WEBCLIENT
		int ngitung=0;
		webclient_init();
		printf("webclient inited !\r\n");
		unsigned char datakeserver[512];
		int wclient=0, jmlData=0, nos=0, flag_nos=0, flag_sumber=0, jmlsumbernya=0;
		//int noPMaktif[JML_SUMBER];
		char il[256], dl[256];
		char ipdest[15], angkaangka[5];
		extern int kirimURL;
		extern char terkirimURL;
		extern int  noawal;
		int tiapKirim=950;
		int maxkirim=12;
#endif

#ifdef PAKAI_KIRIM_BALIK
	
	int anginkecil=0;
	int giliran;
	extern int sumber_datanya;
	extern int target_kirim;
	target_kirim = 4-1;
	sumber_datanya=3-1;

	//giliran=1-1;
	
	printf("Kirim Balik inited, Target data: sumber %d, target kirim: %d !\r\n", sumber_datanya+1, target_kirim+1);
#endif

	/*  Initialise the local timers */
	xStartTime = xTaskGetTickCount ();
	xARPTimer = 0;
	
	/* supaya cukup waktu buat siap2 */
	loop = -1000;
	
	#ifdef BOARD_KOMON_420_SABANG
	vTaskDelay(200);
	#endif

	#ifdef PAKAI_WEBCLIENT
		vTaskDelay(2000);
		vTaskDelay(2000);
	#endif
	//printf("mau loop for\r\n");
	for (;;)
	{
		vTaskDelay(1);
		//portYIELD();
		#if 1
		#ifdef PAKAI_WEBCLIENT
		if (envx->statusWebClient==1) {
			wclient++;
			if (envx->burst==1) {
				jmlData=kirimModul(1, 0, 0, il, dl);
				//jmlsumbernya=1;
				tiapKirim = 500;
			} else 
			{			// kirim 1-1
				/*
				#ifdef BANYAK_SUMBER
					struct t_sumber *pmx;
					pmx = (char *) ALMT_SUMBER;
					jmlsumbernya=0;
					for(selang=0; selang<JML_SUMBER; selang++) {
						if (pmx[selang].status==1) {
							printf("selang: %d, status: %d\r\n", selang, pmx[selang].status);
							noPMaktif[jmlsumbernya]=selang;
							jmlsumbernya++;
						}
					}
					if (jmlsumbernya==0) {
						jmlsumbernya=1;
					}
					//jmlData=kirimModul(0, noPMaktif[sumbernya], 0, il, dl);
					printf("jml Data: %d, sumbernya: %d\r\n", jmlData, sumbernya);
					// kenapa ini ??? harusnya tiap detik, knapa jadi tiap 4 detik ????
					//tiapKirim = (int) (100/jmlsumbernya);
					tiapKirim = (int) (195/jmlsumbernya);
					//printf("wclient: %d\r\n", wclient);
				#endif
				//*/
			}	
			if (wclient == tiapKirim) {
				ngitung++;
				
				struct t_sumber *sumber;
				sumber = (char *) ALMT_SUMBER;
				if (!flag_nos) {
					nos++;
					while(sumber[nos-1].status!=1) {	// cari modul sumber aktif
						if (nos>JML_SUMBER) {
							nos=0;
							jmlsumbernya=0;
						}
						nos++;
					}
				}
				jmlsumbernya++;
				//printf("kirim: %5d, nos: %d, wclient: %d, sumber.status: %d, jmlsumbernya: %d\r\n", ngitung, nos-1, wclient, sumber[nos-1].status, jmlsumbernya);
				
				// cek datanya PM ?? // <--- ternyata gak cuma PM, modul lain juga bisa
				if (sumber[nos-1].tipe==0 || sumber[nos-1].tipe==1 || sumber[nos-1].tipe==100)	{	// PM710 || PM810
					jmlData=kirimModul(0, nos-1, noawal, il, dl);
					if (jmlData==12) {
						flag_nos=1;
					} else {		// sudah habis, reset ke 0
						flag_nos=0;
						noawal=0;
					}
					//printf("nilai noawal: %d, flagnos: %d\r\n", noawal, flag_nos);
				} else	{
					jmlData=kirimModul(0, nos-1, 0, il, dl);
					noawal=0;
				}
				
				// hitung jml loop kirim ke server
				if (flag_sumber<jmlsumbernya) {
					flag_sumber=jmlsumbernya;
					tiapKirim=950/jmlsumbernya;
				}
				
				
				wclient = 0;				

				if (jmlData>0) {
					
					sprintf(ipdest, "%d.%d.%d.%d", envx->GW0, envx->GW1, envx->GW2, envx->GW3);
					//portENTER_CRITICAL();
					//sprintf(datakeserver, "%s?i=%s&p=diesel&j=%d&%s&%s", envx->berkas, envx->SN, jmlData, il, dl);
					strcpy(datakeserver, envx->berkas);
					strcat(datakeserver, "?i=");
					strcat(datakeserver, envx->SN);
					strcat(datakeserver, "&p=diesel&j=");
					sprintf(angkaangka, "%d", jmlData);
					strcat(datakeserver, angkaangka);
					strcat(datakeserver, "&");
					strcat(datakeserver, il);
					strcat(datakeserver, "&");
					strcat(datakeserver, dl);
					//portEXIT_CRITICAL();
					//printf("datakeserver: %s\r\n",datakeserver);
					webclient_get(ipdest, 80, datakeserver);
					
				}
			}
		}
		#endif
		
		//#if defined(BOARD_TAMPILAN) || defined (TAMPILAN_MALINGPING) 
		#ifdef SAMPURASUN_SERVER
		//#ifdef CARI_SUMBERNYA
		loop++;
		if (loop > 200) 		// 50, 40, 80
		{
			loop = 0;
			
			sambungan_connect(mul);	
			
			mul++;
			if (mul > JML_SUMBER) mul = 0;
			
		}
		#endif
		
		#ifdef PAKAI_KIRIM_BALIK
			anginkecil++;
			if (anginkecil > 2000) {	/* tiap 2 detik */
				
				kirim_balik_connect(target_kirim);
				
				anginkecil=0;
				//giliran++;
				//if (giliran > JML_SUMBER) mul = 0;
			}
		#endif
		
		//if (enc28j60WaitForData (uipMAX_BLOCK_TIME) == pdTRUE)
		if (cek_paket())	{
			//printf("masuk cekpaket \r\n");
			#if 1
			  paket_per_menit++;
			  /* Let the network device driver read an entire IP packet
		         into the uip_buf. If it returns > 0, there is a packet in the
		         uip_buf buffer. */
		      if ((uip_len = enc28j60Receive ()) > 0)
		      {				  
				  if (pucUIP_Buffer->type == htons (UIP_ETHTYPE_IP))
		    	  {
		    	            uip_arp_ipin ();
		    	            uip_input ();

		    	            /* If the above function invocation resulted in data that
		    	               should be sent out on the network, the global variable
		    	               uip_len is set to a value > 0. */
		    	            if (uip_len > 0)
		    	            {
		    	              uip_arp_out ();
		    	              enc28j60Send ();
							  paket_kita++;
		    	            }
		    	     }
		    	     else if (pucUIP_Buffer->type == htons (UIP_ETHTYPE_ARP))
		    	     {
		    	            uip_arp_arpin ();

		    	            /* If the above function invocation resulted in data that
		    	               should be sent out on the network, the global variable
		    	               uip_len is set to a value > 0. */
		    	            if (uip_len > 0)
		    	              enc28j60Send ();
		    	     }

		      }
		      #endif
		}
		else		{
		      /* The poll function returned 0, so no packet was
		         received. Instead we check if it is time that we do the
		         periodic processing. */
		      xCurrentTime = xTaskGetTickCount ();

		      if ((xCurrentTime - xStartTime) >= RT_CLOCK_SECOND)
		      {
		        portBASE_TYPE i;

		        /* Reset the timer. */
		        xStartTime = xCurrentTime;

		        /* Periodic check of all connections. */
		        for (i = 0; i < UIP_CONNS; i++)
		        {
		          uip_periodic (i);

		          /* If the above function invocation resulted in data that
		             should be sent out on the network, the global variable
		             uip_len is set to a value > 0. */
		          if (uip_len > 0)
		          {
		            //printf("S:%d", i);
					uip_arp_out ();
		            enc28j60Send ();
		          }
		        }

		#if UIP_UDP_KU
		        for (i = 0; i < UIP_UDP_CONNS; i++)
		        {
		          uip_udp_periodic (i);

		          /* If the above function invocation resulted in data that
		             should be sent out on the network, the global variable
		             uip_len is set to a value > 0. */
		          if (uip_len > 0)
		          {
		            uip_arp_out ();
		            enc28j60Send ();
		          }
		        }
		#endif /* UIP_UDP */

		        /* Periodically call the ARP timer function. */
		        if (++xARPTimer == uipARP_FREQUENCY)
		        {
		          uip_arp_timer ();
		          xARPTimer = 0;
		        }
		      }
			  
			  	if ((xCurrentTime - timer_menit) >= RT_MENIT)
			  	{
				  	extern unsigned int error_ENC;
					
					timer_menit = xCurrentTime;
					loop_menit++;				
					/*
					printf("%d menit, paket = %d, kita=%d, idle=%d\n", loop_menit, paket_per_menit, paket_kita, loop_idle);
					if (error_ENC != 0)
					{
						printf("ERR ENC = %X\n", error_ENC);	
					}*/
					
					paket_per_menit = 0;
					paket_kita = 0;
				}
		 }	// tanpa paket
		
		/*
		// proses ADC dipindah ke shell 1 Okt 2010// 
		#ifdef PAKAI_ADC
			#ifdef BOARD_KOMON_A_RTD
			proses_data_adc();
			#endif
			 
			#ifdef BOARD_KOMON_B_THERMO
			proses_data_adc();
			#endif
			 
			#ifdef BOARD_KOMON_420_SAJA
			proses_data_adc();
			#endif
		#endif
		//*/
	#endif
	}
}


void start_ether(void)
{	//8
	xTaskCreate( tunggu, ( signed portCHAR * ) "UIP/TCP", (configMINIMAL_STACK_SIZE * 12), \
		NULL, tskIDLE_PRIORITY + 2, ( xTaskHandle * ) &hdl_ether );
}

void dispatch_tcp_appcall (void)
{
	struct sambungan_state *sb = (struct sambungan_state *) &(uip_conn->appstate2);
	
#ifdef PAKE_HTTP
	if (uip_conn->lport == HTONS (80)) 
		httpd_appcall ();
#endif

#ifdef PAKE_TELNETD
  	if (uip_conn->lport == HTONS (23))
    	telnetd_appcall ();
#endif

#ifdef BOARD_KOMON
  	if (uip_conn->lport == HTONS(PORT_MONITA))
		monita_appcall();
#endif

#if (PAKAI_KONTROL == 1)
	else if (uip_conn->lport == HTONS(PORT_KONTROL))
		kontrol_appcall();
#endif

//#ifdef CARI_SUMBERNYA
#ifdef SAMPURASUN_SERVER
	// gunakan rport untuk konek ke orang lain
	if (uip_conn->rport == HTONS(PORT_MONITA))
	{
		samb_appcall();
	}
	else if (uip_conn->rport == HTONS(PORT_DAYTIME))
	{
		daytime_appcall();
	}
#endif

#ifdef PAKAI_WEBCLIENT
	/* webclient */
	if (uip_conn->rport == HTONS(80))	{
		webclient_appcall();
	}	  
#endif

#ifdef PAKAI_KIRIM_BALIK
	//*
	if (uip_conn->rport == HTONS(PORT_KIRIM_BALIK))	{
		kirim_balik_appcall();
	}
	//*/	  
#endif
	//printf("%s(): port = %d\r\n", __FUNCTION__, uip_conn->rport);
}



void dispatch_udp_appcall (void)
{
#ifdef CFG_SNTP
  if (uip_udp_conn->rport == HTONS (123))
    sntp_appcall ();
  else
#endif
#ifdef CFG_DHCP
  if (uip_udp_conn->rport == HTONS (DHCPC_SERVER_PORT))
    dhcpc_appcall ();
#endif
}



