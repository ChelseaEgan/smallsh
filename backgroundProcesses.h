//
// Created by Chelsea on 8/4/2019.
//

#ifndef PROGRAM3_BACKGROUNDPROCESSES_H
#define PROGRAM3_BACKGROUNDPROCESSES_H

#include <sys/types.h>

struct backgroundProcesses {
    pid_t* pids;
    int arraySize;
    int numPIDs;
};
struct backgroundProcesses bckgrdPIDs;

bool runInBackground;

void initProccessesArray();
void addToProcesses(int);
void removeFromProcesses(int);
bool checkIfBackgroundProcess(int);
void checkForEndedProcesses();
void checkIfRunInBackground(char**, int*);
void handleBackgroundChild(pid_t);

#endif //PROGRAM3_BACKGROUNDPROCESSES_H
