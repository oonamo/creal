#include "creal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "funcs.c"

#define DEFAULT_FLAGS (TRIM_COMMAND_OUTPUT | COMPARE_OUTPUTS)

static uint32_t flags = DEFAULT_FLAGS;
static const char *COMMENT_STR_LEFT = "[[!";
static const char *COMMENT_STR_RIGHT = "!]]";

static file_t File = {
  .flags = DEFAULT_FLAGS,
};

static const struct {
  Action act;
  char *act_str;
} act_map[] = {
  { COMMAND, "command" },      { RETURNCODE, "returncode" }, { NAME, "name" },
  { ACTION_SIZE, NULL },       { INVALID_ACTION, NULL },     { SINGLE_LINE_OUTPUT, NULL },
  { MULTI_LINE_OUTPUT, NULL },
};

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

/// Prints colored output to console.
void print_c(COLOR c, const char *fmt, ...)
{
  if (flags & COLOR_OFF) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    return;
  }

  va_list args;
  char mod_fmt[2000];

  snprintf(mod_fmt, sizeof(mod_fmt), "\x1b[%dm%s\x1b[%dm", c, fmt, CLEAR);

  va_start(args, fmt);
  vfprintf(stdout, mod_fmt, args);
  va_end(args);
}

void verbose_printf(const char *fmt, ...)
{
  if (flags & VERBOSE) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
  }
}

void verbose_print_c(COLOR c, const char *fmt, ...)
{
  if (flags & VERBOSE) {
    if (flags & COLOR_OFF) {
      va_list args;
      va_start(args, fmt);
      vprintf(fmt, args);
      va_end(args);
      return;
    }
    va_list args;
    char mod_fmt[2000];

    snprintf(mod_fmt, sizeof(mod_fmt), "\x1b[%dm%s\x1b[%dm", c, fmt, CLEAR);

    va_start(args, fmt);
    vfprintf(stdout, mod_fmt, args);
    va_end(args);
  }
}

void debug_print_c(COLOR c, const char *fmt, ...)
{
  if (flags & DEBUG) {
    if (flags & COLOR_OFF) {
      va_list args;
      va_start(args, fmt);
      vprintf(fmt, args);
      va_end(args);
      return;
    }
    va_list args;
    char mod_fmt[2000];

    snprintf(mod_fmt, sizeof(mod_fmt), "\x1b[%dm%s\x1b[%dm", c, fmt, CLEAR);

    va_start(args, fmt);
    vfprintf(stdout, mod_fmt, args);
    va_end(args);
  }
}

void debug_printf(const char *fmt, ...)
{
  if (flags & DEBUG) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
  }
}

int f_var_is_unique(file_t *File, const char *name)
{
  FOR_ALL_VARIABLES(i, File) {
    if (strcmp(File->vars[i].name, name) == 0) {
      return i;
    }
  }
  return -1;
}

file_t *init_file_t()
{
  file_t *File = malloc(sizeof(file_t));
  File->vars = malloc(sizeof(f_vars_t));
  File->l_comment_str = (char *)COMMENT_STR_LEFT;
  File->r_comment_str = (char *)COMMENT_STR_RIGHT;
  File->varcount = 0;
  File->flags = DEFAULT_FLAGS;
  return File;
}
void destroy_file_t(file_t *file)
{
  free(file->r_comment_str);
  free(file->l_comment_str);
  FOR_ALL_VARIABLES(i, file) {
    free(file->vars[i].name);
    free(file->vars[i].value);
  }
  free(file->vars);
}

void f_set_template_str(file_t *File, const char *delimiter, const char symbol);

void f_add_var_to_file(file_t *File, char *variable, char *value)
{
  // increase size by 1
  int var_idx = f_var_is_unique(File, variable);
  if (var_idx < 0) {
    var_idx = File->varcount;
    File->vars = realloc(File->vars, (var_idx + 1) * sizeof(f_vars_t));
    if (File->vars == NULL) {
      fprintf(stderr, "failed to allocate memory for variable");
      exit(1);
    }
  } else {
    free(File->vars[var_idx].name);
    free(File->vars[var_idx].value);
  }

  File->vars[var_idx].name = malloc(strlen(variable) + 1);
  File->vars[var_idx].value = malloc(strlen(value) + 1);

  // [name_of_var=value]
  strcpy(File->vars[var_idx].name, variable);
  strcpy(File->vars[var_idx].value, value);
  File->varcount++;
}

