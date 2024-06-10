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

void destory_creal(Creal *creal)
{
    if (creal == NULL)
        return;
    for (size_t i = 0; i < creal->lines; i++)
        free(creal->output[i]);
    free(creal->output);
    free(creal);
}

void add_line(Creal *creal, const char *line)
{
    // increase size by 1
    creal->output = realloc(creal->output, (creal->lines + 1) * sizeof(char *));
    if (creal->output == NULL)
    {
        fprintf(stderr, "Failed to allocate memory\n");
        exit(1);
    }
    creal->output[creal->lines] = _strdup(line);
    if (creal->output[creal->lines] == NULL)
    {
        fprintf(stderr, "Failed to allocate memory\n");
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
/// 3. Actions
void read_testfile(Creal *input, const char *input_file)
{
    // get flags
    char buffer[1024];
    FILE *file;
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
        if (is_stdout)
        {
            char *copy = strdup(buffer);
            const char *trimmed_value = trim(copy);
            if (strcmp(trimmed_value, "|") == 0)
            {
                is_stdout = 0;
                continue;
            }
            else if (strcmp(trimmed_value, "--") == 0)
            {
                is_stdout = 0;
                continue;
            }
            add_line(input, buffer);
            continue;
        }
        size_t first_char = first_non_empty_char(buffer);
        if (first_char == -1)
        {
            continue;
        }
        if (buffer[first_char] == '#')
        {
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
        size_t action_idx = index_of_char(buffer, ':');
        if (action_idx != -1)
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
                    continue;
                }
                add_line(input, trimmed_value);
            }
            else if (strcmp(action, "command") == 0)
            {
                input->command = (char *)trimmed_value;
            }
        }
    }
    fclose(file);
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
    printf("command: %s\n", creal->command);
    printf("returncode: %d\n", creal->returncode);
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
    read_testfile(expected, test_file);
    printf("finished reading file...\n");
    /* execute_command(actual, argv[1]); */
    /* print_creal(actual); */
    print_creal(expected);
    destory_creal(expected);
    destory_creal(actual);
    printf("done...\n");
    return 0;
}
