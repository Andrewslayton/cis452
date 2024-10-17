#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

int fileReq = 0;
pthread_mutex_t reqMutex;
void cleanup(int sig);

void *workerThread(void *filename)
{
    char *file = (char *)filename;
    srand(time(NULL) + pthread_self());
    int randomVal = rand() % 100; 
    int sleepTime;
    if (randomVal < 80)
    {
        sleepTime = 1;
    }
    else
    {
        sleepTime = 7 + rand() % 4;
    }
    sleep(sleepTime);
    printf("file accessed: %s, simulated time: %d seconds\n", file, sleepTime);
    free(file);
    return NULL;
}

int main()
{
    pthread_t dispatcher;
    char filename[512];
    int threadStatus;
    pthread_mutex_init(&reqMutex, NULL);
    struct sigaction sa;
    sa.sa_handler = cleanup;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    while (1)
    {
        printf("enter name of file: ");
        if (fgets(filename, 512, stdin) != NULL)
        {
            filename[strcspn(filename, "\n")] = '\0';
            char *fileCopy = strdup(filename);
            threadStatus = pthread_create(&dispatcher, NULL, workerThread, fileCopy);
            if (threadStatus != 0)
            {
                fprintf(stderr, "thread creation error %d: %s\n", threadStatus, strerror(threadStatus));
                free(fileCopy);
                continue;
            }
            pthread_detach(dispatcher);
            pthread_mutex_lock(&reqMutex);
            fileReq++;
            pthread_mutex_unlock(&reqMutex);
        }
    }
    return 0;
}

void cleanup(int sig)
{
    pthread_mutex_lock(&reqMutex);
    printf("\nfiles requested : %d\n", fileReq);
    pthread_mutex_unlock(&reqMutex);
    pthread_mutex_destroy(&reqMutex);
    exit(0);
}
