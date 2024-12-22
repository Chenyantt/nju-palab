#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init()
{
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg)
{
  uint32_t info = inl(VGACTL_ADDR);
  *cfg = (AM_GPU_CONFIG_T){
      .present = true, .has_accel = false, .width = info >> 16, .height = info & (0xffff), .vmemsz = 0};
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl)
{
  if (ctl->w && ctl->h)
  {
    uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
    uint32_t width = inl(VGACTL_ADDR) >> 16;
    uint32_t *pixels = ctl->pixels;
    int i = 0;
    for (int y = ctl->y; y < ctl->y + ctl->h; ++y)
    {
      for (int x = ctl->x; x < ctl->x + ctl->w; ++x)
      {
        fb[width * y + x] = pixels[i++];
      }
    }
  }

  if (ctl->sync)
  {
    outl(SYNC_ADDR, 1);
    
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status)
{
  status->ready = true;
}
