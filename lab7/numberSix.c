#include <stdio.h>
#include <unistd.h>

int main()
{
    long phys_pages = sysconf(_SC_PHYS_PAGES);
    printf("Physical pages: %ld\n", phys_pages);
    return 0;
}
