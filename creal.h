#ifndef CREAL_H
#define CREAL_H
#include "funcs.h"
#include <stdint.h>
#include <stdlib.h>
typedef struct
{
    int returncode;
    size_t lines;
    char *command;
    char **output;
    char *name;

} Creal;

typedef enum
{
    NONE = 0,
    STRICT = (1 << 1),
    FAIL_UNEXPECTED_NEWLINES = (1 << 2),
    TRIM_COMMAND_OUTPUT = (1 << 3),
    VERBOSE = (1 << 4),
    COLOR_OFF = (1 << 5),
    DEBUG = (1 << 6),
    APPEND_RELATIVE = (1 << 7),
} Flags;

Creal *init_creal();
void print_c(COLOR c, const char *fmt, ...);
void verbose_printf(const char *fmt, ...);
void verbose_print_c(COLOR c, const char *fmt, ...);
void debug_print_c(COLOR c, const char *fmt, ...);
void debug_printf(const char *fmt, ...);
void destory_creal(Creal *creal, int can_destroy_self);
int flag_is_true(const char *value, int fallback);
int parse_flag(const char *unparsed_flag);
void add_line(Creal *creal, const char *line);
void print_flags();
void comapre_creals(const Creal *actual, const Creal *expected);
int execute_runner(Creal *runner, char **failures, size_t fail_count);
int validate_runner(const Creal *creal);
void add_to_failure(char **failures, Creal *input, size_t fail_count);
void read_testfile(const char *input_file, size_t *count);
char *append_std_err_redir(char *cmd);
char *prepend_shell(char *cmd, const char *prep);
void print_creal(Creal *creal);
void execute_command(Creal *ouput);

#endif
