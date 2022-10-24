#include <stdio.h>
#include <stdlib.h>

int
argparse(int argc, char **argv, int *start_p, int *end_p, int *step_p)
{
    if (argc == 1) {
        printf("%s",
            "usage: range [start] [end] [step]\n"
            "\tstart: range start\n"
            "\tend: range end\n"
            "\tstep: range step\n"
        );
        return -1;
    } else if (argc == 2) {
        *end_p = atoi(argv[1]);
    } else if (argc == 3 || argc == 4) {
        *start_p = atoi(argv[1]);
        *end_p = atoi(argv[2]);
        if (argc == 4) {
            *step_p = atoi(argv[3]);
        }
        if (*step_p == 0 || (*start_p < *end_p && *step_p < 0) || (*start_p > *end_p && *step_p > 0)) {
            fprintf(stderr, "Bad parameters!\n");
            return -1;
        }
    }
    return 0;
}

int
main(int argc, char *argv[])
{
    int start = 0;
    int end = 1;
    int step = 1;
    if (argparse(argc, argv, &start, &end, &step) != 0) {
        return -1;
    }

    for (int i = start; (start < end && i < end) || (start > end && i > end); i += step)
        printf("%d\n", i);
    return 0;
}
