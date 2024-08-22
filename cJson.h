#ifndef _CJSON_H
#define _CJSON_H

#include <string.h>
#include <stdarg.h>
#include <clib/essentials.h>
#include <clib/strlib.h>
#include <clib/pathlib.h>
#include <clib/pathsize.h>

#define DEF_LIST_CAPACITY 8
#define DEF_LIST_RESZ_FACT 2
#define DICT_CAPACITY 32
#define PRINT_INDENT 4

// structure capcable of storing every kind of JSON value
typedef struct JsonValue    JsonValue;
// array list storing JsonValues
typedef struct JsonList     JsonList;
// dictionary storing JsonValues
typedef struct JsonDict     JsonDict;
typedef struct JsonArg      JsonArg;
typedef struct JsonDictItem JsonDictItem;
typedef void*               Null;
typedef struct Pointer      Pointer;

typedef enum{
    t_INTEGER,
    t_FLOAT,
    t_STRING,
    t_BOOLEAN,
    t_LIST,
    t_DICT,
    t_NULL
} ValueType;

const char* const STR_TYPES[] = {
    [t_INTEGER] = "int",
    [t_FLOAT]   = "float",
    [t_BOOLEAN] = "bool",
    [t_NULL]    = "Null",
    [t_STRING]  = "String",
    [t_LIST]    = "JsonList",
    [t_DICT]    = "JsonDict"
};

typedef enum{
    ARG_INDEX,
    ARG_KEY
} ArgType;

struct JsonArg{
    ArgType type;
    union{
        char* key;
        index_t index;
    } value;
    JsonArg* next;
};

struct JsonDictItem{
    JsonValue* value;
    char* key;
    JsonDictItem* next;
};

struct JsonDict{
    JsonDictItem** items;
    size_t size;
};

struct JsonList{
    JsonValue** values;
    size_t capacity;
    size_t size;
};

struct JsonValue{
    ValueType type;
    union{
        JsonDict* dict;
        JsonList* list;
        int integer;
        double floating;
        bool boolean;
        char* string;
        Null null;
    } value;
};

typedef struct{
    JsonValue* first;
} cJSON;

// allocates a cJSON struct and returns the pointer
cJSON*     cJson_create(void);
// frees the memory allocated for a cJSON struct and all its children
void       cJson_free(cJSON* json);
// returns the memory size of a cJSON struct and all its children
size_t     cJson_size(cJSON* json);
/* 
    returns the value stored at a given path
    Usage: cJson_get(<cJSON-object>, <arg count>, <Arg1: Key("key1")>, <Arg2: Index(2)>, ...)
*/
JsonValue* cJson_get(cJSON* json, size_t arg_count, ...);
// prints a visual representation of a cJSON struct into stdout
void       cJson_print(cJSON* json);
// prints a visual representation of a JsonValue into stdout (used by cJson_print, better to use JsonValue_print)
void       cJson_print_value(JsonValue* value, int indent);

// returns a query-arg for cJson_get with an ArgType and its corresponding value (use Key, Index for better readability)
JsonArg*   Arg(ArgType type, void* value);
// macro for creating no warnings when passing an integer
#define    arg(type, value) (Arg(type, (void*)(value)))
// returns a key-query-arg for cJson_get 
JsonArg*   Key(char* key);
// returns a index-query-arg for cJson_get
JsonArg*   Index(index_t index);
// returns the JsonValue stored at the end of an arg-query chain (used by cJson_get)
JsonValue* Args_get(JsonValue* value, JsonArg* args);
// frees the memory allocated for an arg-query chain
void       Args_free(JsonArg* args);

// functions and short-hand macros for creating a JsonValue from a primitive or iterable type

JsonValue* JsonValue_from_null();
JsonValue* JsonValue_from_int(int value);
JsonValue* JsonValue_from_float(double value);
JsonValue* JsonValue_from_bool(bool value);
JsonValue* JsonValue_from_string(char* value);
JsonValue* JsonValue_from_list(JsonList* value);
JsonValue* JsonValue_from_dict(JsonDict* value);