void f_set_comment_str(file_t *File, const char *lhs, const char *rhs)
{
  File->l_comment_str = realloc(File->l_comment_str, strlen(lhs) + 1);
  if (File->l_comment_str == NULL) {
    fprintf(stderr, "failed to allocate memory for variable");
    exit(1);
  }
  File->r_comment_str = realloc(File->r_comment_str, strlen(rhs) + 1);
  if (File->r_comment_str == NULL) {
    fprintf(stderr, "failed to allocate memory for variable");
    exit(1);
  }

  strcpy(File->l_comment_str, lhs);
  strcpy(File->r_comment_str, rhs);
}

char *replace_template_str(file_t *File, const char *string)
{
  // 1. Find template string
  size_t ds = index_of_char(string, '$');
  // '$' does not exist
  if (ds < 0) {
    return NULL;
  }

  // 2. find '{' and '}'
  size_t l_brack = index_of_char(string, '{');
  if (l_brack < 0) {
    return NULL;
  }

  size_t r_brack = index_of_char(string, '}');
  if (r_brack < 0) {
    return NULL;
  }

  // ie $} something {
  if (l_brack >= r_brack) {
    return NULL;
  }

  // TODO: Is this reasonable?
  char var_name[MAX_VAR_LENGTH];
  // Memcpy so if var_name is to large, terminate with nulls
  memcpy(var_name, string + l_brack, r_brack - l_brack);

  FOR_ALL_VARIABLES(i, File) {
    if (strcmp(File->vars[i].name, var_name) == 0) {
      // Replace that string with the value of the variable
      return replace_sub_str(string, ds, r_brack, File->vars[i].value);
    }
  }
  // TODO: Should this error ( on strict ), or allow it to be passed literally?
  return NULL;
}

/// Checks if comment contains both COMMENT_STR_LEFT and COMMENT_STR_RIGHT
/// if so, removes everything from the beginning of COMMENT_STR_LEFT to the
/// right
void remove_comment(char *line)
{
  int lhs = get_substr_index(line, COMMENT_STR_LEFT);
  int rhs = get_substr_index(line, COMMENT_STR_RIGHT);
  if (lhs != -1 && rhs != -1) {
    // HACK: avoid strncpy param overlap
    char *copy = strdup(line);
    // assume that size is sufficient
    strncpy(line, copy, lhs);

    line[lhs] = '\0';
  }
}

Creal *init_creal()
{
  Creal *c = malloc(sizeof(Creal));
  if (c == NULL) {
    fprintf(stderr, "Failed to allocate\n");
    exit(1);
  }
  c->lines = 0;
  c->returncode = 0;
  c->output = NULL;
  c->command = NULL;
  c->name = NULL;
  return c;
}

void destory_creal(Creal *creal, int can_destroy_self)
{
  if (creal == NULL) {
    return;
  }
  if (creal->output != NULL) {
    for (size_t i = 0; i < creal->lines; i++) {
      if (creal->output[i] != NULL) {
        free(creal->output[i]);
      }
    }
    free(creal->output);
  }
  if (creal->name != NULL) {
    free(creal->name);
  }
  if (can_destroy_self) {
    free(creal);
  }
}

int flag_is_true(const char *value, int fallback)
{
  char *bool = str_tolower((char *)value);
  if (strcmp(value, "false") == 0) {
    return 0;
  } else if (strcmp(value, "true") == 0 || strcmp(value, value) == 0
             || isnewline_or_space((char *)value)) {
    return 1;
  } else
    return fallback;
}

