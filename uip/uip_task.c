

#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"

#include "uip.h"
#include "uip_arp.h"

//#include "../monita/monita_uip.h"
#include "../tinysh/enviro.h"


#define 	PORT_MONITA 5001

#define prio ( tskIDLE_PRIORITY + 3)	// paling tinggi dari yang lain

#define RT_CLOCK_SECOND   (configTICK_RATE_HZ)
#define uipARP_FREQUENCY  (20)
#define uipMAX_BLOCK_TIME (RT_CLOCK_SECOND / 4)
#define pucUIP_Buffer ((struct uip_eth_hdr *) &uip_buf [0])

//#define PAKE_HTTP
//#define PAKE_TELNETD

extern struct t_env env2;
extern xTaskHandle hdl_ether;

static portTASK_FUNCTION( tunggu, pvParameters )
{
	portBASE_TYPE xARPTimer;
	uip_ipaddr_t xIPAddr;
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
#endif

	//  Initialise the local timers
	xStartTime = xTaskGetTickCount ();
	xARPTimer = 0;

	for (;;)
	{
		vTaskDelay(2);
		//if (enc28j60WaitForData (uipMAX_BLOCK_TIME) == pdTRUE)
		if (cek_paket())
		{
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
		    }
	}
}


void start_ether(void)
{
	xTaskCreate( tunggu, ( signed portCHAR * ) "UIP/TCP", 1024, NULL, prio, ( xTaskHandle * ) &hdl_ether );
}

void dispatch_tcp_appcall (void)
{
	#ifdef PAKE_HTTP
	if (uip_conn->lport == HTONS (80)) httpd_appcall ();
	#endif

#ifdef PAKE_TELNETD
  if (uip_conn->lport == HTONS (23))
    telnetd_appcall ();
#endif

  	if (uip_conn->lport == HTONS(PORT_MONITA))
	  monita_appcall();

	// gunakan rport untuk konek ke orang lain
	if (uip_conn->rport == HTONS(5002))
	  samb_appcall();
	  
	printf("dispatch call L=%d, R=%d\n", uip_conn->lport , uip_conn->rport);
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
