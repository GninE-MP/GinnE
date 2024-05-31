/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/wasm_args_list.cpp
 *  PURPOSE:     Web assembly args list definition
 *
 *  GninE is available from http://www.sgnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "wasm_args_list.h"
#include "lua/CLuaArguments.h"

cargs* args_create() {
    cargs* args = (cargs*)malloc(sizeof(cargs));

    if (!args) {
        return NULL;
    }

    memset((void*)args, 0, sizeof(cargs));

    args->argsCount = 0;
    args->dataSize = 1;
    args->data = malloc(args->dataSize);
    memset(args->data, 0, args->dataSize);

    return args;
}

void args_delete(cargs* args) {
    if (!args) {
        return;
    }

    if (args->data) {
        free(args->data);
    }

    memset((void*)args, 0, sizeof(cargs));
    
    free((void*)args);
}

size_t args_calculate_arg_heap_size(cargs* args, size_t size, char type) {
    if (!args) {
        return 0;
    }

    size_t syntaxSize = 0;

    switch (type) {
        case C_ARGS_TYPE_ID_CALLABLE:
        case C_ARGS_TYPE_ID_STRING:
            syntaxSize += C_ARGS_STRING_SYNTAX_LENGTH * 2;
            break;
        case C_ARGS_TYPE_ID_LIST:
            syntaxSize += (
                C_ARGS_LIST_OPEN_SYNTAX_LENGTH +
                C_ARGS_LIST_CLOSE_SYNTAX_LENGTH +
                C_ARGS_LIST_ARGC_OPEN_SYNTAX_LENGTH +
                C_ARGS_LIST_ARGC_CLOSE_SYNTAX_LENGTH
            );
            break;
        default:
            break;
    }

    return args->dataSize + size + (args->dataSize == 1 ? -1 : 0) + syntaxSize + C_ARGS_SEPRATOR_LENGTH + C_ARGS_TYPE_IDENTIFIER_LENGTH;
}

void args_push_null(cargs* args) {
    if (!args || !args->data) {
        return;
    }

    char type = C_ARGS_TYPE_ID_NULL;

    size_t lastSize = args->dataSize == 1 ? 0 : args->dataSize;

    size_t size = 0;

    args->argsCount++;
    args->dataSize = args_calculate_arg_heap_size(args, size, type);
    args->data = realloc(args->data, args->dataSize);

    char* data = ((char*)args->data) + lastSize;

    *data = type;
    data += sizeof(type);

    memcpy((void*)data, C_ARGS_SEPRATOR, C_ARGS_SEPRATOR_LENGTH);
}

void args_push_boolean(cargs* args, bool boolean) {
    if (!args || !args->data) {
        return;
    }

    char type = C_ARGS_TYPE_ID_BOOLEAN;

    size_t lastSize = args->dataSize == 1 ? 0 : args->dataSize;

    size_t size = sizeof(bool);

    args->argsCount++;
    args->dataSize = args_calculate_arg_heap_size(args, size, type);
    args->data = realloc(args->data, args->dataSize);

    char* data = ((char*)args->data) + lastSize;

    *data = type;
    data += sizeof(type);

    *data = boolean ? '1' : '0';
    data += sizeof(char);

    memcpy((void*)data, C_ARGS_SEPRATOR, C_ARGS_SEPRATOR_LENGTH);
}

void args_push_number(cargs* args, double number) {
    if (!args || !args->data) {
        return;
    }

    char type = C_ARGS_TYPE_ID_NUMBER;

    size_t lastSize = args->dataSize == 1 ? 0 : args->dataSize;

    char numberBuffer[100];
    size_t size = sprintf(numberBuffer, "%lf", number);

    args->argsCount++;
    args->dataSize = args_calculate_arg_heap_size(args, size, type);
    args->data = realloc(args->data, args->dataSize);

    char* data = ((char*)args->data) + lastSize;
    
    *data = type;
    data += sizeof(type);

    memcpy((void*)data, (void*)numberBuffer, size);
    data += size;

    memcpy((void*)data, C_ARGS_SEPRATOR, C_ARGS_SEPRATOR_LENGTH);
}

