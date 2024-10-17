#include <stdio.h>
#include <sys/resource.h>

int main()
{
    struct rlimit limit;
    getrlimit(RLIMIT_NOFILE, &limit);
    printf("Max open files (soft limit): %ld\n", limit.rlim_cur);
    return 0;
}
