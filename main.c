#define _GNU_SOURCE
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "foregroundProcesses.h"
#include "signals.h"
#include "backgroundProcesses.h"

#define MAX_ARGUMENTS 512
#define MAX_INPUT_LENGTH 2048

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

int keyFromString(char* key) {
    int i;
    for (i = 0; i < NUM_KEYS; i++) {
        cmdStruct command = lookUpTable[i];
        if (strcmp(command.key, key) == 0) {
            return command.val;
        }
    }

    return DEFAULT;
}

// https://www.linuxquestions.org/questions/programming-9/replace-a-substring-with-another-string-in-c-170076/
char* expandToPid(char* arg, char* buffer) {
    const char* expandVar = "$$";
    char* ptrToVar;

    ptrToVar = strstr(arg, expandVar);
    strncpy(buffer, arg, ptrToVar - arg);
    buffer[ptrToVar - arg]='\0';
    sprintf(buffer + (ptrToVar - arg), "%d%s", getpid(), ptrToVar + strlen(expandVar));
}

void checkForExpansion(char** splitInput, int numArgs) {
    const char *expandVar = "$$";
    int i;
    for (i = 0; i < numArgs; i++) {
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
    char* word = strtok(userInput, " \t");
    int i,
        numSpaces = 0;

    while (word) {
        splitInput = realloc(splitInput, sizeof(char*) * ++numSpaces);

        if (splitInput == NULL) {
            printf("ERROR: Memory allocation failed.\n");
            fflush(stdout);
            exit(1);
        }
        splitInput[numSpaces - 1] = word;
        word = strtok(NULL, " \t");
    }

    splitInput = realloc(splitInput, sizeof(char*) * (numSpaces + 1));
    splitInput[numSpaces] = 0;

    *numArgs = numSpaces;
    return splitInput;
}

// https://oregonstate.instructure.com/courses/1729341/pages/3-dot-3-advanced-user-input-with-getline
char* getUserInput() {
    int numCharsEntered = -5;
    char* lineEntered = NULL;
    size_t bufferSize = 0;

    while (1) {
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

        free(lineEntered);
        lineEntered = NULL;
    }

    return lineEntered;
}

void changeDirectory(char** args, int numArgs) {
    if (numArgs == 1) {
        chdir(getenv("HOME"));
    } else {
        chdir(args[1]);
    }
}

// http://www.cs.loyola.edu/~jglenn/702/S2005/Examples/dup2.html
int inRedirect(char* file) {
    int in;

    if ((in = open(file, O_RDONLY)) == -1) {
        printf("cannot open %s for input\n", file);
        fflush(stdout);
        return -1;
    }

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

    if ((out = open(file, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR)) == -1) {
        printf("cannot open %s for output\n", file);
        fflush(stdout);
        return -1;
    }

    if (dup2(out, 1) == -1) {
        printf("cannot redirect input to %s\n", file);
        fflush(stdout);
        return -1;
    }

    close(out);

    return 0;
}

int checkForRedirect(char* arg) {
    if (strcmp(arg, "<") == 0) {
        return 0;
    } else if (strcmp(arg, ">") == 0) {
        return 1;
    }

    return -1;
}

int handleRedirect(char** args, int numArgs) {
    bool inRedirectIsSet = false,
        outRedirectIsSet = false;
    int i,
        result = 0,
        redirectType = -1;

    if (numArgs > 2) {
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

        redirectType = checkForRedirect(args[numArgs - 2]);

        if (redirectType == 0) {
            result = inRedirect(args[numArgs - 1]);
            inRedirectIsSet = true;
        } else if (redirectType == 1) {
            result = outRedirect(args[numArgs - 1]);
            outRedirectIsSet = true;
        }

        if (result != 0) {
            return result;
        } else if (redirectType != -1) {
            args[numArgs - 2] ='\0';
            args[numArgs - 1] = '\0';
        }
    }

    if (numArgs > 4) {
        redirectType = -1;
        redirectType = checkForRedirect(args[numArgs - 4]);

        if (redirectType == 0) {
            result = inRedirect(args[numArgs - 3]);
            inRedirectIsSet = true;
        } else if (redirectType == 1) {
            result = outRedirect(args[numArgs - 3]);
            outRedirectIsSet = true;
        }

        if (result != 0) {
            return result;
        } else if (redirectType != -1) {
            args[numArgs - 4] = '\0';
            args[numArgs - 3] = '\0';
        }
    }

    if (runInBackground && !inRedirectIsSet) {
        if ((result = inRedirect("/dev/null")) != 0) {
            return result;
        }
    }
    if (runInBackground && !outRedirectIsSet) {
        if ((result = outRedirect("/dev/null")) != 0) {
            return result;
        }
    }

    return 0;
}

// https://stackoverflow.com/questions/25261/set-and-oldset-in-sigprocmask
void createChild(char** args, int numArgs) {
    pid_t spawnpid = -5;
    int result = -5;


    sigset_t x;
    sigemptyset(&x);
    sigaddset(&x, SIGTSTP);
    sigprocmask(SIG_BLOCK, &x, NULL);

    spawnpid = fork();
    switch (spawnpid) {
        case -1:
            perror("THEY TOOK MY SQUEEZIN' ARM!\n");
            exit(1);
            break;
        case 0:
            if (runInBackground) {
                signal(SIGINT, SIG_IGN);
            }
            signal(SIGTSTP, SIG_IGN);
            result = handleRedirect(args, numArgs);
            if (result != 0) {
                exit(1);
                break;
            }
            execvp(args[0], args);
            printf("%s: no such file or directory\n", args[0]);
            fflush(stdout);
            exit(1);
            break;
        default:
            if (runInBackground) {
                handleBackgroundChild(spawnpid);
            } else {
                handleForegroundChild(spawnpid);
            }
            sigprocmask(SIG_UNBLOCK, &x, NULL);
            break;
    }
}

void cleanUp() {
    if (runningProcess > 0) {
        kill(runningProcess, SIGKILL);
    }

    int i;
    for (i = 0; i < bckgrdPIDs.numPIDs; i++) {
        kill(bckgrdPIDs.pids[i], SIGKILL);
    }
}

int main() {
    bool isRunning = true;
    int numArgs = 0;
    char** splitInput;

    initForegroundVars();
    initTstpStatus();
    initProccessesArray();

    // https://www.geeksforgeeks.org/atexit-function-in-c-c/
    if (atexit(cleanUp) != 0) {
        printf("atexit registration failed. Exiting.\n");
        fflush(stdout);
        exit(1);
    }

    createSignalHandlers();

    char* userInput;

    while (isRunning) {
        int childExitMethod = -5;

        runningProcess = 0;

        checkForEndedProcesses();

        userInput = getUserInput();
        splitInput = splitCommand(userInput, &numArgs);
        checkForExpansion(splitInput, numArgs);
        if (numArgs < 1) {
            continue;
        }
        switch (keyFromString(splitInput[0])) {
            case CD:
                changeDirectory(splitInput, numArgs);
                break;
            case STATUS:
                printStatus();
                break;
            case COMMENT:
                break;
            case EXIT:
                isRunning = false;
                break;
            default:
                checkIfRunInBackground(splitInput, &numArgs);
                createChild(splitInput, numArgs);
                break;
        }
        free(userInput);
        free(splitInput);
    }

    free(bckgrdPIDs.pids);
    return 0;
}