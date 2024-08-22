#include "../pathlib.h"

char* norm_path(char* path){
    if (!path) return NULL;
    index_t iW=0, iR=0, sC=0;  
    char c;
    size_t path_len = strlen(path);
    for (; iR < path_len; iR++){
        c = path[iR];
        if (c == P_SEP || c == P_SEP_A){
            sC++;
            if (sC%2 == 1){
                path[iW++] = P_SEP;
            }
            else{
                sC -= 2;
            }               
        }
        else{
            path[iW++] = c;
            sC = 0;
        }
    }

    for (; iW < path_len; iW++){
        path[iW] = '\0';
    }
    return path;
}

Response path_normalize(char* path, char* dest, size_t dest_size){
    if (!dest || !path) return (Response) {r_error, ERR_ARG};
    index_t iW=0, iR=0, sepC=0;
    char c;
    memset(dest, 0, dest_size);
    for (iR=0; iR<strlen(path); iR++){
        if (iW >= dest_size-1) return (Response) {r_error, ERR_P_OVERF};
        c = path[iR];
        if (c == P_SEP || c == P_SEP_A){
            sepC ++;
            if (sepC%2 == 1){
                dest[iW++] = P_SEP;
            }
            else{
                sepC--;
            }
        }
        else{
            sepC = 0;
            dest[iW++] = c;
        }
    }
    for (index_t i=iW-sepC; i<iW; i++){
        dest[i] = '\0';
    }
    return (Response) {r_success, 0};
}

char* path_name(char* path){
    char* pcp = path + strlen(path);
    for (int i=0; pcp > path; pcp--, i++){
        if ((*pcp == '\\') || (*pcp == '/')){
            if (i!=1){
                pcp++;
                break;
            }
            *pcp = '\0';
        }
    }
    return pcp;
}

bool path_exists(const char* path){
    if (access(path, F_OK) == -1){
        return false;
    }
    return true;
}

bool path_permitted(const char* path){
    if (access(path, W_OK) == -1 || access(path, R_OK) == -1){
        return false;
    }
    return true;
}

bool path_isfile(const char* path){
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

bool path_isdir(const char* path){
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

char* get_cwd(char* buff, size_t size){
    if (buff != NULL){
        return getcwd(buff, size);
    }
    return NULL;
}

void path_join(const char* path1, const char* path2, char* dest, size_t dest_size) {
    if (!dest || dest_size == 0) return;

    memset(dest, 0, dest_size); // Properly zero out the destination buffer

    if (!path1) {
        strncpy(dest, path2, dest_size - 1);
        return;
    }
    if (!path2) {
        strncpy(dest, path1, dest_size - 1);
        return;
    }

    // Determine the separator
    const char* sep = "/";
    #ifdef _WIN32
    sep = "\\";
    #endif

    size_t p1_len = strlen(path1);
    size_t p2_len = strlen(path2);

    if (p1_len >= dest_size) {
        strncpy(dest, path1, dest_size - 1);
        dest[dest_size - 1] = '\0'; // Ensure null termination
        return;
    }

    // Copy path1 to dest
    strncpy(dest, path1, dest_size - 1);
    dest[dest_size - 1] = '\0'; // Ensure null termination

    // Check if we need to add a separator
    if (dest[p1_len - 1] != sep[0] && p1_len + 1 < dest_size - 1) {
        strncat(dest, sep, dest_size - p1_len - 1);
    }

    // Add path2
    strncat(dest, path2, dest_size - strlen(dest) - 1);
    dest[dest_size - 1] = '\0'; // Ensure null termination
}