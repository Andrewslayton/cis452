#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define FOO 4096

int main()
{
    int shmId;
    char *sharedMemoryPtr;

    // Create the shared memory segment
    if ((shmId = shmget(IPC_PRIVATE, FOO, IPC_CREAT | S_IRUSR | S_IWUSR)) < 0)
    {
        perror("Unable to get shared memory\n");
        exit(1);
    }

    // Attach the shared memory segment to this process's address space
    if ((sharedMemoryPtr = shmat(shmId, 0, 0)) == (void *)-1)
    {
        perror("Unable to attach\n");
        exit(1);
    }

    // Print the shared memory ID
    printf("Shared memory ID: %d\n", shmId);

    // Pause the program to inspect the shared memory segment with ipcs
    pause(); // Manually terminate the program later with ^C (Control-C)

    // Detach the shared memory segment from this process
    if (shmdt(sharedMemoryPtr) < 0)
    {
        perror("Unable to detach\n");
        exit(1);
    }

    // Deallocate the shared memory segment
    if (shmctl(shmId, IPC_RMID, 0) < 0)
    {
        perror("Unable to deallocate\n");
        exit(1);
    }

    return 0;
}
