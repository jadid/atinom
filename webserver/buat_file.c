/*
	buat file2 untuk dipakai oleh
	web server
	
	20 april 2009
	furkan jadid, daun biru engineering
	
	9 Mar 2010
	Adaptasi untuk model web server baru
	
	*/

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


#include "../tinysh/enviro.h"
#include "../adc/ad7708.h"

#ifdef CENDOL
//	#include "../tinysh/setting_eth.c" 
#endif

//extern struct t_env *env2;
//env2 = (char *) ALMT_ENV;
#define BESAR_BUF_HTTP	8192
unsigned char head_buf[512] 				; /*__attribute__ ((section (".eth_test"))); */
unsigned char tot_buf[BESAR_BUF_HTTP] 		__attribute__ ((section (".index_text")));
char ket[30];
//#define		tot_buf	buffer


//*
extern xTaskHandle *hdl_shell;
extern xTaskHandle *hdl_lcd;
extern xTaskHandle *hdl_led;
extern xTaskHandle *hdl_tampilan;
extern xTaskHandle *hdl_ether;
//*/
extern struct t_adc st_adc;

static unsigned int nomer_mesin=0;

#ifdef BOARD_KOMON_420_SAJA
#define BOARD_KOMON_WEB
#endif

#ifdef BOARD_KOMON_A_RTD
#define BOARD_KOMON_WEB
#endif

#ifdef BOARD_KOMON_B_THERMO
#define BOARD_KOMON_WEB
#endif

#ifdef BOARD_KOMON_KONTER
#define BOARD_KOMON_WEB
#endif

#ifdef BOARD_KOMON_420_SABANG
#define BOARD_KOMON_WEB
#endif

#define judul	"<html>\n<head>\n<title>Simple Monita Web Server</title>\n"

//#ifdef BOARD_TAMPILAN
#ifdef CARI_SUMBER
#define LINK_ATAS "<table border=""0"" align=""left"">\n \
  <tbody align=""center"">\n \
	<tr>\n \
      <td bgcolor=""lightGray"" width=""200""><a href=""index.html"">Data</a></td>\n \
      <td bgcolor=""lightGray"" width=""200""><a href=""set_satu.html"">Setting 1-50</a></td>\n \
	  <td bgcolor=""lightGray"" width=""200""><a href=""set_dua.html"">Setting 51-100</a></td>\n \
      <td bgcolor=""lightGray"" width=""200""><a href=""sumber.html"">Sumber Data</a></td>\n \
      <td bgcolor=""lightGray"" width=""200""><a href=""about.html"">About</a></td>\n \
    </tr>\n \
  </tbody>\n \
</table>\n"

#define LINK_BAWAH "<table border=""0"" align=""left"">\n \
  <tbody align=""center"">\n \
	<tr>\n \
      <td bgcolor=""lightGray"" width=""200"">.</td>\n \
      <td bgcolor=""lightGray"" width=""200"">.</td>\n \
      <td bgcolor=""lightGray"" width=""200"">.</td>\n \
      <td bgcolor=""lightGray"" width=""200"">.</td>\n \
      <td bgcolor=""lightGray"" width=""200"">.</td>\n \
    </tr>\n \
  </tbody>\n \
</table>\n"
#endif

#ifdef BOARD_KOMON_WEB

#define LINK_ATAS "<table border=""0"" align=""left"">\n \
  <tbody align=""center"">\n \
	<tr>\n \
      <td bgcolor=""lightGray"" width=""200""><a href=""index.html"">Data</a></td>\n \
      <td bgcolor=""lightGray"" width=""200""><a href=""setting.html"">Setting</a></td>\n \
      <td bgcolor=""lightGray"" width=""200""><a href=""about.html"">About</a></td>\n \
    </tr>\n \
  </tbody>\n \
</table>\n"

#define LINK_BAWAH "<table border=""0"" align=""left"">\n \
  <tbody align=""center"">\n \
	<tr>\n \
      <td bgcolor=""lightGray"" width=""200"">.</td>\n \
      <td bgcolor=""lightGray"" width=""200"">.</td>\n \
      <td bgcolor=""lightGray"" width=""200"">.</td>\n \
    </tr>\n \
  </tbody>\n \
</table>\n"

#endif

void buat_head(unsigned int flag) {
	struct t_env *env2;
	env2 = (char *) ALMT_ENV;
	
	char head_buf[512];
	
	/* flag = 1, perlu di refresh */
	if (flag == 1)
		sprintf(head_buf, "%s<meta http-equiv=""refresh"" content=""3"">\n</head>\n<body>\n\n<h1>Monita Online Monitoring System</h1>\n", judul);
	else if (flag == 0)
		sprintf(head_buf, "%s\n</head>\n<body>\n\n<h1>Monita Online Monitoring System</h1>\n", judul);
	
	sprintf(tot_buf, "%s", head_buf);
	
	sprintf(head_buf, "%s<br>", LINK_ATAS);
	strcat(tot_buf, head_buf);

	strcat(tot_buf, "\n<h4>Daun Biru Engineering");
	sprintf(head_buf, "<br>Monita %s v %s</h4>\n", NAMA_BOARD, VERSI_KOMON);
	strcat(tot_buf, head_buf);

	sprintf(head_buf, "<h4><p>Nama Modul : %s</h4>", env2->nama_board);
	strcat(tot_buf, head_buf);
	sprintf(head_buf, "<h4><p>Alamat IP  : %d.%d.%d.%d</h4>", env2->IP0, env2->IP1, env2->IP2, env2->IP3);
	strcat(tot_buf, head_buf);
}

