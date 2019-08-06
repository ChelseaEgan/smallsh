/*
 * CLASS: CS 344-400
 * ASSIGNMENT: Program 3
 * FILE NAME: foregroundProcesses.h
 * DESCRIPTION: Header file for foregroundProcesses.c
 * AUTHOR: Chelsea Egan (eganch@oregonstate.edu)
 */

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
