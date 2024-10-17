#include <stdio.h>
#include <sys/stat.h>

int main()
{
    printf("IRUSR: %o\n", S_IRUSR);
    printf("IWUSR: %o\n", S_IWUSR);
    printf("IXUSR: %o\n", S_IXUSR);
    printf("IRGRP: %o\n", S_IRGRP);
    printf("IWGRP: %o\n", S_IWGRP);
    printf("IXGRP: %o\n", S_IXGRP);
    printf("IROTH: %o\n", S_IROTH);
    printf("IWOTH: %o\n", S_IWOTH);
    printf("IXOTH: %o\n", S_IXOTH);

    return 0;
}