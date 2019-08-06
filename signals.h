/*
 * CLASS: CS 344-400
 * ASSIGNMENT: Program 3
 * FILE NAME: signals.h
 * DESCRIPTION: Header file for signals.c
 * AUTHOR: Chelsea Egan (eganch@oregonstate.edu)
 */

#ifndef PROGRAM3_SIGNALS_H
#define PROGRAM3_SIGNALS_H

#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>

int tstpStatus;
sigset_t x;

void initTstpStatus();
void catchSIGINT(int);
void catchSIGTSTP();
void createSignalHandlers();
void blockTSTP();
void ignoreSignals();
void unblockSignals();

#endif //PROGRAM3_SIGNALS_H
