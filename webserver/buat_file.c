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
#include "hardware.h"

#ifdef BOARD_TAMPILAN
	#define BESAR_BUF_HTTP	256		//8192
#else
	#define BESAR_BUF_HTTP	8192		//8192
#endif

#ifdef PAKAI_HTTP

#include "../tinysh/enviro.h"
#include "../modul/adc/ad7708.h"

#ifdef CENDOL
//	#include "../tinysh/setting_eth.c" 
#endif


	unsigned char tot_buf[BESAR_BUF_HTTP] 		__attribute__ ((section (".index_text")));
//#endif
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

#ifdef BOARD_KOMON_420_SABANG_2_3
#define BOARD_KOMON_WEB
#endif


#define judul	"<html>\n<head>\n<title>Simple Monita Web Server</title>\n"

#ifdef PAKAI_PM
	extern char * judulnya_pm[];
	extern char * satuannya_pm[];
#endif


#ifdef BOARD_TAMPILAN
//#ifdef BOARD_DISPLAY
//#ifdef CARI_SUMBERNYA
#define LINK_ATAS "<table border=\"0\" align=\"left\">\n \
  <tbody align=\"center\">\n \
	<tr>\n \
      <td bgcolor=\"lightGray\" width=\"200\"><a href=\"index.html\">Data</a></td>\n \
      <td bgcolor=\"lightGray\" width=\"200\"><a href=\"set_satu.html\">Setting 1-50</a></td>\n \
	  <td bgcolor=\"lightGray\" width=\"200\"><a href=\"set_dua.html\">Setting 51-100</a></td>\n \
      <td bgcolor=\"lightGray\" width=\"200\"><a href=\"sumber.html\">Sumber Data</a></td>\n \
      <td bgcolor=\"lightGray\" width=\"200\"><a href=\"about.html\">About</a></td>\n \
    </tr>\n \
  </tbody>\n \
</table>\n"

#define LINK_BAWAH "<table border=\"0\" align=\"left\">\n \
  <tbody align=\"center\">\n \
	<tr>\n \
      <td bgcolor=\"lightGray\" width=\"200\">.</td>\n \
      <td bgcolor=\"lightGray\" width=\"200\">.</td>\n \
      <td bgcolor=\"lightGray\" width=\"200\">.</td>\n \
      <td bgcolor=\"lightGray\" width=\"200\">.</td>\n \
      <td bgcolor=\"lightGray\" width=\"200\">.</td>\n \
    </tr>\n \
  </tbody>\n \
</table>\n"
#endif

#ifdef BOARD_KOMON_WEB

#define LINK_ATAS "<table border=\"0\" align=\"left\">\n \
  <tbody align=\"center\">\n \
	<tr>\n \
      <td bgcolor=\"lightGray\" width=\"200\"><a href=\"index.html\">Data</a></td>\n \
      <td bgcolor=\"lightGray\" width=\"200\"><a href=\"setting.html\">Setting</a></td>\n \
      <td bgcolor=\"lightGray\" width=\"200\"><a href=\"about.html\">About</a></td>\n \
    </tr>\n \
  </tbody>\n \
</table>\n"

#define LINK_BAWAH "<table border=\"0\" align=\"left\">\n \
  <tbody align=\"center\">\n \
	<tr>\n \
      <td bgcolor=\"lightGray\" width=\"200\">.</td>\n \
      <td bgcolor=\"lightGray\" width=\"200\">.</td>\n \
      <td bgcolor=\"lightGray\" width=\"200\">.</td>\n \
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
		sprintf(head_buf, "%s<meta http-equiv=\"refresh\" content=\"3\">\n</head>\n<body>\n\n<h1>Monita Online Monitoring System</h1>\n", judul);
	else if (flag == 0)
		sprintf(head_buf, "%s\n</head>\n<body>\n\n<h1>Monita Online Monitoring System</h1>\n", judul);
	
	sprintf(tot_buf, "%s", head_buf);
	
	sprintf(head_buf, "%s<br>", LINK_ATAS);
	strcat(tot_buf, head_buf);

	strcat(tot_buf, "\n<h4>Daun Biru Engineering<br/>\n");
	sprintf(head_buf, "Monita %s v %s<br/>\n", NAMA_BOARD, VERSI_KOMON);
	strcat(tot_buf, head_buf);

	sprintf(head_buf, "Nama Modul : %s<br/>\n", env2->nama_board);
	strcat(tot_buf, head_buf);
	sprintf(head_buf, "Alamat IP  : %d.%d.%d.%d</h4>\n", env2->IP0, env2->IP1, env2->IP2, env2->IP3);
	strcat(tot_buf, head_buf);
}

