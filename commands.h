//
// Created by andrew on 10/31/18.
//

#ifndef PROGRAM3_COMMANDS_H

#include <zconf.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include "userinput.h"



void cd(const char*);
bool relativeChange(const char*);
void exitProg(int*);
void getStatus(int);

#define PROGRAM3_COMMANDS_H

#endif //PROGRAM3_COMMANDS_H
