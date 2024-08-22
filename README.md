# cJson.h
A simple C library for parsing and manipulating JSON data.
## Features
- **Simple API**: Easy-to-use functions for creating and manipulating JSON objects and arrays.
- **Memory Management**: Functions to manage memory, including freeing JSON structures and calculating their memory size.
- **Flexible Data Structures**: Supports nested JSON objects and arrays.
- **Serialization and Deserialization**: Functions to read JSON data from files and write JSON data to files.
- **Querying**: Retrieve nested JSON values using key or index-based queries.
## How to use
Simply include the header file:
``` c
#include "cJson.h"
```
When compiling please, you have to link with the c files in the ```./src``` folder. 
I know I am wrong for putting you up for that but I simply haven't quite figured out how build systems work and how to distribute such projects.
(Please make pull requests for fixing this if you want to help me :) )

### Creating a cJSON Object

To create a cJSON object, use the cJson_create() function:

```c
cJSON* json = cJson_create();
```
### Manipulating JSON Values

The JsonValue structure supports various types, including integers, floats, strings, booleans, arrays, and dictionaries. You can create a JsonValue from a primitive or iterable type using functions such as JsonValue_from_int(), JsonValue_from_string(), and JsonValue_from_dict().

```c
JsonValue* intValue = JsonValue_from_int(42);
JsonValue* strValue = JsonValue_from_string("Hello, world!");
```
### Accessing Nested Values

Use the cJson_get() function to retrieve a value at a specific path within a cJSON object. The path can be specified using keys for dictionaries or indices for arrays.

```c
JsonValue* value = cJson_get(json, 2, Key("data"), Index(1));
```
### Serializing and Deserializing

You can read JSON data from a file and convert it to a cJSON structure using the cJson_read() function. To write a cJSON structure back to a file, use the cJson_write() function.

```c
cJSON* json = cJson_read("data.json");
Result result = cJson_write(json, "output.json");
```
### Printing JSON Data

The library provides functions to print JSON data to stdout for debugging purposes. Use cJson_print() to print the entire cJSON structure, or JsonValue_print() to print a specific JsonValue.

```c
cJson_print(json);
```
### Memory Management

To free the memory allocated for a cJSON structure and all its children, use the cJson_free() function:

```c
cJson_free(json);
```
### Data Structures
#### JsonValue

Represents any JSON value. It can be one of the following types:

    t_INTEGER for integer values
    t_FLOAT for floating-point values
    t_STRING for string values
    t_BOOLEAN for boolean values
    t_LIST for JSON arrays
    t_DICT for JSON objects
    t_NULL for null values

#### JsonList

Represents a JSON array. It is a dynamically resizable list of JsonValue pointers.
#### JsonDict

Represents a JSON object. It stores key-value pairs where keys are strings and values are JsonValue pointers.

