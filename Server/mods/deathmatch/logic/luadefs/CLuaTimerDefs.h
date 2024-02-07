/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaTimerDefs.h
 *  PURPOSE:     Lua function definitions class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaTimerDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    LUA_DECLARE(SetTimer);
    LUA_DECLARE(KillTimer);
    LUA_DECLARE(ResetTimer);
    LUA_DECLARE(GetTimers);
    LUA_DECLARE(IsTimer);
    LUA_DECLARE(GetTimerDetails);
};