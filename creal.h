#ifndef CREAL_H
#define CREAL_H
#include <stdlib.h>
typedef struct
{
    int returncode;
    size_t lines;
    char *command;
    char **output;
} Creal;

Creal *init_creal();
void destory_creal(Creal *creal);
void read_command();
void execute_command(Creal *ouput, char *cmd);
void add_line(Creal *creal, const char *line);
void print_creal(Creal *creal);

void read_testfile(Creal *input, const char *input_file);
#endif
