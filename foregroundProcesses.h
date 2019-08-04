//
// Created by Chelsea on 8/4/2019.
//

#ifndef PROGRAM3_FOREGROUNDPROCESSES_H
#define PROGRAM3_FOREGROUNDPROCESSES_H

#include <sys/types.h>

int runningProcess;
int exitStatus;
int exitSignal;

void initForegroundVars();
void setExitStatus(int);
void printStatus();
void handleForegroundChild(pid_t pid);

#endif //PROGRAM3_FOREGROUNDPROCESSES_H
