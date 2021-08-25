/* strtok example */
#include <stdio.h>
#include <string.h>

int main()
{
    const char *current_args[5];
    char str[] = "This a sample string";
    char* rest = str;
    int i = 0;
    printf("Splitting string \"%s\" into tokens:\n", str);

    while ((current_args[i] = strtok_r(rest," ",&rest)))
    {
        printf("#%d: %s\n", i,current_args[i++]);
    }
    
    return 0;
}
