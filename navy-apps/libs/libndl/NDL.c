#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>

static int evtdev = 3;
static int fbdev = 4;
static int screen_w = 0, screen_h = 0;
static int canvas_w = 0, canvas_h = 0;
static int canvas_x = 0, canvas_y = 0;

static uint32_t init_time = 0;

uint32_t NDL_GetTicks()
{
  struct timeval tv;
  assert(gettimeofday(&tv, 0) == 0);
  return ((uint32_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000)) - init_time;
}

int NDL_PollEvent(char *buf, int len)
{
  int ret = read(evtdev, buf, len);
  if (ret == 0)
    return 0;
  else
    return 1;
}

void NDL_OpenCanvas(int *w, int *h)
{
  if (getenv("NWM_APP"))
  {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w;
    screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1)
    {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0)
        continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0)
        break;
    }
    close(fbctl);
  }

  assert(screen_h >= *h && screen_w >= *w);
  if (*w == 0 && *h == 0)
  {
    *w = canvas_h = screen_h;
    *h = canvas_w = screen_w;
  }
  else
  {
    canvas_h = *h;
    canvas_w = *w;
    canvas_x = (screen_w - canvas_w) / 2;
    canvas_y = (screen_h - canvas_h) / 2;
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h)
{
  for (int i = 0; i < h && y + i < canvas_h; ++i)
  {

    lseek(fbdev, ((y + canvas_y + i) * screen_w + (x + canvas_x)) * 4, SEEK_SET);
    write(fbdev, pixels + i * w, 4 * (w < canvas_w - x ? w : canvas_w - x));
  }
}

void NDL_OpenAudio(int freq, int channels, int samples)
{
}

void NDL_CloseAudio()
{
}

int NDL_PlayAudio(void *buf, int len)
{
  return 0;
}

int NDL_QueryAudio()
{
  return 0;
}

#define BUFFER_SIZE 1024

int NDL_Init(uint32_t flags)
{
  if (getenv("NWM_APP"))
  {
    evtdev = 3;
    fbdev = 4;
  }
  struct timeval tv;
  assert(gettimeofday(&tv, 0) == 0);
  init_time = (uint32_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000);

  char buf[BUFFER_SIZE];
  int fd = open("/proc/dispinfo", 0, 0);
  int ret = read(fd, buf, BUFFER_SIZE);
  assert(close(fd) == 0);

  int w, h, i;
  assert(strncmp(buf, "WIDTH", 5) == 0);
  // 这一行将 i 增加 5，以跳过字符串 "WIDTH"。
  i += 5;
  while (buf[i] == ' ')
    ++i;
  assert(buf[i] == ':');
  ++i;
  while (buf[i] == ' ')
    ++i;
  while (buf[i] != '\n')
  {
    assert(buf[i] >= '0' && buf[i] <= '9');
    w = w * 10 + buf[i] - '0';
    ++i;
  }
  ++i;
  assert(strncmp(buf + i, "HEIGHT", 6) == 0);
  i += 6;
  while (buf[i] == ' ')
    ++i;
  assert(buf[i] == ':');
  ++i;
  while (buf[i] == ' ')
    ++i;
  while (buf[i] != '\n')
  {
    assert(buf[i] >= '0' && buf[i] <= '9');
    h = h * 10 + buf[i] - '0';
    ++i;
  }
  screen_w = w, screen_h = h;
  return 0;
}

void NDL_Quit()
{
}
