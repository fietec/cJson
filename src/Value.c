/*
    Functions for handling JsonValues
*/

#include "../cJson.h"

void JsonValue_clear(JsonValue* value);

JsonValue* JsonValue_from_null() {
    JsonValue* new_value = (JsonValue*) malloc(sizeof(JsonValue));
    if (!new_value) return NULL;
    new_value->value.null = NULL;
    new_value->type = t_NULL;
    return new_value;
}

JsonValue* JsonValue_from_int(int value) {
    JsonValue* new_value = (JsonValue*) malloc(sizeof(JsonValue));
    if (!new_value) return NULL;
    new_value->value.integer = value;
    new_value->type = t_INTEGER;
    return new_value;
}

JsonValue* JsonValue_from_float(double value) {
    JsonValue* new_value = (JsonValue*) malloc(sizeof(JsonValue));
    if (!new_value) return NULL;
    new_value->value.floating = value;
    new_value->type = t_FLOAT;
    return new_value;
}

JsonValue* JsonValue_from_bool(bool value) {
    JsonValue* new_value = (JsonValue*) malloc(sizeof(JsonValue));
    if (!new_value) return NULL;
    new_value->value.boolean = value;
    new_value->type = t_BOOLEAN;
    return new_value;
}

JsonValue* JsonValue_from_string(char* value) {
    JsonValue* new_value = (JsonValue*) malloc(sizeof(JsonValue));
    if (!new_value) return NULL;
    new_value->value.string = str_dup(value);
    new_value->type = t_STRING;
    return new_value;
}

JsonValue* JsonValue_from_list(JsonList* value) {
    JsonValue* new_value = (JsonValue*) malloc(sizeof(JsonValue));
    if (!new_value) return NULL;
    new_value->value.list = value;
    new_value->type = t_LIST;
    return new_value;
}

JsonValue* JsonValue_from_dict(JsonDict* value) {
    JsonValue* new_value = (JsonValue*) malloc(sizeof(JsonValue));
    if (!new_value) return NULL;
    new_value->value.dict = value;
    new_value->type = t_DICT;
    return new_value;
}

Result JsonValue_set_null(JsonValue* value){
    if (!value) return r_error;
    JsonValue_clear(value);
    value->type = t_NULL;
    value->value.null = NULL;
    return r_success;
}

Result JsonValue_set_int(JsonValue* value, int integer){
    if (!value) return r_error;
    JsonValue_clear(value);
    value->type = t_INTEGER;
    value->value.integer = integer;
    return r_success;
}

Result JsonValue_set_float(JsonValue* value, double floating){
    if (!value) return r_error;
    JsonValue_clear(value);
    value->type = t_FLOAT;
    value->value.floating = floating;
    return r_success;
}

Result JsonValue_set_bool(JsonValue* value, bool boolean){
    if (!value) return r_error;
    JsonValue_clear(value);
    value->type = t_BOOLEAN;
    value->value.boolean = boolean;
    return r_success;
}

Result JsonValue_set_string(JsonValue* value, char* string){
    if (!value || !string) return r_error;
    JsonValue_clear(value);
    value->type = t_STRING;
    value->value.string = str_dup(string);
    return r_success;
}

Result JsonValue_set_list(JsonValue* value, JsonList* list){
    if (!value || !list) return r_error;
    JsonValue_clear(value);
    value->type = t_LIST;
    value->value.list = list;
    return r_success;
}

Result JsonValue_set_dict(JsonValue* value, JsonDict* dict){
    if (!value || !dict) return r_error;
    JsonValue_clear(value);
    value->type = t_DICT;
    value->value.dict = dict;
    return r_success;
}

Null JsonValue_to_null(JsonValue* value) {
    return NULL;
}

int JsonValue_to_int(JsonValue* value) {
    if (!value) return 0;
    if (value->type != t_INTEGER) {
        fprintf(stderr, "[ERROR] Could not extract integer from JsonValue of type %s!\n", STR_TYPES[value->type]);
        return 0;
    }
    return value->value.integer;
}