void args_push_string(cargs* args, const char* string, size_t size) {
    if (!args || !args->data) {
        return;
    }

    if (!string) {
        size = 0;
    }

    char* cpy = (char*)malloc(size);

    if (size > 0) {
        size_t stringPassCount = 0;

        char* pos = (char*)string;
        
        for (long long i = 0; i < size; i++, pos++) {
            if (strncmp(pos, C_ARGS_STRING_SYNTAX, C_ARGS_STRING_SYNTAX_LENGTH) == 0) {
                size += C_ARGS_STRING_PASS_SYNTAX_LENGTH;

                cpy = (char*)realloc((void*)cpy, size);
                memcpy((void*)(cpy + i), C_ARGS_STRING_PASS_SYNTAX, C_ARGS_STRING_PASS_SYNTAX_LENGTH);

                i += C_ARGS_STRING_PASS_SYNTAX_LENGTH;
            }
            
            cpy[i] = *pos;
        }
    }

    char type = C_ARGS_TYPE_ID_STRING;

    size_t lastSize = args->dataSize == 1 ? 0 : args->dataSize;

    args->argsCount++;
    args->dataSize = args_calculate_arg_heap_size(args, size, type);
    args->data = realloc(args->data, args->dataSize);

    char* data = ((char*)args->data) + lastSize;

    *data = type;
    data += sizeof(type);

    memcpy((void*)data, C_ARGS_STRING_SYNTAX, C_ARGS_STRING_SYNTAX_LENGTH);
    data += C_ARGS_STRING_SYNTAX_LENGTH;

    if (cpy && size > 0) {
        memcpy((void*)data, cpy, size);
        data += size;
    
        free((void*)cpy);
    }

    memcpy((void*)data, C_ARGS_STRING_SYNTAX, C_ARGS_STRING_SYNTAX_LENGTH);
    data += C_ARGS_STRING_SYNTAX_LENGTH;

    memcpy((void*)data, C_ARGS_SEPRATOR, C_ARGS_SEPRATOR_LENGTH);
}

void args_push_callable(cargs* args, uint8_t* string) {
    if (!args || !args->data) {
        return;
    }

    size_t size = C_CALLABLE_HASH_SIZE;

    if (!string) {
        size = 0;
    }

    char* cpy = (char*)malloc(size);

    if (size > 0) {
        size_t stringPassCount = 0;

        char* pos = (char*)string;
        
        for (long long i = 0; i < size; i++, pos++) {
            if (strncmp(pos, C_ARGS_STRING_SYNTAX, C_ARGS_STRING_SYNTAX_LENGTH) == 0) {
                size += C_ARGS_STRING_PASS_SYNTAX_LENGTH;

                cpy = (char*)realloc((void*)cpy, size);
                memcpy((void*)(cpy + i), C_ARGS_STRING_PASS_SYNTAX, C_ARGS_STRING_PASS_SYNTAX_LENGTH);

                i += C_ARGS_STRING_PASS_SYNTAX_LENGTH;
            }
            
            cpy[i] = *pos;
        }
    }

    char type = C_ARGS_TYPE_ID_CALLABLE;

    size_t lastSize = args->dataSize == 1 ? 0 : args->dataSize;

    args->argsCount++;
    args->dataSize = args_calculate_arg_heap_size(args, size, type);
    args->data = realloc(args->data, args->dataSize);

    char* data = ((char*)args->data) + lastSize;

    *data = type;
    data += sizeof(type);

    memcpy((void*)data, C_ARGS_STRING_SYNTAX, C_ARGS_STRING_SYNTAX_LENGTH);
    data += C_ARGS_STRING_SYNTAX_LENGTH;

    if (cpy && size > 0) {
        memcpy((void*)data, cpy, size);
        data += size;
    
        free((void*)cpy);
    }

    memcpy((void*)data, C_ARGS_STRING_SYNTAX, C_ARGS_STRING_SYNTAX_LENGTH);
    data += C_ARGS_STRING_SYNTAX_LENGTH;

    memcpy((void*)data, C_ARGS_SEPRATOR, C_ARGS_SEPRATOR_LENGTH);
}

