#include <stdio.h>
#include <windows.h>
#include <winver.h>

int main(int argc, char **argv){
    HMODULE hm = LoadLibraryA(argv[1]);
    printf("has module? %u %s\n", hm, argv[1]);

    LPVOID output;

    DWORD filesize = GetFileVersionInfoSizeA(argv[1], 0);
    printf("file size? %u\n", filesize);
    printf("last error?%d\n", GetLastError());

    int hasFileVersion = GetFileVersionInfoA(
        argv[1],
        0,
        filesize,
        output
    );
    printf("has file version?%d\n", hasFileVersion);
    printf("last error?%d\n", GetLastError());
}