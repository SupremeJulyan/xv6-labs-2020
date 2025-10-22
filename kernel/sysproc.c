#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
//trace系统调用，初始化需要跟踪的系统调用掩码
uint64
sys_trace(void)
{
  int mask;

  if(argint(0, &mask) < 0)//获取用户态函数的int参数
    return -1;
  myproc()->syscall_need_trace_mask = mask;//初始化需要跟踪的系统调用掩码

  return 0;
}
extern uint64 getprocnum();
extern uint64 getfreememsize();
//sysinfo系统调用，获取空闲内存和进程数
uint64
sys_sysinfo(void)
{
  struct sysinfo kinfo;//内核结构体
  kinfo.nproc = getprocnum();
  kinfo.freemem = getfreememsize();
  
  uint64 uinfo;//用户结构体
  if(argaddr(0, &uinfo) < 0)//获取用户态函数的地址参数
    return -1;
  //from vm.c 将内核数据拷贝到用户 
  return copyout(myproc()->pagetable, uinfo, (char*)&kinfo, sizeof(kinfo));
}