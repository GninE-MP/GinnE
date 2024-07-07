/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyDatabaseDefs.h
 *  PURPOSE:     Web assembly definitions base class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CWebAssemblyDefs.h"

class CWebAssemblyDatabaseDefs : public CWebAssemblyDefs
{
public:
    static void RegisterFunctionTypes();
    static void RegisterFunctions(CWebAssemblyScript* script);

    DEFINE_WASM_FUNCTION(DbConnect, env, args, results);
    DEFINE_WASM_FUNCTION(DbExec, env, args, results);
    DEFINE_WASM_FUNCTION(DbQuery, env, args, results);
    DEFINE_WASM_FUNCTION(DbFree, env, args, results);
    DEFINE_WASM_FUNCTION(DbPoll, env, args, results);
    DEFINE_WASM_FUNCTION(DbPrepareString, env, args, results);

    DEFINE_WASM_FUNCTION(ExecuteSQLCreateTable, env, args, results);
    DEFINE_WASM_FUNCTION(ExecuteSQLDropTable, env, args, results);
    DEFINE_WASM_FUNCTION(ExecuteSQLDelete, env, args, results);
    DEFINE_WASM_FUNCTION(ExecuteSQLInsert, env, args, results);
    DEFINE_WASM_FUNCTION(ExecuteSQLSelect, env, args, results);
    DEFINE_WASM_FUNCTION(ExecuteSQLUpdate, env, args, results);
    DEFINE_WASM_FUNCTION(ExecuteSQLQuery, env, args, results);
};
