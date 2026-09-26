#ifndef __DLL_DUPLICATOR__
    #define __DLL_DUPLICATOR__
    #include <windows.h>

    typedef dll dll;
    typedef dll_linked_list dll_linked_list;

    struct dll {
        HANDLE h_file;
        LPVOID lp_base_of_dll;
        LPWSTR dll_path;
        dll* next;
    };

    struct dll_linked_list {
        dll* current;
        dll_linked_list* next;
    };

    void print_help();
    
    LPWSTR get_dll_path(HANDLE dll_handle);
    dll_linked_list* get_executable_dll(LPWSTR application_name);
    void print_dll_list(dll_linked_list list);
#endif