void buat_bottom(void) {
	char head_buf[256];
	
	unsigned int sec;
	unsigned int menit;
	unsigned int jam;
	unsigned int hari;
	unsigned int tahun;
	
	/* data uptime */
	uptime(&sec, &menit, &jam, &hari, &tahun);
	strcat(tot_buf, "<h4>Uptime = ");
	if (tahun !=0)	{
		sprintf(head_buf, "%d thn ", tahun);
		strcat(tot_buf, head_buf);	
	}
	if (hari !=0) 	{
		sprintf(head_buf, "%d hari ", hari);
		strcat(tot_buf, head_buf);		
	}
	if (jam !=0)	{
		sprintf(head_buf, "%d jam ", jam);
		strcat(tot_buf, head_buf);		
	}
	if (menit !=0) 	{
		sprintf(head_buf, "%d mnt ", menit);
		strcat(tot_buf, head_buf);		
	}
		
	sprintf(head_buf, "%d dtk</h4>\n", sec);
	strcat(tot_buf, head_buf);		
	
	sprintf(head_buf, "%s<br>", LINK_BAWAH);
	strcat(tot_buf, head_buf);
	
	/* sprintf(head_buf,"<hr>\n<h5>ARM-GCC %s : %s : %s\n", __VERSION__, __DATE__, __TIME__); */
	sprintf(head_buf,"<h4>ARM-GCC %s : %s : %s\n", __VERSION__, __DATE__, __TIME__);
	strcat(tot_buf, head_buf);
	strcat(tot_buf, "<br>NXP LPC2387, 60 MHz, FreeRTOS 5.1.1</h4>\n");
	
	
	// close html
    sprintf(head_buf, "\n</body>\n</html>\n");
    strcat(tot_buf, head_buf);
}

#define DIGANTI 1

//int ganti(char* input);

int ganti_karakter(char *dest, char *src) {
	char *asli[] = {"+"};
	char *pengganti[] = {" "};
	char * pch;

	int i=0, j=0, k=0;
	for (j=0; src[j] != '\0'; ++j) {
		dest[i] = src[j];

		for (k=0; k<DIGANTI; k++) {
			pch = strstr (dest,asli[k]);
			
			if (pch) {
				strncpy (pch,pengganti[k],strlen(pengganti[k]));
				strcat(dest, pengganti[k]);
				i += strlen(pengganti[k])-strlen(asli[k]);
			}
		}
		++i;
	}
	dest[i] = '\0';
}

void ganti_setting(char *str) {
	printf("Data telah diubah: %s\r\n", str);
	char tmp[30], kets[30];
	
	int nk=0, no=0, titik=0;
	char * pch, *pch2, *ids, *ket, *stat;
	pch=strchr(str,'?');

	while(pch!=NULL) {
		pch2 = pch;
		pch2=strchr(pch2+1, '&');
		
		if ( (pch!=NULL)&&(pch2==NULL) ) {
			pch2=strchr(pch, '\0');
		}
		nk = pch2-pch-1;
		
		strncpy(tmp, pch+1, nk);
		tmp[nk]='\0';
		
		// Parsing ke tiap variabel ID, status, Ket yg bersesuaian 
		if (strncmp(tmp, "i", 1)==0) {
			ids = strchr(tmp,'i');
			no = atoi(ids+1);
			//printf("  no: %d\r\n", no);
			
			ids = strchr(tmp,'=');
			titik = atoi(ids+1);
			//printf("  id: %d\r\n", titik);
		} 
		if (strncmp(tmp, "k", 1)==0) {
			ket = strchr(tmp,'=');
			sprintf(kets, ket+1);
			//printf("  ket: %s, kets: %s\r\n", ket+1, kets);
		}
		if (strncmp(tmp, "s", 1)==0) {
			stat = strchr(tmp,'=')+1;
			//printf("  stat: %s, %d\r\n", stat);
		}
		pch=strchr(pch+1, '&');
	}
	
	int jmlData = (sizeof(data_f)/sizeof(float));
	struct t_setting *p_sbr;
	//printf("Jml Data : %d\r\n", jmlData);
	p_sbr = pvPortMalloc( jmlData * sizeof (struct t_setting) );
	
	if (p_sbr == NULL)
	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		return;	// -1
	}
	//printf(" %s(): Mallok ok di %X\r\n", __FUNCTION__, p_sbr);

	portENTER_CRITICAL();
	memcpy((char *) p_sbr, (char *) ALMT_KONFIG, (jmlData * sizeof (struct t_setting)));
	portEXIT_CRITICAL();
	
	p_sbr[no-1].id = titik;
	sprintf(p_sbr[no-1].ket, kets);
	p_sbr[no-1].status = (atoi(stat))?1:0;
	//printf("Isi kanal:%d, Titik: %d, Ket: %s, Ket2: %s, Status: %s\r\n", no, p_sbr[no-1].id, p_sbr[no-1].ket, kets, (p_sbr[no-1].status)?"aktif":"mati");
	
	if (simpan_konfig( p_sbr ) < 0)
	{
		vPortFree( p_sbr );
	}
	vPortFree( p_sbr );
}



