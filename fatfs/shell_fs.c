/*
	shell_fs
	furkan jadid
	
	27 April 2009
	
	*/	
	
#if PAKAI_BLACKFIN
#include "../penting.h"
#if (SIMULATOR == 0)	
#include "FreeRTOS.h"
#endif
#else
#include "FreeRTOS.h"
#define SIMULATOR	0
#endif

#include "ff.h"     	/* FatFs declarations */
#include "diskio.h"   	/* Include file for user provided disk functions */
#include "sysdefs.h"
#include <string.h>

#ifdef PAKAI_FILE_SIMPAN

FATFS Fatfs;
DIR dirs;
FRESULT res;
unsigned char buf_lfn[255];
char buffer [100];
char abs_path[128]; 
FILINFO fileInfo;
 
static void mundurkan_path(void); 
 
static FRESULT scan_files_ex (char *path, int *total_size, int *total_files, int *total_dirs)
{
  if ((res = f_opendir (&dirs, path)) == FR_OK) 
  {
    int i = strlen (path);

    while (((res = f_readdir (&dirs, &fileInfo)) == FR_OK) && fileInfo.fname [0]) 
    {
      if (fileInfo.fattrib & AM_DIR) 
      {
        *total_dirs += 1;
        *(path + i) = '/'; 
        strcpy (path + i + 1, &fileInfo.fname [0]);
        res = scan_files_ex (path, total_size, total_files, total_dirs);
        *(path + i) = '\0';

        if (res != FR_OK) 
          return res;
      } 
      else 
      {
        *total_files += 1;
        *total_size += fileInfo.fsize;
      }
    }
  }

  return res;
}

static FRESULT scan_files (char *path, int *total_size, int *total_files, int *total_dirs)
{
  *total_size = 0;
  *total_files = 0;
  *total_dirs = 0;

  return scan_files_ex (path, total_size, total_files, total_dirs);
}

void set_fs_mount(void)
{
	if (f_mount(0, &Fatfs) == FR_OK)
	{
		printf("%s(): OK\r\n", __FUNCTION__);
	}
	else
	{
		printf("%s(): ERROR\r\n", __FUNCTION__);
	}
}

int cek_fs_free(void)
{
	U32 p2;
	FATFS *fs;
	FRESULT res;
	int acc_size;
	int acc_files;
	int acc_dirs;
	
	if ((res = f_getfree ("", (U32 *) &p2, &fs)))
	{ 
		//f_printerror (res); 
		printf("get free error = %d!\r\n", res);
		return res;
	}

	printf ("FAT type = %u\r\nBytes/Cluster = %u\r\nNumber of FATs = %u\r\n"
	    "Root DIR entries = %u\r\nSectors/FAT = %u\r\nNumber of clusters = %u\r\n"
	    "FAT start (lba) = %u\r\nDIR start (lba,clustor) = %u\r\nData start (lba) = %u\r\n",
	    fs->fs_type, fs->csize * 512 , fs->n_fats,
	    fs->n_rootdir, fs->sects_fat, fs->max_clust - 2,
	    fs->fatbase, fs->dirbase, fs->database
	);

	acc_size = acc_files = acc_dirs = 0;

	buffer [0] = '\0';

	#if 1
	if ((res = scan_files (buffer, &acc_size, &acc_files, &acc_dirs)))
	{  
		return res;
	}

	printf ("\r\n%u files, %u bytes.\r\n%u folders.\r\n"
	    "%u bytes total disk space.\r\n%u bytes available\r\n",
	    acc_files, acc_size, acc_dirs,
	    (fs->max_clust - 2) * fs->csize * 512, p2 * fs->csize * 512 /* fs->sects_clust */
	    );
	#endif

	return 0;
}
	
static int util_ls(int argc, char **argv)
{	
	FRESULT res;
	U32 size;
	U16 files;
	U16 jum_dirs;
	FATFS *fs;
	
	fileInfo.lfname = buf_lfn;
	fileInfo.lfsize = sizeof (buf_lfn);
	
	char *path;
	char *nama;
	
	if (argc == 1)
	{
		//sprintf(path, "");
		path = abs_path;
	}
	else
		path = argv[1];
	

	if ((res = f_opendir (&dirs, path)))
	{ 
		printf("%s(): ERROR = %d\r\n", __FUNCTION__, res);
		return 0;
	}
	#if 1
	for (size = files = jum_dirs = 0;;)
	{
		if (((res = f_readdir (&dirs, &fileInfo)) != FR_OK) || !fileInfo.fname [0]) 
		    break;

		if (fileInfo.fattrib & AM_DIR) 
		    jum_dirs++;
		else 
		{
		    files++; 
		    size += fileInfo.fsize;
		}

		if (fileInfo.lfname[0] == 0)
			nama = &(fileInfo.fname [0]);
		else
			nama = &(fileInfo.lfname[0]);
			 	
		printf ("\r\n%c%c%c%c%c %u/%02u/%02u %02u:%02u %9u  %s",
		    (fileInfo.fattrib & AM_DIR) ? 'D' : '-',
		    (fileInfo.fattrib & AM_RDO) ? 'R' : '-',
		    (fileInfo.fattrib & AM_HID) ? 'H' : '-',
		    (fileInfo.fattrib & AM_SYS) ? 'S' : '-',
		    (fileInfo.fattrib & AM_ARC) ? 'A' : '-',
		    (fileInfo.fdate >> 9) + 1980, (fileInfo.fdate >> 5) & 15, fileInfo.fdate & 31,
		    (fileInfo.ftime >> 11), (fileInfo.ftime >> 5) & 63,
		    fileInfo.fsize, nama);
		    //fileInfo.fsize, &(fileInfo.fname [0]));
	}

	printf ("\r\n%4u File(s),%10u bytes\r\n%4u Dir(s)", files, size, jum_dirs);

	if (f_getfree (path, (U32 *) &size, &fs) == FR_OK)
	printf (", %10uK bytes free\r\n",size * fs->csize / 2);

	// langsung ditampilkan di LCD
	// cls_layar();
	// caption("File Manager");
	//menu_file_manager(1);

	#endif
	return 0;
}

