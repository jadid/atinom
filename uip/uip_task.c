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

#ifdef PAKAI_ETH
//#if 1

#include "uip.h"
#include "uip_arp.h"



#ifdef PAKAI_WEBCLIENT
	#include "../webserver/webclient/webclient.h"
#endif


//#include "../monita/monita_uip.h"
//#include "../app/monita/monita_uip.h"
#include "../tinysh/enviro.h"

#ifdef PAKE_TELNETD
#include "apps/telnetd/telnetd.h"
#endif

#ifdef PAKAI_WEBCLIENT
	#ifdef WEBCLIENT_GPS
		#include "../app/gps/gps.h"
	#endif
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
//#define PAKE_HTTP
#endif

#ifdef BOARD_TAMPILAN
//#ifdef CARI_SUMBER
//#define PAKE_HTTP
#endif

#ifdef PAKAI_HTTP
#define PAKE_HTTP
#endif

//#define DEBUG_WEBCLIENT
//#define PAKAI_ETH_TEST
//#define DEBUG_UIP_TASK
//#define DEBUG_ARP_TASK

unsigned int paket_per_menit=0;
unsigned int paket_kita=0;
	
unsigned char status_eth = 0;
		
extern xTaskHandle hdl_ether;
//#define DEBUG_UIP_TASK

#ifdef PAKAI_ETH_TEST
	void eth_test_init(void) {
		//uip_listen(HTONS(PORT_MODBUSTCP));
		uip_listen(HTONS(21));
	}
#endif

#ifdef PAKAI_ETH_TEST
void eth_test_appcall(void) {
	if(uip_newdata() || uip_rexmit()) {
		uip_send("ok\n", 3);
		printf("kirim OK !!\r\n");
	}
}
#endif

#ifdef PAKAI_MODBUSTCP
	void modbustcp_init() {
		printf(" Monita : modbus TCP init: %d !\r\n", PORT_MODBUSTCP);
		//uip_listen(HTONS(PORT_MODBUSTCP));
		//uip_listen(HTONS(502));
	}
	
	void modbustcp_appcall() {
		int len;
		unsigned char str_monita[32];
		int i;
		unsigned char ipne[32];		
		struct t_reg_modbus regmod;
		
		if (uip_newdata())	{
			len = uip_datalen();
			sprintf(ipne, " :%d.%d.%d.%d", \
			htons(uip_conn->ripaddr[0]) >> 8, htons(uip_conn->ripaddr[0]) & 0xFF, \
			htons(uip_conn->ripaddr[1]) >> 8, htons(uip_conn->ripaddr[1]) & 0xFF );
			
			printf("newdata modbus = %d %s\n", len, ipne);
			portENTER_CRITICAL();
			memcpy(str_monita, (char *) uip_appdata, len);
			portEXIT_CRITICAL();
			
			if (len >= 10)		{
				for (i=0; i<len; i++) {
					printf("%d ", str_monita[i]);
					if (i>=0 && i<5) {
						regmod.ref[i] = (unsigned char) str_monita[i];
					}
					if (i==6)
						regmod.ui = (unsigned char) str_monita[i];
					if (i==7)	
						regmod.func = (unsigned char) str_monita[i];
					if (i==8) {
						regmod.reg = (unsigned int) (str_monita[i]<<8) + (unsigned int) str_monita[i+1] + 1;
						i++;
					}
					if (i==10) {
						regmod.nReg = (unsigned int) (str_monita[i]<<8) + (unsigned int) str_monita[i+1];
						i++;
					}
				}	
				printf("\r\n");

				printf ("idCoil: %d, Reg: %d, nData: %d\r\n", regmod.func, regmod.reg, regmod.nReg);
				
				aksiModbus(regmod);
			}
			else 	{
				uip_close();	
			}	
		}
	
		if (uip_poll())	{
			uip_close();	
		}
	}
	
	void aksiModbus(struct t_reg_modbus regmod) {
		//struct t_reg_modbus_respon modrespon;
		int tt=0, www;
		
		for(tt=0; tt<regmod.nReg; tt++) {
			www = (rand()%1000+5);
			regmod.data[tt*2] = (www>>8);
			regmod.data[tt*2+1] = www;
			printf("www: %d : %d %d\r\n", www, regmod.data[tt], regmod.data[tt+1]);
		}
		printf("\r\n");
		regmod.nReg *= 2;
		regmod.len = 3 + regmod.nReg;
		
		for(tt=0; tt<(9+regmod.nReg); tt++) {
			printf("%hX ", * ((char *) &regmod+tt));
		}
		printf("\r\n");
		
		/*
		portENTER_CRITICAL();
		memcpy(xdata.buf, (char *) &data_float, sizeof (data_float));
		portEXIT_CRITICAL();
		//*/	
		//send data ke network
		uip_send((char *) &regmod, 9+regmod.nReg);
	}
