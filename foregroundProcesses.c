/*
 * CLASS: CS 344-400
 * ASSIGNMENT: Program 3
 * FILE NAME: foregroundProcesses.c
 * DESCRIPTION: Functions to manage children processes that run
 * in the foreground.
 * AUTHOR: Chelsea Egan (eganch@oregonstate.edu)
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "foregroundProcesses.h"

void initForegroundVars() {
    // Indicates there is no currently running foreground process
    runningProcess = 0;
    // Indicates an exit status has not been set
    exitStatus = -5;
    // Indicates an exit signal has not been set
    exitSignal = -5;
}

void setExitStatus(int exitMethod) {
    // Set the exit status if process exited normally
    if (WIFEXITED(exitMethod)) {
        exitStatus = WEXITSTATUS(exitMethod);
        if (exitStatus > 1) exitStatus = 1;
        exitSignal = -5;
    // Set the exit signal if process exited due to a signal
    } else if (WIFSIGNALED(exitMethod) && WTERMSIG(exitMethod) < 32) {
        exitSignal = WTERMSIG(exitMethod);
        exitStatus = -5;
    }
}

void printStatus() {
    // Print either the exit status or signal if set
    if (exitStatus != -5) {
        printf("exit value %d\n", exitStatus);
    } else if (exitSignal != -5) {
        printf("\nterminated by signal %d\n", exitSignal);
    } else {
        // Neither was set, default to 0
        printf("exit value 0\n");
    }
    fflush(stdout);
}

void handleForegroundChild(pid_t spawnpid) {
    int childExitMethod = -5;

    // Set the flag indicating there is a running process
    runningProcess = spawnpid;

    // Wait until the process ends and set the exit status
    waitpid(spawnpid, &childExitMethod, 0);

    // Set flag indicating no running process
    runningProcess = 0;

    // Update status/signal
    setExitStatus(childExitMethod);
}