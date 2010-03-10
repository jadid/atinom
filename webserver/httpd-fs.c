/*
 * Copyright (c) 2001, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 * $Id: httpd-fs.c,v 1.1 2006/06/07 09:13:08 adam Exp $
 * 
 * 
 * 9 Maret 2010
 * furkan jadid, daun biru engineering
 * 
 * edit untuk menampilkan isi MMC
 * 
 */

#include "httpd.h"
#include "httpd-fs.h"
#include "httpd-fsdata.h"

#ifndef NULL
#define NULL 0
#endif /* NULL */

#include "httpd-fsdata.c"

#if HTTPD_FS_STATISTICS
static u16_t count[HTTPD_FS_NUMFILES];
#endif /* HTTPD_FS_STATISTICS */

FIL fweb;
DIR dirs;
FATFS *fs;
FILINFO fileInfo;
char link_asli[128];
	
extern unsigned char buf_lfn[255];

#include "FreeRTOS.h"

#define judul	"<html>\n<head>\n<title>Simple Monita Web Server</title>\n"

int http_fs_baca_dir(char *buf, int *pjg, struct httpd_state *s)
{
	int ret;
	char *nama;
	char temp[256];
	char *name = s->filename;
	
	if ( s->file.nomer == 0 ) /* awal baca direktori */
	{
		fileInfo.lfname = buf_lfn;
		fileInfo.lfsize = sizeof (buf_lfn);
	
		

		/* HEAD */
		sprintf(buf, "%s\n</head>\n<body>\n\n<h1>Monita Online Monitoring System</h1>\n", judul);		
		strcat( buf, "<br>");
		
		/* nama diganti html, biar langsung ditampilkan */
		//sprintf( name, "data.html");
		
		s->file.nomer = 1;		
		s->len = strlen(buf);	/* pjg yang harus ditransmit */		
		*pjg = s->len;
		
		return 1;				/* masih harus dipanggil lagi */		
	}
	
	if (((ret = f_readdir (&dirs, &fileInfo)) != FR_OK) || !fileInfo.fname [0])
	{
		/* sudah habis, BOTTOM */
		sprintf( buf, "\n</body>\n</html>\n");
			
		s->len = strlen(buf);	/* pjg yang harus ditransmit */
		*pjg = 0;
		return 0;				/* sudah habis, tidak perlu dipanggil lagi */
	}
	
	sprintf(buf, "");
	
	if (fileInfo.lfname[0] == 0)
		nama = &(fileInfo.fname [0]);
	else
		nama = &(fileInfo.lfname[0]);
			
	if (fileInfo.fattrib & AM_DIR)
	{
		sprintf(temp, "<img src=\"/www/folder.gif\" alt=\"[DIR]\"> ");
		strcat( buf, temp );
	}
			
	sprintf(temp, "<a href=\"%s/%s\">", link_asli, nama);
	strcat( buf, temp);
			/*		
			sprintf (temp, "\r\n%c%c%c%c%c %u/%02u/%02u %02u:%02u %9u  %s",
				(fileInfo.fattrib & AM_DIR) ? 'D' : '-',
				(fileInfo.fattrib & AM_RDO) ? 'R' : '-',
				(fileInfo.fattrib & AM_HID) ? 'H' : '-',
				(fileInfo.fattrib & AM_SYS) ? 'S' : '-',
				(fileInfo.fattrib & AM_ARC) ? 'A' : '-',
				(fileInfo.fdate >> 9) + 1980, (fileInfo.fdate >> 5) & 15, fileInfo.fdate & 31,
				(fileInfo.ftime >> 11), (fileInfo.ftime >> 5) & 63,
				fileInfo.fsize, nama);
			*/
	sprintf (temp, "%s", nama);
	strcat( buf, temp);
	strcat( buf, "</a>\n<br>");
	
	s->len = strlen(buf);	/* pjg yang harus ditransmit */
	*pjg = s->len;
	return 1;				/* masih harus dipanggil lagi */
		
}

/*-----------------------------------------------------------------------------------*/
static u8_t
httpd_fs_strcmp(const char *str1, const char *str2)
{
  u8_t i;
  i = 0;
 loop:

  if(str2[i] == 0 ||
     str1[i] == '\r' ||
     str1[i] == '\n') {
    return 0;
  }

  if(str1[i] != str2[i]) {
    return 1;
  }


  ++i;
  goto loop;
}
/*-----------------------------------------------------------------------------------*/
int
httpd_fs_open(const char *name, struct httpd_fs_file *file)
{
	int ret;
	unsigned int size;
	unsigned int files;
	unsigned int jum_dirs;
	char *nama;
	char temp[256];
	
	printf("%s(): %s\r\n", __FUNCTION__, name);
	file->flag = 0;

	
	if (ret = f_open(&fweb, name, FA_READ | FA_WRITE))
	{
		printf("%s(): Buka file error %d !\r\n", __FUNCTION__, ret);
		
		/* kemungkinan bukan file, tetapi direktori, coba cek dan list direktori */
		if ((ret = f_opendir (&dirs, name)))
		{ 
			printf("%s(): Buka direktori error = %d !\r\n", __FUNCTION__, ret);
			return 0;
		}
		
		file->len = 20000; // dummy saja
		file->fd = NULL;
		file->flag = 29;	/* baca direktori */
		file->nomer = 0;	/* awal baca direktori */
		
		/* simpan link asli dulu */
		sprintf( link_asli, "%s", name);
		
		sprintf( name, "data.html");
		
		return 1;
	}
	
	/* buka file sukses */
	file->len = fweb.fsize;
	file->fd = &fweb;
	
	return 1;
	
	#if 0
	printf2("%s(): %s\r\n", __FUNCTION__, name);
	
#if HTTPD_FS_STATISTICS
  u16_t i = 0;
#endif /* HTTPD_FS_STATISTICS */
  struct httpd_fsdata_file_noconst *f;

  for(f = (struct httpd_fsdata_file_noconst *)HTTPD_FS_ROOT;
      f != NULL;
      f = (struct httpd_fsdata_file_noconst *)f->next) {

    if(httpd_fs_strcmp(name, f->name) == 0) {
      file->data = f->data;
      file->len = f->len;
#if HTTPD_FS_STATISTICS
      ++count[i];
#endif /* HTTPD_FS_STATISTICS */
      return 1;
    }
#if HTTPD_FS_STATISTICS
    ++i;
#endif /* HTTPD_FS_STATISTICS */

  }
  return 0;
  #endif
}
/*-----------------------------------------------------------------------------------*/
void
httpd_fs_init(void)
{
#if HTTPD_FS_STATISTICS
  u16_t i;
  for(i = 0; i < HTTPD_FS_NUMFILES; i++) {
    count[i] = 0;
  }
#endif /* HTTPD_FS_STATISTICS */
}
/*-----------------------------------------------------------------------------------*/
#if HTTPD_FS_STATISTICS
u16_t httpd_fs_count
(char *name)
{
  struct httpd_fsdata_file_noconst *f;
  u16_t i;

  i = 0;
  for(f = (struct httpd_fsdata_file_noconst *)HTTPD_FS_ROOT;
      f != NULL;
      f = (struct httpd_fsdata_file_noconst *)f->next) {

    if(httpd_fs_strcmp(name, f->name) == 0) {
      return count[i];
    }
    ++i;
  }
  return 0;
}
#endif /* HTTPD_FS_STATISTICS */
/*-----------------------------------------------------------------------------------*/
