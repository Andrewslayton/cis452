#include <stdio.h>
#include <sys/shm.h>

int main()
{
    int max_size = 0;
    int shmid;
    for (int size = 1;; size++)
    {
        shmid = shmget(IPC_PRIVATE, size, IPC_CREAT | 0666);
        if (shmid < 0)
        {
            printf("max shared memory size: %d\n", size - 1);
            break;
        }
        else
        {
            shmctl(shmid, IPC_RMID, NULL);
        }
    }
    return 0;
}
