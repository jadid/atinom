/*
 * gpio.h
 *
 *  Created on: Dec 9, 2008
 *      Author: jadid
 */

#include "FreeRTOS.h"

#ifndef GPIO_H_
#define GPIO_H_

#if defined(BOARD_KOMON_KONTER) || defined(BOARD_KOMON_KONTER_3_0)

#define kont_10		0x10	// P24, konter_10
#define kont_9		0x08	// P23
#define kont_8		0x04	// P22
#define kont_7		0x02	// P21
#define kont_6		0x01	// P20

#ifdef BOARD_KOMON_KONTER_3_1
#define kont_1		BIT(5) 	// 0x20	// P25, konter_5
#define kont_2		BIT(6)	// 0x40	// P26
#define kont_3		BIT(7)	// 0x80	// P27
#define kont_4		BIT(8)	// 0x00	// P28
#define kont_5		BIT(9)	// 0x00	// P29
#endif

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

void init_gpio(void);

#endif
#endif /* GPIO_H_ */
