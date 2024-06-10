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
} Flags;

Creal *init_creal();
void destory_creal(Creal *creal, int can_destroy_self);
void read_command();
void execute_command(Creal *ouput);
void add_line(Creal *creal, const char *line);
void print_creal(Creal *creal);

void comapre_creals(const Creal *actual, const Creal *expected, uint32_t flags);

Creal *read_testfile(const char *input_file, size_t *count, uint32_t *flags);
#endif
