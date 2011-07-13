/**
 * \addtogroup apps
 * @{
 */

/**
 * \defgroup httpd Web server
 * @{
 * The uIP web server is a very simplistic implementation of an HTTP
 * server. It can serve web pages and files from a read-only ROM
 * filesystem, and provides a very small scripting language.

 */

/**
 * \file
 *         Web server
 * \author
 *         Adam Dunkels <adam@sics.se>
 */


/*
 * Copyright (c) 2004, Adam Dunkels.
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
 * This file is part of the uIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 * $Id: httpd.c,v 1.2 2006/06/11 21:46:38 adam Exp $
 */



//#ifdef PAKAI_HTTP
//#if 1

#include "uip.h"
#include "httpd.h"
#include "httpd-fs.h"
#include "httpd-cgi.h"
#include "http-strings.h"

#include <string.h>

#if defined(PAKAI_ETH) && defined(PAKAI_HTTP)
#include "buat_file.c"

#define STATE_WAITING 0
#define STATE_OUTPUT  1

#define ISO_nl      0x0a
#define ISO_space   0x20
#define ISO_bang    0x21
#define ISO_percent 0x25
#define ISO_period  0x2e
#define ISO_slash   0x2f
#define ISO_colon   0x3a

/*---------------------------------------------------------------------------*/
#ifdef BOARD_TAMPILAN
	//#define BESAR_BUF_HTTP	1024  //	8192
#else
	//#define BESAR_BUF_HTTP	8192  //	8192
#endif
//extern unsigned char head_buf[1024] 				; /*__attribute__ ((section (".eth_test"))); */
extern unsigned char tot_buf[BESAR_BUF_HTTP] 		__attribute__ ((section (".index_text")));

static unsigned short
generate_part_of_file(void *state)
{
  struct httpd_state *s = (struct httpd_state *)state;
	int terbaca;

  if(s->file.len > uip_mss()) {
    s->len = uip_mss();
  } else {
    s->len = s->file.len;
  }
  memcpy(uip_appdata, s->file.data, s->len);
  
  #ifdef PAKAI_FILE_SIMPAN
  //if ( s->file.fd == NULL )
  if ( s->file.flag == 29 )
  {
		if ( s->file.flag == 29 )
		{
			if (http_fs_baca_dir( uip_appdata, &terbaca, s) == 0)
			{
				s->file.len = 0; // supaya tidak dipanggil lagi ! //
			}			
		}
		//else		
		//	memcpy(uip_appdata, s->file.data, s->len);
  }
  else
  {		// baca langsung ke MMC / file system //
		f_read( s->file.fd, uip_appdata, s->len, &terbaca);
		
		// jika sudah habis (EOF) //
		//if (terbaca <= s->file.len)
		{
			//printf(" Close file\r\n");
			//f_close( s->file.fd );
		}
	}
	#endif
  
  return s->len;
}

