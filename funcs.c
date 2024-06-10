#include "funcs.h"
#include <ctype.h>
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
    char *dest = (char *)malloc((idx + 1) * sizeof(char));
    if (dest == NULL)
    {
        printf("failed to allocate memory\n");
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
