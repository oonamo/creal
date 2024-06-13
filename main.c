#include "creal.h"
#include "funcs.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_FLAGS (TRIM_COMMAND_OUTPUT)
static uint32_t flags = DEFAULT_FLAGS;

void print_c(COLOR c, const char *fmt, ...)
{
    if (flags & COLOR_OFF)
    {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
        return;
    }

    va_list args;
    char mod_fmt[2000];

    snprintf(mod_fmt, sizeof(mod_fmt), "\x1b[%dm%s\x1b[%dm", c, fmt, CLEAR);

    va_start(args, fmt);
    vfprintf(stdout, mod_fmt, args);
    va_end(args);
}

void verbose_printf(const char *fmt, ...)
{
    if (flags & VERBOSE)
    {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
    }
}

void verbose_print_c(COLOR c, const char *fmt, ...)
{
    if (flags & VERBOSE)
    {
        // TODO: Figure out how to redirect this to verbose_printf
        if (flags & COLOR_OFF)
        {
            va_list args;
            va_start(args, fmt);
            vprintf(fmt, args);
            va_end(args);
            return;
        }
        va_list args;
        char mod_fmt[2000];

        snprintf(mod_fmt, sizeof(mod_fmt), "\x1b[%dm%s\x1b[%dm", c, fmt, CLEAR);

        va_start(args, fmt);
        vfprintf(stdout, mod_fmt, args);
        va_end(args);
    }
}

void debug_print_c(COLOR c, const char *fmt, ...)
{
    if (flags & DEBUG)
    {
        // TODO: Figure out how to redirect this to verbose_printf
        if (flags & COLOR_OFF)
        {
            va_list args;
            va_start(args, fmt);
            vprintf(fmt, args);
            va_end(args);
            return;
        }
        va_list args;
        char mod_fmt[2000];

        snprintf(mod_fmt, sizeof(mod_fmt), "\x1b[%dm%s\x1b[%dm", c, fmt, CLEAR);

        va_start(args, fmt);
        vfprintf(stdout, mod_fmt, args);
        va_end(args);
    }
}

void debug_printf(const char *fmt, ...)
{
    if (flags & DEBUG)
    {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
    }
}

Creal *init_creal()
{
    Creal *c = malloc(sizeof(Creal));
    if (c == NULL)
    {
        fprintf(stderr, "Failed to allocate\n");
        exit(1);
    }
    c->lines = 0;
    c->returncode = 0;
    c->output = NULL;
    c->command = NULL;
    c->name = NULL;
    return c;
}

void destory_creal(Creal *creal, int can_destroy_self)
{
    if (creal == NULL)
    {
        return;
    }
    if (creal->output != NULL)
    {
        for (size_t i = 0; i < creal->lines; i++)
        {
            if (creal->output[i] != NULL)
            {
                free(creal->output[i]);
            }
        }
        free(creal->output);
    }
    if (creal->name != NULL)
    {
        free(creal->name);
    }
    if (can_destroy_self)
    {
        free(creal);
    }
}

int flag_is_true(const char *value, int fallback)
{
    char *bool = str_tolower((char *)value);
    if (strcmp(value, "false") == 0)
    {
        return 0;
    }
    else if (strcmp(value, "true") == 0 || strcmp(value, value) == 0 ||
             isnewline_or_space((char *)value))
    {
        return 1;
    }
    else
        return fallback;
}

int parse_flag(const char *unparsed_flag)
{
    char *flag = copy_sub_str(unparsed_flag, '=');
    size_t value_idx = index_of_char(unparsed_flag, '=');
    char *value = copy_sub_str_offset(unparsed_flag, value_idx + 1);
    int flag_on = flag_is_true(value, -1);
    Flags e_flag = NONE;
    if (strcmp(flag, "fail_on_unexpected_newline") == 0)
    {
        e_flag = FAIL_UNEXPECTED_NEWLINES;
        debug_print_c(CYAN, "found flag 'fail_on_unexpected_newline'\n");
    }
    else if (strcmp(flag, "strict") == 0)
    {
        e_flag = STRICT;
        debug_print_c(CYAN, "found flag 'strict'\n");
    }
    else if (strcmp(flag, "trim_command_output") == 0)
    {
        e_flag = TRIM_COMMAND_OUTPUT;
        debug_print_c(CYAN, "found flag 'trim_command_output'\n");
    }
    else if (strcmp(flag, "verbose") == 0)
    {
        e_flag = VERBOSE;
        verbose_print_c(CYAN, "found flag 'verbose'\n");
    }
    else if (strcmp(flag, "color_off") == 0)
    {
        e_flag = COLOR_OFF;
        debug_print_c(CYAN, "found flag 'color_off'\n");
    }
    else if (strcmp(flag, "debug") == 0)
    {
        e_flag = DEBUG;
    }
    if (e_flag == NONE)
    {
        debug_print_c(RED, "found no valid flag for vvalue '%s'\n", flag);
    }
    if (flag_on == 1)
    {
        flags |= e_flag;
        debug_print_c(CYAN, "enabling...\n");
    }
    else if (flag_on == 0)
    {
        flags &= ~(e_flag);
        debug_print_c(CYAN, "disabling...\n");
    }
    else
    {
        if (flags & STRICT)
        {
            return -1;
        }
    }
    return 0;
}

