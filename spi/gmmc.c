/*
	24 juli 06
	Implementasi MMC
	furkan jadid, jakarta, daun biru engineering
	
	25 juli 06,
	pada saat awal inisiasi, harus dibawah 400 kHz
	sesudah CMD1 sukses, dapat ditingkatkan lagi	
	test pada ssio max 8 MHz sukses
	
	pada saat power_up pertama kali, inisiasinya lama sekali?
	
	21 Nov 06, furkan jadid
	adaptasi untuk blackfin
	
	PF2 --> CS onboard boot flash
	PF1 --> CS MMC
	
	koman panjangnya selalu 6 byte dan MSB first
	pada saat tx, sekalian ngambil di rx
	
	kirim word ini harusnya kirim byte
	
	6 april 07, bekasi
	cid & csd diperbaiki
	
	14 mei 08, jakarta
	diporting ke gcc
	
	15 mei 08, dibuat library
	
	30 mei 08, disederhanakan
	baca2 multi dibuang untuk hemat tempat
	
	3 juni 08
	gunakan serial mmc untuk set mac
	
	6 juni 08
	test untuk LPC2368
	
	9 juni 08
	get_cid_csd disimplekan
	*/

#include "gmmc.h"
#include "FreeRTOS.h"

#define 	stop_trans			0xFD
#define 	BESAR_MMC_BLOK	512

//extern struct konfig_mesin kfg_mesin;
#include "../tinysh/enviro.h"
//extern struct t_env env2;
//struct t_env *env2;
//env2 = (char *) ALMT_EN
//#define kirim_word(c) kirim_word_mmc(c)

//get_cid_csd diambil dari kernel Uclinux
//=============================================

struct cid_str {
	unsigned int		manfid;
	char			prod_name[8];
	unsigned int		serial;
	unsigned short		oemid;
	unsigned short		year;
	unsigned char		hwrev;
	unsigned char		fwrev;
	unsigned char		month;
};

struct csd_str {				/* __csd field name__*/
	unsigned char		mmca_vsn;	/* CSD_STRUCTURE */
	unsigned short		cmdclass;	/* CCC */
	unsigned short		tacc_clks;	/* TAAC */
	unsigned int		tacc_ns;	/* NSAC */
	unsigned int		max_dtr;	/* TRANS_SPEED */
	unsigned int		read_blkbits;	/* READ_BL_LEN */
	unsigned int		capacity;
};

struct cid_str cid;
struct csd_str csd;

unsigned int get_sector_count(void)	{
	return csd.capacity;	
}

void copy_csd(void *str_gmmc)	{
	portENTER_CRITICAL();
	memcpy(str_gmmc, (char *) &csd, sizeof (csd));
	portEXIT_CRITICAL();
}

void copy_cid(void *str_gmmc)	{
	portENTER_CRITICAL();
	memcpy(str_gmmc, (char *) &cid, sizeof (cid));
	portEXIT_CRITICAL();
}
BOOL getbit(void* ptr, unsigned int n) {
	unsigned int byte_nr;
	unsigned int bit_nr;

	byte_nr = n/8;
	bit_nr = n % 8;

	return (BOOL)(((unsigned char*)ptr)[byte_nr] >> bit_nr) & 1;
}

unsigned int getvalue(void* ptr, unsigned int n, unsigned int len) {
	unsigned int value=0;
	int i=0;

	for(i=0;i<len; i++) {	
		value += ((unsigned int)getbit(ptr, n+i))<<i;
	}
	return value;
}

void mmc_error_out(void)
{
	printf("\n MMC error !!");	
}

