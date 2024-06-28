#include "actions.h"

#include <stdlib.h>
#include <string.h>

#include "funcs.h"

void a_handle_command(Creal* input, char* cmd)
{
  input->command = malloc(strlen(cmd) + 1);
  strcpy(input->command, cmd);
}

void a_handle_returncode(Creal* input, char* returncode)
{
  input->returncode = atoi(returncode);
}

void a_handle_name(Creal* input, char* name)
{
  input->name = name;
}

const struct {
  Action act;
  char* act_str;
  void (*act_handler)(Creal*, char*);
} act_map[] = {
  { COMMAND, "command", &a_handle_command },
  { RETURNCODE, "returncode", &a_handle_returncode },
  { NAME, "name", &a_handle_name },
  { ACTION_SIZE, NULL, NULL },
  { INVALID_ACTION, NULL },
  { SINGLE_LINE_OUTPUT, NULL },
  { MULTI_LINE_OUTPUT, NULL },
};

int is_in_output(creal_str_t* line)
{
  // TODO: How can we compare using characters ?
  /*if (strcmp(line->str, "|") == 0) {*/
  /*  *isoutput ^= ~(*isoutput);*/
  /*  return 1;*/
  /*}*/
  /*return 0;*/
  return strcmp(line->str, "|") != 0;
}

int is_runner(creal_str_t* line)
{
  /*if (strcmp(line->str, "---") == 0) {*/
  /*  return 1;*/
  /*}*/
  /*return 0;*/
  return strcmp(line->str, "---") == 0;
}

int is_flag(creal_str_t* line)
{
  int idx = c_first_non_empty_char(line);
  if (idx == -1)
    return -1;
  if (line->str[idx] == '#') {
    return idx;
  }
  return -1;
}

int is_action(creal_str_t* line)
{
  int idx = c_index_of_char(line, ':');
  return idx;
}

int is_multiline(creal_str_t* line)
{
  return strcmp(line->str, "|") != 0;
}
