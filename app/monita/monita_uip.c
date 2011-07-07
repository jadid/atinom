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
 * 23 februari 2009
 * diberikan paksa close jika data diterima kurang dari jumlah seharusnya
 * 
 */


#include "FreeRTOS.h"
#include "monita_uip.h"
#include "uip.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef PAKAI_ETH

struct t_xdata 			xdata  		;/*__attribute__ ((section (".eth_test")));*/
struct t_data_float 	data_float  ;/*__attribute__ ((section (".eth_test")));*/

#ifdef PAKAI_MODBUSTCP
	
#endif

//extern struct t_data_float s_data[JML_SUMBER];

unsigned int loop_kirim;

#ifdef BOARD_KOMON_A_RTD
#include "../adc/ad7708.h"
extern struct t_adc st_adc;
#define BOARD_KOMON
#endif

#ifdef BOARD_KOMON_420_SAJA
#include "../adc/ad7708.h"
#include "../tinysh/enviro.h"
extern struct t_adc st_adc;
#define BOARD_KOMON
#endif

#ifdef BOARD_KOMON_420_SABANG
//#define BOARD_KOMON
#endif

#ifdef BOARD_KOMON_B_THERMO
#include "../adc/ad7708.h"
extern struct t_adc st_adc;
#define BOARD_KOMON
#endif

#ifdef BOARD_KOMON_KONTER
#include "../tinysh/enviro.h"
//extern struct t_env env2;
#endif

#ifdef PAKAI_MMC
	//int status_MMC=0;
#endif

/*
#ifdef PAKAI_MODBUSTCP
	void modbustcp_init() {
		printf(" Monita : modbus TCP init !\r\n");
		uip_listen(HTONS(PORT_MODBUSTCP));
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
		
		//portENTER_CRITICAL();
		//memcpy(xdata.buf, (char *) &data_float, sizeof (data_float));
		//portEXIT_CRITICAL();
		//send data ke network
		uip_send((char *) &regmod, 9+regmod.nReg);
	}
#endif
//*/

#ifdef BOARD_KOMON

void monita_init(void)
{
	loop_kirim = 0;
	
	//printf("Monita : sampurasun client init !\r\n");
	uip_listen(HTONS(PORT_MONITA));
}

