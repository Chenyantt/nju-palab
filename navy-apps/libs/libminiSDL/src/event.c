#include <NDL.h>
#include <SDL.h>

#define keyname(k) #k,
#define BUFFER_SIZE 64

static const char *keyname[] = {
    "NONE",
    _KEYS(keyname)};

int SDL_PushEvent(SDL_Event *ev)
{
  return 0;
}

int SDL_PollEvent(SDL_Event *ev)
{
  return 0;
}

int SDL_WaitEvent(SDL_Event *event)
{
  char buffer[BUFFER_SIZE];
  int ret = NDL_PollEvent(buffer, BUFFER_SIZE);
  if (ret == 0)
    return 0;
  if (*buffer == 'k')
  {
    event->type = (*(buffer + 1) == 'd') ? SDL_KEYDOWN : SDL_KEYUP;
    event->key = 
  }SDL_Keys
  return ret;
  
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask)
{
  return 0;
}

uint8_t *SDL_GetKeyState(int *numkeys)
{
  return NULL;
}
