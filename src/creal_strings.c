#include "creal_strings.h"

#include <stdio.h>
#include <string.h>

#include "funcs.h"

static const char* COMMENT_STR_LEFT = "[[!";
static const char* COMMENT_STR_RIGHT = "!]]";

// TODO: setting size to include the nullbyte character is very confusing. This should be changed to
// reflect what is given by strlen

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
  ASSERT_NOT_NULL(new);
  new->str = NULL;
  new->size = 0;
  new->allocated = 0;
  return new;
}

creal_str_t* from_str(char* str, int allocate)
{
  ASSERT_NOT_NULL(str);
  size_t len = strlen(str) + 1;
  creal_str_t* new = new_str();
  if (allocate) {
    new->str = malloc(len);
    ASSERT_NOT_NULL(new->str);
    new->allocated = 1;
    strncpy(new->str, str, len);
    new->str[len - 1] = '\0';
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
    /*free(dest->str);*/
    /*dest->str = malloc(len);*/
    dest->str = realloc(dest->str, len);
    /*printf("reallocating memory\n");*/
    if (dest->str == NULL) {
      fprintf(stderr, "Failed to allocate memory for a string");
      exit(EXIT_FAILURE);
    }
  }
  dest->size = len;
  strncpy(dest->str, source, dest->size);
  dest->str[len - 1] = '\0';
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

void free_creal_str_t(creal_str_t* str)
{
  if (str->allocated && str->str != NULL) {
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

void c_trim(creal_str_t* str)
{
  // TODO: change all references to str->size to str->size + 1
  char* copy = malloc(str->size);
  strncpy(copy, str->str, str->size);
  copy[str->size - 1] = '\0';
  char* result = trim(copy);
  if (result[strlen(result)] != '\0') {
    fprintf(stderr, "last char is not null\n");
  }
  copy_str(str, result);
  free(copy);
}

int c_get_substr_index(const creal_str_t* str, const char* sub_str)
{
  if (str->str == NULL)
    return -1;
  char* result = strstr(str->str, sub_str);
  int position = result - str->str;
  return position;
}

int c_substr_n(const creal_str_t* str, size_t start, size_t end);
creal_str_t* c_copy_till(const creal_str_t* str, size_t idx)
{
  if (str->size - 1 < idx) {
    return NULL;
  }
  creal_str_t* new = allocate(idx);
  strncpy(new->str, str->str, new->size);
  new->str[new->size - 1] = '\0';
  return new;
}

void copy_till(creal_str_t* str, size_t idx)
{
  if (str->size - 1 < idx) {
    // TODO: Runtime error
    return;
  }
  char* copy = malloc(idx + 1);
  ASSERT_NOT_NULL(copy);
  strncpy(copy, str->str, idx);
  // copy[idx + 1] = '\0';  // <--- buffer overflow
  copy[idx] = '\0';

  if (!str->allocated) {
    str->str = malloc(idx + 1);
  } else {
    str->str = realloc(str->str, idx + 1);
  }
  ASSERT_NOT_NULL(str->str);
  copy_str(str, copy);
}
creal_str_t* c_copy_till_char(const creal_str_t* str, char l);

creal_str_t* c_copy_after(const creal_str_t* str, size_t idx)
{
  if (str->size < idx) {
    return NULL;
  }
  creal_str_t* new = allocate(str->size - idx);
  strncpy(new->str, str->str + idx, new->size);
  new->str[new->size - 1] = '\0';
  return new;
}

creal_str_t* c_copy_from(const creal_str_t* str, size_t start, size_t len)
{
  if (str->size < len) {
    return NULL;
  }
  creal_str_t* new = allocate(len);
  strncpy(new->str, str->str + start, new->size);
  return new;
}
