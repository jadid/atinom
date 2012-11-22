/**
 * \addtogroup apps
 * @{
 */

/**
 * \defgroup webclient Web client
 * @{
 *
 * This example shows a HTTP client that is able to download web pages
 * and files from web servers. It requires a number of callback
 * functions to be implemented by the module that utilizes the code:
 * webclient_datahandler(), webclient_connected(),
 * webclient_timedout(), webclient_aborted(), webclient_closed().
 */

/**
 * \file
 * Implementation of the HTTP client.
 * \author Adam Dunkels <adam@dunkels.com>
 */

/*
 * Copyright (c) 2002, Adam Dunkels.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: webclient.c,v 1.2 2006/06/11 21:46:37 adam Exp $
 *
 */

//#if 1
#include "FreeRTOS.h"
#include "uip.h"
#include "uiplib.h"
#include "webclient.h"
//#include "resolv.h"

#include <string.h>

#ifdef PAKAI_WEBCLIENT


#define uchr unsigned char
#define WEBCLIENT_TIMEOUT 1000

#define WEBCLIENT_STATE_STATUSLINE 0
#define WEBCLIENT_STATE_HEADERS    1
#define WEBCLIENT_STATE_DATA       2
#define WEBCLIENT_STATE_CLOSE      3

#define HTTPFLAG_NONE   0
#define HTTPFLAG_OK     1
#define HTTPFLAG_MOVED  2
#define HTTPFLAG_ERROR  3


#define ISO_nl       0x0a
#define ISO_cr       0x0d
#define ISO_space    0x20

int kirimURL=0;
char terkirimURL=0;
int noawal;

static struct webclient_state s;

extern int noawal;

/*-----------------------------------------------------------------------------------*/
char *
webclient_mimetype(void)
{
  return s.mimetype;
}
/*-----------------------------------------------------------------------------------*/
char *
webclient_filename(void)
{
  return s.file;
}
/*-----------------------------------------------------------------------------------*/
char *
webclient_hostname(void)
{
  return s.host;
}
/*-----------------------------------------------------------------------------------*/
unsigned short
webclient_port(void)
{
  return s.port;
}
/*-----------------------------------------------------------------------------------*/
void webclient_init(void) {
	kirimURL=0;
	//uip_listen(HTONS(PORT_HTTP));
	
}
/*-----------------------------------------------------------------------------------*/
static void init_connection(void) {
  s.state = WEBCLIENT_STATE_STATUSLINE;

	#if 0
		printf("%s() masuk\r\n", __FUNCTION__);
	#endif

  s.getrequestleft = sizeof(http_get) - 1 + 1 +
    sizeof(http_10) - 1 +
    sizeof(http_crnl) - 1 +
    sizeof(http_host) - 1 +
    sizeof(http_crnl) - 1 +
    strlen(http_user_agent_fields) +
    strlen(s.file) + strlen(s.host);
  s.getrequestptr = 0;

  s.httpheaderlineptr = 0;
}
/*-----------------------------------------------------------------------------------*/
void
webclient_close(void)
{
  s.state = WEBCLIENT_STATE_CLOSE;
}

/*-----------------------------------------------------------------------------------*/
//unsigned char webclient_get(char *host, u16_t port, char *file) __attribute__ ((naked));


