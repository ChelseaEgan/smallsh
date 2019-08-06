/*
 * CLASS: CS 344-400
 * ASSIGNMENT: Program 3
 * FILE NAME: userInput.c
 * DESCRIPTION: Functions to handle the user input, split
 * into an args array, and check for variable expansion
 * AUTHOR: Chelsea Egan (eganch@oregonstate.edu)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "userInput.h"

const char *expandVar = "$$";

// https://www.linuxquestions.org/questions/programming-9/replace-a-substring-with-another-string-in-c-170076/
char* expandToPid(char* arg, char* buffer) {
    char* ptrToVar;

    // Find location of "$$" in string if there
    ptrToVar = strstr(arg, expandVar);

    // Copy into buffer everything up to "$$"
    strncpy(buffer, arg, ptrToVar - arg);
    buffer[ptrToVar - arg] = '\0';

    // Insert the process ID then copy the rest of the string
    sprintf(buffer + (ptrToVar - arg), "%d%s", getpid(), ptrToVar + strlen(expandVar));
}

void checkForExpansion(char** splitInput, int numArgs) {
    int i;

    // Check if "$$" is in any of the args
    for (i = 0; i < numArgs; i++) {
        // If "$$" is found, expand
        while (strstr(splitInput[i], expandVar) != NULL) {
            char* buffer = (char *)malloc(4096 * sizeof(char));
            expandToPid(splitInput[i], buffer);
            strcpy(splitInput[i], buffer);
            free(buffer);
        }
    }
}

// https://stackoverflow.com/a/11198630
// https://linux.die.net/man/3/strstr
char** splitCommand(char* userInput, int* numArgs) {
    char** splitInput = NULL;
    int numSpaces = 0;

    // Split input by whitespace
    char* word = strtok(userInput, " \t");

    while (word) {
        // Resize splitInput array
        splitInput = realloc(splitInput, sizeof(char*) * ++numSpaces);

        if (splitInput == NULL) {
            printf("ERROR: Memory allocation failed.\n");
            fflush(stdout);
            exit(1);
        }

        // Insert word into splitInput array
        splitInput[numSpaces - 1] = word;
        word = strtok(NULL, " \t");
    }

    splitInput = realloc(splitInput, sizeof(char*) * (numSpaces + 1));

    // Insert null at end of input array
    splitInput[numSpaces] = 0;

    // Set the number of args to the number of whitespaces found
    *numArgs = numSpaces;

    return splitInput;
}

// https://oregonstate.instructure.com/courses/1729341/pages/3-dot-3-advanced-user-input-with-getline
char* getUserInput() {
    int numCharsEntered = -5;
    char* lineEntered = NULL;
    size_t bufferSize = 0;

    while (1) {
        // Prompt for input
        printf("\r: ");
        fflush(stdout);

        // Get a line from the user
        numCharsEntered = getline(&lineEntered, &bufferSize, stdin);
        if (numCharsEntered == -1) {
            clearerr(stdin);
        } else {
            // Remove the trailing \n that getline adds
            lineEntered[strcspn(lineEntered, "\n")] = '\0';
            break;
        }

        // Reset line
        free(lineEntered);
        lineEntered = NULL;
    }

    return lineEntered;
}

int checkIfComment(char* userInput) {
    // "#" indicates a comment, which should be ignored
    char* commentKey = "#";
    return strncmp(userInput, commentKey, 1);
}