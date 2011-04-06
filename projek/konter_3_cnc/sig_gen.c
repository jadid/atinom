/* sin example */
#include <stdio.h>
#include <math.h>

#define PI 3.14159265

int main ()
{
	double param, result;
  param = 30;
	
	printf ("7 ^ 3 = %lf\n", pow (param,3));
  printf ("4.73 ^ 12 = %lf\n", pow (4.73,12));
  printf ("32.01 ^ 1.54 = %lf\n", pow (32.01,1.54));
	
	
  
  //result = sin (param*PI/180);
  //printf ("The sine of %lf degrees is %lf.\n", param, result );
  return 0;
}
 
 
/*
#include <stdlib.h>
#include <math.h>

#define x			12
#define maxKonter	0xFFFF
#define pi 			3.142857
#define www			30
#define sudut		(i*pi*www/180)		// 3*pi*30/180
//#define rad			(i*pi*)

int main() {
	int nilai[x];
	int i,j;
	//float sudut = 0.000;
	float sudutrad = 0.000;
	
	//for(i=0; i<3; i++) {
		//nilai[i] = sin(sudut);
		//sudut = i*2.00/x;
		//sudutrad = sudut*180;
		//sudut *= pi;
		
		i=1;
		//printf("%d: sudut: %.3f, sin(%.0f): %.3f\r\n", i+1, sudut, sudutrad, sin((double) sudut) );
		printf("%d: sin(%.3f): %.3f\r\n", i, sudut , sin(sudut) );
	//}
	
	//i=30;
	
	//printf("testing: sin(30): %f\r\n", sin((double)(www*pi)) );

	return 0;
}
//*/
