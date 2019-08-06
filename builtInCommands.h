/*
 * CLASS: CS 344-400
 * ASSIGNMENT: Program 3
 * FILE NAME: builtInCommands.h
 * DESCRIPTION: Header file for builtInCommands.c
 * AUTHOR: Chelsea Egan (eganch@oregonstate.edu)
 */

#ifndef PROGRAM3_BUILTINCOMMANDS_H
#define PROGRAM3_BUILTINCOMMANDS_H

// https://stackoverflow.com/a/4014981
enum Commands {DEFAULT = -1, EXIT = 1, CD, STATUS, COMMENT};
typedef struct {char *key; int val; } cmdStruct;
static cmdStruct lookUpTable[] = {
        {"exit", EXIT},
        {"cd", CD},
        {"status", STATUS},
        {"#", COMMENT}
};
#define NUM_KEYS (sizeof(lookUpTable)/sizeof(cmdStruct))

int keyFromString(char*);
void changeDirectory(char**, int);
int inRedirect(char*);
int outRedirect(char*);
int checkForRedirect(char*);
int handleRedirect(char**, int);
bool runCommand(char**, int);

#endif //PROGRAM3_BUILTINCOMMANDS_H
