#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "creal.h"
#include "creal_strings.h"

#define DEFAULT_FLAGS (TRIM_COMMAND_OUTPUT | COMPARE_OUTPUTS)

file_t *init_file_t()
{
  file_t *new = malloc(sizeof(file_t));
  new->vars = malloc(sizeof(f_vars_t));
  new->varcount = 0;
  new->flags = DEFAULT_FLAGS;
  return new;
}

void free_file_t(file_t *file)
{
  if (file == NULL) {
    return;
  }
  FOR_ALL_VARIABLES(i, file) {
    free(file->vars[i].name);
    free(file->vars[i].value);
  }
  if (file->varcount != 0) {
    free(file->vars);
  }
  free(file);
}

int f_var_is_unique(file_t *file, const char *name)
{
  FOR_ALL_VARIABLES(i, file) {
    if (strcmp(file->vars[i].name, name) == 0) {
      return i;
    }
  }
  return -1;
}

void f_add_var_to_file(file_t *file, char *variable, char *value)
{
  int var_idx = f_var_is_unique(file, variable);
  if (var_idx < 0) {
    var_idx = file->varcount;
    file->vars = realloc(file->vars, (var_idx + 1) * sizeof(f_vars_t));
    if (file->vars == NULL) {
      fprintf(stderr, "failed to allocate memory for variable");
      exit(1);
    }
  } else {
    free(file->vars[var_idx].name);
    free(file->vars[var_idx].value);
  }

  file->vars[var_idx].name = malloc(strlen(variable) + 1);
  file->vars[var_idx].value = malloc(strlen(value) + 1);

  // [name_of_var=value]
  strcpy(file->vars[var_idx].name, variable);
  strcpy(file->vars[var_idx].value, value);
  file->varcount++;
}
