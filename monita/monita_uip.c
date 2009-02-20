/*
 * monita_uip.c
 *
 *  Created on: Dec 10, 2008
 *      Author: jadid
 *
 * server monita dengan UIP stack
 *
 * 19 februari 2009
 * minta daytime service ke server
 * 
 * 20 februari 2009
 * model monita_appcall diperbaiki
 * tidak usah pakai protothread ... banyak BAD TCP menurut wireshark
 * 
 */


#include "monita_uip.h"
#include "uip.h"
#include <stdio.h>
#include <stdlib.h>

#include "../GPIO/gpio.h"
extern struct t2_konter konter;

struct t_xdata xdata  __attribute__ ((section (".eth_test")));
struct t_data_float data_float  __attribute__ ((section (".eth_test")));

extern struct t_data_float s_data[JML_SUMBER];

unsigned int loop_kirim;

#ifdef BOARD_KOMON

void monita_init(void)
{
	loop_kirim = 0;
	uip_listen(HTONS(PORT_MONITA));
}

void monita_appcall(void)
{
	int l;
	unsigned char buf[32];
	float temp_rpm;
	int i;
	int t;
	
#ifdef DEBUG	
	loop_kirim++;
	printf("\n * %d:", loop_kirim);
	if (uip_connected())
	{
		printf("konek\n");
	}
	if (uip_closed())
	{
		printf("close\n");
	}
	if (uip_aborted())
	{
		printf("abort\n");
	}
	if (uip_timedout())
	{
		printf("timeout\n");
	}	
	if(uip_acked()) 
	{
		printf("ack\n");
	}
	if (uip_poll())
	{
		printf("poll\n");	
	}
#endif
	
	if (uip_newdata())
	{
		l = uip_datalen();
		//printf("newdata = %d,", l);
		
		if (l >= 10)
		{
			portENTER_CRITICAL();
			memcpy(buf, (char *) uip_appdata, 10);
			portEXIT_CRITICAL();
			
			if (strncmp(buf, "sampurasun", 10) == 0)
			{
				loop_kirim++;
				//printf("data monita");	
				t=0;
				for (i=0; i<10;i++)
				{
					data_float.data[t] = konter.t_konter[i].hit;
					t++;
					// cari frekuensi
					temp_rpm = (float) 1000000000.00 / (konter.t_konter[i].beda); // beda msh dlm nS
					// rpm
					data_float.data[t] = temp_rpm * 60;
					t++;
				}
				
				xdata.nomer = loop_kirim;
				//xdata.flag = 30;		//pulsa
				xdata.flag = 10;
				xdata.alamat = 1;		// stacking board nomer 1
				strcpy(xdata.mon, "monita1");
				memcpy(xdata.buf, (char *) &data_float, sizeof (data_float));
				
				//send data ke network
				uip_send((char *) &xdata, sizeof (xdata));
			}
			//printf("\n");	
		}	
	}
}
#endif

#ifdef BOARD_TAMPILAN
////*************************************** ethernet tampilan **************************************/

extern struct t_sumber sumber[];
extern struct t_titik titik[];
extern unsigned char daytime[32];

struct t_status status[JML_MODUL] __attribute__ ((section (".usb_text")));


void sambungan_init(void)
{
	int i;
	
	for (i=0; i<JML_MODUL; i++)
	{
		status[i].stat = 0;
		status[i].reply = 0;
	}	
	printf("sambungan ethernet init\n");	
}

/* 
 * sambungan konek dipanggil periodik oleh uip_task.c
 * sesuai dengan server / modul yang akan
 * diminta datanya, misalnya dipanggil dari 
 * task led
 */

void sambungan_connect(int no)
{
	struct uip_conn *conn;
	struct sambungan_state *samb3;
	uip_ipaddr_t ip_modul;
	
	if (sumber[no].status == 1 && status[no].stat == 0)	// harus diaktfikan lagi
	{
		#ifdef DEBUG
		printf("Init sumber %d : %10s : ", (no+1), sumber[no].nama);
		printf("%d.%d.%d.%d\r\n", sumber[no].IP0, sumber[no].IP1, sumber[no].IP2, sumber[no].IP3);
		#endif
		
		uip_ipaddr(ip_modul, sumber[no].IP0, sumber[no].IP1, sumber[no].IP2, sumber[no].IP3);	
		
		conn = uip_connect(ip_modul, HTONS(PORT_MONITA));
		if (conn == NULL)
		{
			printf("ERR: Koneksi Penuh\r\n");	
			return ;
		}
		//printf("..%X..L=%d, R=%d .. OK\r\n", conn, conn->lport, conn->rport);

		status[no].stat = 1;
		samb3 = (struct sambungan_state *) &conn->appstate2;
		samb3->nomer_samb = no;
		samb3->state = 1;
		samb3->timer = 0;
	
		PSOCK_INIT((struct psock *) &samb3->p, (char *) &samb3->in_buf, sizeof (struct t_xdata));	
		return;
	}
	/* daytime server, harusnya hanya ada 1 server */
	else if (sumber[no].status == 5 && status[no].stat == 0)	// harus diaktfikan lagi
	{
		#ifdef DEBUG
		printf("Init daytime %d : %10s : ", (no+1), sumber[no].nama);
		printf("%d.%d.%d.%d\r\n", sumber[no].IP0, sumber[no].IP1, sumber[no].IP2, sumber[no].IP3);
		#endif
		uip_ipaddr(ip_modul, sumber[no].IP0, sumber[no].IP1, sumber[no].IP2, sumber[no].IP3);	
		
		conn = uip_connect(ip_modul, HTONS(PORT_DAYTIME));
		if (conn == NULL)
		{
			printf("ERR: Koneksi Penuh\r\n");	
			return ;
		}
		//printf("..%X..L=%d, R=%d .. OK\r\n", conn, conn->lport, conn->rport);

		status[no].stat = 1;
		samb3 = (struct sambungan_state *) &conn->appstate2;
		samb3->nomer_samb = no;
		samb3->state = 1;
		samb3->timer = 0;
	
		PSOCK_INIT((struct psock *) &samb3->p, (char *) &samb3->in_buf, 26);	
		return;
	}
	return;
}

