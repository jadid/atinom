
   Macam Projek
===================

Direktori Projek merupakan bagian utama dari aplikasi monita, untuk
- kompilasi source code : main.c
- Setting dan konfigurasi : FreeRTOSConfig.h
- nyuntik hasil kompilasi ke board tujuan.

Klasifikasi
Pembagian folder ini berdasarkan
- jenis aplikasi : 
	- pm_sabang untuk board powermeter SAJA (tanpa perlu adc), contoh penyulang lombok taman
	- sabang_lopana untuk monita suhu tekanan (pakai adc), contoh lopana, mesin 7 & 8 ampenan
	- konter untuk board rpm dan onoff pulsa, contoh flowmeter, rpm, LBS
	- gps_sabang untuk board gps SAJA
- turunan board (perbedaan hardware) : 
	- sabang_lopana_2.1 : board versi < 2.2 (pakai ENC28)
	- sabang_lopana_2.3 : board versi >= 2.3 (pakai ENC624)
	- konter_3_2368 : board versi 3 pakai LPC2368
	- konter_penyulang : board versi 3.1

	
catatan :
- NXP
	float	: 4 bit
	short	: 2 bit
	int		: 4 bit


coba porting ke FreeRTOS v7.1.0
- http://linux.softpedia.com/progChangelog/FreeRTOS-Changelog-45191.html