/*---------------------------------------------------------------------------*/
static
PT_THREAD(send_file(struct httpd_state *s))
{
	
  PSOCK_BEGIN(&s->sout);
	
  do {
    PSOCK_GENERATOR_SEND(&s->sout, generate_part_of_file, s);
    s->file.len -= s->len;
    s->file.data += s->len;
  } while(s->file.len > 0);
	
	#ifdef PAKAI_FILE_SIMPAN
	if ( s->file.flag == 27 )
	{
		//printf(" Close file\r\n");
		
		f_close( s->file.fd );
		
    }
    #endif
	
  PSOCK_END(&s->sout);
}
/*-------------------- INI dipanggil oleh script ---------------------------*/
static
PT_THREAD(send_part_of_file(struct httpd_state *s))
{
  PSOCK_BEGIN(&s->sout);

  PSOCK_SEND(&s->sout, s->file.data, s->len);
  
  PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/

//int nEth = 0;

static void
next_scriptstate(struct httpd_state *s)
{
  char *p;
  p = strchr(s->scriptptr, ISO_nl) + 1;
  s->scriptlen -= (unsigned short)(p - s->scriptptr);
  s->scriptptr = p;
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(handle_script(struct httpd_state *s))
{
  char *ptr;
  
  PT_BEGIN(&s->scriptpt);


  while(s->file.len > 0) {

    /* Check if we should start executing a script. */
    if(*s->file.data == ISO_percent &&
       *(s->file.data + 1) == ISO_bang) {
      s->scriptptr = s->file.data + 3;
      s->scriptlen = s->file.len - 3;
      if(*(s->scriptptr - 1) == ISO_colon) {
	httpd_fs_open(s->scriptptr + 1, &s->file);
	PT_WAIT_THREAD(&s->scriptpt, send_file(s));
      } else {
	PT_WAIT_THREAD(&s->scriptpt,
		       httpd_cgi(s->scriptptr)(s, s->scriptptr));
      }
      next_scriptstate(s);
      
      /* The script is over, so we reset the pointers and continue
	 sending the rest of the file. */
      s->file.data = s->scriptptr;
      s->file.len = s->scriptlen;
    } else {
      /* See if we find the start of script marker in the block of HTML
	 to be sent. */

      if(s->file.len > uip_mss()) {
	s->len = uip_mss();
      } else {
	s->len = s->file.len;
      }

      if(*s->file.data == ISO_percent) {
	ptr = strchr(s->file.data + 1, ISO_percent);
      } else {
	ptr = strchr(s->file.data, ISO_percent);
      }
      if(ptr != NULL &&
	 ptr != s->file.data) {
	s->len = (int)(ptr - s->file.data);
	if(s->len >= uip_mss()) {
	  s->len = uip_mss();
	}
      }
      PT_WAIT_THREAD(&s->scriptpt, send_part_of_file(s));
      s->file.data += s->len;
      s->file.len -= s->len;
      
    }
  }
  
  PT_END(&s->scriptpt);
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(send_headers(struct httpd_state *s, const char *statushdr))
{
  char *ptr;

  PSOCK_BEGIN(&s->sout);

  PSOCK_SEND_STR(&s->sout, statushdr);

  ptr = strrchr(s->filename, ISO_period);
  if(ptr == NULL) {
    PSOCK_SEND_STR(&s->sout, http_content_type_binary);
  } else if(strncmp(http_html, ptr, 5) == 0 ||
	    strncmp(http_shtml, ptr, 6) == 0) {
    PSOCK_SEND_STR(&s->sout, http_content_type_html);
  } else if(strncmp(http_css, ptr, 4) == 0) {
    PSOCK_SEND_STR(&s->sout, http_content_type_css);
  } else if(strncmp(http_png, ptr, 4) == 0) {
    PSOCK_SEND_STR(&s->sout, http_content_type_png);
  } else if(strncmp(http_gif, ptr, 4) == 0) {
    PSOCK_SEND_STR(&s->sout, http_content_type_gif);
  } else if(strncmp(http_jpg, ptr, 4) == 0) {
    PSOCK_SEND_STR(&s->sout, http_content_type_jpg);
  } else {
    PSOCK_SEND_STR(&s->sout, http_content_type_plain);
  }
  PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(handle_output(struct httpd_state *s))
{
	char *ptr;
  
	PT_BEGIN(&s->outputpt);

	if(!httpd_fs_open(s->filename, &s->file)) {
		//printf(" HTTP file %s tidak ada\n", s->filename);
		httpd_fs_open(http_404_html, &s->file);
    	strcpy(s->filename, http_404_html);
    	PT_WAIT_THREAD(&s->outputpt,
			   send_headers(s,
			   http_header_404));
	    PT_WAIT_THREAD(&s->outputpt,
			   send_file(s));

	} else {
		//printf("%s() ... else %d\r\n", s->filename, strlen(tot_buf));
#if 0
		if (strncmp(s->filename, "/about", 6) == 0) {
			printf(" Buat file about\r\n");
			
			buat_file_about();
			//printf("isi: %s\r\n", tot_buf);
			//buat_file_about1();
			
			s->file.len = strlen(tot_buf);
			
			portENTER_CRITICAL();
			s->file.data = tot_buf;
			portEXIT_CRITICAL();
		}
#endif

//*	
		#ifdef PAKAI_HTTP
		if (strncmp(s->filename, "/about", 6) == 0) {
			//printf(" Buat file about\r\n");
			
			buat_file_about();
			s->file.len = strlen(tot_buf);
			
			portENTER_CRITICAL();
			s->file.data = tot_buf;
			portEXIT_CRITICAL();
		}

		else if (strncmp(s->filename, "/setting", 8) == 0) {
			//printf(" str: %s\r\n", s->filename);
			if (strncmp(s->filename,"/setting.html?u=1",17)==0) {
				ganti_setting(s->filename);
				buat_file_setting(1, s->filename);
			} else if (strncmp(s->filename,"/setting.html?smb=1",19)==0) {
				buat_file_setting(2, s->filename);
			} else if (strncmp(s->filename,"/setting.html?smb=3",19)==0) {
				buat_file_setting(3, s->filename);
			} else if (strncmp(s->filename,"/setting.html?smb=4",19)==0) {
				buat_file_setting(4, s->filename);
			#ifdef TES_GET_WEB
			} else if (strncmp(s->filename,"/setting.html?smb=5",19)==0) {
				buat_file_setting(5, s->filename);
			#endif
			} else {
				buat_file_setting(0,"");
			}

			s->file.len = strlen(tot_buf);
			
			portENTER_CRITICAL();
			s->file.data = tot_buf;
			portEXIT_CRITICAL();

		}
		#ifdef PAKAI_PM
		else if (strncmp(s->filename, "/index.html?sbr=1", 17) == 0) {
			//printf("buat file index\r\n di PAKAI PM\r\n");
			buat_file_index(1, s->filename);
			s->file.len = strlen(tot_buf);
			portENTER_CRITICAL();
			s->file.data = tot_buf;
			portEXIT_CRITICAL();

		} 
		#endif
		else {
			//printf(" Buat file index @else, strlen(tot_buf): %d\r\n", strlen(tot_buf));
			//buat_file_index(0, "");
			buat_file_index(0, s->filename);
			//printf("isi tot_buf:\r\n%s\r\n%s", tot_buf, s->filename);
			//*
			s->file.len = strlen(tot_buf);
			portENTER_CRITICAL();
			s->file.data = tot_buf;
			portEXIT_CRITICAL();

		}
		#endif
			//*/
	
		//printf("sampai sini ...");
		PT_WAIT_THREAD(&s->outputpt, send_headers(s, http_header_200));
		ptr = strchr(s->filename, ISO_period);
		
		if(ptr != NULL && strncmp(ptr, http_shtml, 6) == 0) {
			//printf(" script !\n");
			PT_INIT(&s->scriptpt);
			PT_WAIT_THREAD(&s->outputpt, handle_script(s));
		} else {
			//printf(" biasa !\n");
			PT_WAIT_THREAD(&s->outputpt, send_file(s));
		}
		
	}
	
	//printf("sock close ...");
	PSOCK_CLOSE(&s->sout);
	PT_END(&s->outputpt);
	//printf("pt end\r\n");
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(handle_input(struct httpd_state *s))
{
  PSOCK_BEGIN(&s->sin);

  PSOCK_READTO(&s->sin, ISO_space);
	//printf("http 1: %s\r\n", s->inputbuf);
  if(strncmp(s->inputbuf, http_get, 4) != 0) {
	
    PSOCK_CLOSE_EXIT(&s->sin);
  }
  PSOCK_READTO(&s->sin, ISO_space);
	//printf("http 2: %s\r\n", s->inputbuf);

  if(s->inputbuf[0] != ISO_slash) {
    PSOCK_CLOSE_EXIT(&s->sin);
  }

  if(s->inputbuf[1] == ISO_space) {
    strncpy(s->filename, http_index_html, sizeof(s->filename));
  } else {
    s->inputbuf[PSOCK_DATALEN(&s->sin) - 1] = 0;
    strncpy(s->filename, &s->inputbuf[0], sizeof(s->filename));
  }

  //*  
   //httpd_log_file(uip_conn->ripaddr, s->filename);
  //*/
  
  s->state = STATE_OUTPUT;

  while(1) {
    PSOCK_READTO(&s->sin, ISO_nl);

    if(strncmp(s->inputbuf, http_referer, 8) == 0) {
      s->inputbuf[PSOCK_DATALEN(&s->sin) - 2] = 0;
      /*      httpd_log(&s->inputbuf[9]);*/
    }
  }
  
  PSOCK_END(&s->sin);
  //nEth = 0;
}
/*---------------------------------------------------------------------------*/


static void
handle_connection(struct httpd_state *s)
{

	handle_input(s);
	if(s->state == STATE_OUTPUT && strcmp(s->filename, "/favicon.ico")) {
		handle_output(s);
  }	
}
/*---------------------------------------------------------------------------*/


void httpd_appcall(void)
{
  struct httpd_state *s = (struct httpd_state *)&(uip_conn->appstate2);

  if(uip_closed() || uip_aborted() || uip_timedout()) {
  } else if(uip_connected()) {
    PSOCK_INIT(&s->sin, s->inputbuf, sizeof(s->inputbuf) - 1);
    PSOCK_INIT(&s->sout, s->inputbuf, sizeof(s->inputbuf) - 1);
    PT_INIT(&s->outputpt);
    s->state = STATE_WAITING;
    /*    timer_set(&s->timer, CLOCK_SECOND * 100);*/
    s->timer = 0;
    handle_connection(s);
  } else if(s != NULL) {
    if(uip_poll()) {
      ++s->timer;
      if(s->timer >= 20) {
	uip_abort();
      }
    } else {
      s->timer = 0;
    }
    handle_connection(s);
  } else {
    uip_abort();
  }
}
/*---------------------------------------------------------------------------*/
/**
 * \brief      Initialize the web server
 *
 *             This function initializes the web server and should be
 *             called at system boot-up.
 */
#ifdef PAKAI_HTTP
void
httpd_init(void)
{
  uip_listen(HTONS(PORT_HTTP));
}
#endif
/*---------------------------------------------------------------------------*/
/** @} */

//#endif
#endif
