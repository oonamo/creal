#ifndef FUNCS_H
#define FUNCS_H
#include <stdint.h>
#include <stdlib.h>
typedef enum COLOR
{
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

void print_c(COLOR c, const char *fmt, ...);
int isnewline_or_space(char *str);

#endif
