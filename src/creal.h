#ifndef CREAL_H
#define CREAL_H
#include <stdint.h>
#include <stdlib.h>

#include "creal_file.h"
#include "creal_strings.h"
#include "funcs.h"

typedef struct {
  int allocated;
  char delimter;
  char surround[3];
} creal_template_str_t;

typedef struct {
  size_t fail_count;
  char **failures;
} creal_runner_fail_t;

typedef struct {
  int returncode;
  size_t lines;
  char *command;
  char **output;
  creal_str_t *name;
  file_t *file_h;
} Creal;

/* Templateing */
int replace_template_str(file_t *File, char *variable);

/// To Add a new Flag follow these steps
/// 1. Create a member with a similar name convention as below
/// 2. Assign the new member with a unique value
/// 3. Place the new Action so that the enum is in chronological order
/// 4. In the same order, go to creal.c, then add your member in the same order, and write it's
/// represenation in the test file. The string name does **not** have to be the same as the member
/// name
/// 5. If the flag is not to be used externally, place after FLAG_SIZE, then in creal.c, in the
/// same order, place the member and write it's string as NULL

/// Enum representing the flags available in creal
typedef enum {
  /// Disable all active_flags
  NONE = 0,
  /// Creal will be stricter on the tests, and the creal file
  STRICT = (1 << 1),
  /// Fails if the diffrenece between the defined runner and result differs by a newline(s).
  FAIL_UNEXPECTED_NEWLINES = (1 << 2),
  /// Trims the command output from the left and right, creating a reasonable expectation of the
  /// output. Default, as it trims '\r\n' characters.
  TRIM_COMMAND_OUTPUT = (1 << 3),
  /// Print more information about every runner, failures, or unexpected results.
  VERBOSE = (1 << 4),
  /// Color will no be used in printing to stdout.
  COLOR_OFF = (1 << 5),
  /// Internal use only. Show output if debug flag is set
  DEBUG = (1 << 6),
  /// Unimplemented.
  APPEND_RELATIVE = (1 << 7),
  /// Always shows the output of both the expected output and the actual output.
  ALWAYS_SHOW_OUTPUT = (1 << 8),
  /// Unimplemented.
  SET_COMMENT_STRING = (1 << 9),
  /// Default flag. If set, compares the output between the runner and the result
  COMPARE_OUTPUTS = (1 << 10),
  /// Set variable
  SETVAR = (1 << 9),
  /// Internal use only. Signifies the end of the user flags.
  FLAG_SIZE = (1 << 11),
  /// Signifies that a flag in the creal file is invalid
  INVALID_FLAG = (1 << 12),
} Flags;

#define FOR_ALL_FLAGS(i) for (int i = NONE; 1 << i <= FLAG_SIZE; i++)

/// To Add a new Action follow these steps
/// 1. Create a member with a similar name convention as below
/// 2. In the same order, go to creal.c, then add your member in the same order, and write it's
/// represenation in the test file. The string name does **not** have to be the same as the member
/// 3. If the Action is not to be accessed externally, place after ACTION_SIZE. Then, go to creal.c,
/// and in the same order, plave the member in act_map and write its string as NULL.

/// Enum representing the actions to be used by users in creal file
typedef enum {
  /// Creates a pipe using command as it's input. The output is then recorded into a Creal instance.
  COMMAND,
  /// Returncode of the command created. Passed to Creal.
  RETURNCODE,
  /// Name of the runner to be tested.
  NAME,
  /// Internal use only. Signifies the end of user actions.
  ACTION_SIZE,
  /// Signifies an invalid action in the creal file.
  INVALID_ACTION,
  /// Used when output is a one liner.
  SINGLE_LINE_OUTPUT,
  /// Used when output is multi-line.
  MULTI_LINE_OUTPUT,
} Action;

/* Creal management */
Creal *init_creal();
void free_creal(Creal *creal, int can_destroy_self);

/* User Interface */
void print_c(COLOR c, const char *fmt, ...);
void verbose_printf(const char *fmt, ...);
void verbose_print_c(COLOR c, const char *fmt, ...);
void debug_print_c(COLOR c, const char *fmt, ...);
void debug_printf(const char *fmt, ...);
void print_flags();
void print_creal(Creal *creal);

/* Creal Interperter */
int read_testfile(const char *input_file, size_t *count);
int flag_is_true(const char *value, int fallback);
void add_line(Creal *creal, const creal_str_t *line);
void comapre_creals(const Creal *actual, const Creal *expected);
int execute_runner(Creal *runner, char **failures, size_t fail_count);
int validate_runner(const Creal *creal);
// void remove_comment(char *line);
void execute_command(Creal *ouput);
void add_to_failure(char **failures, Creal *input, size_t fail_count);
char *append_std_err_redir(char *cmd);
char *prepend_shell(char *cmd, const char *prep);
void print_diff(const Creal *expected, const Creal *actual, size_t start_of_diff);

void remove_comment(creal_str_t *line);

#endif
