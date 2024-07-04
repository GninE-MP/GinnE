/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyCameraDefs.h
 *  PURPOSE:     Web assembly definitions base class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CWebAssemblyDefs.h"

class CWebAssemblyCameraDefs : public CWebAssemblyDefs
{
public:
    static void RegisterFunctionTypes();
    static void RegisterFunctions(CWebAssemblyScript* script);

    DEFINE_WASM_FUNCTION(GetCameraMatrix, env, args, results);
    DEFINE_WASM_FUNCTION(GetCameraTarget, env, args, results);
    DEFINE_WASM_FUNCTION(GetCameraInterior, env, args, results);

    DEFINE_WASM_FUNCTION(SetCameraMatrix, env, args, results);
    DEFINE_WASM_FUNCTION(SetCameraTarget, env, args, results);
    DEFINE_WASM_FUNCTION(SetCameraInterior, env, args, results);
    DEFINE_WASM_FUNCTION(FadeCamera, env, args, results);
};
