#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define program "alarmKeras"
#define P_HIDUP "hidup!"
#define P_MATI  "mati!"
#define P_TES   "tes!"
#define PORT	"/dev/ttyUSB1"

void help() {
	printf("%s untuk mengontrol ALARM rotasi.\n",program);
	printf("Command : %s [perintah]\n", program);
	printf("Perintah\n");
	printf("   %s hidup! << untuk menyalakan ALARM\n", program);
	printf("   %s mati!  << untuk membungkam ALARM\n", program);
	printf("   %s tes!   << untuk melihat koneksi ALARM\n", program);
}

//float RPM=0.0, BB=0.0;
int tot=0;
float BB=0.0;

float nilai_BB() {
	return (BB+=0.3);
}

float nilai_RPM() {
	return (rand()%1000);
}

int main(int argc, char *argv[]) {
	//printf("Jml argc : %d\n", argc);
	//int i,k;
	char perintah [100];
	
	
	//char *perintah = "";
	//char *isi = "";
	
	int fd = open( PORT, O_RDWR );
	int iOut;
	while (1) {
		sleep(1);
		read(fd,perintah, 100);
		if (strncmp(perintah,"s\r", 2)==0) {
			printf("blabnskas\r\n");
			if (fd>0) {
			sprintf(perintah, "RPM %.2f, BB %.1f us, Tot %d\r\n", nilai_RPM(), nilai_BB(), tot++);
			//strcpy(perintah,"masuk dong");
			iOut = write(fd,perintah, strlen(perintah));
		}
		}
		/*
		if (fd>0) {
			sprintf(perintah, "RPM %.2f, BB %.1f us, Tot %d\r\n", nilai_RPM(), nilai_BB(), tot++);
			//strcpy(perintah,"masuk dong");
			iOut = write(fd,perintah, strlen(perintah));
		}
		//*/
	}
}