unsigned char webclient_get(char *host, u16_t port, char *file)		{
	int gg=0;
  struct uip_conn *conn;
  uip_ipaddr_t *ipaddr;
  static uip_ipaddr_t addr;

#ifndef PAKAI_RESOLV
  uip_ipaddr_t ip_modul;
	unsigned int ret_ip;
	ret_ip = baca_ip(host);	

  //uip_ipaddr(ip_modul, 192,168, 1, 75);
  uip_ipaddr(ip_modul, (uchr)(ret_ip >> 24), (uchr)(ret_ip >> 16), (uchr)(ret_ip >> 8), (uchr)(ret_ip));
#endif
  /* First check if the host is an IP address. */
  ipaddr = &addr; 
  
  #ifdef PAKAI_RESOLV
  
  if(uiplib_ipaddrconv(host, (unsigned char *)addr) == 0) {
    ipaddr = (uip_ipaddr_t *)resolv_lookup(host);
    
    if(ipaddr == NULL) {
      return 0;
    }
  }
  #endif

#ifdef PAKAI_RESOLV
	conn = uip_connect(ipaddr, htons(port));
#else
	conn = uip_connect( ip_modul, htons(port));
#endif

	if (conn == NULL) {
		return 0;
	} else {
		//printf("uip_connect bukan NULL\r\n");
	}

	s.port = port;
	strncpy(s.file, file, sizeof(s.file));
	strncpy(s.host, host, sizeof(s.host));
  
	//printf("___%s(): host: %s, port %d, %s, strlen(s.file): %d\r\n", __FUNCTION__,host, port, file, strlen(s.file));
  
	init_connection();

	#if 0
	//printf("rport: %d\r\n",uip_conn->rport);
	//printf("port: %d, host: %s, file: %s\r\n",s.port, s.host, s.file);

	//printf("___%s(): %s, host %s, port %d, file: %s, pjg: %d, sizeof(s.file): %d\r\n", __FUNCTION__,host, port, s.file, strlen(s.file), sizeof(s.file));
  
	printf("___%s(): host: %s, port %d, %s, strlen(s.file): %d, len: %d\r\n", __FUNCTION__,host, port, file, strlen(s.file), uip_len);

	/*
	for (gg=0; gg<40; gg++) {
		if (gg%16==0) { 
			printf("\r\n");
		} else if (gg%8==0) {
			printf("  "); 
		}
		printf("%02x ", uip_buf[gg]);
	}
	printf("\r\n\r\n");
	//*/
	
	#endif 
	
	return 1;
}
/*-----------------------------------------------------------------------------------*/
static unsigned char *
copy_string(unsigned char *dest,
	    const unsigned char *src, unsigned char len)
{
  strncpy(dest, src, len);
  return dest + len;
}
/*-----------------------------------------------------------------------------------*/
static void
senddata(void)
{
  u16_t len;
  char *getrequest;
  char *cptr;
  
  #if 0
	printf("%s() masuk\r\n", __FUNCTION__);
  #endif
  
  if(s.getrequestleft > 0) {
    cptr = getrequest = (char *)uip_appdata;

    cptr = copy_string(cptr, http_get, sizeof(http_get) - 1);
    cptr = copy_string(cptr, s.file, strlen(s.file));
    *cptr++ = ISO_space;
    cptr = copy_string(cptr, http_10, sizeof(http_10) - 1);

    cptr = copy_string(cptr, http_crnl, sizeof(http_crnl) - 1);
    
    cptr = copy_string(cptr, http_host, sizeof(http_host) - 1);
    cptr = copy_string(cptr, s.host, strlen(s.host));
    cptr = copy_string(cptr, http_crnl, sizeof(http_crnl) - 1);

    cptr = copy_string(cptr, http_user_agent_fields,
		       strlen(http_user_agent_fields));
    
    len = s.getrequestleft > uip_mss()?
      uip_mss():
      s.getrequestleft;
     
    uip_send(&(getrequest[s.getrequestptr]), len);
  }
  //terkirimURL=1;
  //printf("%s(): Terkirim\r\n", __FUNCTION__);
}
/*-----------------------------------------------------------------------------------*/
static void
acked(void)
{
  u16_t len;
  
  if(s.getrequestleft > 0) {
    len = s.getrequestleft > uip_mss()?
      uip_mss():
      s.getrequestleft;
    s.getrequestleft -= len;
    s.getrequestptr += len;
  }
}
/*-----------------------------------------------------------------------------------*/
#if 0
static u16_t
parse_statusline(u16_t len)
{
  char *cptr;
  char *ku = (char *) uip_appdata;
  
  //printf("%s() masuk, len: %d\r\n", __FUNCTION__, len);
  
  while(len > 0 && s.httpheaderlineptr < sizeof(s.httpheaderline)) {
    //s.httpheaderline[s.httpheaderlineptr] = *(char *)uip_appdata;
    //++((char *)uip_appdata);
	//++uip_appdata;
	s.httpheaderline[s.httpheaderlineptr] = *ku;
	++(char *)ku;		
    --len;
	
    if(s.httpheaderline[s.httpheaderlineptr] == ISO_nl) {

      if((strncmp(s.httpheaderline, http_10,
		  sizeof(http_10) - 1) == 0) ||
	 (strncmp(s.httpheaderline, http_11,
		  sizeof(http_11) - 1) == 0)) {
	cptr = &(s.httpheaderline[9]);
	s.httpflag = HTTPFLAG_NONE;
	if(strncmp(cptr, http_200, sizeof(http_200) - 1) == 0) {
	  /* 200 OK */
	  s.httpflag = HTTPFLAG_OK;
	} else if(strncmp(cptr, http_301, sizeof(http_301) - 1) == 0 ||
		  strncmp(cptr, http_302, sizeof(http_302) - 1) == 0) {
	  /* 301 Moved permanently or 302 Found. Location: header line
	     will contain thw new location. */
	  s.httpflag = HTTPFLAG_MOVED;
	} else {
	  s.httpheaderline[s.httpheaderlineptr - 1] = 0;
	}
      } else {
	uip_abort();
	//webclient_aborted();
	//printf("%s(): Aborted\r\n", __FUNCTION__);
	return 0;
      }
      
      /* We're done parsing the status line, so we reset the pointer
	 and start parsing the HTTP headers.*/
      s.httpheaderlineptr = 0;
      s.state = WEBCLIENT_STATE_HEADERS;
      break;
    } else {
      ++s.httpheaderlineptr;
    }
  }
  return len;
}
#endif

