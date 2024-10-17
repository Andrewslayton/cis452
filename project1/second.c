#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024
#define APPLE 1

// Global variables for pipes
int **pipes;
int k;       // Number of processes (including parent)
pid_t *pids; // To store the child process ids

// Function to handle signals and shutdown gracefully
void signal_handler(int sig)
{
    printf("\nShutting down...\n");
    for (int i = 0; i < k; i++)
    {
        close(pipes[i][0]); // Close read ends
        close(pipes[i][1]); // Close write ends
        if (i != 0)
        {
            kill(pids[i], SIGTERM); // Kill child processes
        }
    }
    exit(0);
}

// Function for each node
void node_function(int node_id)
{
    char buffer[BUFFER_SIZE];

    while (1)
    {
        printf("Node %d waiting for message...\n", node_id);
        read(pipes[node_id][0], buffer, BUFFER_SIZE); // Read from pipe
        printf("Node %d received the apple with message: %s\n", node_id, &buffer[2]);

        if (buffer[0] == APPLE)
        { // Has the apple
            if (buffer[1] == node_id)
            {
                printf("Node %d: Message is for me: %s\n", node_id, &buffer[2]);
                buffer[1] = -1; // Mark message as empty
            }
            else
            {
                printf("Node %d: Message is not for me, passing to next node.\n", node_id);
            }

            // Send apple to next node
            printf("Node %d passing apple to node %d.\n", node_id, (node_id + 1) % k);
            write(pipes[(node_id + 1) % k][1], buffer, BUFFER_SIZE);
        }
    }
}

// Parent function to spawn processes and handle message passing
void parent_function()
{
    char message[BUFFER_SIZE];
    int dest_node;

    while (1)
    {
        // Get message and destination from user
        printf("\nEnter message: ");
        fgets(message, BUFFER_SIZE - 2, stdin);
        message[strcspn(message, "\n")] = 0; // Remove newline

        printf("Enter destination node: ");
        scanf("%d", &dest_node);
        getchar(); // Clear input buffer

        // Prepare message: APPLE, destination, message
        char buffer[BUFFER_SIZE];
        buffer[0] = APPLE;
        buffer[1] = dest_node;
        strcpy(&buffer[2], message);

        // Send message to the next node
        printf("Parent (Node 0) sending message to Node 1.\n");
        write(pipes[0][1], buffer, BUFFER_SIZE);

        // Wait for the apple to return to parent
        read(pipes[k - 1][0], buffer, BUFFER_SIZE);
        printf("Parent received the apple back, ready to send the next message.\n");
    }
}

int main()
{
    printf("Enter the number of processes (including parent): ");
    scanf("%d", &k);
    getchar(); // Clear input buffer

    // Allocate memory for pipes and pids
    pipes = malloc(k * sizeof(int *));
    pids = malloc(k * sizeof(pid_t));
    for (int i = 0; i < k; i++)
    {
        pipes[i] = malloc(2 * sizeof(int));
        pipe(pipes[i]); // Create pipe for each process
    }

    // Signal handling for graceful shutdown
    signal(SIGINT, signal_handler);

    // Fork processes
    for (int i = 1; i < k; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            // Child process
            node_function(i);
            exit(0);
        }
        else
        {
            pids[i] = pid;
        }
    }

    // Parent process (Node 0)
    parent_function();

    // Wait for all child processes to finish
    for (int i = 1; i < k; i++)
    {
        wait(NULL);
    }

    // Free memory for pipes and pids
    for (int i = 0; i < k; i++)
    {
        free(pipes[i]);
    }
    free(pipes);
    free(pids);

    return 0;
}
