/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyPickupDefs.h
 *  PURPOSE:     Web assembly definitions base class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CWebAssemblyDefs.h"

class CWebAssemblyPickupDefs : public CWebAssemblyDefs
{
public:
    static void RegisterFunctionTypes();
    static void RegisterFunctions(CWebAssemblyScript* script);

    DEFINE_WASM_FUNCTION(CreatePickup, env, args, results);

    DEFINE_WASM_FUNCTION(GetPickupType, env, args, results);
    DEFINE_WASM_FUNCTION(GetPickupWeapon, env, args, results);
    DEFINE_WASM_FUNCTION(GetPickupAmount, env, args, results);
    DEFINE_WASM_FUNCTION(GetPickupAmmo, env, args, results);
    DEFINE_WASM_FUNCTION(GetPickupRespawnInterval, env, args, results);
    DEFINE_WASM_FUNCTION(IsPickupSpawned, env, args, results);

    DEFINE_WASM_FUNCTION(SetPickupType, env, args, results);
    DEFINE_WASM_FUNCTION(SetPickupRespawnInterval, env, args, results);
    DEFINE_WASM_FUNCTION(UsePickup, env, args, results);
};
