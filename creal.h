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
void destory_creal(Creal *creal, int can_destroy_self);
void read_command();
void execute_command(Creal *ouput);
void add_line(Creal *creal, const char *line);
void print_creal(Creal *creal);

void comapre_creals(const Creal *actual, const Creal *expected);

Creal *read_testfile(const char *input_file, size_t *count);
#endif
