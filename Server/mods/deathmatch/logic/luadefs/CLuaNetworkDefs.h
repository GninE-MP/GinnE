/*****************************************************************************
 *
 *  PROJECT:     GninE
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaNetworkDefs : public CLuaDefs
{
public:
    static void LoadFunctions();

    LUA_DECLARE(CallRemote);
    LUA_DECLARE(FetchRemote);
    LUA_DECLARE(GetRemoteRequests);
    LUA_DECLARE(GetRemoteRequestInfo);
    LUA_DECLARE(AbortRemoteRequest);
};
