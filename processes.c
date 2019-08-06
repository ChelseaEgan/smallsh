/*
 * CLASS: CS 344-400
 * ASSIGNMENT: Program 3
 * FILE NAME: processes.c
 * DESCRIPTION: Functions to create and terminate child processes
 * AUTHOR: Chelsea Egan (eganch@oregonstate.edu)
 */

#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "backgroundProcesses.h"
#include "builtInCommands.h"
#include "foregroundProcesses.h"
#include "processes.h"
#include "signals.h"

// https://stackoverflow.com/questions/25261/set-and-oldset-in-sigprocmask
void createChild(char** args, int numArgs) {
    pid_t spawnpid = -5;
    int result = -5;

    // Block the TSTP signal as it should not be handled by children
    blockTSTP();

    // Create new process
    spawnpid = fork();
    switch (spawnpid) {
        case -1:
            // Fork failed
            perror("THEY TOOK MY SQUEEZIN' ARM!\n");
            exit(1);
            break;
        case 0:
            // New child process
            // Should not react to SIGINT if background
            // Should not react to SIGTSTP
            ignoreSignals();

            // Check if i/o should be redirected
            result = handleRedirect(args, numArgs);
            if (result != 0) {
                exit(1);
                break;
            }

            // Execute command
            execvp(args[0], args);

            // Execution failed
            printf("%s: no such file or directory\n", args[0]);
            fflush(stdout);
            exit(1);
            break;
        default:
            // Parent
            // Call functions to handle waiting on the child process
            if (runInBackground) {
                handleBackgroundChild(spawnpid);
            } else {
                handleForegroundChild(spawnpid);
            }

            // Let signals through to the parent again
            unblockSignals();
            break;
    }
}

void cleanUp() {
    // If there is a foreground process - kill it
    if (runningProcess > 0) {
        kill(runningProcess, SIGKILL);
    }

    int i;
    // Kill any background processes
    for (i = 0; i < bckgrdPIDs.numPIDs; i++) {
        kill(bckgrdPIDs.pids[i], SIGKILL);
    }

    // Clean up memory for pids array
    free(bckgrdPIDs.pids);
}

// https://www.geeksforgeeks.org/atexit-function-in-c-c/
void registerExitFunction() {
    // On program termination, terminate any running processes
    if (atexit(cleanUp) != 0) {
        printf("atexit registration failed. Exiting.\n");
        fflush(stdout);
        exit(1);
    }
}