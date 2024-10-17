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
    shmctl(shmId, IPC_RMID, NULL);
    exit(0);
}

int main()
{
    key_t key = ftok(__FILE__, 65);
    if (key == -1)
    {
        perror("Error generating key\n");
        exit(1);
    }

    shmId = shmget(key, SHM_SIZE, 0666 | IPC_CREAT);
    if (shmId < 0)
    {
        perror("Error creating shared memory\n");
        exit(1);
    }

    sharedMemoryPtr = (char *)shmat(shmId, NULL, 0);
    if (sharedMemoryPtr == (void *)-1)
    {
        perror("Error attaching shared memory\n");
        exit(1);
    }

    flag = (int *)sharedMemoryPtr;
    *flag = FLAG_READ;
    signal(SIGINT, cleanup);

    char input[100];
    while (1)
    {
        printf("Enter a string to write to shared memory: ");
        fgets(input, sizeof(input), stdin);

        while (*flag == FLAG_WRITTEN);
        strcpy(sharedMemoryPtr + sizeof(int), input);
        *flag = FLAG_WRITTEN;
    }
    return 0;
}
