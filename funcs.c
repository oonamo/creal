#include "funcs.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

size_t first_non_empty_char(const char *str)
{
    size_t i = 0;
    while (str[i] != '\0' && str[i] == ' ')
    {
        i++;
    }
    if (str[i] == '\0')
        return -1;
    return i;
}

size_t index_of_char(const char *str, char c)
{
    size_t i = -1;
    char *ret = strchr(str, c);
    if (ret == NULL)
        return i;
    i = ret - str;
    return i;
}

char *copy_sub_str(const char *source, char delimiter)
{
    size_t idx = index_of_char(source, delimiter);
    if (idx == -1)
    {
        return strdup(source);
    }
    char *dest = (char *)malloc((idx + 1));
    if (dest == NULL)
    {
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
    return strdup(source + offset);
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
    while (isspace(*--back))
        ;
    *(back + 1) = '\0';
    return s;
}

char *trim(char *s) { return rtrim(ltrim(s)); }

void print_bits(uint32_t bits)
{
    for (int i = 0; i < 32; i++)
    {
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

void print_c(COLOR c, const char *fmt, ...)
{
    va_list args;
    char mod_fmt[2000];

    snprintf(mod_fmt, sizeof(mod_fmt), "\x1b[%dm%s\x1b[%dm", c, fmt, CLEAR);

    va_start(args, fmt);
    vfprintf(stdout, mod_fmt, args);
    va_end(args);
}

int isnewline_or_space(char *str)
{
    int isblank = -1;
    char *s;
    for (s = str; *s != '\0'; s++)
    {
        if (!isspace(*s))
        {
            isblank = 0;
            break;
        }
    }
    return isblank;
}
