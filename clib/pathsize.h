#ifndef PATH_SIZE_H
#define PATH_SIZE_H

#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include "types.h"

f_size file_size(const char* path);
f_size dir_size(const char* path);

#include "src/pathsize.c"
#endif //PATH_SIZE_H