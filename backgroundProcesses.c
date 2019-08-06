/*
 * CLASS: CS 344-400
 * ASSIGNMENT: Program 3
 * FILE NAME: backgroundProcess.c
 * DESCRIPTION: Functions to manage the child processes that
 * run in the background.
 * AUTHOR: Chelsea Egan (eganch@oregonstate.edu)
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "backgroundProcesses.h"
#include "signals.h"

// https://stackoverflow.com/a/3536261
void initProccessesArray() {
    // Flag indicating if process should run in background
    runInBackground = false;

    // Array holding pids of processes running in background
    bckgrdPIDs.pids = (pid_t *)malloc(5 * sizeof(pid_t));

    // No current background processes
    bckgrdPIDs.numPIDs = 0;

    // Start with an array size of 5
    bckgrdPIDs.arraySize = 5;
}

void addToProcesses(pid_t pid) {
    // Resize processes array if full
    if (bckgrdPIDs.arraySize == bckgrdPIDs.numPIDs) {
        bckgrdPIDs.arraySize *= 2;
        bckgrdPIDs.pids = (pid_t *)realloc(bckgrdPIDs.pids, bckgrdPIDs.arraySize * sizeof(pid_t));
    }

    // Update the size and add the pid
    bckgrdPIDs.pids[bckgrdPIDs.numPIDs++] = pid;
}

void removeFromProcesses(pid_t pid) {
    int pidIndex = -5,
            i = 0;

    // Search for matching pid
    while (pidIndex == -5 && i < bckgrdPIDs.numPIDs) {
        if (bckgrdPIDs.pids[i] == pid) {
            pidIndex = i;
        }
    }

    // If pid is found in array, shift other pids up to erase it
    if (pidIndex != -5) {
        for (i = pidIndex; i < bckgrdPIDs.numPIDs; i++) {
            bckgrdPIDs.pids[i] = bckgrdPIDs.pids[i + 1];
        }

        bckgrdPIDs.numPIDs--;
    }
}

bool checkIfBackgroundProcess(pid_t pid) {
    int i;

    // Search through pids array and return true if match is found
    for (i = 0; i < bckgrdPIDs.numPIDs; i++) {
        if (bckgrdPIDs.pids[i] == pid) {
            return true;
        }
    }

    // Not a background process
    return false;
}

// http://www.microhowto.info/howto/reap_zombie_processes_using_a_sigchld_handler.html
void checkForEndedProcesses() {
    int childExitMethod, status;
    pid_t pid = -5;

    // Loop until no more ending processes
    while ((pid = waitpid((pid_t)(-1), &childExitMethod, WNOHANG)) > 0) {
        // If the process that ended was running in the background...
        if (checkIfBackgroundProcess(pid)) {
            // Print status to console
            printf("\nbackground pid %d is done: ", pid);

            // Set exit status/signal
            if (WIFEXITED(childExitMethod)) {
                status = WEXITSTATUS(childExitMethod);
                if (status > 1) status = 1;
                printf("exit value %d\n", status);
            } else if (WIFSIGNALED(childExitMethod)) {
                status = WTERMSIG(childExitMethod);
                printf("terminated by signal %d\n", status);
            } else {
                printf("Failed to get exit status\n");
            }
            fflush(stdout);

            // Update the background pids array
            removeFromProcesses(pid);
        }
    }
}

void checkIfRunInBackground(char** args, int* numArgs) {
    // & at the end of the command indicates request to run in background
    if (strcmp(args[*numArgs - 1], "&") == 0) {
        // If in foreground-only mode, cannot run in background
        runInBackground = tstpStatus == 0 ? true : false;

        // Delete from commands as it is no longer needed
        args[*numArgs - 1] = '\0';
        *numArgs -= 1;
    } else {
        // Not requested to run in background
        runInBackground = false;
    }
}

void handleBackgroundChild(pid_t spawnpid) {
    // Add pid to background processes array
    addToProcesses(spawnpid);
    printf("background pid is %d\n", spawnpid);
    fflush(stdout);
}