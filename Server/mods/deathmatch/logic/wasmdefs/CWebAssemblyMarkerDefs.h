/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyMarkerDefs.h
 *  PURPOSE:     Web assembly definitions base class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CWebAssemblyDefs.h"

class CWebAssemblyMarkerDefs : public CWebAssemblyDefs
{
public:
    static void RegisterFunctionTypes();
    static void RegisterFunctions(CWebAssemblyScript* script);

    DEFINE_WASM_FUNCTION(CreateMarker, env, args, results);

    DEFINE_WASM_FUNCTION(GetMarkerCount, env, args, results);
    DEFINE_WASM_FUNCTION(GetMarkerType, env, args, results);
    DEFINE_WASM_FUNCTION(GetMarkerSize, env, args, results);
    DEFINE_WASM_FUNCTION(GetMarkerColor, env, args, results);
    DEFINE_WASM_FUNCTION(GetMarkerTarget, env, args, results);
    DEFINE_WASM_FUNCTION(GetMarkerIcon, env, args, results);

    DEFINE_WASM_FUNCTION(SetMarkerType, env, args, results);
    DEFINE_WASM_FUNCTION(SetMarkerSize, env, args, results);
    DEFINE_WASM_FUNCTION(SetMarkerColor, env, args, results);
    DEFINE_WASM_FUNCTION(SetMarkerTarget, env, args, results);
    DEFINE_WASM_FUNCTION(SetMarkerIcon, env, args, results);
};