#if 0
void buat_file_index(void) {
	
	buat_head(0);
	
	
	buat_bottom();
	printf("%s", tot_buf);
	/*
	#if 1
	char head_bufs[256];
	sprintf(tot_buf, "%s", judul);
	sprintf(head_bufs, "</head><body><h1>Tes</h1></body>\n", judul);
	strcat(tot_buf, head_bufs);
	printf("%s", tot_buf);
	#endif
	//*/
	//buat_bottom();
}
#endif

#if 1
void buat_file_index(void) {
	/* 
		data masing2 mesin secara bergiliran ditampilkan
		setiap refresh index.html		
	*/
		
	int i;
	unsigned int cek_mesin;
	float fl;
	float temp_rpm;
	struct t_env *env2;
	env2 = (char *) ALMT_ENV;
	
	buat_head(1);
						
//#ifdef BOARD_TAMPILAN
#ifdef CARI_SUMBER
	nomer_mesin++;
	if (nomer_mesin == 5) nomer_mesin = 0;
	
	cek_mesin = TIAP_MESIN * nomer_mesin;
	sprintf(head_buf, "<br>Mesin %d : %s </h4>", (nomer_mesin + 1), mesin[nomer_mesin].nama);
	strcat(tot_buf, head_buf);
	
	strcat(tot_buf, "<table border>\n");
	strcat(tot_buf, "<col width = ""100px"" />\n");		// titik
	strcat(tot_buf, "<col width = ""200px"" />\n");		// nilai
	strcat(tot_buf, "<col width = ""500px"" />\n");		// keterangan	
	
	strcat(tot_buf, "<tr>\n<th>Titik</th>\n");
	strcat(tot_buf, "<th>Nilai</th>\n");
	strcat(tot_buf, "<th>Keterangan</th>\n</tr>\n");
	
	for (i=0; i< TIAP_MESIN ; i++)
	{
		// titik & nilai
		sprintf(head_buf, "<tr>\n<th>%d</th>\n<td>%1.4f</td>\n", (i+1+cek_mesin), titik[i + cek_mesin].data);
		strcat(tot_buf, head_buf);
		// keterangan	
		
		if (keter[i] == "")
			sprintf(head_buf, "<td>--</td>\n");	
		else
			sprintf(head_buf, "<td>%s</td>\n", keter[i]);	
		
		strcat(tot_buf, head_buf);
		
		#if 0
		// sumber & IP
		sprintf(head_buf, "<td>%d</td>\n<td>%d.%d.%d.%d</td>\n", titik[i].ID_sumber, \
			sumber[ titik[i].ID_sumber ].IP0, sumber[ titik[i].ID_sumber ].IP1, \
			sumber[ titik[i].ID_sumber ].IP2, sumber[ titik[i].ID_sumber ].IP3); 
		strcat(tot_buf, head_buf);
		
		// modul & kanal
		sprintf(head_buf, "<td>%d</td>\n<td>%d</td>\n</tr>\n", titik[i].alamat, titik[i].kanal); 
		strcat(tot_buf, head_buf);
		#endif
	}
#endif

#ifdef BOARD_KOMON_WEB	
	strcat(tot_buf, "</h4>");				
	strcat(tot_buf, "<table border>\n");
	strcat(tot_buf, "<col width = \"70px\" />\n");
	strcat(tot_buf, "<col width = \"90px\" />\n");
#ifdef BOARD_KOMON_420_SAJA	
	strcat(tot_buf, "<col width = \"100px\" />\n");
#endif
	strcat(tot_buf, "<col width = \"100px\" />\n");
	strcat(tot_buf, "<col width = \"200px\" />\n");
	strcat(tot_buf, "<tr>\n<th>Kanal</th>\n");
#endif

#ifdef BOARD_KOMON_A_RTD	
	strcat(tot_buf, "<th>Teg ADC</th>\n");
	strcat(tot_buf, "<th>Nilai</th>\n");
	strcat(tot_buf, "<th>Keterangan</th>\n</tr>\n");
#endif

#ifdef BOARD_KOMON_420_SAJA	
	strcat(tot_buf, "<th>Teg ADC</th>\n");
	strcat(tot_buf, "<th>Status</th>\n");
	strcat(tot_buf, "<th>Nilai</th>\n");
	strcat(tot_buf, "<th>Keterangan</th>\n</tr>\n");
#endif

#ifdef BOARD_KOMON_420_SABANG	
	strcat(tot_buf, "<th>Nilai</th>\n");
	strcat(tot_buf, "<th>Keterangan</th>\n</tr>\n");
#endif

#ifdef BOARD_KOMON_B_THERMO	
	strcat(tot_buf, "<th>Teg (Volt)</th>\n");
	strcat(tot_buf, "<th>Celcius</th>\n");
	strcat(tot_buf, "<th>Keterangan</th>\n</tr>\n");
#endif

#ifdef BOARD_KOMON_KONTER
	strcat(tot_buf, "<th>Puls (Count)</th>\n");
	strcat(tot_buf, "<th>Frek (rpm)</th>\n");
	strcat(tot_buf, "<th>Keterangan</th>\n</tr>\n");
#endif

#ifdef BOARD_KOMON_A_RTD
	for (i=0; i< 5; i++)
	{		
		/*  tegangan */		
		temp_rpm = st_adc.data[i] * faktor_pengali_RTD / 0xffff;
		
		sprintf(head_buf, "<tr>\n<td>Kanal %d</td>\n<td>%1.4f</td>\n", (i+1), temp_rpm);
		strcat(tot_buf, head_buf);
		
		/* satuan yang diinginkan */
		st_adc.flt_data[i] = (float) (temp_rpm * env2->kalib[i].m) + env2->kalib[i].C;
		
		sprintf(head_buf, "<td>%3.3f</td>\n<td>%s</td>\n</tr>\n", st_adc.flt_data[i], env2.kalib[i].ket);		
		strcat(tot_buf, head_buf);
				
	}
	
	/* data 4-20 mA */
	for (i=5; i< 10; i++)
	{		
		/*  tegangan */		
		temp_rpm = st_adc.data[i] * faktor_pengali_420 / 0xffff;
		
		sprintf(head_buf, "<tr>\n<td>Kanal %d</td>\n<td>%1.4f</td>\n", (i+1), temp_rpm);
		strcat(tot_buf, head_buf);
		
		/* satuan yang diinginkan */
		st_adc.flt_data[i] = (float) (temp_rpm * env2file->kalib[i].m) + env2file->kalib[i].C;
		
		sprintf(head_buf, "<td>%3.3f</td>\n<td>%s</td>\n</tr>\n", st_adc.flt_data[i], env2file->kalib[i].ket);		
		strcat(tot_buf, head_buf);
				
	}
#endif
//*
#ifdef BOARD_KOMON_420_SABANG
#ifdef PAKAI_PM
	int h=0;
	struct t_setting *konfig;
	konfig = (char *) ALMT_KONFIG;
	
	for (i=0; i< PER_SUMBER; i++)
	{		

		sprintf(head_buf, "<tr>\n<td>Kanal %d</td>\n<td align=\"right\">%1.4f</td>\n", (i+1), data_f[i*PER_SUMBER+0]);
		strcat(tot_buf, head_buf);
		ganti_karakter(ket, konfig[i].ket);
		sprintf(head_buf, "<td>%s</td>\n</tr>\n", ket);		
		strcat(tot_buf, head_buf);
		/*
		 printf("   kwh		: %.2f kWh\r\n", printf("   kwh		: %.2f kWh\r\n", data_f[i*PER_SUMBER+0]);		// 41
		printf("   kvah		: %.2f kVAh\r\n", data_f[i*PER_SUMBER+1]);		// 41
		printf("   kvarh	: %.2f kVArh\r\n", data_f[i*PER_SUMBER+2]);		// 41
		printf("   kw		: %.2f kW\r\n", data_f[i*PER_SUMBER+3]);		// 41
		printf("   kva		: %.2f kVA\r\n", data_f[i*PER_SUMBER+4]);		// 41
		printf("   kvar		: %.2f kVAr\r\n", data_f[i*PER_SUMBER+5]);		// 41
		printf("   pf		: %.2f\r\n", data_f[i*PER_SUMBER+6]);		// 41
		printf("   volt1	: %.2f V\r\n", data_f[i*PER_SUMBER+7]);		// 41
		printf("   volt2	: %.2f V\r\n", data_f[i*PER_SUMBER+8]);		// 41
		printf("   amp		: %.2f A\r\n", data_f[i*PER_SUMBER+9]);		// 41
		printf("   frek		: %.2f Hz\r\n", data_f[i*PER_SUMBER+10]);		// 41
		printf("   ampA		: %.2f A\r\n", data_f[i*PER_SUMBER+11]);		// 41
		printf("   ampB		: %.2f A\r\n", data_f[i*PER_SUMBER+12]);		// 41
		printf("   ampC		: %.2f A\r\n", data_f[i*PER_SUMBER+13]);		// 41
		printf("   ampN		: %.2f A\r\n", data_f[i*PER_SUMBER+14]);		// 41
		printf("   voltA_B	: %.2f V\r\n", data_f[i*PER_SUMBER+15]);		// 41
		printf("   voltB_C	: %.2f V\r\n", data_f[i*PER_SUMBER+16]);		// 41
		printf("   voltA_C	: %.2f V\r\n", data_f[i*PER_SUMBER+17]);		// 41
		printf("   voltA_N	: %.2f V\r\n", data_f[i*PER_SUMBER+18]);		// 41);		// 41
		printf("   kvah		: %.2f kVAh\r\n", data_f[i*PER_SUMBER+1]);		// 41
		printf("   kvarh	: %.2f kVArh\r\n", data_f[i*PER_SUMBER+2]);		// 41
		printf("   kw		: %.2f kW\r\n", data_f[i*PER_SUMBER+3]);		// 41
		printf("   kva		: %.2f kVA\r\n", data_f[i*PER_SUMBER+4]);		// 41
		printf("   kvar		: %.2f kVAr\r\n", data_f[i*PER_SUMBER+5]);		// 41
		printf("   pf		: %.2f\r\n", data_f[i*PER_SUMBER+6]);		// 41
		printf("   volt1	: %.2f V\r\n", data_f[i*PER_SUMBER+7]);		// 41
		printf("   volt2	: %.2f V\r\n", data_f[i*PER_SUMBER+8]);		// 41
		printf("   amp		: %.2f A\r\n", data_f[i*PER_SUMBER+9]);		// 41
		printf("   frek		: %.2f Hz\r\n", data_f[i*PER_SUMBER+10]);		// 41
		printf("   ampA		: %.2f A\r\n", data_f[i*PER_SUMBER+11]);		// 41
		printf("   ampB		: %.2f A\r\n", data_f[i*PER_SUMBER+12]);		// 41
		printf("   ampC		: %.2f A\r\n", data_f[i*PER_SUMBER+13]);		// 41
		printf("   ampN		: %.2f A\r\n", data_f[i*PER_SUMBER+14]);		// 41
		printf("   voltA_B	: %.2f V\r\n", data_f[i*PER_SUMBER+15]);		// 41
		printf("   voltB_C	: %.2f V\r\n", data_f[i*PER_SUMBER+16]);		// 41
		printf("   voltA_C	: %.2f V\r\n", data_f[i*PER_SUMBER+17]);		// 41
		printf("   voltA_N	: %.2f V\r\n", data_f[i*PER_SUMBER+18]);		// 41
		 
		//*/	
	}
#endif
	
#endif
//*/
#ifdef BOARD_KOMON_420_SAJA
#ifdef PAKAI_ADC
	/* data 4-20 mA */
	for (i=0; i< KANALNYA; i++)
	{		
		/*  tegangan */		
		temp_rpm = st_adc.data[i] * faktor_pengali_420 / 0xffff;
		
		sprintf(head_buf, "<tr>\n<th>%d</th>\n<td align=\"right\">%1.4f V</td>\n", (i+1), temp_rpm);
		strcat(tot_buf, head_buf);
		
		sprintf(head_buf, "<td>%s</td>\n", (env2->kalib[i].status==0)?"Tegangan":"OnOff");
		strcat(tot_buf, head_buf);
		
		/* satuan yang diinginkan */
		if (env2->kalib[i].status==0) {	// "Tegangan"
			st_adc.flt_data[i] = (float) (temp_rpm * env2->kalib[i].m) + env2->kalib[i].C;
			sprintf(head_buf, "<td align=\"right\">%3.3f</td>\n<td>%s</td>\n</tr>\n", st_adc.flt_data[i], env2->kalib[i].ket);		
		} else {
			sprintf(head_buf, "<td align=\"left\">%s</td>\n<td>%s</td>\n</tr>\n", \
				(st_adc.data[i]>20000)?"On/Tertutup":"Off/Terbuka", env2->kalib[i].ket);		
		}

		strcat(tot_buf, head_buf);
				
	}
#endif
#endif

#ifdef BOARD_KOMON_B_THERMO
	for (i=0; i< 10; i++)
	{		
		/*  tegangan */		
		temp_rpm = st_adc.data[i] * faktor_pengali / 0xffff;
		
		sprintf(head_buf, "<tr>\n<td>Kanal %d</td>\n<td>%1.4f</td>\n", (i+1), temp_rpm);
		strcat(tot_buf, head_buf);
		
		/* satuan yang diinginkan */
		st_adc.flt_data[i] = (float) (temp_rpm * env2->kalib[i].m) + env2->kalib[i].C;
		
		sprintf(head_buf, "<td>%3.3f</td>\n<td>%s</td>\n</tr>\n", st_adc.flt_data[i], env2.kalib[i].ket);		
		strcat(tot_buf, head_buf);
				
	}
#endif

#ifdef BOARD_KOMON_KONTER	
	extern unsigned int data_putaran[];
	extern unsigned int data_hit[];
	
	for (i=0; i< 10; i++)
	{		
		if (data_putaran[i])
		{
			/*  cari frekuensi */
			temp_rpm = 1000000000.00 / data_putaran[i]; // data_rpm msh dlm nS
			/* rpm */
			fl = temp_rpm * 60;
		}
		else
		{
			temp_rpm = 0;
			fl = 0;
		}
		
		/* 
			faktor kalibrasi dipakai pada konter 
			
			faktor kalibrasi kanal 0 untuk data_hit 0
				 
		*/
		unsigned int t_hit;
		t_hit = (unsigned int) (data_hit[i] * env2.kalib[i].m);
							
		sprintf(head_buf, "<tr>\n<td>Kanal %d</td>\n<td>%d</td>\n", (i+1), t_hit );
		strcat(tot_buf, head_buf);	
		
		#if (KONTER_MALINGPING == 1)
		/* data kanal 1 adalah adc1 (adc0 internal) */
		if (i== 0)
		{
			extern float volt_supply;				
			fl = (float) ((volt_supply * env2.kalib[0].m) + env2.kalib[0].C);
		}		
				
		#endif
				
		sprintf(head_buf, "<td>%3.3f</td>\n<td>%s</td>\n</tr>\n", fl, env2.kalib[i].ket);		
		strcat(tot_buf, head_buf);
				
	}
#endif
	
	strcat(tot_buf, "</table>\n");
	
	buat_bottom();
	
	//printf("pjg index = %d\r\n", strlen(tot_buf));
	return;
}
#endif