void monita_appcall(void)
{
	int len;
	unsigned char str_monita[32];
	float temp_rpm;
	int i;
	int t;
	char stack;
	unsigned char ipne[32];
	
#if 0
			sprintf(ipne, " :%d.%d.%d.%d", \
			htons(uip_conn->ripaddr[0]) >> 8, htons(uip_conn->ripaddr[0]) & 0xFF, \
			htons(uip_conn->ripaddr[1]) >> 8, htons(uip_conn->ripaddr[1]) & 0xFF );
			
	loop_kirim++;
	printf("\n * %d:", loop_kirim);
	if (uip_connected())	{
		printf("konek %s\n", ipne);
	}
	if (uip_closed())	{
		printf("close %s\n", ipne);
	}
	if (uip_aborted())	{
		printf("abort %s\n", ipne);
	}
	if (uip_timedout())	{
		printf("timeout %s\n", ipne);
	}	
	if(uip_acked()) 	{
		printf("ack %s\n", ipne);
	}
	if (uip_poll())	{
		printf("poll %s\n", ipne);	
	}
#endif
	
	
	//printf("_______________________________ada yg manggil = \r\n\n");
	if (uip_newdata())	{
		len = uip_datalen();
		//printf("newdata = %d %s\n", len, ipne);
		
		if (len >= 10)		{
			portENTER_CRITICAL();
			memcpy(str_monita, (char *) uip_appdata, 11);
			portEXIT_CRITICAL();
			if (strncmp(str_monita, "sampurasun", 10) == 0)
			{
				loop_kirim++;
				stack = str_monita[10]-'0';

				#ifdef BOARD_KOMON_A_RTD
				hitung_data_float();
				
				for (i=0; i<20;i++)			{
					data_float.data[i] = st_adc.flt_data[i];
				}		
				#endif
				
				#ifdef BOARD_KOMON_420_SAJA
					for (i=0; i<PER_SUMBER;i++)		{
						data_float.data[i] = data_f[i];
						//printf("data_f[%d]: %.2f\r\n", i+1, data_f[i]);
					}		
				#endif
				
				#if defined(BOARD_KOMON_420_SABANG) || defined(BOARD_KOMON_420_SABANG_2_3)
					#ifdef BANYAK_SUMBER
						for (i=0; i<PER_SUMBER;i++)		{
							data_float.data[i] = data_f[(stack-1)*PER_SUMBER+i];
							//printf("data_f[%d]: %.2f\r\n", (stack-1)*PER_SUMBER+i+1, data_f[(stack-1)*PER_SUMBER+i]);
						}
					#else
						for (i=0; i<PER_SUMBER;i++)		{
							data_float.data[i] = data_f[i];
							//printf("data_f[%d]: %.2f\r\n", i+1, data_f[i]);
						}
					#endif
				#endif
				
				#ifdef BOARD_KOMON_B_THERMO
				hitung_data_float();
				
				for (i=0; i<20;i++)
				{
					data_float.data[i] = st_adc.flt_data[i];
				}			
				#endif
				
				#ifdef BOARD_KOMON_KONTER
					for (i=0; i<PER_SUMBER;i++)		{
						data_float.data[i] = data_f[i];
						//printf("data_f[%d]: %.2f\r\n", i+1, data_f[i]);
					}
				/*
				extern unsigned int data_putaran[];
				extern unsigned int data_hit[];				
				
				struct t_env *env2;
				env2 = (char *) ALMT_ENV;
				
				t=0;
				for (i=0; i<10;i++)
				{
					//data_float.data[t] = data_hit[i];
					data_float.data[t] = (unsigned int) (data_hit[i] * env2->kalib[i].m);
					t++;
					
					if (data_putaran[i])
					{
						// cari frekuensi
						temp_rpm = (float) 1000000000.00 / data_putaran[i]; // beda msh dlm nS
						// rpm
						data_float.data[t] = temp_rpm * 60;
					}
					else
						data_float.data[t] = 0;
					
					t++;
				}
				#if (KONTER_MALINGPING == 1)
				// data kanal 1 adalah adc1 (adc0 internal) //
				extern float volt_supply;
				
				data_float.data[0] = (float) ((volt_supply * env2.kalib[0].m) + env2.kalib[0].C);
	
				#endif
				//*/
				#endif
				
				xdata.nomer = loop_kirim;
				//xdata.flag = 30;		//pulsa
				xdata.flag = (unsigned char) PER_SUMBER;
				xdata.alamat = stack;		// stacking board nomer 1
				strcpy(xdata.mon, "monita1");
				
				portENTER_CRITICAL();
				memcpy(xdata.buf, (char *) &data_float, sizeof (data_float));
				portEXIT_CRITICAL();
				
				//send data ke network
				uip_send((char *) &xdata, sizeof (xdata));
			}
			//printf("\n");	
		}
		else 
		{
			/*
			sprintf(ipne, " :%d.%d.%d.%d", \
			htons(uip_conn->ripaddr[0]) >> 8, htons(uip_conn->ripaddr[0]) & 0xFF, \
			htons(uip_conn->ripaddr[1]) >> 8, htons(uip_conn->ripaddr[1]) & 0xFF );
			
			printf("Unknown request dari %s\n", ipne);
			//*/
			uip_close();	
		}	
	}
	
	if (uip_poll())
	{
		/*
		sprintf(ipne, " :%d.%d.%d.%d", \
			htons(uip_conn->ripaddr[0]) >> 8, htons(uip_conn->ripaddr[0]) & 0xFF, \
			htons(uip_conn->ripaddr[1]) >> 8, htons(uip_conn->ripaddr[1]) & 0xFF );
			
		printf("Invalid pool %s\n", ipne);
		//*/
		uip_close();	
	}
}
#endif

//#ifdef BOARD_TAMPILAN
//#ifdef CARI_SUMBERNYA		// pengganti BOARD_TAMPILAN
#ifdef SAMPURASUN_SERVER
////*************************************** ethernet tampilan **************************************/

//extern struct t_sumber sumber[];
//extern struct t_titik titik[];
//extern float data_f [ (JML_SUMBER * PER_SUMBER) ];
extern unsigned char daytime[32];

//struct t_status status[JML_MODUL] __attribute__ ((section (".usb_text")));
struct t_status 	status[ JML_SUMBER ]; 	/* __attribute__ ((section (".eth_test"))); */
struct t_xdata 		in_buf;				/* __attribute__ ((section (".eth_test"))); */

int wer = 0;

void sambungan_init(void)	{
	int i;
	
	for (i=0; i < JML_SUMBER ; i++)	{
		status[i].stat = 0;
		status[i].reply = 0;
	}	
	//printf(" Monita : sampurasun server init !!\n");	
}

/* 
 * sambungan konek dipanggil periodik oleh uip_task.c
 * sesuai dengan server / modul yang akan
 * diminta datanya.
 * 
 * dipanggil setiap 50 ms (memutar), sehingga jika ada 20
 * client, setiap client akan dipanggil 1000 ms ( 1 detik ).
 * 
 * no : mulai dari nol sampai (JML_SUMBER - 1)
 */

