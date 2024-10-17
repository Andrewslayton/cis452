#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define READ 0
#define WRITE 1
int main()
{
    int fd[2];
    int pipeCreationResult;
    int pid;
    pipeCreationResult = pipe(fd);
    char myStringOutput[] = "This a test!";
    char myStringInput[50];
    if (pipeCreationResult < 0)
    {
        perror("Failed pipe creation\n");
        exit(1);
    }
    pid = fork();
    if (pid < 0) // Fork failed
    {
        perror("Fork failed");
        exit(1);
    }
    int output = 3;
    int input;
    if (pid == 0)
    { // Child process
        close(fd[0]);
        write(fd[1], myStringOutput, strlen(myStringOutput) +1 );
        printf("Child wrote [%s]\n", myStringOutput);
    }

    else
    {
        close(fd[1]);
        int bytes = read(fd[0],myStringInput, sizeof(myStringInput));
        printf("Parent received [%s] (%d bytes read) from child process\n", myStringInput, bytes);
    }
    return 0;
}