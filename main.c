#include "creal.h"
#include "funcs.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_FLAGS (FAIL_UNEXPECTED_NEWLINES | STRICT)

void read_command() {}
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
    return c;
}

void destory_creal(Creal *creal, int can_destroy_self)
{
    if (creal == NULL)
        return;
    for (size_t i = 0; i < creal->lines; i++)
        if (creal->output[i] != NULL)
            free(creal->output[i]);
    free(creal->output);
    if (can_destroy_self)
        free(creal);
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
    creal->output[creal->lines] = _strdup(line);
    if (creal->output[creal->lines] == NULL)
    {
        fprintf(stderr, "Failed to allocate memory in lines\n");
        exit(1);
    }
    creal->lines++;
}

/// read line by line
/// Priority List:
/// 1. multiline stdout: Everything is stored into array. No other process is
/// ran until the multiline stdout is done. Delimited with 'stdout: |". To end,
/// add '|' to a line with nothing else
/// 2. Flags: Delimited with '#'s
/// 3. Runner Seperators: seperated with '--'
/// 3. Actions
Creal *read_testfile(const char *input_file, size_t *count, uint32_t *flags)
{
    // get flags
    char buffer[1024];
    FILE *file;
    Creal *inputs = malloc(sizeof(Creal));
    inputs[0].output = NULL;
    inputs[0].command = NULL;
    inputs[0].returncode = 0;
    inputs[0].lines = 0;
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
    int set_flags = 0;
    size_t runner_count = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        char *copy = strdup(buffer);
        char *trimmed_buf = trim(copy);
        size_t first_char = first_non_empty_char(buffer);
        size_t action_idx = index_of_char(buffer, ':');
        if (is_stdout)
        {
            if (strcmp(trimmed_buf, "|") == 0)
            {
                is_stdout = 0;
                continue;
            }
            add_line(&inputs[runner_count - 1], buffer);
            continue;
        }
        else if (first_char == -1)
        {
            continue;
        }
        else if (strcmp(trimmed_buf, "---") == 0)
        {
            runner_count++;
            if (runner_count == 1)
            {
                continue;
            }
            else
                inputs = realloc(inputs, runner_count * sizeof(Creal));
            if (inputs == NULL)
            {
                fprintf(stderr, "failed to allocate memory\n");
                exit(EXIT_FAILURE);
            }
            if (runner_count - 1 < 0)
            {
                fprintf(
                    stderr,
                    "please use '---' in a new line to create a new runner");
                exit(EXIT_FAILURE);
            }
            inputs[runner_count - 1].output = NULL;
            inputs[runner_count - 1].command = NULL;
            inputs[runner_count - 1].returncode = 0;
            inputs[runner_count - 1].lines = 0;
            continue;
        }
        else if (buffer[first_char] == '#')
        {
            if (runner_count != 0)
            {
                fprintf(stderr, "set flags before creating runner");
            }
            set_flags = 1;
            char *unparsed = trim(&trimmed_buf[first_char + 1]);
            char *flag = copy_sub_str(unparsed, '=');
            size_t value_idx = index_of_char(unparsed, '=');
            char *value = copy_sub_str_offset(unparsed, value_idx + 1);
            if (strcmp(flag, "fail_on_unexpected_newline") == 0)
            {
                if (strcmp(value, "TRUE") == 0)
                {
                    *flags |= FAIL_UNEXPECTED_NEWLINES;
                }
                else if (strcmp(value, "FALSE") == 0)
                {
                    *flags &= ~(FAIL_UNEXPECTED_NEWLINES);
                }
                continue;
            }
            else if (strcmp(flag, "strict") == 0)
            {
                if (strcmp(value, "TRUE") == 0)
                {
                    *flags |= STRICT;
                }
                else if (strcmp(value, "FALSE") == 0)
                {
                    *flags &= ~(STRICT);
                }
                continue;
            }
            continue;
        }
        else if (action_idx != -1)
        {
            if (runner_count - 1 < 0)
            {
                fprintf(
                    stderr,
                    "please use '---' in a new line to create a new runner");
                exit(EXIT_FAILURE);
            }
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
                    continue;
                }
                add_line(&inputs[runner_count - 1], trimmed_value);
            }
            else if (strcmp(action, "command") == 0)
            {
                inputs[runner_count - 1].command = (char *)trimmed_value;
            }
            else if (strcmp(action, "returncode") == 0)
            {
                inputs[runner_count - 1].returncode = atoi(trimmed_value);
            }
        }
    }
    fclose(file);
    *count = (size_t)runner_count;
    return inputs;
}

void append_std_err_redir(char *cmd)
{
    size_t len = strlen(cmd);
    const char *str = copy_sub_str_offset(cmd, len - 4);
    if (strcmp(str, "2>&1") != 0)
        strcat(cmd, " 2>&1");
}

void execute_command(Creal *creal)
{
    FILE *fp;
    char buffer[1024];
    size_t line = 0;
    append_std_err_redir(creal->command);
#ifdef _WIN32
    fp = _popen(creal->command, "rb"); // redirect to stderr
#else
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
    for (size_t i = 0; i < creal->lines; i++)
        printf("%s", creal->output[i]);
    printf("\n");
}

void compare_creals(const Creal *actual, const Creal *expected, uint32_t flags)
{
    if (flags & FAIL_UNEXPECTED_NEWLINES)
    {
        printf("will fail on unexpected newlines\n");
    }
    if (flags & STRICT)
    {
        printf("will be strict\n");
    }
    int is_match = 0;
    printf("expected stdout:\n");
    for (size_t i = 0; i < expected->lines; i++)
        printf("%s", expected->output[i]);
    printf("\n");
    printf("attempting to print actual stdout:\n");
    for (size_t i = 0; i < actual->lines; i++)
        printf("%s", actual->output[i]);
    printf("\n");
    if (actual->command != expected->command)
    {
        fprintf(stderr, "unexpected behavior occured\n");
        printf("expected:\n");
        printf("\t%s\n", expected->command);
        printf("actual:\n");
        printf("\t%s\n", actual->command);
        is_match++;
    }
    if (actual->returncode != expected->returncode)
    {
        fprintf(stderr, "return codes are diffrent\n");
        printf("expected:\n");
        printf("\t%d\n", expected->returncode);
        printf("actual:\n");
        printf("\t%d\n", actual->returncode);
        is_match++;
    }
    if (actual->lines != expected->lines)
    {
        fprintf(stderr,
                "the two have diffrent amount of lines printed to stdout\n");
        fprintf(stderr, "return codes are diffrent\n");
        printf("expected:\n");
        printf("\t%zu\n", expected->lines);
        printf("actual:\n");
        printf("\t%zu\n", actual->lines);
        is_match++;
    }
    if (is_match != 0)
    {
        printf("assertions failed\n");
    }
    else
    {
        printf("assertion pasesd\n");
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("USAGE: creal <file>\n");
    }
    const char *test_file = argv[1];
    size_t runner_count = 0;
    uint32_t flags = DEFAULT_FLAGS;
    Creal *runners = read_testfile(test_file, &runner_count, &flags);
    for (size_t i = 0; i < runner_count; i++)
    {
        Creal *test = &runners[i];
        Creal *actual = init_creal();
        actual->command = test->command;
        execute_command(actual);
        compare_creals(actual, test, flags);
        destory_creal(actual, 1);
    }
    printf("tested all creals\n");
    for (size_t i = 0; i < runner_count; i++)
        destory_creal(&runners[i], 0);
    printf("done...\n");
    return 0;
}
