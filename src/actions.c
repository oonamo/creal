#include <stdlib.h>
#include <string.h>

#include "creal.h"

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
