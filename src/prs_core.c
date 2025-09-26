#include "prs_core.h"
#include "prs_utils.h"
#include <stdio.h>

static bool prc_parse_list(const char *text, PRS_ConfigValue *out);
static bool prs_parse_object(const char *text, PRS_ConfigValue *out);

static PRS_ValueType prs_autodect_type(const char *text)
{
    size_t len = strlen(text);
    if (len >= 2 && text[0] == '"' && text[len - 1] == '"')
    {
        return TYPE_STR;
    }
    if (strcmp(text, "true") == 0 || strcmp(text, "false") == 0 || strcmp(text, "1") == 0 || strcmp(text, "0") == 0 ||
        strcmp(text, "on") == 0 || strcmp(text, "off") == 0)
    {
        return TYPE_BOOL;
    }
    bool has_dot = strchr(text, '.') != NULL;
    if (strspn(text, "-0123456789.") == len)
    {
        return has_dot ? TYPE_FLOAT : TYPE_INT;
    }
    return TYPE_UNKNOWN;
}

bool PRS_ParseValue(const char *text, PRS_ValueType explicit_type, PRS_ConfigValue *out)
{
    PRS_Trim((char *) text);
    size_t len = strlen(text);
    if (len == 0)
    {
        return false;
    }

    if (text[0] == '[' && text[len - 1] == ']')
    {
        return prc_parse_list(text + 1, out);
    }

    if (text[0] == '{' && text[len - 1] == '}')
    {
        return prs_parse_object(text + 1, out);
    }

    // determine type
    PRS_ValueType t = explicit_type != TYPE_UNKNOWN ? explicit_type : prs_autodect_type(text);
    out->type = t;

    switch (t)
    {
        case TYPE_STR:
        {
            out->string_val = strndup(text + 1, len - 2);
        }
        break;
        case TYPE_INT:
        {
            out->int_val = atoi(text);
        }
        break;
        case TYPE_FLOAT:
        {
            out->float_val = strtof(text, NULL);
        }
        break;
        case TYPE_BOOL:
        {
            if (strcasecmp(text, "true") == 0 || strcasecmp(text, "on") == 0 || strcmp(text, "1") == 0)
            {
                out->bool_val = true;
            }
            else if (strcasecmp(text, "false") == 0 || strcasecmp(text, "off") == 0 || strcmp(text, "0") == 0)
            {
                out->bool_val = false;
            }
            else
            {
                fprintf(stderr, "Invalide boolean value: %s", text);
                return false;
            }
        }
        break;
        default:
        {
            fprintf(stderr, "Unknown scalar value: %s\n", text);
            return false;
        }
        break;
    }
    return true;
}

static bool prc_parse_list(const char *text, PRS_ConfigValue *out)
{
    out->type = TYPE_LIST;
    out->list_val.count = 0;
    size_t capacity = 4;

    out->list_val.items = calloc(capacity, sizeof(PRS_ConfigValue));
    if (!out->list_val.items)
    {
        fprintf(stderr, "Failed to allocate memory for list items");
        return false;
    }

    char *copy = strdup(text);
    if (!copy)
    {
        fprintf(stderr, "Failed to duplicate text");
        return false;
    }
    char *saveptr = NULL;
    char *tok = strtok_r(copy, ",", &saveptr);

    while (tok)
    {
        PRS_Trim(tok);
        if (*tok)
        {
            if (out->list_val.count >= capacity)
            {
                capacity *= 2;
                out->list_val.items = realloc(out->list_val.items, capacity * sizeof(PRS_ConfigValue));
            }
            if (!PRS_ParseValue(tok, TYPE_UNKNOWN, &out->list_val.items[out->list_val.count]))
            {
                fprintf(stderr, "Failed to parse list element: %s\n", tok);
            }
            else
            {
                out->list_val.count++;
            }
        }
        tok = strtok_r(NULL, ",", &saveptr);
    }
    free(copy);
    return true;
}

static bool prs_parse_object(const char *text, PRS_ConfigValue *out)
{
    out->type = TYPE_OBJECT;
    out->object_val.count = 0;
    size_t capacity = 4;
    out->object_val.entries = calloc(capacity, sizeof(PRS_ConfigEntry));

    char *copy = strdup(text);
    char *saveptr = NULL;
    char *line = strtok_r(copy, ";", &saveptr);

    while (line)
    {
        PRS_Trim(line);
        if (*line)
        {
            // Check for explicit type: key:type=value
            char *equal = strchr(line, '=');
            if (!equal)
            {
                line = strtok_r(NULL, ";", &saveptr);
                continue;
            }
            *equal = '\0';
            char *lhs = line;
            char *rhs = equal + 1;
            PRS_Trim(lhs);
            PRS_Trim(rhs);

            char *colon = strchr(lhs, ':');
            PRS_ValueType explicit_type = TYPE_UNKNOWN;
            char *key = lhs;
            if (colon)
            {
                *colon = '\0';
                key = lhs;
                char *type_str = colon + 1;
                PRS_Trim(type_str);
                if (strcasecmp(type_str, "int") == 0)
                    explicit_type = TYPE_INT;
                else if (strcasecmp(type_str, "float") == 0)
                    explicit_type = TYPE_FLOAT;
                else if (strcasecmp(type_str, "bool") == 0)
                    explicit_type = TYPE_BOOL;
                else if (strcasecmp(type_str, "str") == 0)
                    explicit_type = TYPE_STR;
                else if (strcasecmp(type_str, "list") == 0)
                    explicit_type = TYPE_LIST;
                else if (strcasecmp(type_str, "object") == 0)
                    explicit_type = TYPE_OBJECT;
            }

            if (out->object_val.count >= capacity)
            {
                capacity *= 2;
                out->object_val.entries = realloc(out->object_val.entries, capacity * sizeof(PRS_ConfigEntry));
            }

            PRS_ConfigEntry *entry = &out->object_val.entries[out->object_val.count];
            entry->key = strdup(key);
            if (!PRS_ParseValue(rhs, explicit_type, &entry->value))
            {
                fprintf(stderr, "Failed to parse value for key: %s\n", key);
            }
            else
            {
                out->object_val.count++;
            }
        }
        line = strtok_r(NULL, ";", &saveptr);
    }
    free(copy);
    return true;
}

void PRS_DestroyConfigValues(PRS_ConfigValue *val)
{
    if (!val)
        return;

    switch (val->type)
    {
        case TYPE_STR:
            free(val->string_val);
            break;
        case TYPE_LIST:
            for (size_t i = 0; i < val->list_val.count; i++)
                PRS_DestroyConfigValues(&val->list_val.items[i]);
            free(val->list_val.items);
            break;
        case TYPE_OBJECT:
            for (size_t i = 0; i < val->object_val.count; i++)
            {
                free(val->object_val.entries[i].key);
                PRS_DestroyConfigValues(&val->object_val.entries[i].value);
            }
            free(val->object_val.entries);
            break;
        default:
            break;
    }
}
