// Andrew Freitas
// Program 3 "Smallsh"

#include "userinput.h"
#include "commands.h"
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

// a few global variables to make execution easier, mostly involved with signals
bool forkBegun;
pid_t foregroundPID;
bool foregroundOnly;
struct sigaction SIGINT_action = {0},  SIGTSTP_action = {0};


// function for catching the sigint
void catchSIGINT(int signo){
    int childExitMethod;
    int result = waitpid(foregroundPID, &childExitMethod, WNOWAIT);
    int myresult = WIFSIGNALED(childExitMethod);
    //this is a check to make sure there is a current foregroundPID and it is not finished
    if(result == -1 && foregroundPID != -5){
        char* message = "terminated by signal 2\n";
        write(STDOUT_FILENO, message, 23);
    }
}
// function for catching sigtstp
void catchSIGTSTP(int signo){
    char* message;
    int exitstatus;
    waitpid(foregroundPID, &exitstatus, 0);
    // check whether the foreground mode is on or not and write a message and set the bool
    if(!foregroundOnly){
        foregroundOnly = true;
        message = "Entering foreground-only mode (& is now ignored)\n";
        write(STDOUT_FILENO, message, 49);
    }
    else{
        foregroundOnly = false;
        message = "Exiting foreground-only mode\n";
        write(STDOUT_FILENO, message, 29);
    }

}

