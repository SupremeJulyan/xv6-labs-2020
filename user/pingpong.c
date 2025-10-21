#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc,char **argv){
    int fds[2][2];//一对管道描述符0,1分别代表读写端 0号管道父->子 1号管道子->父
    if(pipe(fds[0]) < 0 || pipe(fds[1]) < 0){
        printf("pipe error\n");
        exit(-1);
    }
    int pid = fork();
    if(pid < 0){
        printf("fork error\n");
        exit(-1);
    }else if(pid == 0){//子进程  子收到ping向父发pong
        char buf[5]= {0};
        read(fds[0][0],buf,sizeof(buf));//从0号管道读出
        if(!strcmp(buf,"ping")){
            printf("%d: received ping\n", getpid());
            write(fds[1][1],"pong",5);//写入1号管道写端
            close(fds[1][1]);//1号管道写端不需要了，关闭,如果不关，数据为空时会堵塞读
        }else{
            printf("child received error data:%s\n !",buf);
            exit(-1);
        }
        
    }else{//父进程，父向子发ping
        write(fds[0][1],"ping",5);//写入0号管道写端
        close(fds[0][1]);//0号管道写端不需要了，关闭
        char buf[5] = {0};
        read(fds[1][0],buf,sizeof(buf));//从1号管道读出
        if(!strcmp(buf,"pong")){
            printf("%d: received pong\n", getpid());
        }else{
            printf("parent received error data:%s\n !",buf);
            exit(-1);
        }
        //等待子进程结束
        wait((int *)0);
    }
    close(fds[0][0]);
    close(fds[1][0]);
    exit(0);//父子进程都要终止，关闭文件描述符
    return 0; 
}