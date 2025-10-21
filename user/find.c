#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

#define MAXSIZE 256
//从后往前比较文件名，如usr/file.c vs usr/local/file.c 他们路径不一样但是文件名相同
int fileBasename_match(const char *p,const char *q){
    int i  = strlen(p) - 1;
    int j = strlen(q) - 1;
    while(i >= 0 && j >= 0){
        if(p[i] != q[j]) 
            return 0;
        i--;
        j--;
    }
    return 1;
}


void find_file(char *path,char *filename){
    struct dirent de;
    struct stat st;
    int fd;
    
    if((fd = open(path, 0)) < 0){
        printf("find: cannot open %s\n", path);
        return;
    }
    if(fstat(fd, &st) < 0){
        printf("find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type){
        case T_DEVICE:
        case T_FILE:
            if(fileBasename_match(path,filename))
                printf("%s\n", path);
        break;
        case T_DIR:
        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            if(de.inum == 0)
                continue;
            if(!strcmp(".",de.name) || !strcmp("..",de.name))
                continue;
            //拼接完整文件路径
            char buf[MAXSIZE*2+1]={0};
            strcpy(buf,path);
            char *p = buf + strlen(path);
            *(p++) = '/';
            strcpy(p,de.name);
            *(p+strlen(de.name)) = '\0';
            find_file(buf,filename);
        }
        return;
    }
    close(fd);

}

int main(int argc,char **argv){
    if(argc < 2){
        printf("Usage:find [target path] [filenamee] ...");
        exit(0);
    }
    for(int i = 1;i < argc;i++){
        if(strlen(argv[i]) > MAXSIZE){
            printf("parameters too long\n");
            exit(0);
        }
    }
    for(int i = 2;i < argc;i++){
        find_file(argv[1],argv[i]);
    }  
    return 0;
}