#include <stdio.h>
#include <getopt.h>

#include "dllduplicator.h"

const char* EXECUTABLE_EXTENSION = ".exe";
const int EXTENSION_SIZE = 4; // the char length of EXECUTABLE_EXTENSION

struct option long_options[] = {
    {"help", no_argument, NULL, 'h'},
    {"verbose", no_argument, NULL, 'v'},
    {"save", required_argument, NULL, 's'},
    {"copy", required_argument, NULL, 'c'},
    {NULL, 0, NULL, -1} // to deal with unknown options (unregistered options)
};

void print_help();

int main(int argc, char** argv){
    LPWSTR target_executable = NULL;
    LPWSTR save_path = NULL;
    LPWSTR copy_path = NULL;
    LPSTR target_executable_s = calloc(MAX_PATH, sizeof(CHAR));
    LPSTR save_path_s = calloc(MAX_PATH, sizeof(CHAR));
    LPSTR copy_path_s = calloc(MAX_PATH, sizeof(CHAR));
    bool verbose = FALSE;

    int option_index = 0;
    int character = 0; // ASCII index returned by getopt_long
    int option_arguments = 0; // record how many options (and option arguments) received

    // use getopt() to get arguments of options
    while ((character = getopt_long(argc, argv, "hvs:c:", long_options, &option_index)) != -1){
        switch (character){
            case 'h':
                print_help();
                return 0;
            case 'v':
                verbose = TRUE;
                break;
            case 's':
                free(save_path);
                save_path = (LPWSTR) malloc(sizeof(WCHAR) * MAX_PATH);
                strcpy(save_path_s, optarg);
                mbstowcs(save_path, save_path_s, MAX_PATH);
                option_arguments++;
                break;
            case 'c':
                free(copy_path);
                copy_path = (LPWSTR) malloc(sizeof(WCHAR) * MAX_PATH);
                strcpy(copy_path_s, optarg);
                mbstowcs(copy_path, copy_path_s, MAX_PATH);
                option_arguments++;
                break;
        }
    }

    // check for missing arguments and duplicate file name
    save_path_s = save_path ? save_path_s : "dll_list.txt";
    copy_path_s = copy_path ? copy_path_s : "dll_list";
    save_path = calloc(MAX_PATH, sizeof(WCHAR));
    copy_path = calloc(MAX_PATH, sizeof(WCHAR));
    mbstowcs(save_path, save_path_s, MAX_PATH);
    mbstowcs(copy_path, copy_path_s, MAX_PATH);

    LPSTR full_save_path = calloc(MAX_PATH, sizeof(CHAR));
    LPSTR full_copy_path = calloc(MAX_PATH, sizeof(CHAR));
    LPSTR full_target_executable = calloc(MAX_PATH, sizeof(CHAR));

    GetFullPathNameA(save_path_s, MAX_PATH, full_save_path, NULL);
    GetFullPathNameA(copy_path_s, MAX_PATH, full_copy_path, NULL);

    if (strcmp(full_save_path, full_copy_path) == 0){
        fprintf(stderr, "The save path and copy path cannot be the same.\n", 64);
        exit(EXIT_FAILURE);
    }

    if (target_executable == NULL){
        if (argc - option_arguments*2 < 1){
            fprintf(stderr, "Missing file argument.\n", 64);
            exit(EXIT_FAILURE);
        } else {
            // iterate argv to find possible .exe file
            for (int i = 1, argument_length = 0, equal_extension = -1; i < argc; i++){
                argument_length = strlen(argv[i]);
                // check if the argument is at least longer than length of ".exe"
                if (argument_length > EXTENSION_SIZE){
                    // check if the argument end with ".exe"
                    equal_extension = strcmp(argv[i] + argument_length - EXTENSION_SIZE, EXECUTABLE_EXTENSION);
                    if (equal_extension == 0){
                        // check if the executable has the same name as output file / path
                        GetFullPathNameA(argv[i], MAX_PATH, full_target_executable, NULL);
                        // the executable argument is valid if it is not equal to the output file / path
                        if (strcmp(full_target_executable, full_save_path) != 0 && strcmp(full_target_executable, full_copy_path) != 0){
                            strcpy(target_executable_s, argv[i]);
                            target_executable = (LPWSTR) malloc(sizeof(WCHAR) * MAX_PATH);
                            mbstowcs(target_executable, target_executable_s, MAX_PATH);
                            break;
                        }
                    }
                }
            }

            if (target_executable == NULL){
                fprintf(stderr, "No valid executable file found.\n", 64);
                printf("The executable file name cannot be the same as save path or copy path.");
                exit(EXIT_FAILURE);
            }
        }
    }

    // save full path name for dllduplicator functions (for verbose)
    mbstowcs(save_path, full_save_path, MAX_PATH);
    mbstowcs(copy_path, full_copy_path, MAX_PATH);
    mbstowcs(target_executable, full_target_executable, MAX_PATH);

    dll_linked_list* dll_list = get_executable_dll(target_executable, verbose);
    save_dll_list(dll_list, save_path, verbose);
    copy_dll_list(dll_list, copy_path, verbose);

    printf("DLL list of %ls:\n", target_executable);
    print_dll_list(dll_list, verbose);

    return 0;
}

void print_help(){

}