#endif


static portTASK_FUNCTION( tunggu, pvParameters )	{
	unsigned char spi1, spi2;
	int r=0, g;
	
	struct t_env *envx;
	envx = (char *) ALMT_ENV;
	
	portBASE_TYPE xARPTimer;
	uip_ipaddr_t xIPAddr;
	int loop;
	int mul;
	unsigned int timer_menit=0;
	unsigned int loop_menit=0;	
	static volatile portTickType xStartTime, xCurrentTime;
	
	vTaskDelay(300);
	
	/* baca environtment (dapat IP dll dulu) */
	baca_env(0);
	printf("Init Application ......\r\n");
	printf(" UIP : uip_init\r\n");

	uip_init ();
	
	//uip_ipaddr( xIPAddr, uipIP_ADDR0, uipIP_ADDR1, uipIP_ADDR2, uipIP_ADDR3 );
	uip_ipaddr( xIPAddr, envx->IP0, envx->IP1, envx->IP2, envx->IP3 );
	uip_sethostaddr( xIPAddr );

	printf(" ARP : arp_init\r\n");
	uip_arp_init ();
	
	#if defined(BOARD_KOMON_420_SABANG_2_3) && defined(PAKAI_ENCX24J600) 
		printf(" Init ENC624J600.. ");
		
		if ( (status_eth=Enc624Init())== 1)	{
			 printf(".. ENC OK\r\n");
		}	else	{
			printf("ENC tidak respons !\r\n");
		}
	#elif defined(BOARD_KOMON_420_SABANG_2_3) && defined(PAKAI_ENC28J60) 
		printf(" Init ENC28J .. ");
		if ( (status_eth=enc28j60Init())== 1)	{
			 printf(" .. ENC OK\r\n");
		}	else	{
			printf(" ENC tidak respons !\r\n");
		}
	#else
		printf(" Init ENC28J .. ");
		if ( (status_eth=enc28j60Init())== 1)	{
			 printf(" .. ENC OK\r\n");
		}	else	{
			printf(" ENC tidak respons !\r\n");
		}
	#endif
	
	vTaskDelay(100);
	
	if (status_eth==0)	{
		printf("\r\nEthernet ERROR !!!\r\n");
		printf("Komunikasi dan Perintah via Ethernet tidak bisa dilakukan !!!\r\n");
		vTaskDelay(1000);
		for (;;)	{
			vTaskDelay(1000);
		}
	}

	#if defined(PAKE_HTTP) || defined(PAKAI_HTTP)
	printf(" Monita : http init\r\n");
	httpd_init ();
	#endif
	
	#ifdef PAKAI_ETH_TEST
		printf("    ETH TEST INIT... !!!\r\n");
		eth_test_init();
	#endif

	#ifdef PAKAI_MODBUSTCP
		modbustcp_init();
	#endif
	
	#ifdef PAKE_TELNETD
	printf(" MONITA : telnet init\r\n");
    telnetd_init ();
	#endif

#ifdef BOARD_KOMON
    //printf(" MONITA : monita init\r\n");
#endif
	
#if (PAKAI_KONTROL == 1)
	printf(" MONITA : monita kontrol init\r\n");
	kontrol_init();
#endif

//#ifdef BOARD_TAMPILAN
//#ifdef CARI_SUMBERNYA
#ifdef SAMPURASUN_SERVER
	//printf(" MONITA : sambungan_aktif init\r\n");
	sambungan_init();
	printf(" Monita : sampurasun server init !!\n");	
	mul = 0;
#endif

#ifdef SAMPURASUN_CLIENT
    //printf("MONITA : monita init\r\n");
    monita_init();
    printf(" Monita : sampurasun client init !\r\n");
#endif

#ifdef PAKAI_WEBCLIENT
		int ngitung=0;
		webclient_init();
		printf(" Monita : webclient init !! [%d:%s]\r\n", envx->statusWebClient, (envx->statusWebClient?"aktif":"mati"));
		unsigned char datakeserver[512];
		int wclient=0, jmlData=0, nos=0, flag_nos=0, flag_sumber=0, jmlsumbernya=0;
		//int noPMaktif[JML_SUMBER];
	#ifdef WEBCLIENT_DATA
		char il[256], dl[256];
		char angkaangka[5];
		#ifdef DEBUG_WEBCLIENT
		char countwc[15];
		int wc=0;
		#endif
		int maxkirim=12;
		extern int kirimURL;
		extern char terkirimURL;
		extern int  noawal;
		
		int selang=0;
		
	#endif
		char ipdest[15];
		int tiapKirim=950;

	#ifdef  PAKAI_GPS
		#ifdef WEBCLIENT_GPS
			extern nmeaINFO infoGPS;
			extern nmeaPOS dposGPS;
			char waktu[20];		//		20110609-220228-000
			char bujur[12], lintang[12], arah[12], satelit[12];
			tiapKirim = 1;		// 1 detik
			int menit = 0;
			
			//double lintang=0.0, bujur=0.0;
		#endif
	#endif
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

	//printf("UIP_LLH_LEN: %d\r\n", UIP_LLH_LEN);
	
	//  Initialise the local timers //
	xStartTime = xTaskGetTickCount ();
	xARPTimer = 0;
	
	// supaya cuc fkup waktu buat siap2 //
	loop = -1000;
	
	#if defined(BOARD_KOMON_420_SABANG) || defined (BOARD_KOMON_420_SABANG_2_3)
	vTaskDelay(200);
	#endif

	int gg;

	#ifdef PAKAI_WEBCLIENT
		vTaskDelay(2000);
		if (envx->statusWebClient==1) {
			for(gg=0; gg<5; gg++)
				vTaskDelay(2000);
		}
	#endif
	
	
	unsigned char ipne[32];	
	//printf("mau loop for\r\n");
	for (;;)	{
		vTaskDelay(1);
		//portYIELD();
		#ifdef PAKAI_WEBCLIENT
		if (envx->statusWebClient==1) {
			wclient++;
			if (envx->burst==1) {
				#ifdef WEBCLIENT_DATA
					jmlData=kirimModul(1, 0, 0, il, dl);
					//jmlsumbernya=1;
					tiapKirim = 500;
				#endif
			} else 
			{			// kirim 1-1
				/*
				#ifdef BANYAK_SUMBER
					struct t_sumber *pmx;
					pmx = (char *) ALMT_SUMBER;
					jmlsumbernya=0;
					for(selang=0; selang<JML_SUMBER; selang++) {
						if (pmx[selang].status==1) {
							//printf("selang: %d, status: %d\r\n", selang, pmx[selang].status);
							//noPMaktif[jmlsumbernya]=selang;
							jmlsumbernya++;
						}
					}
					if (jmlsumbernya==0) {
						jmlsumbernya=1;
					}
					//jmlData=kirimModul(0, noPMaktif[sumbernya], 0, il, dl);
					//printf("jml Data: %d, sumbernya: %d\r\n", jmlData, jmlsumbernya);
					// kenapa ini ??? harusnya tiap detik, knapa jadi tiap 4 detik ????
					//tiapKirim = (int) (100/jmlsumbernya);
					tiapKirim = (int) (1900/jmlsumbernya);
					//printf("wclient: %d\r\n", wclient);
				#endif
				//*/
			}
			
			#ifdef WEBCLIENT_DATA
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
				/*
				if (flag_sumber<jmlsumbernya) {
					flag_sumber=jmlsumbernya;
					//tiapKirim=950/jmlsumbernya;
					tiapKirim=2000/jmlsumbernya;
				}
				//*/
				
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
					#ifdef DEBUG_WEBCLIENT
						strcat(datakeserver, "&cc=");
						wc++;
						sprintf(countwc, "%d", wc);
						strcat(datakeserver, countwc);
					#endif
					strcat(datakeserver, "&");
					strcat(datakeserver, il);
					strcat(datakeserver, "&");
					strcat(datakeserver, dl);
					//portEXIT_CRITICAL();
					
					//printf("datakeserver: %s\r\n",datakeserver);
					webclient_get(ipdest, PORT_HTTP, datakeserver);
					
				}
			}
			#endif
			
			#ifdef WEBCLIENT_GPS
				if (wclient>1000) {		// 1 detik, 60000 = 60 detik = 1 menit
					wclient=0;
					menit++;
				}
				if (menit == tiapKirim) {
					//wclient = 0;
					menit=0;
					sprintf(ipdest, "%d.%d.%d.%d", envx->GW0, envx->GW1, envx->GW2, envx->GW3);
					strcpy(datakeserver, envx->berkas);
					strcat(datakeserver, "?i=");
					strcat(datakeserver, envx->SN);
					strcat(datakeserver, "&p=diesel&w=");
					sprintf(waktu, "%d%02d%02d-%02d%02d%02d-%03d", \
						infoGPS.utc.year, infoGPS.utc.mon, infoGPS.utc.day, \
						infoGPS.utc.hour, infoGPS.utc.min, infoGPS.utc.sec, infoGPS.utc.hsec);
					strcat(datakeserver, waktu);

					sprintf(satelit, "&s=%d", infoGPS.satinfo.inuse);
					strcat(datakeserver, satelit);
					
					sprintf(lintang, "&l=%.3f", nmea_radian2degree(dposGPS.lat));
					strcat(datakeserver, lintang);
					
					sprintf(bujur, "&b=%.3f", nmea_radian2degree(dposGPS.lon));
					strcat(datakeserver, bujur);
					
					sprintf(arah, "&a=%.3f", infoGPS.direction);
					strcat(datakeserver, arah);

					//printf("datakeserver: %d : %s\r\n",strlen(datakeserver), datakeserver);
					webclient_get(ipdest, PORT_HTTP, datakeserver);
				}
			#endif
		}
		#endif
		
		//#if defined(BOARD_TAMPILAN) || defined (TAMPILAN_MALINGPING) 
		#ifdef SAMPURASUN_SERVER
		//#ifdef CARI_SUMBERNYA
		loop++;
		if (loop > 500) 		// 50, 40, 80		// aslinya 200
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
		//#if defined(PAKAI_ENC28J60) || defined(PAKAI_ENCX24J600)
		if (cek_paket())	{
		//#else
		//if (0) {
		//#endif
			//printf("masuk cek paket !!!\r\n");
			#if 1
			  paket_per_menit++;
			  /* Let the network device driver read an entire IP packet
		         into the uip_buf. If it returns > 0, there is a packet in the
		         uip_buf buffer. */
		      #if defined(PAKAI_ENC28J60)
		      if ((uip_len = enc28j60Receive ()) > 0)
		      #elif defined(PAKAI_ENCX24J600)
		      if ((uip_len = Enc624Terima()) > 0)
		      #endif
		      {  
					//printf("uiplen: %d, tipe: %d, IP: %d, ARP: %d\r\n", \
					//	uip_len, pucUIP_Buffer->type, htons (UIP_ETHTYPE_IP), htons (UIP_ETHTYPE_ARP));
				  //printf("tipe: %d\r\n", pucUIP_Buffer->type);
				  
				  
				  if (pucUIP_Buffer->type == htons (UIP_ETHTYPE_IP))
		    	  {
					#ifdef DEBUG_UIP_TASK
						printf("UIP_ETHTYPE_IP: %04x masuk.\r\n", UIP_ETHTYPE_IP);
						#if 1
						//for (gg=0; gg<uip_len; gg++) {
						for (gg=0; gg<32; gg++) {
							if (gg%16==0) { 
								//printf("\r\n");
								printf("  "); 
							} else if (gg%8==0) {
								printf("  "); 
							}
							printf("%02x ", uip_buf[gg]);
						}
						printf("\r\n\r\n");
						#endif
					#endif
						uip_arp_ipin ();
		    	        uip_input ();		// uip.h #define  uip_process(UIP_DATA) << ada data masuk
		    	       
		    	            /* If the above function invocation resulted in data that
		    	               should be sent out on the network, the global variable
		    	               uip_len is set to a value > 0. */
		    	            if (uip_len > 0)    {
		    	              uip_arp_out ();
		    	              #if defined(PAKAI_ENC28J60)
		    	              enc28j60Send ();
		    	              #elif defined(PAKAI_ENCX24J600)
		    	              //printf("______kirim balek UIP !!!___\r\n");
		    	              Enc624Kirim();
		    	              #endif
							  paket_kita++;
		    	            }
		    	     }
		    	     else if (pucUIP_Buffer->type == htons (UIP_ETHTYPE_ARP))
		    	     {
		    	          #ifdef DEBUG_ARP_TASK
		    	          printf("UIP_ETHTYPE_ARP: %04x masuk.\r\n", UIP_ETHTYPE_ARP);
		    	          #if 0
							//for (gg=0; gg<uip_len; gg++) {
							for (gg=0; gg<32; gg++) {
								if (gg%16==0) { 
									//printf("\r\n");
									printf("  ");
								} else if (gg%8==0) {
									printf("  "); 
								}
								printf("%02x ", uip_buf[gg]);
							}
							printf("\r\n\r\n");
						  #endif
						  #endif
		    	          
		    	            uip_arp_arpin ();
							//printf("______terima ARP !!!___: %d\r\n", uip_len);
		    	            /* If the above function invocation resulted in data that
		    	               should be sent out on the network, the global variable
		    	               uip_len is set to a value > 0. */
		    	            if (uip_len > 0)	{
		    	              #if defined(PAKAI_ENC28J60)
		    	              enc28j60Send ();
		    	              #elif defined(PAKAI_ENCX24J600)
		    	              //printf("______kirim balek ARP !!!___\r\n");
		    	              Enc624Kirim();
		    	              #endif
						  }
		    	     }

		      }
		      #endif
		      
		      //BacaStat();
		}
		else		{
			//printf("tidak ada paket  !!!\r\n");
		    /* The poll function returned 0, so no packet was
		         received. Instead we check if it is time that we do the
		         periodic processing. */
			xCurrentTime = xTaskGetTickCount ();

			if ((xCurrentTime - xStartTime) >= RT_CLOCK_SECOND)      {
				portBASE_TYPE i;

		        /* Reset the timer. */
		        xStartTime = xCurrentTime;

		        /* Periodic check of all connections. */
		        for (i = 0; i < UIP_CONNS; i++)		{
		          uip_periodic (i);

		          /* If the above function invocation resulted in data that
		             should be sent out on the network, the global variable
		             uip_len is set to a value > 0. */
					if (uip_len > 0)	{
						//printf("S:%d", i);
						uip_arp_out ();
						//enc28j60Send ();
		            
						if (uip_len > 0) {
							#if defined(PAKAI_ENC28J60)
							enc28j60Send ();
							#elif defined(PAKAI_ENCX24J600)
							Enc624Kirim();
							#endif
						}
					}
				}

		#if UIP_UDP_KU
		        for (i = 0; i < UIP_UDP_CONNS; i++)		{
					uip_udp_periodic (i);

		          /* If the above function invocation resulted in data that
		             should be sent out on the network, the global variable
		             uip_len is set to a value > 0. */
		            if (uip_len > 0)	{
						uip_arp_out ();
						//enc28j60Send ();
						#if defined(PAKAI_ENC28J60)
							enc28j60Send ();
						#elif defined(PAKAI_ENCX24J600)
							Enc624Kirim();
						#endif
					}
				}
		#endif /* UIP_UDP */

		        /* Periodically call the ARP timer function. */
		        if (++xARPTimer == uipARP_FREQUENCY)	{
					uip_arp_timer ();
					xARPTimer = 0;
				}
			}
			  
			if ((xCurrentTime - timer_menit) >= RT_MENIT)	{
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
	}
}

