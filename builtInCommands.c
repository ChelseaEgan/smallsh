/*
 * CLASS: CS 344-400
 * ASSIGNMENT: Program 3
 * FILE NAME: builtInCommands.c
 * DESCRIPTION: Functions to respond to requests for
 * built-in commands: change directory, status, exit,
 * and i/o redirection
 * AUTHOR: Chelsea Egan (eganch@oregonstate.edu)
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdbool.h>
#include "builtInCommands.h"
#include "backgroundProcesses.h"
#include "foregroundProcesses.h"
#include "processes.h"

int keyFromString(char* key) {
    int i;
    // Run through list of possible commands and return if match
    for (i = 0; i < NUM_KEYS; i++) {
        cmdStruct command = lookUpTable[i];
        if (strcmp(command.key, key) == 0) {
            return command.val;
        }
    }

    // No match found
    return DEFAULT;
}

void changeDirectory(char** args, int numArgs) {
    // No provided dir name - change to home dir
    if (numArgs == 1) {
        chdir(getenv("HOME"));
    } else {
        chdir(args[1]);
    }
}

// http://www.cs.loyola.edu/~jglenn/702/S2005/Examples/dup2.html
int inRedirect(char* file) {
    int in;

    // Try to open the file for input
    if ((in = open(file, O_RDONLY)) == -1) {
        printf("cannot open %s for input\n", file);
        fflush(stdout);
        return -1;
    }

    // Redirect input to the file
    if (dup2(in, 0) == -1) {
        printf("cannot redirect input to %s\n", file);
        fflush(stdout);
        return -1;
    }

    close(in);

    return 0;
}

// http://www.cs.loyola.edu/~jglenn/702/S2005/Examples/dup2.html
int outRedirect(char* file) {
    int out;

    // Try to open the file for output
    // Create if doesn't exist, truncate if it does
    if ((out = open(file, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR)) == -1) {
        printf("cannot open %s for output\n", file);
        fflush(stdout);
        return -1;
    }

    // Redirect output to that file
    if (dup2(out, 1) == -1) {
        printf("cannot redirect input to %s\n", file);
        fflush(stdout);
        return -1;
    }

    close(out);

    return 0;
}

int checkForRedirect(char* arg) {
    // Request to redirect input
    if (strcmp(arg, "<") == 0) {
        return 0;
    // Request to redirect output
    } else if (strcmp(arg, ">") == 0) {
        return 1;
    }

    // No redirect symbol
    return -1;
}

int handleRedirect(char** args, int numArgs) {
    bool inRedirectIsSet = false,
        outRedirectIsSet = false;
    int i,
        result = 0,
        redirectType = -1;

    // If there are enough args to contain a redirect request
    if (numArgs > 2) {
        // Check if the redirect comes at the end of the arguments list
        for (i = 0; i < numArgs; i++) {
            if (strcmp(args[i], ">") == 0 || strcmp(args[i], "<") == 0) {
                if (i == 0) {
                    printf("invalid command \n");
                    fflush(stdout);
                    return -1;
                }

                if (numArgs > (i + 2) && strcmp(args[i + 2], ">") != 0 && (strcmp(args[i + 2], "<") != 0)
                    && strcmp(args[i + 2], "&") != 0 && strcmp(args[i + 2], "\0") != 0) {
                    printf("invalid command \n");
                    fflush(stdout);
                    return -1;
                }
            }
        }

        // Check if input/output redirect
        redirectType = checkForRedirect(args[numArgs - 2]);

        // Perform the redirect
        if (redirectType == 0) {
            result = inRedirect(args[numArgs - 1]);
            inRedirectIsSet = true;
        } else if (redirectType == 1) {
            result = outRedirect(args[numArgs - 1]);
            outRedirectIsSet = true;
        }

        // If redirect fails, return result
        if (result != 0) {
            return result;
        // If redirect succeeds, "delete" the commands
        } else if (redirectType != -1) {
            args[numArgs - 2] ='\0';
            args[numArgs - 1] = '\0';
        }
    }

    // If the args list is long enough for both input and output redirection
    if (numArgs > 4) {
        redirectType = -1;

        // Check if input/output redirect
        redirectType = checkForRedirect(args[numArgs - 4]);

        // Perform the redirect
        if (redirectType == 0) {
            result = inRedirect(args[numArgs - 3]);
            inRedirectIsSet = true;
        } else if (redirectType == 1) {
            result = outRedirect(args[numArgs - 3]);
            outRedirectIsSet = true;
        }

        // If redirect fails, return result
        if (result != 0) {
            return result;
        // If redirect succeeds, "delete" the commands
        } else if (redirectType != -1) {
            args[numArgs - 4] = '\0';
            args[numArgs - 3] = '\0';
        }
    }

    // If it's a background process and no redirect was done on
    // the input, then redirect to /dev/null
    if (runInBackground && !inRedirectIsSet) {
        if ((result = inRedirect("/dev/null")) != 0) {
            return result;
        }
    }

    // If it's a background process and no redirect was done on
    // the output, then redirect to /dev/null
    if (runInBackground && !outRedirectIsSet) {
        if ((result = outRedirect("/dev/null")) != 0) {
            return result;
        }
    }

    return 0;
}

bool runCommand(char** splitInput, int numArgs) {
    bool isRunning = true;

    // Switch based on first arg in commands list
    switch (keyFromString(splitInput[0])) {
        case CD:
            // change current working directory
            changeDirectory(splitInput, numArgs);
            break;
        case STATUS:
            // print exit status of last process
            printStatus();
            break;
        case COMMENT:
            // ignore
            break;
        case EXIT:
            // break out of program loop
            isRunning = false;
            break;
        default:
            // run in background if requested then fork
            checkIfRunInBackground(splitInput, &numArgs);
            createChild(splitInput, numArgs);
            break;
    }

    return isRunning;
}