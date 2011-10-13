
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#ifdef PAKAI_TSC

#ifndef __SHELL_TSC__
#define __SHELL_TSC__

#include "../app/tsc/fma1125.h"
#include "enviro.h"

extern xTaskHandle *hdl_tampilan;

static void ganti_reg_fma(int argc, char **argv)	{
	unsigned char buf[64];

	int dat;
	int reg;
	
	if (argc > 1)
	{
		sprintf(buf, "%s", argv[1]);
		sscanf(buf, "%d", &reg);
	
		if (argc > 2)
		{
			sprintf(buf, "%s", argv[2]);
			sscanf(buf, "%d", &dat);
			//i2c_write( addr, reg, 1, &val, 1

			//portENTER_CRITICAL();
			//if (i2c_write(0x68, reg, 1, &dat, 1)) 
			if (i2c_set_register(0x68, reg, dat))
				printf("Gagal !\r\n");
			else			
				printf("Set register %d (0x%02X) ke %d\r\n", reg, reg, dat);
				
			if (reg==41 || reg==42) {
				struct t_env *p_env2;
				p_env2 = pvPortMalloc( sizeof (struct t_env) );
				
				if (p_env2 == NULL)	{
					printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
					return -1;
				}
				//printf(" %s(): Mallok ok di %X\r\n", __FUNCTION__, p_env2);	
				portENTER_CRITICAL();
				memcpy((char *) p_env2, (char *) ALMT_ENV, sizeof (struct t_env));
				portEXIT_CRITICAL();
				
				if (reg==41) {
					p_env2->k1 = dat;
				} else if (reg==42) {
					p_env2->k2 = dat;
				}
				
				if (simpan_env( p_env2 ) < 0) {
					vPortFree( p_env2 );
				}
				vPortFree( p_env2 );
			}
			//portEXIT_CRITICAL();
			vTaskDelay(20);
		}
		else
			printf("Argument kurang, set_filter nomer_filter gain_code\r\n");
		
	}
}

static void baca_reg_fma(int argc, char **argv)
{
	unsigned char buf[32];

	int reg;
	unsigned char qq;
	
	if (argc > 1)
	{
		sprintf(buf, "%s", argv[1]);		//printf("isi buf: %s\r\n", buf);
		sscanf(buf, "%d", &reg);			//printf("isi buf: %d\r\n", reg+2);
		{
			struct t_env *env2;
			env2 = (char *) ALMT_ENV;
			//printf("k1: %d, k2: %d\r\n", env2->k1, env2->k2);
		}
		{
			//portENTER_CRITICAL();
			i2c_read_register(0x68, reg, &qq);		//i2c_read( 0x68, reg, 1, &dat, 1);
			//portEXIT_CRITICAL();
			printf("register fma %d (0x%02X) = %d\r\n", reg, reg, qq);
		}
		
	}
}

static tinysh_cmd_t set_fma_cmd={0,"set_fma","Mengeset waktu","thn,bulan,tgl,jam,menit",ganti_reg_fma,0,0,0};  

static tinysh_cmd_t read_fma_cmd={0,"read_fma","Mengeset waktu","thn,bulan,tgl,jam,menit",baca_reg_fma,0,0,0}; 

static void setting_fma(int argc, char **argv)
{
	int c;
	
	/* suspend task haliza utama dulu */
	vTaskSuspend( hdl_tampilan );
	xSerialGetChar(1, &c, 20 );

	/* disable interrupt touchpad */
	//*pFIO_MASKA_S = 0;
	//ssync();
	
	while( 1 )
	{
		if (xSerialGetChar(1, &c, 20 ) == pdTRUE) break;
		cls_layar();
		read_key();
		update_lcd();
	}
	
	//cls_layar();	
	//*pFIO_MASKA_S = PORT_INT_KEYPAD;
	//ssync();
	vTaskResume( hdl_tampilan );

}

static tinysh_cmd_t setting_fma_cmd={0,"fma","Mengeset waktu","thn,bulan,tgl,jam,menit",
                              setting_fma,0,0,0};

#define ADDR_FMA1	0x68


void baca_register_tsc_lagi() {
	unsigned char qq;
	int ww, zz;
	
	//printf("\t%s\r\n", __FUNCTION__);
	i2c_read_register(ADDR_FMA1, TSC_TOUCH_BYTE, &qq);
	printf("0x%02X: TOUCH_BYTE: %d\r\n", TSC_TOUCH_BYTE, qq);
	
	for (ww=0; ww<8; ww++) 
	{
		i2c_read_register(ADDR_FMA1, (TSC_ALPHA_00+ww), &qq);		// TSC_ALPHA_00
		printf("0x%02X: ALPHA%d: %d   ", (TSC_ALPHA_00+ww), ww, qq);

		i2c_read_register(ADDR_FMA1, (TSC_STR_THRES_00+ww), &qq);
		printf("0x%02X: STR_THRES%d: %d   ", (TSC_STR_THRES_00+ww),  ww, qq);

		//*	
		i2c_read_register(ADDR_FMA1, (TSC_STRENGTH_00+ww), &qq); // TSC_STRENGTH_00   0x50
		printf("0x%02X: STRENGTH%d: %d   ", (TSC_STRENGTH_00+ww), ww, qq);

		i2c_read_register(ADDR_FMA1, (TSC_IMP_00+ww), &qq);	// SC_IMP_00   0x58
		printf("0x%02X: IMP%d: %d   ", (TSC_IMP_00+ww), ww, qq);
		
		i2c_read_register(ADDR_FMA1, (TSC_REF_IMP_00+ww), &qq);		// 0x60
		printf("0x%02X: REF_IMP%d: %d\r\n", (TSC_REF_IMP_00+ww), ww, qq);
	}
	printf("\r\n");
	//*/
}

static tinysh_cmd_t baca_register_tsc_cmd={0,"tsc","Mengeset waktu","thn,bulan,tgl,jam,menit",
                              baca_register_tsc_lagi,0,0,0};

#endif		// __SHELL_TSC__

#endif		// PAKAI_TSC
