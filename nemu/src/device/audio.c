/***************************************************************************************
 * Copyright (c) 2014-2024 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include <common.h>
#include <device/map.h>
#include <SDL2/SDL.h>

enum
{
  reg_freq,
  reg_channels,
  reg_samples,
  reg_sbuf_size,
  reg_init,
  reg_count,
  nr_reg
};

static uint8_t *sbuf = NULL;
static uint32_t *audio_base = NULL;

static int buf_cnt = 0;

void callBack_fillAudioData(void *userdata, uint8_t *stream, int len)
{
  SDL_memset(stream, 0, len);
  if (buf_cnt == 0)
    return;
  len = (len > buf_cnt ? buf_cnt : len);
  SDL_MixAudio(stream, sbuf, len, SDL_MIX_MAXVOLUME);
  if (len < buf_cnt)
  {
    buf_cnt -= len;
    memmove(sbuf, sbuf + len, buf_cnt);
  }
  else
    buf_cnt = 0;
}

static void audio_io_handler(uint32_t offset, int len, bool is_write)
{
  if (offset == 12)
  {
    return;
    
  }
  else if (offset == 20)
  {
    assert(!is_write);
    audio_base[reg_count] = buf_cnt;
  }
  else
  {
    assert(is_write);
    if (audio_base[reg_init] == 1)
    {
      SDL_AudioSpec s = {};
      s.format = AUDIO_S16SYS;
      s.userdata = NULL;
      s.freq = audio_base[reg_freq];
      s.channels = audio_base[reg_channels];
      s.samples = audio_base[reg_samples];
      s.callback = callBack_fillAudioData;
      SDL_InitSubSystem(SDL_INIT_AUDIO);
      if (SDL_OpenAudio(&s, NULL) == -1)
      {
        perror("Cannot open the audio device!\n");
      }
      else
      {
        SDL_PauseAudio(0);
      }
      audio_base[reg_init] = 0;
    }
  }
}

static void stream_io_handler(uint32_t offset, int len, bool is_write)
{
  assert(is_write);
  assert(offset == buf_cnt);
  buf_cnt += len;
}

void init_audio()
{
  uint32_t space_size = sizeof(uint32_t) * nr_reg;
  audio_base = (uint32_t *)new_space(space_size);
#ifdef CONFIG_HAS_PORT_IO
  add_pio_map("audio", CONFIG_AUDIO_CTL_PORT, audio_base, space_size, audio_io_handler);
#else
  add_mmio_map("audio", CONFIG_AUDIO_CTL_MMIO, audio_base, space_size, audio_io_handler);
#endif

  sbuf = (uint8_t *)new_space(CONFIG_SB_SIZE);
  add_mmio_map("audio-sbuf", CONFIG_SB_ADDR, sbuf, CONFIG_SB_SIZE, stream_io_handler);
  audio_base[reg_sbuf_size] = CONFIG_SB_SIZE;
}
