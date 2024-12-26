#include <common.h>
#include "syscall.h"

#define strace(); Log("[strace]:%s, a0:%d, a1:%d, a2:%d, a3:%d, ret:%d", syscall_name[a[0]], a[0], c->GPR1, c->GPR2, c->GPRx);

static char* syscall_name[] = {"exit", "yield", "open", "read",
                               "write", "kill", "getpid", "close",
                               "lseek", "brk", "fstat", "time",
                               "signal", "execve", "fork", "link",
                               "unlink", "wait", "times", "gettimeofday"};

static void do_yield(Context *c){
  yield();
  c->GPRx = 0;
}

// static void do_exit(Context *c){
//   halt(c->GPR2);
// }

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  switch (a[0]) {
    //case SYS_exit: do_exit(c); break;
    case SYS_yield: do_yield(c); break;
    case 2: 
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
  strace();
}
