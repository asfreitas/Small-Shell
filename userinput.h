//
// Created by andrew on 10/31/18.
//

#ifndef PROGRAM3_USERINPUT_H





#define PROGRAM3_USERINPUT_H

#define _GNU_SOURCE
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <memory.h>
#include <pwd.h>
#include <string.h>

char* getInput();
bool isComment(const char*);
bool isEmptyString(const char*);
char* promptUser();
char* getCommand(char*);
void getArguments( char**, char*, char**, char**);
void replaceWithProcessID(char*, char*, int);
bool isBackground(char*);

#endif
