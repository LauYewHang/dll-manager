#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <shellapi.h>
#include <string.h>

void printHelp();

int main(int argc, char **argv){
    if (argc < 2){
        printf("duplicator: missing argument(s)\n");
        printf("Try 'duplicator --help' for more information.\n");
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0){
        printHelp();
        return EXIT_SUCCESS;
    }

    int dirStringSize = 512;
    char *dirString;
    GetCurrentDirectory(dirStringSize, dirString);

    FILE *duplicatorBat = fopen("duplicator.bat", "w");
    fprintf(duplicatorBat, "d:\ncd %s\nldd %s\npause", dirString, argv[1]);

    ShellExecuteA(NULL, "runas", "duplicator.bat", NULL, dirString, SW_SHOWDEFAULT);
}

void printHelp(){
    printf("");
}