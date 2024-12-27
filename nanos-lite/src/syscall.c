#include <common.h>
#include "syscall.h"

#define strace() \
  ;              \
  Log("[strace %s]: a0:%d, a1:%d, a2:%d, ret:%d", syscall_name[a[0]], a[1], c->GPR3, c->GPR4, c->GPRx);

static char *syscall_name[] = {"exit", "yield", "open", "read",
                               "write", "kill", "getpid", "close",
                               "lseek", "brk", "fstat", "time",
                               "signal", "execve", "fork", "link",
                               "unlink", "wait", "times", "gettimeofday"};

static void do_yield(Context *c)
{
  yield();
  c->GPRx = 0;
}

static void do_exit(Context *c)
{
  halt(c->GPR2);
}

static void do_write(Context *c)
{
  uintptr_t fd = c->GPR2;
  char *buf = (char *)c->GPR3;
  uintptr_t len = c->GPR4;
  if (fd == 0 || fd == 1)
  {
    for (uint32_t i = 0; i < len; ++i)
    {
      putch(buf[i]);
    }
    c->GPRx = len;
  }
  else
  {
    c->GPRx = -1;
  }
}

void do_syscall(Context *c)
{
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  switch (a[0])
  {
  case SYS_exit:
    strace();
    do_exit(c);
    break;
  case SYS_yield:
    do_yield(c);
    break;
  case SYS_write:
    do_write(c);
    break;
  case SYS_brk:
    c->GPRx = 0;
  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }
  //  strace();
}
