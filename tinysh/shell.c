#include <stdio.h>
#include <errno.h>
#include <float.h>
#include <math.h>

#include "tinysh.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "rw_flash.c"
#include "set_ipaddr.c"

#include "../hardware/hardware.h"
#include "utils.c"

#define debug_printf printf

#ifdef PAKAI_SHELL

#if (VERSI_KONFIG == 2)

#include "group.c"
#include "sumber.c"
//#include "../monita/monita_kontrol_2.c"
#include "data_kontrol.c"
#include "sfile.c"
#endif

#ifdef PAKAI_FILE_SIMPAN
	#include "sfile.c"
#endif

#ifdef PAKAI_GPS
	#include "gps.c"
#endif

#ifdef PAKAI_TSC
	#include "tsc.c"
#endif

/*
#ifdef CARI_SUMBERNYA
	#include "utils.c"
	#include "group.c"
	#include "sumber.c"
#endif
//*/

#ifdef BOARD_TAMPILAN
	//#include "group.c"
#endif

#ifdef BANYAK_SUMBER
	#include "sumber.c"
	#include "data_kontrol.c"
#endif

#ifdef CENDOL
	#include "setting_eth.c"
#endif

//#ifdef BOARD_TAMPILAN
#ifdef CARI_SUMBER
#include "mesin.c"
#include "titik.c"
#endif

#ifdef PAKAI_ADC
	#include "../modul/adc/command_adc.c"
	#include "set_kanal.c"
#endif

#ifdef BOARD_KOMON_420_SAJA
#include "../adc/command_adc.c"
#include "set_kanal.c"
#include "utils.c"
#endif

#ifdef  BOARD_KOMON_420_SABANG_2_3
	#include "utils.c"
//	#define TXDE	BIT(24)
#endif 

#ifdef  BOARD_KOMON_420_SABANG
	#include "utils.c"
	#define TXDE	BIT(24)
#endif 

#ifdef BOARD_KOMON_A_RTD
#include "../adc/command_adc.c"
#include "set_kanal.c"
#endif

#ifdef BOARD_KOMON_B_THERMO
#include "../adc/command_adc.c"
#include "set_kanal.c"
#endif

#if defined(BOARD_KOMON_KONTER) || defined(BOARD_KOMON_KONTER_3_0)
//#include "GPIO/gpio.c"
#include "utils.c"
#include "set_kanal.c"
#include "rpm.c"

//int rtcRate[KANALNYA];	// 	__attribute__ ((section (".rtcram_rate")));
#endif

#ifdef PAKAI_PM
//#include "pm_server.c"
#include "sumber.c"
#endif

#ifdef PAKAI_MMC
int status_MMC=0;
#include "../fatfs/shell_fs.c"
#endif

#ifdef PAKAI_RTC
#include "../modul/system/rtc.h"
#endif

#ifdef PAKAI_GSM_FTP
#include "smodem.c"
#endif

#ifdef PAKAI_SMS
#include "../gsm/sms.c"
#endif

#ifdef PAKAI_CRON
#include "cron.c"
#endif

#ifdef PAKAI_SELENOID
#include "../GPIO/selenoid.c"
#endif

#ifdef PAKAI_MULTI_SERIAL
	#define TXDE	BIT(24)
#endif

#ifdef PAKAI_GPIO_DIMMER
	#include "dimmer.c"
#endif

#include "enviro.h"
//#include "GPIO/gpio.h"
//#include "../monita/monita_uip.h"
#include "monita/monita_uip.h"

#include <stdlib.h>

void reset_cpu(void);

//extern struct t_mesin mesin[];
//extern struct t_titik titik[];
//extern struct t_sumber sumber[];

extern struct sambungan_state samb;

#ifdef PAKAI_SHELL
extern xTaskHandle *hdl_shell;
#endif

extern xTaskHandle *hdl_lcd;

#ifdef PAKAI_LED_UTAMA 
	extern xTaskHandle *hdl_led;
#endif

#ifdef PAKAI_TAMPILAN
extern xTaskHandle *hdl_tampilan;
#endif

#ifdef PAKAI_ALARM
extern xTaskHandle *hdl_alarm;
#endif

#ifdef PAKAI_ETH
	extern xTaskHandle *hdl_ether;
	#include "uipcmd.c"
#endif
extern xTaskHandle *hdl_ambilcepat;

#ifdef PAKAI_GPS
	extern xTaskHandle *hdl_gps;
#endif

