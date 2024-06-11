#ifndef CREAL_H
#define CREAL_H
#include <stdint.h>
#include <stdlib.h>
typedef struct
{
    int returncode;
    size_t lines;
    char *command;
    char **output;

} Creal;

typedef enum
{
    STRICT = (1 << 1),
    FAIL_UNEXPECTED_NEWLINES = (1 << 2),
    TRIM_COMMAND_OUTPUT = (1 << 3),
    VERBOSE = (1 << 4),
    COLOR_OFF = (1 << 5),
} Flags;

Creal *init_creal();
void destory_creal(Creal *creal, int can_destroy_self);
void add_line(Creal *creal, const char *line);
void comapre_creals(const Creal *actual, const Creal *expected);
Creal *read_testfile(const char *input_file, size_t *count);
void append_std_err_redir(char *cmd);
void execute_command(Creal *ouput);
void print_creal(Creal *creal);

#endif