// set the main signals for the shell for both SIGINT and SIGTSTP
void setSignalStructsMain(){


    SIGINT_action.sa_handler = catchSIGINT;
    sigfillset(&SIGINT_action.sa_mask);
    SIGINT_action.sa_flags = SA_RESTART;
    sigaction(SIGINT, &SIGINT_action, NULL);


    SIGTSTP_action.sa_handler = catchSIGTSTP;
    SIGTSTP_action.sa_flags = 0;
    sigfillset(&SIGTSTP_action.sa_mask);
    sigaction(SIGTSTP, &SIGTSTP_action, NULL);

}
// set the child signals for the shell for both SIGINT and SIGTSTP
void setSignalStructsChild(){
    SIGINT_action.sa_handler = SIG_DFL;
    sigfillset(&SIGINT_action.sa_mask);
    SIGINT_action.sa_flags = 0;
    sigaction(SIGINT, &SIGINT_action, NULL);

    SIGTSTP_action.sa_handler = SIG_IGN;
    SIGTSTP_action.sa_flags = 0;
    sigfillset(&SIGTSTP_action.sa_mask);
    sigaction(SIGTSTP, &SIGTSTP_action, NULL);

}
// checks to see if there
bool isBuiltInCommand(char* command){
    if(command != NULL && (strcmp(command, "exit") == 0 || strcmp(command, "cd") == 0
           || strcmp(command, "status") == 0) ) {
        return true;
    }
    return false;

}
// runs one of the built-in commands
void parentProcess(char* command, char* argument, int childExitMethod, int* bgPID){
    if(strcmp(command, "exit") == 0){
        exitProg(bgPID);
    }
    else if(strcmp(command, "cd") == 0){
        cd(argument);
    }
    else if(strcmp(command, "status") == 0) {
        getStatus(childExitMethod);
    }
}
// calls exec on the arguments passed in
void childProcess(char** arguments){
        // this is maybe not necessary but one last sanity check that it's not built-in
        if(!isBuiltInCommand(*arguments)){
            execvp(*arguments, arguments);
        }

}
void redirection(char* outputRedir, char* inputRedir, bool backgroundProcess, bool foregroundOnly){
    // redirect to dev/null if it is running in background
    if(backgroundProcess && !foregroundOnly){
        int fileOut = open("/dev/null", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        dup2(fileOut, 1);

        int fileIn = open("/dev/null", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        dup2(fileIn, 0);
        return;
    }
    // otherwise both input and output for if they are passed in and redirect them
    // to the stdin or stdout as necessary
    if(outputRedir != NULL)
    {
            int fileOut = open(outputRedir, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(fileOut, 1);
    }

    if(inputRedir != NULL)
    {
            int fileIn = open(inputRedir, O_RDONLY);
            if(fileIn == -1){
                printf("cannot open %s for input\n", inputRedir);
                exit(1);
            }
            dup2(fileIn, 0);


    }

}
// checks for an empty spot in the array and then inserts the pid of a
// bg process inside
void insertBGPID(int* bgPID, int pid){
    int x = 0;
    while(bgPID[x] != 0){
        x++;
    }
    bgPID[x] = pid;

}
// check if the background processes are finished and prints their pid and status
void checkBGPIDComplete(int* bgPID){
    int x, pidNo, pid, childExitMethod;
    for(x = 0; x < 50; x++){
        // gets both the pid and the exit status
        pidNo = bgPID[x];
        pid = waitpid(pidNo, &childExitMethod, WNOHANG);
        // checks to make sure it's a valid pid and has exited
        if(pid == 0 || pidNo == 0)
            continue;
        printf("background pid %d is done: ", pidNo);
        // call getStatus to return status
        getStatus(childExitMethod);
        // remove pid from array
        bgPID[x] = 0;

    }
}
int forkProcess(){
    return fork();
}

void main() {
    // a bunch of variables on the stack
    int backgroundPIDList[500] = {0};
    foregroundOnly = false;
    bool backgroundProcess;
    int mainPID = getpid();
    forkBegun = false;
    int childExitMethod = 0;
    char* userInput;
    char* arguments[512];
    memset(arguments, 0, sizeof(arguments));
    char* outputRedir = NULL;
    char* inputRedir = NULL;
    char myInput[2048];
    memset(myInput, 0, 2048);


    setSignalStructsMain();


    while(1)
    {

        foregroundPID = -5;
        fflush(stdout);
        // check for any finished background processes
        checkBGPIDComplete(backgroundPIDList);
        // get user input
        userInput = promptUser();
        // set variable for background processes
        backgroundProcess = isBackground(userInput);
        // free the memory from the stack here because we don't need it anymore
        replaceWithProcessID(userInput, myInput, mainPID);
        // check if the user entered a comment or empty string
        if(isComment(myInput) || isEmptyString(myInput))
            continue;
        // separates all of the
        getArguments(arguments, myInput, &outputRedir, &inputRedir);


        /* Only fork if there isn't a fork and it isn't a built in command being run */

        if(!forkBegun && !isBuiltInCommand(arguments[0])){
            foregroundPID = forkProcess(); // fork process
            forkBegun = true;
            switch(foregroundPID)
            {
                case -1:
                    perror("Failed to fork process");
                    exit(1);
                case 0:

                    setSignalStructsChild();
                    // change the signals for the child
                    redirection(outputRedir, inputRedir, backgroundProcess, foregroundOnly);
                    // redirect all of the output, taking into consideration if it is running in background
                    childProcess(arguments);
                    // run the process and declare an error if it doesn't run
                    printf("%s: no such file or directory\n", *arguments);
                    exit(1);

                default:
                    // check to see if the user is NOT asking for a background process
                    // also if it is foreground only also wait for the process to finish
                    if(!backgroundProcess || foregroundOnly)
                        waitpid(foregroundPID, &childExitMethod, 0);
                    else { // don't wait, print the backgroundpid, and add it to the array
                        printf("background pid is %d\n", foregroundPID);
                        insertBGPID(backgroundPIDList, foregroundPID);
                    }
                    break;
            }

        } else{ // otherwise run a background process
            parentProcess(arguments[0], arguments[1], childExitMethod, backgroundPIDList);
        }
        // reset some variables at the end for next command
        memset(arguments, 0, sizeof(arguments));
        outputRedir = NULL;
        inputRedir = NULL;
        forkBegun = false;
        memset(myInput, 0, 2048);


    }
}

