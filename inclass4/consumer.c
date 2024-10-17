#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>

struct myStruct
{
    int number;
    char text[40];
};

int main()
{
    key_t key = ftok("producer.c", 65);
    if (key == -1)
    {
        perror("Key generation failed");
        exit(EXIT_FAILURE);
    }

    int sharedMemoryID = shmget(key, sizeof(struct myStruct), S_IWUSR | S_IRUSR | S_IRGRP | S_IWGRP | IPC_CREAT);
    if (sharedMemoryID == -1)
    {
        perror("Shared memory access failed");
        exit(EXIT_FAILURE);
    }

    struct myStruct *sharedMemoryPointer = (struct myStruct *)shmat(sharedMemoryID, NULL, 0);
    if (sharedMemoryPointer == (void *)-1)
    {
        perror("Memory attach failed");
        exit(EXIT_FAILURE);
    }

    printf("Consumer: Integer: %d\n", sharedMemoryPointer->number);
    printf("Consumer: String: %s\n", sharedMemoryPointer->text);

    if (shmdt(sharedMemoryPointer) == -1)
    {
        perror("Memory detach failed");
        exit(EXIT_FAILURE);
    }

    if (shmctl(sharedMemoryID, IPC_RMID, NULL) == -1)
    {
        perror("Memory segment removal failed");
        exit(EXIT_FAILURE);
    }

    printf("Shared memory segment destroyed.\n");

    // Run ipcs to list shared memory segments
    system("ipcs -m");

    return 0;
}
