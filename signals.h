//
// Created by Chelsea on 8/4/2019.
//

#ifndef PROGRAM3_SIGNALS_H
#define PROGRAM3_SIGNALS_H

int tstpStatus;

void initTstpStatus();
void catchSIGINT(int);
void catchSIGTSTP();
void createSignalHandlers();

#endif //PROGRAM3_SIGNALS_H
