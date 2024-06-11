/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyTeamDefs.h
 *  PURPOSE:     Web assembly definitions base class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CWebAssemblyDefs.h"

class CWebAssemblyTeamDefs : public CWebAssemblyDefs
{
public:
    static void RegisterFunctionTypes();
    static void RegisterFunctions(CWebAssemblyScript* script);

    DEFINE_WASM_FUNCTION(CreateTeam, env, args, results);

    DEFINE_WASM_FUNCTION(GetTeamFromName, env, args, results);
    DEFINE_WASM_FUNCTION(GetTeamName, env, args, results);
    DEFINE_WASM_FUNCTION(GetTeamColor, env, args, results);
    DEFINE_WASM_FUNCTION(GetTeamFriendlyFire, env, args, results);
    DEFINE_WASM_FUNCTION(GetPlayersInTeam, env, args, results);
    DEFINE_WASM_FUNCTION(CountPlayersInTeam, env, args, results);

    DEFINE_WASM_FUNCTION(SetPlayerTeam, env, args, results);
    DEFINE_WASM_FUNCTION(SetTeamName, env, args, results);
    DEFINE_WASM_FUNCTION(SetTeamColor, env, args, results);
    DEFINE_WASM_FUNCTION(SetTeamFriendlyFire, env, args, results);
};
