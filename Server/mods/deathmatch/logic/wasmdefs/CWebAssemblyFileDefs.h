/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyFileDefs.h
 *  PURPOSE:     Web assembly definitions base class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CWebAssemblyDefs.h"

class CWebAssemblyFileDefs : public CWebAssemblyDefs
{
public:
    static void RegisterFunctionTypes();
    static void RegisterFunctions(CWebAssemblyScript* script);

    DEFINE_WASM_FUNCTION(FileOpen, env, args, results);
    DEFINE_WASM_FUNCTION(FileCreate, env, args, results);
    DEFINE_WASM_FUNCTION(FileExists, env, args, results);
    DEFINE_WASM_FUNCTION(FileCopy, env, args, results);
    DEFINE_WASM_FUNCTION(FileRename, env, args, results);
    DEFINE_WASM_FUNCTION(FileDelete, env, args, results);

    DEFINE_WASM_FUNCTION(FileClose, env, args, results);
    DEFINE_WASM_FUNCTION(FileFlush, env, args, results);
    DEFINE_WASM_FUNCTION(FileRead, env, args, results);
    DEFINE_WASM_FUNCTION(FileWrite, env, args, results);
    DEFINE_WASM_FUNCTION(FileGetContents, env, args, results);

    DEFINE_WASM_FUNCTION(FileGetPos, env, args, results);
    DEFINE_WASM_FUNCTION(FileGetSize, env, args, results);
    DEFINE_WASM_FUNCTION(FileGetPath, env, args, results);
    DEFINE_WASM_FUNCTION(FileIsEOF, env, args, results);

    DEFINE_WASM_FUNCTION(FileSetPos, env, args, results);
};