void args_push_userdata(cargs* args, CWebAssemblyUserData userdata) {
    if (!args || !args->data) {
        return;
    }

    char type = C_ARGS_TYPE_ID_USERDATA;

    size_t lastSize = args->dataSize == 1 ? 0 : args->dataSize;

    char userdataBuffer[100];
    size_t size = sprintf(userdataBuffer, "%d", (CWebAssemblyUserData)userdata);

    args->argsCount++;
    args->dataSize = args_calculate_arg_heap_size(args, size, type);
    args->data = realloc(args->data, args->dataSize);

    char* data = ((char*)args->data) + lastSize;

    *data = type;
    data += sizeof(type);

    memcpy((void*)data, (void*)userdataBuffer, size);
    data += size;

    memcpy((void*)data, C_ARGS_SEPRATOR, C_ARGS_SEPRATOR_LENGTH);
}

void args_push_list(cargs* args, cargs* list) {
    if (!args || !args->data) {
        return;
    }

    size_t size = 0;
    
    if (list && list->data) {
        size = list->dataSize == 1 ? 0 : list->dataSize;
    }

    size_t lastSize = args->dataSize == 1 ? 0 : args->dataSize;

    char type = C_ARGS_TYPE_ID_LIST;

    args->argsCount++;
    args->dataSize = args_calculate_arg_heap_size(args, size, type);

    char argsCountString[12];
    int argsCountStringLength = snprintf(argsCountString, sizeof(argsCountString), "%zu", list->argsCount);
    
    args->dataSize += argsCountStringLength;

    args->data = realloc(args->data, args->dataSize);

    char* data = ((char*)args->data) + lastSize;

    *data = type;
    data += sizeof(type);

    memcpy((void*)data, C_ARGS_LIST_ARGC_OPEN_SYNTAX, C_ARGS_LIST_ARGC_OPEN_SYNTAX_LENGTH);
    data += C_ARGS_LIST_ARGC_OPEN_SYNTAX_LENGTH;

    memcpy((void*)data, argsCountString, argsCountStringLength);
    data += argsCountStringLength;

    memcpy((void*)data, C_ARGS_LIST_ARGC_CLOSE_SYNTAX, C_ARGS_LIST_ARGC_CLOSE_SYNTAX_LENGTH);
    data += C_ARGS_LIST_ARGC_CLOSE_SYNTAX_LENGTH;

    memcpy((void*)data, C_ARGS_LIST_OPEN_SYNTAX, C_ARGS_LIST_OPEN_SYNTAX_LENGTH);
    data += C_ARGS_LIST_OPEN_SYNTAX_LENGTH;

    if (list && list->data && size > 0) {
        memcpy((void*)data, list->data, size);
        data += size;
    }

    memcpy((void*)data, C_ARGS_LIST_CLOSE_SYNTAX, C_ARGS_LIST_CLOSE_SYNTAX_LENGTH);
    data += C_ARGS_LIST_CLOSE_SYNTAX_LENGTH;

    memcpy((void*)data, C_ARGS_SEPRATOR, C_ARGS_SEPRATOR_LENGTH);
}

