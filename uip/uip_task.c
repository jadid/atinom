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


#define prio ( tskIDLE_PRIORITY + 3)	// paling tinggi dari yang lain

#define RT_CLOCK_SECOND   (configTICK_RATE_HZ)
#define uipARP_FREQUENCY  (20)
#define uipMAX_BLOCK_TIME (RT_CLOCK_SECOND / 4)
#define pucUIP_Buffer ((struct uip_eth_hdr *) &uip_buf [0])
#define RT_MENIT		(configTICK_RATE_HZ * 15)

#ifdef BOARD_KOMON
#define PAKE_HTTP
#endif
//#define PAKE_TELNETD


unsigned int paket_per_menit=0;
unsigned int paket_kita=0;
	
extern struct t_env env2;
extern xTaskHandle hdl_ether;
extern unsigned int loop_idle;

static portTASK_FUNCTION( tunggu, pvParameters )
{
	portBASE_TYPE xARPTimer;
	uip_ipaddr_t xIPAddr;
	int loop;
	int mul;
	unsigned int timer_menit=0;
	unsigned int loop_menit=0;
	
	
	static volatile portTickType xStartTime, xCurrentTime;

	vTaskDelay(200);
	// baca environtment (dapat IP dll dulu)
	baca_env(0);
	
	printf("UIP : uip_init\n");
	uip_init ();
	//uip_ipaddr( xIPAddr, uipIP_ADDR0, uipIP_ADDR1, uipIP_ADDR2, uipIP_ADDR3 );
	uip_ipaddr( xIPAddr, env2.IP0, env2.IP1, env2.IP2, env2.IP3 );
	uip_sethostaddr( xIPAddr );

	printf("ARP : arp_init\n");
	uip_arp_init ();

	printf("Init ENC28J .. ");

	if (enc28j60Init() == 1)
	{
		 printf(" .. OK\n");
	}
	else
		printf("ENC tidak respons !\n");

	#ifdef PAKE_HTTP
	httpd_init ();
	#endif

	#ifdef PAKE_TELNETD
    telnetd_init ();
	#endif

#ifdef BOARD_KOMON
    printf("MONITA : monita init\n");
    monita_init();
#endif

	
#ifdef BOARD_TAMPILAN
	sambungan_init();
	mul = 0;
#endif

	//  Initialise the local timers
	xStartTime = xTaskGetTickCount ();
	xARPTimer = 0;
	
	// supaya cukup waktu buat siap2
	loop = -1000;

	for (;;)
	{
		vTaskDelay(1);
		
		#ifdef BOARD_TAMPILAN
		loop++;
		if (loop > 50) 		// 50, 40, 80
		{
			loop = 0;
			
			//mul=8;
			sambungan_connect(mul);	
			
			mul++;
			if (mul > JML_MODUL) mul = 0;
		}
		#endif
	
		//if (enc28j60WaitForData (uipMAX_BLOCK_TIME) == pdTRUE)
		if (cek_paket())
		{
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
		}
		else
		{
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
					loop_idle = 0;
				}
		 }	// tanpa paket
	}
}


void start_ether(void)
{
	xTaskCreate( tunggu, ( signed portCHAR * ) "UIP/TCP", 1024, NULL, prio, ( xTaskHandle * ) &hdl_ether );
}

void dispatch_tcp_appcall (void)
{
	struct sambungan_state *sb = (struct sambungan_state *) &(uip_conn->appstate2);
	
#ifdef PAKE_HTTP
	if (uip_conn->lport == HTONS (80)) httpd_appcall ();
#endif

#ifdef PAKE_TELNETD
  if (uip_conn->lport == HTONS (23))
    telnetd_appcall ();
#endif

#ifdef BOARD_KOMON
  	if (uip_conn->lport == HTONS(PORT_MONITA))
	{
		monita_appcall();
	}
#endif

#ifdef BOARD_TAMPILAN
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



/* 
 * 18 feb 2009, idle hook
 * dipakai untuk cek paket
 * soalnya dalam loop 1 ms, masih sering paket hilang ?
 * 
 * 
 * */
void vApplicationIdleHook( void )
{
	loop_idle++;	
}