#if 1
static u16_t
parse_statusline(u16_t len)
{
  char *cptr;
  char *ku = (char *) uip_appdata;
  
  #if 1
  int potong=0;
  #endif
  
  while(len > 0 && s.httpheaderlineptr < sizeof(s.httpheaderline)) {
    //s.httpheaderline[s.httpheaderlineptr] = *(char *)uip_appdata;
    //++((char *)uip_appdata);
    //--len;
    //printf("%02x ", *ku);
    s.httpheaderline[s.httpheaderlineptr] = *ku;
	++(char *)ku;		
    --len;
    potong++;
    
    
    if(s.httpheaderline[s.httpheaderlineptr] == ISO_nl) {
		s.nHeader = potong;
		//printf("\r\n potong: %d, ISO_NL: len: %d, snHeader: %d\r\n", potong, len, s.nHeader);
      if((strncmp(s.httpheaderline, http_10,
		  sizeof(http_10) - 1) == 0) ||
	 (strncmp(s.httpheaderline, http_11,
		  sizeof(http_11) - 1) == 0)) {
	cptr = &(s.httpheaderline[9]);
	s.httpflag = HTTPFLAG_NONE;
	if(strncmp(cptr, http_200, sizeof(http_200) - 1) == 0) {
	  /* 200 OK */
	  s.httpflag = HTTPFLAG_OK;
	} else if(strncmp(cptr, http_301, sizeof(http_301) - 1) == 0 ||
		  strncmp(cptr, http_302, sizeof(http_302) - 1) == 0) {
	  /* 301 Moved permanently or 302 Found. Location: header line
	     will contain thw new location. */
	  s.httpflag = HTTPFLAG_MOVED;
	} else {
	  s.httpheaderline[s.httpheaderlineptr - 1] = 0;
	}
      } else {
	uip_abort();
	//webclient_aborted();
	return 0;
      }
      
      /* We're done parsing the status line, so we reset the pointer
	 and start parsing the HTTP headers.*/
      s.httpheaderlineptr = 0;
      s.state = WEBCLIENT_STATE_HEADERS;
      break;
    } else {
      ++s.httpheaderlineptr;
    }
  }
  //printf("len akhir: %d, state: %d\r\n", len, s.state);
  return len;
}