#ifdef BOARD_CNC 
	extern xTaskHandle *hdl_cnc;
#endif

#ifdef PAKAI_CRON
	//extern xTaskHandle *hdl_cron;
#endif

#ifdef PAKAI_GSM_FTP
	extern xTaskHandle *hdl_modem;
#endif
//extern struct t_env env2;



#ifdef PAKAI_SELENOID
	extern xTaskHandle *hdl_relay;
#endif

char str[20];

#if 1
//#ifdef PAKAI_SHELL

/*****************************************************************************/
// komand2 daun biru komon-kounter

#define per_oflow (0xFFFFFFFF * 0.050) / (1000 * 1000 * 60)
#define per_tik		0.050 / (1000 * 1000 * 60)

/*
static tinysh_cmd_t save_env_cmd={0,"save_env","menyimpan environment","[args]",
                              save_env,0,0,0};
//*/						  
static tinysh_cmd_t printenv_cmd={0,"cek_env","menampilkan environment","[args]",
                              print_env,0,0,0};
							  
static tinysh_cmd_t reset_cmd={0,"reset","reset cpu saja","[args]",
                              reset_cpu,0,0,0};
							  
//static tinysh_cmd_t defenv_cmd={0,"defenv","set default environment","[args]",
//                              getdef_env,0,0,0};

#ifdef PAKAI_MULTI_SERIAL
void kirim_serial (int argc, char **argv) {
	int sumb=0, i=0;
	int lope;
	unsigned char kirim_ser[100];
	// serial 2 AT\r\n
	if (argc < 3) {
		if (argc > 1)	{
			if (strcmp(argv[1], "help") == 0) {
				printf("serial 2 AT\r\n");
				return;
			}
		}
		printf(" ERR: argument kurang !\r\n");
		//printf(" coba set_group help \r\n");
		return;	
	}
	
	display_args(argc,argv);
	sprintf(kirim_ser, "%s", argv[1]);
	sumb = cek_nomer_valid(kirim_ser, 5);

	#ifdef PAKAI_SERIAL_1
	if (1 == sumb) {
		ganti_kata(kirim_ser, argv[2]);
		serX_putstring(1, kirim_ser);
		#ifdef PAKAI_GSM_FTP
		if	(PAKAI_GSM_FTP == 1)
			baca_hasil();			// fitur yang perlu jawaban
		#endif
		return;
	}
	#endif

	#ifdef PAKAI_SERIAL_2
	if (2 == sumb) {
		for (i=0; i<10; i++)
			ser2_getchar(1, &lope, 20 );
		
		ganti_kata(kirim_ser, argv[2]);
		serX_putstring(2, kirim_ser);
		//*
		#ifdef PAKAI_GSM_FTP
		if	(PAKAI_GSM_FTP==2)
			baca_hasil();			// fitur yang perlu jawaban
		#endif
		//*/
		return;
	}
	#endif
	
	#ifdef PAKAI_SERIAL_3
	if (3 == sumb) {
		ganti_kata(kirim_ser, argv[2]);
		
		#ifdef PAKAI_MAX485
		FIO0SET = TXDE;		// on	---> bisa kirim
		#endif
		serX_putstring(3, kirim_ser);
		
		#ifdef PAKAI_MAX485
		//FIO0SET &= ~TXDE;	// off	---> gak bisa kirim
		//FIO0CLR = TXDE;
		#endif
		
		#ifdef PAKAI_GSM_FTP
		if	(PAKAI_GSM_FTP==3)
			baca_hasil();			// fitur yang perlu jawaban
		#endif
		return;
	}
	#endif
	
}

static tinysh_cmd_t kirim_serial_cmd={0,"serial","mengirim string ke serial","[args]",
                              kirim_serial,0,0,0};
#endif


