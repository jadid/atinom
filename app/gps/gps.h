

typedef struct __attribute__((packed)) {
	//Info dasar
	unsigned char utc_time[11]; //hhmmss:sss 
	unsigned char date[7];//ddmmyy
	unsigned char status[2]; //A: Valid, V: Invalid
	
	//Lokasi
	unsigned char latitude[10];
	unsigned char north_south[2];//Lintang N: North, S: South
	
	unsigned char longitude[10];//
	unsigned char east_west[2];//Bujur E: East, W: West
	
	//Course
	unsigned char speed[10];//dalam knots
	unsigned char arah[7];//derajat. 0 derajat berarti Utara
	
	//lain-lain
	unsigned char mode_indicator[2]; //A: Autonomous, M: Manual, D: DGPS, E: Dead Reckoning, S: Simulation, N: Data Invalid
	unsigned char ceksum[4]; 
} data_gps ;