void buat_file_setting(unsigned int flag, char *kata)
{
	int i;
	unsigned int cek_mesin;
	unsigned int mulai = 0;
	unsigned int akhir = 0;
	struct t_env *env2;
	env2 = (char *) ALMT_ENV;
	
	buat_head(0);
	
	//#ifdef BOARD_TAMPILAN
	#ifdef CARI_SUMBER
	
	if (flag == 1)
		nomer_mesin++;
	
	if (nomer_mesin == 5) nomer_mesin = 0;
	
	cek_mesin = TIAP_MESIN * nomer_mesin;
	sprintf(head_buf, "<br>Mesin %d : %s </h4>\n", (nomer_mesin + 1), mesin[nomer_mesin].nama);
	strcat(tot_buf, head_buf);
	
	strcat(tot_buf, "<table border=0 bgcolor=""lightGray"">\n");
	
	strcat(tot_buf, "<col width = ""100px"">\n");		// titik
	strcat(tot_buf, "<col width = ""120px"">\n");		// sumber
	strcat(tot_buf, "<col width = ""200px"">\n");		// IP sumber
	strcat(tot_buf, "<col width = ""120px"">\n");		// Kanal
	strcat(tot_buf, "<col width = ""120px"">\n");		// Modul / board
	strcat(tot_buf, "<col width = ""120px"">\n");		// status
	
	strcat(tot_buf, "<tbody align=\"center\" bgcolor=\"white\">\n");
	strcat(tot_buf, "<tr>\n<th>Titik</th>\n");
	strcat(tot_buf, "<th>Sumber</th>\n");
	strcat(tot_buf, "<th>IP Sumber</th>\n");
	strcat(tot_buf, "<th>Kanal</th>\n");
	strcat(tot_buf, "<th>Modul</th>\n");
	strcat(tot_buf, "<th>Status</th>\n</tr>\n");
	
	
	#if 0
	strcat(tot_buf, "<tr>\n<th bgcolor=\"white\">Titik</th>\n");
	strcat(tot_buf, "<th bgcolor=""white"">Sumber</th>\n");
	strcat(tot_buf, "<th bgcolor=""white"">IP Sumber</th>\n");
	strcat(tot_buf, "<th bgcolor=""white"">Kanal</th>\n");
	strcat(tot_buf, "<th bgcolor=""white"">Modul</th>\n");
	strcat(tot_buf, "<th bgcolor=""white"">Status</th>\n</tr>\n");
	#endif
	
	if (flag == 1)
	{
		mulai = 0;
		akhir = 50;
	}
	if (flag == 2)
	{
		mulai = 50;
		akhir = TIAP_MESIN;
	}
	
	for (i=mulai; i<akhir; i++)
	{
		// titik & sumber
		sprintf(head_buf, "<tr>\n<th>%d</th>\n<td>%d</td>\n", (i+1+cek_mesin), titik[i+cek_mesin].ID_sumber);
		strcat(tot_buf, head_buf);
		
		// IP sumber & kanal	
		sprintf(head_buf, "<td>%d.%d.%d.%d</td>\n<td>%d</td>\n", sumber[ titik[i+cek_mesin].ID_sumber-1 ].IP0, \
			sumber[ titik[i+cek_mesin ].ID_sumber-1 ].IP1, \
			sumber[ titik[i+cek_mesin ].ID_sumber-1 ].IP2, \
			sumber[ titik[i+cek_mesin ].ID_sumber-1 ].IP3, \
			titik[i+cek_mesin ].kanal ); 
			
		strcat(tot_buf, head_buf);
		
		// Modul & status
		sprintf(head_buf, "<td>%d</td>\n<td>%d</td>\n</tr>\n", sumber[ titik[i+cek_mesin ].ID_sumber - 1].alamat, \
		sumber[ titik[i+cek_mesin ].ID_sumber - 1].status);
		strcat(tot_buf, head_buf);
		
	}
	strcat(tot_buf, "</tbody>\n</table>\n");
	#endif
	
	#ifdef BOARD_KOMON_WEB
	#ifdef PAKAI_ADC
	strcat(tot_buf, "<br>Faktor kalibrasi (y = mx + C)</h4>\n");
	strcat(tot_buf, "<table border=0 bgcolor=""lightGray"">\n");
	
	strcat(tot_buf, "<col width = ""100px"">\n");		// Kanal
	strcat(tot_buf, "<col width = ""200px"">\n");		// m
	strcat(tot_buf, "<col width = ""200px"">\n");		// C

	
	strcat(tot_buf, "<tbody align=\"center\" bgcolor=\"white\">\n");
	strcat(tot_buf, "<tr>\n<th>Kanal</th>\n");
	strcat(tot_buf, "<th>m</th>\n");
	strcat(tot_buf, "<th>C</th>\n");

	for (i=0; i<10; i++)
	{
		// Kanal, m & C
		sprintf(head_buf, "<tr>\n<th>%d</th>\n<td>%f</td>\n<td>%f</td>\n</tr>\n", \
			i+1, env2->kalib[i].m, env2->kalib[i].C);
		strcat(tot_buf, head_buf);
	}
	strcat(tot_buf, "</tbody>\n</table>\n");
	#endif
	
	#ifdef CENDOL		// akses php
		if (flag) {
			sprintf(head_buf, "<br/><font color=\"red\">Data telah diubah: %s</font><br/>", kata);
			//strcat(tot_buf, "<br/><font color=\"red\">Data telah diubah: %s</font><br/>");
			strcat(tot_buf, head_buf);
		} else {
			strcat(tot_buf, "<br/>");
		}
		
		strcat(tot_buf, "<h3>Info Kanal</h3>\n");
		strcat(tot_buf, "<table border=0 bgcolor=""lightGray"">\n");
		strcat(tot_buf, "<col width = ""50px"">\n");		// Kanal
		strcat(tot_buf, "<col width = ""40px"">\n");		// id
		strcat(tot_buf, "<col width = ""130px"">\n");		// keterangan
		strcat(tot_buf, "<col width = ""130px"">\n");		// Status
		strcat(tot_buf, "<col width = ""50x"">\n");			// Ubah

		strcat(tot_buf, "<tbody align=\"center\" bgcolor=\"white\">\n");
		strcat(tot_buf, "<tr>\n<th>Kanal</th>\n");
		strcat(tot_buf, "<th>ID Titik</th>\n");
		strcat(tot_buf, "<th>Keterangan</th>\n");
		strcat(tot_buf, "<th>Status Kirim</th>\n");
		strcat(tot_buf, "<th>Ganti</th>\n");
		
		struct t_setting *konfig;
		//int jmlData = (sizeof(data_f)/sizeof(float));
		konfig = (char *) ALMT_KONFIG;
		
		for (i=0; i<PER_SUMBER; i++)
		{
			// Kanal, id & Keterangan
			ganti_karakter(ket, konfig[i].ket);
			
			sprintf(head_buf, "<tr><form action=\"setting.html\"><input type=\"hidden\" name=\"u\" value=\"1\" />" \ 
							"<th>%d</th>\n<td><input type=\"text\" name=\"i%d\" value=\"%d\" size=\"8\"/></td>\n" \
							"<td align=\"left\"><input type=\"text\" name=\"k%d\" value=\"%s\" size=\"30\"/></td>\n" \
							"<td align=\"left\"><input type=\"radio\" name=\"s%d\" value=\"1\" %s/>Aktif" \
							"<input type=\"radio\" name=\"s%d\" value=\"0\" %s/>Mati</td>\n" \
							"<td><input type=\"submit\" value=\"Ganti\" /></td></form>\n</tr>", \
				i+1, i+1, konfig[i].id, \
				i+1, ket, \
				i+1, (konfig[i].status?"checked":""), \
				i+1, (konfig[i].status?"":"checked"));
			strcat(tot_buf, head_buf);
		}
		
		strcat(tot_buf, "</tbody>\n</table>\n");

	#endif
	#endif
	
	buat_bottom();
	
	//printf("pjg setting = %d\r\n", strlen(tot_buf));
	
	return;
}


