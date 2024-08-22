#include "../pathsize.h"

f_size file_size(const char* file_path){
    struct stat file;
    if (stat(file_path, &file) == -1){
        errno = ENOFILE;
        return -1;
    }
    return (f_size) file.st_size;
}

f_size dir_size(const char* dir_path){
    f_size total_size = 0;
    DIR* dir = opendir(dir_path);
    if (!dir) return -1;

    struct dirent* entry;
    struct stat entry_data;
    char file_path[FILENAME_MAX+1];
    f_size sub_dir_size = 0;
    
    while ((entry = readdir(dir))){
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")){
            snprintf(file_path, FILENAME_MAX, "%s\\%s", dir_path, entry->d_name);
            file_path[FILENAME_MAX] = '\0';
            if (stat(file_path, &entry_data) == -1){
                closedir(dir);
                errno = ENOFILE;
                return -1;                
            }
            if (S_ISDIR(entry_data.st_mode)){
                sub_dir_size = dir_size(file_path);
                if (sub_dir_size == -1){
                    closedir(dir);
                    return -1;
                }
                total_size += sub_dir_size;
            }
            else{
                total_size += entry_data.st_size;
            }
        }
    }
    closedir(dir);
    return total_size;
}