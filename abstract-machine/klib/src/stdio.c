#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...)
{
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap)
{
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...)
{
  assert((out != NULL) && (fmt != NULL));
  va_list args;
  va_start(args, fmt);
  char *buf = out;
  const char *fmt_ptr = fmt;
  while (*fmt_ptr)
  {
    if (*fmt_ptr == '%')
    {
      ++fmt_ptr;
      if (*fmt_ptr)
      {
        switch (*fmt_ptr)
        {
        case 'd':
        {
          int i = va_arg(args, int);
          if (i == 0)
          {
            *buf++ = '0';
            break;
          }
          char tmp[10] = {0};
          int width = -1;
          if (i < 0)
            i = -i, *buf++ = '-';
          for (int t = i; t; tmp[++width] = '0' + t % 10, t /= 10)
            ;
          for (int j = width; j >= 0; --j)
            *buf++ = tmp[j];
          break;
        }
        case 's':
        {
          char *s = va_arg(args, char *);
          for (; *s; s++)
            *buf++ = *s;
          break;
        }
        default:
          break;
        }
        fmt_ptr++;
      }
    }
    else
    {
      *buf++ = *fmt_ptr++;
    }
  }
  *buf = '\0';
  va_end(args);
  return (int)(buf-out);
}

int snprintf(char *out, size_t n, const char *fmt, ...)
{
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap)
{
  panic("Not implemented");
}

#endif
