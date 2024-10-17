#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

void sigHandler(int sigNum);
void childProcess(pid_t parentPID);
volatile sig_atomic_t shutDown = 0;
int main()
{
    pid_t pid;
    struct sigaction sa;

    sa.sa_handler = sigHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    signal(SIGINT, sigHandler);

    pid = fork();

    if (pid < 0){
        perror("Fork failed");
        exit(1);
    }

    if (pid == 0){
        sleep(1);
        childProcess(getppid());
    }
    else
    {
        printf("spawned child PID# %d\n", pid);
        while (!shutDown)
        {
            printf("waiting... ");
            pause();
        }
        printf("^C received. Shutting down (gracefully)\n");
        kill(pid, SIGTERM);
        exit(EXIT_SUCCESS);
    }

    return 0;
}

void sigHandler(int sigNum)
{
    if (sigNum == SIGUSR1){
        printf("received a SIGUSR1 signal\n");
    }
    else if (sigNum == SIGUSR2){
        printf("received a SIGUSR2 signal\n");
    }
    else if (sigNum == SIGINT && !shutDown){
        shutDown = 1;
    }
}

void childProcess(pid_t parentPID)
{
    while (1){
        int randomTime = rand() % 5 + 1;
        sleep(randomTime);
        int randomSignal = (rand() % 2 == 0) ? SIGUSR1 : SIGUSR2;
        kill(parentPID, randomSignal);
    }
}
