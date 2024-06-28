#include "../../src/creal.h"
#include "../../src/creal_strings.c"
#include "../../src/flags.c"
#include "../../src/funcs.c"

// Copied from creal.c
// Adapted for testing
Flags test_parse_flag(Creal *input, const creal_str_t *line, int flag_start)
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

    printf("got here\n");
  }

  if (is_set(flag)) {
    trim(value);
    char *name = malloc(strlen(flag) - 3);
    strncpy(name, flag + 3, strlen(flag) - 3);
    char *trimname = trim(name);
    printf("name: %s\n", trimname);

    printf("varname: %s, value: %s\n", trimname, value);
    free(flag);
    free(name);
    return SETVAR;
  }

  printf("testing done\n");

  FOR_ALL_FLAGS(i) {
    if (strcmp(flag_map[i].flag_str, flag) == 0) {
      printf("flag: %s\n", flag);
      free(flag);
      if (value != NULL) {
        printf("value: %s\n", value);
        free(value);
      }
      return flag_map[i].flag;
    }
  }
  free(flag);
  if (value != NULL) {
    free(value);
  }
  return 1;
}

struct creal_flag_testcase {
  Flags flag;
  char *unparsed;
};

int main()
{
  struct creal_flag_testcase tests[] = {
    { STRICT, "#strict" },
    { STRICT, "#strict=true" },
    { FAIL_UNEXPECTED_NEWLINES, "#fail_on_unexpected_newline" },
    { TRIM_COMMAND_OUTPUT, "#trim_command_output" },
    { VERBOSE, "#verbose" },
    { COLOR_OFF, "#color_off" },
    { DEBUG, "#debug" },
    { APPEND_RELATIVE, "#append_relative" },
    { ALWAYS_SHOW_OUTPUT, "#always_show_output" },
    { SET_COMMENT_STRING, "#set_comment_string" },
    { COMPARE_OUTPUTS, "#compare_outputs" },
    { SETVAR, "#set pwsh=something" },
    { NONE, NULL },
  };
  struct creal_flag_testcase *tp = tests;
  int fails = 0;
  while (tp->unparsed != NULL) {
    printf("testing flag: %s\n", tp->unparsed);
    creal_str_t *c_flag = from_str(tp->unparsed, 1);
    printf("teting flag\n");
    Flags result = test_parse_flag(NULL, c_flag, 0);
    printf("parsed flag, checking result.\n");
    if (result != tp->flag) {
      printf("failed to find unparsed flag '%s'\n", tp->unparsed);
      fails++;
    }
    free_creal_str_t(c_flag);
    tp++;
  }
  if (fails) {
    printf("failed.\n");
  } else {
    printf("paseed.\n");
  }
  return fails != 0;
}
