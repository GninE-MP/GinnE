/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyTeamDefs.cpp
 *  PURPOSE:     Web assembly definitions base class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CWebAssemblyDefs.h"
#include <SharedUtil.Memory.h>
#include "CStaticFunctionDefinitions.h"
#include "CDummy.h"
#include "Utils.h"

#include "../wasm/CWebAssemblyContext.h"
#include "../wasm/CWebAssemblyVariable.h"
#include "../wasm/CWebAssemblyArgReader.h"

#include "CWebAssemblyTeamDefs.h"

void CWebAssemblyTeamDefs::RegisterFunctionTypes()
{
    SetFunctionType("create_team", "es*");

    SetFunctionType("get_team_from_name", "es");
    SetFunctionType("get_team_name", "xesx");
    SetFunctionType("get_team_color", "be*");
    SetFunctionType("get_team_friendly_fire", "be");
    SetFunctionType("get_players_in_team", "ie*i");
    SetFunctionType("count_players_in_team", "ie");

    SetFunctionType("set_player_team", "bee");
    SetFunctionType("set_team_name", "bes");
    SetFunctionType("set_team_color", "be*");
    SetFunctionType("set_team_friendly_fire", "beb");
}

void CWebAssemblyTeamDefs::RegisterFunctions(CWebAssemblyScript* script)
{
    CWebAssemblyCFunctionMap functions = {
        { "create_team", CreateTeam },

        { "get_team_from_name", GetTeamFromName },
        { "get_team_name", GetTeamName },
        { "get_team_color", GetTeamColor },
        { "get_team_friendly_fire", GetTeamFriendlyFire },
        { "get_players_in_team", GetPlayersInTeam },
        { "count_players_in_team", CountPlayersInTeam },

        { "set_player_team", SetPlayerTeam },
        { "set_team_name", SetTeamName },
        { "set_team_color", SetTeamColor },
        { "set_team_friendly_fire", SetTeamFriendlyFire }
    };

    WASM_REGISTER_API(script, functions);
}

WebAssemblyApi(CWebAssemblyTeamDefs::CreateTeam, env, args, results)
{
    SString name;
    struct C
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    }* color;

    C c;
    c.r = 235;
    c.g = 221;
    c.b = 178;
    c.a = 255;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadString(name);
    argStream.ReadPointer(color, &c);

    CResource* resource = GetWebAssemblyEnvResource(env);

    if (!resource)
    {
        return argStream.ReturnNull();
    }

    CTeam* team = CStaticFunctionDefinitions::CreateTeam(resource, name.c_str(), color->r, color->g, color->b);

    if (!team)
    {
        return argStream.ReturnNull();
    }

    CElementGroup* group = resource->GetElementGroup();

    if (group)
    {
        group->Add(team);
    }

    return argStream.Return((CElement*)team);
}

WebAssemblyApi(CWebAssemblyTeamDefs::GetTeamFromName, env, args, results)
{
    SString name;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadString(name);

    CTeam* team = CStaticFunctionDefinitions::GetTeamFromName(name.c_str());

    if (!team)
    {
        return argStream.ReturnNull();
    }

    return argStream.Return((CElement*)team);
}

WebAssemblyApi(CWebAssemblyTeamDefs::GetTeamName, env, args, results)
{
    CTeam*                           team;
    CWebAssemblyMemoryPointerAddress outName;
    uint32_t                         maxSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(team);
    argStream.ReadPointerAddress(outName);
    argStream.ReadUInt32(maxSize, 0xffffffff);

    if (!team || outName == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(0);
    }

    SString name;

    if (!CStaticFunctionDefinitions::GetTeamName(team, name))
    {
        return argStream.Return(0);
    }

    uint32_t size = std::min(maxSize, (uint32_t)name.size());

    argStream.WritePointer(outName, name.data(), size);

    return argStream.Return(size);
}

WebAssemblyApi(CWebAssemblyTeamDefs::GetTeamColor, env, args, results)
{
    CTeam*                           team;
    CWebAssemblyMemoryPointerAddress outColor;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(team);
    argStream.ReadPointerAddress(outColor);

    if (!team)
    {
        return argStream.Return(false);
    }

    struct
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    } color;
    memset(&color, 0, sizeof(color));

    if (!CStaticFunctionDefinitions::GetTeamColor(team, color.r, color.g, color.b))
    {
        return argStream.Return(false);
    }

    color.a = 255;

    argStream.WritePointer(outColor, &color);

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyTeamDefs::GetTeamFriendlyFire, env, args, results)
{
    CTeam* team;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(team);

    if (!team)
    {
        return argStream.Return(false);
    }

    bool friendlyFire = false;

    if (!CStaticFunctionDefinitions::GetTeamFriendlyFire(team, friendlyFire))
    {
        return argStream.Return(false);
    }

    return argStream.Return(friendlyFire);
}

WebAssemblyApi(CWebAssemblyTeamDefs::GetPlayersInTeam, env, args, results)
{
    CTeam*                           team;
    CWebAssemblyMemoryPointerAddress outPlayers;
    uint32_t                         maxItemSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(team);
    argStream.ReadPointerAddress(outPlayers);
    argStream.ReadUInt32(maxItemSize, 0xffffffff);

    if (!team || outPlayers == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(0);
    }

    uint32_t count = 0;

    std::list<CPlayer*>::const_iterator iter = team->PlayersBegin();

    for (; iter != team->PlayersEnd() && maxItemSize > 0; iter++)
    {
        CPlayer* player = *iter;

        if (player->IsBeingDeleted())
        {
            continue;
        }

        CWebAssemblyUserData udata = ELEMENT_TO_WASM_USERDATA(player);

        argStream.WritePointer(outPlayers + (count * sizeof(udata)), &udata);

        count++;
        maxItemSize--;
    }

    return argStream.Return(count);
}

WebAssemblyApi(CWebAssemblyTeamDefs::CountPlayersInTeam, env, args, results)
{
    CTeam* team;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(team);

    if (!team)
    {
        return argStream.Return(0);
    }

    uint32_t count = 0;

    if (!CStaticFunctionDefinitions::CountPlayersInTeam(team, count))
    {
        return argStream.Return(0);
    }

    return argStream.Return(count);
}

WebAssemblyApi(CWebAssemblyTeamDefs::SetPlayerTeam, env, args, results)
{
    CPlayer* player;
    CTeam*   team;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadUserData(team);

    if (!player || !team)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetPlayerTeam(player, team));
}

WebAssemblyApi(CWebAssemblyTeamDefs::SetTeamName, env, args, results)
{
    CTeam*  team;
    SString name;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(team);
    argStream.ReadString(name);

    if (!team)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetTeamName(team, name.c_str()));
}

WebAssemblyApi(CWebAssemblyTeamDefs::SetTeamColor, env, args, results)
{
    CTeam* team;
    struct
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    }* color;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(team);
    argStream.ReadPointer(color);

    if (!team || !color)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetTeamColor(team, color->r, color->g, color->b));
}

WebAssemblyApi(CWebAssemblyTeamDefs::SetTeamFriendlyFire, env, args, results)
{
    CTeam* team;
    bool   friendlyFire;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(team);
    argStream.ReadBoolean(friendlyFire);

    if (!team)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetTeamFriendlyFire(team, friendlyFire));
}
