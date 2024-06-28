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
  printf("adding name: %s\n", name);
  if (input->name == NULL) {
    input->name = from_str(name, 1);
  } else {
    copy_str(input->name, name);
  }
}

const struct {
  Action act;
  char* act_str;
  void (*act_handler)(Creal*, char*);
} act_map[] = {
  { COMMAND, "command", &a_handle_command },
  { RETURNCODE, "returncode", &a_handle_returncode },
  { NAME, "name", &a_handle_name },
  { SINGLE_LINE_OUTPUT, NULL, NULL },
  { MULTI_LINE_OUTPUT, NULL, NULL },
  { ACTION_SIZE, NULL, NULL },
  { INVALID_ACTION, NULL, NULL },
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

  // check for single line output
  if (strcmp(action, "output") == 0) {
    if (strcmp(value->str, "|") == 0) {
      c_action = MULTI_LINE_OUTPUT;
    } else {
      c_action = SINGLE_LINE_OUTPUT;
    }
    free(action);
    free_creal_str_t(value);
    return c_action;
  }

  FOR_ALL_ACTIONS(i) {
    if (strcmp(act_map[i].act_str, action) == 0) {
      printf("found action: %s\n", act_map[i].act_str);
      act_map[i].act_handler(input, value->str);
      c_action = act_map[i].act;
    }
  }

  free(action);
  free_creal_str_t(value);
  return c_action;
}
