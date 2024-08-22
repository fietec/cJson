/*
    Functions for handling arg-querys for cJson_get
*/

#include "cJson.h"

JsonArg* Arg(ArgType type, void* value){
    JsonArg* arg = (JsonArg*) malloc(sizeof(JsonArg));
    if (arg){
        arg->type = type;
        arg->next = NULL;
        if (type == ARG_KEY) arg->value.key = str_dup(value);
        else arg->value.index = (int) value;
    }
    return arg;
}

JsonArg* Index(index_t index){
    JsonArg* arg = (JsonArg*) malloc(sizeof(JsonArg));
    if (arg){
        arg->type = ARG_INDEX;
        arg->next = NULL;
        arg->value.index = index;
    }
    return arg;
}

JsonArg* Key(char* key){
    JsonArg* arg = (JsonArg*) malloc(sizeof(JsonArg));
    if (arg){
        arg->type = ARG_KEY;
        arg->next = NULL;
        arg->value.key = str_dup(key);
    }
    return arg;
}

JsonValue* Args_get(JsonValue* value, JsonArg* arg){
    if (!value || !arg) return NULL;
    if (value->type == t_DICT && arg->type == ARG_KEY){
        JsonValue* next = JsonDict_get(value->value.dict, arg->value.key);
        if (!next) return NULL;
        if (arg->next) return Args_get(next, arg->next);
        return next;
    }
    else if (value->type == t_LIST && arg->type == ARG_INDEX){
        JsonValue* next = JsonList_get(value->value.list, arg->value.index);
        if (!next) return NULL;
        if (arg->next) return Args_get(next, arg->next);
        return next;
    }
    return NULL;
}

void Args_free(JsonArg* args){
    JsonArg* next;
    while (args){
        next = args->next;
        if (args->type == ARG_KEY) free(args->value.key);
        free(args);
        args = next;
    }
}