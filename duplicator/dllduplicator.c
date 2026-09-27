#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include <windows.h>
#include <psapi.h>

#include "dllduplicator.h"

dll_linked_list* get_executable_dll(LPWSTR application_name){
    STARTUPINFOW startup_info;
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

    LPDEBUG_EVENT debug_event = (LPDEBUG_EVENT) malloc(sizeof(DEBUG_EVENT));
    dll_linked_list* dll_list = (dll_linked_list*) malloc(sizeof(dll_linked_list));
    dll_list->current = NULL;
    dll_list->next = NULL;
    dll_linked_list* dll_list_pointer = dll_list;
    
    do {
        WaitForDebugEvent(debug_event, INFINITE);
        
        switch(debug_event->dwDebugEventCode){
            case LOAD_DLL_DEBUG_EVENT:
                dll* new_dll = (dll*) malloc(sizeof(dll));
                new_dll->h_file = debug_event->u.LoadDll.hFile;
                new_dll->lp_base_of_dll = debug_event->u.LoadDll.lpBaseOfDll;
                new_dll->dll_path = get_dll_path(new_dll->h_file);
                dll_list_pointer->current = new_dll;
                dll_list_pointer->next = (dll_linked_list*) malloc(sizeof(dll_linked_list));
                dll_list_pointer->next->current = NULL;
                dll_list_pointer = dll_list_pointer->next;
                break;
            default:
                break;        
        }

        ContinueDebugEvent(debug_event->dwProcessId, debug_event->dwThreadId, DBG_CONTINUE);
    }while(debug_event->dwDebugEventCode != CREATE_THREAD_DEBUG_EVENT);

    return dll_list;
}

LPWSTR get_dll_path(HANDLE dll_handle){
    LPWSTR path_name = (LPWSTR) malloc(MAX_PATH * sizeof(WCHAR));
    DWORD get_path_result = GetFinalPathNameByHandleW(dll_handle, path_name, MAX_PATH, VOLUME_NAME_DOS);

    // remove the path specifier "\\?\" from path_name
    // information for path specifier: https://learn.microsoft.com/en-us/dotnet/standard/io/file-path-formats#dos-device-paths
    wcsncpy(path_name, path_name + 4, wcslen(path_name));

    return get_path_result <= MAX_PATH ? path_name : NULL;
}

void print_dll_list(dll_linked_list* list){
    dll_linked_list* list_pointer = list;

    while (list_pointer->current != NULL){
        printf("%ls\n", list_pointer->current->dll_path);
        list_pointer = list_pointer->next;
    }
}

bool save_dll_list(dll_linked_list* list, LPWSTR destination_path){
    FILE* save_file;
    if (destination_path == NULL){
        save_file = fopen("dll_list.txt", "w");
    }else {
        char* file_path = (char*) malloc(sizeof(char) * MAX_PATH);
        wcstombs(file_path, destination_path, MAX_PATH);
    }

    if (save_file){
        dll_linked_list* list_pointer = list;

        while (list_pointer->current != NULL){
            fprintf(save_file, "%ls\n", list_pointer->current->dll_path);
            list_pointer = list_pointer->next;
        }

        return 1;
    }

    return 0;
}
