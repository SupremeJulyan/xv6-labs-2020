#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
int main(int argc ,char **argv){
    if(argc != 2){
        printf("Usage:sleep [time]\n");
    }else{
        int t = atoi(argv[1]);
        printf("Pause for %d clock ticks.\n",t);        
        sleep(t);//系统调用
        exit(0);
    }
    return 0;
}