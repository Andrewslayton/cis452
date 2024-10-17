#include <stdio.h>
#include <semaphore.h>
#include <errno.h>

int main()
{
    sem_t sem;
    sem_init(&sem, 0, 0);
    int max_value = 0;

    while (sem_post(&sem) == 0)
    {
        max_value++;
    }

    printf("max semaphore value: %d\n", max_value);
    sem_destroy(&sem);
    return 0;
}