int parse_flag(const char *unparsed_flag)
{
  char *flag = copy_sub_str(unparsed_flag, '=');
  size_t value_idx = index_of_char(unparsed_flag, '=');
  char *value = copy_sub_str_offset(unparsed_flag, value_idx + 1);
  int flag_on = flag_is_true(value, -1);
  Flags e_flag = INVALID_FLAG;
  debug_printf("flag: %s, value: %s\n", flag, value);

  FOR_ALL_FLAGS(i) {
    if (strcmp(flag, flag_map[i].flag_str) == 0) {
      e_flag = flag_map[i].flag;
      break;
    }
  }

  if (e_flag == INVALID_FLAG) {
    debug_print_c(RED, "found no valid flag for value '%s'\n", flag);
    return -1;
  } else if (e_flag == SETVAR) {
    // set a variable
  } else if (e_flag == NONE) {
    flags = DEFAULT_FLAGS;
  }

  if (flag_on == 1) {
    flags |= e_flag;
    debug_print_c(CYAN, "enabling...\n");
  } else if (flag_on == 0) {
    flags &= ~(e_flag);
    debug_print_c(CYAN, "disabling...\n");
  } else {
    if (flags & STRICT) {
      return -1;
    }
  }
  return e_flag == INVALID_FLAG ? -1 : 0;
}

Action parse_action(Creal *input, const char *action, const char *value)
{
  Action creal_act = INVALID_ACTION;
  if (strcmp(action, "output") == 0) {
    if (strcmp(value, "|") == 0) {
      creal_act = MULTI_LINE_OUTPUT;
    } else {
      creal_act = SINGLE_LINE_OUTPUT;
      add_line(input, value);
    }
    return creal_act;
  }
  FOR_ALL_ACTIONS(i) {
    // avoids *unintentional* access to protected Actions
    if (strcmp(action, act_map[i].act_str) == 0) {
      creal_act = i;
      switch (creal_act) {
      case COMMAND: {
        input->command = malloc(strlen((char *)value) + 1);
        strcpy(input->command, value);
        break;
      }
      case RETURNCODE: {
        input->returncode = atoi(value);
        break;
      }
      case NAME: {
        input->name = (char *)value;
        break;
      }
      case SINGLE_LINE_OUTPUT:
      case MULTI_LINE_OUTPUT:
      case INVALID_ACTION:
      case ACTION_SIZE:
        creal_act = INVALID_ACTION;
        break;
      }
    }
  }
  return creal_act;
}

void add_line(Creal *creal, const char *line)
{
  // increase size by 1
  creal->output = realloc(creal->output, (creal->lines + 1) * sizeof(char *));
  if (creal->output == NULL) {
    fprintf(stderr, "Failed to allocate memory in output\n");
    exit(1);
  }
  if (flags & TRIM_COMMAND_OUTPUT) {
    char *trimmed_line = trim(strdup(line));
    creal->output[creal->lines] = malloc(strlen(trimmed_line) + 1);
    strcpy(creal->output[creal->lines], trimmed_line);
  } else {
    creal->output[creal->lines] = malloc(strlen(line) + 1);
    strcpy(creal->output[creal->lines], line);
  }
  if (creal->output[creal->lines] == NULL) {
    fprintf(stderr, "Failed to allocate memory in lines\n");
    exit(1);
  }
  creal->lines++;
}

void print_flags()
{
  if (flags & VERBOSE) {
    FOR_ALL_FLAGS(i) {
      if (flags & flag_map[i].flag) {
        print_c(CYAN, "\t%s\n", flag_map[i].flag_str);
      }
    }
    printf("\n");
  }
}

void print_diff(const Creal *expected, const Creal *actual, size_t start_of_diff)
{
  size_t line = 0;
  print_c(BLUE, "Printing Diff:\n");
  while (1) {
    if (line >= expected->lines && line > actual->lines)
      break;
    // print normal stdout, before their diff
    if (line < start_of_diff) {
      verbose_printf("expected and actual: \n");
      printf("%s\n", expected->output[line]);
      line++;
      continue;
    }
    if (line < expected->lines) {
      print_c(GREEN, "%s\n", expected->output[line]);
    } else {
      print_c(GREEN, ">>>\n");
    }
    if (line < actual->lines) {
      print_c(RED, "%s\n", actual->output[line]);
    } else {
      print_c(RED, ">>>\n");
    }
    line++;
  }
  printf("\n");
}

