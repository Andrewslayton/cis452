#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct
{
    int destNode;
    char content[256];
} Message;

volatile sig_atomic_t terminateFlag = 0;

void handleSigint(int sig)
{
    terminateFlag = 1;
}

void handleSigterm(int sig)
{
    terminateFlag = 1;
}

int main()
{
    int k;
    printf("Enter the number of nodes: ");
    fflush(stdout);
    scanf("%d", &k);

    int pipes[k][2];
    for (int i = 0; i < k; i++)
    {
        if (pipe(pipes[i]) == -1)
        {
            perror("pipe");
            exit(1);
        }
    }

    struct sigaction sa_int;
    sa_int.sa_handler = &handleSigint;
    sa_int.sa_flags = 0;
    sigemptyset(&sa_int.sa_mask);
    if (sigaction(SIGINT, &sa_int, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    int syncPipe[2];
    if (pipe(syncPipe) == -1)
    {
        perror("pipe");
        exit(1);
    }

    pid_t childPids[k - 1];
    for (int i = 1; i < k; i++)
    {
        pid_t pid = fork();
        if (pid == -1)
        {
            perror("fork");
            exit(1);
        }
        if (pid == 0)
        {
            int nodeId = i;
            close(syncPipe[0]);

            struct sigaction sa_term;
            sa_term.sa_handler = &handleSigterm;
            sa_term.sa_flags = 0;
            sigemptyset(&sa_term.sa_mask);
            if (sigaction(SIGTERM, &sa_term, NULL) == -1)
            {
                perror("sigaction");
                exit(1);
            }

            for (int j = 0; j < k; j++)
            {
                if (j != (nodeId - 1 + k) % k)
                {
                    close(pipes[j][0]);
                }
                if (j != nodeId)
                {
                    close(pipes[j][1]);
                }
            }

            printf("Node %d: Process created (PID %d)\n", nodeId, getpid());
            fflush(stdout);

            write(syncPipe[1], "x", 1);

            while (!terminateFlag)
            {
                Message msg;
                ssize_t bytesRead = read(pipes[(nodeId - 1 + k) % k][0], &msg, sizeof(Message));
                if (bytesRead == -1)
                {
                    if (errno == EINTR)
                    {
                        continue;
                    }
                    else
                    {
                        perror("read");
                        break;
                    }
                }
                else if (bytesRead == 0)
                {
                    break;
                }
                else if (bytesRead == sizeof(Message))
                {
                    if (msg.destNode == -1)
                    {
                        printf("Node %d: Received and passing the apple \n", nodeId);
                        fflush(stdout);
                    }
                    else
                    {
                        printf("Node %d: Received message destined for Node %d\n", nodeId, msg.destNode);
                        fflush(stdout);
                    }
                    if (msg.destNode == nodeId)
                    {
                        printf("Node %d: Message received: %s\n", nodeId, msg.content);
                        fflush(stdout);
                        msg.destNode = -1;
                    }
                    write(pipes[nodeId][1], &msg, sizeof(Message));
                }
            }

            printf("Node %d: Terminating\n", nodeId);
            fflush(stdout);
            exit(0);
        }
        else
        {
            childPids[i - 1] = pid;
        }
    }

    int nodeId = 0;
    close(syncPipe[1]);
    for (int i = 0; i < k - 1; i++)
    {
        char buf;
        read(syncPipe[0], &buf, 1);
    }
    close(syncPipe[0]);

    for (int j = 0; j < k; j++)
    {
        if (j != (nodeId - 1 + k) % k)
        {
            close(pipes[j][0]);
        }
        if (j != nodeId)
        {
            close(pipes[j][1]);
        }
    }

    printf("Node %d: Process created (PID %d)\n", nodeId, getpid());
    fflush(stdout);

    Message msg;
    msg.destNode = -1;
    write(pipes[nodeId][1], &msg, sizeof(Message));

    while (!terminateFlag)
    {
        ssize_t bytesRead = read(pipes[(nodeId - 1 + k) % k][0], &msg, sizeof(Message));
        if (bytesRead == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                perror("read");
                break;
            }
        }
        else if (bytesRead == 0)
        {
            break;
        }
        else if (bytesRead == sizeof(Message))
        {
            if (msg.destNode == -1)
            {
                printf("Node %d: Received apple\n", nodeId);
                fflush(stdout);
                printf("Enter the destination node (0 - %d): ", k - 1);
                fflush(stdout);
                int scanfResult = scanf("%d", &msg.destNode);
                if (scanfResult == EOF || terminateFlag)
                {
                    break;
                }
                printf("Enter message: ");
                fflush(stdout);
                getchar();
                if (fgets(msg.content, sizeof(msg.content), stdin) == NULL)
                {
                    break;
                }
                msg.content[strcspn(msg.content, "\n")] = '\0';
                printf("Node %d: Sending message to Node %d\n", nodeId, msg.destNode);
                fflush(stdout);
            }
            else
            {
                printf("Node %d: Received message destined for Node %d\n", nodeId, msg.destNode);
                fflush(stdout);
                if (msg.destNode == nodeId)
                {
                    printf("Node %d: Message received: %s\n", nodeId, msg.content);
                    fflush(stdout);
                    msg.destNode = -1;
                }
            }
            write(pipes[nodeId][1], &msg, sizeof(Message));
        }
    }
    printf("Node %d: Terminating child processes\n", nodeId);
    fflush(stdout);
    for (int i = 0; i < k - 1; i++)
    {
        kill(childPids[i], SIGTERM);
    }

    for (int i = 0; i < k - 1; i++)
    {
        waitpid(childPids[i], NULL, 0);
    }

    printf("Node %d: Terminating\n", nodeId);
    fflush(stdout);
    return 0;
}
