#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    printf("process id: %d\n", getpid());
    pid_t childProcessID = fork();
    if (getpid() < 0)
    {
        perror("eror");
        exit(1);
    }
    printf("[%d]The childProcessID value is [%d]\n", getpid(), childProcessID);
    if (childProcessID == 0)
    {
        printf("I have completed running, its a child with process ID:  %d \n", getpid());
    }
    else
    {
        wait(NULL);
        printf("I have completed running, its a parent with process ID: %d \n", getpid());
    }
    return 0;
}