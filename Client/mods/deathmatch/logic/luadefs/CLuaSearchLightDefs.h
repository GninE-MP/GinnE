/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaSearchLightDefs.h
 *  PURPOSE:     Lua searchlight class functions
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaSearchLightDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    LUA_DECLARE(CreateSearchLight);

    LUA_DECLARE_OOP(GetSearchLightStartPosition);
    LUA_DECLARE_OOP(GetSearchLightEndPosition);
    LUA_DECLARE(GetSearchLightStartRadius);
    LUA_DECLARE(GetSearchLightEndRadius);

    LUA_DECLARE(SetSearchLightStartPosition);
    LUA_DECLARE(SetSearchLightEndPosition);
    LUA_DECLARE(SetSearchLightStartRadius);
    LUA_DECLARE(SetSearchLightEndRadius);
};