void print_output(const Creal *expected, const Creal *actual)
{
  if (flags & ALWAYS_SHOW_OUTPUT) {
    print_c(BLUE, "Expected:\n");
    for (size_t i = 0; i < expected->lines; i++) {
      printf("%s\n", expected->output[i]);
    }
    print_c(BLUE, "Actual:\n");
    for (size_t i = 0; i < actual->lines; i++) {
      printf("%s\n", actual->output[i]);
    }
  }
}

int compare_creals(const Creal *expected, const Creal *actual)
{
  int failed = 0;
  verbose_print_c(YELLOW, "testing command '%s'\n", expected->command);
  verbose_print_c(YELLOW, "testing runner '%s'\n", expected->name);
  print_flags();
  size_t num_lines = expected->lines;
  if (flags & COMPARE_OUTPUTS) {
    if (actual->lines != expected->lines) {
      if (flags & STRICT) {
        print_c(RED, "The output contains diffrent number of lines than "
                     "what was expected:\n");
        print_c(RED, "Expected %zu lines. Found %zu lines.\n", expected->lines, actual->lines);
        failed |= 1;
      } else {
        verbose_print_c(YELLOW, "The output contains diffrent number of lines than "
                                "what was expected:\n");
        verbose_print_c(YELLOW, "Expected %zu lines. Found %zu lines.\n", expected->lines,
                        actual->lines);
      }
      verbose_print_c(YELLOW, "This may result in a pass or failed test, "
                              "which may depend on the active flags\n");
      num_lines = actual->lines > expected->lines ? actual->lines : expected->lines;
    }
    for (size_t i = 0; i < num_lines; i++) {
      if (i >= actual->lines || i >= expected->lines) {
        print_diff(expected, actual, i);
        failed |= 1;
        break;
      } else if (strcmp(actual->output[i], expected->output[i]) != 0) {
        // print diff
        if (flags & TRIM_COMMAND_OUTPUT) {
          if (strcmp(trim(actual->output[i]), trim(expected->output[i])) != 0) {
            size_t act_len = strlen(actual->output[i]);
            size_t exp_len = strlen(expected->output[i]);
            print_diff(expected, actual, i);
            failed |= 1;
            break;
          } else {
            verbose_print_c(YELLOW, "Found an extra new line or space. Ignoring due "
                                    "to 'fail_on_unexpected_newline' flag being "
                                    "active");
          }
        }
      }
    }
  }
  if (actual->returncode != expected->returncode) {
    print_c(RED, "return codes are diffrent\n");
    print_c(RED, "expected:\n");
    printf("\t%d\n", expected->returncode);
    printf("actual:\n");
    printf("\t%d\n", actual->returncode);
    failed |= 1;
  }
  if (failed == 1) {
    print_c(RED, "'%s' failed.\n", expected->name);
  } else {
    print_c(GREEN, "'%s' passed.\n", expected->name);
  }
  print_output(expected, actual);
  return failed;
}

int execute_runner(Creal *runner, char **failures, size_t fail_count)
{
  Creal *actual = init_creal();
  // actual->command = runner->command; <-- May throw error
  actual->command = malloc(strlen(runner->command) + 1);
  strcpy(actual->command, runner->command);
  execute_command(actual);
  int res = compare_creals(runner, actual);
  destory_creal(actual, 1);
  if (res) {
    add_to_failure(failures, runner, fail_count);
    fail_count++;
  }
  return res;
}

void add_to_failure(char **failures, Creal *runner, size_t fail_count)
{
  failures = realloc(failures, (fail_count + 1) * sizeof(char *));
  if (failures == NULL) {
    print_c(RED, "failed to allocate memory for failues\n");
    exit(EXIT_FAILURE);
  }
  failures[fail_count] = malloc(strlen(runner->name) + 1);
  strcpy(failures[fail_count], runner->name);
}

int validate_runner(const Creal *creal)
{
  int ok = 0;
  ok |= creal->command != NULL;
  if (flags & STRICT) {
    ok |= creal->output != NULL;
  }
  return ok;
}

