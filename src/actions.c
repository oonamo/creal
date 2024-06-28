#include "actions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "creal_strings.h"

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

#define FOR_ALL_ACTIONS(i) for (int i = act_map[0].act; act_map[i].act_str != NULL; i++)

int is_in_output(creal_str_t* line)
{
  // TODO: How can we compare using characters ?
  return strcmp(line->str, "|") != 0;
}

int is_runner(creal_str_t* line)
{
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

Action parse_action(Creal* input, creal_str_t* line, int semicolon_idx)
{
  char* action = malloc(semicolon_idx + 1);
  ASSERT_NOT_NULL(action);
  strncpy(action, line->str, semicolon_idx);
  action[semicolon_idx] = '\0';
  Action c_action = INVALID_ACTION;

  // HACK: using creal_str_t to dynamicallly create trimmed string
  creal_str_t* value = from_str(line->str + semicolon_idx + 1, 1);
  c_trim(value);

  FOR_ALL_ACTIONS(i) {
    if (strcmp(act_map[i].act_str, action) == 0) {
      act_map[i].act_handler(input, value->str);
      c_action = act_map[i].act;
    }
  }

  free(action);
  free_creal_str_t(value);
  return c_action;
}
