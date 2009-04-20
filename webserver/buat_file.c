/*
	buat file2 untuk dipakai oleh
	web server
	
	20 april 2009
	furkan jadid, daun biru engineering
	
	*/

#define judul	"<html>\n<head>\n<title>Simple Monita Web Server</title>\n"

#define LINK_ATAS "<table border=""0"" align=""left"">\n \
  <tbody align=""center"">\n \
	<tr>\n \
      <td bgcolor=""lightGray"" width=""200""><a href=""index.html"">Data</a></td>\n \
      <td bgcolor=""lightGray"" width=""200""><a href=""setting.html"">Setting</a></td>\n \
      <td bgcolor=""lightGray"" width=""200""><a href=""status.html"">Status</a></td>\n \
      <td bgcolor=""lightGray"" width=""200""><a href=""about.html"">About</a></td>\n \
    </tr>\n \
  </tbody>\n \
</table>\n"

void buat_head(unsigned int flag)
{
	/* flag = 1, perlu di refresh */
	if (flag == 1)
		sprintf(head_buf, "%s<meta http-equiv=""refresh"" content=""3"">\n</head>\n<body>\n\n<h1>Monita Online Monitoring System</h1>\n", judul);
	else if (flag == 0)
		sprintf(head_buf, "%s\n</head>\n<body>\n\n<h1>Monita Online Monitoring System</h1>\n", judul);
	
	sprintf(tot_buf, "%s", head_buf);

	sprintf(head_buf, "%s<br>", LINK_ATAS);
	strcat(tot_buf, head_buf);

	//strcat(tot_buf, "<hr>\n<h4>Daun Biru Engineering");
	strcat(tot_buf, "\n<h4>Daun Biru Engineering");
	sprintf(head_buf, "<br>Monita %s v %s</h4>\n", NAMA_BOARD, VERSI_KOMON);
	strcat(tot_buf, head_buf);

	//sprintf(head_buf, "<p>Modul %s", NAMA_BOARD);
	//strcat(tot_buf, head_buf);

	sprintf(head_buf, "<p>Nama Modul : %s</p>\n", env2.nama_board);
	strcat(tot_buf, head_buf);
}

void buat_bottom(void)
{
	unsigned int sec;
	unsigned int menit;
	unsigned int jam;
	unsigned int hari;
	unsigned int tahun;
	
	/* data uptime */
	uptime(&sec, &menit, &jam, &hari, &tahun);
	strcat(tot_buf, "<h5>Uptime = ");
	if (tahun !=0)
	{
		sprintf(head_buf, "%d thn ", tahun);
		strcat(tot_buf, head_buf);	
	}
	if (hari !=0)
	{
		sprintf(head_buf, "%d hari ", hari);
		strcat(tot_buf, head_buf);		
	}
	if (jam !=0)
	{
		sprintf(head_buf, "%d jam ", jam);
		strcat(tot_buf, head_buf);		
	}
	if (menit !=0)
	{
		sprintf(head_buf, "%d mnt ", menit);
		strcat(tot_buf, head_buf);		
	}
		
	sprintf(head_buf, "%d dtk</h5>\n", sec);
	strcat(tot_buf, head_buf);		
	
	sprintf(head_buf,"<hr>\n<h5>ARM-GCC %s : %s : %s\n", __VERSION__, __DATE__, __TIME__);
	strcat(tot_buf, head_buf);
	strcat(tot_buf, "<br>NXP LPC2368, 60 MHz, FreeRTOS 5.1.1</h5>\n");
	
	
	// close html
    sprintf(head_buf, "\n</body>\n</html>\n");
    strcat(tot_buf, head_buf);
}


