/*
 * monita_uip.c
 *
 *  Created on: Dec 10, 2008
 *      Author: jadid
 *
 *      server monita dengan protocol UIP
 *
 */


#include "monita_uip.h"
#include "uip.h"
#include <stdio.h>
#include <stdlib.h>

#include "../GPIO/gpio.h"
extern struct t2_konter konter;

#define 	PORT_MONITA 5001


struct t_xdata xdata  __attribute__ ((section (".eth_test")));
struct t_data_float data_float  __attribute__ ((section (".eth_test")));

extern struct t_data_float s_data[JML_SUMBER];

unsigned int loop_kirim;

void monita_init(void)
{
	loop_kirim = 0;
	uip_listen(HTONS(PORT_MONITA));
}

static PT_THREAD(handle_input(struct monita_state *s))
{
	float temp_rpm;
	int i;
	int t;

	PSOCK_BEGIN(&s->p);
	PSOCK_READBUF(&s->p);

	if (strncmp(s->in_buf, "sampurasun", 10) == 0)
	{
		loop_kirim++;
		//printf("dari monita \n");
		
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

		// debug
		/*
		printf("Req monita ke %d\n", loop_kirim);
		for (i=0; i<10;)
		{
			printf("  hit = %4.2f ,", data_float.data[i]);
			printf("rpm = %4.2f\n", data_float.data[i+1]);
			i++;
			i++;
		}*/

		xdata.nomer = loop_kirim;
		//xdata.flag = 30;		//pulsa
		xdata.flag = 10;
		xdata.alamat = 1;		// stacking board nomer 1
		strcpy(xdata.mon, "monita1");
		memcpy(xdata.buf, (char *) &data_float, sizeof (data_float));

		PSOCK_SEND(&s->p, (char *) &xdata, sizeof (xdata));
	}
	else
	{
		printf("unknown request !\n");
	}
	//printf("didapat %s", s->in_buf);
	//PSOCK_SEND_STR(&s->p, "hello \n");
	PSOCK_CLOSE(&s->p);
	PSOCK_END(&s->p);
}

static void handle_connection(struct monita_state *s)
{
	//printf("dalam handle\n");
	handle_input(s);
	//printf("sesudah handle\n");
}

void monita_appcall (void)
{
	struct monita_state *s = (struct monita_state *) &(uip_conn->appstate2);

	//printf("monita call !\n");

	if (uip_connected())
	{
		//printf("KONEKTED !\n");
		PSOCK_INIT(&s->p, s->in_buf, sizeof (s->in_buf));
	}

	//printf("before handle\n");
	handle_connection(s);
}

#ifdef BOARD_TAMPILAN
////*************************************** ethernet tampilan **************************************/

#define JML_MODUL	20

extern struct t_sumber sumber[];
extern struct t_titik titik[];

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

/* sambungan konek dipanggil periodik
 * sesuai dengan server / modul yang akan
 * diminta datanya, misalnya dipanggil dari 
 * task led
 */

void sambungan_connect(void)
{
	struct uip_conn *conn;
	struct sambungan_state *samb3;
	uip_ipaddr_t ip_modul;
	
	int i;
	
	for (i=0; i<JML_MODUL; i++)
	{
		if (sumber[i].status == 1 && status[i].stat == 0)	// harus diaktfikan lagi
		{
			printf("Init sumber %d : %10s : ", (i+1), sumber[i].nama);
			printf("%d.%d.%d.%d\r\n", sumber[i].IP0, sumber[i].IP1, sumber[i].IP2, sumber[i].IP3);  
			
			uip_ipaddr(ip_modul, sumber[i].IP0, sumber[i].IP1, sumber[i].IP2, sumber[i].IP3);	
			
			conn = uip_connect(ip_modul, HTONS(PORT_MONITA));
			if (conn == NULL)
			{
				printf("ERR: Koneksi Penuh\r\n");	
				return ;
			}
			//printf("..%X..L=%d, R=%d .. OK\r\n", conn, conn->lport, conn->rport);
	
			status[i].stat = 1;
			samb3 = (struct sambungan_state *) &conn->appstate2;
			samb3->nomer_samb = i;
			samb3->state = 1;
			
			//PSOCK_INIT(PP, (char *) &samb3->in_buf, 20);
			PSOCK_INIT((struct psock *) &samb3->p, (char *) &samb3->in_buf, sizeof (struct t_xdata));
			//PSOCK_INIT(&samb2[jum_kon].p, (char *) &samb2[jum_kon].in_buf, sizeof(samb2[jum_kon].in_buf));
			
			//samb.state = 1;
			//PSOCK_INIT(&samb.p, (char *) &samb.in_buf, sizeof (struct t_xdata));
		}
	}
}

//static PT_THREAD(samb_thread(void))
static PT_THREAD(samb_thread(struct sambungan_state *sbg))
{
#ifdef LAMA	
	//printf("sambungan thread\n");	
	PSOCK_BEGIN(&samb.p);
	
	//printf("  sampur ..");
	PSOCK_SEND_STR(&samb.p, "sampurasun\n");
	//printf(".. rasun\n");
	
	PSOCK_READBUF(&samb.p);
	//printf("hasil %s\n", samb.in_buf.mon);
	
	
	/*
	if (strncmp(samb.in_buf, "monita1", 7) == 0)
	{
		printf("  data dari server !\n");	
	}*/
	
	PSOCK_CLOSE(&samb.p);
	PSOCK_END(&samb.p);	
#endif
	int i;
	
	PSOCK_BEGIN(&sbg->p);
	PSOCK_SEND_STR(&sbg->p, "sampurasun\n");
	
	PSOCK_READBUF(&sbg->p);
	
	portENTER_CRITICAL();	
	if (strncmp(sbg->in_buf.mon, "monita1", 7) == 0)
	{
		status[sbg->nomer_samb].reply++;
		//memcpy((char *) &data_float, sbg->in_buf.buf, sizeof (data_float));
		memcpy((char *) &s_data[sbg->nomer_samb], sbg->in_buf.buf, sizeof (data_float));
		
		//cari titik pemilik data ini 
		/*
		for (i=0; i<(TIAP_MESIN * JML_MESIN); i++)
		{
			if (titik[i].ID_sumber == sbg->nomer_samb)
			{
				titik[i].data = data_float.data[titik[i].kanal];	
			}	
		}*/		
		
		//printf("  data dari server !\n");	
	}
	portEXIT_CRITICAL();
	
	PSOCK_CLOSE(&sbg->p);
	PSOCK_END(&sbg->p);

}

void samb_appcall (void)
{
	struct sambungan_state *sb = (struct sambungan_state *) &(uip_conn->appstate2);
	
	//printf("sambungan called\n");
	if(uip_closed()) 
	{
  		//printf("sambungan closed\n");
  		sb->state = 0;
		status[sb->nomer_samb].stat = 0;
		return;
	}
	if(uip_aborted() || uip_timedout()) 
	{
  		printf("Sumber %d : L=%d aborted / timeout\n", (sb->nomer_samb+1), uip_conn->lport);
  		sb->state = 0;
		status[sb->nomer_samb].stat = 0;
		status[sb->nomer_samb].reply = 0;
		return;
	}
	samb_thread(sb);
}
#endif