#ifdef PAKAI_RTC
void set_date(int argc, char **argv)
{	
	unsigned char *str_rtc;
	struct rtc_time tmku;
	int ret;
	time_t clk;	
	
	rtc_reset();
	rtc_init();
	rtc_start();
	
	str_rtc = pvPortMalloc(512);
	if (str_rtc == NULL) {
		printf("ERR: alok failed\r\n");
		//free(str_rtc);
		vPortFree(str_rtc);
		return;
	}
	
	//printf("dapat free %X\r\n", str_rtc);	
	memset(str_rtc, 0, 512);	
	
	printf(" set_date tahun bulan tanggal jam menit\r\n");
	printf("   misalnya : set_date 2010 3 5 10 22\r\n");
	printf("   artinya  : set waktu ke tgl 5 Maret 2010, jam 10:22 pagi\r\n");
	
	if (argc < 5) 	{
		printf("Argument kurang !\r\n");
		vPortFree(str_rtc);
		return;
	}
		
	//display_args(argc, argv);
	sprintf(str_rtc, "%s:%s:%s:%s:%s", argv[1], argv[2], argv[3], argv[4], argv[5]); 
	ret = sscanf(str_rtc, "%d:%d:%d:%d:%d", &tmku.tm_year, &tmku.tm_mon, &tmku.tm_mday, &tmku.tm_hour, &tmku.tm_min); 
	if (ret < 5)	{
		printf(" ERR: format salah !\r\n");
		vPortFree(str_rtc);
		return;
	}

	printf(" Set : %d-%d-%d %d:%d ",  tmku.tm_year, tmku.tm_mon, tmku.tm_mday, tmku.tm_hour, tmku.tm_min); 
		
	tmku.tm_year = tmku.tm_year - 1900;
	tmku.tm_mon  = tmku.tm_mon - 1;
	tmku.tm_sec = 0;
		
	debug_printf(" OK : %d-%d-%d %d:%d\r\n",  tmku.tm_year, tmku.tm_mon, tmku.tm_mday, tmku.tm_hour, tmku.tm_min); 
	/*
	if (rtc_valid_tm(&tmku)) 
	{
		printf(" ERR: waktu tidak mungkin !\r\n");
		return;
	}*/
	//rtc_set_time_tm( tmku );
	rtcWrite( &tmku );
	//clk = mktime(&tmku);	
	//ret = rtc_time_to_bfin(clk);
	//bfin_write_RTC_STAT(ret);
	vPortFree(str_rtc);
	printf(" ..OK\r\n");
}							 

static tinysh_cmd_t set_date_cmd={0,"set_date","Mengeset waktu","thn bulan tgl jam menit",
                              set_date,0,0,0};							 

#endif
							  
void cek_stack(void)
{
	printf("Sisa stack masing2 task (bytes)\r\n");
	garis_bawah();
	printf(     " Shell      : %d\r\n", uxTaskGetStackHighWaterMark(hdl_shell));
	printf(     " Led        : %d\r\n", uxTaskGetStackHighWaterMark(hdl_led));
	
	#ifdef BOARD_TAMPILAN
	//#ifdef CARI_SUMBERNYA
	printf(     " Tampilan   : %d\r\n", uxTaskGetStackHighWaterMark(hdl_tampilan));
	printf(     " LCD        : %d\r\n", uxTaskGetStackHighWaterMark(hdl_lcd));
	#endif
	
	#ifdef PAKAI_ALARM
	printf(     " ALarm      : %d\r\n", uxTaskGetStackHighWaterMark(hdl_alarm));
	#endif
	
	#if (TAMPILAN_MALINGPING == 1)
	//extern xTaskHandle hdl_proses_pm;  matikan dulu, pm belum diaktifkan
	
	printf(	   " Tampilan    : %d\r\n", uxTaskGetStackHighWaterMark(hdl_tampilan));
	printf(	   " LCD         : %d\r\n", uxTaskGetStackHighWaterMark(hdl_lcd));
	#endif
	
	#ifdef PAKAI_ETH
	printf(    " Ether      : %d\r\n", uxTaskGetStackHighWaterMark(hdl_ether));
	#endif
	
	#ifdef PAKAI_SELENOID
		printf(" Relay       : %d\r\n", uxTaskGetStackHighWaterMark(hdl_relay));
	#endif
		
	#if defined(PAKAI_GSM_FTP) || defined(PAKAI_SMS)
		printf(" GSM         : %d\r\n", uxTaskGetStackHighWaterMark(hdl_modem));
	#endif
	
	#ifdef PAKAI_CRON
		printf(" CRON        : %d\r\n", uxTaskGetStackHighWaterMark(hdl_cron));
	#endif
	
	#ifdef AMBIL_PM
//		printf(" Proses PM: %d\r\n", uxTaskGetStackHighWaterMark(hdl_proses_pm));
	#endif
	
	#ifdef BOARD_CNC
		printf(" CNC         : %d\r\n", uxTaskGetStackHighWaterMark(hdl_cnc));
	#endif
	
	printf(    " AmbilCepat : %d\r\n", uxTaskGetStackHighWaterMark(hdl_ambilcepat));
}							 