void cleanup(Creal *creal, file_t *file, char **failures, size_t failcount, FILE *fp)
{
  if (creal != NULL) {
    destory_creal(creal, 1);
  }
  if (file != NULL) {
    destroy_file_t(file);
  }
  if (failures != NULL) {
    for (size_t i = 0; i < failcount; i++) {
      free(failures[i]);
    }
    free(failures);
  }
  if (fp != NULL) {
    fclose(fp);
  }
}

/// read line by line
/// Priority List:
/// 1. multiline stdout: Everything is stored into array. No other process is
/// ran until the multiline stdout is done. Delimited with 'stdout: |". To end,
/// add '|' to a line with nothing else
/// 2. Flags: Delimited with '#'s
/// 3. Runner Seperators: seperated with '---'
/// 3. Actions
int read_testfile(const char *input_file, size_t *count)
{
  // get flags
  char buffer[1024];
  FILE *file;
  Creal *input = init_creal();
#ifdef _WIN32
  fopen_s(&file, input_file, "r");
#else
  file = fopen(input_file, "r");
#endif

  if (file == NULL) {
    cleanup(input, NULL, NULL, 0, file);
    return EXIT_FAILURE;
  }

  int is_output = 0;
  size_t runner_count = 0;
  size_t fail_count = 0;
  char **failures = NULL;

  verbose_printf("---\n");
  while (fgets(buffer, sizeof(buffer), file) != NULL) {
    char *copy = strdup(buffer);
    remove_comment(copy);
    char *trimmed_buf = trim(copy);
    char first_char = first_non_empty_char(trimmed_buf);

    // if its empty line
    if (first_char == -1 && !is_output) {
      continue;
    }

    size_t action_idx = index_of_char(copy, ':');

    // if still inside stdout: |
    //                         |
    // block
    if (is_output) {
      if (strcmp(trimmed_buf, "|") == 0) {
        is_output = 0;
        continue;
      }
      add_line(input, buffer);
      continue;
    }
    // Indicates new runner
    else if (strcmp(trimmed_buf, "---") == 0) {
      if (input->name == NULL) {
        input->name = malloc(sizeof(char) * num_digits(runner_count) + 1);
        sprintf(input->name, "%zu", runner_count);
      }

      if (!validate_runner(input)) {
        if (flags & STRICT) {
          print_c(RED, "Bad runner. Make sure to fill all fields or "
                       "disable strict mode\n");
          cleanup(input, NULL, failures, fail_count, file);
          return EXIT_FAILURE;
        }
        // TODO: Print availabe information of runner
        print_c(RED, "Bad runner found, skipping: \n");
        print_creal(input);
        destory_creal(input, 0);
        input = init_creal();
        continue;
      }

      runner_count++;
      execute_runner(input, failures, fail_count);
      destory_creal(input, 0);
      input = init_creal();
      verbose_printf("---\n");
      continue;
    }
    // Indicates flag
    else if (copy[first_char] == '#') {
      char *unparsed = str_tolower(&trimmed_buf[first_char + 1]);
      int res = parse_flag(unparsed);
      if (res == -1) {
        if (flags & STRICT) {
          print_c(RED, "did not find value/flag %s\n", unparsed);
          cleanup(input, NULL, failures, fail_count, file);
          return EXIT_FAILURE;
        }
        verbose_print_c(YELLOW, "flag '%s' does not exist.\n", unparsed);
      }
      continue;
    }
    // indicates action
    else if (action_idx != -1) {
      char *action = copy_sub_str(copy, ':');
      const char *untrimmed = copy_sub_str_offset(copy, action_idx + 1);
      size_t value_idx = first_non_empty_char(untrimmed);
      const char *trimmed_value = trim(copy_sub_str_offset(untrimmed, value_idx));
      Action act = parse_action(input, action, trimmed_value);

      if (act == INVALID_ACTION) {
        if (flags & STRICT) {
          print_c(RED, "invalid action: '%s'. Fatal\n", action);
          cleanup(input, NULL, failures, fail_count, file);
          return EXIT_FAILURE;
        }
        print_c(YELLOW, "invalid action: '%s'. Continuing...\n", action);
      }
      if (act == MULTI_LINE_OUTPUT) {
        is_output = 1;
      }
    }
  }

  // User puts --- at the end of file
  if (!validate_runner(input)) {
    free(input);
  }
  // User removes --- at the end of file
  else {
    if (flags & STRICT) {
      print_c(RED, "found runner at the end of file, but it is never "
                   "terminated.\n");
      cleanup(input, NULL, failures, fail_count, file);
      return EXIT_FAILURE;
    }
    if (input->name == NULL) {
      input->name = malloc(sizeof(char) * num_digits(runner_count));
      sprintf(input->name, "%zu", runner_count);
    }
    runner_count++;
    execute_runner(input, failures, runner_count);

    destory_creal(input, 1);

    verbose_printf("---\n");
    if (fail_count) {
      print_c(RED, "failed runner: \n");
      for (size_t i = 0; i < fail_count; i++) {
        print_c(RED, "\t%s\n", failures[i]);
        free(failures[i]);
      }
      free(failures);
    }
  }
  fclose(file);
  *count = runner_count;
  return fail_count != 0;
}

