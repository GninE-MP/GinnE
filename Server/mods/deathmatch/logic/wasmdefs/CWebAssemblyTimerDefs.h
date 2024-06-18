/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyTimerDefs.h
 *  PURPOSE:     Web assembly definitions base class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CWebAssemblyDefs.h"

class CWebAssemblyTimerDefs : public CWebAssemblyDefs
{
public:
    static void RegisterFunctionTypes();
    static void RegisterFunctions(CWebAssemblyScript* script);

    DEFINE_WASM_FUNCTION(SetTimer, env, args, results);
    DEFINE_WASM_FUNCTION(KillTimer, env, args, results);
    DEFINE_WASM_FUNCTION(ResetTimer, env, args, results);
    DEFINE_WASM_FUNCTION(GetTimers, env, args, results);
    DEFINE_WASM_FUNCTION(IsTimer, env, args, results);
    DEFINE_WASM_FUNCTION(GetTimerDetails, env, args, results);
};
