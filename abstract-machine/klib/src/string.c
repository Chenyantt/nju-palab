#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s)
{
  assert(s != NULL);
  size_t len = 0;
  while (*s++)
    ++len;
  return len;
}

char *strcpy(char *dst, const char *src)
{
  assert((dst != NULL) && (src != NULL));
  char *ret = dst;
  while ((*dst++ = *src++))
    ;
  return ret;
}

char *strncpy(char *dst, const char *src, size_t n)
{
  assert((dst != NULL) && src != (NULL));
  char *ret = dst;
  while (n-- && (*dst++ = *src++))
    ;
  while (n--)
    *dst++ = '\0';
  return ret;
}

char *strcat(char *dst, const char *src)
{
  assert((dst != NULL) && (src != NULL));
  char *ret = dst;
  for (; *dst; dst++)
    ;
  while ((*dst++ = *src++))
    ;
  return ret;
}

int strcmp(const char *s1, const char *s2)
{
  assert((s1 != NULL) && (s2 != NULL));
  while (*s1 && (*s1 == *s2))
    ++s1, ++s2;
  return *s1 - *s2;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
  assert((s1 != NULL) && (s2 != NULL));
  if (!n)
    return 0;
  while (--n && *s1 && *s1 == *s2)
    s1++, s2++;
  return *s1 - *s2;
}

void *memset(void *s, int c, size_t n)
{
  assert(s != NULL);
  char *tmp = s;
  while (n--)
  {
    *tmp++ = c;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n)
{
  assert((dst != NULL) && (src != NULL));

  if (src < dst)
  {
    char *pdst = (char *)dst + n - 1;
    char *psrc = (char *)src + n - 1;
    while (n--)
    {
      *pdst-- = *psrc--;
    }
  }
  else
  {
    char *pdst = (char *)dst;
    char *psrc = (char *)src;
    while (n--)
    {
      *pdst++ = *psrc++;
    }
  }
  return dst;
}

void *memcpy(void *out, const void *in, size_t n)
{
  assert((out != NULL) && (in != NULL));

  if (in < out)
  {
    char *pout = (char *)out + n - 1;
    char *pin = (char *)in + n - 1;
    while (n--)
    {
      *pout-- = *pin--;
    }
  }
  else
  {
    char *pout = (char *)out;
    char *pin = (char *)in;
    while (n--)
    {
      *pout++ = *pin++;
    }
  }
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
  assert((s1 != NULL) && (s2 != NULL));
  const unsigned char *su1, *su2;
  int res = 0;

  for (su1 = s1, su2 = s2; 0 < n; su1++, su2++, n--)
    if ((res = *su1 - *su2) != 0)
      break;
  return res;
}

#endif
