#ifndef ACTIONS_H
#define ACTIONS_H

#include "creal.h"
#include "creal_strings.h"

void a_handle_command(Creal* input, char* cmd);
void a_handle_returncode(Creal* input, char* returncode);
void a_handle_name(Creal* input, char* name);

int is_in_output(creal_str_t* line);
int is_multiline(creal_str_t* line);
int is_runner(creal_str_t* line);
int is_flag(creal_str_t* line);
int is_action(creal_str_t* line);

#endif
