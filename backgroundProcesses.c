//
// Created by Chelsea on 8/4/2019.
//

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
    runInBackground = false;
    bckgrdPIDs.pids = (pid_t *)malloc(5 * sizeof(pid_t));
    bckgrdPIDs.numPIDs = 0;
    bckgrdPIDs.arraySize = 5;
}

void addToProcesses(int pid) {
    // Resize processes array if full
    if (bckgrdPIDs.arraySize == bckgrdPIDs.numPIDs) {
        bckgrdPIDs.arraySize *= 2;
        bckgrdPIDs.pids = (pid_t *)realloc(bckgrdPIDs.pids, bckgrdPIDs.arraySize * sizeof(pid_t));
    }

    // Update the size and add the pid
    bckgrdPIDs.pids[bckgrdPIDs.numPIDs++] = pid;
}

void removeFromProcesses(int pid) {
    int pidIndex = -5,
            i = 0;

    while (pidIndex == -5 && i < bckgrdPIDs.numPIDs) {
        if (bckgrdPIDs.pids[i] == pid) {
            pidIndex = i;
        }
    }

    if (pidIndex != -5) {
        for (i = pidIndex; i < bckgrdPIDs.numPIDs; i++) {
            bckgrdPIDs.pids[i] = bckgrdPIDs.pids[i + 1];
        }

        bckgrdPIDs.numPIDs--;
    }
}

bool checkIfBackgroundProcess(int pid) {
    int i;
    for (i = 0; i < bckgrdPIDs.numPIDs; i++) {
        if (bckgrdPIDs.pids[i] == pid) {
            return true;
        }
    }
    return false;
}

// http://www.microhowto.info/howto/reap_zombie_processes_using_a_sigchld_handler.html
void checkForEndedProcesses() {
    int childExitMethod, status;
    pid_t pid = -5;

    while ((pid = waitpid((pid_t)(-1), &childExitMethod, WNOHANG)) > 0) {
        if (checkIfBackgroundProcess(pid)) {
            printf("\nbackground pid %d is done: ", pid);
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
            removeFromProcesses(pid);
        }
    }
}

void checkIfRunInBackground(char** args, int* numArgs) {
    if (strcmp(args[*numArgs - 1], "&") == 0) {
        runInBackground = tstpStatus == 0 ? true : false;
        args[*numArgs - 1] = '\0';
        *numArgs -= 1;
    } else {
        runInBackground = false;
    }
}

void handleBackgroundChild(pid_t spawnpid) {
    addToProcesses(spawnpid);
    printf("background pid is %d\n", spawnpid);
    fflush(stdout);
}