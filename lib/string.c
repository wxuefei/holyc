#ifndef HOLYC_LIB_STRING
#define HOLYC_LIB_STRING

#include <lib/stdint.h>
#include <lib/stddef.h>

int strlen(char *str) {
  int i = 0;
  while(str[i])
    i++;
  return i;
}

void reverse(char *str) {
  char tmp;
  int len = strlen(str);

  for (int i = 0, j = len - 1; i < j; i++, j--) {
    tmp = str[j];
    str[j] = str[i];
    str[i] = tmp;
  }
}

void itoa(int n, char *str) {
  int i = 0,
      orig = n;

  if (n < 0)
    n *= -1;

  do {
    str[i++] = n % 10 + '0';
  } while ((n /= 10) > 0);

  if (orig < 0)
    str[i++] = '-';

  str[i] = '\0';
  reverse(str);
}

char lower(char c) {
  if (c >= 'A' && c <= 'Z')
    return c + 'a' - 'A';
  return c;
}

void *memcpy(void *dest, const void *src, size_t n) {
  char *_dest = (char *)dest;
  char *_src = (char *)src;

  if (_dest == NULL || _src == NULL)
    return dest;

  while (n) {
    *(_dest++) = *(_src++);
    --n;
  }

  return dest;
}

#endif // HOLYC_LIB_STRING
