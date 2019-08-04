//
// Created by Chelsea on 8/4/2019.
//

#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include "signals.h"
#include "foregroundProcesses.h"

void initTstpStatus() {
    tstpStatus = 0;
}

// http://web.engr.oregonstate.edu/~brewsteb/CS344Slides/3.3%20Signals.pdf
void catchSIGINT(int signo) {
    if (runningProcess > 0) {
        setExitStatus(signo);
        printStatus();
    }
    printf("\n");
    fflush(stdout);
}

void catchSIGTSTP() {
    if (tstpStatus == 0) {
        printf("\nEntering foreground-only mode (& is now ignored)\n");
        tstpStatus = 1;
    } else {
        printf("\nExiting foreground-only mode\n");
        tstpStatus = 0;
    }
}

void createSignalHandlers() {
    // http://web.engr.oregonstate.edu/~brewsteb/CS344Slides/3.3%20Signals.pdf
    struct sigaction SIGINT_action = {0};
    SIGINT_action.sa_handler = catchSIGINT;
    sigfillset(&SIGINT_action.sa_mask);
    SIGINT_action.sa_flags = 0;
    sigaction(SIGINT, &SIGINT_action, NULL);

    struct sigaction SIGTSTP_action = {0};
    SIGTSTP_action.sa_handler = catchSIGTSTP;
    sigfillset(&SIGTSTP_action.sa_mask);
    SIGTSTP_action.sa_flags = 0;
    sigaction(SIGTSTP, &SIGTSTP_action, NULL);
}