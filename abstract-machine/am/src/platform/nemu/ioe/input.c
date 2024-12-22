#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t scancode inl(KBD_ADDR);
  if(scancode & KEYDOWN_MASK){
    kbd->keycode = scancode ^ KEYDOWN_MASK;
    kbd->keydown = 1;
  }else{
    kbd->keycode = scancode;
    kbd->keydown = 0;
  }
}
