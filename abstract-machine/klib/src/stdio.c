#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...)
{
  assert(fmt != NULL);
  int ret = 0;
  va_list args;
  va_start(args, fmt);
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
        case 'p':
        case 'd':
        {
          int i = va_arg(args, int);
          if (i == 0)
          {
            putch('0'), ++ret;
            break;
          }
          char tmp[10] = {0};
          int width = -1;
          if (i < 0)
            i = -i, putch('-'), ++ret;
          for (int t = i; t; tmp[++width] = '0' + t % 10, t /= 10)
            ;
          for (int j = width; j >= 0; --j)
            putch(tmp[j]), ++ret;
          break;
        }
        case 's':
        {
          char *s = va_arg(args, char *);
          for (; *s; s++)
            putch(*s), ++ret;
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
      putch(*fmt_ptr++), ++ret;
    }
  }
  va_end(args);
  return ret;
}

int vsprintf(char *out, const char *fmt, va_list ap)
{
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
          int i = va_arg(ap, int);
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
          char *s = va_arg(ap, char *);
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
  return (int)(buf - out);
}

int sprintf(char *out, const char *fmt, ...)
{
  assert((out != NULL) && (fmt != NULL));
  va_list args;
  va_start(args, fmt);
  int ret = vsprintf(out, fmt, args);
  va_end(args);
  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...)
{
  assert((out != NULL) && (fmt != NULL));
  if(n == 0) return 0;
  va_list args;
  va_start(args, fmt);
  int ret = vsnprintf(out, n, fmt, args);
  va_end(args);
  return ret;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap)
{
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
          int i = va_arg(ap, int);
          if (i == 0)
          {
            *buf++ = '0';
            if((int)(buf - out) >= n) return n;
            break;
          }
          char tmp[10] = {0};
          int width = -1;
          if (i < 0){
            i = -i, *buf++ = '-';
            if((int)(buf - out) >= n) return n;
          }
          for (int t = i; t; tmp[++width] = '0' + t % 10, t /= 10)
            ;
          for (int j = width; j >= 0; --j){
            *buf++ = tmp[j];
            if((int)(buf - out) >= n) return n;
          }
          break;
        }
        case 's':
        {
          char *s = va_arg(ap, char *);
          for (; *s; s++){
            *buf++ = *s;
            if((int)(buf - out) >= n) return n;
          }
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
      if((int)(buf - out) >= n) return n;
    }
  }
  return (int)(buf - out);
}

#endif
