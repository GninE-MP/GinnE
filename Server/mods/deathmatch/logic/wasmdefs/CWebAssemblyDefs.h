/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyDefs.h
 *  PURPOSE:     Web assembly definitions base class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include <unordered_map>

#include "../wasm/WebAssemblyImports.h"

#ifndef GninE_CLIENT
    #include "CGame.h"
#endif

#define DEFINE_WASM_FUNCTION(functionName, envName, argsName, resultsName) static WebAssemblyApi(functionName, envName, argsName, resultsName)

#define WASM_REGISTER_API(script, functions) \
{ \
    for (const std::pair<SString, CWebAssemblyCFunction>& function : functions) \
    { \
        script->RegisterApiFunction(GetApiName(function.first), GetFunctionType(function.first), function.second); \
    } \
}

class CWebAssemblyVariables;
class CWebAssemblyFunctionType;
class CWebAssemblyScript;

typedef std::unordered_map<SString, CWebAssemblyCFunction> CWebAssemblyCFunctionMap;

class CWebAssemblyDefs
{
public:
    static void RegisterApiFunctionTypes();

    static void RegisterApi(CWebAssemblyScript* script);

    static void                     SetFunctionType(const SString& functionName, CWebAssemblyFunctionType functionType);
    static void                     SetFunctionType(const SString& functionName, const SString& structure);
    static CWebAssemblyFunctionType GetFunctionType(const SString& functionName);

    static bool ExistsFunctionType(const SString& functionName);

    static SString GetApiName(const SString& functionName);

private:
    static void PushType(CWebAssemblyVariables* vars, const char& flag);
};
