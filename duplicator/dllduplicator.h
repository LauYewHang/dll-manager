#ifndef __DLL_DUPLICATOR__
    #define __DLL_DUPLICATOR__
    #include <windows.h>

    typedef struct dll dll;
    typedef struct dll_linked_list dll_linked_list;

    struct dll {
        HANDLE h_file;
        LPVOID lp_base_of_dll;
        LPWSTR dll_path;
    };

    struct dll_linked_list {
        dll* current;
        dll_linked_list* next;
    };
    
    dll_linked_list* get_executable_dll(LPWSTR application_name);
    LPWSTR get_dll_path(HANDLE dll_handle);
    void print_dll_list(dll_linked_list* list);
    bool save_dll_list(dll_linked_list* list, LPWSTR destination_path);
    bool copy_dll_list(dll_linked_list* list, LPWSTR destination_path);
#endif
