/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/luadefs/CLuaFireDefs.h
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/
#pragma once

#include "CLuaDefs.h"

class CLuaFireDefs : public CLuaDefs
{
public:
    static void LoadFunctions();

    LUA_DECLARE(CreateFire);
    LUA_DECLARE(ExtinguishFire);
};
