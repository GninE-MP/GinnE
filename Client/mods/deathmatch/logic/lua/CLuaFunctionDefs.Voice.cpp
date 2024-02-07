/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Voice.cpp
 *  PURPOSE:     Lua Voice function definitions class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"

int CLuaFunctionDefs::IsVoiceEnabled(lua_State* luaVM)
{
    lua_pushboolean(luaVM, g_pClientGame->GetVoiceRecorder()->IsEnabled());
    return 1;
}