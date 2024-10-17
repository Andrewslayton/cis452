#include <stdio.h>
#include <sys/resource.h>

int main()
{
    struct rlimit limit;
    getrlimit(RLIMIT_NPROC, &limit);
    printf("max processes per user: %ld\n", limit.rlim_cur);
    return 0;
}
