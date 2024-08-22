#ifndef _ESSENTIAL_H
#define _ESSENTIAL_H

/*
    essential.h by cdm ©2024

    Description:
        essential definitions of unified return feedback values and type definitions for better code readability

    Resources:
        - stdio.h
        - stdlib.h
        - stdbool.h
        - assert.h (if ESSENTIAL_IMPLEMENTATION is defined)

        - clib::types.h
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "types.h"

typedef enum ErrorCode{
    r_success = 0,
    r_error = 1
} Result;

typedef struct{
    Result result;
    int code;
} Response;

#define PrintResult(res) (printf("%s\n", (res == r_success)? "Success!":"Error!"))
#define PrintResponse(res) (printf("%s => %d\n", (res.result == r_success)? "Success!":"Error!", res.code))

#define eprint(msg, ...) (fprintf(stderr, "[ERROR] " msg "\n", __VA_ARGS__))


#define S_Bool(b) ((b == true)? "true":"false")
#define Arr_Len(arr) (sizeof(arr)/sizeof(arr[0]))
#define I_In(i, min, max) (min<=i && i<=max)

#ifdef ESSENTIAL_IMPLEMENTATION
#include <assert.h>
char* shift_args(int* argc, char*** argv)
{
    assert(argc > 0 && "Index out of bounds!");
    char* result = **argv;
    *argc -= 1;
    *argv += 1;
    return result;
}
#endif // ESSENTIAL IMPLEMENTATION

#endif // _ESSENTIAL_H