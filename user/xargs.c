#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

#define MAX_STDIN_SIZE 512
#define STDIN 0 //标准输入流文件描述符
static char Args[MAX_STDIN_SIZE];
static int ArgsIndex = 0;

void exec_pro(char *command,char **argv){
    int pid = fork();
    if(pid < 0){
        printf("fork error\n");
        exit(-1);
    }else if(pid > 0){ 
        wait((int *)0);
    }else{
        //注意argv的格式{command,arg1，arg2...,0}
        if(exec(command,argv) < 0 ){
            printf("exec %s error\n",command);
            exit(-1);
        }
        
    }
}
//将xargs 指令后一条指令的参数与标准输入中的字符合并起来,
void handle_args(int s,int e,char *argv[]){
    for(int i = s ;i <= e;i++){
        for(char *p = argv[i];*p;p++){
            Args[ArgsIndex++] = *p;
        }
        Args[ArgsIndex++] = ' ';
    }
    char ch;
    while(read(STDIN,&ch,sizeof(ch))){
        Args[ArgsIndex++] = ch;
    }
    Args[ArgsIndex] = '\0';
}


void __xargs(char *command,int n){
    char ch;
    int t = n;
    char *p = Args,*q = Args;
    char *argv[MAXARG] = {0};
    int v = 0;
    argv[v++] = command;
    char *str;
    for(int i = 0;i <= ArgsIndex;i++){
        ch = Args[i];
        if(ch == '\0'){
            if(*(q-1) == '\n')
                *(q-1) = '\0';
            str = (char*)malloc((int)(q-p)+1);
            strcpy(str,p);
            argv[v++] = str;
            break;
        }
        if((ch == '\n' || ch == ' ')&& !(--t)){
            t = n;
            *q = '\0';
            str = (char*)malloc((int)(q-p)+1);
            strcpy(str,p);        
            argv[v++] = str;//以空格与回车分界截取字符串
            p = q+1;
        }
        q++;
    }
    argv[v] = 0;

    exec_pro(command,argv);
    for(v=0;argv[v];v++)
        free(argv[v]);
}

int main(int argc,char **argv){
    ArgsIndex = 0;
    memset(Args,0,sizeof(Args));
    for(int i = 1;i< argc;i++){
        if(!strcmp(argv[i],"-n")){
            if(argc < 4){
                printf("Usage:xargs [-n] [num] [command]\n");
                exit(-1);
            }
            int n = atoi(argv[i+1]);
            handle_args(i+3,argc-1,argv);
            __xargs(argv[3],n);
            return 0;
        }
    }
    if(argc < 2){
        printf("Usage:xargs [command]\n");
        exit(-1);
    }
    handle_args(2,argc-1,argv);
    __xargs(argv[1],1);

    return 0;
}