//static 
tinysh_cmd_t cek_stack_cmd={0,"cek_stack","data kounter/rpm","[args]", cek_stack,0,0,0};

void cek_versi(void)	{
	printf("\n %s v%s\r\n", NAMA_BOARD, VERSI_KOMON);
	
  	printf(" ARM-GCC %s : %s : %s\r\n", __VERSION__, __DATE__, __TIME__);
  	printf(" CPU = LPC 2387, %d MHz,", configCPU_CLOCK_HZ/1000000);
  	printf(" FreeRTOS 5.1.1\r\n");	
}							 

//static
tinysh_cmd_t version_cmd={0,"version","menampilkan versi firmware","[args]", cek_versi,0,0,0};
							  
unsigned int uptime_ovflow=0;
unsigned int tik_lama=0;

void uptime(unsigned int *sec, unsigned int *min, unsigned int *jam, unsigned int *hari, unsigned int *thn)	{
	unsigned int detik;
	unsigned int tik;
	
	tik = xTaskGetTickCount();
	if (tik < tik_lama)
			uptime_ovflow++;
	
	tik_lama = tik;
	
	detik = ((uptime_ovflow * 0xFFFFFFFF) + tik)/1000;	// port tick HZ
	
	*sec = detik % 60;
	*min = detik / 60;
	if (*min > 60) 
		*min = *min % 60;
	
	*jam = detik / 3600;
	if (*jam >= 24)
		*jam = *jam % 24;
	
	*hari = detik / 86400;
	if (*hari >= 365)
		*hari = *hari % 365;
	
	*thn = detik / (8640 * 365);
	
}

//static 
void cek_uptime(int argc, char **argv)	{
	unsigned int sec;
	unsigned int menit;
	unsigned int jam;
	unsigned int hari;
	unsigned int tahun;
	
	extern unsigned int tot_idle;
	
	uptime(&sec, &menit, &jam, &hari, &tahun);
	printf(" Up  = ");
	if (tahun !=0)
	{
		printf("%d thn ", tahun);	
	}
	if (hari !=0)
	{
		printf("%d hari ", hari);		
	}
	if (jam !=0)
	{
		printf("%d jam ", jam);		
	}
	if (menit !=0)
	{
		printf("%d mnt ", menit);		
	}
		
	printf("%d dtk : idle = %d\n", sec, tot_idle);
	
	#ifdef PAKAI_RTC
	RTCTime wt;
	
	wt = rtc_get_time();
	printf(" Now = %02d:%02d:%02d  %d-%d-%d\r\n", wt.RTC_Hour, wt.RTC_Min, wt.RTC_Sec, \
			wt.RTC_Mday, wt.RTC_Mon, wt.RTC_Year);
	#endif
		
	return ;
}

//static
tinysh_cmd_t uptime_cmd={0,"uptime","lama running","[args]",  cek_uptime,0,0,0};

#if defined(BOARD_KOMON_420_SAJA)
void hitung_datanya() {
	#ifdef PAKAI_ADC
	struct t_env *env2;
	env2 = (char *) ALMT_ENV;
	float temp;
	int fx=0;
	for (fx=0; fx<KANALNYA; fx++)	{
		if (env2->kalib[fx].status==0) {	// "Tegangan"
			temp = st_adc.data[fx] * faktor_pengali_420 / 0xffff;
			data_f[fx] = (float) (temp * env2->kalib[fx].m) + env2->kalib[fx].C;
		} else {
			if (st_adc.data[fx]>10000)
				data_f[fx] = 1;		// "On/Tertutup"
			else
				data_f[fx] = 0;		// "Off/Terbuka"
		}
	}
	#endif
}

void lihat_datanya() {
	int dd=0;
	struct t_env *env2;
	env2 = (char *) ALMT_ENV;

	printf("No       Nilai    Satuan   Status\r\n");
	garis_bawah2();
	for (dd=0; dd<KANALNYA; dd++) {
		if (env2->kalib[dd].status==0) {
			printf("(%2d) %11.2f   (V)     Tegangan\r\n", dd+1, data_f[dd]);
		} else {
			printf("(%2d) %-12s          OnOff\r\n", dd+1, ((int) data_f[dd]==1)?"On/Tertutup":"Off/Terbuka");
		}
	}
}

static tinysh_cmd_t lihat_data_cmd={0,"cek_data","data ","[args]",
                              lihat_datanya,0,0,0};


#endif