void sambungan_connect(int no)
{
	struct uip_conn *conn;
	struct sambungan_state *samb3;
	uip_ipaddr_t ip_modul;
	int i;
	char ipne[32];
	//printf("masuk menuju sambungan konek %d\r\n",no);
	

	struct t_sumber *sumber;
	sumber = (char *) ALMT_SUMBER;
	/*
	wer++;
	if (wer>100) {
		printf("status sumber %d: %d, status.stat: %d\r\n",no, sumber[no].status, status[no].stat);
		
	}
	if (wer>120) wer = 0;
	//*/
	if (sumber[no].status == 1 && sumber[no].alamat == 0 && status[no].stat == 0)	// harus diaktfikan lagi
	{
		#ifdef DEBUG
		printf("Init sumber %d : %10s : ", (no+1), sumber[no].nama);
		printf("%d.%d.%d.%d\r\n", sumber[no].IP0, sumber[no].IP1, sumber[no].IP2, sumber[no].IP3);
		#endif
		
		uip_ipaddr(ip_modul, sumber[no].IP0, sumber[no].IP1, sumber[no].IP2, sumber[no].IP3);	
		/*
		if (wer==0) {
			printf("ip modul no %d: %s\r\n",no, ip_modul);
		
		}
		//*/
		
		conn = uip_connect(ip_modul, HTONS(PORT_MONITA));
		if (conn == NULL)	{
			#ifdef BOARD_TAMPIL
			debug_out_h("ERR: Koneksi Penuh");	
			#endif
			//printf("ERR: Koneksi Penuh");
			return ;
		}
		//printf("..%X..L=%d, R=%d .. OK\r\n", conn, conn->lport, conn->rport);
		
		conn->nomer_sambung = no;
		status[no].stat = 1;
		status[no].lport = conn->lport;
		status[no].timer = 0;
		status[no].reply_lama = 0;
		/* rport akan selalu sama */
		//debug_out_h("init %d lport %d, rport %d\n", no+1, status[no].lport, conn->rport);
		sprintf(ipne, " [%d][%d]:%d.%d.%d.%d", no+1, uip_conn->rport, \
			sumber[no].IP0, sumber[no].IP1, sumber[no].IP2, sumber[no].IP3);
		//printf("Konek %s\r\n", ipne);	
		#ifdef BOARD_TAMPIL
		debug_out_h("Konek %s", ipne);	
		#endif
		return;
	}
	else if (sumber[no].status == 1 && sumber[no].alamat == 0 && status[no].stat != 0)		{
		/* 
		 * kemungkinan tidak close, tidak dpt acknoledge dll, 
		 * di increment reply_lama
		 * jika sudah lebih dari 60 detik dipaksa untuk konek lagi
		 */	
		
		status[no].reply_lama++;
		if (status[no].reply_lama > 60)		{
			/* cari dulu koneksi yang lama
			 * dan set menjadi belum konek */
			#ifdef BOARD_TAMPIL
			debug_out_h("forced to connect ");
			#endif
			for(i = 0; i < UIP_CONNS; i++) 		{
		  		if (uip_conns[i].nomer_sambung == no && uip_conn[i].lport == HTONS(PORT_MONITA))		{
					uip_conns[i].tcpstateflags = UIP_CLOSED;
		  			uip_conns[i].nomer_sambung = 0;
					
					//printf("con %d ", i);
					#ifdef BOARD_TAMPIL
					debug_out_h("con %d ", i);
					#endif
				}
	  		}
			
			status[no].stat = 0;
			status[no].timer = 0;
			status[no].reply_lama = 0;
			
			#ifdef BOARD_TAMPIL
			//debug_out_h("rekonek sumber %d :", no+1);
			//debug_out_h("  %d.%d.%d.%d", sumber[no].IP0, sumber[no].IP1, sumber[no].IP2, sumber[no].IP3);	
			#endif
		}
	}
	/* daytime server, harusnya hanya ada 1 server */
	else if (sumber[no].status == 5 && sumber[no].alamat == 0 && status[no].stat == 0)	// harus diaktfikan lagi
	{
		#ifdef DEBUG
		printf("Init daytime %d : %10s : ", (no+1), sumber[no].nama);
		printf("%d.%d.%d.%d\r\n", sumber[no].IP0, sumber[no].IP1, sumber[no].IP2, sumber[no].IP3);
		#endif
		uip_ipaddr(ip_modul, sumber[no].IP0, sumber[no].IP1, sumber[no].IP2, sumber[no].IP3);	
		
		conn = uip_connect(ip_modul, HTONS(PORT_DAYTIME));
		if (conn == NULL)
		{
			//debug_out_h("ERR: Koneksi Penuh");	
			printf("ERR: Koneksi Penuh");	
			return ;
		}
		//printf("..%X..L=%d, R=%d .. OK\r\n", conn, conn->lport, conn->rport);

		conn->nomer_sambung = no;
		status[no].stat = 1;
		status[no].lport = conn->lport;
		status[no].timer = 0;
		status[no].reply_lama = 0;
	
		return;
	}
	return;
}

/* 
 * sambungan_appcall yang akan dipanggil ketika uip 
 * menemukan paket untuk port 5001
 */

