/*
 * CLASS: CS 344-400
 * ASSIGNMENT: Program 3
 * FILE NAME: backgroundProcesses.h
 * DESCRIPTION: Header file for backgroundProcesses.c
 * AUTHOR: Chelsea Egan (eganch@oregonstate.edu)
 */

#ifndef PROGRAM3_BACKGROUNDPROCESSES_H
#define PROGRAM3_BACKGROUNDPROCESSES_H

#include <stdbool.h>
#include <sys/types.h>

struct backgroundProcesses {
    pid_t* pids;
    int arraySize;
    int numPIDs;
};
struct backgroundProcesses bckgrdPIDs;

bool runInBackground;

void initProccessesArray();
void addToProcesses(pid_t);
void removeFromProcesses(pid_t);
bool checkIfBackgroundProcess(pid_t);
void checkForEndedProcesses();
void checkIfRunInBackground(char**, int*);
void handleBackgroundChild(pid_t);

#endif //PROGRAM3_BACKGROUNDPROCESSES_H
