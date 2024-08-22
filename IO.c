/*
    Functions for parsing and writing JSON files
*/

#include "cJson.h"

typedef struct{
    JsonValue* value;
    index_t index;
} ParsedValue;

index_t index_after_space(char* buffer, index_t start);
ParsedValue cJson_parse_value(char* buffer, index_t index);
void cJson_print_value(JsonValue* value, int indent);
void cJson_fprint_value(FILE* file, JsonValue* value, int indent);

Result cJson_write(cJSON* json, char* filename){
    if (!json || !filename) return r_error;
    FILE* file = fopen(filename, "w");
    if (!file) return r_error;
    cJson_fprint_value(file, json->first, 0);
    fclose(file);
    return r_success;
}

cJSON* cJson_read(char* filename){
    if (!path_isfile(filename)) return NULL;
    f_size size = file_size(filename);
    FILE* file = fopen(filename, "r");
    if (!file) return NULL;
    char* f_content = (char*) calloc(size+1, sizeof(char));
    if (!f_content){
        fclose(file);
        return NULL;
    }
    char c;
    char* w = f_content;
    for (f_size i=0; i<size; i++){
        c = fgetc(file);
        switch (c){
            case '\n':{
                continue;
            } break;
            case EOF:{
                i=size; 
            } break;
            default:{
                *w++ = c;
            }
        }
    }
    *w = '\0';
    ParsedValue val = cJson_parse_value(f_content, 0);
    if (!val.value) {
        fclose(file);
        free(f_content);
        return NULL;
    }
    cJSON* json = cJson_create();
    json->first = val.value;
    return json;
}

void cJson_print(cJSON* json){
    if (!json) return;
    cJson_print_value(json->first, 0);
}

ParsedValue cJson_parse_list(char* buffer, index_t index){
    if (!buffer)return (ParsedValue) {0};
    JsonList* list = JsonList_create();
    if (!list) return (ParsedValue) {0};
    bool loop = true;
    bool sep_found = false;
    while (loop){
        switch (buffer[index]){
            case 0x9: break;
            case ' ': break;
            case ',':{
                if (sep_found){
                    fprintf(stderr, "[ERROR] Expected value, found ','!\n");
                    goto list_parse_error;
                }
                sep_found = true;
            }break;
            case ']':{
                if (sep_found){
                    fprintf(stderr, "[ERROR] Expected value, found ']'!\n");
                    goto list_parse_error;
                }
                loop = false;
            } break;
            case '}':{
                if (sep_found){
                    fprintf(stderr, "[ERROR] Expected value, found '}'!\n");
                    goto list_parse_error;
                }
                fprintf(stderr, "[ERROR] Expected closing of list (']') but found closing of dict ('}')!\n");
                goto list_parse_error;
            } break;
            default:{
                ParsedValue p_val = cJson_parse_value(buffer, index);
                if (!p_val.value) goto list_parse_error;
                index = p_val.index-1;
                JsonList_append(list, p_val.value);
                sep_found = false;
            }
        }
        index ++;
    }
    JsonValue* value = JsonValue_from_list(list);
    if (!value) goto list_parse_error;
    return (ParsedValue) {.value=value, .index=index};
    list_parse_error:
        JsonList_free(list);
        return (ParsedValue) {0};
}