#endif
/*-----------------------------------------------------------------------------------*/
static char
casecmp(char *str1, const char *str2, char len)
{
  static char c;
  
  while(len > 0) {
    c = *str1;
    /* Force lower-case characters. */
    if(c & 0x40) {
      c |= 0x20;
    }
    if(*str2 != c) {
      return 1;
    }
    ++str1;
    ++str2;
    --len;
  }
  return 0;
}
/*-----------------------------------------------------------------------------------*/
static u16_t
parse_headers(u16_t len)
{
	#if 0
		printf("%s() masuk: %d\r\n", __FUNCTION__, len);
		
	#endif
  char *cptr;
  char *ku = (char *) uip_appdata;
  static unsigned char i;
  int potong=0;
  
  while(len > 0 && s.httpheaderlineptr < sizeof(s.httpheaderline)) {
	  
	  //printf("%02x ", *ku);
	  
    //s.httpheaderline[s.httpheaderlineptr] = *(char *)uip_appdata;
    //++((char *)uip_appdata);
	//++uip_appdata;
	s.httpheaderline[s.httpheaderlineptr] = *ku;
	++(char *)ku;	
    --len;
    potong++;
    
    
    if(s.httpheaderline[s.httpheaderlineptr] == ISO_nl) {
		s.nData = potong;
		//printf("potong: %d, ISO_NL: len: %d, snData: %d", potong, len, s.nData);
		
      /* We have an entire HTTP header line in s.httpheaderline, so
	 we parse it. */
      if(s.httpheaderline[0] == ISO_cr) {
	/* This was the last header line (i.e., and empty "\r\n"), so
	   we are done with the headers and proceed with the actual
	   data. */
	s.state = WEBCLIENT_STATE_DATA;
	return len;
      }

      s.httpheaderline[s.httpheaderlineptr - 1] = 0;
      /* Check for specific HTTP header fields. */
      if(casecmp(s.httpheaderline, http_content_type,
		     sizeof(http_content_type) - 1) == 0) {
	/* Found Content-type field. */
	cptr = strchr(s.httpheaderline, ';');
	if(cptr != NULL) {
	  *cptr = 0;
	}
	strncpy(s.mimetype, s.httpheaderline +
		sizeof(http_content_type) - 1, sizeof(s.mimetype));
      } else if(casecmp(s.httpheaderline, http_location,
			    sizeof(http_location) - 1) == 0) {
	cptr = s.httpheaderline +
	  sizeof(http_location) - 1;
	
	if(strncmp(cptr, http_http, 7) == 0) {
	  cptr += 7;
	  for(i = 0; i < s.httpheaderlineptr - 7; ++i) {
	    if(*cptr == 0 ||
	       *cptr == '/' ||
	       *cptr == ' ' ||
	       *cptr == ':') {
	      s.host[i] = 0;
	      break;
	    }
	    s.host[i] = *cptr;
	    ++cptr;
	  }
	}
	strncpy(s.file, cptr, sizeof(s.file));
	/*	s.file[s.httpheaderlineptr - i] = 0;*/
      }


      /* We're done parsing, so we reset the pointer and start the
	 next line. */
      s.httpheaderlineptr = 0;
    } else {
      ++s.httpheaderlineptr;
    }
  }
  return len;
}
/*-----------------------------------------------------------------------------------*/
static void
newdata(void)
{
  u16_t len;

  len = uip_datalen();

	#if 0
		printf("____________________________%s() masuk, s.state: %d, len: %d : %d\r\n", __FUNCTION__, s.state, len, uip_datalen());
	#endif

  if(s.state == WEBCLIENT_STATE_STATUSLINE) {				// 0
    len = parse_statusline(len);
	//printf("len : %d : WEBCLIENT_STATE_STATUSLINE\r\n", len);
  }
  
  if(s.state == WEBCLIENT_STATE_HEADERS && len > 0) {		// 1
    len = parse_headers(len);
    //printf("len : %d : WEBCLIENT_STATE_HEADERS\r\n", len);
  }

  if(len > 0 && s.state == WEBCLIENT_STATE_DATA &&
     s.httpflag != HTTPFLAG_MOVED) {
    webclient_datahandler((char *)uip_appdata, len);		// 2
    //printf("len : %d : WEBCLIENT_STATE_DATA\r\n", len);
  }
}
/*-----------------------------------------------------------------------------------*/
void
webclient_appcall(void)
{
	//printf("%s() masuk\r\n", __FUNCTION__);
  if(uip_connected()) {
    s.timer = 0;
    s.state = WEBCLIENT_STATE_STATUSLINE;
    senddata();
    //webclient_connected();
    //printf("%s(): Connected\r\n", __FUNCTION__);
	return;
  }

  if(s.state == WEBCLIENT_STATE_CLOSE) {
    //webclient_closed();
    //printf("%s(): Closed\r\n", __FUNCTION__);
	uip_abort();
    return;
  }

  if(uip_aborted()) {	 
    //webclient_aborted();
	//printf("%s(): Aborted\r\n", __FUNCTION__);
  }
  if(uip_timedout()) {
    //webclient_timedout();
	//printf("%s(): Timeout 1\r\n", __FUNCTION__);
  }

  
  if(uip_acked()) {
    s.timer = 0;
    acked();
  }
  if(uip_newdata()) {
	  //printf("%s() masuk .... ke f newdata\r\n", __FUNCTION__);
    s.timer = 0;
    newdata();
  }
  if(uip_rexmit() ||
     uip_newdata() ||
     uip_acked()) {
    senddata();
  } else if(uip_poll()) {
    ++s.timer;
    if(s.timer == WEBCLIENT_TIMEOUT) {
      //webclient_timedout();
	  //printf("%s(): Timeout 2\r\n", __FUNCTION__);
      uip_abort();
      return;
    }
        /*    senddata();*/
  }

  if(uip_closed()) {
	//printf("%s(): uip closed\r\n", __FUNCTION__);
    if(s.httpflag != HTTPFLAG_MOVED) {
      /* Send NULL data to signal EOF. */
      webclient_datahandler(NULL, 0);
    } else {
	  /*
	  if(resolv_lookup(s.host) == NULL) {
		resolv_query(s.host);
      }*/
      webclient_get(s.host, s.port, s.file);
    }
  }
}
/*-----------------------------------------------------------------------------------*/