void samb_appcall(void)	{
	int len;
	char str_monita[64];
	int i;
	
	struct t_sumber *sumber;
	sumber = (char *) ALMT_SUMBER;
	
	unsigned int nomer_sambung = (unsigned int) uip_conn->nomer_sambung;
	
	if (nomer_sambung > (JML_SUMBER-1))	{
		printf("Nomer sambung invalid !");
		//debug_out_h("Nomer sambung invalid !");
		uip_close();
		return;	
	}
	
	//struct sambungan_state *sb = (struct sambungan_state *) &(uip_conn->appstate2);
	char ipne[32];
	
	//*
	sprintf(ipne, " [%d][%d]:%d.%d.%d.%d", nomer_sambung, uip_conn->lport, \
			htons(uip_conn->ripaddr[0]) >> 8, htons(uip_conn->ripaddr[0]) & 0xFF, \
			htons(uip_conn->ripaddr[1]) >> 8, htons(uip_conn->ripaddr[1]) & 0xFF );
	
	//loop_kirim++;
	//printf("\n * %d:%s", loop_kirim, ipne);
	//*/
	if (uip_connected())	{
		//printf("konek no:%d IP: %s\n",nomer_sambung, ipne);
		if (status[nomer_sambung].stat == 1)	{
			status[nomer_sambung].stat = 2;
			
			sprintf(ipne, " [%d][%d]:%d.%d.%d.%d", nomer_sambung+1, uip_conn->lport, \
			htons(uip_conn->ripaddr[0]) >> 8, htons(uip_conn->ripaddr[0]) & 0xFF, \
			htons(uip_conn->ripaddr[1]) >> 8, htons(uip_conn->ripaddr[1]) & 0xFF );
			
			/* sampurasun + nomor modul (dalam karakter) */
			sprintf(str_monita, "sampurasun%d", sumber[nomer_sambung].stack);	

			uip_send((char *) str_monita, 11);		// 10
		}
		else	{
			sprintf(ipne, " [%d][%d]:%d.%d.%d.%d", nomer_sambung+1, uip_conn->lport, \
			htons(uip_conn->ripaddr[0]) >> 8, htons(uip_conn->ripaddr[0]) & 0xFF, \
			htons(uip_conn->ripaddr[1]) >> 8, htons(uip_conn->ripaddr[1]) & 0xFF );
			
			//debug_out_h("force to close %s", ipne);
			//printf("force to close %s", ipne);
			uip_close();
			return;
		}
	}
	if (uip_closed())	{
		status[nomer_sambung].stat = 0;
		//printf("close %s\n", ipne);
	}
	if (uip_aborted())	{
		status[nomer_sambung].stat = 0;
		//printf("abort %s\n", ipne);
	}
	if (uip_timedout())	{
		status[nomer_sambung].stat = 0;
		//printf("timeout %s\n", ipne);
	}	
	if(uip_acked()) 	{
		//printf("ack %s\n", ipne);
		if (status[nomer_sambung].stat == 2)		{
			status[nomer_sambung].stat = 3;
			//printf("sampur ack\n");
		}
		else		{
			sprintf(ipne, " [%d][%d]:%d.%d.%d.%d", nomer_sambung+1, uip_conn->lport, \
			htons(uip_conn->ripaddr[0]) >> 8, htons(uip_conn->ripaddr[0]) & 0xFF, \
			htons(uip_conn->ripaddr[1]) >> 8, htons(uip_conn->ripaddr[1]) & 0xFF );
			
			//debug_out_h("invalid acknoledge %s", ipne);
			//printf("invalid acknoledge %s", ipne);
			uip_close();	
		}
	}
	if (uip_poll())		{
		//printf("poll %s\n", ipne);
		status[nomer_sambung].timer++;
		if (status[nomer_sambung].timer > 5)		{
			sprintf(ipne, " [%d][%d]:%d.%d.%d.%d", nomer_sambung+1, uip_conn->lport, \
			htons(uip_conn->ripaddr[0]) >> 8, htons(uip_conn->ripaddr[0]) & 0xFF, \
			htons(uip_conn->ripaddr[1]) >> 8, htons(uip_conn->ripaddr[1]) & 0xFF );
			
			//debug_out_h("pool timeout %s", ipne);
			//printf("pool timeout %s", ipne);
			uip_close();		
		}			
	}
	if (uip_newdata())	{
		len = uip_datalen();
		//printf("newdata = %d %s, stat: %d\n", len, ipne, status[nomer_sambung].stat);
		
		//if (status[nomer_sambung].stat == 3 && len == 248)
		if (status[nomer_sambung].stat == 3)	{
			portENTER_CRITICAL();
			memcpy((char *) &in_buf, uip_appdata, len);
			portEXIT_CRITICAL();
			
			if (strncmp(in_buf.mon, "monita", 6) == 0)
			{
				//printf("Data MOMON\n");	
				status[nomer_sambung].reply++;
				//memcpy((char *) &s_data[nomer_sambung].data, in_buf.buf, sizeof (data_float));
				//memcpy((char *) &data_f[nomer_sambung*PER_SUMBER].data, in_buf.buf, sizeof (data_float));
				//memcpy((char *) &data_f[nomer_sambung*PER_SUMBER], in_buf.buf, sizeof (data_float));
				portENTER_CRITICAL();
				memcpy( (char *) &data_f[nomer_sambung*PER_SUMBER], in_buf.buf, (PER_SUMBER*sizeof (float)) );
				portEXIT_CRITICAL();
				#ifdef BOARD_KOMON_420_SABANG
					
					
					//portENTER_CRITICAL();
					//memcpy( (char *) &datanya[0], in_buf.buf, (PER_SUMBER*sizeof (float)) );
					//portEXIT_CRITICAL();
				#endif
				
				//debug_out_h("-->[%d], mod %d", (nomer_sambung + 1), in_buf.alamat);
				//printf("-->[%d], mod %d", (nomer_sambung + 1), in_buf.alamat);
				
				#ifdef DEBUG_DATA
				printf(" %d:", sbg->nomer_samb);
				for (i=0; i<10; i++)	{
					printf("%3.3f ", s_data[sbg->nomer_samb].data[i]);	
				}
				printf("\n");
				#endif
				
			}
			
			uip_close();
			return;
		}
		else if (len > 0)
		{
			sprintf(ipne, " [%d][%d]:%d.%d.%d.%d", nomer_sambung+1, uip_conn->lport, \
			htons(uip_conn->ripaddr[0]) >> 8, htons(uip_conn->ripaddr[0]) & 0xFF, \
			htons(uip_conn->ripaddr[1]) >> 8, htons(uip_conn->ripaddr[1]) & 0xFF );
	
			//debug_out_h("Invalid uip_newdata() %s, leng = %d", ipne, len);
			printf("Invalid uip_newdata() %s, leng = %d", ipne, len);
			uip_close();
			return;	
		}
	}
}

