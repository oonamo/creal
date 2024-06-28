#include "flags.h"

#include <stdio.h>
#include <string.h>

#include "creal.h"
#include "creal_file.c"

static const struct {
  Flags flag;
  char *flag_str;
} flag_map[] = {
  { NONE, "none" },
  { STRICT, "strict" },
  { FAIL_UNEXPECTED_NEWLINES, "fail_on_unexpected_newline" },
  { TRIM_COMMAND_OUTPUT, "trim_command_output" },
  { VERBOSE, "verbose" },
  { COLOR_OFF, "color_off" },
  { DEBUG, "debug" },
  { APPEND_RELATIVE, "append_relative" },
  { ALWAYS_SHOW_OUTPUT, "always_show_output" },
  { SET_COMMENT_STRING, "set_comment_string" },
  { COMPARE_OUTPUTS, "compare_outputs" },
  { SETVAR, "set" },
  { FLAG_SIZE, NULL },
  { INVALID_FLAG, NULL },
};

int is_set(const char *flag)
{
  // check if first three chars make 'set'
  char three_chars[4];
  strncpy(three_chars, flag, 4);
  three_chars[3] = '\0';
  int res = strcmp(three_chars, "set");

  // check if fourth character is space
  return res == 0 && flag[3] == ' ';
}

int flag_is_true(const char *value, int fallback)
{
  if (value == NULL) {
    return -1;
  }
  return strcmp(value, "true") == 0;
}

int parse_flag(Creal *input, const creal_str_t *line, int flag_start)
{
  // check if flag has value
  int value_idx = c_index_of_char(line, '=');
  char *value = NULL;
  char *flag = NULL;

  if (value_idx != -1 && value_idx <= flag_start) {
    // SYNTAX ERROR
    fprintf(stderr, "Syntax error\n");
    /*exit(EXIT_FAILURE);*/
    return 1;
  }

  if (value_idx != -1) {
    value = malloc(line->size - value_idx - 1);
    ASSERT_NOT_NULL(value);
    flag = malloc(value_idx - flag_start);
    ASSERT_NOT_NULL(flag);

    strncpy(value, line->str + value_idx + 1, line->size - value_idx - 1);
    strncpy(flag, line->str + flag_start + 1, value_idx - flag_start - 1);

    value[line->size - value_idx - 2] = '\0';
    flag[value_idx - flag_start - 1] = '\0';

    printf("flag: %s, value: %s\n", flag, value);

  } else {
    flag = malloc(line->size - flag_start - 1);
    ASSERT_NOT_NULL(flag);

    strncpy(flag, line->str + flag_start + 1, line->size - flag_start - 1);
    flag[line->size - flag_start - 2] = '\0';
  }

  if (is_set(flag)) {
    trim(value);
    char *name = malloc(strlen(flag) - 3);
    char *varname = trim(name);
    strncpy(name, flag + 3, strlen(flag) - 3);
    f_add_var_to_file(input->file_h, varname, value);
    free(name);
    free(flag);
    return 0;
  }

  if (strcmp(flag, "set") == 0) {
    trim(value);
    // value also holds flag
    // handle variable
    /*f_add_var_to_file(input->file_h, value);*/
    free(flag);
    if (value != NULL) {
      free(value);
    }
    return 0;
  }

  FOR_ALL_FLAGS(i) {
    if (strcmp(flag_map[i].flag_str, flag) == 0) {
      int result = flag_is_true(value, 0);
      if (result == 1) {  // FIXME: seg fault
        input->file_h->flags &= flag_map[i].flag;
      } else {
        input->file_h->flags ^= ~flag_map[i].flag;
      }
      free(flag);
      if (value != NULL) {
        free(value);
      }
      return 0;
    }
  }
  free(flag);
  if (value != NULL) {
    free(value);
  }
  return 1;
}
