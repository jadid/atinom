/*
	menu popup untuk tampilan monita
	furkan jadid
	daun biru engineering
	19 januari 2008
	*/

#include "../tampilan.h"
#include "../monita/monita_uip.h"

extern struct t_sumber sumber[];
extern struct t_data_float s_data[JML_SUMBER];

extern unsigned char isi_sumber[20];

void kotak_pilih(unsigned char cur)
{
	
}

void data_sumber(unsigned int no_sumber, unsigned int key)
{
	int i;
	char tk[32];
	
	hapus(15, 10, 104, 204);
	move_ke(100, 25+(9*key));
	line_ke(100, 10);
	line_ke(9, 10);
	line_ke(9, 203);
	line_ke(100, 203);
	line_ke(100, 35+(9*key));
	line_ke(300, 35+(9*key));
	line_ke(300, 25+(9*key));
	line_ke(100, 25+(9*key));
	
	
	teks_layar(12, 12, "Data Kanal");
	
	for (i=0; i<20; i++)
	{
		sprintf(tk, "%2d. %.3f", (i+1), s_data[isi_sumber[key]].data[i]);
		teks_layar(12, 22 + (9*i), tk);			
	}	
}


char pos_cur=0;
char pos_lama=0;

void menu_OK(unsigned char p, unsigned char mesin, unsigned char jok)
{
	unsigned int key_press;
	unsigned char key_index=0;
	unsigned char mesin_index=0;
	int i;
	int jum_aktif=0;
	int loop;
	
	//menu_pilih(p, mesin);
	if (p == 7 && jok == 1)
	{
		/*
		//hapus(80, 25, 300, 180);	
		//kotak3d(79,26, 301, 181);
		if (p > pos_lama)
		{
			p = pos_lama;
			pos_cur++;	
		}
		else if (p < pos_lama);
		{
			p = pos_lama;
			pos_cur--;	
		}
		
		// cursor paling atas dulu
		kotak(68, 24+(9*pos_cur), 300, 35+(9*pos_cur));
		*/
		
		// cari jumlah sumber aktif
		for (i=0; i<JML_SUMBER; i++)
		{
			if (sumber[i].status == 1) jum_aktif++;
		}
		
		key_index = 0;
		
		for (;;)
		{
			if ((FIO_KEYPAD & PF14) == PF14)
			{
				cls_layar();
				// cek tombol apa yang ditekan
				key_press = (FIO1PIN & KEY_DAT);
				
				if (key_press == ATAS)
				{
					key_index--;

					if (key_index == 255) key_index = (jum_aktif-1);
				}
				else if ( key_press == BAWAH )
				{
					key_index++;	
					if (key_index > (jum_aktif-1)) key_index = 0;
				}
				else if ( key_press == OK)
				{
					//jum_OK++;	
				}
				else if ( key_press == CANCEL)
				{
					//jum_OK--;
					return;	
				}
				
				menu_monita(7);
				menu_pilih(7, mesin);
				//kotak(68, 25+(9*key_index), 300, 35+(9*key_index));
				data_sumber(1, key_index);
				update_lcd();
				loop = 0;
			}
			vTaskDelay(100);
			
			loop++;
			if (loop > 5)
			{
				loop = 0;
				cls_layar();
				menu_monita(7);
				menu_pilih(7, mesin);
				data_sumber(1, key_index);
				update_lcd();	
			}
		}
	}
	//else if (p == 7 && jok == 1)
	{
		
	}
}
