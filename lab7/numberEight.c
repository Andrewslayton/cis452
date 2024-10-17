#include <stdio.h>
#include <sys/resource.h>

int main()
{
    struct rlimit limit;
    getrlimit(RLIMIT_FSIZE, &limit);
    printf("Max file size: %ld bytes\n", limit.rlim_cur);
    return 0;
}
