#include "creal.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "actions.c"
#include "creal_strings.c"
#include "flags.c"
#include "funcs.c"

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
  c->name = new_str();
  c->file_h = init_file_t();
  return c;
}

void free_creal(Creal *creal, int can_destroy_self)
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
    /*free(creal->name);*/
    free_creal_str_t(creal->name);
  }
  if (can_destroy_self) {
    /*free(creal->file_h);*/
    free_file_t(creal->file_h);
    free(creal);
  }
}

void remove_comment(creal_str_t *line)
{
  int start, end;
  if ((start = c_get_substr_index(line, "[[!")) != -1
      && (end = c_get_substr_index(line, "!]]")) != -1) {
    if (start >= end) {
      return;
    }
    /*printf("found comment\n");*/
    copy_till(line, start);
    c_trim(line);
  }
}

void add_line(Creal *input, const creal_str_t *line)
{
  input->output = realloc(input->output, (input->lines + 1) * sizeof(char *));
  ASSERT_NOT_NULL(input->output);

  input->output[input->lines] = malloc(line->size);
  ASSERT_NOT_NULL(input->output[input->lines]);

  strcpy(input->output[input->lines], line->str);
  input->output[input->lines][line->size - 1] = '\0';

  input->lines++;
}

// 1. Check if it is in multiline output, as it escapes everything.
// 2. Check if the line is a flag
// 3. Check if the line is an action
// 4. Check if the line is the end of a runner
// 5. Check if the line is an action
int read_creal_file(const char *file)
{
  FILE *fp = NULL;
#ifdef _WIN32
  fopen_s(&fp, file, "r");
#else
  file = fopen(file, "r");
#endif
  if (file == NULL) {
    fprintf(stderr, "error, could not open file");
    return EXIT_FAILURE;
  }

  Creal *input = init_creal();
  int is_output;
  char buffer[1024];

  while (fgets(buffer, sizeof(buffer), fp) != NULL) {
    creal_str_t *buf_cpy = from_str(buffer, 1);
    c_trim(buf_cpy);

    // escapes everyting
    if (is_output) {
      // If exiting multiline output
      if (is_multiline(buf_cpy)) {
        is_output = 0;
      } else {
        add_line(input, buf_cpy);
      }
      goto clean;
    }

    int idx;
    remove_comment(buf_cpy);
    print_str_debug(buf_cpy);

    // idx will be the first index of '#'
    if ((idx = is_flag(buf_cpy)) != -1) {
      // check if flag has value
      if (parse_flag(input, buf_cpy, idx) == 1) {
        printf("error: %s is not a valid line. The flag or value in the line is not valid.\n",
               buf_cpy->str);
        exit(EXIT_FAILURE);
      }
      // idx will be the first index of ':'
    } else if ((idx = is_action(buf_cpy)) != -1) {
      Action act = parse_action(input, buf_cpy, idx);
      if (act == MULTI_LINE_OUTPUT) {
        is_output = 1;
      } else if (act == SINGLE_LINE_OUTPUT) {
        add_line(input, buf_cpy);
      }
    } else if (is_runner(buf_cpy)) {
    }

  clean:
    free_creal_str_t(buf_cpy);
    free_creal(input, 0);

    // HACK: Free, then create new object
    input = init_creal();
    continue;
  }

  return 0;
}

int main(int argc, char *argv[])
{
  int result = read_creal_file(argv[1]);
}
