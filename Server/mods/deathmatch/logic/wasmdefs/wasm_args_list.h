/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/wasm_args_list.h
 *  PURPOSE:     Web assembly args list definition
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "wasm/WebAssemblyImports.h"

class CLuaArgument;

typedef struct
{
    size_t argsCount;
    size_t dataSize;
    void*  data;
} cargs;

cargs*               args_create();
void                 args_delete(cargs* args);
size_t               args_calculate_arg_heap_size(cargs* args, size_t size, char type);
void                 args_push_null(cargs* args);
void                 args_push_boolean(cargs* args, bool boolean);
void                 args_push_number(cargs* args, double number);
void                 args_push_string(cargs* args, const char* string, size_t size);
void                 args_push_callable(cargs* args, uint8_t* string);
void                 args_push_userdata(cargs* args, CWebAssemblyUserData userdata);
void                 args_push_list(cargs* args, cargs* list);
void                 args_pop(cargs* args, size_t count);
long long            args_find_arg_start_position(cargs* args, size_t index, size_t from);
char                 args_get_arg_type(cargs* args, size_t index);
bool                 args_get_arg_as_boolean(cargs* args, size_t index);
double               args_get_arg_as_number(cargs* args, size_t index);
char*                args_get_arg_as_string(cargs* args, size_t index, size_t* bsize);
uint8_t*             args_get_arg_as_callable(cargs* args, size_t index);
CWebAssemblyUserData args_get_arg_as_userdata(cargs* args, size_t index);
cargs*               args_get_arg_as_list(cargs* args, size_t index);
void                 args_read_clua_argument(cargs* args, CLuaArgument* arg);
