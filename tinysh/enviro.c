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

#if 0
void getdef_env(int argc, char **argv)
{
	printf(" Set Default Environtment \r\n");
	garis_bawah();	
	set_default_ip();
	printf(" OK\n");
}
#endif

void save_env(int argc, char **argv)
{
	printf(" Saving environtment\r\n");
	garis_bawah();

	tulis_env();
	
	printf("\r\n");
}

int tulis_env(void)
{
	int i;
	
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
			printf(" prepare error !\r\n");
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
			printf(" hapus error !\r\n");
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
			printf(" prepare2 error !\r\n");
			return;
		}

		printf("..");
		command[0] = 51;
		command[1] = 0x007A000;	// tujuan flash (seuai dengan sektor flash)
		command[2] = (unsigned short *) &env2;		// source ram
		command[3] = uk;
		command[4] = 60000;// PCLK = 60000 KHz

		taskENTER_CRITICAL();
		iap_entry(command,result);
		taskEXIT_CRITICAL();

		if (result[0]) {
			printf(" flash write error %d!\r\n", result[0]);
			return;
		}
		printf(".. OK");
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
		printf(" Data Environtment \r\n");
		garis_bawah();
	}
	else
	{
		/* pada saat awal start, pasti akses ini dulu */
		ev = (struct t_env *) 0x7A000;
		memcpy((char *)&env2, (char *) 0x7A000, sizeof (env2));	
	}
	
	if (ev->magic1 == 0xAA)
	{
		if (ev->magic2 == 0x77)
		{			
			printf(" Nama board = ");
			printf(env2.nama_board);
			printf("\r\n IP Address = ");
			printf("%d.%d.%d.%d\r\n", env2.IP0, env2.IP1, env2.IP2, env2.IP3);
			
			// tampilkan GW address, pakai env2 sekalian buat ngetes
			printf(" Gateway IP = ");
			printf("%d.%d.%d.%d\r\n", env2.GW0, env2.GW1, env2.GW2, env2.GW3); 
			
			if (tampil == 1)
			{
				printf(" Faktor kalibrasi (y = mx + C) :\r\n");
				
				extern char tek;
				
				double ff = 0.123;
				int t;
				
				/*
				portENTER_CRITICAL();
				//snprintf(teku, 64, " float %f", ff);
				//teku = (char *) fcvt(ff, 6, i, t);
				printf("%f", ff);
				
				portEXIT_CRITICAL();
				*/
				
				for (i=0; i<20; i++)
				{
					printf("  (%2d) m = %3.3f, C = %3.3f", i+1, env2.kalib[i].m, env2.kalib[i].C);
					i++;
					printf("  (%2d) m = %3.3f, C = %3.3f\r\n", i+1, env2.kalib[i].m, env2.kalib[i].C);
				}
				
				printf(" Keterangan kanal :\r\n");
				for (i=0; i<20; i++)
				{
					printf("  (%2d) %s\r\n", i+1, env2.kalib[i].ket);
				}
				printf("\n");
			}			
			return 0;
		}
		else
		{
			printf(" Magic number2 salah !\r\n");
			set_default_ip();
			
			return -1;
		}
	}
	else
	{
		printf(" Magic number1 salah !\r\n");		
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
	printf("%d.%d.%d.%d\r\n", env2.IP0, env2.IP1, env2.IP2, env2.IP3);
	
	printf(" Default GW = ");	
	env2.GW0 = 192;
	env2.GW1 = 168;
	env2.GW2 = 1;
	env2.GW3 = 13;
	printf("%d.%d.%d.%d\r\n", env2.GW0, env2.GW1, env2.GW2, env2.GW3);

	set_dafault_kalib();

	sprintf(env2.nama_board, "Gantilah namanya !");
}

void set_dafault_kalib(void)
{
	int i;
	
	for (i=0; i<20; i++)
	{
		env2.kalib[i].m = 1.00;
		env2.kalib[i].C = 0.00;
		sprintf(env2.kalib[i].ket, "--");
	} 

}
