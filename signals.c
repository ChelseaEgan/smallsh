/*
 * CLASS: CS 344-400
 * ASSIGNMENT: Program 3
 * FILE NAME: signals.c
 * DESCRIPTION: Functions to manage the signals that are
 * caught by the program
 * AUTHOR: Chelsea Egan (eganch@oregonstate.edu)
 */

#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include "signals.h"
#include "foregroundProcesses.h"
#include "backgroundProcesses.h"

void initTstpStatus() {
    // Indicates not in foreground-only mode
    tstpStatus = 0;
}

// http://web.engr.oregonstate.edu/~brewsteb/CS344Slides/3.3%20Signals.pdf
void catchSIGINT(int signo) {
    // If there is a currently running process
    if (runningProcess > 0) {
        // Set the status with the signal that terminated it
        setExitStatus(signo);
        printStatus();
    }
    printf("\n");
    fflush(stdout);
}

void catchSIGTSTP() {
    // If not in foreground-only mode, enter that mode
    if (tstpStatus == 0) {
        printf("\nEntering foreground-only mode (& is now ignored)\n");
        tstpStatus = 1;
    // If in foreground-only mode, exit that mode
    } else {
        printf("\nExiting foreground-only mode\n");
        tstpStatus = 0;
    }
    fflush(stdout);
}

// Create the signal handlers for SIGINT (handled by catchSIGINT)
// and SIGTSTP (handled by catch SIGTSTP)
// http://web.engr.oregonstate.edu/~brewsteb/CS344Slides/3.3%20Signals.pdf
void createSignalHandlers() {
    struct sigaction SIGINT_action = {0};
    struct sigaction SIGTSTP_action = {0};

    SIGINT_action.sa_handler = catchSIGINT;
    SIGTSTP_action.sa_handler = catchSIGTSTP;

    sigfillset(&SIGINT_action.sa_mask);
    sigfillset(&SIGTSTP_action.sa_mask);

    SIGINT_action.sa_flags = 0;
    SIGTSTP_action.sa_flags = 0;

    sigaction(SIGINT, &SIGINT_action, NULL);
    sigaction(SIGTSTP, &SIGTSTP_action, NULL);
}

// Block the SIGTSTP signal until unblocked
void blockTSTP() {
    sigemptyset(&x);
    sigaddset(&x, SIGTSTP);
    sigprocmask(SIG_BLOCK, &x, NULL);
}

void ignoreSignals() {
    // background processes should not receive SIGINT
    if (runInBackground) {
        signal(SIGINT, SIG_IGN);
    }
    // all children processes should not receive SIGTSTP
    signal(SIGTSTP, SIG_IGN);
}

void unblockSignals() {
    sigprocmask(SIG_UNBLOCK, &x, NULL);
}