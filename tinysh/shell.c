#include <stdio.h>
#include <errno.h>

#include "tinysh.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "rw_flash.c"
#include "set_ipaddr.c"
#include "enviro.h"

#include "../GPIO/gpio.h"

#include <stdlib.h>

void reset_cpu(void);
extern struct t2_konter konter;

/*****************************************************************************/
// komand2 daun biru komon-kounter

#define per_oflow (0xFFFFFFFF * 0.050) / (1000 * 1000 * 60)
#define per_tik		0.050 / (1000 * 1000 * 60)


static tinysh_cmd_t save_env_cmd={0,"saveenv","menyimpan env","[args]",
                              save_env,0,0,0};
							  
static tinysh_cmd_t printenv_cmd={0,"printenv","menampilkan env","[args]",
                              print_env,0,0,0};
							  
static tinysh_cmd_t reset_cmd={0,"reset","reset cpu saja","[args]",
                              reset_cpu,0,0,0};
							  
char *uptime(void)
{
	float upt;
	unsigned int i;
	char buf[32];
	
	upt = (float) konter.ovflow * per_oflow;
	i = T1TC;
	upt = upt + (i * (float) per_tik);
	if (upt > 60)
	{
		upt = upt / 60;
		if (upt > 24)
		{
			sprintf(buf,"%.3 hari", upt);
			return buf;
		}
		sprintf(buf, "%.3f jam\n", upt);
		return buf;
	}
	else
	{
		sprintf(buf, "%.3f menit\n", upt);
		return buf;
	}
	
}

static void cek_rpm(int argc, char **argv)
{
	unsigned int i;
	float uptime;

	printf("Global hit = %d\n", konter.global_hit);
	printf("Ov flow = %d\n", konter.ovflow);

	for (i=0; i<10; i++)
	{
		printf(" %2d : beda = %12d ns, hit = %d\n", (i+1), konter.t_konter[i].beda, konter.t_konter[i].hit);

	}
	uptime = (float) konter.ovflow * per_oflow;
	i = T1TC;
	uptime = uptime + (i * (float) per_tik);
	if (uptime > 60)
	{
		uptime = uptime / 60;
		printf("Uptime = %.3f jam\n", uptime);
	}
	else
		printf("Uptime = %.3f menit\n", uptime);
}

static tinysh_cmd_t cek_rpm_cmd={0,"cek_rpm","data kounter/rpm","[args]",
                              cek_rpm,0,0,0};


/*****************************************************************************/

//extern void *tinysh_get_arg(void);

//static 
void display_args(int argc, char **argv)
{
  int i;
  for(i=0;i<argc;i++)
    {
      printf(" argv[%d]=\"%s\"\n",i,argv[i]);
    }
}

static void foo_fnt(int argc, char **argv)
{
  printf("foo command called\n");
  display_args(argc,argv);
}

static tinysh_cmd_t myfoocmd={0,"foo","foo command","[args]",
                              foo_fnt,0,0,0};

static void item_fnt(int argc, char **argv)
{
  printf("item%d command called\n",(int)tinysh_get_arg());
  display_args(argc,argv);
}

static tinysh_cmd_t ctxcmd={0,"ctx","contextual command","item1|item2",
                            0,0,0,0};
static tinysh_cmd_t item1={&ctxcmd,"item1","first item","[args]",item_fnt,
                           (void *)1,0,0};
static tinysh_cmd_t item2={&ctxcmd,"item2","second item","[args]",item_fnt,
                           (void *)2,0,0};

static void reset_to_0(int argc, char **argv)
{
  int *val;
  val=(int *)tinysh_get_arg();
  *val=0;
}

static void atoxi_fnt(int argc, char **argv)
{
  int i;

  for(i=1;i<argc;i++)
    {
      printf("\"%s\"-->%u (0x%x)\n",
             argv[i],tinysh_atoxi(argv[i]),tinysh_atoxi(argv[i]));
    }
}

static tinysh_cmd_t atoxi_cmd={0,"atoxi","demonstrate atoxi support",
                               "[args-to-convert]",atoxi_fnt,0,0,0};

xSemaphoreHandle bg_sem;

static void bg_cmd_fnt(int argc, char **argv)
{
  printf("bg_cmd command called\n");
  xSemaphoreGive( bg_sem );
}

static tinysh_cmd_t bgcmd={0,"bg_cmd","bg_cmd command","[args]",
                              bg_cmd_fnt,0,0,0};

portTASK_FUNCTION( bg_cmd_thread, pvParameters )
{
	vSemaphoreCreateBinary( bg_sem );
    xSemaphoreTake( bg_sem, 0 );
	for(;;) {
	    while( xSemaphoreTake( bg_sem, 10000 ) != pdPASS );
	    vTaskDelay( 5 );
		printf("**** this is a back ground task *****\n");
	}
}

//int shell(int argc, char **argv)
portTASK_FUNCTION(shell, pvParameters )
{
  	int c;
  	int again=1;
  	xTaskHandle xHandle;

  	printf("\nStarting Babelan Komon-Counter 1.0\n");
  	printf("Daun Biru Engineering, Des 2008\n");
  	printf("=========================================\n");
  	printf("ARM-GCC %s : %s : %s\n", __VERSION__, __DATE__, __TIME__);
  	printf("CPU = NXP LPC 2368, %d MHz\n", configCPU_CLOCK_HZ/1000000);
  	printf("FreeRTOS 5.1.1 by Richard Barry\n");
	printf("uIP TCP/IP by Adam Dunkels\n");

  	//tes_tulis_flash();
  	//tes_baca_flash();

	/* 
	 * add command
	 */
  	tinysh_add_command(&myfoocmd);
  	tinysh_add_command(&printenv_cmd);
  	tinysh_add_command(&cek_rpm_cmd);
	tinysh_add_command(&setenv_cmd);
	tinysh_add_command(&save_env_cmd);
	tinysh_add_command(&reset_cmd);

	/* add sub commands
 	*/
  	//tinysh_add_command(&ctxcmd);
  	//tinysh_add_command(&item1);
  	//tinysh_add_command(&item2);

	/* use a command from the stack
 	* !!! this is only possible because the shell will have exited
 	* before the stack of function main !!!
 	*/
  	/*
	{
    	tinysh_cmd_t quitcmd={0,"quit","exit shell",0,reset_to_0,
                          (void *)&again,0,0};
    	tinysh_add_command(&quitcmd);
  	}*/

	/* add atoxi support test command */
  	//tinysh_add_command(&atoxi_cmd);



	/* add a background command */
  	//tinysh_add_command(&bgcmd);
  	//xTaskCreate( bg_cmd_thread, "bg_cmd", 1000, NULL, 2, &xHandle);
	

  	vTaskDelay(450);
  	tinysh_set_prompt("Babelan $ ");
	/* 
	 * main loop shell
  	 */
  	while(again)
    {
	  xSerialGetChar(1, &c, 0xFFFF );
	  tinysh_char_in((unsigned char)c);
    }
  	printf("\nBye\n");
  	return;
}

void init_shell(void)
{
	//xTaskCreate( shell, "Shell", 1000, NULL, 1, ( xTaskHandle * ) NULL);
	xTaskCreate( shell, "UsrTsk1", 2048, NULL, tskIDLE_PRIORITY, ( xTaskHandle * ) NULL);
}