#if defined(BOARD_KOMON_420_SABANG) || defined(BOARD_KOMON_420_SABANG_2_3)
void hitung_datanya() {
	#ifdef PAKAI_ADC
	struct t_env *env2;
	env2 = (char *) ALMT_ENV;
	float temp;
	int fx=0;
	for (fx=0; fx<KANALNYA; fx++)	{
		if (env2->kalib[fx].status==0) {	// "Tegangan"
			temp = st_adc.data[fx] * faktor_pengali_420 / 0xffff;
			data_f[fx] = (float) (temp * env2->kalib[fx].m) + env2->kalib[fx].C;
		} else {
			if (st_adc.data[fx]>10000)
				data_f[fx] = 1;		// "On/Tertutup"
			else
				data_f[fx] = 0;		// "Off/Terbuka"
		}
	}
	#endif
}

void lihat_datanya() {
	int dd=0;
	struct t_env *env2;
	env2 = (char *) ALMT_ENV;

	printf("No       Nilai    Satuan   Status\r\n");
	garis_bawah2();
	for (dd=0; dd<KANALNYA; dd++) {
		if (env2->kalib[dd].status==0) {
			printf("(%2d) %11.2f   (V)     Tegangan\r\n", dd+1, data_f[dd]);
		} else {
			printf("(%2d) %-12s          OnOff\r\n", dd+1, ((int) data_f[dd]==1)?"On/Tertutup":"Off/Terbuka");
		}
	}
}

static tinysh_cmd_t lihat_data_cmd={0,"cek_data","data ","[args]",
                              lihat_datanya,0,0,0};


#endif


#if defined(BOARD_KOMON_KONTER) || defined(BOARD_KOMON_KONTER_3_0)

/*		
unsigned int is_angka(float a)	{
	return (a == a);
}
//*/


#endif
/*****************************************************************************/
 
void display_args(int argc, char **argv)	{
  int i;
  for(i=0;i<argc;i++)
    {
      printf(" argv[%d]=\"%s\"\n",i,argv[i]);
    }
}

#ifdef CONTOH_SHELL
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
#endif // contoh shell

#ifdef PAKE_TELNETD
//static
void matikan_telnet(int argc, char **argv)	{
	printf("Quit telnet !\r\n");
	telnetdDisconnect();
}

//static
tinysh_cmd_t matikan_telnet_cmd={0,"quit","keluar dari telnet","[args]",
                              matikan_telnet,0,0,0};

/* 	
	dummy command untuk semacam password telnet
	jadi, perintah pertama saat telnet adalah harus komand password
	misalnya komand pertama harus "diesel", maka akan bisa akses
	jika tidak 
	*/	

#endif


#if (PAKAI_GSM_FTP == 1)
/* GSM FTP */
int gsm_ftp(int argc, char *argv[]);
extern int saat_gsm_ftp;

void mulai_gsm_ftp(void)
{
	if (saat_gsm_ftp == 0)
	{
		saat_gsm_ftp = 1;
	}
	else
		printf(" TASK gsm_ftp masih bekerja !!\r\n");
		
	return;
}

void gsm_ftp_exe() {
	saat_gsm_aksi=1;
	vTaskDelay(200);
	
}

static tinysh_cmd_t gsm_ftp_cmd={0,"gsm_ftp_exe","proses gsm_ftp","[args]",
                              mulai_gsm_ftp,0,0,0};
#endif

#ifdef PAKAI_ADC
	unsigned char status_adcnya=0; 
#endif 

