/*
    Implementation of an array list of JsonValues
*/

#include "cJson.h"

JsonList* JsonList_create(void){
    JsonList* list = (JsonList*) malloc(sizeof(JsonList));
    if (list){
        list->values = (JsonValue**) calloc(DEF_LIST_CAPACITY, sizeof(JsonValue*));
        if (!list->values){
            free(list);
            return NULL;
        }
        list->capacity = DEF_LIST_CAPACITY;
        list->size = 0;
    }
    return list;
}

void JsonList_free(JsonList* list){
    if (list){
        if (list->values){
            for (size_t i=0; i<list->size; i++){
                JsonValue_free(list->values[i]);
            }
            free(list->values);
        }
        free(list);
    }
}

Result JsonList_append(JsonList* list, JsonValue* value){
    if (!list || !value) return r_error;
    if (list->size >= list->capacity){
        size_t new_capacity = list->capacity*DEF_LIST_RESZ_FACT;
        JsonValue** new_values = (JsonValue**) realloc(list->values, new_capacity*sizeof(JsonValue*));
        if (!new_values) return r_error;
        list->values = new_values;
        list->capacity = new_capacity;
    }
    list->values[list->size++] = value;
    return r_success;
}

JsonValue* JsonList_get(JsonList* list, index_t index){
    if (!list || index >= list->size) return NULL;
    return list->values[index];
}

Result JsonList_set(JsonList* list, index_t index, JsonValue* value){
    if  (!list || !value || index >= list->size) return r_error;
    JsonValue_free(list->values[index]);
    list->values[index] = value;
    return r_success;
}

Result JsonList_remove(JsonList* list, index_t index){
    if (!list || index >= list->size) return r_error;
    JsonValue_free(list->values[index]);
    for (size_t i=index; i<list->size-1; i++){
        list->values[i] = list->values[i+1];
    }
    list->values[list->size-1] = NULL;
    return r_success;
}

size_t JsonList_size(JsonList* list){
    if (!list) return 0;
    size_t size = sizeof(JsonList);
    if (list->values){
        for (size_t i=0; i<list->size; i++){
            size += JsonValue_size(list->values[i]);
        }
    }
    return size;
}

void JsonList_print(JsonList* list){
    if (!list) return;
    printf("[\n");
    for (int i=0; i<list->size; i++){
        printf("%*c", PRINT_INDENT, ' ');
        cJson_print_value(list->values[i], 1);
        if ((i+1) == list->size) printf("\n");
        else printf(",\n");
    }
    printf("]\n");
}

JsonList* JsonList_duplicate(JsonList* list){
    if (!list) return NULL;
    JsonList* new_list = (JsonList*) malloc(sizeof(*new_list));
    if (new_list){
        new_list->values = (JsonValue**) calloc(list->capacity, sizeof(JsonValue*));
        if (!new_list->values){
            free(new_list);
            return NULL;
        }
        new_list->capacity = list->capacity;
        new_list->size = list->size;
        for (size_t i=0; i<list->size; i++){
            new_list->values[i] = JsonValue_duplicate(list->values[i]);
        }
    }
    return new_list;
}