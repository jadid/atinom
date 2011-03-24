//
//  $Id: telnetd.c 267 2008-10-31 03:29:52Z jcw $
//  $Revision: 267 $
//  $Author: jcw $
//  $Date: 2008-10-30 23:29:52 -0400 (Thu, 30 Oct 2008) $
//  $HeadURL: http://tinymicros.com/svn_public/arm/lpc2148_demo/trunk/uip/apps/telnetd/telnetd.c $
//

/*
 * Copyright (c) 2003, Adam Dunkels.
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
 * This file is part of the uIP TCP/IP stack
 *
 * $Id: telnetd.c 267 2008-10-31 03:29:52Z jcw $
 *
 */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include <string.h>

#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y)((x)>(y)?(x):(y))

#define CFG_CONSOLE_UART0

#define HELLO_1 "Monita Online Monitoring System \n"
#define HELLO_2 "Modul "
#define HELLO_3	"Welcome ...\n \n \n"

#define LOG_PASS "Passwd : "

#if 0
#if defined CFG_CONSOLE_USB
#include "../../usbser/usbser.h"
#elif defined CFG_CONSOLE_UART0
#include "../../uart/uart0.h"
#elif defined CFG_CONSOLE_UART1
#include "../../uart/uart1.h"
#else
#error "Eeek!  No console devices defined!"
#endif

#include "../../../main.h"
#endif
#include "../../uip/uip.h"
#include "../../lib/memb.h"
#include "telnetd.h"

#include "../../tinysh/enviro.h"
//extern struct t_env env2;

//
//
//
#define ISO_nl 0x0a
#define ISO_cr 0x0d

#define TELNET_IAC   255
#define TELNET_WILL  251
#define TELNET_WONT  252
#define TELNET_DO    253
#define TELNET_DONT  254

//
//
//
typedef struct telnetdBuf_s
{
  char bufmem [TELNETD_CONF_NUMLINES * TELNETD_CONF_LINELEN];
  int ptr;
  int size;
}
telnetdBuf_t;

//
//
//
static telnetdState_t *telnetdState = NULL;
static telnetdBuf_t telnetdBuf;
static xQueueHandle telnetdQueueTx;
static xQueueHandle consoleQueue = NULL;

static unsigned int poll_no_komand = 0;
static unsigned int passwd_benar = 0;

//
//  Useful for debugging, as it disables printing to the telnet client
//
#if 0
#include <stdio.h>
#include <stdarg.h>

static void tprintf (char *fmt, ...)
{
  va_list ap;
  telnetdState_t *telnetdStateTemp = telnetdState;

  telnetdState = NULL;
  va_start (ap, fmt);
  vprintf (fmt, ap);
  va_end (ap);
  telnetdState = telnetdStateTemp;
}
#endif

//
//
//
static void telnetdBufferInit (telnetdBuf_t *buf)
{ 
  buf->ptr = 0;
  buf->size = TELNETD_CONF_NUMLINES * TELNETD_CONF_LINELEN;
}

//
//
//
static int telnetdBufferAppend (telnetdBuf_t *buf, const char *data, int len)
{ 
  int copylen;

  copylen = MIN (len, buf->size - buf->ptr);
  memcpy (&buf->bufmem [buf->ptr], data, copylen);
  buf->ptr += copylen;

  return copylen;
}

//
//
//
static void telnetBufferCopyTo (telnetdBuf_t *buf, char *to, int len)
{
  memcpy (to, &buf->bufmem [0], len);
}

//
//
//
static void telnetBufferPop (telnetdBuf_t *buf, int len)
{
  int poplen;

  poplen = MIN (len, buf->ptr);
  memcpy (&buf->bufmem [0], &buf->bufmem [poplen], buf->ptr - poplen);
  buf->ptr -= poplen;
}

//
//
//
static int telnetBufferLength (telnetdBuf_t *buf)
{
  return buf->ptr;
}

//
//
//
static int telnetdBufferHasSpace (telnetdBuf_t *buf)
{
  return buf->size - buf->ptr;
}

//
//
//
static void telnetdAcked (void)
{
  if (telnetdState)
    telnetBufferPop (&telnetdBuf, telnetdState->numsent);
}

//
//
//
static void telnetdSendData (void)
{
  if (telnetdState)
  {
    int len;

    len = MIN (telnetBufferLength (&telnetdBuf), uip_mss ());
    telnetBufferCopyTo (&telnetdBuf, uip_appdata, len);
    uip_send (uip_appdata, len);
    telnetdState->numsent = len;
  }
}

