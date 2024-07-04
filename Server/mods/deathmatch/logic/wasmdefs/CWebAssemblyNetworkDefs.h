/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyNetworkDefs.h
 *  PURPOSE:     Web assembly definitions base class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CWebAssemblyDefs.h"

class CWebAssemblyNetworkDefs : public CWebAssemblyDefs
{
public:
    static void RegisterFunctionTypes();
    static void RegisterFunctions(CWebAssemblyScript* script);

    DEFINE_WASM_FUNCTION(CallRemote, env, args, results);
    DEFINE_WASM_FUNCTION(FetchRemote, env, args, results);
    DEFINE_WASM_FUNCTION(GetRemoteRequests, env, args, results);
    DEFINE_WASM_FUNCTION(GetRemoteRequestInfo, env, args, results);
    DEFINE_WASM_FUNCTION(CleanupRemoteRequestInfo, env, args, results);
    DEFINE_WASM_FUNCTION(AbortRemoteRequest, env, args, results);
};
