//
// Created by Chelsea on 8/4/2019.
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "foregroundProcesses.h"

void initForegroundVars() {
    runningProcess = 0;
    exitStatus = -5;
    exitSignal = -5;
}

void setExitStatus(int exitMethod) {
    fflush(stdout);
    if (WIFEXITED(exitMethod)) {
        exitStatus = WEXITSTATUS(exitMethod);
        if (exitStatus > 1) exitStatus = 1;
        exitSignal = -5;
    } else if (WIFSIGNALED(exitMethod) && WTERMSIG(exitMethod) < 32) {
        exitSignal = WTERMSIG(exitMethod);
        exitStatus = -5;
    }
}

void printStatus() {
    if (exitStatus != -5) {
        printf("exit value %d\n", exitStatus);
    } else if (exitSignal != -5) {
        printf("\nterminated by signal %d\n", exitSignal);
    } else {
        printf("exit value 0\n");
    }
    fflush(stdout);
}

void handleForegroundChild(pid_t spawnpid) {
    int childExitMethod = -5;

    runningProcess = spawnpid;
    waitpid(spawnpid, &childExitMethod, 0);
    runningProcess = 0;
    setExitStatus(childExitMethod);
}