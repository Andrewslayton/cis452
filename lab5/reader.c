#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>

#define SHM_SIZE 1024
#define FLAG_WRITTEN 1
#define FLAG_READ 0
int shmId;
char *sharedMemoryPtr;
int *flag;

void cleanup(int sig)
{
    shmdt(sharedMemoryPtr);
    exit(0);
}

int main()
{
    key_t key = ftok("writer.c", 65);
    if (key == -1)
    {
        perror("Error generating key\n");
        exit(1);
    }

    shmId = shmget(key, SHM_SIZE, 0666);
    if (shmId < 0)
    {
        perror("Error accessing shared memory\n");
        exit(1);
    }

    sharedMemoryPtr = (char *)shmat(shmId, NULL, 0);
    if (sharedMemoryPtr == (void *)-1)
    {
        perror("Error attaching shared memory\n");
        exit(1);
    }

    flag = (int *)sharedMemoryPtr;
    signal(SIGINT, cleanup);
    char buffer[100];

    while (1)
    {
        while (*flag == FLAG_READ)
            ;

        strcpy(buffer, sharedMemoryPtr + sizeof(int));
        printf("Reader: %s", buffer);
        *flag = FLAG_READ;
    }
    return 0;
}
