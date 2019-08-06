/*
 * CLASS: CS 344-400
 * ASSIGNMENT: Program 3
 * FILE NAME: userInput.h
 * DESCRIPTION: Header file for userInput.c
 * AUTHOR: Chelsea Egan (eganch@oregonstate.edu)
 */

#ifndef PROGRAM3_USERINPUT_H
#define PROGRAM3_USERINPUT_H

char* expandToPid(char*, char*);
void checkForExpansion(char**, int);
char** splitCommand(char*, int*);
char* getUserInput();
int checkIfComment(char*);

#endif //PROGRAM3_USERINPUT_H
