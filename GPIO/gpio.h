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

#endif /* GPIO_H_ */
