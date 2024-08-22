#include "../strlib.h"

bool str_is_int(char* s){
    if (!s) return false;
    char c;
    if (*s == '+' || *s == '-') s++;
    if (*s == '\0') return false;
    while ((c = *s++)){
        if (!isdigit(c)) return false;
    }
    return true;
}

bool str_is_float(char* s){
    char* ep = NULL;
    strtod(s, &ep);
    return (ep && !*ep);
}

int str_char_index(char* s, char c){
    if (!s) return -1;
    const char* p = s;
    do{
        if (*p == c) return p-s;
    } while (*++p != '\0');
    return -1;
}

void str_char_remove(char* s, char c){
    if (!s) return;
    char *pr = s, *pw = s;
    while (*pr) {
        *pw = *pr++;
        pw += (*pw != c);
    }
}

void str_char_replace(char* s, char o, char n){
    if (!s) return;
    char* p = s;
    while (*p){
        if (*p == o) *p = n;
        p++;
    }
}

char* str_to_higher(char* s){
    char* w=s;
    do{
        if (0x60 < *w && *w < 0x7b) *w &= 0xdf;
    } while (*++w != '\0');
    return s;
}

char* str_to_lower(char* s){
    char* w=s;
    do{
        if (0x40 < *w && *w < 0x5b) *w |= 0x20;
    } while (*++w != '\0');
    return s;
}

size_t str_size(const char* s){
    if (!s) return 0;
    char* r= (char*) s;
    while (*r++!=0){}
    return r-s;
}

char* str_dup(char* s){
    if (!s) return NULL;
    char* d = (char*) calloc(str_size(s), sizeof(char));
    if (d){
        strcpy(d, s);
    }
    return d;
}

void put_str(char* s){
    if (!s) return;
    char c;
    while ((c = *s++) != '\0'){
        putchar(c);
    }
}

char* str_human_bytes(char* buffer, size_t buff_len, f_size bytes){
    if (!buffer) return NULL;
    size_t suff_len = sizeof(BYTE_SFXS)/sizeof(char*);
    size_t i = 0;
    double dbl_bytes = bytes;
    if (bytes > BYTE_DEL){
        while (bytes >= (int) BYTE_DEL && i++ < suff_len-1){
            dbl_bytes = bytes / BYTE_DEL;
            bytes /= (int) BYTE_DEL;
        }
    }
    int n_p = snprintf(buffer, buff_len, "%.02lf %s", dbl_bytes, BYTE_SFXS[i]);  
    if (n_p < 0 || n_p >= buff_len) return NULL;
    return buffer;
}