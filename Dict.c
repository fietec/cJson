/*
    Implementation of a hashmap (aka dictionary) of JsonValues
*/

#include "cJson.h"

unsigned long Dict_hash(char* key);
JsonDictItem* Dict_create_item(char* key, JsonValue* value);
void Dict_free_item(JsonDictItem* item);
JsonDictItem* Dict_duplicate_item(JsonDictItem* item);

JsonDict* JsonDict_create(void){
    JsonDict* dict = (JsonDict*) malloc(sizeof(JsonDict));
    if (dict){
        dict->items = (JsonDictItem**) calloc(DICT_CAPACITY, sizeof(JsonDictItem*));
        if (!dict->items){
            free(dict);
            return NULL;
        }
        dict->size = 0;
    }
    return dict;
}

void JsonDict_free(JsonDict* dict){
    if (dict){
        if (dict->items){
            for (index_t i=0; i<DICT_CAPACITY; i++){
                Dict_free_item(dict->items[i]);
            }
            free(dict->items);
        }
        free(dict);
    }
}

Result JsonDict_insert(JsonDict* dict, char* key, JsonValue* value){
    if (!dict || !dict->items || !key || !value) return r_error;
    index_t index = (index_t) Dict_hash(key);
    JsonDictItem* curr_item = dict->items[index];
    JsonDictItem* new_item = Dict_create_item(key, value);
    if (!new_item) return r_error;
    if (curr_item){
        if (!strcmp(curr_item->key, key)){
            Dict_free_item(curr_item);
            dict->items[index] = new_item;
            return r_success;
        }
        while (curr_item->next){
            if (!strcmp(curr_item->next->key, key)){
                new_item->next = curr_item->next->next;
                Dict_free_item(curr_item->next);
                curr_item->next = new_item;
                return r_success;
            }
            curr_item = curr_item->next;
        }
        curr_item->next = new_item;
    }
    else{
        dict->items[index] = new_item;
    }
    dict->size ++;
    return r_success;
}

JsonValue* JsonDict_get(JsonDict* dict, char* key){
    if (!dict || !dict->items || !key) return NULL;
    JsonDictItem* item = dict->items[Dict_hash(key)];
    while (item){
        if (!strcmp(item->key, key)){
            return item->value;
        }
        item = item->next;
    }
    return NULL;
}

Result JsonDict_remove(JsonDict* dict, char* key){
    if (!dict || !dict->items || !key) return r_error;
    index_t index = Dict_hash(key);
    JsonDictItem* curr_item = dict->items[index];
    if (!curr_item) return r_error;
    if (!strcmp(curr_item->key, key)){
        dict->items[index] = curr_item->next;
        Dict_free_item(curr_item);
        dict->size --;
        return r_success;
    }
    JsonDictItem* last_item = curr_item;
    curr_item = curr_item->next;
    while (curr_item){
        if (!strcmp(curr_item->key, key)){
            last_item->next = curr_item->next;
            Dict_free_item(curr_item);
            dict->size --;
            return r_success;
        }
        last_item = curr_item;
        curr_item = curr_item->next;
    }
    fprintf(stderr, "[ERROR] Could not find this value!\n");
    return r_error;
}

size_t JsonDict_size(JsonDict* dict){
    if (!dict) return 0;
    size_t size = sizeof(JsonDict);
    JsonDictItem* curr_item;
    for (index_t i=0; i<DICT_CAPACITY; i++){
        curr_item = dict->items[i];
        while (curr_item){
            size += (sizeof(JsonDict) + JsonValue_size(curr_item->value) + str_size(curr_item->key));
            curr_item = curr_item->next;
        }
    }
    return size;
}

JsonDict* JsonDict_duplicate(JsonDict* dict){
    if (!dict) return NULL;
    JsonDict* new_dict = (JsonDict*) malloc(sizeof(JsonDict));
    if (new_dict){
        new_dict->items = (JsonDictItem**) calloc(DICT_CAPACITY, sizeof(JsonDictItem*));
        if (!new_dict->items){
            free(new_dict);
            return NULL;
        }
        new_dict->size = dict->size;
        for (size_t i=0; i<DICT_CAPACITY; i++){
            JsonDictItem* new_item = Dict_duplicate_item(dict->items[i]);
            new_dict->items[i] = new_item;
        }
    }
    return new_dict;
}

void JsonDict_print(JsonDict* dict){
    if (!dict) return;
     printf("{\n");
    size_t size = dict->size;
    for (int i=0; i<DICT_CAPACITY; i++){
        JsonDictItem* item = dict->items[i];
        while (item){
            printf("%*c", PRINT_INDENT, ' ');       
            printf("\"%s\": ", item->key);
            cJson_print_value(item->value, 1);
            if (--size > 0){
                printf(",\n");
            } else{
                printf("\n");
            }
            item = item->next;
        }
    }
    printf("}\n");
}

// private functions

unsigned long Dict_hash(char* key) {
    // DJB2 hashing
    unsigned long hash = 5381;
    int c;
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % DICT_CAPACITY;
}

JsonDictItem* Dict_create_item(char* key, JsonValue* value){
    JsonDictItem* item = (JsonDictItem*) malloc(sizeof(JsonDictItem));
    if (item){
        item->key = str_dup(key);
        item->value = value;
        item->next = NULL;
    }
    return item;
}

void Dict_free_item(JsonDictItem* item){
    JsonDictItem* next;
    while (item){
        next = item->next;
        JsonValue_free(item->value);
        free(item->key);
        free(item);
        item = next;
    }
}

JsonDictItem* Dict_duplicate_item(JsonDictItem* item){
    if (!item) return NULL;
    JsonDictItem* new_item = (JsonDictItem*) malloc(sizeof(JsonDictItem));
    if (new_item){
        new_item->key = str_dup(item->key);
        new_item->value = JsonValue_duplicate(item->value);
        new_item->next = Dict_duplicate_item(item->next);
    }
    return new_item;
}