extern int usb_terup;
portTASK_FUNCTION(shell, pvParameters )
{
  	int c;
  	xTaskHandle xHandle;
  	printf("\n%s v%s\r\n", NAMA_BOARD, VERSI_KOMON);

  	printf("Daun Biru Engineering, Des 2008\r\n");
  	printf("=========================================\r\n");
  	printf("ARM-GCC %s : %s : %s\r\n", __VERSION__, __DATE__, __TIME__);
  	printf("CPU = LPC 2387, %d MHz,", configCPU_CLOCK_HZ/1000000);
  	printf(" FreeRTOS 5.1.1\r\n");

	if (configUSE_PREEMPTION == 0)
		printf("NON Preemptive kernel digunakan !\r\n"); 
	else
		printf("Preemptive kernel digunakan !\r\n");
	
	#ifdef USB_TEST
	Host_Init();               /* Initialize the lpc2468 host controller                                    */
    //c = Host_EnumDev();       /* Enumerate the device connected */     
   	// if (c == 0) printf("Ketemu !\r\n");  
   	
   	#if 1
   	if (OHCIInit() == 0)
   	{
   		printf("------------ Init error \r\n");
   	}
   	
   	//install_usb_interrupt();
   	#endif
	#endif
	
	/* 
	 * add command
	 */
  	//tinysh_add_command(&myfoocmd);
  	tinysh_add_command(&printenv_cmd);
	tinysh_add_command(&setenv_cmd);
//	tinysh_add_command(&save_env_cmd);
	tinysh_add_command(&reset_cmd);
	tinysh_add_command(&cek_stack_cmd);
	tinysh_add_command(&uptime_cmd);
	tinysh_add_command(&version_cmd);
	
#if defined(BOARD_KOMON_KONTER) || defined(BOARD_KOMON_KONTER_3_0)
	tinysh_add_command(&cek_rpm_cmd);
	tinysh_add_command(&set_kanal_cmd);
#endif

//#ifdef BOARD_TAMPILAN
#ifdef CARI_SUMBER
	tinysh_add_command(&cek_sumber_cmd);
	tinysh_add_command(&set_sumber_cmd);
	//tinysh_add_command(&cek_mesin_cmd);
	//tinysh_add_command(&set_mesin_cmd);
	//tinysh_add_command(&cek_titik_cmd);
	//tinysh_add_command(&set_titik_cmd);
	
	tinysh_add_command(&save_sumber_cmd);
	//tinysh_add_command(&save_mesin_cmd);
	//tinysh_add_command(&save_titik_cmd);
#endif


#ifdef BOARD_KOMON_A_RTD
	tinysh_add_command(&cek_adc_cmd);
	tinysh_add_command(&set_kanal_cmd);
#endif

#ifdef BOARD_KOMON_420_SAJA
#ifdef PAKAI_ADC
	tinysh_add_command(&cek_adc_cmd);
	tinysh_add_command(&lihat_data_cmd);
#endif
	tinysh_add_command(&set_kanal_cmd);
#endif

#if defined(BOARD_KOMON_420_SABANG) || defined(BOARD_KOMON_420_SABANG_2_3)
#ifdef PAKAI_ADC
	tinysh_add_command(&cek_adc_cmd);
	//tinysh_add_command(&lihat_data_cmd);

	tinysh_add_command(&set_kanal_cmd);
#endif
#endif

#ifdef BOARD_KOMON_B_THERMO
	tinysh_add_command(&cek_adc_cmd);
	tinysh_add_command(&set_kanal_cmd);
#endif

#ifdef PAKE_TELNETD
	tinysh_add_command(&matikan_telnet_cmd);
#endif

#ifdef PAKAI_PM
//	tinysh_add_command(&cek_pm_cmd);
//	tinysh_add_command(&set_pm_cmd);
//	tinysh_add_command(&cek_konfig_pmnya_cmd);
#endif	
	
#ifdef PAKAI_GSM_FTP
	tinysh_add_command(&set_modem_ftp_cmd);
	tinysh_add_command(&cek_modem_cmd);
	tinysh_add_command(&set_modem_gsm_cmd);
	tinysh_add_command(&gsm_ftp_cmd);
	tinysh_add_command(&cek_ftp_cmd);
	
#endif	

#ifdef PAKAI_SMS
	tinysh_add_command(&cek_pulsa_cmd);
	tinysh_add_command(&kirim_sms_cmd);
	tinysh_add_command(&hapus_sms_cmd);
	tinysh_add_command(&baca_sms_cmd);
	tinysh_add_command(&baca_sms_semua_cmd);
	tinysh_add_command(&sms_monita_cmd);
#endif 

#ifdef PAKAI_CRON
	tinysh_add_command(&set_cron_cmd);
	tinysh_add_command(&cek_cron_cmd);
#endif

//#ifdef CARI_SUMBERNYA
#ifdef BOARD_TAMPILAN
	tinysh_add_command(&cek_group_cmd);
	tinysh_add_command(&set_group_cmd);
	
	//tinysh_add_command(&cek_sumber_cmd);
	//tinysh_add_command(&set_sumber_cmd);
	
	// data
	//tinysh_add_command(&set_data_cmd);
	//tinysh_add_command(&cek_data_cmd);
	//printf("board_tampilan\r\n");

#endif
vTaskDelay(100);
#ifdef BANYAK_SUMBER
	tinysh_add_command(&cek_sumber_cmd);
	tinysh_add_command(&set_sumber_cmd);
	tinysh_add_command(&set_data_cmd);
	tinysh_add_command(&cek_data_cmd);
	//printf("banyak sumber\r\n");
#endif
vTaskDelay(100);

#if (VERSI_KONFIGx == 2)
	tinysh_add_command(&cek_group_cmd);
	tinysh_add_command(&set_group_cmd);
	
	tinysh_add_command(&cek_sumber_cmd);
	tinysh_add_command(&set_sumber_cmd);
	
	// data
	tinysh_add_command(&set_data_cmd);
	tinysh_add_command(&cek_data_cmd);
#endif	

// simpan file
#ifdef PAKAI_FILE_SIMPAN	
	tinysh_add_command(&cek_file_cmd);
	tinysh_add_command(&set_file_cmd);
	tinysh_add_command(&del_direktori_cmd);
	tinysh_add_command(&cari_doku_cmd);
	tinysh_add_command(&hapus_filenya_cmd);
#endif

#ifdef PAKAI_ETH
	tinysh_add_command(&cek_mac_cmd);
#endif

#ifdef CENDOL
	tinysh_add_command(&cek_konfig_cmd);
	tinysh_add_command(&set_konfig_cmd);
#endif

#ifdef PAKAI_MULTI_SERIAL
	#if defined(PAKAI_SERIAL_1) || defined(PAKAI_SERIAL_2) || defined(PAKAI_SERIAL_3)
		tinysh_add_command(&kirim_serial_cmd);
	#endif
#endif

#ifdef PAKAI_GPS
	tinysh_add_command(&cek_gps_cmd);
#endif

#ifdef PAKAI_TSC
	tinysh_add_command(&setting_fma_cmd);
	tinysh_add_command(&set_fma_cmd);
	tinysh_add_command(&read_fma_cmd);
	tinysh_add_command(&baca_register_tsc_cmd);
#endif

#ifdef PAKAI_GPIO_DIMMER
	tinysh_add_command(&set_dimmer_cmd);
	tinysh_add_command(&cek_dimmer_cmd);
#endif

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

	#ifdef PAKAI_MMC
  	vTaskDelay(340);
  	#else
  	vTaskDelay(50);
  	#endif

	#ifdef PAKAI_SELENOID
		static tinysh_cmd_t set_relay_cmd={0,"set_relay","setting relay", "help default ",set_relay,0,0,0};
		tinysh_add_command(&set_relay_cmd);
	#endif
	
	
//#ifdef BOARD_TAMPILAN	
	#ifdef CARI_SUMBERx	
	// cek ukuran struk
	printf("size struct Mesin  = %d\r\n", sizeof (struct t_mesin) * JML_MESIN);
	printf("size struct Sumber = %d\r\n", sizeof (struct t_sumber) * JML_SUMBER);
	printf("size struct Titik  = %d\r\n", sizeof (struct t_titik) * JML_MESIN * TIAP_MESIN);
	//printf("size struct sambungan = %d\r\n", sizeof (samb));
	#endif

	#ifdef PAKAI_ADC	
		#ifdef BOARD_KOMON_420_SAJA
			kalibrasi_adc1();
			vTaskDelay(100);
			start_adc_1();
		#endif
		
		#if defined(BOARD_KOMON_420_SABANG) || defined(BOARD_KOMON_420_SABANG_2_3)
			//status_adcnya = kalib_adc();
			status_adcnya = kalibrasi_adc1();
			vTaskDelay(100);
			if (status_adcnya)
				start_adc_1();
			else {
				printf("gak start adc 1\r\n");
			}
		#endif
		
		#ifdef BOARD_KOMON_A_RTD
			kalibrasi_adc1();
			vTaskDelay(100);
			start_adc_1();
		#endif
	
		#ifdef BOARD_KOMON_B_THERMO
			kalibrasi_adc1();
			vTaskDelay(100);
			start_adc_1();
		#endif
	#endif
	
	#if defined(BOARD_KOMON_KONTER) || defined(BOARD_KOMON_KONTER_3_0)
		#ifdef PAKAI_RTC
		rtc_init();
		vTaskDelay(100);
		//baca_rtc_mem();
		#endif
	#endif
	
	#ifdef PAKAI_MMC
	tinysh_add_command(&util_ls_cmd);
	tinysh_add_command(&util_mkdir_cmd);
	tinysh_add_command(&util_cd_cmd);
	tinysh_add_command(&util_pwd_cmd);
	tinysh_add_command(&util_view_cmd);
	
	init_gpio_mmc();
	uncs_mmc();
	vTaskDelay(5);
	set_fs_mount();
	vTaskDelay(5);
	status_MMC = cek_fs_free();
	
	struct t_simpan_file *ts;
	ts = (char *) ALMT_SFILE;
	vTaskDelay(5);
	
	if (status_MMC) {
		printf("_____MMC ERROR !!!!_____ %d\r\n", status_MMC);
		if (status_MMC==13) 
			printf("_____MMC FR_NO_FILESYSTEM, kudu diformat FAT32 !!!\r\n");
		
		if (ts->set==1) {
			set_file_mati();
			printf("simpan data ke file dimatikan !!\r\n");
		}
	} else {
		status_MMC = 1;
		printf("MMC aktif. Siap simpan data: %d\r\n", status_MMC);
	}
	/*
	FR_NOT_READY,		3 
	FR_NO_FILE,			4 
	FR_NO_PATH,			5 
	FR_INVALID_NAME,	6 
	FR_DENIED,			7 
	FR_EXIST,			8 
	FR_INVALID_OBJECT,	9 
	FR_WRITE_PROTECTED,	10
	FR_INVALID_DRIVE,	11
	FR_NOT_ENABLED,		12
	FR_NO_FILESYSTEM,	13
	FR_MKFS_ABORTED,	14
	FR_TIMEOUT			15
	//*/
	
	
	sprintf(abs_path, "%s", "");
	#endif
	
	#ifdef PAKAI_RTC	
	tinysh_add_command(&set_date_cmd);
	#endif
	
	vTaskDelay(1000);
	
	struct t_env *envx;
	envx = (char *) ALMT_ENV;

	sprintf(str,"%s%d$ ", PROMPT, (envx->IP3));
	tinysh_set_prompt(str);
	//tinysh_set_prompt( PROMPT );
	/* force untuk tampil prompt */
	tinysh_char_in('\r');
	int perdetiknya=0;

	/*
	printf("nChr: %d, nInt: %d, nLInt: %d, nFloat: %d, nDouble: %d\r\n", \
		sizeof(char), sizeof(int), sizeof(long int), sizeof(float), sizeof(double));
	printf("nChr: %d, nInt: %d, nLInt: %d, nFloat: %d, nDouble: %d\r\n", \
		255, 20000000, 20000000, 20000000, 20000000);
	//*/
	/*
	 * main loop shell
  	 */
  	int lop = 0;
  	while(1)
    {
		vTaskDelay(1);
	  lop++;
	  if (xSerialGetChar(1, &c, 1000 ) == pdTRUE)
	  {
			lop = 0;
			tinysh_char_in((unsigned char)c);
	  }	
	  
	  /* dilindungi password setiap menit tidak ada aktifitas*/
	  if (lop > 60)
	  {
			lop = 0;
			printf("\r\nPasswd lock!\r\n");
			while(1)
			{
				if (xSerialGetChar(1, &c, 1000) == pdTRUE)	{
					if (proses_passwd( &c ) == 1) break;
				}
				#ifdef PAKAI_MMC
					#ifdef PAKAI_FILE_SIMPAN
						perdetiknya++;
						if (perdetiknya==10 && status_MMC==1) {
							proses_simpan_file();
							perdetiknya=0;
						}
					#endif
				#endif

			}
	  }
		
		#if defined(BOARD_KOMON_KONTER) || defined(BOARD_KOMON_KONTER_3_0)
		//	data_frek_rpm();
		#endif
	  
		#ifdef PAKAI_MMC
			#ifdef PAKAI_FILE_SIMPAN
				perdetiknya++;
				if (perdetiknya==10 && status_MMC==1) {
					proses_simpan_file();
					perdetiknya=0;
				}
			#endif
		#endif
	  
	  #ifdef USB_TEST
	  c = HC_INT_STAT ;
	  {
	  	printf("%4d: usb stat 0x%X\r\n", lop, c);
	  	
	  	HC_INT_STAT |= c;
	  	
	  	usb_terup = 0;
	  }
	  #endif
    }
  	
  	return;
}

void init_shell(void)	{
	//xTaskCreate( shell, "UsrTsk1", (configMINIMAL_STACK_SIZE * 6), 
	xTaskCreate( shell, "UsrTsk1", (configMINIMAL_STACK_SIZE * 10), NULL, tskIDLE_PRIORITY+2, ( xTaskHandle * ) &hdl_shell);
}
#endif

#endif
