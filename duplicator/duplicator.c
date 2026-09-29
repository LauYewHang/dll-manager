#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>

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
    printf("1\n");
    int hasError = executeScript(argv[1]);
    printf("2\n");
}

void printHelp(){
    printf("");
}

int executeScript(const char *filename){
    bool isdll;
    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;
    wchar_t *cmd = L"dummy";
    LPWSTR cmd2;
    WINBOOL createdProcess = CreateProcessW(
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

    printf("created process? %d\n", createdProcess);
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

    LPDEBUG_EVENT ev;

    while (1){
        printf("waiting debug\n");
        WaitForDebugEvent(ev, INFINITE);

        printf("debug code? %u\n", ev->dwDebugEventCode);
        // printf("createprocessinfo? %p\n", ev.u.CreateProcessInfo.lpBaseOfImage);

        if (ev->dwDebugEventCode == LOAD_DLL_DEBUG_EVENT){
            printf("dll address? %p\n", ev->u.LoadDll.lpBaseOfDll);
            wchar_t *dllname = (wchar_t*) malloc(1024 * sizeof(wchar_t));
            LPSTR dlname = malloc (sizeof(char) * 1024);
            DWORD gfea = GetModuleFileNameExW(NULL, (HMODULE) ev->u.LoadDll.lpBaseOfDll, dllname, 1024);
            printf("dll name: %ls %u\n", dllname, gfea);
            wchar_t dllname2[1024];
            GetFinalPathNameByHandleW(ev->u.LoadDll.hFile, dllname2, 1024, VOLUME_NAME_DOS);
            printf("dll name2: %ls\n", dllname2);
        }
        
        LPVOID lpbuffer = malloc(4096);
        size_t bytesread;
        WINBOOL canreadprocess;
        PIMAGE_DOS_HEADER dosheader = (PIMAGE_DOS_HEADER) alloca (4096);
        PIMAGE_NT_HEADERS ntheader;
        PVOID entry;

        // ntheader = (PIMAGE_NT_HEADERS)((PBYTE) dosheader + dosheader->e_lfanew);
        // entry = ev.u.CreateProcessInfo.lpBaseOfImage + ntheader->OptionalHeader.AddressOfEntryPoint;
        // WINBOOL canwrite = WriteProcessMemory(pi.hProcess, entry, &int3, 1, &bytesread);
        // printf("canwrite? %d\n", canwrite);
        sleep(1);
        ContinueDebugEvent(ev->dwProcessId, ev->dwThreadId, DBG_CONTINUE);
    }

    wchar_t dllname[256];
    int someint = 10;
    //GetFinalPathNameByHandleW(ev.u.LoadDll.hFile, dllname, PATH_MAX, 0);
    printf("test extract dll: %p\nsomeintp: %p\n", ev->u.LoadDll.lpBaseOfDll, &someint);

    return 0;
}
