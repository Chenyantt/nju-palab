#include <unistd.h>
#include <stdio.h>
#include <NDL.h>

int main() {
  NDL_Init(0);
  printf("Timer start!\n");
  uint32_t ms = 5000;
  uint32_t passed_time = NDL_GetTicks();
  while (1) {
    while(passed_time < ms){
      passed_time = NDL_GetTicks();
    }
    printf("mseconds = %u\n", ms);
    ms += 5000;
  }
  return 0;
}
