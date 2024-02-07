/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaClientDefs.h
 *  PURPOSE:     Lua client definitions class
 *
 *  GninE is available from https://gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CLuaDefs.h"

class CLuaClientDefs : public CLuaDefs
{
public:
    static void LoadFunctions();

private:
    static bool SetTransferBoxVisible(bool visible);
    static bool IsTransferBoxVisible();
    static bool IsTransferBoxAlwaysVisible();
    static bool ShowChat(bool bVisible, std::optional<bool> optInputBlocked);
    static bool IsChatVisible();
    static bool IsChatInputBlocked();
    static bool ClearDebug();
    static bool IsGninEWindowFocused();
    static bool IsCapsLockEnabled();
};