static PT_THREAD(samb_thread(struct sambungan_state *sbg))
{
	int i;
	
	PSOCK_BEGIN(&sbg->p);
	PSOCK_SEND_STR(&sbg->p, "sampurasun");	
	PSOCK_READBUF(&sbg->p);
	
	portENTER_CRITICAL();	
	if (strncmp(sbg->in_buf.mon, "monita1", 7) == 0)
	{
		status[sbg->nomer_samb].reply++;
		//memcpy((char *) &data_float, sbg->in_buf.buf, sizeof (data_float));
		memcpy((char *) &s_data[sbg->nomer_samb].data, sbg->in_buf.buf, sizeof (data_float));
		
		#ifdef DEBUG_DATA
		printf(" %d:", sbg->nomer_samb);
		for (i=0; i<10; i++)
		{
			printf("%3.3f ", s_data[sbg->nomer_samb].data[i]);	
		}
		printf("\n");
		#endif
		
		//cari titik pemilik data ini 
		for (i=0; i<(TIAP_MESIN * JML_MESIN); i++)
		{
			if (titik[i].ID_sumber == (sbg->nomer_samb + 1))
			{
				titik[i].data = s_data[sbg->nomer_samb].data[titik[i].kanal -1];	
			}	
		}		
	}
	portEXIT_CRITICAL();	
	
	PSOCK_CLOSE(&sbg->p);
	PSOCK_END(&sbg->p);
}

static PT_THREAD(daytime_thread(struct sambungan_state *sbg))
{	
	PSOCK_BEGIN(&sbg->p);
	//PSOCK_READTO(&sbg->p, ISO_nl);
	PSOCK_READBUF(&sbg->p);
	
	portENTER_CRITICAL();	
	status[sbg->nomer_samb].reply++;
	memcpy(daytime, (char *) &sbg->in_buf, 24);
	portEXIT_CRITICAL();	
	
	PSOCK_CLOSE(&sbg->p);
	PSOCK_END(&sbg->p);
}

void samb_appcall (void)
{
	struct sambungan_state *sb = (struct sambungan_state *) &(uip_conn->appstate2);
	
	if(uip_closed()) 
	{
  		#ifdef DEBUG
		printf("sambungan closed : %d.%d.%d.%d\n", \
			htons(uip_conn->ripaddr[0]) >> 8, htons(uip_conn->ripaddr[0]) & 0xFF, \
			htons(uip_conn->ripaddr[1]) >> 8, htons(uip_conn->ripaddr[1]) & 0xFF );
  		#endif
		
		sb->state = 0;
		status[sb->nomer_samb].stat = 0;
		return;
	}
	if(uip_aborted() || uip_timedout()) 
	{
  		#ifdef DEBUG
  		printf(" Sumber %d :%d.%d.%d.%d aborted / timeout\n", (sb->nomer_samb+1), \
			htons(uip_conn->ripaddr[0]) >> 8, htons(uip_conn->ripaddr[0]) & 0xFF, \
			htons(uip_conn->ripaddr[1]) >> 8, htons(uip_conn->ripaddr[1]) & 0xFF );
		#endif
		
		sb->state = 0;
		status[sb->nomer_samb].stat = 0;
		status[sb->nomer_samb].reply = 0;
		return;
	}
	
	if (uip_poll()) 
	{
		printf(" POOL %d.%d.%d.%d\n", \
			htons(uip_conn->ripaddr[0]) >> 8, htons(uip_conn->ripaddr[0]) & 0xFF, \
			htons(uip_conn->ripaddr[1]) >> 8, htons(uip_conn->ripaddr[1]) & 0xFF );
			
		sb->timer++;
		if (sb->timer > 10)
		{
			sb->timer = 0;	
			sb->state = 0;
			status[sb->nomer_samb].stat = 0;
			status[sb->nomer_samb].reply = 0;
		
			printf(" POOL timeout");
			uip_abort();
			return;
		}
	}
	//if (uip_acked()) printf("acked");
	if (uip_conn->rport == HTONS(PORT_MONITA))
		samb_thread(sb);
	else if (uip_conn->rport == HTONS(PORT_DAYTIME))
	{
		daytime_thread(sb);
	}
}
#endif
