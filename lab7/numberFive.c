#include <stdio.h>
#include <unistd.h>

int main()
{
    long page_size = sysconf(_SC_PAGESIZE);
    printf("page size: %ld bytes\n", page_size);
    return 0;
}
