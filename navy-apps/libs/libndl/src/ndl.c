#include <sys/time.h>
#include <assert.h>
#include <stdint.h>

static uint32_t init_time = 0;

int NDL_Init(uint32_t flags){
    struct timeval tv;
    assert(gettimeofday(&tv, 0) == 0);
    init_time = (uint32_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
    return 0;
}

uint32_t NDL_GetTicks() {
  struct timeval tv;
  assert(gettimeofday(&tv, 0) == 0);
  return ((uint32_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000)) - init_time;
}