void daytime_appcall(void)	{
	int len;
	unsigned int nomer_sambung = (unsigned int) uip_conn->nomer_sambung;
	
	if (nomer_sambung > (JML_SUMBER-1))	{
		printf("Nomer sambung invalid !");
		uip_close();
		return;	
	}
	
	if (uip_connected())	{
		if (status[nomer_sambung].stat == 1)		{
			status[nomer_sambung].stat = 2;
		}
		else	{
			printf("force to close daytime\n");
			uip_close();
			return;
		}
	}
	if (uip_closed())	{
		status[nomer_sambung].stat = 0;
	}
	if (uip_aborted())	{
		status[nomer_sambung].stat = 0;
	}
	if (uip_timedout())	{
		status[nomer_sambung].stat = 0;
	}
	if (uip_poll())		{
		status[nomer_sambung].timer++;
		if (status[nomer_sambung].timer > 5)		{
			printf("pool timeout daytime\n");
			uip_close();		
		}			
	}
	if (uip_newdata())		{
		len = uip_datalen();
		if (len == 26)
		{
			status[nomer_sambung].reply++;
			portENTER_CRITICAL();	
//			memcpy(daytime, uip_appdata, 24);
			portEXIT_CRITICAL();			
			uip_close();	
		}
	}
}

#endif // TAMPILAN

//#ifdef BOARD_TAMPILAN
#ifdef PAKAI_KIRIM_BALIK		// pengganti BOARD_TAMPILAN
////*************************************** ethernet tampilan **************************************/

//extern struct t_sumber sumber[];
//extern struct t_titik titik[];
//extern float data_f [ (JML_SUMBER * PER_SUMBER) ];
extern unsigned char daytime[32];

//struct t_status status[JML_MODUL] __attribute__ ((section (".usb_text")));
struct t_status 	status[ JML_SUMBER ]; 	/* __attribute__ ((section (".eth_test"))); */
struct t_xdata 		in_buf;				/* __attribute__ ((section (".eth_test"))); */

//int wer = 0;



void kirim_balik_init(void)
{
	int i;
	
	for (i=0; i < JML_SUMBER ; i++)	{
		status[i].stat = 0;
		status[i].reply = 0;
	}	
	printf("kirim_balik_init\n");	
}

/* 
 * sambungan konek dipanggil periodik oleh uip_task.c
 * sesuai dengan server / modul yang akan
 * diminta datanya.
 * 
 * dipanggil setiap 50 ms (memutar), sehingga jika ada 20
 * client, setiap client akan dipanggil 1000 ms ( 1 detik ).
 * 
 * no : mulai dari nol sampai (JML_SUMBER - 1)
 */
int target_kirim;
int sumber_datanya;

