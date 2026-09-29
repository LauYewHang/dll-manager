#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>

#include <windows.h>
#include <psapi.h>

int main(int argc, char** argv){
    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;
    wchar_t *cmd = (wchar_t*) malloc(sizeof(wchar_t) * MAX_PATH);
    mbstowcs(cmd, argv[1], MAX_PATH);
    CreateProcessW(
        NULL,
        cmd,
        NULL,
        NULL,
        FALSE,
        DEBUG_ONLY_THIS_PROCESS,
        NULL,
        NULL,
        &si,
        &pi
    );
    printf("???");
}