//
//
//
static int telnetdOpen (void)
{
  char tt[64];
  struct t_env *envx;
  envx = (char *) ALMT_ENV;
  
  poll_no_komand = 0;
  passwd_benar = 0;
  
  uip_unlisten (HTONS (23));

  telnetdBufferInit (&telnetdBuf);
  telnetdState = (telnetdState_t *) &(uip_conn->appstate2);
  telnetdState->state = TELNETDSTATE_NORMAL;
  
  telnetdBufferAppend (&telnetdBuf, HELLO_1, strlen (HELLO_1));
  telnetdBufferAppend (&telnetdBuf, HELLO_2, strlen (HELLO_2));
    
  sprintf(tt, "%s v%s \n", NAMA_BOARD, VERSI_KOMON);
  telnetdBufferAppend (&telnetdBuf, tt, strlen (tt));
  
  //sprintf(tt, "IP: %d.%d.%d.%d \n", env2.IP0, env2.IP1, env2.IP2, env2.IP3); 
  sprintf(tt, "IP: %d.%d.%d.%d \n", envx->IP0, envx->IP1, envx->IP2, envx->IP3); 
  telnetdBufferAppend (&telnetdBuf, tt, strlen (tt));
  
  telnetdBufferAppend (&telnetdBuf, HELLO_3, strlen (HELLO_3));
  
  //telnetdBufferAppend (&telnetdBuf, PROMPT, strlen (PROMPT));
	telnetdBufferAppend (&telnetdBuf, LOG_PASS, strlen (LOG_PASS));	
	
  return 1;
}

//
//  When a telnet close is requested, we need to read the Tx queue until it
//  returns empty.  This prevents us from hanging on trying to send data to a
//  closed socket.
//
static void telnetdClose (void)
{
  portCHAR c;

  while (xQueueReceive (telnetdQueueTx, &c, 0) == pdTRUE);

  telnetdState = NULL;

  uip_listen (HTONS (23));
}

// fungsi sederhana untuk cek passwd
static int  jum=0;
static char pas[32];

void cek_passwd(char *cc)
{
	if (strncmp(cc, "diesel", 6) == 0)
	{
		passwd_benar = 1;
		telnetdBufferAppend (&telnetdBuf, PROMPT, strlen (PROMPT));
	}
	else
	{
		//printf("SALAH !\r\n");
		telnetdBufferAppend (&telnetdBuf, LOG_PASS, strlen (LOG_PASS));
	}
	jum = 0;
}

//
//  Send a character to the monitor queue to be processed.  CRs are stripped
//  because telnet always sends CR/LF, and we only care about LF.  Post the
//  received character into the monitor queue.
//

static void telnetdNewRxChar (u8_t c)
{ 
  	if (!c || (c == ISO_cr) || !consoleQueue)
    	return;

	poll_no_komand = 0;
	
	if ( passwd_benar == 1)
  		xQueueSend (consoleQueue, &c, portMAX_DELAY);
  	else
  	{
  		pas[jum] = c;
  		jum++;
  		
  		if (jum > 32) 
  		{
  			jum = 0;
  			printf("kepanjangan !\r\n");
  		}
  		
  		if (c == ISO_nl)
  		{
  			
  			//printf("enter ditekan !\r\n");
  			cek_passwd(pas);
  		}	
  	}
}

//
//  See if the monitor has placed any data in the Tx queue, and if so,
//  stuff it into the outgoing telnet buffer.
//
static void telnetdNewTxData (void)
{
  	portCHAR c;
	/* secara reguler dipanggil sekitar 100 ms */
	
	poll_no_komand++;
	if (poll_no_komand > 1800)
	{
		printf(" Tidak ada aktifitas dalam 3 menit\r\n");
		
		telnetdDisconnect();
		poll_no_komand = 0;
	}
	
  	while (telnetdBufferHasSpace (&telnetdBuf) && (xQueueReceive (telnetdQueueTx, &c, 0) == pdTRUE))
    telnetdBufferAppend (&telnetdBuf, (char *) &c, sizeof (c));
  
}

//
//  Telnet will/wont/do/dont formatting
//
static void telnetdSendOpt (u8_t option, u8_t value)
{
  char line [4];

  line [0] = TELNET_IAC;
  line [1] = option;
  line [2] = value;
  line [3] = 0;

  telnetdBufferAppend (&telnetdBuf, line, 4);
}

