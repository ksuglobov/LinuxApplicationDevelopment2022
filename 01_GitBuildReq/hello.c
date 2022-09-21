#include <stdio.h>
#include <stdlib.h>

int
main(void)
{
    size_t str_len = 3;
    char *buffer = (char *) malloc((str_len + 1) * sizeof(*buffer));

    size_t i = 0;
    char c = 0;

    scanf("%c", &c);
    while (c > 0 && c != '\n') {
        if (i == str_len) {
            str_len *= 2;
            buffer = (char *) realloc(buffer, (str_len + 1) * sizeof(*buffer));
        }
        buffer[i++] = c; 
        scanf("%c", &c);
    }
    buffer[i] = 0;

//    printf("[%s], %ld \n", buffer, str_len);
    printf("Hello, %s!\n", buffer);

    return 0;
}

