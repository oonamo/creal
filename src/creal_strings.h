#ifndef CREAL_STRINGS
#define CREAL_STRINGS
#include <stdint.h>
#include <stdlib.h>

typedef struct {
  char* str;
  int allocated;
  size_t size;
} creal_str_t;

// creation
creal_str_t* new_str();
creal_str_t* from_str(char* str, int allocate);
creal_str_t* allocate(size_t size);
void delete_str(creal_str_t* str);

// chars
int c_first_non_empty_char(const creal_str_t* str);
int c_index_of_char(const creal_str_t* str, char c);

// strings
creal_str_t* c_replace_sub_str(creal_str_t* orignal, size_t start, size_t end, char* replacement);
creal_str_t* c_copy_sub_str_offset(const char* creal_string_t, size_t offset);
creal_str_t* c_str_tolower(creal_str_t* str);
creal_str_t* c_str_toupper(creal_str_t* str);
creal_str_t* c_isnewline_or_space(creal_str_t* str);

creal_str_t* c_ltrim(creal_str_t* str);
creal_str_t* c_rtrim(creal_str_t* str);
creal_str_t* c_trim(creal_str_t* str);

int c_get_substr_index(const creal_str_t* str, const creal_str_t* sub_str);
int c_substr_n(const creal_str_t* str, size_t start, size_t end);

#endif