void add_line(Creal *creal, const char *line)
{
    // increase size by 1
    creal->output = realloc(creal->output, (creal->lines + 1) * sizeof(char *));
    if (creal->output == NULL)
    {
        fprintf(stderr, "Failed to allocate memory in output\n");
        exit(1);
    }
    if (flags & TRIM_COMMAND_OUTPUT)
        creal->output[creal->lines] = trim(_strdup(line));
    else
        creal->output[creal->lines] = _strdup(line);
    if (creal->output[creal->lines] == NULL)
    {
        fprintf(stderr, "Failed to allocate memory in lines\n");
        exit(1);
    }
    creal->lines++;
}

void print_flags()
{
    verbose_printf("active flags for runner:\n");
    if (flags & FAIL_UNEXPECTED_NEWLINES)
    {
        verbose_print_c(CYAN, "\tfail_unexpected_newlines\n");
    }
    if (flags & STRICT)
    {
        verbose_print_c(CYAN, "\tstrict\n");
    }
    if (flags & VERBOSE)
    {
        verbose_print_c(CYAN, "\tverbose\n");
    }
    if (flags & TRIM_COMMAND_OUTPUT)
    {
        verbose_print_c(CYAN, "\ttrim_command_output\n");
    }
    if (flags & COLOR_OFF)
    {
        verbose_print_c(CYAN, "\tcolor_off\n");
    }
    verbose_printf("\n");
}

int compare_creals(const Creal *actual, const Creal *expected)
{
    int failed = 0, is_match = 0;
    verbose_print_c(YELLOW, "testing command '%s'\n", expected->command);
    verbose_print_c(YELLOW, "testing runner '%s'\n", expected->name);
    print_flags();
    size_t num_lines = expected->lines;
    if (actual->command != expected->command)
    {
        fprintf(stderr, "unexpected behavior occured\n");
        printf("expected:\n");
        printf("\t%s\n", expected->command);
        printf("actual:\n");
        printf("\t%s\n", actual->command);
        is_match++;
    }
    if (expected->lines != 0)
    {
        if (actual->lines != expected->lines)
        {
            if (flags & STRICT)
            {
                print_c(RED,
                        "The output contains diffrent number of lines than "
                        "what was expected:\n");
                print_c(RED, "Expected %zu lines. Found %zu lines.\n",
                        expected->lines, actual->lines);
                is_match++;
            }
            else
            {
                verbose_print_c(
                    YELLOW, "The output contains diffrent number of lines than "
                            "what was expected:\n");
                verbose_print_c(YELLOW,
                                "Expected %zu lines. Found %zu lines.\n",
                                expected->lines, actual->lines);
            }
            verbose_print_c(YELLOW, "This may result in a pass or failed test, "
                                    "which may depend on the active flags\n");
            num_lines = actual->lines > expected->lines ? actual->lines
                                                        : expected->lines;
        }
        for (size_t i = 0; i < num_lines; i++)
        {
            if (i >= actual->lines)
            {
                if (isnewline_or_space(expected->output[i]) != 0)
                {
                    if (flags & FAIL_UNEXPECTED_NEWLINES)
                    {
                        print_c(RED,
                                "expected has an extra new line or space\n");
                        failed |= 1;
                    }
                }
                else
                {
                    print_c(RED, "%s\n", expected->output[i]);
                    failed |= 1;
                }
                continue;
            }
            if (i >= expected->lines)
            {
                if (isnewline_or_space(actual->output[i]) != 0)
                {
                    if (flags & FAIL_UNEXPECTED_NEWLINES)
                    {
                        print_c(RED, "ouput has an extra new line or space\n");
                        failed |= 1;
                    }
                    else
                    {
                        verbose_print_c(
                            YELLOW,
                            "Found an extra new line or space. Ignoring due to "
                            "'fail_on_unexpected_newline' flag being active");
                    }
                }
                else
                {
                    print_c(RED, "%s\n", actual->output[i]);
                    failed |= 1;
                }
                continue;
            }
            if (strcmp(actual->output[i], expected->output[i]) != 0)
            {
                if (flags & TRIM_COMMAND_OUTPUT)
                {
                    if (strcmp(trim(actual->output[i]),
                               trim(expected->output[i])) != 0)
                    {
                        size_t act_len = strlen(actual->output[i]);
                        size_t exp_len = strlen(expected->output[i]);
                        print_c(RED, "Lines %zu differ\n", i);
                        print_c(RED, "expected:\n");
                        printf("%s\n", expected->output[i]);
                        verbose_printf("size of expected line: %zu\n", exp_len);
                        print_c(RED, "actual:\n");
                        printf("%s\n", actual->output[i]);
                        verbose_printf("size of actual line: %zu\n", act_len);
                        failed |= 1;
                    }
                    else
                    {
                        verbose_print_c(
                            YELLOW,
                            "Found an extra new line or space. Ignoring due to "
                            "'fail_on_unexpected_newline' flag being active");
                    }
                }
            }
            else
            {
                verbose_printf("%s\n", actual->output[i]);
            }
        }
    }
    if (actual->returncode != expected->returncode)
    {
        fprintf(stderr, "return codes are diffrent\n");
        printf("expected:\n");
        printf("\t%d\n", expected->returncode);
        printf("actual:\n");
        printf("\t%d\n", actual->returncode);
        is_match++;
        failed |= 1;
    }
    if (failed == 1)
    {
        print_c(RED, "assertions failed\n");
    }
    else
    {
        print_c(GREEN, "assertions passed\n");
    }
    return failed;
}