char *append_std_err_redir(char *cmd)
{
  size_t len = strlen(cmd);
  if (len < 4) {
    cmd = realloc(cmd, strlen(cmd) + strlen(" 2>&1") + 1);
    strcat(cmd, " 2>&1");
    if (cmd == NULL) {
      exit(EXIT_FAILURE);
    }
    return cmd;
  }
  const char *str = copy_sub_str_offset(cmd, len - 4);
  if (strcmp(str, "2>&1") != 0) {
    cmd = realloc(cmd, strlen(cmd) + strlen(" 2>&1") + 1);
    if (cmd == NULL) {
      exit(EXIT_FAILURE);
    }
    strcat(cmd, " 2>&1");
    return cmd;
  }
  return cmd;
}

char *prepend_shell(char *cmd, const char *prep)
{
  size_t len_pre = strlen(prep);
  size_t len_cmd = strlen(cmd);
  char *result = malloc(len_cmd + len_pre + 1);
  if (result != NULL) {
    memmove(result + len_pre, cmd, len_cmd + 1);
    memcpy(result, prep, len_pre);
  }
  return result;
}

void execute_command(Creal *creal)
{
  FILE *fp;
  char buffer[1000];
  size_t line = 0;
  char *command = malloc(strlen(creal->command) + 1);
  strcpy(command, creal->command);
  char *appended = append_std_err_redir(command);
#ifdef _WIN32
  char *cmd = prepend_shell(appended, "cmd /c ");
  fp = _popen(cmd, "r");
#else
  fp = popen(creal->command, "r");
#endif
  if (fp == NULL) {
    fprintf(stderr, "failed to run command %s\n", creal->command);
    exit(EXIT_FAILURE);
  }

  // read line_wise
  while (fgets(buffer, sizeof(buffer), fp) != NULL) {
    add_line(creal, buffer);
  }

#ifdef _WIN32
  creal->returncode = _pclose(fp);
#else
  creal->returncode = pclose(fp) / 256;
#endif
  return;
}

void print_creal(Creal *creal)
{
  if (creal->command)
    printf("command: %s\n", creal->command);
  if (creal->returncode)
    printf("returncode: %d\n", creal->returncode);
  if (creal->lines)
    printf("lines: %zu\n", creal->lines);
  if (creal->name)
    printf("name: %s\n", creal->name);
  for (size_t i = 0; i < creal->lines; i++)
    printf("%s\n", creal->output[i]);
  printf("\n");
}

int main(int argc, char *argv[])
{
  if (argc < 2) {
    printf("USAGE: creal <file>\n");
    exit(EXIT_FAILURE);
  }
  const char *test_file = argv[1];
  size_t runner_count = 0;
  int failed = read_testfile(test_file, &runner_count);
  debug_printf("exited creal successfully.\n");
  if (failed) {
    exit(EXIT_FAILURE);
  }
  exit(EXIT_SUCCESS);
}