void buat_file_index(void)
{
	int besar;
	int i;
	float fl;
	float temp_rpm;

	buat_head(1);
						
#ifdef BOARD_TAMPILAN
	strcat(tot_buf, "<table border>\n");
	strcat(tot_buf, "<col width = ""100px"" />\n");		// titik
	strcat(tot_buf, "<col width = ""200px"" />\n");		// nilai
	strcat(tot_buf, "<col width = ""500px"" />\n");		// keterangan
	/*
	strcat(tot_buf, "<col width = ""120px"" />\n");		// sumber
	strcat(tot_buf, "<col width = ""200px"" />\n");		// IP sumber
	strcat(tot_buf, "<col width = ""120px"" />\n");		// Kanal
	strcat(tot_buf, "<col width = ""120px"" />\n");		// Modul / board
	*/
	
	strcat(tot_buf, "<tr>\n<th>Titik</th>\n");
	strcat(tot_buf, "<th>Nilai</th>\n");
	strcat(tot_buf, "<th>Keterangan</th>\n</tr>\n");
	/*
	strcat(tot_buf, "<th>Sumber</th>\n");
	strcat(tot_buf, "<th>IP</th>\n");
	strcat(tot_buf, "<th>Modul</th>\n");
	strcat(tot_buf, "<th>Kanal</th>\n</tr>\n");
	*/
	
	for (i=0; i<70; i++)
	{
		// titik & nilai
		sprintf(head_buf, "<tr>\n<th>%d</th>\n<td>%1.4f</td>\n", (i+1), titik[i].data);
		strcat(tot_buf, head_buf);
		// keterangan	
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
		
#else	// konter & RTD			
	strcat(tot_buf, "<table border>\n");
	strcat(tot_buf, "<col width = ""120px"" />\n");
	strcat(tot_buf, "<col width = ""140px"" />\n");
	strcat(tot_buf, "<col width = ""170px"" />\n");
	strcat(tot_buf, "<col width = ""400px"" />\n");
	strcat(tot_buf, "<tr>\n<th>Kanal</th>\n");
#endif

#ifdef BOARD_KOMON_A_RTD	
	strcat(tot_buf, "<th>Teg (Volt)</th>\n");
	strcat(tot_buf, "<th>Celcius / Barg</th>\n");
	strcat(tot_buf, "<th>Keterangan</th>\n</tr>\n");
#endif

#ifdef BOARD_KOMON_KONTER
	strcat(tot_buf, "<th>Puls (Count)</th>\n");
	strcat(tot_buf, "<th>Frek (rpm)</th>\n");
	strcat(tot_buf, "<th>Keterangan</th>\n</tr>\n");
#endif

	
#ifdef BOARD_KOMON_A_RTD
	for (i=0; i< 10; i++)
	{		
		/*  tegangan */
		
		temp_rpm = st_adc.data[i] * faktor_pengali / 0xffff;
		
		sprintf(head_buf, "<tr>\n<td>Kanal %d</td>\n<td>%1.4f</td>\n", (i+1), temp_rpm);
		strcat(tot_buf, head_buf);
		
		/* satuan yang diinginkan */
		st_adc.flt_data[i] = (float) (temp_rpm * env2.kalib[i].m) + env2.kalib[i].C;
		
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
		sprintf(head_buf, "<tr>\n<td>Kanal %d</td>\n<td>%d</td>\n", (i+1), data_hit[i]);
		strcat(tot_buf, head_buf);
		
		sprintf(head_buf, "<td>%3.3f</td>\n<td>%s</td>\n</tr>\n", fl, env2.kalib[i].ket);	
		strcat(tot_buf, head_buf);
				
	}
#endif
	
	//strcat(tot_buf, "</table>\n<p>Test report kondisi adalah sabagaimana ...</p>\n");
	strcat(tot_buf, "</table>\n");
	//sprintf(head_buf,"<h5>-- %d --\n<br>", http_hit);
	//strcat(tot_buf, head_buf);
	
	buat_bottom();
	
	printf("pjg index = %d\r\n", strlen(tot_buf));
	return;
}

void buat_file_setting(void)
{
	int i;
	buat_head(0);
	
	#ifdef BOARD_TAMPILAN
	strcat(tot_buf, "\n<table border=0 bgcolor=""lightGray"">\n");
	
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
	
	for (i=0; i<70; i++)
	{
		// titik & sumber
		sprintf(head_buf, "<tr>\n<th>%d</th>\n<td>%d</td>\n", (i+1), titik[i].ID_sumber);
		strcat(tot_buf, head_buf);
		// IP sumber & kanal	
		sprintf(head_buf, "<td>%d.%d.%d.%d</td>\n<td>%d</td>\n", sumber[ titik[i].ID_sumber ].IP0, sumber[ titik[i].ID_sumber ].IP1, \
			sumber[ titik[i].ID_sumber ].IP2, sumber[ titik[i].ID_sumber ].IP3, titik[i].kanal ); 
			
		strcat(tot_buf, head_buf);
		
		// Modul & status
		sprintf(head_buf, "<td>%d</td>\n<td>%d</td>\n</tr>\n", sumber[ titik[i].ID_sumber ].alamat, sumber[ titik[i].ID_sumber ].status);
		strcat(tot_buf, head_buf);
		
	}
	#endif
	strcat(tot_buf, "</tbody>\n</table>\n");
	
	buat_bottom();
	
	printf("pjg setting = %d\r\n", strlen(tot_buf));
}

void buat_file_status(void)
{
	int i;
	buat_head(0);
	
	buat_bottom();
	printf("pjg status = %d\r\n", strlen(tot_buf));
}