int execute_runner(Creal *runner, char ***failures, size_t *fail_count)
{
    Creal *actual = init_creal();
    actual->command = runner->command;
    execute_command(actual);
    int res = compare_creals(actual, runner);
    destory_creal(actual, 1);
    if (res)
    {
        add_to_failure(failures, runner, fail_count);
    }
    return res;
}

void add_to_failure(char ***failures, Creal *runner, size_t *fail_count)
{
    *failures = realloc(*failures, (*fail_count + 1) * sizeof(char *));
    if (*failures == NULL)
    {
        print_c(RED, "failed to allocate memory for failues\n");
        exit(EXIT_FAILURE);
    }
    *failures[*fail_count] = malloc(strlen(runner->name) + 1);
    strcpy(*failures[*fail_count], runner->name);
    *fail_count++;
}

int validate_runner(const Creal *creal)
{
    int ok = 0;
    ok |= creal->command != NULL;
    if (flags & STRICT)
        ok |= creal->output != NULL;
    return ok;
}

/// read line by line
/// Priority List:
/// 1. multiline stdout: Everything is stored into array. No other process is
/// ran until the multiline stdout is done. Delimited with 'stdout: |". To end,
/// add '|' to a line with nothing else
/// 2. Flags: Delimited with '#'s
/// 3. Runner Seperators: seperated with '--'
/// 3. Actions
/// TODO: Dont store as an array
void read_testfile(const char *input_file, size_t *count)
{
    // get flags
    char buffer[1024];
    FILE *file;
    Creal *input = init_creal();
#ifdef _WIN32
    fopen_s(&file, input_file, "r");
#else
    file = fopen(input_file, "r");
#endif
    if (file == NULL)
    {
        fprintf(stderr, "failed to read input file %s\n", input_file);
        exit(EXIT_FAILURE);
    }
    int is_stdout = 0;
    size_t runner_count = 0;
    size_t fail_count = 0;
    char **failures = NULL;
    printf("---\n");
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        char *copy = strdup(buffer);
        char *trimmed_buf = trim(buffer);
        char first_char = first_non_empty_char(trimmed_buf);
        size_t action_idx = index_of_char(buffer, ':');
        // if still inside stdout: |
        //                         |
        // block
        if (is_stdout)
        {
            if (strcmp(trimmed_buf, "|") == 0)
            {
                is_stdout = 0;
                continue;
            }
            add_line(input, buffer);
            continue;
        }
        // Indicates new runner
        else if (strcmp(trimmed_buf, "---") == 0)
        {
            if (input->name == NULL)
            {
                input->name = malloc(sizeof(char) * num_digits(runner_count));
                itoa(runner_count, input->name, 10);
            }
            int ok = validate_runner(input);
            if (!ok)
            {
                print_creal(input);
                if (flags & STRICT)
                {
                    print_c(RED, "Bad runner. Make sure to fill all fields or "
                                 "disable strict mode\n");
                    exit(EXIT_FAILURE);
                }
                // TODO: Print availabe information of runner
                print_c(RED, "Bad runner found, skipping\n");
                continue;
            }
            runner_count++;
            Creal *actual = init_creal();
            actual->command = input->command;
            execute_command(actual);
            int res = compare_creals(actual, input);
            if (res)
            {
                printf("runner '%s' was a failure", input->name);
            }
            destory_creal(input, 0);
            destory_creal(actual, 1);
            input = init_creal();
            printf("---\n");
            continue;
        }
        // Indicates flag
        else if (buffer[first_char] == '#')
        {
            char *unparsed = str_tolower(&trimmed_buf[first_char + 1]);
            int res = parse_flag(unparsed);
            if (res == -1)
            {
                printf("did not find value %s\n", unparsed);
            }
            continue;
        }
        // indicates action
        else if (action_idx != -1)
        {
            char *action = copy_sub_str(buffer, ':');
            const char *untrimmed = copy_sub_str_offset(buffer, action_idx + 1);
            size_t value_idx = first_non_empty_char(untrimmed);
            const char *trimmed_value =
                trim(copy_sub_str_offset(untrimmed, value_idx));
            if (strcmp(action, "stdout") == 0)
            {
                if (strcmp(trimmed_value, "|") == 0)
                {
                    is_stdout = 1;
                }
                else
                    add_line(input, trimmed_value);
            }
            else if (strcmp(action, "command") == 0)
            {
                input->command = (char *)trimmed_value;
            }
            else if (strcmp(action, "returncode") == 0)
            {
                input->returncode = atoi(trimmed_value);
            }
            else if (strcmp(action, "name") == 0)
            {
                input->name = (char *)trimmed_value;
            }
            else
            {
                if (flags & STRICT)
                {
                    print_c(RED, "invalid action: '%s'. Fatal\n", action);
                    exit(EXIT_FAILURE);
                }
                print_c(YELLOW, "invalid action: '%s'. Continuing...\n",
                        action);
            }
        }
    }
    fclose(file);
    *count = runner_count;

    // User puts --- at the end of file
    if (!validate_runner(input))
    {
        free(input);
    }
    // User removes --- at the end of file
    else
    {
        debug_printf("found unran runner, running\n");
        if (input->name == NULL)
        {
            input->name = malloc(sizeof(char) * num_digits(runner_count));
            itoa(runner_count, input->name, 10);
        }
        runner_count++;
        Creal *actual = init_creal();
        actual->command = input->command;
        execute_command(actual);

        int res = compare_creals(actual, input);
        if (res)
        {
            add_to_failure(&failures, input, &fail_count);
        }

        destory_creal(input, 1);
        destory_creal(actual, 1);
        printf("---\n");

        if (fail_count)
        {
            print_c(RED, "failed runner: \n");
            for (size_t i = 0; i < fail_count; i++)
            {
                print_c(RED, "\t%s\n", failures[i]);
                free(failures[i]);
            }
            free(failures);
        }
    }
}

