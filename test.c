#include <stdio.h>
#include <string.h>

int main()
{
    if (strcmp("hello", "hello") != 0)
    {
        printf("strings are not equal");
    }
    else
    {
        printf("strings are equal");
    }
}