#ifdef BOARD_TAMPILAN
	void start_ether(void)	{	//8
		xTaskCreate( tunggu, ( signed portCHAR * ) "UIP/TCP", (configMINIMAL_STACK_SIZE * 10), \
			NULL, tskIDLE_PRIORITY + 1, ( xTaskHandle * ) &hdl_ether );
	}
#else
	void start_ether(void)	{	//8
		xTaskCreate( tunggu, ( signed portCHAR * ) "UIP/TCP", (configMINIMAL_STACK_SIZE * 15), \
			NULL, tskIDLE_PRIORITY + 1, ( xTaskHandle * ) &hdl_ether );
	}
#endif

void dispatch_tcp_appcall (void)	{
	struct sambungan_state *sb = (struct sambungan_state *) &(uip_conn->appstate2);

	//printf("lport: %d, rport:%d, HTTP: %d\r\n", uip_conn->lport, uip_conn->rport, HTONS (PORT_HTTP));
	
#ifdef PAKAI_ETH_TEST
	//if (uip_conn->lport == HTONS (PORT_MODBUSTCP))
	if (uip_conn->lport == HTONS (21))
		eth_test_appcall ();
#endif
	
#if defined(PAKE_HTTP) || defined(PAKAI_HTTP)
	if (uip_conn->lport == HTONS (PORT_HTTP))
		httpd_appcall ();
#endif

#ifdef PAKE_TELNETD
  	if (uip_conn->lport == HTONS (23))
    	telnetd_appcall ();
#endif

#ifdef PAKAI_MODBUSTCP
  	if (uip_conn->lport == HTONS (PORT_MODBUSTCP))
		modbustcp_appcall();
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
	if (uip_conn->rport == HTONS(PORT_HTTP))	{
		//printf("panggil webclient_appcall, rport:%d\r\n", uip_conn->rport);
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



void dispatch_udp_appcall (void)	{
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

#endif

