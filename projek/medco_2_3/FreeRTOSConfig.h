/*
	FreeRTOS.org V5.1.1 - Copyright (C) 2003-2008 Richard Barry.

	This file is part of the FreeRTOS distribution.

	FreeRTOS is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	FreeRTOS is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FreeRTOS; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	A special exception to the GPL can be applied should you wish to distribute
	a combined work that includes FreeRTOS, without being obliged to provide
	the source code for any proprietary components.  See the licensing section
	of http://www.FreeRTOS.org for full details of how and when the exception
	can be applied.

	***************************************************************************
	See http://www.FreeRTOS.org for documentation, latest information, license
	and contact details.  Please ensure to read the configuration and relevant
	port sections of the online documentation.
	***************************************************************************
*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <stdio.h>
#include "lpc23xx.h"



#define PAKAI_ID
#define CENDOL		// cendol adalah versi terbaru komunikasi data modul monita
					// cendol memungkinkan client mengakses file php di server untuk menyimpan datanya

#define BOARD_KOMON
//#define BOARD_KOMON_420_SABANG
#define BOARD_KOMON_420_SABANG_2_3
#define VERSI_KOMON		"1.70"


#define BANYAK_SUMBER

#define _printf	printf2
#define printf	printf2

//#define PAKAI_MULTI_SERIAL
//#define PAKAI_ADC

//#define PAKAI_SERIAL_2			// mendefinisikan aktif serial 1
//#define PAKAI_PM			2		// nyambung ke SERIAL 3

//#define AMBIL_PM
//#define PAKAI_MAX485		1
//#define PAKAI_SERIAL_3				// mendefinisikan aktif serial 1
//#define PAKAI_PM			3		// nyambung ke SERIAL 3

#ifdef PAKAI_PM
	#define BAUD_PM			19200
	#define TIPE_PM810
	#define TIPE_PM710
#endif


//#define TES_GET_WEB

#define PROMPT 		"BAS@"
#define NAMA_BOARD	"Babelan Komon-420"

#define PAKAI_LED_UTAMA

//#define PAKAI_SERIAL_2
#ifdef PAKAI_SERIAL_2
	#define PAKAI_SERIAL_2_P0		4800
//	#define PAKAI_GPS				2
//	#define DEBUG_GPS
#endif

#define PAKAI_SHELL

//#define PAKAI_ETH
#ifdef PAKAI_ETH
//	#define PAKAI_ENC28J60		// bukan untuk modul sabang 2.3
	#define PAKAI_ENCX24J600

	#define PAKAI_MODBUSTCP
//	#define SAMPURASUN_CLIENT
//	#define SAMPURASUN_SERVER
//	#define PAKAI_WEBCLIENT
//	#define PAKE_TELNETD
	#define PAKAI_HTTP
#endif

//#define PAKAI_ADC
#define PAKAI_GPIO_DIMMER
#ifdef PAKAI_GPIO_DIMMER
	#define PAKAI_TIMER2
	
	
#endif
//#define DEBUG_PWM_GPIO

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

/* Value to use on old rev '-' devices. */
//#define configPINSEL2_VALUE	0x50151105

/* Value to use on rev 'A' and newer devices. */

#define INCLUDE_uxTaskGetStackHighWaterMark 	1

#define configPINSEL2_VALUE 	0x50150105

#ifndef configPINSEL2_VALUE
	#error Please uncomment one of the two configPINSEL2_VALUE definitions above, depending on the revision of the LPC2000 device being used.
#endif

#define configUSE_PREEMPTION		0
#define configUSE_IDLE_HOOK         1
#define configUSE_TICK_HOOK         0
//#define configCPU_CLOCK_HZ          ( ( unsigned portLONG ) 48000000 )	/* =12Mhz xtal multiplied by 5 using the PLL. */
#define configCPU_CLOCK_HZ          ( ( unsigned portLONG ) 60000000 )
#define configTICK_RATE_HZ          ( ( portTickType ) 1000 )
#define configMAX_PRIORITIES		( ( unsigned portBASE_TYPE ) 4 )
#define configMINIMAL_STACK_SIZE	( ( unsigned portSHORT ) 104 )
#define configTOTAL_HEAP_SIZE		( ( size_t ) ( 30 * configMINIMAL_STACK_SIZE * 12) )
#define configMAX_TASK_NAME_LEN		( 10 )
#define configUSE_TRACE_FACILITY	0		// 1
#define configUSE_16_BIT_TICKS		0
#define configIDLE_SHOULD_YIELD		1
#define configUSE_MUTEXES			1

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )


/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet            0
#define INCLUDE_uxTaskPriorityGet           0
#define INCLUDE_vTaskDelete                 0
#define INCLUDE_vTaskCleanUpResources       0
#define INCLUDE_vTaskSuspend                0
#define INCLUDE_vTaskDelayUntil             0
#define INCLUDE_vTaskDelay                  1
#define INCLUDE_xTaskGetCurrentTaskHandle	0


#endif /* FREERTOS_CONFIG_H */


#ifndef sbi
#define sbi(x,y)	x|=(1 << (y))
#endif

#ifndef cbi
#define cbi(x,y)	x&=~(1 << (y))
#endif

#ifndef tstb
#define tstb(x,y)	(x & (1 << (y)) ? 1 : 0)
#endif

#ifndef toggle
#define toggle(x,y)	x^=(1 << (y))
#endif

#ifndef BIT
#define BIT(x)	(1 << (x))

#endif
