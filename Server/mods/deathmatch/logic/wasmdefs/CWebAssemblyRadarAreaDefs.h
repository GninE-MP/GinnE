/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyRadarAreaDefs.h
 *  PURPOSE:     Web assembly definitions base class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CWebAssemblyDefs.h"

class CWebAssemblyRadarAreaDefs : public CWebAssemblyDefs
{
public:
    static void RegisterFunctionTypes();
    static void RegisterFunctions(CWebAssemblyScript* script);

    DEFINE_WASM_FUNCTION(CreateRadarArea, env, args, results);

    DEFINE_WASM_FUNCTION(GetRadarAreaSize, env, args, results);
    DEFINE_WASM_FUNCTION(GetRadarAreaColor, env, args, results);
    DEFINE_WASM_FUNCTION(IsRadarAreaFlashing, env, args, results);
    DEFINE_WASM_FUNCTION(IsInsideRadarArea, env, args, results);

    DEFINE_WASM_FUNCTION(SetRadarAreaSize, env, args, results);
    DEFINE_WASM_FUNCTION(SetRadarAreaColor, env, args, results);
    DEFINE_WASM_FUNCTION(SetRadarAreaFlashing, env, args, results);
};
