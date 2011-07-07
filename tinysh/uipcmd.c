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

#endif
#endif
