#include "funcs.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

size_t first_non_empty_char(const char *str)
{
  size_t i = 0;
  while (str[i] != '\0' && str[i] == ' ') {
    i++;
  }
  if (str[i] == '\0')
    return -1;
  return i;
}

int index_of_char(const char *str, char c)
{
  int i = -1;
  char *ret = strchr(str, c);
  if (ret == NULL)
    return i;
  i = ret - str;
  return i;
}

char *replace_sub_str(char *original, size_t start, size_t end, const char *replacement)
{
  int suffix_len = strlen(original) - end - 1;
  int replacement_len = strlen(replacement);

  int new_len = start + replacement_len + suffix_len + 1;

  original = (char *)realloc(original, new_len * sizeof(char));
  if (original == NULL) {
    fprintf(stderr, "Could not allocate memory\n");
    exit(EXIT_FAILURE);
  }

  memmove(original + start + replacement_len, original + end + 1, suffix_len);
  strncpy(original + start, replacement, replacement_len);
  original[new_len - 1] = '\0';
  return original;
}

char *copy_sub_str(const char *source, char delimiter)
{
  size_t idx = index_of_char(source, delimiter);
  if (idx == -1) {
    return strdup(source);
  }
  char *dest = (char *)malloc((idx + 1));
  if (dest == NULL) {
    fprintf(stderr, "failed to allocate memory\n");
    exit(EXIT_FAILURE);
  }
  strncpy(dest, source, idx);
  dest[idx] = '\0';
  return dest;
}

char *copy_sub_str_offset(const char *source, size_t offset)
{
  size_t len = strlen(source);
  if (offset >= len)
    return strdup("");
  size_t sub_len = len - offset;
  char *sub_str = (char *)malloc(sub_len + 1);
  strcpy(sub_str, source + offset);
  return sub_str;
}

char *ltrim(char *s)
{
  while (isspace(*s))
    s++;
  return s;
}

char *rtrim(char *s)
{
  char *back = s + strlen(s);
  while (back > s && isspace(*(back - 1)))
    back--;

  *back = '\0';

  return s;
}

char *trim(char *s)
{
  return rtrim(ltrim(s));
}

void print_bits(uint32_t bits)
{
  for (int i = 0; i < 32; i++) {
    uint8_t mask = 1 << i;
    uint8_t bit = (bits & mask) >> i;
    printf("%u", bit);
  }
}

char *str_tolower(char *str)
{
  for (char *p = str; *p; p++)
    *p = tolower(*p);
  return str;
}

char *str_toupper(char *str)
{
  for (char *p = str; *p; p++)
    *p = toupper(*p);
  return str;
}

int isnewline_or_space(char *str)
{
  int isblank = -1;
  char *s;
  for (s = str; *s != '\0'; s++) {
    if (!isspace(*s)) {
      isblank = 0;
      break;
    }
  }
  return isblank;
}

// HACK: Don't need anything fancy, just assume that digits are reasonable sized
int num_digits(size_t num)
{
  if (num < 0) {
    fprintf(stderr, "Runner counnt is less than zero\n");
  }
  if (num < 10)
    return 1;
  if (num < 100)
    return 2;
  if (num < 1000)
    return 3;
  if (num < 10000)
    return 4;
  if (num < 100000)
    return 5;
  if (num < 1000000)
    return 6;
  if (num < 10000000)
    return 7;
  if (num < 100000000)
    return 8;
  if (num < 1000000000)
    return 9;
  return 10;
}

int get_substr_index(const char *str, const char *sub_str)
{
  char *result = strstr(str, sub_str);
  if (result) {
    return result - str;
  }
  return -1;
}

char *get_first_word(const char *str)
{
  int str_len = strlen(str);
  char *copy = (char *)malloc(str_len + 1);
  strncpy(copy, str, str_len);
  return strtok(copy, " ");
}

char *sub_str_s_e(const char *str, size_t start, size_t end)
{
  size_t new_len = end - start;
  char *new_str = malloc(sizeof(char) * new_len + 1);
  if (new_str == NULL) {
    return NULL;
  }
  strncpy(new_str, str + start, new_len);
  new_str[end] = '\0';  // ensure a valid c str
  return new_str;
}
