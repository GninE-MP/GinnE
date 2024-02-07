/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/LuaUtils.h
 *  PURPOSE:     Lua Utility functions
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

struct lua_State;

SString GetDebugMessage(lua_State* luaVM);