#if 0
//=============================================
unsigned char get_cid_csd(void)
{
	unsigned char temp;
	unsigned char buf_mmc[18];	//tempat cid
	unsigned char raw_csd[18]; // 16 byte + 2 byte CRC
	
	unsigned short c_size_mult=0;
	unsigned short c_size=0;
	
	unsigned short read_bl_len=0;
	unsigned int cap = 0;
   	int t;
   
   	temp = mmc_respon2();
	cs_mmc();
		
	komand_mmc(10, 0x34, 0xff);
	temp = mmc_respon2();
	t=0;
		
	if (temp == 0)
	{
		temp = ambil_word_mmc();
		while (temp != 0xfe)
		{
				temp = ambil_word_mmc();
				t++;
				if (t > 8000) mmc_error_out(); 
				//serial_puts("Panik !! MMC CID tidak didapatkan !!");
		}
		for (t=0; t<18; t++)
		{
			buf_mmc[t] = ambil_word_mmc();
		}
		uncs_mmc();	
		
		env2.mmc_serial = getvalue(buf_mmc, 127-47, 32);		
	}

	cs_mmc();
	//ambil CSD		
	komand_mmc(9, 0x34, 0xff);
	temp = mmc_respon2();
	t=0;
		
	if (temp == 0)
	{
		temp = ambil_word_mmc();
		while (temp != 0xfe)
		{
				temp = ambil_word_mmc();
				t++;
				if (t > 8000) mmc_error_out();
				//serial_puts("Panik !! MMC CID tidak didapatkan !!");
		}
		for (t=0; t<18; t++)
		{
			raw_csd[t] = ambil_word_mmc();
		}
		
		uncs_mmc();
	
		// for calculating capacity(in blocks)
		c_size = ((((unsigned short)raw_csd[6]) & 0x03) << 10) | (((unsigned short)raw_csd[7]) << 2) | (((unsigned short)raw_csd[8]) & 0xc0) >> 6;
		c_size_mult = ((raw_csd[9] & 0x03) << 1) | ((raw_csd[10] & 0x80) >> 7);
		//csd.capacity = (c_size+1) * (1 << (c_size_mult + 2));
		env2.mmc_tot_sektor = (c_size+1) * (1 << (c_size_mult + 2));
	
		printf("\n MMC total sektor = %d", env2.mmc_tot_sektor);
		printf(", serial # %X", env2.mmc_serial); 
	}
	
	//kfg_mesin.mmc_serial = cid.serial;
	
	return 0;
	//sti(mask_d);
}
#endif

unsigned char get_cid_csd(void)
{
	unsigned char temp;
	unsigned char buf_mmc[18];	//tempat cid
	unsigned char raw_csd[18]; // 16 byte + 2 byte CRC
	
	unsigned short c_size_mult=0;
	unsigned short c_size=0;
	
	unsigned short read_bl_len=0;
	unsigned int cap = 0;
   	int t;
   	
   	
   	
   	temp = mmc_respon2();
	cs_mmc();
		
	komand_mmc(10, 0x34, 0xff);
	temp = mmc_respon2();
	t=0;
		
	if (temp == 0)	{
		temp = ambil_word_mmc();
		while (temp != 0xfe)	{
				portYIELD();
				temp = ambil_word_mmc();
				t++;
				//if (t > 8000) 
				if (t > 100) 
				{
					printf("\r\nERROR: MMC CID tidak didapatkan !!\r\n");
					uncs_mmc();
					return -1;
				}
		}
		for (t=0; t<18; t++)	{
			buf_mmc[t] = ambil_word_mmc();
		}
		uncs_mmc();	
		cid.manfid = getvalue(buf_mmc, 127-127, 8);
		memcpy(cid.prod_name, buf_mmc+3, 7);
		cid.serial = getvalue(buf_mmc, 127-47, 32);
		cid.oemid = getvalue(buf_mmc, 127-119, 16);
		cid.year = 1997 + (getvalue(buf_mmc, 127-15, 8) & 0x0F);
		cid.hwrev = (getvalue(buf_mmc, 127-55, 8) & 0xF0) >> 4;
		cid.fwrev = getvalue(buf_mmc, 127-55, 8) & 0x0F;
		cid.month = (getvalue(buf_mmc, 127-15, 8) & 0xF0) >> 4;	
		
	}

	cs_mmc();
	//ambil CSD		
	komand_mmc(9, 0x34, 0xff);
	temp = mmc_respon2();
	t=0;
		
	if (temp == 0)	{
		temp = ambil_word_mmc();
		while (temp != 0xfe)	{
				portYIELD();
				temp = ambil_word_mmc();
				t++;
				//if (t > 8000) 
				if (t > 100) 
				{
					printf("\r\nERROR: MMC CSD tidak didapatkan !!\r\n");
					uncs_mmc();
					return -1;
				}
		}
		for (t=0; t<18; t++)	{
			raw_csd[t] = ambil_word_mmc();
		}
	
	uncs_mmc();
	vTaskDelay(5);	
	csd.mmca_vsn = (raw_csd[0] & 0x3c) >> 2;
	csd.cmdclass = (((unsigned short)raw_csd[4]) << 4) | ((raw_csd[5] & 0xf0) >> 4);
	csd.tacc_clks = raw_csd[1];
	csd.tacc_ns = raw_csd[2];
	csd.max_dtr = raw_csd[3];
	csd.read_blkbits = raw_csd[5] & 0x0f;
	
	
	// for calculating capacity(in blocks)
	c_size = ((((unsigned short)raw_csd[6]) & 0x03) << 10) | (((unsigned short)raw_csd[7]) << 2) | (((unsigned short)raw_csd[8]) & 0xc0) >> 6;
	c_size_mult = ((raw_csd[9] & 0x03) << 1) | ((raw_csd[10] & 0x80) >> 7);
	csd.capacity = (c_size+1) * (1 << (c_size_mult + 2));
	vTaskDelay(5);
	printf("\n MMC total sektor = %d", csd.capacity);
	
	// for printing capacity in bytes
	read_bl_len = raw_csd[5] & 0x0f;	
	cap = (c_size+1) * (1 << (c_size_mult + 2)) * (1 << read_bl_len);
	
		
		printf("\n MMC: OK Size: %dM Name: %s Rev: %d.%d Date: %d/%d\r\n Serial: 0x%x (%u) Blk: %d Max: %d MB/s\r\n", \
			cap/(1024*1024), cid.prod_name, cid.hwrev, cid.fwrev, cid.year, cid.month, cid.serial, cid.serial, (unsigned int)pow(2, read_bl_len), csd.max_dtr);
		
		/*
		sprintf(tek," MMC : %dM : %s Rev: %d.%d : %d/%d", cap/(1024*1024), cid.prod_name, cid.hwrev, cid.fwrev, cid.year, cid.month);
		sprintf(tek, "Serial : 0x%x : %d : %d MHz", cid.serial, (unsigned int)pow(2, read_bl_len), csd.max_dtr);
		*/
	//if (cid.serial!=0)
	//		masuk=1;
			
	//	}
	
	}
	
	struct t_env *p_env;
	p_env = pvPortMalloc( sizeof (struct t_env) );
	memcpy((char *) p_env, (char *) ALMT_ENV, (sizeof (struct t_env)));
	
	//env2.mmc_serial = cid.serial;
	p_env->mmc_serial = cid.serial;
	uncs_mmc();
		
	
	return 0;
	//sti(mask_d);
}

