// Andrew Freitas
// Program 3 "Smallsh"
#include "userinput.h"


// checks to see if the input is a comment
bool isComment(const char* input) {
    if(input[0] == '#')
        return true;
    return false;

}
// checks to see if the input is an empty string
bool isEmptyString(const char* input) {
    if(strlen(input) == 1 || strlen(input) == 0)
        return true;
    return false;
}
// a lot goes on here but this eventually replaces $$ with the processid
// of the shell and this method was suggested by Benjamin Brewster in CS344
void replaceWithProcessID(char* input, char* myString, int processid)
{
    memset(myString, 0, 2048);
    char* pidTag = "$$";
    char* temp;

    // myString is passed on the stack and input is on the heap
    // and this uses myString to copy it over properly
    temp = strstr(input, pidTag);
    while(temp != NULL)
    {
        strncpy(temp, "%d", 2);
        sprintf(myString, input, processid);
        memset(input, 0, strlen(input));
        strcpy(input, myString);
        temp = strstr(input, pidTag);
    }
    // finally copy everything over to myString and free input since we don't need it anymore
    strcpy(myString, input);
    free(input);


}

char* promptUser() {
    size_t bufferSize = 0;
    char* lineEntered = NULL;
    int numCharsEntered;

    while(1)
    {

        char* userInput;
        printf(": ");
        numCharsEntered = getline(&lineEntered, &bufferSize, stdin);
        if(numCharsEntered ==  -1)
            clearerr(stdin);
        else if(numCharsEntered < 2049)
            break;
    }
    lineEntered[strcspn(lineEntered, "\n")] = '\0';
    return lineEntered;


}
void getArguments( char** arguments, char* input, char** outputRedir, char** inputRedir){

    char* token = strtok(input, " ");
    int i = 0;
    while(token != NULL){
        // check various conditions while checking each argument delimited by " "
        if(strcmp(token, ">") == 0){
            token = strtok(NULL, " ");
            *outputRedir = token;
        }
        else if(strcmp(token, "<") == 0){
            token = strtok(NULL, " ");
            *inputRedir = token;
        }
        else if(strcmp(token, "&") != 0){  // don't want to add & to the set of arguments

            arguments[i++] = token;
        }
        token = strtok(NULL, " ");
    }

}
// checks to see if the user is asking to run process in background
bool isBackground(char* input){
    int length = strlen(input);
    if(input[length - 1] == '&') // check last character for ampersand
        return true;
    return false;
}