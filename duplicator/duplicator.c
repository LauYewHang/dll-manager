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

    // get the current directory so that the .bat script can navigate to here and run the ldd command
    int dirStringSize = 512;
    char *dirString;
    GetCurrentDirectory(dirStringSize, dirString);

    // pretest.bat
    // used to test if ldd.exe exists and is in the environment path
    FILE *pretestBat = fopen("pretest.bat", "w");
    char driveDirectory[3]; // driveDirectory variable to record the drive path (e.g. "D:", "C:")
    strncpy(driveDirectory, dirString, 2); // copy the first two char (the drive directory)
    driveDirectory[2] = '\0'; // close the string

    char *batScript =   "set lddPathList=where ldd\n"
                        "\%lddPathList\% > lddPathList.txt";

    // write into pretest.bat
    // 1. check where the current directory is (where is the p)
    fprintf(pretestBat, "%s\ncd %s\n%s", driveDirectory, dirString, batScript);
    fclose(pretestBat);

    FILE *duplicatorBat = fopen("duplicator.bat", "w");
    fprintf(duplicatorBat, "d:\ncd %s\nwhere ldd\nldd %s\npause", dirString, argv[1]);

    ShellExecuteA(NULL, "runas", "pretest.bat", NULL, NULL, SW_SHOWDEFAULT);
    GetLastError();
}

void printHelp(){
    printf("");
}
