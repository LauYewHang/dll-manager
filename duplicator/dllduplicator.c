#include <stdlib.h>
#include <getopt.h>

#include <psapi.h>

#include "dllduplicator.h"

dll_linked_list* get_executable_dll(LPWSTR application_name){
    STARTUPINFO startup_info;
    PROCESS_INFORMATION process_info;

    ZeroMemory(&startup_info, sizeof(startup_info));
    startup_info.cb = sizeof(startup_info);
    ZeroMemory(&process_info, sizeof(process_info));

    CreateProcessW(
        NULL,
        application_name,
        NULL,
        NULL,
        FALSE,
        DEBUG_ONLY_THIS_PROCESS,
        NULL,
        NULL,
        &startup_info,
        &process_info
    );

    LPDEBUG_EVENT debug_event;

    do {
        WaitForDebugEvent(debug_event, INFINITE);
        
        switch(debug_event->dwDebugEventCode){
            case LOAD_DLL_DEBUG_EVENT:
                break;
            default:
                break;
        }
    }while(debug_event->dwDebugEventCode != CREATE_THREAD_DEBUG_EVENT);
}

LPWSTR get_dll_path(HANDLE dll_handle){
    LPWSTR path_name = (LPWSTR) malloc(MAX_PATH * sizeof(WCHAR));
    DWORD get_path_result = GetFinalPathNameByHandleW(dll_handle, path_name, MAX_PATH, VOLUME_NAME_DOS);

    return get_path_result <= MAX_PATH ? path_name : NULL;
}

void print_dll_list(dll_linked_list list){

}
