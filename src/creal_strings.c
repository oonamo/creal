#include "creal_strings.h"

#include <stdio.h>
#include <string.h>

#include "funcs.h"

static const char* COMMENT_STR_LEFT = "[[!";
static const char* COMMENT_STR_RIGHT = "!]]";

void print_str_debug(creal_str_t* str)
{
  if (str != NULL) {
    printf("str: %s, size: %zu, allocated: %d\n", str->str, str->size, str->allocated);
  }
}

creal_str_t* new_str()
{
  /*creal_str_t new = { .str = NULL, .size = 0, .allocated = 0 };*/
  /*return new;*/
  creal_str_t* new = malloc(sizeof(creal_str_t));
  new->str = NULL;
  new->size = 0;
  new->allocated = 0;
  return new;
}

creal_str_t* from_str(char* str, int allocate)
{
  size_t len = strlen(str) + 1;
  creal_str_t* new = new_str();
  if (allocate) {
    new->str = malloc(len);
    new->allocated = 1;
    strncpy(new->str, str, len);
  } else {
    new->str = str;
  }
  new->size = len;
  return new;
}

void copy_str(creal_str_t* dest, char* source)
{
  size_t len = strlen(source) + 1;
  if (!dest->allocated) {
    dest->str = malloc(len);
    dest->size = len;
    dest->allocated = 1;
  }
  if (dest->allocated && dest->size != len) {
    dest->str = realloc(dest->str, len);
    if (dest->str == NULL) {
      fprintf(stderr, "Failed to allocate memory for a string");
      exit(EXIT_FAILURE);
    }
  }
  dest->size = len;
  strcpy(dest->str, source);
  dest->str[len] = '\0';
}

creal_str_t* allocate(size_t size)
{
  /*creal_str_t new = {*/
  /*  .size = size,*/
  /*  .str = malloc(size),*/
  /*  .allocated = 1,*/
  /*};*/
  creal_str_t* new = new_str();
  new->str = malloc(size);
  new->allocated = 1;
  return new;
}

void delete_str(creal_str_t* str)
{
  if (str->allocated) {
    printf("freeing\n");
    free(str->str);
  }
  free(str);
}

// chars
int c_first_non_empty_char(const creal_str_t* str)
{
  int i = 0;
  while (str->str[i] != '\0' && str->str[i] == ' ') {
    i++;
  }
  if (str->str[i] == '\0') {
    return -1;
  }
  return i;
}

int c_index_of_char(const creal_str_t* str, char c)
{
  int i = -1;
  char* ret = strchr(str->str, c);
  if (ret == NULL) {
    return i;
  }
  i = ret - str->str;
  return i;
}

// strings
creal_str_t* c_replace_sub_str(creal_str_t* original, size_t start, size_t end, char* replacement)
{
  int suffix_len = original->size - end - 1;
  int rep_len = strlen(replacement);

  int new_len = start + rep_len + suffix_len + 1;
  if (original->allocated) {
    original->str = (char*)realloc(original->str, new_len * sizeof(char));
    memmove(original->str + start + rep_len, original->str + end + 1, suffix_len);
    strncpy(original->str + start, replacement, rep_len);
  } else {
  }
}

creal_str_t* c_copy_sub_str_offset(const char* creal_string_t, size_t offset);
creal_str_t* c_str_tolower(creal_str_t* str);
creal_str_t* c_str_toupper(creal_str_t* str);
creal_str_t* c_isnewline_or_space(creal_str_t* str);

creal_str_t* c_ltrim(creal_str_t* str);
creal_str_t* c_rtrim(creal_str_t* str);
creal_str_t* c_trim(creal_str_t* str)
{
  return from_str(trim(str->str), 1);
}

void remove_comment(creal_str_t* str)
{
  int lhs = get_substr_index(str->str, COMMENT_STR_LEFT);
  int rhs = get_substr_index(str->str, COMMENT_STR_RIGHT);
  if (lhs != -1 && rhs != -1) {
    // FIXME: Is their a more efficent way to do this?
    char* copy = strdup(str->str);
    if (str->allocated) {
      str->str = realloc(str->str, lhs);
    } else {
      str->str = malloc(lhs);
    }
    str->allocated = 1;
    strncpy(str->str, copy, lhs);
    str->str[lhs] = '\0';
    str->size = lhs;
  }
}

int c_get_substr_index(const creal_str_t* str, const creal_str_t* sub_str);
int c_substr_n(const creal_str_t* str, size_t start, size_t end);