double JsonValue_to_float(JsonValue* value) {
    if (!value) return 0.0;
    if (value->type != t_FLOAT) {
        fprintf(stderr, "[ERROR] Could not extract float from JsonValue of type %s!\n", STR_TYPES[value->type]);
        return 0.0;
    }
    return value->value.floating;
}

bool JsonValue_to_bool(JsonValue* value) {
    if (!value) return false;
    if (value->type != t_BOOLEAN) {
        fprintf(stderr, "[ERROR] Could not extract boolean from JsonValue of type %s!\n", STR_TYPES[value->type]);
        return false;
    }
    return value->value.boolean;
}

char* JsonValue_to_string(JsonValue* value) {
    if (!value) return NULL;
    if (value->type != t_STRING) {
        fprintf(stderr, "[ERROR] Could not extract String from JsonValue of type %s!\n", STR_TYPES[value->type]);
        return NULL;
    }
    return value->value.string;
}

JsonList* JsonValue_to_list(JsonValue* value) {
    if (!value) return NULL;
    if (value->type != t_LIST) {
        fprintf(stderr, "[ERROR] Could not extract JsonList from JsonValue of type %s!\n", STR_TYPES[value->type]);
        return NULL;
    }
    return value->value.list;
}

JsonDict* JsonValue_to_dict(JsonValue* value) {
    if (!value) return NULL;
    if (value->type != t_DICT) {
        fprintf(stderr, "[ERROR] Could not extract JsonDict from JsonValue of type %s!\n", STR_TYPES[value->type]);
        return NULL;
    }
    return value->value.dict;
}

JsonValue* JsonValue_get(JsonValue* value, size_t arg_count, ...){
    if (!value || !(value->type == t_DICT || value->type == t_LIST) || arg_count == 0) return NULL;
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
    JsonValue* found_value = Args_get(value, first_arg);
    if (!found_value){
        fprintf(stderr, "[ERROR] Could not find any value!\n");
    }
    Args_free(first_arg);
    return found_value;
}

JsonValue* JsonValue_duplicate(JsonValue* value){
    if (!value) return NULL;
    JsonValue* new_value = (JsonValue*) malloc(sizeof(JsonValue));
    if (new_value){
        new_value->type = value->type;
        if (new_value->type == t_LIST){
            new_value->value.list = JsonList_duplicate(value->value.list);
        }
        else if (new_value->type == t_DICT){
            new_value->value.dict = JsonDict_duplicate(value->value.dict);
        }
        else if (new_value->type == t_STRING){
            new_value->value.string = str_dup(value->value.string);
        }
        else{
            new_value->value = value->value;
        }
    }
    return new_value;
}

size_t JsonValue_size(JsonValue* value){
    if (!value) return 0;
    size_t size = sizeof(JsonValue);
    switch(value->type){
        case t_LIST:{
            size += JsonList_size(value->value.list);
        } break;
        case t_DICT:{
            size += JsonDict_size(value->value.dict);
        } break;
        case t_STRING:{
            size += str_size(value->value.string);
        } break;
    }
    return size;
}

void JsonValue_clear(JsonValue* value){
    if (!value) return;
    switch (value->type){
        case t_LIST:{
            JsonList_free(value->value.list);
        } break;
        case t_DICT:{
            JsonDict_free(value->value.dict);
        } break;
        case t_STRING:{
            free(value->value.string);
        } break;
    }
    memset(&value->value, 0, sizeof(value->value));
}

void JsonValue_free(JsonValue* value){
    if (!value) return;
    switch(value->type){
        case t_LIST:{
            JsonList_free(value->value.list);
        } break;
        case t_DICT:{
            JsonDict_free(value->value.dict);
        } break;
        case t_STRING:{
            free(value->value.string);
        } break;
    }
    free(value);
}

void JsonValue_print(JsonValue* value){
    if (value){
        cJson_print_value(value, 0);
        putchar('\n');
    }
}