//
//  New data has been received from the client
//
static void telnetdNewRxData (void)
{
  if (telnetdState)
  {
    u16_t len;
    u8_t c;
    u8_t *dataptr;

    len = uip_datalen ();
    dataptr = (u8_t *) uip_appdata;

    while (len > 0)
    {
      c = *dataptr++;
      len--;

      switch (telnetdState->state) 
      {
        case TELNETDSTATE_NORMAL :
          {
            if (c == TELNET_IAC) 
              telnetdState->state = TELNETDSTATE_IAC;
            else
              telnetdNewRxChar (c);
          }
          break;

        case TELNETDSTATE_IAC :
          {
            if (c == TELNET_IAC) 
            {
              telnetdNewRxChar (c);
              telnetdState->state = TELNETDSTATE_NORMAL;
            } 
            else 
            {
              switch (c) 
              {
                case TELNET_WILL :
                  telnetdState->state = TELNETDSTATE_WILL;
                  break;

                case TELNET_WONT :
                  telnetdState->state = TELNETDSTATE_WONT;
                  break;

                case TELNET_DO :
                  telnetdState->state = TELNETDSTATE_DO;
                  break;

                case TELNET_DONT :
                  telnetdState->state = TELNETDSTATE_DONT;
                  break;

                default :
                  telnetdState->state = TELNETDSTATE_NORMAL;
                  break;
              }
            }
          }
          break;

        case TELNETDSTATE_WILL : /* Reply with a DONT */
          {
            telnetdSendOpt (TELNET_DONT, c);
            telnetdState->state = TELNETDSTATE_NORMAL;
          }
          break;

        case TELNETDSTATE_WONT : /* Reply with a DONT */
          {
            telnetdSendOpt (TELNET_DONT, c);
            telnetdState->state = TELNETDSTATE_NORMAL;
          }
          break;

        case TELNETDSTATE_DO : /* Reply with a WONT */
          {
            telnetdSendOpt (TELNET_WONT, c);
            telnetdState->state = TELNETDSTATE_NORMAL;
          }
          break;

        case TELNETDSTATE_DONT : /* Reply with a WONT */
          {
            telnetdSendOpt (TELNET_WONT, c);
            telnetdState->state = TELNETDSTATE_NORMAL;
          }
          break;

        case TELNETDSTATE_CLOSE :
          break;
      }
    }
  }
}

//
//  Character from monitor task, to be sent to telnet client.  Return pdFALSE
//  if telnet session not open.  Return pdTRUE if session open and character
//  posted.  If the queue doesn't clear for 5 seconds, close the session and
//  return errQUEUE_FULL
//
portBASE_TYPE telnetdPutChar (portCHAR c)
{
  portBASE_TYPE r = pdFALSE;

  if (telnetdState && (telnetdState->state == TELNETDSTATE_NORMAL))
    if ((r = xQueueSend (telnetdQueueTx, &c, 5000 / portTICK_RATE_MS)) != pdTRUE)
      telnetdClose ();

  return r;
}

//
//
//
void telnetdDisconnect (void)
{
  if (telnetdState)
    telnetdState->state = TELNETDSTATE_CLOSE;
}

//
//
//
int telnetd_init (void)
{
  	#if 1
  	if (!telnetdQueueTx)
    	if (!(telnetdQueueTx = xQueueCreate (256, (unsigned portBASE_TYPE) sizeof (signed portCHAR))))
      		return 0;
	#endif
	
  uip_listen (HTONS (23));
  telnetdBufferInit (&telnetdBuf);
  telnetdState = NULL;

#if 1
#if defined CFG_CONSOLE_USB
  usbserGetRxQueue (&consoleQueue);
#elif defined CFG_CONSOLE_UART0
  uart0GetRxQueue (&consoleQueue);
#elif defined CFG_CONSOLE_UART1
  uart1GetRxQueue (&consoleQueue);
#else
#error "Eeek!  No console devices defined!"
#endif
#endif

  return 1;
}

static int ping=0;
//
//
//
void telnetd_appcall (void)
{
  	//ping++;
  	if (uip_connected ())
  	{ 
    	//printf(" %d konekted\n", ping);
    	if (!telnetdOpen ()) 
     	{
     		//printf(" %d !Open\n", ping);
     		return;
     	}
  	}

  	if (telnetdState && (telnetdState->state == TELNETDSTATE_CLOSE))
  	{
    	//printf(" %d Close\n", ping);
    	uip_close ();
    }

  	if (uip_closed () || uip_aborted () || uip_timedout ())
 	{
    	//printf(" %d telnet close\n", ping);
    	telnetdClose ();
	}
	
  	if (uip_acked ())
  	{
    	//printf(" %d telnet ack\n", ping);
    	telnetdAcked ();
	}
	
  	if (uip_newdata ())
  	{
    	//printf(" %d new data\n", ping);
    	telnetdNewRxData ();
	}
	
  	if (uip_rexmit () || uip_newdata () || uip_acked () || uip_connected ())
  	{
    	//printf(" %d Send data\n", ping);
    	telnetdSendData ();
  	}
  	else if (uip_poll ())  
  	{
    	//printf(" %d Poll\n", ping);
    	telnetdNewTxData ();
    	telnetdSendData ();
  	}
}
