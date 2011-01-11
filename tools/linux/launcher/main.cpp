#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int pid,status;

char APP[] = "./bin/WarlocksGauntlet.bin32";

int main(int argc, char* argv[]){
    if (!(pid=fork())){
        char * ld = getenv("LD_LIBRARY_PATH");
        char lib[] = ":./libs32";
        int length = strlen(lib) + 1;
        if (ld != NULL) {
            length += strlen(ld);
        }
        char * new_ld = (char *)malloc(length*sizeof(char));
        strcpy(new_ld, "");
        if (ld != NULL) {
            strcpy(new_ld, ld);
        }
        strcat(new_ld, lib);

        setenv("LD_LIBRARY_PATH", new_ld, 1);
        ld = getenv("LD_LIBRARY_PATH");
        int i = execvp(APP, argv);
        fprintf(stderr, "Failed to launch %s, aborting\n", APP);
        _exit(-1);
    }
    while(pid!=wait(&status)){
        // do nothing
    }
}
