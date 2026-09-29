#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include <windows.h>
#include <psapi.h>
#include <winbase.h>

#include "dllduplicator.h"

const char* VERBOSE_GET_EXECUTABLE_DLL = "function - get_executable_dll():";
const char* VERBOSE_GET_DLL_PATH = "function - get_dll_path():";
const char* VERBOSE_PRINT_DLL_LIST = "function - print_dll_list():";
const char* VERBOSE_SAVE_DLL_LIST = "function - save_dll_list():";
const char* VERBOSE_COPY_DLL_LIST = "function - copy_dll_list():";

dll_linked_list* get_executable_dll(LPWSTR application_name, bool verbose){
    if (verbose)
        printf("%s Getting dll of executable '%ls'.\n", VERBOSE_GET_EXECUTABLE_DLL, application_name);

    STARTUPINFOW startup_info;
    PROCESS_INFORMATION process_info;

    ZeroMemory(&startup_info, sizeof(startup_info));
    startup_info.cb = sizeof(startup_info);
    ZeroMemory(&process_info, sizeof(process_info));

    if (verbose)
        printf("%s Creating debug process '%ls'.\n", VERBOSE_GET_EXECUTABLE_DLL, application_name);
    
    CreateProcessW(
        application_name,
        NULL,
        NULL,
        NULL,
        FALSE,
        DEBUG_ONLY_THIS_PROCESS,
        NULL,
        NULL,
        &startup_info,
        &process_info
    );

    if (verbose)
        printf("%s Created debug process '%ls'\n.", VERBOSE_GET_EXECUTABLE_DLL, application_name);

    LPDEBUG_EVENT debug_event = (LPDEBUG_EVENT) malloc(sizeof(DEBUG_EVENT));
    dll_linked_list* dll_list = (dll_linked_list*) malloc(sizeof(dll_linked_list));
    dll_list->current = NULL;
    dll_list->next = NULL;
    dll_linked_list* dll_list_pointer = dll_list;
    
    do {
        if (verbose)
            printf("%s Waiting for debug event.\n", VERBOSE_GET_EXECUTABLE_DLL);
        
        WaitForDebugEvent(debug_event, INFINITE);

        if (verbose)
            printf("%s Debug event occured, debug code - %d\n", VERBOSE_GET_EXECUTABLE_DLL, debug_event->dwDebugEventCode);

        switch(debug_event->dwDebugEventCode){
            case LOAD_DLL_DEBUG_EVENT:
                if (verbose)
                    printf("%s Debug event - LOAD_DLL_DEBUG_EVENT.\n", VERBOSE_GET_EXECUTABLE_DLL);
                
                dll* new_dll = (dll*) malloc(sizeof(dll));
                new_dll->h_file = debug_event->u.LoadDll.hFile;
                new_dll->lp_base_of_dll = debug_event->u.LoadDll.lpBaseOfDll;

                if (verbose)
                    printf("%s Getting dll path from loaded dll handle - %p.\n", VERBOSE_GET_EXECUTABLE_DLL, new_dll->h_file);
                
                new_dll->dll_path = get_dll_path(new_dll->h_file, verbose);
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

    if (verbose)
        printf("%s Finished getting executable dll, returning dll_list.\n", VERBOSE_GET_EXECUTABLE_DLL);

    return dll_list;
}

LPWSTR get_dll_path(HANDLE dll_handle, bool verbose){
    if (verbose)
        printf("%s Getting dll path from loaded dll handle - %p.\n", VERBOSE_GET_DLL_PATH, dll_handle);

    LPWSTR path_name = (LPWSTR) malloc(MAX_PATH * sizeof(WCHAR));
    DWORD get_path_result = GetFinalPathNameByHandleW(dll_handle, path_name, MAX_PATH, VOLUME_NAME_DOS);

    // remove the path specifier "\\?\" from path_name
    // information for path specifier: https://learn.microsoft.com/en-us/dotnet/standard/io/file-path-formats#dos-device-paths
    wcsncpy(path_name, path_name + 4, wcslen(path_name));

    if (verbose)
        printf("%s Gotten dll path name - %ls.\n", VERBOSE_GET_DLL_PATH, path_name);

    return get_path_result <= MAX_PATH ? path_name : NULL;
}

void print_dll_list(dll_linked_list* list, bool verbose){
    if (verbose)
        printf("%s Printing dll list.\n", VERBOSE_PRINT_DLL_LIST);

    dll_linked_list* list_pointer = list;

    while (list_pointer->current != NULL){
        printf("%ls\n", list_pointer->current->dll_path);
        list_pointer = list_pointer->next;
    }
}

bool save_dll_list(dll_linked_list* list, LPWSTR destination_path, bool verbose){
    if (verbose)
        printf("%s Saving dll list to '%ls'.\n", VERBOSE_SAVE_DLL_LIST, destination_path);

    FILE* save_file = NULL;
    if (destination_path == NULL){
        save_file = fopen("dll_list.txt", "w");
    } else {
        char* file_path = (char*) malloc(sizeof(char) * MAX_PATH);
        wcstombs(file_path, destination_path, MAX_PATH);
        save_file = fopen(file_path, "w");
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

bool copy_dll_list(dll_linked_list* list, LPWSTR destination_path, bool verbose){
    if (verbose)
        printf("%s Copying dll list to '%ls'.\n", VERBOSE_COPY_DLL_LIST, destination_path);

    char* copy_path = (char*) malloc(sizeof(char) * MAX_PATH);
    if (destination_path == NULL){
        copy_path[0] = '\0';
    }else {
        wcstombs(copy_path, destination_path, MAX_PATH);
    }

    char* mkdir_command = (char*) malloc(sizeof(char) * MAX_PATH * 2);
    strncpy(mkdir_command, "mkdir ", MAX_PATH);
    strcat(mkdir_command, copy_path);
    system(mkdir_command);

    dll_linked_list* list_pointer = list;
    while (list_pointer->current != NULL){
        char* file_path = (char*) malloc(sizeof(char) * MAX_PATH);
        wcstombs(file_path, list_pointer->current->dll_path, MAX_PATH);

        char* copy_command = (char*) malloc(sizeof(char) * MAX_PATH * 3);
        strncpy(copy_command, "copy ", MAX_PATH);
        strcat(copy_command, file_path);
        strcat(copy_command, " ");
        strcat(copy_command, copy_path);
        system(copy_command);

        list_pointer = list_pointer->next;
    }

    return 0;
}
