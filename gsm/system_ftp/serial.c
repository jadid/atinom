/*
	bagian2 serialnya
	supaya jika di port ke arm tinggal bagian ini saja
	yang diganti
	
	furkan jadid
	30 des 2009, daun biru engineering
	
	11 Maret 2010
	untuk LPC2387, ini akan dipanggil oleh
	task led_utama, maka watchdog harus ditendang disini
	
	*/
int fd = 0;
	
#if ( PAKAI_LINUX == 1)
static wait_flag;
static struct termios oldtio,newtio;
static struct sigaction saio;           /* definition of signal action */

void signal_handler_IO (int status);
#else

//#define read(a, b, c) ser2_getchar(a, b, 1000);	\
//		tendang_wdog();

int write(int a, char *bf, int len)
{
	int i;
	
	for (i=0; i<len; i++)
	{
		xSerialPutChar2( 0, *bf++, 1000 );
	}
}

#endif

#define BAUDRATE B115200

int buka_serial(char *dev)
{
	//fd = 0;
	
	/* open the device to be non-blocking (read will return immediatly) */
    //fd = open( dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
	
	#if (DEBUG == 1)
	printf(" %s(): %s\r\n", __FUNCTION__, dev);
	#endif
	
	#if ( PAKAI_LINUX == 1)
    fd = open( dev, O_RDWR | O_NOCTTY );
	
	if (fd <0) {
    	
    	printf("Tidak dapat buka port %s\r\n", dev);
    	printf("Port serial kemungkinan salah !\r\n");
		printf("gunakan -d /dev/ttyXXX untuk port yang lain !\r\n");
    	
    	sleep(1);    	
    	perror(dev); 
		exit(-1); 
    }
	
	#if 0 /* asyncron */
	/* install the signal handler before making the device asynchronous */
    saio.sa_handler = signal_handler_IO;
    saio.sa_flags = 0;
    saio.sa_restorer = NULL;
    sigaction(SIGIO,&saio,NULL);
          
    /* allow the process to receive SIGIO */
    fcntl(fd, F_SETOWN, getpid());
    /* Make the file descriptor asynchronous (the manual page says only 
       O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
    fcntl(fd, F_SETFL, FASYNC);
        
    tcgetattr(fd,&oldtio); /* save current port settings */
    /* set new port settings for canonical input processing */
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    
    newtio.c_oflag = 0;
    newtio.c_lflag = 0;		// non-canonical
    newtio.c_cc[VMIN]=1;	// wait 1 karakter
    newtio.c_cc[VTIME]=0;
    
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&newtio);
	
	wait_flag = true;
	#endif
	
	#if 0 /* Canonical */
		tcgetattr(fd,&oldtio); /* save current serial port settings */
         bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */
        
        /* 
          BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
          CRTSCTS : output hardware flow control (only used if the cable has
                    all necessary lines. See sect. 7 of Serial-HOWTO)
          CS8     : 8n1 (8bit,no parity,1 stopbit)
          CLOCAL  : local connection, no modem contol
          CREAD   : enable receiving characters
        */
         newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
         
        /*
          IGNPAR  : ignore bytes with parity errors
          ICRNL   : map CR to NL (otherwise a CR input on the other computer
                    will not terminate input)
          otherwise make device raw (no other input processing)
        */
         newtio.c_iflag = IGNPAR | ICRNL;
         
        /*
         Raw output.
        */
         newtio.c_oflag = 0;
         
        /*
          ICANON  : enable canonical input
          disable all echo functionality, and don't send signals to calling program
        */
         newtio.c_lflag = ICANON;
         
        /* 
          initialize all control characters 
          default values can be found in /usr/include/termios.h, and are given
          in the comments, but we don't need them here
        */
         newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */ 
         newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
         newtio.c_cc[VERASE]   = 0;     /* del */
         newtio.c_cc[VKILL]    = 0;     /* @ */
         newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */
         //newtio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
         //newtio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
		 newtio.c_cc[VTIME]    = 2;     /* timeout  */
		 newtio.c_cc[VMIN]     = 0;
         newtio.c_cc[VSWTC]    = 0;     /* '\0' */
         newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */ 
         newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
         newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
         newtio.c_cc[VEOL]     = 0;     /* '\0' */
         newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
         newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
         newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
         newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
         newtio.c_cc[VEOL2]    = 0;     /* '\0' */
        
        /* 
          now clean the modem line and activate the settings for the port
        */
         tcflush(fd, TCIFLUSH);
         tcsetattr(fd,TCSANOW,&newtio);
		 
		 
	#endif
	
	#if 1 /* non canonical */
		tcgetattr(fd,&oldtio); /* save current port settings */
        
        bzero(&newtio, sizeof(newtio));
        newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
        newtio.c_iflag = IGNPAR;
        newtio.c_oflag = 0;
        
        /* set input mode (non-canonical, no echo,...) */
        newtio.c_lflag = 0;
         
        newtio.c_cc[VTIME]    = 10;   /* inter-character timer unused */
        newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */
        
        tcflush(fd, TCIFLUSH);
        tcsetattr(fd,TCSANOW,&newtio);
	#endif
	
	#else	/* PAKAI LINUX */
	
	if (fd == 0)
	{
		printf("Init Serial Port 2\r\n");
		serial2_init( 115200, 64);
		fd = 1;
	}
	
	#endif
	//write(fd, buf, strlen(buf));
}


int baca_serial(char *buf, int len, int timeout)
{
	int res;
	int c;
	int tout=0;
	int masuk = 0;
	
	while(len--)
	{
		//res = read( fd, &c, 1);
		#if (PAKAI_GSM_FTP == 1) 
			res = ser1_getchar(fd, &c, 1 );
		#elif (PAKAI_GSM_FTP == 2) 			
			res = ser2_getchar(fd, &c, 1 );
		#elif (PAKAI_GSM_FTP == 3)
			res = ser3_getchar(fd, &c, 1 );
		#endif 
		if (res != 0)
		{
			if ( (char) c == 0x0A || (char) c == 0x0D )
			{
				if (masuk > 0) 
				{	
					/* supaya buffer lama tidak ikut di printout */
					buf[masuk] = 0;
					return 0;
				}
			}
			else
			{
				buf[ masuk ] = (char ) c;								
				masuk++;
				
				#if (DEBUG == 1)
				printf(" %s(): res=%d : msk=%02d : 0x%02X : %c\r\n", __FUNCTION__, res, masuk, (char) c, (char) c);
				#endif
				/*
				if ( (char) c == 0x0A && masuk > 2)
				{
					if (buf[ masuk - 2 ] == 0x0D)
					return 0;
				}*/
			}
		}
		else
		{
			printf(" %s(): %d :timeout\r\n", __FUNCTION__, tout);
			len++;
			tout++;
			if ( tout > timeout) 
				return 0;
				//return -1;
		}
	}
}

int tulis_serial(char *buf, int len, int timeout)
{
	
	#if (DEBUG == 1)
	printf(" %s(): len=%d : %s\n", __FUNCTION__, len, buf);
	#endif
	
	write(fd, buf, len);
}

int tulis_char(char c)
{
	write(fd, &c, 1);
}

int tutup_serial()	{
	#if ( PAKAI_LINUX == 1)
	tcsetattr(fd,TCSANOW,&oldtio);
	close( fd );
	#endif
	//return 1;
}

#if 0
      /***************************************************************************
      * signal handler. sets wait_flag to FALSE, to indicate above loop that     *
      * characters have been received.                                           *
      ***************************************************************************/
        
void signal_handler_IO (int status)
{
   wait_flag = FALSE;
}
#endif
