/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaEffectDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaEffectDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    LUA_DECLARE(fxAddBlood);
    LUA_DECLARE(fxAddWood);
    LUA_DECLARE(fxAddSparks);
    LUA_DECLARE(fxAddTyreBurst);
    LUA_DECLARE(fxAddBulletImpact);
    LUA_DECLARE(fxAddPunchImpact);
    LUA_DECLARE(fxAddDebris);
    LUA_DECLARE(fxAddGlass);
    LUA_DECLARE(fxAddWaterHydrant);
    LUA_DECLARE(fxAddGunshot);
    LUA_DECLARE(fxAddTankFire);
    LUA_DECLARE(fxAddWaterSplash);
    LUA_DECLARE(fxAddBulletSplash);
    LUA_DECLARE(fxAddFootSplash);
    LUA_DECLARE(CreateEffect);
    LUA_DECLARE(SetEffectSpeed);
    LUA_DECLARE(GetEffectSpeed);
    LUA_DECLARE(SetEffectDensity);
    LUA_DECLARE(GetEffectDensity);
};
