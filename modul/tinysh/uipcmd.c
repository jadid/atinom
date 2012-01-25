/*
	berkaiatan dengan sumber ip address 
	afrendy bayu
	daun biru engineering

	24 Sept 2010
	Setting data id via webserver
//*/

#ifndef __SHELL_UIP__
#define __SHELL_UIP__

#include "FreeRTOS.h"
#include "../uip/uip/uip_arp.h"

#ifdef PAKAI_ETH

extern int persen_pwm;

void cek_ip_client(int argc, char **argv)	{
	uip_arp_table_list();
}

static tinysh_cmd_t cek_mac_cmd={0,"cek_mac","cek mac dan IP client","", cek_ip_client,0,0,0};

#if 1
void webclient_coba(int argc, char **argv)	{
	status_webc_i = 1;
	vTaskDelay(1);
}

static tinysh_cmd_t cek_webcc_cmd={0,"cek_webc","ambil data dari internet","", webclient_coba,0,0,0};
#endif

#ifdef PAKAI_RESOLV
void cek_server(int argc, char **argv)	{
	if (argc!=2)
		return;

	status_webc_i = 2;
	printf("argv[0]:%s, argv[1]: %s : %d\r\n", argv[0], argv[1], status_webc_i);
	strcpy(urlweb, argv[1]);
	vTaskDelay(1);
}

static tinysh_cmd_t cek_server_cmd={0,"cek_server","ambil data dari internet","", cek_server,0,0,0};

void cek_dns(int argc, char **argv)	{
	resolv_table();
}

static tinysh_cmd_t cek_dns_cmd={0,"cek_dns","ambil data dari internet","", cek_dns,0,0,0};

void cek_ip(int argc, char **argv)	{
	if (argc!=2)
		return;

	status_webc_i = 4;
	printf("argv[0]:%s, argv[1]: %s : %d\r\n", argv[0], argv[1], status_webc_i);
	strcpy(urlweb, argv[1]);
}

static tinysh_cmd_t cek_ip_cmd={0,"cek_ip","ambil data dari internet","", cek_ip,0,0,0};

#endif

#endif
#endif
