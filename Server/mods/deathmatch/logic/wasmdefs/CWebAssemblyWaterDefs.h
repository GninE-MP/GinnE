/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyWaterDefs.h
 *  PURPOSE:     Web assembly definitions base class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CWebAssemblyDefs.h"

class CWebAssemblyWaterDefs : public CWebAssemblyDefs
{
public:
    static void RegisterFunctionTypes();
    static void RegisterFunctions(CWebAssemblyScript* script);

    DEFINE_WASM_FUNCTION(CreateWater, env, args, results);
    DEFINE_WASM_FUNCTION(SetWaterLevel, env, args, results);
    DEFINE_WASM_FUNCTION(ResetWaterLevel, env, args, results);
    DEFINE_WASM_FUNCTION(GetWaterVertexPosition, env, args, results);
    DEFINE_WASM_FUNCTION(SetWaterVertexPosition, env, args, results);
    DEFINE_WASM_FUNCTION(GetWaterColor, env, args, results);
    DEFINE_WASM_FUNCTION(SetWaterColor, env, args, results);
    DEFINE_WASM_FUNCTION(ResetWaterColor, env, args, results);
};
