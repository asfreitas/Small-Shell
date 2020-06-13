// Andrew Freitas
// Program 3 "Smallsh"
#include "commands.h"
#include "userinput.h"

void cd(const char * path)
{

    char updatedDir[PATH_MAX];
    /* Check all of the potential states for cd */
    if(path == NULL){
        const char *homedir = getenv("HOME");
        strcpy(updatedDir, homedir);
    }
    else{
        strcpy(updatedDir, path);
    }
    chdir(updatedDir);

}
// this is called to exit the program
void exitProg(int* bgPID){
    int x, pidNo, pid, childExitMethod;
    // go through each process and kill them if they are still running
    for(x = 0; x < 50; x++){
        pidNo = bgPID[x];

        pid = waitpid(pidNo, &childExitMethod, WNOHANG);
        if(pidNo != 0 && pid == 0){
            kill(pidNo, SIGTERM);
        }
        else{
            continue;
        }


    }
    //exit with status 0
    exit(0);
}
// this code was adapted from the CS344 lectures at Oregon State University
void getStatus(int childExitMethod) {

    if (WIFEXITED(childExitMethod)) {

        int exitStatus = WEXITSTATUS(childExitMethod);
        printf("exit value %d\n", exitStatus);
    }

    else if(WIFSIGNALED(childExitMethod)){
        int termSignal = WTERMSIG(childExitMethod);
        printf("terminated by signal %d\n", termSignal);
    }
}