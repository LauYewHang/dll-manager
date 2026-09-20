#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define READ "r"
#define WRITE "w"
#define EXTENDED "+"
#define AND "&"

void printHelp();
int executeScript();

int main(int argc, char **argv){
    // if (argc < 2){
    //     printf("duplicator: missing argument(s)\n");
    //     printf("Try 'duplicator --help' for more information.\n");
    //     return EXIT_FAILURE;
    // }

    // if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0){
    //     printHelp();
    //     return EXIT_SUCCESS;
    // }

    int hasError = executeScript();
    printf("has error? %d", hasError);
}

void printHelp(){
    printf("");
}

int executeScript(){
    int e1 = system(
        "set cwd=\%cd\%" AND
        "echo \%cd\%>test2.txt" AND
        "pause"
    );

    FILE* fp = fopen("test.txt", WRITE EXTENDED);
    FILE* fp2 = fopen("somefile.txt", READ);
    if (fp){
        fprintf(fp, "some text");
    }

    return e1;
}
