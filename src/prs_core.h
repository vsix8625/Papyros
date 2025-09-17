#pragma GCC system_header
#ifndef PRS_CORE_H
#define PRS_CORE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef enum
{
    TYPE_UNKNOWN,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_BOOL,
    TYPE_STR,
    TYPE_LIST,
    TYPE_OBJECT,
} PRS_ValueType;

typedef struct ConfigValue
{
    int32_t int_val;
    float float_val;
    bool bool_val;
    char *string_val;
    struct
    {
        size_t count;
        struct ConfigValue *items;
    } list_val;
} ConfigValue;

#endif  // PRS_CORE_H