void args_pop(cargs* args, size_t count) {
    if (!args || !args->data) {
        return;
    }

    if (count > args->argsCount) {
        count = args->argsCount;
    }

    if (count < 1) {
        return;
    }

    if (args->dataSize <= C_ARGS_SEPRATOR_LENGTH) {
        return;
    }

    char* data = (char*)args->data;

    int  openedList = 0;
    bool isInsideString = false;

    for (long long i = args->dataSize - 1 - C_ARGS_SEPRATOR_LENGTH; i >= 0 && count > 0; i--) {
        char* pos = data + i;

        if (isInsideString) {
            if (strncmp(pos, C_ARGS_STRING_SYNTAX, C_ARGS_STRING_SYNTAX_LENGTH) == 0) {
                if (strncmp(pos - 1, C_ARGS_STRING_PASS_SYNTAX, C_ARGS_STRING_PASS_SYNTAX_LENGTH) == 0) {
                    continue;
                }

                isInsideString = false;
            }

            continue;
        }
        
        if (strncmp(pos, C_ARGS_STRING_SYNTAX, C_ARGS_STRING_SYNTAX_LENGTH) == 0) {
            isInsideString = true;
            continue;
        }

        if (openedList > 0) {
            if (strncmp(pos, C_ARGS_LIST_CLOSE_SYNTAX, C_ARGS_LIST_CLOSE_SYNTAX_LENGTH) == 0) {
                openedList++;
            }else if (strncmp(pos, C_ARGS_LIST_OPEN_SYNTAX, C_ARGS_LIST_OPEN_SYNTAX_LENGTH) == 0) {
                openedList--;
            }

            continue;
        }
        
        if (strncmp(pos, C_ARGS_LIST_CLOSE_SYNTAX, C_ARGS_LIST_CLOSE_SYNTAX_LENGTH) == 0) {
            openedList++;
        }

        if (strncmp(pos, C_ARGS_SEPRATOR, C_ARGS_SEPRATOR_LENGTH) == 0) {
            size_t popSize = args->dataSize - i - C_ARGS_SEPRATOR_LENGTH;

            args->argsCount--;
            args->dataSize -= popSize;
            
            if (args->dataSize < 1) {
                args->dataSize = 1;
            }

            memset((void*)(data + args->dataSize), 0, popSize);

            args->data = realloc(args->data, args->dataSize);

            count--;
        }
    }

    if (count > 0) {
        args->argsCount = 0;
        args->dataSize = 1;
        args->data = realloc(args->data, args->dataSize);
        memset(args->data, 0, args->dataSize);
    }
}

long long args_find_arg_start_position(cargs* args, size_t index, size_t from) {
    if (!args || !args->data || args->argsCount == 0) {
        return C_ARGS_INVALID_POSITION;
    }

    if (index > args->argsCount || index < 1) {
        return C_ARGS_INVALID_POSITION;
    }

    if (args->dataSize < C_ARGS_SEPRATOR_LENGTH) {
        return C_ARGS_INVALID_POSITION;
    }

    bool isFirst = index == 1;

    index = args->argsCount - index + 1;

    char* data = (char*)args->data;

    int  openedList = 0;
    bool isInsideString = false;

    long long position = 0;

    for (long long i = args->dataSize - 1 - C_ARGS_SEPRATOR_LENGTH; i >= from && index > 0; i--) {
        char* pos = data + i;

        if (isInsideString) {
            if (strncmp(pos, C_ARGS_STRING_SYNTAX, C_ARGS_STRING_SYNTAX_LENGTH) == 0) {
                if (strncmp(pos - 1, C_ARGS_STRING_PASS_SYNTAX, C_ARGS_STRING_PASS_SYNTAX_LENGTH) == 0) {
                    continue;
                }

                isInsideString = false;
            }

            continue;
        }
        
        if (strncmp(pos, C_ARGS_STRING_SYNTAX, C_ARGS_STRING_SYNTAX_LENGTH) == 0) {
            isInsideString = true;
            continue;
        }

        if (openedList > 0) {
            if (strncmp(pos, C_ARGS_LIST_CLOSE_SYNTAX, C_ARGS_LIST_CLOSE_SYNTAX_LENGTH) == 0) {
                openedList++;
            }else if (strncmp(pos, C_ARGS_LIST_OPEN_SYNTAX, C_ARGS_LIST_OPEN_SYNTAX_LENGTH) == 0) {
                openedList--;
            }

            continue;
        }
        
        if (strncmp(pos, C_ARGS_LIST_CLOSE_SYNTAX, C_ARGS_LIST_CLOSE_SYNTAX_LENGTH) == 0) {
            openedList++;
        }

        if (strncmp(pos, C_ARGS_SEPRATOR, C_ARGS_SEPRATOR_LENGTH) == 0) {
            position = i + C_ARGS_SEPRATOR_LENGTH;

            index--;
        }
    }

    if (index > 0) {
        if (isFirst) {
            return 0;
        }

        return C_ARGS_INVALID_POSITION;
    }

    return position;
}

char args_get_arg_type(cargs* args, size_t index) {
    long long pos = args_find_arg_start_position(args, index, 0);

    if (pos == C_ARGS_INVALID_POSITION) {
        return C_ARGS_TYPE_ID_UNKNOWN;
    }

    return *((char*)args->data + pos);
}

