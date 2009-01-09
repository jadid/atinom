/*
	environtment & baca tulisnya
	
	furkan jadid
	daun biru engineering
	
	12 Desember 2008
	
	*/

#include "enviro.h"
#include "FreeRTOS.h"
#include "task.h"

// kalau ditaruh di ethram, nulisnya jadi error
//struct t_env env2  __attribute__ ((section (".eth_test")));
struct t_env env2;

unsigned int command[5]; 	// For Command Table
unsigned int result[2]; 	// For Result Table
IAP iap_entry;

void tulis_env_flash(struct t_env *ev);
int tulis_env(void);

void print_env(int argc, char **argv)
{
	baca_env(1);
}

void getdef_env(int argc, char **argv)
{
	printf(" Set Default Environtment \n");
	garis_bawah();	
	set_default_ip();
	printf(" OK\n");
}


void save_env(int argc, char **argv)
{
	printf(" Saving environtment\n");
	garis_bawah();

	tulis_env();
	
	printf("\n");
}

int tulis_env(void)
{
	//struct t_env env;
	int i;
	
	/*
	sprintf(env.nama_board, "Tes konter");
	env.IP0 = 192;
	env.IP1 = 168;
	env.IP2 = 1;
	env.IP3 = 5;

	for (i=0; i<20; i++)
	{
		env.kalib[i].m = 1234;		// 1.234
		env.kalib[i].y = 1;			// 0.001
	}
	*/
	/*
	taskENTER_CRITICAL();
	sh = malloc(sizeof (struct t_env));
	taskEXIT_CRITICAL();
	if (sh == 0)
	{
		printf("Memory alloc gagal !\n");
		return 0;	
	}

	taskENTER_CRITICAL();
	memcpy((char *) &sh, (char *) &env2, sizeof env2);
	taskEXIT_CRITICAL();
	*/	
	env2.magic1 = 0xAA;
	env2.magic2 = 0x77;

	int uk=256;

	if (sizeof (struct t_env) > 256) uk = 512;
	if (sizeof (struct t_env) > 512) uk = 1024;
	if (sizeof (struct t_env) > 1024) uk = 4096;

	printf(" Tulis env .. %d bytes ..", uk);
		
		iap_entry = (IAP)IAP_LOCATION; // Set Function Pointer

		command[0] = 50; 	// Prepare sector(s) for a Erase/Write Operation
		command[1] = 24;	// start sector
		command[2] = 24;	// end sector

		taskENTER_CRITICAL();
		iap_entry(command,result);
		taskEXIT_CRITICAL();

		if (result[0]) {
			printf(" prepare error !\n");
			return;
		}

		printf("..");
		command[0] = 52; 	// Hapus dulu
		command[1] = 24;	// start sector
		command[2] = 24;	// end sector
		command[3] = 60000;	// PCLK

		taskENTER_CRITICAL();
		iap_entry(command,result);
		taskEXIT_CRITICAL();

		if (result[0]) {
			printf(" hapus error !\n");
			return;
		}

		printf("..");
		command[0] = 50; 	// Prepare sector(s) for a Erase/Write Operation
		command[1] = 24;	// start sector
		command[2] = 24;	// end sector

		taskENTER_CRITICAL();
		iap_entry(command,result);
		taskEXIT_CRITICAL();

		if (result[0]) {
			printf(" prepare2 error !\n");
			return;
		}

		printf("..");
		command[0] = 51;
		command[1] = 0x007A000;	// tujuan flash (seuai dengan sektor flash)
		command[2] = (unsigned short *) &env2;		// source ram
		//command[2] = &sh;	
		command[3] = uk;
		command[4] = 60000;// PCLK = 60000 KHz

		taskENTER_CRITICAL();
		iap_entry(command,result);
		taskEXIT_CRITICAL();

		if (result[0]) {
			printf(" flash write error %d!\n", result[0]);
			return;
		}
		printf(".. OK");
		
		//free(sh);
	//tulis_env_flash(&env);
}

void tulis_env_flash(struct t_env *ev)
{

}

int baca_env(char tampil)
{
	struct t_env *ev;
	int i;
	char teku[128];
	
	if (tampil == 1)
	{
		printf(" Data Environtment \n");
		garis_bawah();
	}
	else
	{
		// pada saat awal start, pasti akses ini dulu
		ev = (struct t_env *) 0x7A000;
		memcpy((char *)&env2, (char *) 0x7A000, sizeof (env2));	
	}
	
	if (ev->magic1 == 0xAA)
	{
		if (ev->magic2 == 0x77)
		{			
			printf(" Nama board = ");
			printf(env2.nama_board);
			printf("\n IP Address = ");
			printf("%d.%d.%d.%d\n", env2.IP0, env2.IP1, env2.IP2, env2.IP3);
			
			// tampilkan GW address, pakai env2 sekalian buat ngetes
			printf(" Gateway IP = ");
			printf("%d.%d.%d.%d\n", env2.GW0, env2.GW1, env2.GW2, env2.GW3); 
			
			if (tampil == 1)
			{
				printf(" Faktor kalibrasi :\n");
				
				extern char tek;
				
				double ff = 0.123;
				int t;
				
				portENTER_CRITICAL();
				//snprintf(teku, 64, " float %f", ff);
				//teku = (char *) fcvt(ff, 6, i, t);
				printf("%f", ff);
				
				portEXIT_CRITICAL();
				/*
				for (i=0; i<20; i++)
				{
					printf("  (%2d) m = %3.3f, y=%3.3f", i+1, env2.kalib[i].m, env2.kalib[i].y);
					i++;
					printf("  (%2d) m = %3.3f, y=%3.3f\n", i+1, env2.kalib[i].m, env2.kalib[i].y);
				}*/
				
				printf(" Keterangan kanal :\n");
				for (i=0; i<20; i++)
				{
					printf("  (%2d) %s\n", i+1, env2.kalib[i].ket);
				}
				printf("\n");
			}			
			return 0;
		}
		else
		{
			printf(" Magic number2 salah !\n");
			set_default_ip();
			
			return -1;
		}
	}
	else
	{
		printf(" Magic number1 salah !\n");		
		set_default_ip();
		
		return -1;
	}
}

void set_default_ip(void)
{
	int i;	

	printf(" Default IP = ");	
	env2.IP0 = 192;
	env2.IP1 = 168;
	env2.IP2 = 1;
	env2.IP3 = 250;	
	printf("%d.%d.%d.%d\n", env2.IP0, env2.IP1, env2.IP2, env2.IP3);
	
	printf(" Default GW = ");	
	env2.GW0 = 192;
	env2.GW1 = 168;
	env2.GW2 = 1;
	env2.GW3 = 13;
	printf("%d.%d.%d.%d\n", env2.GW0, env2.GW1, env2.GW2, env2.GW3);

	for (i=0; i<20; i++)
	{
		env2.kalib[i].m = 1.00;
		env2.kalib[i].y = 0.00;
		sprintf(env2.kalib[i].ket, "--");
	} 

	sprintf(env2.nama_board, "Gantilah namanya !");
}