void kirim_balik_connect(int no)		// sumber_datanya, target_kirim
{
	struct uip_conn *conn;
	struct sambungan_state *samb3;
	uip_ipaddr_t ip_modul;
	int i;
	char ipne[32];
	//printf("masuk menuju sambungan konek %d\r\n",no);
	int wer;

	struct t_sumber *sumber;
	sumber = (char *) ALMT_SUMBER;
	/*
	wer++;
	if (wer>100) {
		printf("status sumber %d: %d, status.stat: %d\r\n",no, sumber[no].status, status[no].stat);
		
	}
	if (wer>120) wer = 0;
	//*/
	if (sumber[no].status == 1 && status[no].stat == 0)	// harus diaktfikan lagi
	{
		#ifdef DEBUG
		printf("Init sumber %d : %10s : ", (no+1), sumber[no].nama);
		printf("%d.%d.%d.%d\r\n", sumber[no].IP0, sumber[no].IP1, sumber[no].IP2, sumber[no].IP3);
		#endif
		
		uip_ipaddr(ip_modul, sumber[no].IP0, sumber[no].IP1, sumber[no].IP2, sumber[no].IP3);	
		/*
		if (wer==0) {
			printf("ip modul no %d: %s\r\n",no, ip_modul);
		
		}
		//*/
		conn = uip_connect(ip_modul, HTONS(PORT_KIRIM_BALIK));
		if (conn == NULL)
		{
			debug_out_h("ERR: Koneksi Penuh");	
			return ;
		}
		//printf("..%X..L=%d, R=%d .. OK\r\n", conn, conn->lport, conn->rport);
		
		conn->nomer_sambung = no;
		status[no].stat = 1;
		status[no].lport = conn->lport;
		status[no].timer = 0;
		status[no].reply_lama = 0;
		/* rport akan selalu sama */
		//debug_out_h("init %d lport %d, rport %d\n", no+1, status[no].lport, conn->rport);
		sprintf(ipne, " [%d][%d]:%d.%d.%d.%d", no+1, uip_conn->rport, \
			sumber[no].IP0, sumber[no].IP1, sumber[no].IP2, sumber[no].IP3);
		
		debug_out_h("Konek %s", ipne);	
		
		return;
	}
	else if (sumber[no].status == 1 && status[no].stat != 0)	{
		/* 
		 * kemungkinan tidak close, tidak dpt acknoledge dll, 
		 * di increment reply_lama
		 * jika sudah lebih dari 60 detik dipaksa untuk konek lagi
		 */	
		
		status[no].reply_lama++;
		if (status[no].reply_lama > 60)		{
			/* cari dulu koneksi yang lama
			 * dan set menjadi belum konek */
			debug_out_h("forced to connect ");
			for(i = 0; i < UIP_CONNS; i++) 	{
		  		if (uip_conns[i].nomer_sambung == no && uip_conn[i].lport == HTONS(PORT_KIRIM_BALIK))	{
					uip_conns[i].tcpstateflags = UIP_CLOSED;
		  			uip_conns[i].nomer_sambung = 0;
					
					//printf("con %d ", i);
					debug_out_h("con %d ", i);
				}
	  		}
			
			status[no].stat = 0;
			status[no].timer = 0;
			status[no].reply_lama = 0;
			
			debug_out_h("rekonek sumber %d :", no+1);
			debug_out_h("  %d.%d.%d.%d", sumber[no].IP0, sumber[no].IP1, sumber[no].IP2, sumber[no].IP3);	
		}
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
		if (conn == NULL)	{
			debug_out_h("ERR: Koneksi Penuh");	
			return ;
		}
		//printf("..%X..L=%d, R=%d .. OK\r\n", conn, conn->lport, conn->rport);

		conn->nomer_sambung = no;
		status[no].stat = 1;
		status[no].lport = conn->lport;
		status[no].timer = 0;
		status[no].reply_lama = 0;
	
		return;
	}
	return;
}

/* 
 * kirim_balik_appcall yang akan dipanggil ketika uip 
 * menemukan paket untuk port 6543
 */