bool args_get_arg_as_boolean(cargs* args, size_t index) {
    bool result = false;

    long long pos = args_find_arg_start_position(args, index, 0);

    if (pos == C_ARGS_INVALID_POSITION) {
        return result;
    }

    char* data = (char*)args->data + pos;

    if (*data != C_ARGS_TYPE_ID_BOOLEAN) {
        return result;
    }

    data += C_ARGS_TYPE_IDENTIFIER_LENGTH;

    result = *data == '1' ? true : false;

    return result;
}

double args_get_arg_as_number(cargs* args, size_t index) {
    double number = 0;

    long long pos = args_find_arg_start_position(args, index, 0);

    if (pos == C_ARGS_INVALID_POSITION) {
        return number;
    }

    char* data = (char*)args->data + pos;

    if (*data != C_ARGS_TYPE_ID_NUMBER) {
        return number;
    }

    data += C_ARGS_TYPE_IDENTIFIER_LENGTH;

    number = strtod(data, NULL);

    return number;
}

char* args_get_arg_as_string(cargs* args, size_t index, size_t* bsize) {
    long long pos = args_find_arg_start_position(args, index, 0);

    if (pos == C_ARGS_INVALID_POSITION) {
        return NULL;
    }

    long long end = args_find_arg_start_position(args, index + 1, pos);

    if (end == C_ARGS_INVALID_POSITION) {
        end = args->dataSize;
    }

    end -= C_ARGS_SEPRATOR_LENGTH;

    char* data = (char*)args->data + pos;

    if (*data != C_ARGS_TYPE_ID_STRING) {
        return NULL;
    }

    data += C_ARGS_TYPE_IDENTIFIER_LENGTH + C_ARGS_STRING_SYNTAX_LENGTH;

    size_t size = end - pos - C_ARGS_SEPRATOR_LENGTH - C_ARGS_STRING_SYNTAX_LENGTH;

    char* str = (char*)malloc(size + 1);
    memset((void*)str, 0, size + 1);

    size_t sSize = size;
    for (size_t i = 0, ci = 0; i < sSize; i++) {
        char* chunk = data + i;

        if (strncmp(chunk, C_ARGS_STRING_PASS_SYNTAX, C_ARGS_STRING_PASS_SYNTAX_LENGTH) == 0) {
            if (strncmp(chunk + C_ARGS_STRING_PASS_SYNTAX_LENGTH, C_ARGS_STRING_SYNTAX, C_ARGS_STRING_SYNTAX_LENGTH) == 0) {
                size -= C_ARGS_STRING_PASS_SYNTAX_LENGTH;
                str = (char*)realloc((void*)str, size);

                continue;
            }
        }
        
        str[ci] = *chunk;
        ci++;
    }

    if (bsize) {
        *bsize = size;
    }

    return str;
}

uint8_t* args_get_arg_as_callable(cargs* args, size_t index)
{
    long long pos = args_find_arg_start_position(args, index, 0);

    if (pos == C_ARGS_INVALID_POSITION) {
        return NULL;
    }

    long long end = args_find_arg_start_position(args, index + 1, pos);

    if (end == C_ARGS_INVALID_POSITION) {
        end = args->dataSize;
    }

    end -= C_ARGS_SEPRATOR_LENGTH;

    char* data = (char*)args->data + pos;

    if (*data != C_ARGS_TYPE_ID_CALLABLE) {
        return NULL;
    }

    data += C_ARGS_TYPE_IDENTIFIER_LENGTH + C_ARGS_STRING_SYNTAX_LENGTH;

    size_t size = end - pos - C_ARGS_SEPRATOR_LENGTH - C_ARGS_STRING_SYNTAX_LENGTH;

    char* str = (char*)malloc(size + 1);
    memset((void*)str, 0, size + 1);

    size_t sSize = size;
    for (size_t i = 0, ci = 0; i < sSize; i++) {
        char* chunk = data + i;

        if (strncmp(chunk, C_ARGS_STRING_PASS_SYNTAX, C_ARGS_STRING_PASS_SYNTAX_LENGTH) == 0) {
            if (strncmp(chunk + C_ARGS_STRING_PASS_SYNTAX_LENGTH, C_ARGS_STRING_SYNTAX, C_ARGS_STRING_SYNTAX_LENGTH) == 0) {
                size -= C_ARGS_STRING_PASS_SYNTAX_LENGTH;
                str = (char*)realloc((void*)str, size);

                continue;
            }
        }
        
        str[ci] = *chunk;
        ci++;
    }

    return (uint8_t*)str;
}

