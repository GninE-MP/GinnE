/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyBlipDefs.h
 *  PURPOSE:     Web assembly definitions base class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CWebAssemblyDefs.h"

class CWebAssemblyBlipDefs : public CWebAssemblyDefs
{
public:
    static void RegisterFunctionTypes();
    static void RegisterFunctions(CWebAssemblyScript* script);

    DEFINE_WASM_FUNCTION(CreateBlip, env, args, results);
    DEFINE_WASM_FUNCTION(CreateBlipAttachedTo, env, args, results);

    DEFINE_WASM_FUNCTION(GetBlipIcon, env, args, results);
    DEFINE_WASM_FUNCTION(GetBlipSize, env, args, results);
    DEFINE_WASM_FUNCTION(GetBlipColor, env, args, results);
    DEFINE_WASM_FUNCTION(GetBlipOrdering, env, args, results);
    DEFINE_WASM_FUNCTION(GetBlipVisibleDistance, env, args, results);

    DEFINE_WASM_FUNCTION(SetBlipIcon, env, args, results);
    DEFINE_WASM_FUNCTION(SetBlipSize, env, args, results);
    DEFINE_WASM_FUNCTION(SetBlipColor, env, args, results);
    DEFINE_WASM_FUNCTION(SetBlipOrdering, env, args, results);
    DEFINE_WASM_FUNCTION(SetBlipVisibleDistance, env, args, results);
};
