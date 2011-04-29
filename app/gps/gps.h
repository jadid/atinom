


typedef struct
{
	//Info dasar
	char utc_time[11]; //hhmmss:sss 
	char date[7];//ddmmyy
	char status[2]; //A: Valid, V: Invalid
	
	//Lokasi
	char latitude[10];
	char north_south[2];//Lintang N: North, S: South
	
	char longitude[10];//
	char east_west[2];//Bujur E: East, W: West
	
	//Course
	char speed[10];//dalam knots
	char arah[7];//derajat. 0 derajat berarti Utara
	
	//lain-lain
	char mode_indicator[2]; //A: Autonomous, M: Manual, D: DGPS, E: Dead Reckoning, S: Simulation, N: Data Invalid
	char ceksum[4]; 
} data_gps ;
