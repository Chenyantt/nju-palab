#include <common.h>
#include "syscall.h"

int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

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
  void *buf = (void *)c->GPR3;
  uintptr_t len = c->GPR4;
  c->GPRx = fs_write(fd, buf, len);
}

static void do_open(Context *c)
{
  char *pathname = (char *)c->GPR2;
  int flags = c->GPR3;
  int mode = c->GPR4;
  c->GPRx = fs_open(pathname, flags, mode);
}

static void do_read(Context *c)
{
  uintptr_t fd = c->GPR2;
  void *buf = (void *)c->GPR3;
  uintptr_t len = c->GPR4;
  c->GPRx = fs_read(fd, buf, len);
}

static void do_close(Context *c)
{
  uintptr_t fd = c->GPR2;
  c->GPRx = fs_close(fd);
}

static void do_lseek(Context *c)
{
  uintptr_t fd = c->GPR2;
  uintptr_t offset = c->GPR3;
  uintptr_t whence = c->GPR4;
  c->GPRx = fs_lseek(fd, offset, whence);
}

struct  timeval{
  long  tv_sec;
  long  tv_usec;
};

static void do_gettimeofday(Context *c)
{
  struct timeval *tv = (struct timeval *)c->GPR2;
  if (tv)
  {
    uint64_t us = io_read(AM_TIMER_UPTIME).us;
    tv->tv_sec = us / 1000000;
    tv->tv_usec = us - us / 1000000 * 1000000;
  }
  c->GPRx = 0;
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
    break;
  case SYS_open:
    do_open(c);
    break;
  case SYS_read:
    do_read(c);
    break;
  case SYS_lseek:
    do_lseek(c);
    break;
  case SYS_close:
    do_close(c);
    break;
  case SYS_gettimeofday:
    do_gettimeofday(c);
    break;
  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }
  strace();
}
