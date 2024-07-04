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

    DEFINE_WASM_FUNCTION(GetSharedPointerAddress, env, args, results);
    DEFINE_WASM_FUNCTION(GetPointerFromSharedPointer, env, args, results);
    DEFINE_WASM_FUNCTION(DoesSharedPointerAddressBelongToThisModule, env, args, results);
    DEFINE_WASM_FUNCTION(DoesSharedPointerAddressBelongToThisResource, env, args, results);
    DEFINE_WASM_FUNCTION(ReadSharedPointerAddress, env, args, results);
    DEFINE_WASM_FUNCTION(WriteSharedPointerAddress, env, args, results);
    DEFINE_WASM_FUNCTION(GetMaxSharedPointerSize, env, args, results);
    DEFINE_WASM_FUNCTION(GetMemoryFreeSpace, env, args, results);

    DEFINE_WASM_FUNCTION(GetTickCount_, env, args, results);
    DEFINE_WASM_FUNCTION(GetRealTime, env, args, results);
    DEFINE_WASM_FUNCTION(GetUserDataType, env, args, results);
    DEFINE_WASM_FUNCTION(GetColorFromString, env, args, results);

    DEFINE_WASM_FUNCTION(GetDistanceBetweenPoints2D, env, args, results);
    DEFINE_WASM_FUNCTION(GetDistanceBetweenPoints3D, env, args, results);
    DEFINE_WASM_FUNCTION(GetEasingValue, env, args, results);
    DEFINE_WASM_FUNCTION(InterpolateBetween, env, args, results);

    DEFINE_WASM_FUNCTION(DebugSleep, env, args, results);

    DEFINE_WASM_FUNCTION(ToColor, env, args, results);
    DEFINE_WASM_FUNCTION(GetProcessMemoryStats, env, args, results);

    DEFINE_WASM_FUNCTION(CallCallable, env, args, results);
    DEFINE_WASM_FUNCTION(IsValidCallable, env, args, results);
    DEFINE_WASM_FUNCTION(GetCallableDeclaration, env, args, results);
    DEFINE_WASM_FUNCTION(CFunctionToCallable, env, args, results);
    DEFINE_WASM_FUNCTION(GetCallableResource, env, args, results);
    DEFINE_WASM_FUNCTION(GetCallableReference, env, args, results);
    DEFINE_WASM_FUNCTION(IsCallableWasmFunction, env, args, results);
    DEFINE_WASM_FUNCTION(FreeCallable, env, args, results);

    DEFINE_WASM_FUNCTION(ArgsCreate, env, args, results);
    DEFINE_WASM_FUNCTION(ArgsDelete, env, args, results);
    DEFINE_WASM_FUNCTION(ArgsPushNull, env, args, results);
    DEFINE_WASM_FUNCTION(ArgsPushBoolean, env, args, results);
    DEFINE_WASM_FUNCTION(ArgsPushNumber, env, args, results);
    DEFINE_WASM_FUNCTION(ArgsPushString, env, args, results);
    DEFINE_WASM_FUNCTION(ArgsPushCallable, env, args, results);
    DEFINE_WASM_FUNCTION(ArgsPushUserdata, env, args, results);
    DEFINE_WASM_FUNCTION(ArgsPushList, env, args, results);
    DEFINE_WASM_FUNCTION(ArgsPop, env, args, results);
    DEFINE_WASM_FUNCTION(ArgsGetArgType, env, args, results);
    DEFINE_WASM_FUNCTION(ArgsGetArgAsBoolean, env, args, results);
    DEFINE_WASM_FUNCTION(ArgsGetArgAsNumber, env, args, results);
    DEFINE_WASM_FUNCTION(ArgsGetArgAsString, env, args, results);
    DEFINE_WASM_FUNCTION(ArgsGetArgAsCallable, env, args, results);
    DEFINE_WASM_FUNCTION(ArgsGetArgAsUserdata, env, args, results);
    DEFINE_WASM_FUNCTION(ArgsGetArgAsList, env, args, results);
    DEFINE_WASM_FUNCTION(ArgsGetArgCount, env, args, results);
    DEFINE_WASM_FUNCTION(ArgsClone, env, args, results);

    DEFINE_WASM_FUNCTION(ExecuteLuaCode, env, args, results);
};
