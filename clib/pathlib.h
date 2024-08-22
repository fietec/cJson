#ifndef _PATHLIB_H
#define _PATHLIB_H

/*
    pathlib.h by cdm ©2024

    Description:
        pathlib.h is a light-weight c-library for reading file statuses and manipulating file paths.

    Resources:
        c-lib:
            - stdio.h
            - stdbool.h
            - unistd.h
            - sys/stat.h
            - string.h
        other:
            - essential.h
        
*/

#include <stdio.h>
#include "essentials.h"
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>

#ifdef _WIN32 // Win32
#define P_SEP '\\'
#define P_SEP_A '/'
#else // Not Win32
#define P_SEP '/'
#define P_SEP_A '\\'
#endif // _WIN32

#define ERR_P_OVERF 1
#define ERR_ARG 2

#define PATH(name) char (name)[FILENAME_MAX]

// check if a path exists
bool path_exists(const char* path);
// check if the user is permitted to access a path
bool path_permitted(const char* path);
// check if a path refers to a file
bool path_isfile(const char* path);
// check if a path refers to a directory
bool path_isdir(const char* path);

// get the path of the current working directory
char* get_cwd(char* buff, size_t size);

// normalizes a path and returns the pointer to itself
char* norm_path(char* path);
// copy a normalized version of a path to a buffer
Response path_normalize(char* path, char* dest, size_t dest_size);
// return pointer to the base item of a path
char* path_name(char* path);
// copy a joined version of two paths to a buffer
void path_join(const char* path1, const char* path2, char* dest, size_t dest_size);

#include "src/pathlib.c"
#endif // _PATHLIB_H