unsigned char stop_mmc(void)
{
	unsigned char respone;
   	//stop transmisi data saat read multi blok atau write multiblok	
	cs_mmc();
	//ssync();
	komand_mmc(12, 0x00, 0x23);		//cmd12, STOP TRANSMISION	
	respone = mmc_respon2();
	if (respone == 0x00)	{
		uncs_mmc();
	   return 0;
	}
	else	{
		uncs_mmc();
	   return respone;
	}
}


void mmc_crc_off(void)
{
	cs_mmc();
	komand_mmc(59, 0x00, 0x01);
	while(mmc_respon2() != 0x00);
	uncs_mmc();
}

unsigned short mmc_status(void)
{
	unsigned char	resp;
   unsigned char	resp2;
   unsigned short stat;
	int count=0;
	
	cs_mmc();
	komand_mmc(13, 0x0000, 0xff);
	resp =  ambil_word_mmc();
	
	while((resp & 0x80) == 0x80)	{
		resp = ambil_word_mmc();
		count++;
		if (count > 500)	{
			uncs_mmc();
		   return resp; //error
		}
		//serial_puts("resp ok ");
	}	
	stat = resp;
	resp2 = ambil_word_mmc();
	uncs_mmc();
	stat = (stat << 8) + resp2;
	return stat; 
}


short tes_mmc_awal(void)	{
	short t;
   	short coba=1;
	char resp_mmc;
	
	printf("%s(): masuk ..\r\n", __FUNCTION__);
	//kirim dummy clock paling tidak 80 clock
   	uncs_mmc();
   	for (t=0; t<100; t++) kirim_word_mmc(0xFF);  	
   	printf("%s(): 2 ..\r\n", __FUNCTION__);
   	cs_mmc(); 
   	komand_mmc(0x00, 0x00, 0x95); 
  	while(1)
   	{
   		//for (t = 0; t < 20; t++)
   		for (t = 0; t < 20; t++)
   		{
   			resp_mmc = mmc_respon2();
   			if (resp_mmc == 0x01)
   			{
   				uncs_mmc();
   				return coba;
   			}
   		}
   		uncs_mmc();
   		for (t=0; t<100; t++) kirim_word_mmc(0xFF);
   		cs_mmc();
   		komand_mmc(0x00, 0x00, 0x95); 
		coba++;
		if (coba > 20) 	{
		   //kemungkinan MMC sedang error, harusnya di hardware reset (power ON/OFF)
		   //coba cek status dulu
		   
		   //printf(" .. coba %d\r\n", coba);
		   
		   uncs_mmc();
		   return 0;	//fail
		}
		printf("..%d", coba);
		if (coba == 10) printf("\r\n");
   	}
}

