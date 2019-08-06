/*
 * CLASS: CS 344-400
 * ASSIGNMENT: Program 3
 * FILE NAME: main.c
 * DESCRIPTION: This program creates and operates a shell called
 * smallsh. It works similarly to the bash shell with a command line
 * and running commands.
 * AUTHOR: Chelsea Egan (eganch@oregonstate.edu)
 */

#define _GNU_SOURCE
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "backgroundProcesses.h"
#include "builtInCommands.h"
#include "foregroundProcesses.h"
#include "processes.h"
#include "signals.h"
#include "userInput.h"

int main() {
    // Used as a flag for when user chooses to exit
    bool isRunning = true;
    int numArgs = 0;
    char* userInput;
    char** splitInput;

    // Initialize variables for start of program
    initForegroundVars();
    initTstpStatus();
    initProccessesArray();

    // Set up handlers for SIGINT and SIGTSTP
    createSignalHandlers();

    // Set up the clean up for when the program exits
    registerExitFunction();

    // Main program loop
    while (isRunning) {
        // Flag indicating that there is no process currently running
        runningProcess = 0;

        // Clean up any background processes that have ended
        checkForEndedProcesses();

        // Read in command from user
        userInput = getUserInput();

        // If it's a comment, ignore and start loop again
        if (checkIfComment(userInput) == 0) {
            continue;
        }

        // Create array consisting of command and args from user input
        splitInput = splitCommand(userInput, &numArgs);

        // Find any instances of "$$" and expand into the process ID
        checkForExpansion(splitInput, numArgs);

        // If a blank line, ignore and start loop again
        if (numArgs < 1) {
            continue;
        }

        // Perform requested command
        isRunning = runCommand(splitInput, numArgs);

        // Clean up used memory
        free(userInput);
        free(splitInput);
    }

    exit(EXIT_SUCCESS);
}