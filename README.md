# SoalShiftSISOP20_modul4_T10

## Jawaban Soal Shift Sistem Operasi 2020

## Modul 4

Oleh: 

* 05311840000020 Milenia Ulwan Zafira
* 05311840000042 I Komang Aditya Mahadiharja

## Penjelasan source code
List library dan deklarasi variabel global yang akan digunakan
```
#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <pthread.h>
#include <grp.h>
#include <dirent.h>
#include <pwd.h> 

char dirpath[100] = "/home/milenfifi/Documents";
char kunci[100] = "9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO";
```
Fungsi Enkripsi
mengecek diposisi manakah huruf teks[i] pada huruf kunci (anggap di posisi j), kemudian menggantikan huruf pada teks[i] dengan huruf kunci yang berada pada posisi j+10. yang mereturn character terenkrpsi.
```
char* Enkripsi(char enc[100]) 
{
    int i, j;
    for (i=0; enc[i]!='\0'; i++)
	{
        for (j=0; j<strlen(kunci); j++)
        {
            if(enc[i] == kunci[j])
			{
                break;
            }
        }
        if(enc[i] == '.' && (strlen(enc)-i)<5)break; 
        else if(enc[i] != '/')
		{
            enc[i] = kunci[(j+10)%87]; 
        }else{
            enc[i] = '/';   
        }
    }
    return enc;
}
```
Fungsi Dekripsi
mengecek di posisi manakah huruf teks[i] yang telah terenkripsi pada huruf kunci, kemudian dikembalikan lagi dengan cara mengubahnya dengan huruf kunci yang berada pada posisi j-10
```
char* dekripsi(char dec[100]) 
{ 	
    int z, i, j;
    for (i=0; dec[i]!='\0'; i++) 
	{  for (j=0; j<strlen(kunci); j++)
        {   if(dec[i] == kunci[j])
			{  break;  }
        }
        z = (j-10)%87;
        if(z < 0)
		{
            z = z + strlen(kunci);  
        }
        if(dec[i] == '.' && (strlen(dec)-i)<5)break; 
        else if(dec[i] != '/')
		{
            dec[i] = kunci[z]; 
        }else
		{
            dec[i] = '/'; 
        }
    }
    return dec; 
}
```
Fungsi checkEncrypt
mengambil path dari folder terenkripsi, return fpath atau folder path
```
char *checkEncrypt(char fpath[100],const char *path)
{
    int i,j;
    char rev[100],fname[1000]; 
        memset(rev,'\0',100); 
        memset(fname,'\0',1000);
        for (i = 0; i<strlen(path); i++)
        {
            if (path[i]=='/')
            {   break;   }
            rev[i] = path[i]; 
        }
        sprintf(fname,"%s",rev); 
        memset(rev,'\0',100);
        j=0; 
        if (i!=strlen(path))
        {
            while (1){
                rev[j] = path[i]; i++; j++; 
                if(i==strlen(path))
                {   break;  }
                     }
            Enkripsi(rev); 
            strcat(fname,rev); 
            sprintf(fpath, "%s%s", dirpath, fname); 
            printf ("pathattr1 = %s\n",fpath); 
        } 
        else
        {
            sprintf(fpath, "%s%s", dirpath, fname); 
            printf ("pathattr11 = %s\n",fpath); 
        }
    return fpath; 
}
```
Fungsi Create Log
membuat log  perubahan apa saja yang dilakukan. unlink, rmdir, atau mkdir
```
void createlog(char process[100],char fpath[100])
{
    char text[200];
    FILE *fp = fopen("/home/milenfifi/Documents/fs.log","a"); 
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    if (strcmp(process,"unlink")==0)
    {
        sprintf(text, "WARNING::%04d%02d%02d-%02d:%02d:%02d::UNLINK::%s\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,fpath);  
    }
    else if (strcmp(process,"mkdir")==0)
    {
        sprintf(text, "INFO::%04d%02d%02d-%02d:%02d:%02d::MKDIR::%s\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,fpath); 
    }
    else if (strcmp(process,"rmdir")==0)
    {
        sprintf(text, "WARNING::%04d%02d%02d-%02d:%02d:%02d::RMDIR::%s\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,fpath); 
    }
    for (int i = 0; text[i] != '\0'; i++) 
	{
            fputc(text[i], fp); 
    }
    fclose (fp);
}
```
fungsi apabila rename
```
void createlogrename(char from[100], char to[100])
{
    FILE *fp = fopen("/home/milenfifi/Documents/fs.log","a"); 
    time_t t = time(NULL); 
    struct tm tm = *localtime(&t); 
    char text[200]; 
    sprintf(text, "INFO::%04d%02d%02d-%02d:%02d:%02d::RENAME::%s::%s\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,from,to);  
    for (int i = 0; text[i] != '\0'; i++) 
	{
            fputc(text[i], fp);  
    }
    fclose(fp);
}
```
Mendecrypt nama file/folder didalam folder terenkripsi sehingga pada fungsi getattr, nama file akan dienkripsi menghasilkan nama file/folder yang asli
```
while ((de = readdir(dp)) != NULL) 
	{
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12; 
        if(strcmp(de->d_name, ".")!=0 && strcmp(de->d_name, "..")!=0 && flag==1)
		{
            dekripsi(de->d_name);
        }
        res = (filler(buf, de->d_name, &st, 0));                                                     
            if(res!=0) break; 
    }
    closedir(dp);
```
berikut list struct fuse_operation yang jika dipanggil akan menjalankan fungsinya
```
    static struct fuse_operations xmp_oper = 
    {
    .getattr    = xmp_getattr,
    .unlink     = xmp_unlink,
    .readdir    = xmp_readdir,
    .rmdir      = xmp_rmdir,
    .mkdir      = xmp_mkdir,
    .read       = xmp_read,
    .rename     = xmp_rename,
    }; 
```
![Output1](https://raw.githubusercontent.com/MilenFifi/SoalShiftSISOP20_modul4_T10_2/master/Screenshot (97).png)
![Output2](https://raw.githubusercontent.com/MilenFifi/SoalShiftSISOP20_modul4_T10_2/master/Screenshot (96).png)
