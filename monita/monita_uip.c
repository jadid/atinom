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
//static uip_ipaddr_t tujuan;

#define JML_MODUL	10
static uip_ipaddr_t ip_modul[JML_MODUL] __attribute__ ((section (".usb_text")));
extern struct t_sumber sumber[];

//struct uip_conn *conn_modul[JML_MODUL] __attribute__ ((section (".eth_test")));


//static 
struct sambungan_state samb __attribute__ ((section (".eth_test")));

struct sambungan_state samb2[20] __attribute__ ((section (".usb_text")));

extern char titik_siap;

int jum_kon;

void sambungan_init(void)
{
	int i;
	//set tujuan
	samb.state = 0;
	//uip_ipaddr(&tujuan, 192,168,1,53);
	//uip_connect(&tujuan, HTONS(5002));
	
	for (i=0; i<JML_MODUL; i++)
	{
		//uip_ipaddr(&ip_modul[i], sumber[i].IP0, sumber[i].IP1, sumber[i].IP2, sumber[i].IP3);	
		uip_ipaddr(&ip_modul[i], sumber[i].IP0, sumber[i].IP1, sumber[i].IP2, i+2);	
		samb2[i].state = 0;
		samb2[i].nomer_samb = i;
	}
	
	printf("sambungan ethernet init\n");
	titik_siap = 2;
	jum_kon = 0;		
}

/* sambungan konek dipanggil periodik
 * sesuai dengan server / modul yang akan
 * diminta datanya, misalnya dipanggil dari 
 * task led
 */

void sambungan_connect(void)
{
	/*
	struct uip_conn *conn;
	
	samb.state = 0;
	uip_ipaddr(&tujuan, 192,168,1,53);
	
	printf("mulai konek ..%d..", HTONS(5002));
	conn = uip_connect(&tujuan, HTONS(5002));
	if (conn == NULL)
	{
		printf("ERR: Koneksi Penuh\r\n");	
		return ;
	}
	printf("..%X..\r\n", conn);
	
	samb.state = 1;
	//printf("setelah konek\n");
	
	PSOCK_INIT(&samb.p, (char *) &samb.in_buf, sizeof(samb.in_buf));
	*/
	struct uip_conn *conn;
	struct sambungan_state *samb3;
	
	int i;
	
	for (i=0; i<10; i++)
	{
		if (sumber[i].status == 1 && samb2[i].state == 0)	// harus diaktifkan
		{
			jum_kon = i;
			printf("Init sumber %d : %10s : ", (i+1), sumber[i].nama);
			printf("%d.%d.%d.%d\r\n", sumber[i].IP0, sumber[i].IP1, sumber[i].IP2, sumber[i].IP3);  
			
			uip_ipaddr(&ip_modul[i], sumber[i].IP0, sumber[i].IP1, sumber[i].IP2, sumber[i].IP2);	
			
			conn = uip_connect(&ip_modul[jum_kon], HTONS(5002));
			if (conn == NULL)
			{
				printf("ERR: Koneksi Penuh\r\n");	
				return ;
			}
			printf("..%X..L=%d, R=%d .. OK\r\n", conn, conn->lport, conn->rport);
	
			samb2[jum_kon].state = 1;
			//sumber[i].status = 9;		// sedang mencoba konek
			//(uip_sambungan_state *) &conn->appstate2 =0;
			//conn->appstate2->nomer_samb = i;
			samb3 = (struct sambungan_state *) &conn->appstate2;
			samb3->nomer_samb = i;
			
			PSOCK_INIT(&samb2[jum_kon].p, (char *) &samb2[jum_kon].in_buf, sizeof(samb2[jum_kon].in_buf));
		}
		
		//printf("mulai konek ..%d..", HTONS(5002));
		
		//jum_kon++;
	}
	
	/*
	struct uip_conn *conn;
	int i;
	printf("konek !\r\n");
	
	i = 1;
	//for (i=0; i<5; i++)
	{
		samb.state = 0;
		uip_ipaddr(&ip_modul[i], 192,168,1,200+i);
		conn = uip_connect(&ip_modul[i], HTONS(5002));
		
		if (conn == NULL)
		{
			printf("connection %d sudah NULL ..", i);	
			return ;
		}
		printf("%d : %X..\r\n", i, conn);
		samb.state = 1;
		PSOCK_INIT(&samb.p, (char *) &samb.in_buf, sizeof(samb.in_buf));
		//PSOCK_INIT(&samb2[i].p, (char *) &samb2[i].in_buf, sizeof(samb2[i].in_buf));	
	}
	*/
	
}

static PT_THREAD(samb_thread(void))
{
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
	
}

void samb_appcall (void)
{
	struct sambungan_state *sb = (struct sambungan_state *) &(uip_conn->appstate2);
	
	printf("sambungan called\n");
	if(uip_closed()) 
	{
  		//printf("sambungan closed\n");
		//samb.state = 0;
  		sb->state = 0;
		return;
	}
	if(uip_aborted() || uip_timedout()) 
	{
  		printf("sambungan %d : L=%d aborted / timeout\n", sb->nomer_samb, uip_conn->lport);
		//samb.state = 0;
  		sb->state = 0;
		return;
	}
	samb_thread();
}
#endif
