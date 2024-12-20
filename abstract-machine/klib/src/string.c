#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s)
{
  panic("Not implemented");
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
  panic("Not implemented");
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
  if (*s1 > *s2)
    return 1;
  else if (*s1 < *s2)
    return -1;
  else
    return 0;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
  panic("Not implemented");
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
  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n)
{
  panic("Not implemented");
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