#if 1
void buat_file_about(void)
{
	/* 
		menampilkan informasi :
		Operating system Kernel
		versi
		build time
		stack posisi stack
		uptime
		idle loop
		
		*/
	
	int i;
	extern unsigned int tot_idle;
	struct t_env *env2;
	env2 = (char *) ALMT_ENV;
	buat_head(0);
	
	strcat(tot_buf, "<h4>Operating System FreeRTOS 5.1.1");
	sprintf(head_buf, "<br>Modul %s versi %s", NAMA_BOARD, VERSI_KOMON);
	strcat(tot_buf, head_buf);
	sprintf(head_buf, "<br>Prosesor NXP LPC 2368 %d MHz", configCPU_CLOCK_HZ /1000000);
	strcat(tot_buf, head_buf);
	sprintf(head_buf, "<br>Build dengan ARM-GCC %s %s %s",__VERSION__, __DATE__, __TIME__);
	strcat(tot_buf, head_buf);
	if (configUSE_PREEMPTION == 0)
		strcat(tot_buf, "<br>Disabled Preemptive kernel"); 
	else
		strcat(tot_buf, "<br>Enabled Preemptive kernel");
	sprintf(head_buf, "<br>Idle loop check = %d perdetik", tot_idle);
	strcat(tot_buf, head_buf);
//*	
	strcat(tot_buf, "<br><br>Task & Free stack (bytes) :");
	sprintf(head_buf, "<br>&nbsp;&nbsp; Shell    : %d", uxTaskGetStackHighWaterMark(hdl_shell));
	strcat(tot_buf, head_buf);
	
	sprintf(head_buf, "<br>&nbsp;&nbsp; Led      : %d", uxTaskGetStackHighWaterMark(hdl_led));
	strcat(tot_buf, head_buf);
	
	//#ifdef BOARD_TAMPILAN
	#ifdef CARI_SUMBER
	sprintf(head_buf, "<br>&nbsp;&nbsp; Tampilan : %d", uxTaskGetStackHighWaterMark(hdl_tampilan));
	strcat(tot_buf, head_buf);
	sprintf(head_buf, "<br>&nbsp;&nbsp; LCD      : %d", uxTaskGetStackHighWaterMark(hdl_lcd));
	strcat(tot_buf, head_buf);
	#endif
	
	sprintf(head_buf, "<br>&nbsp;&nbsp; Ethernet : %d", uxTaskGetStackHighWaterMark(hdl_ether));
	strcat(tot_buf, head_buf);
//*/	
	/* informasi environtment, nama, ip, gateway dll */
	strcat(tot_buf, "<br><br>Informasi Modul :");
	sprintf(head_buf, "<br>&nbsp;&nbsp;Nama     : %s", env2->nama_board);
	strcat(tot_buf, head_buf);
	sprintf(head_buf, "<br>&nbsp;&nbsp;IP       : %d.%d.%d.%d", env2->IP0, env2->IP1, env2->IP2, env2->IP3);
	strcat(tot_buf, head_buf);   
	sprintf(head_buf, "<br>&nbsp;&nbsp;Gateway  : %d.%d.%d.%d", env2->GW0, env2->GW1, env2->GW2, env2->GW3); 
	strcat(tot_buf, head_buf);
	
	strcat(tot_buf, "<br><br>Dibuat oleh :");
	strcat(tot_buf, "<br>&nbsp;&nbsp; Daun Biru Engineering");
	strcat(tot_buf, "<br>&nbsp;&nbsp; jl Pelitur Raya 21 Kayu Putih");
	strcat(tot_buf, "<br>&nbsp;&nbsp; Jakarta 13210");
	strcat(tot_buf, "<br>&nbsp;&nbsp; (021) 4892155, fax (021) 47881750");
		
	strcat(tot_buf,"</h4>");
	buat_bottom();
	//printf("pjg status = %d\r\n", strlen(tot_buf));
	
	return;
}
#endif

