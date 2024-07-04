/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyVoiceDefs.cpp
 *  PURPOSE:     Web assembly definitions base class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CWebAssemblyDefs.h"
#include <SharedUtil.Memory.h>
#include "CDummy.h"
#include "CStaticFunctionDefinitions.h"
#include "Utils.h"
#include "CScriptDebugging.h"

#include "CMainConfig.h"

#include "../wasm/CWebAssemblyContext.h"
#include "../wasm/CWebAssemblyVariable.h"
#include "../wasm/CWebAssemblyArgReader.h"

#include "CWebAssemblyVoiceDefs.h"

void CWebAssemblyVoiceDefs::RegisterFunctionTypes()
{
    SetFunctionType("is_voice_enabled", "b");
    SetFunctionType("set_player_voice_broadcast_to", "bu*i");
    SetFunctionType("set_player_voice_ignore_from", "bu*i");
}

void CWebAssemblyVoiceDefs::RegisterFunctions(CWebAssemblyScript* script)
{
    CWebAssemblyCFunctionMap functions = {
        { "is_voice_enabled", IsVoiceEnabled },
        { "set_player_voice_broadcast_to", SetPlayerVoiceBroadcastTo },
        { "set_player_voice_ignore_from", SetPlayerVoiceIgnoreFrom }
    };

    WASM_REGISTER_API(script, functions);
}

WebAssemblyApi(CWebAssemblyVoiceDefs::IsVoiceEnabled, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.Return(g_pGame->GetConfig()->IsVoiceEnabled());
}

WebAssemblyApi(CWebAssemblyVoiceDefs::SetPlayerVoiceBroadcastTo, env, args, results)
{
    CPlayer*              player;
    CWebAssemblyUserData* players;
    uint32_t              count;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadPointer(players);
    argStream.ReadUInt32(count);

    if (!player)
    {
        return argStream.Return(false);
    }

    if (!players || count < 1)
    {
        player->SetVoiceBroadcastTo(NULL);

        return argStream.Return(true);
    }

    std::list<CElement*> list;

    for (uint32_t i = 0; i < count; i++)
    {
        CElement* target;

        argStream.ExtractUserData(players[i], target);

        if (!target)
        {
            continue;
        }

        list.push_back(target);
    }

    player->SetVoiceBroadcastTo(list);

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyVoiceDefs::SetPlayerVoiceIgnoreFrom, env, args, results)
{
    CPlayer*              player;
    CWebAssemblyUserData* players;
    uint32_t              count;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadPointer(players);
    argStream.ReadUInt32(count);

    if (!player)
    {
        return argStream.Return(false);
    }

    if (!players || count < 1)
    {
        player->SetVoiceIgnoredElement(NULL);

        return argStream.Return(true);
    }

    std::list<CElement*> list;

    for (uint32_t i = 0; i < count; i++)
    {
        CElement* target;

        argStream.ExtractUserData(players[i], target);

        if (!target)
        {
            continue;
        }

        list.push_back(target);
    }

    player->SetVoiceIgnoredList(list);

    return argStream.Return(true);
}
