#include "../../src/creal_strings.c"
#include "../../src/funcs.c"

// Copied from creal.c
void remove_comment(creal_str_t* line)
{
  int start, end;
  if ((start = c_get_substr_index(line, "[[!")) != -1
      && (end = c_get_substr_index(line, "!]]")) != -1) {
    if (start >= end) {
      return;
    }
    /*printf("found comment\n");*/
    copy_till(line, start);
    c_trim(line);
  }
}

struct creal_testcase {
  char* raw;
  char* expected;
};

int main()
{
  struct creal_testcase tests[] = { { "Hello, I am string", "Hello, I am string" },
                                    { "[[! Full comment !]]", "" },
                                    { "Mixed comment, -> [[! Comment !]]", "Mixed comment, ->" },
                                    { NULL, NULL } };
  struct creal_testcase* ct_p = tests;

  size_t failcount = 0;
  while (ct_p->raw != NULL) {
    creal_str_t* raw_t = from_str(ct_p->raw, 1);
    printf("raw_t before removing comment: %s\n", raw_t->str);
    remove_comment(raw_t);
    printf("raw_t: %s\n", raw_t->str);
    if (strcmp(raw_t->str, ct_p->expected) != 0) {
      fprintf(stderr, "Strings are not the same\n");
      printf("expected: %s\n", ct_p->expected);
      printf("actual: %s\n", raw_t->str);
      failcount++;
    }
    free_creal_str_t(raw_t);
    ct_p++;
  }
  /*return failcount == 0;*/
  /*return 0;*/
  return failcount != 0;
}