void buat_kepala() {
	struct t_env *env2;
	env2 = (char *) ALMT_ENV;
	
	char head_buf[512];
	
	sprintf(head_buf, "%s\n</head>\n<body>\n\n<h1>Monita Online Monitoring System</h1>\n", judul);	
	sprintf(tot_buf, "%s", head_buf);
	
	sprintf(head_buf, "%s<br />", LINK_ATAS);
	strcat(tot_buf, head_buf);
//*	
	strcat(tot_buf, "\r\n<script language=\"JavaScript\">\r\n" \
		"<!--\r\n" \
		"function Titik(str){\r\n" \
		"   var hasil=str.replace(/\\./gi, \",\");\r\n" \
		"   return hasil;\r\n" \
		"}\r\n" \
		"-->\r\n" \
		"</script>\r\n");
	/*
	var str=document.mF.q.value;
	var hasil=str.replace(/\\./gi, \",\");
	document.mF.q.value = hasil;
	//*/
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

#define DIGANTI 5

//int ganti(char* input);

int ganti_karakter(char *dest, char *src) {
	char *asli[] = {"+", "%2F", "%2C", "%23", "C"};
	char *pengganti[] = {" ","/", ".", "#", "."};
	char * pch;

	int i=0, j=0, k=0;
	//for (j=0; src[j] != '\0'; ++j) {
	for (j=0; src[j] != '\0'; j++) {
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

int kali10(int n) {
	int i=1;
	while(n>0) {
		i*=10;
		n--;
	}
	return i;
}

float per10(int n) {
	float f=1;
	while(n>0) {
		f*=0.1;
		n--;
	}
	return f;
}

int angka(char x) {
	if(x>='0' && x<='9') {
		return (int)(x-'0');
	} else {
		return 0;
	}
}

float stof(char * str) {
	int pjg=0, jml=0, i=0, j=0;
	int posTitik;
	float pecahan=0.00;
	
	char * pch, *pch2, pecah[10];
	
	pjg = strlen(str);
	
	//printf("p: %d, string: %s\r\n", pjg, str);
	// cek apakah karakter angka, titik
	for (i=0; i<pjg; i++) {
		if (!(str[i]>=45 && str[i]<=57 && str[i]!=47))
			return 0.00;
	}	
	// cek '-' hanya di awal saja
	pch2=strchr(str,'-');
	if (pch2!=NULL) {
		posTitik = pch2-str+1;
		//printf("posisi: %d\r\n", posTitik);
		if (posTitik>1) {
			return 0.00;
		}
	}
	
	// cari titik pertama, batas bulat dan pecahan
	pch=strchr(str,'.');
	if(pch!=NULL) {
		posTitik=pch-str;
		//printf("posisiTitik: %d\r\n", posTitik);
		pch2 = pch+1;
		pch=strchr(pch+1,'.');
		i=pch-str;
	}
	//*/
	
	jml=strlen(pch2);
	//printf("i: %d, jml: %d\r\n",i, jml);
	// ketemu jml titiknya > 1, hilangkan titik ke 2 dsb
	if (i>0) {
  		i=0;
  		for (j=0; j<jml; j++) {
			if(pch2[j]!='.') {
				pecah[i]=pch2[j];
				i++;
			}
			if (i>7)
				break;		
		}
		pecah[i]='\0';
	} else {
		for (j=0; j<jml; j++) {
			pecah[j]=pch2[j];
		}
		pecah[j]='\0';
		//printf("isi pecah: %s\r\n",pecah);
	}

	// kalikan perslotnya
	for (i=0; i<posTitik; i++) {
		//printf("nilai bulat: %d, pecah: %c, kali10: %d\r\n", angka(str[i])+1, str[i], kali10(posTitik-i-1));
		pecahan += (angka(str[i])*kali10(posTitik-i-1));
	}
	//printf("bulat: %f\r\n", pecahan);
	posTitik=strlen(pecah);
	//printf("jml pecah: %d\r\n", posTitik);
	for (i=0; i<posTitik; i++) {
		//printf("nilai pecah: %d\r\n", angka(pecah[i]));
		pecahan += (angka(pecah[i])*per10(i+1));
	}	
	//printf("pecah: %f\r\n", pecahan);
	// bilangan negatif kah ?
	if (str[0]=='-') {
		pecahan=0-pecahan;
	}
	//printf("return total: %f, +111.111: %f\r\n", pecahan, pecahan+111.111);
	return pecahan;
}


int ganti_setting(char *str) {
//#ifndef BANYAK_SUMBER
	//printf(" str: %s\r\n", str);
	char tmp[30], kets[60], keta[30], ketb[30], ketc[30], ketd[30], ketaz[60];
	char ketip[30];
	unsigned int ret_ip;
	
	int nk=0, no=0, titik=0, z=0;
	char aa=0, bb=0, cc=0, gg=0;
	int kanal=0; 
	float m=0.00, c=0.00;
	char statnya=0;
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
			printf("  id: %d\r\n", titik);
		} 
		else if (strncmp(tmp, "k", 1)==0) {		// nama board
			ket = strchr(tmp,'=');
			ganti_karakter(ketaz, ket+1);
			
			if (strlen(ketaz)>30) {
				strncpy(kets, ketaz, 26);
				kets[27] = '\0';
			}
			else
				strcpy(kets, ketaz);
			printf("  board, len: %d ket: %s, kets: %s\r\n", strlen(kets), ket+1, kets);
		}
		else if (strncmp(tmp, "f", 1)==0) {		// file webclient
			ket = strchr(tmp,'=');
			ganti_karakter(ketaz, ket+1);
			if (strlen(ketaz)>30)
				strncpy(keta, ketaz, 30);
			else
				strcpy(keta, ketaz);
			printf("  file  ket: %s, keta: %s\r\n", ket+1, keta);
		}
		else if (strncmp(tmp, "q", 1)==0) {		// no serial
			ket = strchr(tmp,'=');
			//ganti_karakter(ketb, ket+1);
			ganti_karakter(ketaz, ket+1);
			if (strlen(ketaz)>20) {
				strncpy(ketb, ketaz, 19);
				ketb[27] = '\0';
			} else
				strcpy(ketb, ketaz);
			printf("  seri  ket: %s, ketb: %s\r\n", ket+1, ketb);
		}
		else if (strncmp(tmp, "p", 1)==0) {		// ip
			ket = strchr(tmp,'=');
			strcpy(ketip, "");
			ganti_karakter(ketc, ket+1);
			//printf("  IP    ket: %s, ketc : %s\r\n", ket+1, ketc);
			
			ganti_karakter(ketip, ketc);
			printf("  IP    ket: %s, ketip: %s\r\n", ket+1, ketip);
		}
		else if (strncmp(tmp, "t", 1)==0) {		// gateway
			ket = strchr(tmp,'=');
			ganti_karakter(ketd, ket+1);
			printf("  gatew ket: %s, ketd: %s\r\n", ket+1, ketd);
		}
		else if (strncmp(tmp, "z", 1)==0) {
			ids = strchr(tmp,'=');
			z = atoi(ids+1);
			printf("  z: %d, ids: %s\r\n", z, ids+1);
		}
		else if (strncmp(tmp, "a", 1)==0) {
			ids = strchr(tmp,'=');
			aa = atoi(ids+1);
			//printf("  a: %d, ids: %s\r\n", aa, ids+1);
		}
		else if (strncmp(tmp, "b", 1)==0) {
			ids = strchr(tmp,'=');
			bb = atoi(ids+1);
			//printf("  b: %d, ids: %s\r\n", bb, ids+1);
		}
		else if (strncmp(tmp, "g", 1)==0) {
			ids = strchr(tmp,'=');
			gg = atoi(ids+1);
			//printf("  gg : %d, ids: %s\r\n", gg, ids+1);
		}
		else if (strncmp(tmp, "s", 1)==0) {
			stat = strchr(tmp,'=')+1;
			//printf("  stat: %s\r\n", stat);
		}
		else if (strncmp(tmp, "m", 1)==0) {
			ids = strchr(tmp,'m');
			kanal = atoi(ids+1);
			
			ket = strchr(tmp,'=');
			m = stof(ket+1);
			//printf(" ket:%s,  m: %f\r\n", ket+1, m);
		}
		if (strncmp(tmp, "c", 1)==0) {
			ket = strchr(tmp,'=');
			c = stof(ket+1);
			//printf(" ket:%s,  c: %f\r\n", ket+1, c);
		}
		pch=strchr(pch+1, '&');
	}
	
	
	if (z==3) {
		struct t_sumber *p_sbry;
		//*	
		p_sbry = pvPortMalloc( JML_SUMBER * sizeof (struct t_sumber) );

		if (p_sbry == NULL)	{
			vPortFree(p_sbry);
			printf("%s(): Err allok memory gagal !\r\n");
			return -1;
		}
		
		portENTER_CRITICAL();
		memcpy((char *) p_sbry, (char *) ALMT_SUMBER, (JML_SUMBER * sizeof (struct t_sumber)));
		portEXIT_CRITICAL();
		
		no-=1;
		p_sbry[no].alamat = bb;
		p_sbry[no].status = (atoi(stat))?1:0;
		p_sbry[no].stack  = (aa==0?1:aa);
		sprintf(p_sbry[no].nama,"%s", kets);
		p_sbry[no].tipe  = gg;
		
		ret_ip = baca_ip(ketc);	
		p_sbry[no].IP0 = (unsigned char)(ret_ip >> 24);
		p_sbry[no].IP1 = (unsigned char)(ret_ip >> 16);
		p_sbry[no].IP2 = (unsigned char)(ret_ip >> 8);
		p_sbry[no].IP3 = (unsigned char)(ret_ip);
		
		//*
		if (simpan_sumber(p_sbry) < 0)		{
			vPortFree(p_sbry);
			return -1;
		}
		
		vPortFree(p_sbry);
		//*/
		return 1;
	} else if (z==4) {
		struct t_env *p_sbr;
		
		p_sbr = pvPortMalloc( sizeof (struct t_env) );
		
		portENTER_CRITICAL();
		memcpy((char *) p_sbr, (char *) ALMT_ENV, (sizeof (struct t_env)));
		portEXIT_CRITICAL();
		
		//printf(" %s(): Mallok ok di %X\r\n", __FUNCTION__, p_sbr);
		//*
		sprintf(p_sbr->nama_board,"%s", kets);
		sprintf(p_sbr->SN,"%s", ketb);
		sprintf(p_sbr->berkas,"%s", keta);
		
		//printf("ip: %s\r\n", ketip);
		ret_ip = baca_ip(ketip);	
		p_sbr->IP0 = (unsigned char)(ret_ip >> 24);
		p_sbr->IP1 = (unsigned char)(ret_ip >> 16);
		p_sbr->IP2 = (unsigned char)(ret_ip >> 8);
		p_sbr->IP3 = (unsigned char)(ret_ip);
		
		ret_ip = baca_ip(ketd);	
		p_sbr->GW0 = (unsigned char)(ret_ip >> 24);
		p_sbr->GW1 = (unsigned char)(ret_ip >> 16);
		p_sbr->GW2 = (unsigned char)(ret_ip >> 8);
		p_sbr->GW3 = (unsigned char)(ret_ip);
		//*/
		
		p_sbr->statusWebClient = aa;
		p_sbr->burst = bb;
		
		if (p_sbr == NULL) {
			vPortFree(p_sbr);
			printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
			return -1;
		}
		if (simpan_env(p_sbr) < 0) {
			vPortFree(p_sbr);
			return -1;
		}		
		vPortFree(p_sbr);
		
		return 1;
	#ifdef TES_GET_WEB 
	} else if (z==5) {
		printf("Ganti Setting Modul\r\n");		
		return 1;
	#endif
	} else {
		if (no>0) {
			int jmlData=0;
			//#if (defined PAKAI_PM)
				jmlData = (sizeof(data_f)/sizeof(float));
			//#else
			//	jmlData = KANALNYA;
			//#endif
			
			struct t_setting *p_sbr;
			//printf("Jml Data : %d\r\n", jmlData);
			p_sbr = pvPortMalloc( jmlData * sizeof (struct t_setting) );
			
			if (p_sbr == NULL)
			{
				printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
				vPortFree( p_sbr );
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
			
			#ifndef BOARD_TAMPILAN
			if (simpan_konfignya( p_sbr ) < 0)	{
				vPortFree( p_sbr );
				return;
			}
			vPortFree( p_sbr );
			#endif
		}
	}
	#ifdef PAKAI_ADCx
	if (kanal>0) {
		printf("_________masuk kanal %d\r\n", kanal);
		struct t_env *envzzz;
		envzzz = pvPortMalloc( sizeof (struct t_env) );
		
		if (envzzz == NULL)	{
			printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
			vPortFree( envzzz );
			return;	// -1
		}

		envzzz->kalib[kanal-1].m = m;
		envzzz->kalib[kanal-1].C = c;
		envzzz->kalib[kanal-1].status = (atoi(stat))?1:0;
		envzzz->magic1 = 0xAA;
		envzzz->magic2 = 0x77;
		
		/*
		portENTER_CRITICAL();
		if (simpan_env( envzzz ) < 0)	{
			vPortFree( envzzz );
			return;
		}
		portEXIT_CRITICAL();
		//*/
		vPortFree( envzzz );
	}
	//printf();
	#endif
//#endif
	return 1;
}

#if 0
void buat_file_index(unsigned int flag, char *kata) {
	buat_head(1);
	buat_bottom();
}
#endif

#if 1
	void buat_file_index(unsigned int flag, char *kata) {

	int i, no=0;
	unsigned int cek_mesin;
	float fl;
	float temp_rpm;
	struct t_env *env2;
	env2 = (char *) ALMT_ENV;
	char head_buf[512];
	buat_head(1);
						
//#ifdef BOARD_TAMPILAN
#ifdef CARI_SUMBER
	nomer_mesin++;
	if (nomer_mesin == 5) nomer_mesin = 0;
	
	cek_mesin = TIAP_MESIN * nomer_mesin;
	sprintf(head_buf, "Mesin %d : %s </h4>", (nomer_mesin + 1), mesin[nomer_mesin].nama);
	strcat(tot_buf, head_buf);
	
	strcat(tot_buf, "<table border>\n");
	strcat(tot_buf, "<col width = \"100px\" />\n");		// titik
	strcat(tot_buf, "<col width = \"200px\" />\n");		// nilai
	strcat(tot_buf, "<col width = \"500px\" />\n");		// keterangan	
	
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

	#ifdef BANYAK_SUMBER
		char tmp[5]; char *pch, *pch2;
		int pertamax=0, nk=0;
		struct t_setting *konfig;
		konfig = (char *) ALMT_KONFIG;
		
		struct t_sumber *pmx;
		pmx = (char *) ALMT_SUMBER;
		
		if(flag) {
			pch=strchr(kata,'&');
			pch=strchr(pch,'=');
			pch2=strchr(pch+1, '\0');
			nk=pch2-pch-1;
						
			strncpy(tmp, pch+1, nk);
			tmp[nk]='\0';
			nk = atoi(tmp);
			//printf("nk: %d, tmp: %s, tmp: %d\r\n", nk, tmp, atoi(tmp)+111);
		}
				
		strcat(tot_buf, "<b>No Modul : </b>");
		for (i=0; i<JML_SUMBER; i++)	{
			/* status */
			if (pmx[i].status == 1) {
				if (flag && (i+1)==nk) {
					sprintf(head_buf, " <font color=\"red\" size=\"5\"><b>[%d]</b></font> ", i+1);
					no = i;
				} else if (pertamax==0 && flag==0) {
					sprintf(head_buf, " <font color=\"red\" size=\"5\"><b>[%d]</b></font> ", i+1);
					no = i;
				} else {
					sprintf(head_buf, "[<a href=\"index.html?sbr=1&d=%d\">%d</a>] ", i+1, i+1);
				}
				strcat(tot_buf, head_buf);
				pertamax++;
			}
			/*
			if (pmx[i].status == 1) {
				if (flag && pmx[i].alamat==nk) {
					sprintf(head_buf, " <font color=\"red\" size=\"5\"><b>[%d]</b></font> ", pmx->pm[i].alamat);
					no = i;
				} else if (pertamax==0 && flag==0) {
					sprintf(head_buf, " <font color=\"red\" size=\"5\"><b>[%d]</b></font> ", pmx->pm[i].alamat);
				} else {
					sprintf(head_buf, "[<a href=\"index.html?pm=1&d=%d\">%d</a>] ", pmx[i].alamat, pmx[i].alamat);
				}
				strcat(tot_buf, head_buf);
				pertamax++;
			}
			//*/
		}
		if (!pertamax)	strcat(tot_buf, "Tidak ada Sumber Aktif</br>\n");
	#endif

	strcat(tot_buf, "<table border>\n");
	//strcat(tot_buf, "<col width = \"70px\" />\n");
	//strcat(tot_buf, "<col width = \"90px\" />\n");
#ifdef BOARD_KOMON_420_SAJA	
	//strcat(tot_buf, "<col width = \"100px\" />\n");
#endif
	//strcat(tot_buf, "<col width = \"100px\" />\n");
	//strcat(tot_buf, "<col width = \"200px\" />\n");
	//strcat(tot_buf, "<tr>\n<th width=\"100px\">Kanal</th>\n");
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

#ifdef BOARD_KOMON_420_SABANG			// buat PM meter
	strcat(tot_buf, "<tr>\n<th width=\"100px\">Kanal</th>\n");
	strcat(tot_buf, "<th width=\"100px\">Nilai</th>\n");
	strcat(tot_buf, "<th width=\"100px\">Satuan</th>\n");
	strcat(tot_buf, "<th width=\"200px\">Keterangan</th>\n</tr>\n");
#endif

#ifdef BOARD_KOMON_420_SABANG_2_3		// buat PM meter
	strcat(tot_buf, "<tr>\n<th width=\"100px\">Kanal</th>\n");
	strcat(tot_buf, "<th width=\"100px\">Nilai</th>\n");
	strcat(tot_buf, "<th width=\"100px\">Satuan</th>\n");
	strcat(tot_buf, "<th width=\"200px\">Keterangan</th>\n</tr>\n");
#endif

#ifdef BOARD_KOMON_B_THERMO	
	strcat(tot_buf, "<th>Teg (Volt)</th>\n");
	strcat(tot_buf, "<th>Celcius</th>\n");
	strcat(tot_buf, "<th>Keterangan</th>\n</tr>\n");
#endif

#ifdef BOARD_KOMON_KONTER
	if (pertamax>0) {
		strcat(tot_buf, "<tr>\n<th width=\"50px\">No</th>\n");
		//printf("env2->IP3: %d, pmx[no].IP3: %d", env2->IP3, pmx[no].IP3);
		if ((env2->IP3 == pmx[no].IP3) && (pmx[no].status==1)) {
			strcat(tot_buf, "<th width=\"100px\">Frek (rpm)</th>\n");
			strcat(tot_buf, "<th width=\"100px\">Pulsa</th>\n");	
		} 
		else if (pmx[no].status==1) {
			strcat(tot_buf, "<th width=\"100px\">Kanal</th>\n");
			strcat(tot_buf, "<th width=\"100px\">Nilai</th>\n");
			strcat(tot_buf, "<th width=\"100px\">Satuan</th>\n");
		}
		strcat(tot_buf, "<th width=\"240px\">Keterangan</th>\n</tr>\n");
		
		struct t_dt_set *p_dt;
		p_dt = (char *) ALMT_DT_SET;
		
		//*
		if ((env2->IP3 == pmx[no].IP3) && (pmx[no].status==1)) {
			for (i=0; i<10; i++)	{
				#ifndef BOARD_KOMON_KONTER_3_1
				if (i>6) 
				#endif
				{
					sprintf(head_buf, "<tr>\n<td align=\"center\">%d</td><td align=\"right\">%.2f</td>\n<td align=\"right\">%.0f</td>", \
						(i+1), data_f[no*PER_SUMBER+(i*2)], data_f[no*PER_SUMBER+(i*2)+1]);
					strcat(tot_buf, head_buf);
					
					sprintf(head_buf, "<td>Konter Kanal %d</td>\n</tr>\n", i+1);	
					strcat(tot_buf, head_buf);
				}
			}
		} 
		//*
		else if (pmx[no].status==1) {
			if (pmx[no].alamat==0) {			// Modul Monita
				for (i=0; i<PER_SUMBER; i++)	{
					ganti_karakter(ket, p_dt[no*PER_SUMBER+i].nama);
					sprintf(head_buf, "<tr>\n<td align=\"center\">%d</td><td align=\"center\">%d</td>\n<td align=\"right\">%.2f</td>\n", i+1, (no*PER_SUMBER+i+1), data_f[no*PER_SUMBER+i]);
					strcat(tot_buf, head_buf);
					sprintf(head_buf, "<td>%s</td><td>%s</td>\n</tr>\n", strlen(p_dt[no*PER_SUMBER+i].satuan)?p_dt[no*PER_SUMBER+i].satuan:"-", strlen(ket)>0?ket:"-");		
					strcat(tot_buf, head_buf);
				}
			}
		}
		//*/
	}
	strcat(tot_buf, "</table>\n");


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

#if defined(BOARD_KOMON_420_SABANG) || defined(BOARD_KOMON_420_SABANG_2_3)
	struct t_dt_set *p_dt;
	p_dt = (char *) ALMT_DT_SET;
	//printf("no: %d, alamat: %d\r\n", no, pmx[i].alamat);

	if (pmx[no].alamat==0) {			// Modul Monita
		for (i=0; i<PER_SUMBER; i++)	{
			ganti_karakter(ket, p_dt[no*PER_SUMBER+i].nama);
			sprintf(head_buf, "<tr>\n<td>Kanal %d</td>\n<td align=\"right\">%.2f</td>\n", (no*PER_SUMBER+i+1), data_f[no*PER_SUMBER+i]);
			strcat(tot_buf, head_buf);
			sprintf(head_buf, "<td>%s</td><td>%s</td>\n</tr>\n", p_dt[no*PER_SUMBER+i].satuan, ket);		
			strcat(tot_buf, head_buf);
		}
		//strcat(tot_buf, "</table>\n");
	}
	
#ifdef PAKAI_PM
	else if (pmx[no].tipe==0 || pmx[no].tipe==1) {		// Power Meter
		for (i=0; i< PER_SUMBER; i++)	{
			sprintf(head_buf, "<tr>\n<td>Kanal %d</td>\n<td align=\"right\">%.2f</td>\n", (no*PER_SUMBER+i+1), data_f[no*PER_SUMBER+i]);
			strcat(tot_buf, head_buf);
			sprintf(head_buf, "<td>%s</td><td>%s</td>\n</tr>\n", judulnya_pm[i], satuannya_pm[i]);		
			strcat(tot_buf, head_buf);
		}
		//strcat(tot_buf, "</table>\n");
	}
#endif

#if defined(BOARD_KOMON_420_SAJA)
#ifdef PAKAI_ADC
	struct t_setting *konfig;
	konfig = (char *) ALMT_KONFIG;
	
	/* data 4-20 mA */
	for (i=0; i< KANALNYA; i++)
	{		
		ganti_karakter(ket, konfig[i].ket);
		//printf("ket: %s\r\n", ket);

		/*  tegangan */		
		temp_rpm = st_adc.data[i] * faktor_pengali_420 / 0xffff;
		
		sprintf(head_buf, "<tr>\n<th>%d</th>\n<td align=\"right\">%1.4f V</td>\n", (i+1), temp_rpm);
		strcat(tot_buf, head_buf);
		
		sprintf(head_buf, "<td>%s</td>\n", (env2->kalib[i].status==0)?"Tegangan":"OnOff");
		strcat(tot_buf, head_buf);
		
		/* satuan yang diinginkan */
		if (env2->kalib[i].status==0) {	// "Tegangan"
			sprintf(head_buf, "<td align=\"right\">%3.3f</td>\n<td>%s</td>\n</tr>\n", data_f[i], ket);		
		} else {
			sprintf(head_buf, "<td align=\"left\">%s</td>\n<td>%s</td>\n</tr>\n", \
				((int)data_f[i]==1)?"On/Tertutup":"<font color=\"red\">Off/Terbuka</font>", ket);		
		}

		strcat(tot_buf, head_buf);
	}
#endif
#endif

#ifdef BOARD_KOMON_B_THERMO
	for (i=0; i< KANALNYA; i++)
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
/*
	extern unsigned int data_putaran[];
	extern unsigned int data_hit[];
	
	struct t_setting *konfig;
	konfig = (char *) ALMT_KONFIG;

	//for (i=0; i< KANALNYA; i++)	{
	for (i=0; i< KANALNYA; i++)	{		
		if (i>6) {
			sprintf(head_buf, "<tr>\n<td>Kanal %d</td>\n<td align=\"right\">%.0f</td><td align=\"right\">%10.2f</td>\n<td>Pulsa dan Konter</td>\n</tr>\n", \
				i+1, data_f[i*2+1], data_f[(i*2)]);
			strcat(tot_buf, head_buf);	
		}
		#if (KONTER_MALINGPING == 1)
		if (i== 0)
		{
			extern float volt_supply;				
			fl = (float) ((volt_supply * env2.kalib[0].m) + env2.kalib[0].C);
		}		
				
		#endif
				
		//sprintf(head_buf, "<td>%3.3f</td>\n<td>%s</td>\n</tr>\n", fl, env2->kalib[i].ket);		
		//strcat(tot_buf, head_buf);
				
	}
//*/
#endif
	
	strcat(tot_buf, "</table>\n");
#endif
	buat_bottom();
	return;
}

#endif

#if 0
void buat_file_setting(unsigned int flag, char *kata)	{
	buat_head(0);
	
	buat_bottom();
}
#endif



#if 1
void buat_file_setting(unsigned int flag, char *kata)	{
	int i=0;
	unsigned int cek_mesin;
	unsigned int mulai = 0;
	unsigned int akhir = 0;
	struct t_env *env2;
	env2 = (char *) ALMT_ENV;
	char head_buf[1024]; char ketr[30];
	char tmp[5]; char *pch, *pch2;
	//buat_head(0);
	
	strcpy(head_buf, kata);
	buat_kepala();
	//#ifdef BOARD_TAMPILAN
	#ifdef CARI_SUMBER
	
	if (flag == 1)
		nomer_mesin++;
	
	if (nomer_mesin == 5) nomer_mesin = 0;
	
	cek_mesin = TIAP_MESIN * nomer_mesin;
	sprintf(head_buf, "<br>Mesin %d : %s </h4>\n", (nomer_mesin + 1), mesin[nomer_mesin].nama);
	strcat(tot_buf, head_buf);
	
	strcat(tot_buf, "<table border=0 bgcolor=\"lightGray\">\n");
	
	strcat(tot_buf, "<col width = \"100px"">\n");		// titik
	strcat(tot_buf, "<col width = \"120px"">\n");		// sumber
	strcat(tot_buf, "<col width = \"200px"">\n");		// IP sumber
	strcat(tot_buf, "<col width = \"120px"">\n");		// Kanal
	strcat(tot_buf, "<col width = \"120px"">\n");		// Modul / board
	strcat(tot_buf, "<col width = \"120px"">\n");		// status
	
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
	
	if (flag == 1)	{
		mulai = 0;
		akhir = 50;
	}
	if (flag == 2)	{
		mulai = 50;
		akhir = TIAP_MESIN;
	}
	
	for (i=mulai; i<akhir; i++)	{
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
	
	strcat(tot_buf, "<br/>: <a href=\"setting.html\">Info Titik Ukur</a> :: <a href=\"setting.html?smb=4\">Info Modul</a> :");
	
	#ifdef BANYAK_SUMBER
		strcat(tot_buf, ": <a href=\"setting.html?smb=3\">Info Sumber</a> :");
	#endif
	
	#ifdef TES_GET_WEB
	strcat(tot_buf, ": <a href=\"setting.html?smb=5\">Info Tes</a> :");
	#endif
	strcat(tot_buf, "<br/>\n");
	
	#ifdef PAKAI_ADCx
		strcat(tot_buf, "<h3>Faktor kalibrasi (y = mx + C)</h3>\n");

		strcat(tot_buf, "<table border=\"0\" bgcolor=\"lightGray\">\n");
		strcat(tot_buf, "<tbody align=\"center\" bgcolor=\"white\">\n");
		strcat(tot_buf, "<tr>\n<th width=\"50px\">Kanal</th>\n");
		strcat(tot_buf, "<th width=\"60px\">m</th>\n");		// klo pake input 40 aja
		strcat(tot_buf, "<th width=\"60px\">C</th>\n");		// klo pake input 40 aja
		strcat(tot_buf, "<th width=\"220px\">Status</th>\n");
		//strcat(tot_buf, "<th width=\"60px\">Ganti</th>\n");
		strcat(tot_buf, "</tr>\n");
		
		for (i=0; i<KANALNYA; i++)	{
			// Kanal, m & C
			/* JANGAN DIHAPUS
			sprintf(head_buf,	"<tr>\n<form action=\"setting.html\"><input type=\"hidden\" name=\"u\" value=\"1\" />\n" \
								"<th>%d</th>\n<td><input type=\"text\" name=\"m%d\" value=\"%f\" size=\"10\"/></td>\n" \
								"<td><input type=\"text\" name=\"c%d\" value=\"%f\" size=\"10\"/></td>\n" \
								"<td align=\"left\"><input type=\"radio\" name=\"s%d\" value=\"0\" %s/>Suhu/Tekanan" \
								"<input type=\"radio\" name=\"s%d\" value=\"1\" %s/>OnOff</td>\n" \
								"<td><input type=\"submit\" value=\"Ganti\" /></td></form></tr>\n", \
				i+1, i+1, env2->kalib[i].m, \
				i+1, env2->kalib[i].C, \
				i+1, (env2->kalib[i].status==0)?"checked":"", \
				i+1, (env2->kalib[i].status==0)?"":"checked");
			//*/
			sprintf(head_buf,	"<tr>\n<th>%d</th>\n<td align=\"right\">%6.3f\n" \
								"<td align=\"right\">%6.3f</td>\n" \
								"<td align=\"left\"><input type=\"radio\" name=\"s%d\" value=\"0\" %s/>Suhu/Tekanan" \
								"<input type=\"radio\" name=\"s%d\" value=\"1\" %s/>OnOff</td>\n" \
								"</tr>\n", \
				i+1, env2->kalib[i].m, env2->kalib[i].C, \
				i+1, (env2->kalib[i].status==0)?"checked":"", \
				i+1, (env2->kalib[i].status==0)?"":"checked");
			strcat(tot_buf, head_buf);
		}
		strcat(tot_buf, "</tbody>\n</table>\n");
	#endif
	
	#ifdef BOARD_KOMON_KONTERx
		//*
		strcat(tot_buf, "<h3>Faktor kalibrasi (y = mx + C)</h3>\n");

		strcat(tot_buf, "<table border=\"0\" bgcolor=\"lightGray\">\n");
		strcat(tot_buf, "<tbody align=\"center\" bgcolor=\"white\">\n");
		strcat(tot_buf, "<tr>\n<th width=\"50px\">Kanal</th>\n");
		strcat(tot_buf, "<th width=\"60px\">m</th>\n");		// klo pake input 40 aja
		strcat(tot_buf, "<th width=\"60px\">C</th>\n");		// klo pake input 40 aja
		//strcat(tot_buf, "<th width=\"60px\">Ganti</th>\n");
		strcat(tot_buf, "</tr>\n");
		
		for (i=0; i<KANALNYA; i++)	{
			if (i>6) {
				// Kanal, m & C
				/*
				sprintf(head_buf,	"<tr>\n<form action=\"setting.html\"><input type=\"hidden\" name=\"u\" value=\"1\" />\n" \
									"<th>%d</th>\n<td><input type=\"text\" name=\"m%d\" value=\"%f\" size=\"10\"/></td>\n" \
									"<td><input type=\"text\" name=\"c%d\" value=\"%f\" size=\"10\"/></td>\n" \
									"<td align=\"left\"><input type=\"radio\" name=\"s%d\" value=\"0\" %s/>Suhu/Tekanan" \
									"<input type=\"radio\" name=\"s%d\" value=\"1\" %s/>OnOff</td>\n" \
									"<td><input type=\"submit\" value=\"Ganti\" /></td></form></tr>\n", \
					i+1, i+1, env2->kalib[i].m, \
					i+1, env2->kalib[i].C, \
					i+1, (env2->kalib[i].status==0)?"checked":"", \
					i+1, (env2->kalib[i].status==0)?"":"checked");
				//*/
				sprintf(head_buf,	"<tr>\n<th>%d</th>\n<td align=\"right\">%6.3f\n" \
									"<td align=\"right\">%6.3f</td>\n" \
									"</tr>\n", \
					i+1, env2->kalib[i].m, env2->kalib[i].C);
				strcat(tot_buf, head_buf);
			}
		}
		strcat(tot_buf, "</tbody>\n</table>\n");
		//*/
	#endif
	
	#ifdef CENDOL		// akses php

		if (flag==1) {
			ganti_karakter(ket, kata);
			sprintf(head_buf, "<font color=\"red\">Data telah diubah: %s</font><br/>", ket);
			strcat(tot_buf, head_buf);
			//printf("kata: %s\r\n", kata);
			pch=strchr(kata,'z');
			if (pch!=NULL) {
				i = atoi(pch+2);
				//printf("i: %d, pch: %s\r\n", i, pch+2);
				if (i==3) {				// setting sumber
					flag=3;
				} else if (i==4) {		// setting modul
					flag=4;
				#ifdef TES_GET_WEB
				} else if (i==5) {		// tes web
					flag=5;
				#endif
				}
			} 
		} else {
			strcat(tot_buf, "<br/>");
		}
		if (flag==3) {			// konfig modul sumber 
			#ifdef BANYAK_SUMBER
				struct t_sumber *p_sbrw;
				p_sbrw = (char *) ALMT_SUMBER;
				
				strcat(tot_buf, "\r\n<script language=\"JavaScript\">\r\n" \
					"<!--\r\n" \
					"function gantiTitik(F){\r\n" \
					"   var str;\r\n" \
					"   var Fx=F;\r\n" \
					"	str=Titik(Fx.p.value);\r\n" \
					"	Fx.p.value = str;\r\n" \
					"}\r\n" \
					"-->\r\n" \
					"</script>\r\n");
				
				strcat(tot_buf, "<h3>Info Modul Sumber</h3>\n");				
				strcat(tot_buf, "<table border=\"0\" bgcolor=\"lightGray\"><tbody bgcolor=\"white\">\r\n" \
						"<tr><th width=\"40\">No</th>\r\n<th width=\"150\">Nama Modul</th>\r\n" \
						"<th width=\"100\">IP</th>\r\n<th width=\"50\">Stack</th>\r\n<th width=\"60\">Alamat</th>\r\n" \
						"<th width=\"400\">Tipe</th>\r\n" \
						"<th width=\"150\">Status</th>\r\n<th width=\"50\">Ganti</th></tr>\r\n");

				for (i=0; i<JML_SUMBER; i++) {					
					ganti_karakter(ket, p_sbrw[i].nama);
					
					sprintf(head_buf, "%d.%d.%d.%d", p_sbrw[i].IP0, p_sbrw[i].IP1, p_sbrw[i].IP2, p_sbrw[i].IP3);
					ganti_karakter(ketr, head_buf);
					
					sprintf(head_buf, "<tr><form action=\"setting.html\" name=\"mF%d\">\r\n" \
						"<input type=\"hidden\" name=\"u\" value=\"1\" />\r\n" \
						"<input type=\"hidden\" name=\"z\" value=\"3\" />\r\n" \ 
						"<input type=\"hidden\" name=\"i%d\" value=\"%d\" />\r\n" \ 
						"<th>%d</th><td><input type=\"text\" name=\"k\" value=\"%s\"></td>\r\n" \
						"<td align=\"right\"> <input type=\"text\" name=\"p\" value=\"%s\" size=\"10\"></td>\r\n" \
						"<td align=\"center\"><input type=\"text\" name=\"a\" value=\"%d\" size=\"3\"></td>\r\n" \
						"<td align=\"center\"><input type=\"text\" name=\"b\" value=\"%d\" size=\"3\"></td>\r\n" \
						"<td><input type=\"radio\" name=\"g\" value=\"0\" %s/>PM710 <input type=\"radio\" name=\"g\" value=\"1\" %s/>PM810 <input type=\"radio\" name=\"g\" value=\"2\" %s/>KTA <input type=\"radio\" name=\"g\" value=\"100\" %s/>Modul Monita &nbsp;</td>\r\n" \
						"<td><input type=\"radio\" name=\"s\" value=\"1\" %s/>Aktif <input type=\"radio\" name=\"s\" value=\"0\" %s/>Mati</td></td>\r\n" \
						"<td><input type=\"submit\" value=\"Ganti\" onClick=\"gantiTitik(mF%d)\"/>\r\n</td>" \
						"</form></tr>\r\n", \
						i+1, i+1, i+1, i+1, \
						ket, \
						ketr, \
						p_sbrw[i].stack, \
						p_sbrw[i].alamat, \
						(p_sbrw[i].tipe==0?"checked":" "), (p_sbrw[i].tipe==1?"checked":" "), (p_sbrw[i].tipe==2?"checked":" "), (p_sbrw[i].tipe==100?"checked":" "), \
						(p_sbrw[i].status?"checked":" "), (p_sbrw[i].status?" ":"checked"), \
						i+1 );
					strcat(tot_buf, head_buf);
					//printf("%2d. tipe: %d, status: %d\r\n", i+1, p_sbrw[i].tipe, p_sbrw[i].status);
				}
				strcat(tot_buf, "</tbody></table>\r\n");
			#endif
		#ifdef TES_GET_WEB
		} else if (flag==5) {
			struct t_env *env2ww;
			env2ww = (char *) ALMT_ENV;
			
			
		#endif
		} else if (flag==4) {	// info modul
			strcat(tot_buf, "\r\n<script language=\"JavaScript\">\r\n" \
				"<!--\r\n" \
				"function gantiTitik(){\r\n" \
				"	var strx;\r\n" \
				"	strx=document.mF.q.value;\r\n" \
				"   document.mF.q.value=Titik(strx);\r\n" \
				"	strx=document.mF.f.value;\r\n" \
				"   document.mF.f.value=Titik(strx);\r\n" \
				"	strx=document.mF.p.value;\r\n" \
				"   document.mF.p.value=Titik(strx);\r\n" \
				"	strx=document.mF.t.value;\r\n" \
				"   document.mF.t.value=Titik(strx);\r\n" \
				"	strx=document.mF.k.value;\r\n" \
				"   document.mF.k.value=Titik(strx);\r\n" \
				"}\r\n" \
				"-->\r\n" \
				"</script>\r\n");
			
			struct t_env *env2ww;
			env2ww = (char *) ALMT_ENV;
			
			strcat(tot_buf, "<h3>Info Modul</h3>\n");
			strcat(tot_buf, "<form action=\"setting.html\"  name=\"mF\">\r\n" \
					"<input type=\"hidden\" name=\"u\" value=\"1\" />\r\n" \
					"<input type=\"hidden\" name=\"z\" value=\"4\" />\r\n");
			
			strcat(tot_buf, "<table border=\"0\" bgcolor=\"lightGray\"><tbody bgcolor=\"white\">\r\n" \
					"<tr><th width=\"120\">Keterangan</th>\r\n" \
					"<th width=\"200\">Status</th></tr>\r\n");
			
			ganti_karakter(ket, env2ww->nama_board);
			sprintf(head_buf, "<tr>" \
					"<td>Nama Board</td><td><input type=\"text\" name=\"k\" value=\"%s\" size=\"30\"></td>" \
					"</tr>\r\n", ket);
			strcat(tot_buf, head_buf);
			
			ganti_karakter(ket, env2ww->SN);
			sprintf(head_buf, "<tr>" \
					"<td>No Seri</td><td><input type=\"text\" name=\"q\" value=\"%s\" size=\"30\"></td>" \
					"</tr>\r\n", ket);
			strcat(tot_buf, head_buf);

			sprintf(head_buf, "%d.%d.%d.%d", env2ww->IP0, env2ww->IP1, env2ww->IP2, env2ww->IP3);
			ganti_karakter(ket, head_buf);
			sprintf(head_buf, "<tr>" \ 
					"<td>Alamat IP</td><td><input type=\"text\" name=\"p\" value=\"%s\"></td>" \
					"</tr>\r\n", ket);
			strcat(tot_buf, head_buf);
		
			sprintf(head_buf, "%d.%d.%d.%d", env2ww->GW0, env2ww->GW1, env2ww->GW2, env2ww->GW3);
			ganti_karakter(ket, head_buf);
			sprintf(head_buf, "<tr>" \
					"<td>Gateway IP</td><td><input type=\"text\" name=\"t\" value=\"%s\"></td>" \
					"</tr>\r\n", ket);
			strcat(tot_buf, head_buf);

			sprintf(head_buf, "<tr>" \
					"<td>Status Webclient</td><td><input type=\"radio\" name=\"a\" value=\"1\" %s>Aktif <input type=\"radio\" name=\"a\" value=\"0\" %s>Mati</td>" \
					"</tr>\r\n", env2ww->statusWebClient?"checked":" ", env2ww->statusWebClient?" ":"checked");
			strcat(tot_buf, head_buf);
			
			sprintf(head_buf, "<tr>" \
					"<td>Mode Burst</td><td><input type=\"radio\" name=\"b\" value=\"1\" %s>Aktif <input type=\"radio\" name=\"b\" value=\"0\" %s>Mati</td>" \
					"</tr>\r\n", env2ww->burst?"checked":" ", env2ww->burst?" ":"checked");
			strcat(tot_buf, head_buf);
			//*
			ganti_karakter(ket, env2ww->berkas);
			sprintf(head_buf, "<tr>" \
					"<td>Lokasi File</td><td><input type=\"text\" name=\"f\" value=\"%s\" size=\"30\"></td>" \
					"</tr>\r\n", ket);
			strcat(tot_buf, head_buf);
			//*/
			strcat(tot_buf, "</tbody></table>\r\n");
			strcat(tot_buf, "<input type=\"submit\" value=\"Ganti\" onClick=\"gantiTitik()\"/>\r\n</form>\r\n");
			
			#ifdef PAKAI_ADC
			
			#endif
			
		} else {
			strcat(tot_buf, "<h3>Info Titik Ukur</h3>\n");
			int pertamax=0, nk=0, no=0;

			struct t_sumber *pmx;
			pmx = (char *) ALMT_SUMBER;
			#ifdef BANYAK_SUMBER	
				if(flag==2) {
					pch=strchr(kata,'&');
					pch=strchr(pch,'=');
					pch2=strchr(pch+1, '\0');
					nk=pch2-pch-1;
								
					strncpy(tmp, pch+1, nk);
					tmp[nk]='\0';
					nk = atoi(tmp);
				}
				
				if (flag==1) {
					pch=strchr(kata,'&');
					pch=strchr(pch,'=');
					pch2=strchr(pch+1, '&');
					nk=pch2-pch-1;
								
					strncpy(tmp, pch+1, nk);
					tmp[nk]='\0';
					nk = atoi(tmp);
				}
				
				strcat(tot_buf, "<b>Alamat Modul : </b>");
				for (i=0; i<JML_SUMBER; i++)	{
					if (pmx[i].status == 1) {
						if (flag && (i+1)==nk) {
							sprintf(head_buf, " <font color=\"red\" size=\"5\"><b>[%d]</b></font> ", i+1);
							no = i;
						} else if (pertamax==0 && flag==0) {
							sprintf(head_buf, " <font color=\"red\" size=\"5\"><b>[%d]</b></font> ", i+1);
							no = i;
						} else {
							sprintf(head_buf, "[<a href=\"setting.html?smb=1&d=%d\">%d</a>] ", i+1, i+1);
						}
						
						strcat(tot_buf, head_buf);
						pertamax++;
					}
				}
				if (pertamax==0)
					strcat(tot_buf, "<b>Tidak ada modul yang aktif</b><br/>");
				
			#endif
		
			if (pertamax>0) {
				strcat(tot_buf, "<table border=0 bgcolor=\"lightGray\">\n");
				strcat(tot_buf, "<tbody align=\"center\" bgcolor=\"white\">\n");
				#if defined(BOARD_KOMON_420_SABANG) || defined(BOARD_KOMON_420_SABANG_2_3)
					strcat(tot_buf, "<tr>\n<th width=\"50px\">No</th>\n");
				#endif
				#ifdef BOARD_KOMON_KONTER
				strcat(tot_buf, "<th width=\"50px\">No</th>\n");
				#endif
				strcat(tot_buf, "<th width=\"50px\">Kanal</th>\n");
				strcat(tot_buf, "<th width=\"40px\">ID Titik</th>\n");
				strcat(tot_buf, "<th width=\"200px\">Keterangan</th>\n");
				#ifdef BOARD_KOMON_KONTERx
				strcat(tot_buf, "<th width=\"100px\">Nilai</th>\n");
				#endif
				strcat(tot_buf, "<th width=\"130px\">Status Kirim</th>\n");
				strcat(tot_buf, "<th width=\"50px\">Ganti</th></tr>\n");
			}
			struct t_setting *konfig;
			konfig = (char *) ALMT_KONFIG;
		
			// buahaya disini !!!! //
			#ifdef BOARD_KOMON_420_SAJA
				for (i=0; i<PER_SUMBER; i++)		{
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
				//*/
				}
				strcat(tot_buf, "</tbody>\n</table>\n");
			#endif
		
			#if defined(BOARD_KOMON_420_SABANG) || defined(BOARD_KOMON_420_SABANG_2_3)
				struct t_dt_set *p_dt;
				p_dt = (char *) ALMT_DT_SET;
				
				//printf("no: %d, pmx[no].alamat: %d\r\n", no, pmx[no].alamat);
				if (pertamax>0) {
					for (i=0; i<PER_SUMBER; i++)	{	
						if (pmx[no].alamat==0) { 		// Modul Monita
							ganti_karakter(ket, p_dt[no*PER_SUMBER+i].nama);
							//printf("nama %d: %s, aslinya: %s\r\n", i+1, ket, p_dt[no*PER_SUMBER+i].nama);
							sprintf(head_buf, "<tr><form action=\"setting.html\">" \
								"<input type=\"hidden\" name=\"u\" value=\"1\" /><input type=\"hidden\" name=\"d\" value=\"%d\" />" \ 
								"<th>%d</th><th>%d</th>\n<td align=\"right\"><input type=\"text\" name=\"i%d\" value=\"%d\" size=\"8\"/></td>\n" \
								"<td><input type=\"text\" name=\"k\" value=\"%s\" size=\"20\"/></td>\n" \
								"<td><input type=\"radio\" name=\"s\" value=\"1\" %s/>Aktif" \
								"<input type=\"radio\" name=\"s\" value=\"0\" %s/>Mati</td>\n" \
								"<td><input type=\"submit\" value=\"Ganti\" /></td>" \
								"</form>\n</tr>", \
								no+1, i+1, (no*PER_SUMBER+i)+1, (no*PER_SUMBER+i)+1, konfig[PER_SUMBER*no+i].id, \
								ket, \
								(konfig[PER_SUMBER*no+i].status?"checked":" "), \
								(konfig[PER_SUMBER*no+i].status?" ":"checked") \
								);
							strcat(tot_buf, head_buf);
						}
						else {
							#ifdef PAKAI_PM
							if (pmx[no].alamat>0 && (pmx[no].tipe==0 || pmx[no].tipe==1)) {
								ganti_karakter(ket, judulnya_pm[i]);
								//printf("nama %d: %s, aslinya: %s\r\n", i+1, ket, p_dt[no*PER_SUMBER+i].nama);
								sprintf(head_buf, "<tr><form action=\"setting.html\">" \
									"<input type=\"hidden\" name=\"u\" value=\"1\" /><input type=\"hidden\" name=\"d\" value=\"%d\" />" \ 
									"<th>%d</th><th>%d</th>\n<td align=\"right\"><input type=\"text\" name=\"i%d\" value=\"%d\" size=\"8\"/></td>\n" \
									"<td align=\"left\">%s</td>\n" \
									"<td><input type=\"radio\" name=\"s\" value=\"1\" %s/>Aktif" \
									"<input type=\"radio\" name=\"s\" value=\"0\" %s/>Mati</td>\n" \
									"<td><input type=\"submit\" value=\"Ganti\" /></td>" \
									"</form>\n</tr>", \
									no+1, i+1, (no*PER_SUMBER+i)+1, (no*PER_SUMBER+i)+1, konfig[PER_SUMBER*no+i].id, \
									ket, \
									(konfig[PER_SUMBER*no+i].status?"checked":" "), \
									(konfig[PER_SUMBER*no+i].status?" ":"checked") \
									);
								strcat(tot_buf, head_buf);
							}
							#endif
						}
					}
				}
				strcat(tot_buf, "</tbody>\n</table>\n");
			#endif

			#ifdef BOARD_KOMON_KONTER
			struct t_dt_set *p_dt;
			p_dt = (char *) ALMT_DT_SET;
			
			int z=0;
			if (pertamax>0)	{
				if (env2->IP3 == pmx[no].IP3)	{
					for (i=0; i<KANALNYA; i++)		{
						#ifndef BOARD_KOMON_KONTER_3_1 
						if (i>6)
						#endif 
						{
							sprintf(head_buf, "<tr><form action=\"setting.html\"><input type=\"hidden\" name=\"u\" value=\"1\" /><input type=\"hidden\" name=\"d\" value=\"%d\" />" \ 
											"<td>%d</td><td>%d</td>\n<td><input type=\"text\" name=\"i%d\" value=\"%d\" size=\"8\"/></td>\n" \
											"<td align=\"left\">Frekuensi/RPM kanal %d</td>\n" \
											"<td align=\"left\"><input type=\"radio\" name=\"s\" value=\"1\" %s/>Aktif" \
											"<input type=\"radio\" name=\"s\" value=\"0\" %s/>Mati</td>\n" \
											"<td><input type=\"submit\" value=\"Ganti\" /></td></form>\n</tr>", \
								no+1, z+1, i+1, i*2+1, konfig[i*2].id, \
								i+1, \
								//data_f[i*2],
								(konfig[i*2].status?"checked":""), \
								(konfig[i*2].status?"":"checked"));
							strcat(tot_buf, head_buf);
							z++;
							
							sprintf(head_buf, "<tr><form action=\"setting.html\"><input type=\"hidden\" name=\"u\" value=\"1\" /><input type=\"hidden\" name=\"d\" value=\"%d\" />" \ 
											"<td>%d</td><td>%d</td>\n<td><input type=\"text\" name=\"i%d\" value=\"%d\" size=\"8\"/></td>\n" \
											"<td align=\"left\">Pulsa konter kanal %d</td>\n" \
											"<td align=\"left\"><input type=\"radio\" name=\"s%d\" value=\"1\" %s/>Aktif" \
											"<input type=\"radio\" name=\"s%d\" value=\"0\" %s/>Mati</td>\n" \
											"<td><input type=\"submit\" value=\"Ganti\" /></td></form>\n</tr>", \
								no+1, z+1, i+1, i*2+2, konfig[i*2+1].id, \
								i+1, \
								//data_f[i*2+1],
								i+1, (konfig[i*2+1].status?"checked":""), \
								i+1, (konfig[i*2+1].status?"":"checked"));
							strcat(tot_buf, head_buf);
							z++;
						}
					}
				//*/
				} else {
					for (i=0; i<PER_SUMBER; i++)		{
						ganti_karakter(ket, p_dt[no*PER_SUMBER+i].nama);
						//printf("ket: %.2f\r\n", data_f[PER_SUMBER*no+i]);
						
						sprintf(head_buf, "<tr><form action=\"setting.html\">" \
								"<input type=\"hidden\" name=\"u\" value=\"1\" /><input type=\"hidden\" name=\"d\" value=\"%d\" />" \ 
								"<th>%d</th><th>%d</th>\n<td align=\"right\"><input type=\"text\" name=\"i%d\" value=\"%d\" size=8/></td>\n" \
								"<td align=\"left\"><input type=\"text\" name=\"k\" value=\"%s\" size=15/></td>\n" \
								"<td><input type=\"radio\" name=\"s\" value=1 %s/>Aktif" \
								"<input type=\"radio\" name=\"s\" value=0 %s/>Mati</td>\n" \
								"<td><input type=\"submit\" value=\"Ganti\" /></td>" \
								"</form>\n</tr>", \
								no+1, i+1, (no*PER_SUMBER+i)+1, (no*PER_SUMBER+i)+1, konfig[PER_SUMBER*no+i].id, \
								strlen(ket)>0?ket:"-", \
								(konfig[PER_SUMBER*no+i].status?"checked":" "), \
								(konfig[PER_SUMBER*no+i].status?" ":"checked") \
								);
						/*
						sprintf(head_buf, "<tr><form action=\"setting.html\"><input type=\"hidden\" name=\"u\" value=\"1\" /><input type=\"hidden\" name=\"d\" value=\"%d\" />" \ 
											"<td>%d</td><td>%d</td>\n<td><input type=\"text\" name=\"i%d\" value=\"%d\" size=\"8\"/></td>\n" \
											"<td align=\"left\"><input type=\"text\" name=\"k\" value=\"%s\" size=\"20\"/></td>\n<td align=\"right\">%.2f</td>" \
											"<td align=\"left\"><input type=\"radio\" name=\"s%d\" value=\"1\" %s/>Aktif" \
											"<input type=\"radio\" name=\"s%d\" value=\"0\" %s/>Mati</td>\n" \
											"<td><input type=\"submit\" value=\"Ganti\" /></td></form>\n</tr>", \
							no+1, i+1, PER_SUMBER*no+i+1, PER_SUMBER*no+i+1, konfig[PER_SUMBER*no+i].id, \
							strlen(ket)>0?ket:"-", data_f[PER_SUMBER*no+i],	\
							i+1, (konfig[PER_SUMBER*no+i].status?"checked":""), \
							i+1, (konfig[PER_SUMBER*no+i].status?"":"checked"));
						//*/
						strcat(tot_buf, head_buf);
					}
				}
				strcat(tot_buf, "</tbody>\n</table>\n");
			}
			#endif
			
		}
		//strcat(tot_buf, "</tbody>\n</table>\n");

	#endif	
	#endif

	buat_bottom();
	
	//printf("pjg setting = %d\r\n", strlen(tot_buf));
	
	return;
}
#endif

#if 1
void buat_file_about(void)	{
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
	char head_buf[512];
	buat_head(0);
	
	strcat(tot_buf, "<h4>Operating System FreeRTOS 5.1.1");
	sprintf(head_buf, "<br>Modul %s versi %s", NAMA_BOARD, VERSI_KOMON);
	strcat(tot_buf, head_buf);
	sprintf(head_buf, "<br>Prosesor %s %d MHz", uC, configCPU_CLOCK_HZ /1000000);
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
void buat_file_sumber(void)	{
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

#endif //PAKE_HTTP
