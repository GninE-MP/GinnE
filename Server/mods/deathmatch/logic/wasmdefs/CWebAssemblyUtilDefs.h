/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyUtilDefs.h
 *  PURPOSE:     Web assembly definitions base class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CWebAssemblyDefs.h"

class CWebAssemblyUtilDefs : public CWebAssemblyDefs
{
public:
    static void RegisterFunctionTypes();
    static void RegisterFunctions(CWebAssemblyScript* script);

    DEFINE_WASM_FUNCTION(PrintData, env, args, results);
    DEFINE_WASM_FUNCTION(GetTickCount_, env, args, results);
    DEFINE_WASM_FUNCTION(GetRealTime, env, args, results);
    DEFINE_WASM_FUNCTION(GetUserDataType, env, args, results);
    DEFINE_WASM_FUNCTION(GetColorFromString, env, args, results);

    DEFINE_WASM_FUNCTION(GetDistanceBetweenPoints2D, env, args, results);
    DEFINE_WASM_FUNCTION(GetDistanceBetweenPoints3D, env, args, results);
    DEFINE_WASM_FUNCTION(GetEasingValue, env, args, results);
    DEFINE_WASM_FUNCTION(InterpolateBetween, env, args, results);

    DEFINE_WASM_FUNCTION(PregFind, env, args, results);
    DEFINE_WASM_FUNCTION(PregReplace, env, args, results);
    DEFINE_WASM_FUNCTION(PregMatch, env, args, results);

    DEFINE_WASM_FUNCTION(DebugSleep, env, args, results);

    DEFINE_WASM_FUNCTION(ToColor, env, args, results);
    DEFINE_WASM_FUNCTION(GetProcessMemoryStats, env, args, results);
};