void kirim_balik_appcall(void) {
	int len;
	char str_monita[64];
	int i;
	
	struct t_sumber *sumber;
	sumber = (char *) ALMT_SUMBER;
	
	unsigned int nomer_sambung = (unsigned int) uip_conn->nomer_sambung;
	//printf("no sambung KIRIM_BALIK: %d\r\n", nomer_sambung);
	if (nomer_sambung > (JML_SUMBER-1))	{
		//printf("Nomer sambung invalid !");
		debug_out_h("Nomer sambung invalid !");
		uip_close();
		return;	
	}
	
	//struct sambungan_state *sb = (struct sambungan_state *) &(uip_conn->appstate2);
	char ipne[32];
	
	//*
	sprintf(ipne, " [%d][%d]:%d.%d.%d.%d", nomer_sambung, uip_conn->lport, \
			htons(uip_conn->ripaddr[0]) >> 8, htons(uip_conn->ripaddr[0]) & 0xFF, \
			htons(uip_conn->ripaddr[1]) >> 8, htons(uip_conn->ripaddr[1]) & 0xFF );
	
	//loop_kirim++;
	//printf("\n KIRIM BALIK * %d:%s", loop_kirim, ipne);
	//*/
	if (uip_connected())	{
		//printf("konek no:%d IP: %s\n",nomer_sambung, ipne);
		if (status[nomer_sambung].stat == 1)	{
			status[nomer_sambung].stat = 2;
			
			sprintf(ipne, " [%d][%d]:%d.%d.%d.%d", nomer_sambung+1, uip_conn->lport, \
			htons(uip_conn->ripaddr[0]) >> 8, htons(uip_conn->ripaddr[0]) & 0xFF, \
			htons(uip_conn->ripaddr[1]) >> 8, htons(uip_conn->ripaddr[1]) & 0xFF );
			
			/* sampurasun + nomor modul (dalam karakter) */
			//debug_out_h("%s->%s", buf, ipne);
			
			xdata.nomer = loop_kirim;
			//xdata.flag = 30;		//pulsa
			xdata.flag = 10;
			xdata.alamat = 1;		// stacking board nomer 1
			strcpy(xdata.mon, "balikm1");
			
			for (i=0; i<20;i++)	{
				//memcpy( (char *) &data_f[nomer_sambung*PER_SUMBER], in_buf.buf, (PER_SUMBER*sizeof (float)) );
				data_float.data[i] = data_f[sumber_datanya*PER_SUMBER+i];
			}
			
			
			portENTER_CRITICAL();
				memcpy(xdata.buf, (char *) &data_float, sizeof (data_float));
			portEXIT_CRITICAL();
			
			uip_send((char *) &xdata, sizeof (xdata));
			//uip_send((char *) buf, 11);		// 10
		}
		else	{
			sprintf(ipne, " [%d][%d]:%d.%d.%d.%d", nomer_sambung+1, uip_conn->lport, \
			htons(uip_conn->ripaddr[0]) >> 8, htons(uip_conn->ripaddr[0]) & 0xFF, \
			htons(uip_conn->ripaddr[1]) >> 8, htons(uip_conn->ripaddr[1]) & 0xFF );
			
			debug_out_h("force to close %s", ipne);
			uip_close();
			return;
		}
	}
	if (uip_closed())	{
		status[nomer_sambung].stat = 0;
		//printf("close %s\n", ipne);
	}
	if (uip_aborted())	{
		status[nomer_sambung].stat = 0;
		//printf("abort %s\n", ipne);
	}
	if (uip_timedout())	{
		status[nomer_sambung].stat = 0;
		//printf("timeout %s\n", ipne);
	}	
	if(uip_acked()) 	{
		//printf("ack %s\n", ipne);
		if (status[nomer_sambung].stat == 2)	{
			status[nomer_sambung].stat = 3;
			//printf("sampur ack\n");
		}
		else	{
			sprintf(ipne, " [%d][%d]:%d.%d.%d.%d", nomer_sambung+1, uip_conn->lport, \
			htons(uip_conn->ripaddr[0]) >> 8, htons(uip_conn->ripaddr[0]) & 0xFF, \
			htons(uip_conn->ripaddr[1]) >> 8, htons(uip_conn->ripaddr[1]) & 0xFF );
			
			debug_out_h("invalid acknoledge %s", ipne);
			uip_close();	
		}
	}
	if (uip_poll())	{
		//printf("poll %s\n", ipne);
		status[nomer_sambung].timer++;
		if (status[nomer_sambung].timer > 5)	{
			sprintf(ipne, " [%d][%d]:%d.%d.%d.%d", nomer_sambung+1, uip_conn->lport, \
			htons(uip_conn->ripaddr[0]) >> 8, htons(uip_conn->ripaddr[0]) & 0xFF, \
			htons(uip_conn->ripaddr[1]) >> 8, htons(uip_conn->ripaddr[1]) & 0xFF );
			
			debug_out_h("pool timeout %s", ipne);
			uip_close();		
		}			
	}
	if (uip_newdata())	{
		len = uip_datalen();
		//printf("newdata = %d %s\n", len, ipne);
		
		if (status[nomer_sambung].stat == 3 && len == 248)		{
			portENTER_CRITICAL();
			memcpy((char *) &in_buf, uip_appdata, len);
			portEXIT_CRITICAL();
			
			if (strncmp(in_buf.mon, "OKdata", 6) == 0)		{
				printf("Data MOMON\n");	
				status[nomer_sambung].reply++;
				//memcpy((char *) &s_data[nomer_sambung].data, in_buf.buf, sizeof (data_float));
				//memcpy((char *) &data_f[nomer_sambung*PER_SUMBER].data, in_buf.buf, sizeof (data_float));
				//memcpy((char *) &data_f[nomer_sambung*PER_SUMBER], in_buf.buf, sizeof (data_float));
				//memcpy( (char *) &data_f[nomer_sambung*PER_SUMBER], in_buf.buf, (PER_SUMBER*sizeof (float)) );
				
				debug_out_h("-->[%d], mod %d", (nomer_sambung + 1), in_buf.alamat);
				
				#ifdef DEBUG_DATA
				printf(" %d:", sbg->nomer_samb);
				for (i=0; i<10; i++)	{
					printf("%3.3f ", s_data[sbg->nomer_samb].data[i]);	
				}
				printf("\n");
				#endif

			}
			
			uip_close();
			return;
		}
		else if (len > 0)	{
			sprintf(ipne, " [%d][%d]:%d.%d.%d.%d", nomer_sambung+1, uip_conn->lport, \
			htons(uip_conn->ripaddr[0]) >> 8, htons(uip_conn->ripaddr[0]) & 0xFF, \
			htons(uip_conn->ripaddr[1]) >> 8, htons(uip_conn->ripaddr[1]) & 0xFF );
	
			debug_out_h("Invalid uip_newdata() %s, leng = %d", ipne, len);
			uip_close();
			return;	
		}
	}
}
/*
void daytime_kirim_balik_appcall(void)
{
	int len;
	unsigned int nomer_sambung = (unsigned int) uip_conn->nomer_sambung;
	
	if (nomer_sambung > 19)
	{
		printf("Nomer sambung invalid !");
		uip_close();
		return;	
	}
	
	if (uip_connected())
	{
		if (status[nomer_sambung].stat == 1)
		{
			status[nomer_sambung].stat = 2;
		}
		else
		{
			printf("force to close daytime\n");
			uip_close();
			return;
		}
	}
	if (uip_closed())
	{
		status[nomer_sambung].stat = 0;
	}
	if (uip_aborted())
	{
		status[nomer_sambung].stat = 0;
	}
	if (uip_timedout())
	{
		status[nomer_sambung].stat = 0;
	}
	if (uip_poll())
	{
		status[nomer_sambung].timer++;
		if (status[nomer_sambung].timer > 5)
		{
			printf("pool timeout daytime\n");
			uip_close();		
		}			
	}
	if (uip_newdata())
	{
		len = uip_datalen();
		if (len == 26)
		{
			status[nomer_sambung].reply++;
			portENTER_CRITICAL();	
			memcpy(daytime, uip_appdata, 24);
			portEXIT_CRITICAL();			
			uip_close();	
		}
	}
}
//*/
#endif // PAKAI_KIRIM_BALIK