#define    v_dict(dict_val)     (JsonValue_from_dict(dict_val))
#define    v_list(list_val)     (JsonValue_from_list(list_val))
#define    v_string(string_val) (JsonValue_from_string(string_val))
#define    v_int(int_val)       (JsonValue_from_int(int_val))
#define    v_float(float_val)   (JsonValue_from_float(float_val))
#define    v_bool(bool_val)     (JsonValue_from_bool(bool_val))
#define    v_null               (JsonValue_from_null())

// functions for changing the value of a JsonValue (no macros for these since they aren't likely to be inlined)

Result     JsonValue_set_null(JsonValue* value);
Result     JsonValue_set_int(JsonValue* value, int integer);
Result     JsonValue_set_float(JsonValue* value, double floating);
Result     JsonValue_set_bool(JsonValue* value, bool boolean);
Result     JsonValue_set_string(JsonValue* value, char* string);
Result     JsonValue_set_list(JsonValue* value, JsonList* list);
Result     JsonValue_set_dict(JsonValue* value, JsonDict* dict);

// functions and short-hand macros for retreiving the primitive or iterable value stored within a JsonValue

Null       JsonValue_to_null(JsonValue* value);
int        JsonValue_to_int(JsonValue* value);
double     JsonValue_to_float(JsonValue* value); // returns double not float hehe pls don't judge me
bool       JsonValue_to_bool(JsonValue* value);
char*      JsonValue_to_string(JsonValue* value);
JsonList*  JsonValue_to_list(JsonValue* value);
JsonDict*  JsonValue_to_dict(JsonValue* value);

#define    v2dict(val)   (JsonValue_to_dict(val))
#define    v2list(val)   (JsonValue_to_list(val))
#define    v2string(val) (JsonValue_to_string(val))
#define    v2int(val)    (JsonValue_to_int(val))
#define    v2float(val)  (JsonValue_to_float(val))
#define    v2bool(val)   (JsonValue_to_bool(val))
#define    v2null(val)   (JsonValue_to_null(val))

// returns the memory size of JsonValue and all its children
size_t     JsonValue_size(JsonValue* value);
// frees the memory allocated for a JsonValue and all its children
void       JsonValue_free(JsonValue* value);
// prints a visual representation of JsonValue to stdout
void       JsonValue_print(JsonValue* value);
// returns a duplicated JsonValue
JsonValue* JsonValue_duplicate(JsonValue* value);

// allocates a JsonList struct and returns the pointer
JsonList*  JsonList_create(void);
// frees the memory allocated for a JsonList and all its children
void       JsonList_free(JsonList* list);
// appends a JsonValue to a JsonList
Result     JsonList_append(JsonList* list, JsonValue* value);
// returns the JsonValue stored at a given index withing a JsonList
JsonValue* JsonList_get(JsonList* list, index_t index);
// sets the JsonValue at a given index within a JsonList
Result     JsonList_set(JsonList* list, index_t index, JsonValue* value);
// removes and frees the the memory allocated for a JsonValue at a given index within a JsonList
Result     JsonList_remove(JsonList* list, index_t index);
// returns the memory size of a JsonList and all its children
size_t     JsonList_size(JsonList* list);
// returns a duplicated JsonList
JsonList*  JsonList_duplicate(JsonList* list);
// prints a visual representation of a JsonList to stdout
void       JsonList_print(JsonList* list);

// allocates a JsonDict struct and returns the pointer
JsonDict*  JsonDict_create(void);
// frees the memory allocated for a JsonDict and all its children
void       JsonDict_free(JsonDict* dict);
// inserts a key-JsonValue-pair into a JsonDict
Result     JsonDict_insert(JsonDict* dict, char* key, JsonValue* value);
// returns a JsonValue of a corresponding key within a JsonDict
JsonValue* JsonDict_get(JsonDict* dict, char* key);
// removes and frees the memory allocated for a JsonValue of a corresponding key within a JsonDict
Result     JsonDict_remove(JsonDict* dict, char* key);
// returns the memory size of JsonDict and all its children
size_t     JsonDict_size(JsonDict* dict);
// returns a duplicated JsonDict
JsonDict*  JsonDict_duplicate(JsonDict* dict);
// prints a visual representation of a JsonDict to stdout
void       JsonDict_print(JsonDict* dict);

// parses a json files and returns an allocated cJSON representation
cJSON*     cJson_read(char* filename);
// writes a cJSON struct to a file
Result     cJson_write(cJSON* json, char* filename);

#endif // _CJSON_H