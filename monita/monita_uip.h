/*
 * monita_uip.h
 *
 *  Created on: Dec 10, 2008
 *      Author: jadid
 */

#include "../uip/uip/psock.h"
#include "../uip/uip/uipopt.h"


#ifndef MONITA_UIP_H_
#define MONITA_UIP_H_

struct t_data_float {
	float data[20];
};

struct t_xdata {
	char mon[8];             // id bahwa data monita
    unsigned int nomer;      // urutan pengiriman data
    unsigned char flag;      // belum di definisikan
    unsigned char alamat;    // alamat stacking board (address power meter)

	char buf[232];				//16 jan 08, disesuaikan dengan Client Utama
};

struct monita_state {
	struct psock p;
	char in_buf[10];
	char name[40];
};

struct sambungan_state {
	struct psock p;
	char state;
	struct t_xdata in_buf;
	//char in_buf[256];	// harusnya 248 cukup
	//char name[40];
};

//typedef struct sambungan_state uip_tcp_appstate_t;

/*
sumber status :
	0 : tidak ada
	1 : time_out
	2 : normal
*/
struct t_sumber {
	char nama[10];
	char ID_sumber;
	char IP0;
	char IP1;
	char IP2;
	char IP3;
	char status;		// tidak aktif, timeout, dll	
};

struct t_titik {
	char nama[10];
	char ID_mesin;
	char ID_sumber;
	char kanal;
};

struct t_mesin {
	char nama[10];
	char ID_mesin;
};

#endif /* MONITA_UIP_H_ */