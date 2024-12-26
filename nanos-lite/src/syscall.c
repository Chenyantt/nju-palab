#include <common.h>
#include "syscall.h"

static void do_yield(Context *c){
  yield();
  c->GPRx = 0;
}

static void do_exit(Context *c){
  halt(c->GPR2);
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case SYS_exit: do_exit(c); break;
    case SYS_yield: do_yield(c); break;
    case 2: 
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