short init_mmc(void)	{
	short t;
   short coba=1;
	char resp_mmc;
   
   uncs_mmc();
   for (t=0; t<10; t++)
   {
   	kirim_word_mmc(0x55);
   }
   cs_mmc();  			
   //komand_mmc(0x01,0x00ffc000,0xff);
   komand_mmc(0x01,0x00,0xff);		
   while(1)
   {
   	for (t = 0; t < 200; t++)
   	{
   		resp_mmc = mmc_respon2();
   		if (resp_mmc == 0x00)
   		{
   			uncs_mmc();
   			return coba;
   		}
   	} 

   	//komand_mmc(0x01,0x00ffc000,0xff);
   	komand_mmc(0x01,0x00,0xff);	
		coba++;
		if (coba > 100) 
		{
		   uncs_mmc();
		   return 0;	//fail
		}
   }
}


unsigned char tulis_mmc_blok(unsigned int alamat, unsigned char *p)
{
	unsigned char sibuk;
	unsigned char respone;
	int i;
	
	cs_mmc();
	komand_mmc(24, alamat, 0xff);		//cmd24, write blok data	
	respone = mmc_respon2();
	if (respone == 0x00)
	{
		//jika sukses, kirim token dulu
		kirim_word_mmc(0xfe);
		for (i=0; i<512; i++)
   		{
   			kirim_word_mmc(p[i]);		
   		}
   	
   		respone = ambil_word_mmc();  	
   		respone = (respone & 0x0f);
   		while (respone != 0x05)
   		{
   			respone = ambil_word_mmc();  	
   			respone = (respone & 0x0f);
   		}
   	
   		sibuk = ambil_word_mmc();
   		while(sibuk == 0) 
   		{
   	   		sibuk = ambil_word_mmc();
   		}

   		if (respone == 0x05)
   		{
   			uncs_mmc();
   			return 0;	
   		}
   		else
   		{
   			uncs_mmc();  
   			//serial_puts("tidak bisa nulis ");	
   			return respone;
   		}		
	}
	else
	{
		uncs_mmc();
		//serial_puts("disini");
	   	return respone;
	}		
}

unsigned char baca_mmc_blok(unsigned int alamat, unsigned char *p)
{	     
   //0 jika sukses, lainnya berarti error,
	//unsigned char sibuk;
	unsigned char respone;
	int i=0;
	
	cs_mmc();
	komand_mmc(17, alamat, 0x23);		//cmd17, baca sak blok	
	respone = mmc_respon2();
	if (respone == 0x00)
	{
		respone = ambil_word_mmc();
		while (respone != 0xfe) 		//respon read single blok
		{
			respone = ambil_word_mmc();
			i++;
			if (i>8000) 
			{
				uncs_mmc();
			   return 25;		//berarti error
			}
		}
	
		for (i=0; i<512; i++)
		{
			p[i] = ambil_word_mmc();	
		}
		//CRC dummy
		ambil_word_mmc();
		ambil_word_mmc();
		
		uncs_mmc();				
		return 0;
	}
	else 
	{
		uncs_mmc();
		return respone;
	}
}

void komand_mmc(unsigned char cmd, unsigned int argument,unsigned char crc)
{
	//argument MSB dulu	
	unsigned char dt[6];
	short i;
	unsigned char temp;
	
	dt[0] = (cmd | 0x40);	
	for(i=3;i>=0;i--)
	{
    		temp=(unsigned char)(argument>>(8*i));
    		dt[4-i]=temp;
  	}
  	dt[5]=(crc | 0x01);
  	for(i=0;i<6;i++)
  	{
  		kirim_word_mmc(dt[i]);
  	}
}

unsigned char mmc_respon2(void)
{
	short i;
	unsigned char temp;
	
	for (i= 0; i<32; i++)
	{
		temp = ambil_word_mmc();
		if (temp == 0x01)
		{
		   return temp;	
		}
		if (temp == 0x00)
		{
			return temp;
		}	
	}
	return temp;
}