#ifdef PAKAI_TERIMA_BALIK
void terima_balik_init(void)
{
	loop_kirim = 0;
	uip_listen(HTONS(PORT_KIRIM_BALIK));
}

void terima_balik_appcall(void)
{
	int len;
	unsigned char str_monita[32];
	float temp_rpm;
	int i;
	int t;
	unsigned char ipne[32];
	float ada_nilai;
	
#if 0	
			sprintf(ipne, " :%d.%d.%d.%d", \
			htons(uip_conn->ripaddr[0]) >> 8, htons(uip_conn->ripaddr[0]) & 0xFF, \
			htons(uip_conn->ripaddr[1]) >> 8, htons(uip_conn->ripaddr[1]) & 0xFF );
			
	loop_kirim++;
	printf("\n * %d:", loop_kirim);
	if (uip_connected())	{
		printf("konek %s\n", ipne);
	}
	if (uip_closed())	{
		printf("close %s\n", ipne);
	}
	if (uip_aborted())	{
		printf("abort %s\n", ipne);
	}
	if (uip_timedout())		{
		printf("timeout %s\n", ipne);
	}	
	if(uip_acked()) 	{
		printf("ack %s\n", ipne);
	}
	if (uip_poll())		{
		printf("poll %s\n", ipne);	
	}
#endif
	
	if (uip_newdata())	{
		len = uip_datalen();
		//printf("newdata = %d %s\n", len, ipne);
		
		if (len >= 17)	{
			//*
			portENTER_CRITICAL();
			memcpy((char *) &temp_data, uip_appdata, len);
			portEXIT_CRITICAL();
			
			if (strncmp(temp_data.mon, "balikm1", 7) == 0)	{
				loop_kirim++;
				//printf("Masuk newdata balikm cocok: %d  ", loop_kirim);
				//#ifdef BOARD_KOMON_420_SAJA
				//#ifdef PAKAI_CYWUSB
				
					//status[nomer_sambung].reply++;
					memcpy( (char *) &data_tetangga, temp_data.buf, (PER_SUMBER*sizeof (float)) );
					//printf("nilai: %f, nilai2: %f\r\n", data_tetangga.data[1], data_tetangga.data[2]);
					//printf("nilai: %f = %.2f, nilai2: %.2f\r\n", ada_nilai, temp_data.buf[0], temp_data.buf[1]);
				//#endif
			}
			//*/
			strcpy(temp_data.mon, "OKdata");
				
			//portENTER_CRITICAL();
			//memcpy(xdata.buf, (char *) &data_float, sizeof (temp_data.mon));
			//portEXIT_CRITICAL();
				
				//send data ke network
			uip_send((char *) &temp_data.mon, sizeof (temp_data.mon));
			
			//printf("\n");	
		}
		else 
		{
			/*
			sprintf(ipne, " :%d.%d.%d.%d", \
			htons(uip_conn->ripaddr[0]) >> 8, htons(uip_conn->ripaddr[0]) & 0xFF, \
			htons(uip_conn->ripaddr[1]) >> 8, htons(uip_conn->ripaddr[1]) & 0xFF );
			
			printf("Unknown request dari %s\n", ipne);
			//*/
		}
		uip_close();	
	}
	
	if (uip_poll())
	{
		/*
		sprintf(ipne, " :%d.%d.%d.%d", \
			htons(uip_conn->ripaddr[0]) >> 8, htons(uip_conn->ripaddr[0]) & 0xFF, \
			htons(uip_conn->ripaddr[1]) >> 8, htons(uip_conn->ripaddr[1]) & 0xFF );
			
		printf("Invalid pool %s\n", ipne);
		//*/
		uip_close();	
	}
}	
#endif

#endif
