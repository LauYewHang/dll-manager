#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <windows.h>
#include <psapi.h>

#define READ "r"
#define WRITE "w"
#define EXTENDED "+"
#define AND "&"

void printHelp();
int executeScript(const char *filename);

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

    int hasError = executeScript(argv[1]);
}

void printHelp(){
    printf("");
}

int executeScript(const char *filename){
    bool isdll;
    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;
    wchar_t *cmd;
    LPWSTR cmd2;
    // WINBOOL createdProcess = CreateProcessW(
    //     NULL,
    //     cmd,
    //     NULL,
    //     NULL,
    //     FALSE,
    //     DEBUG_ONLY_THIS_PROCESS,
    //     NULL,
    //     NULL,
    //     &si,
    //     &pi
    // );

    // printf("created process? %d\n", createdProcess);
    printf("last error? %u\n", GetLastError());

    HANDLE h = GetCurrentProcess();
    size_t len = 1024;
    wchar_t *buf = (wchar_t *) malloc (len * sizeof(wchar_t));
    wchar_t *filebuf = (wchar_t *) malloc (len * sizeof(wchar_t));
    DWORD res;
    GetModuleFileNameExW(
        h,
        NULL,
        buf,
        len
    );
    printf("filename size: %d\n", strlen(filename));
    mbstowcs(filebuf, filename, strlen(filename) + 1);

    printf("process string? %ls\n", buf);
    printf("strng? %ls \n", filebuf);

    CreateProcessW(
        NULL,
        filebuf,
        NULL,
        NULL,
        FALSE,
        DEBUG_ONLY_THIS_PROCESS,
        NULL,
        NULL,
        &si,
        &pi
    );

    printf("last error? %u\n", GetLastError());

    DEBUG_EVENT ev;
    WaitForDebugEvent(&ev, INFINITE);

    printf("debug code? %u\n", ev.dwDebugEventCode);

    return 0;
}