//extern char prompt[];

static int util_cd(int argc, char **argv)
{
	FRESULT res;
	FATFS *fs;
	int ln;
	int i;
	
	if (argc == 1 ) return;
	
	if ( *argv[1] == '\\')
	{
		sprintf(abs_path, "%s", "");
	}
	else
	{
		if ( strncmp(argv[1], "..", 2) == 0 )
		{
			/* naik satu step keatas */
			mundurkan_path();
		}
		else
		{
			strcat(abs_path, "\\");		
			strcat(abs_path, argv[1]);
		}		
	}
	printf(" >> \\%s\r\n", abs_path);
	
	if ((res = f_opendir (&dirs, abs_path)))
	{ 
		printf("%s(): ERROR = %d\r\n", __FUNCTION__, res);
		
		/* karena error, harus dikembalikan ke path semula */
		mundurkan_path();
		
		return 0;
	}
}

static tinysh_cmd_t util_cd_cmd={0,"cd","change direktory","[args]",
                              util_cd,0,0,0};

static int util_pwd()
{
	printf(" >> %s\r\n", abs_path);
	return 0;
}			

static tinysh_cmd_t util_pwd_cmd={0,"pwd","printf directory","[args]",
                              util_pwd,0,0,0};				  

static void mundurkan_path(void)
{
	int ln;
	int i;

	ln = strlen(abs_path);
		
	for (i=ln; i>=0; i--)
	{
		if ( abs_path[i] == '\\' ) 
		{
			abs_path[i] = 0;			
			break;
		}
	}	
}

static int util_view (int argc, char **argv)
{
	int ret;
	int ln;
	int i;
	FIL fd;
	
	if (argc == 1 ) 
	{
		printf(" ERR: Tolong masukkan nama file !\r\n");
		
		return 0;
	}
	
	strcat(abs_path, "\\");		
	strcat(abs_path, argv[1]);
	
	if (ret = f_open(&fd, abs_path, FA_READ | FA_WRITE))
	{
		printf("%s(): Buka file error %d !\r\n", __FUNCTION__, ret);
		
		/* karena error, harus dikembalikan ke path semula */
		mundurkan_path();
							
		return -1;
	}
	
	/* meskipun tidak error, harus tetap dikembalikan */
	mundurkan_path();
		
	ret = sizeof (buffer);
	for (;;)
	{
		f_read( &fd, buffer, ret, &ln);
		
		printf("%s", buffer);
		
		if (ln < ret) break; // sudah mencapai akhir file		
	}
	f_close( &fd );
	printf("\r\n");
	return;
}

static tinysh_cmd_t util_view_cmd={0,"view","simple text viewer","[args]",
                              util_view,0,0,0};

#if (SIMULATOR == 0)
static tinysh_cmd_t util_ls_cmd={0,"ls","list direktory","[args]",
                              util_ls,0,0,0};

static void util_mkdir(int argc, char **argv)
{
	/* 	untuk membuat direktori
		mkdir dua\dalamdua 
		
		mengikuti konvensi dalam DOS
		*/
	int ret;
		
	if (argc < 2)
	{
		printf("%s(): ERR: Argument kurang !\r\n", __FUNCTION__);	
		printf("%s(): Untuk membuat direktori dalam direktori, gunakan bakslash !\r\n", __FUNCTION__);
		printf("%s(): Misalnya mkdir dua\\dalam_dua\r\n", __FUNCTION__);
	}
	else
	{
		printf("Buat direktori \"%s\"\r\n", argv [1]);

		//f_printerror (f_mkdir (argv [1]));
		if (f_mkdir (argv [1]) == 0)
		{
			printf(" .. OK");
		}		
		return ;
		/*
		sprintf(buf, "%s%s", cur_dir, argv[1]);
		printf(" %s\n", buf);
		
		ret = mkdir(&efs.myFs, buf);
		if (ret == -1) printf(" ERR: already exists");
		else if (ret == -2) printf(" ERR: invalid path");
		*/
	}
}

static tinysh_cmd_t util_mkdir_cmd={0,"mkdir","buat direktory","[args]",
                              util_mkdir,0,0,0};
                              
#endif

#endif
