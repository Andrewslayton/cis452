#include <stdio.h>
#include <sys/resource.h>

int main()
{
    struct rlimit limit;
    getrlimit(RLIMIT_NOFILE, &limit);
    printf("Max open files (hard limit): %ld\n", limit.rlim_max);
    return 0;
}
