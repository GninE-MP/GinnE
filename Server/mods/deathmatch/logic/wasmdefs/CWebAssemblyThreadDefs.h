/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyThreadDefs.h
 *  PURPOSE:     Web assembly definitions base class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CWebAssemblyDefs.h"

class CWebAssemblyThreadDefs : public CWebAssemblyDefs
{
public:
    static void RegisterFunctionTypes();
    static void RegisterFunctions(CWebAssemblyScript* script);

    DEFINE_WASM_FUNCTION(CreateWorker, env, args, results);
    DEFINE_WASM_FUNCTION(TerminateWorker, env, args, results);
    DEFINE_WASM_FUNCTION(RunWorker, env, args, results);
    DEFINE_WASM_FUNCTION(JoinWorker, env, args, results);
    DEFINE_WASM_FUNCTION(GetCurrentWorker, env, args, results);
    DEFINE_WASM_FUNCTION(GetMainWorker, env, args, results);
    DEFINE_WASM_FUNCTION(SleepWorker, env, args, results);
    DEFINE_WASM_FUNCTION(GetWorkerState, env, args, results);
    DEFINE_WASM_FUNCTION(IsWorker, env, args, results);

    DEFINE_WASM_FUNCTION(CreateMutex_, env, args, results);
    DEFINE_WASM_FUNCTION(DestroyMutex_, env, args, results);
    DEFINE_WASM_FUNCTION(LockMutex_, env, args, results);
    DEFINE_WASM_FUNCTION(UnlockMutex_, env, args, results);
    DEFINE_WASM_FUNCTION(IsMutex_, env, args, results);

    DEFINE_WASM_FUNCTION(CreateThread, env, args, results);
    DEFINE_WASM_FUNCTION(TerminateThread, env, args, results);
    //DEFINE_WASM_FUNCTION(JoinThread, env, args, results);
    //DEFINE_WASM_FUNCTION(DetachThread, env, args, results);

    /*DEFINE_WASM_FUNCTION(CreateThread, env, args, results);
    DEFINE_WASM_FUNCTION(CreateThread, env, args, results);
    DEFINE_WASM_FUNCTION(CreateThread, env, args, results);
    DEFINE_WASM_FUNCTION(CreateThread, env, args, results);
    DEFINE_WASM_FUNCTION(CreateThread, env, args, results);
    DEFINE_WASM_FUNCTION(CreateThread, env, args, results);
    DEFINE_WASM_FUNCTION(CreateThread, env, args, results);*/ 
};