/** @} */
/** @} */

void tulis_foto(char *data, unsigned int len);

void webclient_connected() {
	//printf("%s() masuk\r\n", __FUNCTION__);
}

	void parsing_cmd(char * cmd, int len)	{
		//printf("-------------------> %s\r\n", __FUNCTION__);
		//char *isi, *rel, *header;
		char *pch, *pch2, *pch3, *srelay, *swaktu;
		int xData=0, nLoop;
		//int inc;

		pch=strstr(cmd,"OK");
		
		if (pch==NULL)	{
			//printf("Respoon : TIDAK OK !!!\r\n");
			return;
		}
		
		//printf("MANTAP !!!\r\n");
		
		pch2 = strtok (pch,"#");
		while (pch2 != NULL)	{
			//printf ("%s\n",pch2);
			pch3 = strchr(pch2,':')+1;
			if (strstr(pch2,"c") != NULL)  	nLoop = atoi(pch3);
			if (strstr(pch2,"data") != NULL) 	xData = atoi(pch3);//	printf("---> pch3 : %s\r\n", pch3);
			if (strstr(pch2,"relay") != NULL) 	srelay = pch3;//	printf("---> pch3 : %s\r\n", pch3);
			if (strstr(pch2,"time") != NULL) 	swaktu = pch3;//	printf("---> pch3 : %s\r\n", pch3);
			
			pch2 = strtok (NULL, "#");
		}
		
		#ifdef PAKAI_RELAY
		set_relaynya(xData, srelay);
		#endif

		return;
	}


void webclient_datahandler(char *data, u16_t len)
{
	#if 0
		printf("%s() masuk\r\n", __FUNCTION__);
	#endif
	//tulis_foto( data, len );
	
	int panjang = uip_len-s.nData-1;

	#ifdef WEBCLIENT_DATA

	if (len>0)	{
		char *isi = &data[s.nData];
		isi[panjang+1] = 0;
		
		//printf("panjang : %d\r\n", panjang);
		//printf("isinya: %c %c %c\r\n", isi[0], isi[1], isi[2]);
		//printf("isinya: %c %c %c\r\n", isi[panjang-2], isi[panjang-1], isi[panjang]);
		//printf("isi: %s\r\n");		// klo diprintf malah ngehang !!!
		//#ifdef PAKAI_RELAY
		parsing_cmd(isi, panjang);		// file @ app/relay/relay.c
		//#endif

	#if 0
		int wli=0, eqe=64, awal, akhir;
		awal  = len;
		akhir = uip_len - len - 54;

		printf("ada data masuk : %d : %d\r\n", len, uip_len);
		for (wli=0; wli<s.nHeader; wli++) {
			printf("%c", data[wli]);
		}
		printf("-------------------------------------------------\r\n");
		printf("Ganti ......!!! Header !!\r\n");
		for (wli=s.nHeader; wli<s.nData; wli++) {
			printf("%c", data[wli]);
		}
		/*
		for (wli=s.nHeader; wli<s.nData; wli++) {
			printf("%02x ", data[wli]);
		}
		//*/
		printf("-------------------------------------------------\r\n");		
		for (wli=s.nData; wli<uip_len; wli++) {
			printf("%c", data[wli]);
		}
	#endif
	
	#if 0	
		//ldata = uip_len-len;
		printf("ada data masuk : %d : %d\r\n", len, uip_len);
		for (wli=0; wli<len; wli++) {
			printf("%02x ", data[wli]);
			printf("%c", data[wli]);
//*
			eqe--;
			if ((eqe%7)==0)
				printf(" ");
			if (eqe==0)		{
				eqe = 16;
				printf("\r\n");
			}
//*/
		}
	#endif
	}
	//printf("\r\n+++++++++++\r\n");
	#endif	
}
#endif