#if 0
void buat_file_sumber(void)
{
	int i;
	buat_head(0);
	
	//#ifdef BOARD_TAMPILAN
	#ifdef CARI_SUMBER
	strcat(tot_buf, "<br>Informasi Setting Sumber Data</h4>\n");
	strcat(tot_buf, "<table border=0 bgcolor=""lightGray"">\n");
	
	strcat(tot_buf, "<col width = ""100px"">\n");		// No
	strcat(tot_buf, "<col width = ""200px"">\n");		// nama
	strcat(tot_buf, "<col width = ""200px"">\n");		// IP
	strcat(tot_buf, "<col width = ""120px"">\n");		// Modul
	strcat(tot_buf, "<col width = ""300px"">\n");		// Status

	
	strcat(tot_buf, "<tbody align=\"center\" bgcolor=\"white\">\n");
	strcat(tot_buf, "<tr>\n<th>No</th>\n");
	strcat(tot_buf, "<th>Nama</th>\n");
	strcat(tot_buf, "<th>IP</th>\n");
	strcat(tot_buf, "<th>Modul</th>\n");
	strcat(tot_buf, "<th>Status</th>\n");
	
	for (i=0; i<20; i++)
	{
		// Nomer & Nama
		sprintf(head_buf, "<tr>\n<th>%d</th>\n<td>%s</td>\n", (i+1), sumber[i].nama);
		strcat(tot_buf, head_buf);
		
		// IP sumber & LINK kepadanya	
		sprintf(head_buf, "<td><a href=\"http://%d.%d.%d.%d/index.html\">", sumber[i].IP0, \
			sumber[i].IP1, \
			sumber[i].IP2, \
			sumber[i].IP3 ); 		
		strcat(tot_buf, head_buf);
		
		sprintf(head_buf, "%d.%d.%d.%d</td>\n", sumber[i].IP0, \
			sumber[i].IP1, \
			sumber[i].IP2, \
			sumber[i].IP3 ); 
		strcat(tot_buf, head_buf);
		
		// Modul & status
		sprintf(head_buf, "<td>%d</td>\n", sumber[ i ].alamat);		
		strcat(tot_buf, head_buf);
		
		if (sumber[ i ].status == 0)
			strcat(tot_buf, "<td>Tidak aktif</td>\n</tr>\n");
		else if (sumber[ i ].status == 1)
			strcat(tot_buf, "<td>Normal</td>\n</tr>\n");
		else if (sumber[ i ].status == 2)
			strcat(tot_buf, "<td>Time out</td>\n</tr>\n");
		else if (sumber[ i ].status == 5)
			strcat(tot_buf, "<td>Aktif/daytime</td>\n</tr>\n");
		
	}
	
	strcat(tot_buf, "</tbody>\n</table>\n");
	#endif
	strcat(tot_buf,"</h4>");
	buat_bottom();
	
	//printf("pjg sumber = %d\r\n", strlen(tot_buf));	
	
	return;
}

#endif
