/*
    Functions for working with the toplevel cJSON struct
*/

#include "../cJson.h"

cJSON* cJson_create(void){
    cJSON* json = (cJSON*) calloc(1, sizeof(cJSON));
    return json;
}

void cJson_free(cJSON* json){
    if (json){
        if (json->first) JsonValue_free(json->first);
        free(json);
    }
}

size_t cJson_size(cJSON* json){
    if (!json) return 0;
    return sizeof(cJSON) + JsonValue_size(json->first);
}

JsonValue* cJson_get(cJSON* json, size_t arg_count, ...){
    if (!json || arg_count == 0) return NULL;
    va_list args;
    va_start(args, arg_count);
    JsonArg* first_arg;
    JsonArg* cur_arg;
    JsonArg* new_arg;
    for (index_t i=0; i<arg_count; i++){
        new_arg = va_arg(args, JsonArg*);
        if (!new_arg) return NULL;
        if (i==0) first_arg = new_arg;
        else cur_arg->next = new_arg;
        cur_arg = new_arg;
    }
    va_end(args);
    JsonValue* value = Args_get(json->first, first_arg);
    if (!value){
        fprintf(stderr, "[ERROR] Could not find any value!\n");
    }
    Args_free(first_arg);
    return value;
}
/*
    printing functions are located in IO.c
*/