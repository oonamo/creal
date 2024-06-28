#ifndef CREAL_FILE_H
#define CREAL_FILE_H

#include <stdint.h>
typedef struct {
  char *name;
  char *value;
} f_vars_t;

typedef struct {
  size_t varcount;
  f_vars_t *vars;
  uint32_t flags;
} file_t;

#define MAX_VAR_LENGTH             25

#define FOR_ALL_VARIABLES(i, file) for (size_t i = 0; i < file->varcount; i++)

file_t *init_file_t();
void free_file_t(file_t *file);

/* Global Settings */
void f_add_var_to_file(file_t *file, char *variable, char *value);
int f_var_is_unique(file_t *file, const char *name);
#endif