ParsedValue cJson_parse_dict(char* buffer, index_t index){
    if (!buffer) return (ParsedValue) {0};
    JsonDict* dict = JsonDict_create();
    if (!dict) return (ParsedValue) {0};
    
    bool loop = true;
    bool sep_found = false;
    bool kv_sep_found = false;
    bool key_found = false;
    char* key;
    while (loop){
        switch (buffer[index]){
            case 0x9: break;
            case ' ': break;
            case ',':{
                if (sep_found){
                    fprintf(stderr, "[ERROR] Expected value, found ','!\n");
                    goto dict_parse_error;
                }
                if (key_found && !kv_sep_found){
                    fprintf(stderr, "[ERROR] Expected ':', found ','!\n");
                    goto dict_parse_error;
                }
                sep_found = true;
            }break;
            case ':':{
                if (kv_sep_found){
                    fprintf(stderr, "[ERROR] Expected value, found ':'!\n");
                    goto dict_parse_error;
                }
                if (!key_found){
                    fprintf(stderr, "[ERROR] Expected key, found ':'!\n");
                    goto dict_parse_error;
                }
                kv_sep_found = true;
            }break;
            case '}':{
                if (sep_found){
                    fprintf(stderr, "[ERROR] Expected key, found '}'!\n");
                    goto dict_parse_error;
                }
                if (key_found && !kv_sep_found){
                    fprintf(stderr, "[ERROR] Expected ':', found '}'!\n");
                    goto dict_parse_error;
                }
                if (kv_sep_found){
                    fprintf(stderr, "[ERROR] Expected value, found '}'!\n");
                    goto dict_parse_error;
                }
                loop = false;
            } break;
            case ']':{
                if (sep_found){
                    fprintf(stderr, "[ERROR] Expected key, found ']'!\n");
                    goto dict_parse_error;
                }
                if (key_found && !kv_sep_found){
                    fprintf(stderr, "[ERROR] Expected ':', found ']'!\n");
                    goto dict_parse_error;
                }
                if (kv_sep_found){
                    fprintf(stderr, "[ERROR] Expected value, found ']'!\n");
                    goto dict_parse_error;
                }
                fprintf(stderr, "[ERROR] Expected closing of dict ('}'), found closing of list (']')!\n");
                goto dict_parse_error;
            } break;
            default:{
                ParsedValue p_val = cJson_parse_value(buffer, index);
                JsonValue* value = p_val.value;
                if (!value) goto dict_parse_error;
                if (!kv_sep_found){
                    // value must be key
                    if (value->type != t_STRING){
                        fprintf(stderr, "[ERROR] Expected key, found value of type %d!\n", value->type);
                        JsonValue_free(value);
                        goto dict_parse_error;
                    }
                    key = str_dup(value->value.string);
                    JsonValue_free(value);
                    key_found = true;
                } else if (key != NULL){
                    JsonDict_insert(dict, key, p_val.value);
                    sep_found = false;
                    kv_sep_found = false;
                    key_found = false;
                    free(key);
                    key = NULL;
                }
                index = p_val.index-1;
            }
        }
        index ++;
    }
    JsonValue* value = JsonValue_from_dict(dict);
    if (!value) goto dict_parse_error;
    free(key);
    return (ParsedValue) {.value=value, .index=index};
    dict_parse_error:
        free(key);
        JsonDict_free(dict);
        return (ParsedValue) {0};
}

ParsedValue cJson_parse_value(char* buffer, index_t index){
    if (!buffer) return (ParsedValue){0};
    index = index_after_space(buffer, index);
    if (buffer[index] == '{'){
        // value is dict
        return cJson_parse_dict(buffer, index+1);
    }
    else if (buffer[index] == '['){
        // value is list 
        return cJson_parse_list(buffer, index+1);
    }
    else{
        // value is primitive (string, int, float, boolean, null)
        bool is_string = false;
        char* r = buffer + index;
        if (*r == '"') {
            is_string = true;
            index++;
            r++;
        }
        char c;
        while ((c = *r) != '\0'){
            if (is_string){
                if (c == '"') break;
            }
            else if (c == ' ' || c == '}' || c == ']' || c == ',' || c == 0x9){
                break;
            }
            r++;
        }
        size_t value_len = r-buffer-index;
        char value_buff[value_len+1];
        strncpy(value_buff, buffer+index, value_len);
        value_buff[value_len] = '\0';
        JsonValue* value;
        if (is_string) value = JsonValue_from_string(value_buff);
        else if (str_is_int(value_buff)) value = JsonValue_from_int(atoi(value_buff));
        else if (str_is_float(value_buff)) value = JsonValue_from_float(strtod(value_buff, NULL));
        else if (!strcmp(value_buff, "true")) value = JsonValue_from_bool(true);
        else if (!strcmp(value_buff, "false")) value = JsonValue_from_bool(false);
        else if (!strcmp(value_buff, "null")) value = JsonValue_from_null();
        else{
            fprintf(stderr, "[ERROR] This is no valid value: '%s'!\n", value_buff);
            return (ParsedValue) {0};
        }
        return (ParsedValue){.value = value, .index=r-buffer+((is_string)? 1:0)};
    }
}

