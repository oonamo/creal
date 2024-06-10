#include "creal.h"
#include "funcs.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    /* printf("output? %s\n", creal->output[0]); */
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
Creal *read_testfile(Creal *input, const char *input_file, size_t *count)
{
    // get flags
    char buffer[1024];
    FILE *file;
    Creal *inputs;
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
        printf("next\n");
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
            add_line(input, buffer);
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
                inputs = malloc(sizeof(Creal));
            }
            else
                inputs = realloc(inputs, runner_count * sizeof(Creal));
            if (inputs == NULL)
            {
                fprintf(stderr, "failed to allocate memory\n");
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
            printf("is flag\n");
            set_flags = 1;
            char *flag = trim(&buffer[first_char + 1]);
            printf("flag: %s\n", flag);
            size_t val_idx = index_of_char(flag, '=');
            char *value;
            value = strchr(flag, '=');
            if (value == NULL)
            {
                printf("use default value\n");
                continue;
            }
            value = &value[1];
            printf("value: %s\n", value);
            continue;
        }
        else if (action_idx != -1)
        {
            printf("is action\n");
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
                add_line(input, trimmed_value);
                add_line(&inputs[runner_count - 1], trimmed_value);
            }
            else if (strcmp(action, "command") == 0)
            {
                input->command = (char *)trimmed_value;
                inputs[runner_count - 1].command = (char *)trimmed_value;
            }
            else if (strcmp(action, "returncode") == 0)
            {
                input->returncode = atoi(action);
                input[runner_count - 1].returncode = atoi(action);
            }
        }
    }
    fclose(file);
    *count = (size_t)runner_count;
    return inputs;
}

void execute_command(Creal *creal, char *cmd)
{
    FILE *file;
    char buffer[1024];
    size_t line = 0;
#ifdef _WIN32
    file = _popen(cmd, "r");
#else
    fp = popen(cmd, "r");
#endif
    if (file == NULL)
    {
        fprintf(stderr, "failed to run command %s\n", cmd);
        exit(EXIT_FAILURE);
    }

    // read line_wise
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        add_line(creal, buffer);
    }

#ifdef _WIN32
    creal->returncode = _pclose(file);
#else
    creal->returncode = pclose(fp);
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

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("USAGE: creal <file>\n");
    }
    const char *test_file = argv[1];
    Creal *expected = init_creal();
    Creal *actual = init_creal();
    size_t runner_count = 0;
    Creal *runners = read_testfile(expected, test_file, &runner_count);
    printf("finished reading file...\n");
    printf("runner_count: %zu\n", runner_count);
    for (size_t i = 0; i < runner_count; i++)
        print_creal(&runners[i]);
    printf("printed all creals\n");
    for (size_t i = 0; i < runner_count; i++)
        destory_creal(&runners[i], 0);
    print_creal(expected);
    destory_creal(expected, 1);
    destory_creal(actual, 1);
    printf("done...\n");
    return 0;
}
