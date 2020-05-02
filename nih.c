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

char *checkEncryptslash(char fpath[100],const char *path) 
{
    int i,j; 	
    char rev[100],fname[1000]; 
        memset(rev,'\0',100); 
        memset(fname,'\0',1000); 
        for (i = 0; i<strlen(path); i++)
        {
            if ((path[i]=='/' && i!=0) || path[i]=='\0')
            {
                break; 
            }
            rev[i] = path[i]; 
        }
        sprintf(fname,"%s",rev); 
        memset(rev,'\0',100); 
        j=0;
        if (i!=strlen(path))
        {
            while (1)
			{
                rev[j] = path[i]; i++; j++; 
                if(i==strlen(path))
                {   break;      }
            }
            Enkripsi(rev);
            strcat(fname,rev);
            sprintf(fpath, "%s%s", dirpath, fname); 
        }
        else
        {
            sprintf(fpath, "%s%s", dirpath, fname); 
        }
    return fpath;
}

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

static int xmp_getattr(const char *path, struct stat *stbuf) 
{
    int res; 
    char fpath[1000];
    if (strcmp(path, ".")!=0 && strcmp(path, "..")==0)
    {
        sprintf(fpath,"%s%s",dirpath,path); 
    } 
    else if (path[0]!='/' && strncmp(path,"encv1_",6)==0) 
	{
        checkEncrypt(fpath,path); 
    }
    else if (path[0]=='/' && strncmp(path,"/encv1_",7)==0)
	{
        checkEncryptslash(fpath,path); 
    }
    else 
	{
        sprintf(fpath, "%s%s", dirpath, path);  
    }
    res = lstat(fpath, stbuf);  
    if (res == -1)
        return -errno;  
    return 0; 
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    int flag=0; 
    char fpath[1000];  
    if(strcmp(path,"/") == 0)
    {
        path=dirpath; 
        sprintf(fpath,"%s",path); 
    }
    else if (path[0]!='/' && strncmp(path,"encv1_",6)==0)
	{
        flag=1; 
        checkEncrypt(fpath,path); 
    }
    else if (path[0]=='/' && strncmp(path,"/encv1_",7)==0)
	{
        flag=1;
        checkEncryptslash(fpath,path); 
    }
    else
	{
        sprintf(fpath, "%s%s", dirpath, path); 
    }
    int res = 0; 
    DIR *dp; 
    struct dirent *de; 
    (void) offset;
    (void) fi;
    dp = opendir(fpath);
    if (dp == NULL)
        return -errno;
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
    return 0; 
}

static int xmp_mkdir(const char *path,mode_t mode)
{
    int res;
    char fpath[1000];
    sprintf(fpath,"%s%s",dirpath,path); 
    res = mkdir (fpath,0777);
    if(res == -1)
        return -errno;
    char temp[] = "mkdir"; 
    createlog(temp,fpath);
    return 0;
}

static int xmp_rmdir(const char *path)
{
    int res;
    char fpath[1000]; 
    if (path[0]!='/' && strncmp(path,"encv1_",6)==0)
	{
        checkEncrypt(fpath,path);
    }
    else if (path[0]=='/' && strncmp(path,"/encv1_",7)==0) 
	{
        checkEncryptslash(fpath,path); 
    }
    else
	{
        sprintf(fpath, "%s%s", dirpath, path);
    }
    char temp[] = "rmdir";   	
    createlog(temp,fpath);
    res = rmdir(fpath); 
    if (res == -1)
        return -errno;
    return 0;
}

static int xmp_rename(const char *from, const char *to)
{
    int res;
    char frname[1000], toname[1000], frpath[1000], topath[1000]; 
    memset(frname, '\0', sizeof(frname));
    memset(toname, '\0', sizeof(toname)); 
    strcpy(frname, from);
    strcpy(toname, to);
    sprintf(frpath, "%s%s", dirpath, frname);
    sprintf(topath, "%s%s", dirpath, toname);
    res = rename(frpath, topath); 
    if (res == -1)
        return -errno;
    createlogrename(frname,toname); 
    return 0; 
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];  
    if(strcmp(path,"/") == 0)
    {
        path=dirpath;
        sprintf(fpath,"%s",path); 
    }
    else if (path[0]!='/' && strncmp(path,"encv1_",6)==0)
	{
        checkEncrypt(fpath,path);									
    }  		
    else if (path[0]=='/' && strncmp(path,"/encv1_",7)==0)
	{
        checkEncryptslash(fpath,path); 
    }
    else{
        sprintf(fpath, "%s%s", dirpath, path); 
    }
    int res = 0;
    int fd = 0 ; 
    (void) fi; 
    fd = open(fpath, O_RDONLY);
    if (fd == -1)
        return -errno; 
    res = pread(fd, buf, size, offset); 
    if (res == -1)
        res = -errno;
    close(fd); 
    return res; 
}

static int xmp_unlink(const char *path)
{
    int res; 
    char fpath[100]; 
    if (path[0]!='/' && strncmp(path,"encv1_",6)==0)
	{
        checkEncrypt(fpath,path); 
    }
    else if (path[0]=='/' && strncmp(path,"/encv1_",7)==0)  
	{
        checkEncryptslash(fpath,path);  
    }
    else{   
        sprintf(fpath, "%s%s", dirpath, path); 
    }
    char temp[] = "unlink"; 
    createlog(temp,fpath); 
    res = unlink(path); 
    if (res == -1)
        return -errno; 
    return 0; 
}


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


int main(int argc, char *argv[])	   
{
    umask(0); 
    return fuse_main(argc, argv, &xmp_oper, NULL);  
}