void cJson_print_value(JsonValue* value, int indent){
    if (!value){
		puts("--empty--");
		return;
	}
    switch (value->type){
        case t_INTEGER:
            printf("%d", value->value.integer);
            break;
        case t_FLOAT:
            printf("%lf", value->value.floating);
            break;
        case t_BOOLEAN:
            printf("%s", S_Bool(value->value.boolean));
            break;
        case t_STRING:
            printf("\"%s\"", value->value.string);
            break;
        case t_NULL:
            printf("null");
            break;
        case t_LIST:{
            printf("[\n");
            JsonList* list = value->value.list;
            for (int i=0; i<list->size; i++){
                for (int j= 0; j <= indent; j++) {
                    printf("%*c", PRINT_INDENT, ' ');
                }
                cJson_print_value(list->values[i], indent+1);
                if ((i+1) == list->size) printf("\n");
                else printf(",\n");
            }
            for (int i = 0; i < indent; i++) {
                printf("%*c", PRINT_INDENT, ' ');
            }
            putchar(']');
        } break;
        case t_DICT:{
            printf("{\n");
            JsonDict* dict = value->value.dict;
            size_t size = dict->size;
            for (int i=0; i<DICT_CAPACITY; i++){
                JsonDictItem* item = dict->items[i];
                while (item){
                    for (int j=0; j<indent+1; j++){
                        printf("%*c", PRINT_INDENT, ' ');
                    }
                    printf("\"%s\": ", item->key);
                    cJson_print_value(item->value, indent+1);
                    if (--size > 0){
                        printf(",\n");
                    } else{
                        printf("\n");
                    }
                    item = item->next;
                }
            }
            for (int i=0; i<indent; i++){
                printf("%*c", PRINT_INDENT, ' ');
            }
            putchar('}');
        } break;
    }
}

void cJson_fprint_value(FILE* file, JsonValue* value, int indent){
	if (!value) return;
    switch (value->type) {
        case t_INTEGER:
            fprintf(file, "%d", value->value.integer);
            break;
        case t_FLOAT:
            fprintf(file, "%lf", value->value.floating);
            break;
        case t_BOOLEAN:
            fprintf(file, "%s", S_Bool(value->value.boolean));
            break;
        case t_STRING:
            fprintf(file, "\"%s\"", value->value.string);
            break;
        case t_NULL:
            fprintf(file, "null");
            break;
        case t_LIST: {
            fprintf(file, "[\n");
            JsonList* list = value->value.list;
            for (int i = 0; i < list->size; i++) {
                for (int j = 0; j <= indent; j++) {
                    fprintf(file, "%*c", PRINT_INDENT, ' ');
                }
                cJson_fprint_value(file, list->values[i], indent + 1);
                if ((i + 1) == list->size) fprintf(file, "\n");
                else fprintf(file, ",\n");
            }
            for (int i = 0; i < indent; i++) {
                fprintf(file, "%*c", PRINT_INDENT, ' ');
            }
            fputc(']', file);
        } break;
        case t_DICT: {
            fprintf(file, "{\n");
            JsonDict* dict = value->value.dict;
            size_t size = dict->size;
            for (int i = 0; i < DICT_CAPACITY; i++) {
                JsonDictItem* item = dict->items[i];
                while (item) {
                    for (int j = 0; j < indent + 1; j++) {
                        fprintf(file, "%*c", PRINT_INDENT, ' ');
                    }
                    fprintf(file, "\"%s\": ", item->key);
                    cJson_fprint_value(file, item->value, indent + 1);
                    if (--size > 0) {
                        fprintf(file, ",\n");
                    } else {
                        fprintf(file, "\n");
                    }
                    item = item->next;
                }
            }
            for (int i = 0; i < indent; i++) {
                fprintf(file, "%*c", PRINT_INDENT, ' ');
            }
            fputc('}', file);
        } break;
    }
}


index_t index_after_space(char* buffer, index_t start){
    if (!buffer) return -1;
    char* r = buffer + start;
    while (*r++ == ' '){}
    return r-buffer-1;
}