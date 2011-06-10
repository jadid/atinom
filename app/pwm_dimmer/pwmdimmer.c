// 31 Mei 2011	//
// Afrendy Bayu	//

#include "FreeRTOS.h"

int persen_pwm=10;

#ifdef PAKAI_GPIO_DIMMER
	
#define FULL_PERSEN		100

unsigned char loop_dimmer=0;

int xpwm=1;

int init_remang() {
	printf(" init PWM GPIO ...!!! \r\n");
	//hitung_remang();
}

int remangkan() {
	loop_dimmer++;
	
	if (loop_dimmer>FULL_PERSEN)
		loop_dimmer=0;
	if (loop_dimmer<persen_pwm) {
		FIO1SET = BIT(30);
	} else {
		FIO1CLR = BIT(30);
	}
		
	/*
	if ((loop_dimmer%xpwm)==0) {
		FIO1SET = BIT(30);
	} else {
		FIO1CLR = BIT(30);
	}
	//*/
	return 1;
}

#endif 
