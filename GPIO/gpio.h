/*
 * gpio.h
 *
 *  Created on: Dec 9, 2008
 *      Author: jadid
 */

#ifndef GPIO_H_
#define GPIO_H_

#define kont_10		0x10	// P24, konter_10
#define kont_9		0x08	// P23
#define kont_8		0x04	// P22
#define kont_7		0x02	// P21
#define kont_6		0x01	// P20

#define JUM_GPIO	10

#define GPIO_SPEED  BIT(26)     // P0.26 ==> interrupt
#define GPIO_ARAH   BIT(25)     // P0.25 ==> adc input

typedef struct {
	//unsigned int new_period;
	unsigned int last_period;
	unsigned int beda;			// kali 10
	unsigned int hit;
	unsigned int hit_lama;		// untuk mengetahui mati atau enggak
} ts_konter;

struct t2_konter{
	unsigned int global_hit;
	unsigned int ovflow;		// overflow count untuk Timer 1
	ts_konter t_konter[JUM_GPIO];
};

struct t_angin {
	unsigned int hit;
	unsigned int beda;
	unsigned int hit_lama;
	unsigned int last_period;
	//unsigned int arah;
};

struct t_fangin {
	float rps;				// instan rps (kira2 setiap 200 ms)
	float speed;
	float tot_rps;			// akumulasi rps
	unsigned int jum_rps;	// jumlah data akumulasi rps
	unsigned int arah;		// dalam mV
};

#endif /* GPIO_H_ */