void append_std_err_redir(char *cmd)
{
    size_t len = strlen(cmd);
    const char *str = copy_sub_str_offset(cmd, len - 4);
    if (strcmp(str, "2>&1") != 0)
        strcat(cmd, " 2>&1");
}

void prepend_shell(char *cmd, const char *prep)
{
    size_t len = strlen(prep);
    memmove(cmd + len, cmd, strlen(cmd) + 1);
    memcpy(cmd, prep, len);
}

void execute_command(Creal *creal)
{
    FILE *fp;
    char buffer[1024];
    size_t line = 0;
    append_std_err_redir(creal->command);
#ifdef _WIN32
    prepend_shell(creal->command, "cmd /c ");
    fp = _popen(creal->command, "r"); // redirect to stderr
#else
    prepend_shell(creal->command, "sh -c ");
    fp = popen(cmd, "r");
#endif
    if (fp == NULL)
    {
        fprintf(stderr, "failed to run command %s\n", creal->command);
        exit(EXIT_FAILURE);
    }

    // read line_wise
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        add_line(creal, buffer);
    }

#ifdef _WIN32
    creal->returncode = _pclose(fp);
#else
    creal->returncode = pclose(file);
#endif
    return;
}

void print_creal(Creal *creal)
{
    if (creal->command)
        printf("command: %s\n", creal->command);
    if (creal->returncode)
        printf("returncode: %d\n", creal->returncode);
    if (creal->lines)
        printf("lines: %zu\n", creal->lines);
    if (creal->name)
        printf("name: %s\n", creal->name);
    for (size_t i = 0; i < creal->lines; i++)
        printf("%s", creal->output[i]);
    printf("\n");
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("USAGE: creal <file>\n");
        exit(EXIT_FAILURE);
    }
    const char *test_file = argv[1];
    size_t runner_count = 0;
    read_testfile(test_file, &runner_count);
    debug_printf("exited creal successfully.\n");
    return 0;
}