CWebAssemblyUserData args_get_arg_as_userdata(cargs* args, size_t index) {
    long long pos = args_find_arg_start_position(args, index, 0);

    if (pos == C_ARGS_INVALID_POSITION) {
        return NULL;
    }

    char* data = (char*)args->data + pos;

    if (*data != C_ARGS_TYPE_ID_USERDATA) {
        return NULL;
    }

    data += C_ARGS_TYPE_IDENTIFIER_LENGTH;

    if (sizeof(CWebAssemblyUserData) == 8) {
        return (CWebAssemblyUserData)strtoll(data, NULL, 10);
    }

    return (CWebAssemblyUserData)atoi(data);
}

cargs* args_get_arg_as_list(cargs* args, size_t index) {
    long long pos = args_find_arg_start_position(args, index, 0);

    if (pos == C_ARGS_INVALID_POSITION) {
        return NULL;
    }

    long long end = args_find_arg_start_position(args, index + 1, pos);

    if (end == C_ARGS_INVALID_POSITION) {
        end = args->dataSize;
    }

    end -= C_ARGS_SEPRATOR_LENGTH;

    char* data = (char*)args->data + pos;

    if (*data != C_ARGS_TYPE_ID_LIST) {
        return NULL;
    }

    data += C_ARGS_TYPE_IDENTIFIER_LENGTH + C_ARGS_LIST_ARGC_OPEN_SYNTAX_LENGTH;

    uintptr_t lastPlace = (uintptr_t)data;
    size_t argsCount = (size_t)strtol(data, &data, 10);
    uintptr_t argsCountCharLength = (uintptr_t)data - lastPlace;

    data += C_ARGS_LIST_ARGC_CLOSE_SYNTAX_LENGTH + C_ARGS_LIST_OPEN_SYNTAX_LENGTH;

    size_t size = end - pos - C_ARGS_SEPRATOR_LENGTH*2 - C_ARGS_LIST_CLOSE_SYNTAX_LENGTH - argsCountCharLength;

    cargs* list = args_create();

    list->argsCount = argsCount;
    list->dataSize = size < 1 ? 1 : size;
    list->data = malloc(list->dataSize);

    if (size > 0) {
        memcpy(list->data, (void*)data, size);
    }

    return list;
}

void args_read_clua_argument(cargs* args, CLuaArgument* arg)
{
    if (!args || !arg)
    {
        return;
    }

    switch (arg->GetType())
    {
        case LUA_TNIL:
            args_push_null(args);
            break;
        case LUA_TBOOLEAN:
            args_push_boolean(args, arg->GetBoolean());
            break;
        case LUA_TNUMBER:
            args_push_number(args, arg->GetNumber());
            break;
        case LUA_TSTRING:
            args_push_string(args, arg->GetString().c_str(), arg->GetString().size());
            break;
        case LUA_TTABLE:
            {
                cargs* table = args_create();

                CLuaArguments* tableData = arg->GetTableData();

                if (tableData)
                {
                    std::vector<CLuaArgument*>::const_iterator iter = tableData->IterBegin();

                    for (; iter != tableData->IterEnd(); iter++)
                    {
                        args_read_clua_argument(table, *iter);
                    }
                }

                args_push_list(args, table);

                args_delete(table);
            }

            break;
        case LUA_TUSERDATA:
        case LUA_TLIGHTUSERDATA:
            args_push_userdata(args, (CWebAssemblyUserData)arg->GetUserData());
            break;
        case LUA_TFUNCTION:
            {
                CCallable callable = arg->GetCallable();

                uint8_t hash[C_CALLABLE_HASH_SIZE];

                callable.WriteHash(hash);

                args_push_callable(args, hash);
            }
            break;
        default:
            break;
    }
}
