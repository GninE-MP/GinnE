/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyObjectDefs.h
 *  PURPOSE:     Web assembly definitions base class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CWebAssemblyDefs.h"

class CWebAssemblyObjectDefs : public CWebAssemblyDefs
{
public:
    static void RegisterFunctionTypes();
    static void RegisterFunctions(CWebAssemblyScript* script);

    DEFINE_WASM_FUNCTION(CreateObject, env, args, results);

    DEFINE_WASM_FUNCTION(GetObjectRotation, env, args, results);
    DEFINE_WASM_FUNCTION(GetObjectScale, env, args, results);
    DEFINE_WASM_FUNCTION(IsObjectBreakable, env, args, results);

    DEFINE_WASM_FUNCTION(SetObjectRotation, env, args, results);
    DEFINE_WASM_FUNCTION(SetObjectScale, env, args, results);
    DEFINE_WASM_FUNCTION(SetObjectBreakable, env, args, results);
    DEFINE_WASM_FUNCTION(MoveObject, env, args, results);
    DEFINE_WASM_FUNCTION(StopObject, env, args, results);
};
