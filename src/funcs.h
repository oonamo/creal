#ifndef FUNCS_H
#define FUNCS_H
#include <stdint.h>
#include <stdlib.h>
typedef enum COLOR {
  CLEAR = 0,
  BLACK = 30,
  RED = 31,
  GREEN = 32,
  YELLOW = 33,
  BLUE = 34,
  PURPLE = 35,
  CYAN = 36,
  WHITE = 37,
} COLOR;
size_t first_non_empty_char(const char *str);

void print_bits(uint32_t bits);

char *str_tolower(char *str);

char *str_toupper(char *str);

int isnewline_or_space(char *str);

int get_substr_index(const char *str, const char *sub_str);

char *get_first_word(const char *str);
char *sub_str_s_e(const char *str, size_t start, size_t end);

char *trim(char *s);
#endif
