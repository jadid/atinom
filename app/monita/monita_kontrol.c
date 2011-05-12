/*
	24 Juli 2009
	monita_kontrol, furkan jadid
	
	menerima perintah kontrol dari server, misalnya relay_on atau relay_off
	menggunakan PORT_KONTROL, yang akan dipanggil oleh uip_task.c
	
	*/
#include "monita_uip.h"
#include "uip.h"
#include <stdio.h>
#include <stdlib.h>

#define debug		printf
//#define debug(...)	do{} while(0)

struct t_xcontrol kontrol;

unsigned char koil_stat[8];

void kontrol_init(void)
{
	uip_listen(HTONS(PORT_KONTROL));
}

void kontrol_appcall(void)
{
	debug("%s(): ", __FUNCTION__);
	
	if (uip_connected())
	{
		debug("connected\r\n");
	}
	if (uip_closed())
	{
		debug("closed\r\n"); 
	}
	if (uip_aborted())
	{
		debug("aborted\r\n"); 
	}
	if (uip_timedout())
	{
		debug("timedout\r\n"); 
	}
	if(uip_acked()) 
	{
		debug("acked\r\n"); 
	}
	if (uip_poll())
	{
		debug("poll\r\n"); 
	}
	if (uip_newdata())
	{
		debug("newdata\r\n"); 
		portENTER_CRITICAL();
		memcpy((char *) &kontrol, uip_appdata, uip_datalen());
		portEXIT_CRITICAL();
		
		if (strncmp( kontrol.header, "MNT_kontrol", 11) == 0)
		{
			debug("Data kontrol OK\r\n");
			if (kontrol.out_kanal < 8)
			{
				koil_stat[ kontrol.out_kanal ] = kontrol.status;
			}
			else
			{
				debug("ERR: Kanal terlalu besar !\r\n"); 
			}
		}
		else
		{
			debug("ERR: Bukan data kontrol !\r\n");
		}
	}
}
