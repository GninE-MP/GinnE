/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyPlayerDefs.cpp
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
#include "CKeyBinds.h"

#include "../wasm/CWebAssemblyContext.h"
#include "../wasm/CWebAssemblyVariable.h"
#include "../wasm/CWebAssemblyArgReader.h"

#include "CWebAssemblyPlayerDefs.h"

void CWebAssemblyPlayerDefs::RegisterFunctionTypes()
{
    SetFunctionType("get_player_count", "i");
    SetFunctionType("set_player_ammo", "beiii");
    SetFunctionType("get_player_from_name", "es");
    SetFunctionType("get_player_money", "ie");
    SetFunctionType("get_player_ping", "ie");
    SetFunctionType("get_random_player", "e");
    SetFunctionType("is_player_muted", "be");
    SetFunctionType("get_player_team", "ee");
    SetFunctionType("can_player_use_function", "bes");
    SetFunctionType("get_player_wanted_level", "ie");
    SetFunctionType("get_alive_players", "i*i");
    SetFunctionType("get_dead_players", "i*i");
    SetFunctionType("get_player_idle_time", "le");
    SetFunctionType("is_player_scoreboard_forced", "be");
    SetFunctionType("is_player_map_forced", "be");
    SetFunctionType("get_player_nametag_text", "xesx");
    SetFunctionType("get_player_nametag_color", "be*");
    SetFunctionType("is_player_nametag_showing", "be");
    SetFunctionType("get_player_serial", "xeisx");
    SetFunctionType("get_player_community_id", "xesx");
    SetFunctionType("get_player_user_name", "xesx");
    SetFunctionType("get_player_blur_level", "ie");
    SetFunctionType("get_player_name", "xesx");
    SetFunctionType("get_player_ip", "xesx");
    SetFunctionType("get_player_account", "ue");
    SetFunctionType("get_player_version", "xesx");
    SetFunctionType("get_player_ac_info_r", "be****x");
    SetFunctionType("get_player_script_debug_level", "ie");

    SetFunctionType("set_player_money", "beib");
    SetFunctionType("give_player_money", "bei");
    SetFunctionType("take_player_money", "bei");
    SetFunctionType("spawn_player", "be*fiiie");
    SetFunctionType("show_player_hud_component", "besb");
    SetFunctionType("set_player_wanted_level", "bei");
    SetFunctionType("force_player_map", "beb");
    SetFunctionType("set_player_nametag_text", "bes");
    SetFunctionType("set_player_nametag_color", "be*");
    SetFunctionType("set_player_nametag_showing", "beb");
    SetFunctionType("set_player_muted", "beb");
    SetFunctionType("set_player_blur_level", "bei");
    SetFunctionType("redirect_player", "besis");
    SetFunctionType("set_player_name", "bes");
    SetFunctionType("detonate_satchels", "be");
    SetFunctionType("take_player_screen_shot", "beiisiii");
    SetFunctionType("set_player_script_debug_level", "bei");

    SetFunctionType("get_player_announce_value", "xes*x");
    SetFunctionType("set_player_announce_value", "bess");
    SetFunctionType("resend_player_mod_info", "be");
    SetFunctionType("resend_player_ac_info", "be");

    SetFunctionType("bind_key", "bess*u");
    SetFunctionType("bind_key_on_command", "bessss");
    SetFunctionType("unbind_key", "bess*");
    SetFunctionType("unbind_key_on_command", "besss");
    SetFunctionType("is_key_bound", "bess*");
    SetFunctionType("get_functions_bound_to_key", "iess*i");
    SetFunctionType("get_key_bound_to_function", "xe*sx");
    SetFunctionType("get_control_state", "bes");
    SetFunctionType("is_control_enabled", "bes");

    SetFunctionType("set_control_state", "besb");
    SetFunctionType("toggle_control", "besb");
    SetFunctionType("toggle_all_controls", "bebbb");

    SetFunctionType("play_sound_front_end", "bei");
    SetFunctionType("play_mission_audio", "be*i");
    SetFunctionType("preload_mission_audio", "beii");

    SetFunctionType("is_cursor_showing", "be");
    SetFunctionType("show_cursor", "bebb");

    SetFunctionType("show_chat", "bebb");

    SetFunctionType("kick_player", "bees");
    SetFunctionType("ban_player", "uebbbesi");
}

void CWebAssemblyPlayerDefs::RegisterFunctions(CWebAssemblyScript* script)
{
    CWebAssemblyCFunctionMap functions = {
        { "get_player_count", GetPlayerCount },
        { "set_player_ammo", SetPlayerAmmo },
        { "get_player_from_name", GetPlayerFromName },
        { "get_player_money", GetPlayerMoney },
        { "get_player_ping", GetPlayerPing },
        { "get_random_player", GetRandomPlayer },
        { "is_player_muted", IsPlayerMuted },
        { "get_player_team", GetPlayerTeam },
        { "can_player_use_function", CanPlayerUseFunction },
        { "get_player_wanted_level", GetPlayerWantedLevel },
        { "get_alive_players", GetAlivePlayers },
        { "get_dead_players", GetDeadPlayers },
        { "get_player_idle_time", GetPlayerIdleTime },
        { "is_player_scoreboard_forced", IsPlayerScoreboardForced },
        { "is_player_map_forced", IsPlayerMapForced },
        { "get_player_nametag_text", GetPlayerNametagText },
        { "get_player_nametag_color", GetPlayerNametagColor },
        { "is_player_nametag_showing", IsPlayerNametagShowing },
        { "get_player_serial", GetPlayerSerial },
        { "get_player_community_id", GetPlayerCommunityID },
        { "get_player_user_name", GetPlayerUserName },
        { "get_player_blur_level", GetPlayerBlurLevel },
        { "get_player_name", GetPlayerName },
        { "get_player_ip", GetPlayerIP },
        { "get_player_account", GetPlayerAccount },
        { "get_player_version", GetPlayerVersion },
        { "get_player_ac_info_r", GetPlayerACInfo },
        { "get_player_script_debug_level", GetPlayerScriptDebugLevel },

        { "set_player_money", SetPlayerMoney },
        { "give_player_money", GivePlayerMoney },
        { "take_player_money", TakePlayerMoney },
        { "spawn_player", SpawnPlayer },
        { "show_player_hud_component", ShowPlayerHudComponent },
        { "set_player_wanted_level", SetPlayerWantedLevel },
        { "force_player_map", ForcePlayerMap },
        { "set_player_nametag_text", SetPlayerNametagText },
        { "set_player_nametag_color", SetPlayerNametagColor },
        { "set_player_nametag_showing", SetPlayerNametagShowing },
        { "set_player_muted", SetPlayerMuted },
        { "set_player_blur_level", SetPlayerBlurLevel },
        { "redirect_player", RedirectPlayer },
        { "set_player_name", SetPlayerName },
        { "detonate_satchels", DetonateSatchels },
        { "take_player_screen_shot", TakePlayerScreenShot },
        { "set_player_script_debug_level", SetPlayerScriptDebugLevel },

        { "get_player_announce_value", GetPlayerAnnounceValue },
        { "set_player_announce_value", SetPlayerAnnounceValue },
        { "resend_player_mod_info", ResendPlayerModInfo },
        { "resend_player_ac_info", ResendPlayerACInfo },

        { "bind_key", BindKey },
        { "bind_key_on_command", BindKeyOnCommand },
        { "unbind_key", UnbindKey },
        { "unbind_key_on_command", UnbindKeyOnCommand },
        { "is_key_bound", IsKeyBound },
        { "get_functions_bound_to_key", GetFunctionsBoundToKey },
        { "get_key_bound_to_function", GetKeyBoundToFunction },
        { "get_control_state", GetControlState },
        { "is_control_enabled", IsControlEnabled },

        { "set_control_state", SetControlState },
        { "toggle_control", ToggleControl },
        { "toggle_all_controls", ToggleAllControls },

        { "play_sound_front_end", PlaySoundFrontEnd },
        { "play_mission_audio", PlayMissionAudio },
        { "preload_mission_audio", PreloadMissionAudio },

        { "is_cursor_showing", IsCursorShowing },
        { "show_cursor", ShowCursor },

        { "show_chat", ShowChat },

        { "kick_player", KickPlayer },
        { "ban_player", BanPlayer }
    };

    WASM_REGISTER_API(script, functions);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::GetPlayerCount, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    return argStream.Return(CStaticFunctionDefinitions::GetPlayerCount());
}

WebAssemblyApi(CWebAssemblyPlayerDefs::SetPlayerAmmo, env, args, results)
{
    CPlayer* player;
    uint32_t ammo;
    uint32_t  slot;
    uint32_t ammoInClip;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadUInt32(ammo);
    argStream.ReadUInt32(slot, 0xff);
    argStream.ReadUInt32(ammoInClip, 0);

    if (!player)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetPlayerAmmo(player, (uint8_t)slot, (uint16_t)ammo, (uint16_t)ammoInClip));
}

WebAssemblyApi(CWebAssemblyPlayerDefs::GetPlayerFromName, env, args, results)
{
    SString name;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadString(name);

    if (name.empty())
    {
        return argStream.ReturnNull();
    }

    CPlayer* player = CStaticFunctionDefinitions::GetPlayerFromName(name.c_str());

    if (!player)
    {
        return argStream.ReturnNull();
    }

    return argStream.Return(player);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::GetPlayerMoney, env, args, results)
{
    CPlayer* player;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);

    if (!player)
    {
        return argStream.Return(0);
    }

    long money = 0;

    if (!CStaticFunctionDefinitions::GetPlayerMoney(player, money))
    {
        return argStream.Return(0);
    }

    return argStream.Return(money);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::GetPlayerPing, env, args, results)
{
    CPlayer* player;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);

    if (!player)
    {
        return argStream.Return(0);
    }

    uint32_t ping = 0;

    if (!CStaticFunctionDefinitions::GetPlayerPing(player, ping))
    {
        return argStream.Return(0);
    }

    return argStream.Return(ping);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::GetRandomPlayer, env, args, results)
{
    CWebAssemblyArgReader argStream(env, args, results);

    CPlayer* player = CStaticFunctionDefinitions::GetRandomPlayer();

    if (!player)
    {
        return argStream.ReturnNull();
    }

    return argStream.Return(player);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::IsPlayerMuted, env, args, results)
{
    CPlayer* player;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);

    if (!player)
    {
        return argStream.Return(false);
    }

    bool isMuted = false;

    if (!CStaticFunctionDefinitions::IsPlayerMuted(player, isMuted))
    {
        return argStream.Return(false);
    }

    return argStream.Return(isMuted);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::GetPlayerTeam, env, args, results)
{
    CPlayer* player;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);

    if (!player)
    {
        return argStream.ReturnNull();
    }

    CTeam* team = CStaticFunctionDefinitions::GetPlayerTeam(player);

    if (!team)
    {
        return argStream.ReturnNull();
    }

    return argStream.Return((CElement*)team);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::CanPlayerUseFunction, env, args, results)
{
    CPlayer* player;
    SString  functionName;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadString(functionName);

    if (!player)
    {
        return argStream.Return(false);
    }

    bool canUse = false;

    if (!CStaticFunctionDefinitions::CanPlayerUseFunction(player, functionName.c_str(), canUse))
    {
        return argStream.Return(false);
    }

    return argStream.Return(canUse);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::GetPlayerWantedLevel, env, args, results)
{
    CPlayer* player;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);

    if (!player)
    {
        return argStream.Return(0);
    }

    uint32_t wLevel = 0;

    if (!CStaticFunctionDefinitions::GetPlayerWantedLevel(player, wLevel))
    {
        return argStream.Return(0);
    }

    return argStream.Return(wLevel);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::GetAlivePlayers, env, args, results)
{
    CWebAssemblyMemoryPointerAddress listPtr;
    uint32_t                         maxCount;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadPointerAddress(listPtr);
    argStream.ReadUInt32(maxCount, 500);

    if (listPtr == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(0);
    }

    CPlayerManager* pManager = g_pGame->GetPlayerManager();

    uint32_t count = 0;

    std::list<CPlayer*>::const_iterator iter = pManager->IterBegin();

    for (; iter != pManager->IterEnd() && maxCount > 0; iter++)
    {
        CPlayer* player = *iter;

        if (player->IsJoined() && player->IsSpawned() && !player->IsBeingDeleted())
        {
            CWebAssemblyUserData uData = ELEMENT_TO_WASM_USERDATA(player);

            argStream.WritePointer(listPtr + (count * sizeof(uData)), &uData);

            count++;
            maxCount--;
        }
    }

    return argStream.Return(count);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::GetDeadPlayers, env, args, results)
{
    CWebAssemblyMemoryPointerAddress listPtr;
    uint32_t                         maxCount;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadPointerAddress(listPtr);
    argStream.ReadUInt32(maxCount, 500);

    if (listPtr == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(0);
    }

    CPlayerManager* pManager = g_pGame->GetPlayerManager();

    uint32_t count = 0;

    std::list<CPlayer*>::const_iterator iter = pManager->IterBegin();

    for (; iter != pManager->IterEnd() && maxCount > 0; iter++)
    {
        CPlayer* player = *iter;

        if (player->IsJoined() && !player->IsSpawned() && !player->IsBeingDeleted())
        {
            CWebAssemblyUserData uData = ELEMENT_TO_WASM_USERDATA(player);

            argStream.WritePointer(listPtr + (count * sizeof(uData)), &uData);

            count++;
            maxCount--;
        }
    }

    return argStream.Return(count);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::GetPlayerIdleTime, env, args, results)
{
    CPlayer* player;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);

    if (!player)
    {
        return argStream.Return(0);
    }

    int64_t lastTime = player->GetPositionLastChanged();

    if (lastTime == 0)
    {
        return argStream.Return(0);
    }


    return argStream.Return(GetTickCount64_() - lastTime);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::IsPlayerScoreboardForced, env, args, results)
{
    CPlayer* player;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);

    if (!player)
    {
        return argStream.Return(false);
    }

    bool isForced = false;

    if (!CStaticFunctionDefinitions::IsPlayerScoreboardForced(player, isForced))
    {
        return argStream.Return(false);
    }

    return argStream.Return(isForced);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::IsPlayerMapForced, env, args, results)
{
    CPlayer* player;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);

    if (!player)
    {
        return argStream.Return(false);
    }

    bool isForced = false;

    if (!CStaticFunctionDefinitions::IsPlayerMapForced(player, isForced))
    {
        return argStream.Return(false);
    }

    return argStream.Return(isForced);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::GetPlayerNametagText, env, args, results)
{
    CPlayer*                         player;
    CWebAssemblyMemoryPointerAddress outText;
    uint32_t                         maxSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadPointerAddress(outText);
    argStream.ReadUInt32(maxSize, 0xffffffff);

    if (!player || outText == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(0);
    }

    SString text;

    if (!CStaticFunctionDefinitions::GetPlayerNametagText(player, text))
    {
        return argStream.Return(0);
    }

    size_t length = std::min((size_t)maxSize, text.size());

    argStream.WritePointer(outText, text.data(), length);

    return argStream.Return(length);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::GetPlayerNametagColor, env, args, results)
{
    CPlayer*                         player;
    CWebAssemblyMemoryPointerAddress outColor;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadPointerAddress(outColor);

    if (!player || outColor == WEB_ASSEMBLY_NULL_PTR)
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

    color.a = 255;

    if (!CStaticFunctionDefinitions::GetPlayerNametagColor(player, color.r, color.g, color.b))
    {
        return argStream.Return(false);
    }

    argStream.WritePointer(outColor, &color);

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::IsPlayerNametagShowing, env, args, results)
{
    CPlayer* player;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);

    if (!player)
    {
        return argStream.Return(false);
    }

    bool showing = false;

    if (!CStaticFunctionDefinitions::IsPlayerNametagShowing(player, showing))
    {
        return argStream.Return(false);
    }

    return argStream.Return(showing);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::GetPlayerSerial, env, args, results)
{
    CPlayer*                         player;
    uint32_t                         index;
    CWebAssemblyMemoryPointerAddress outSerial;
    uint32_t                         maxSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadUInt32(index, 0);
    argStream.ReadPointerAddress(outSerial);
    argStream.ReadUInt32(maxSize, 0xffffffff);

    if (!player || outSerial == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(0);
    }

    SString serial = CStaticFunctionDefinitions::GetPlayerSerial(player, index);

    uint32_t size = std::min((size_t)maxSize, serial.size());

    argStream.WritePointer(outSerial, serial.data(), size);

    return argStream.Return(size);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::GetPlayerCommunityID, env, args, results)
{
    CPlayer*                         player;
    CWebAssemblyMemoryPointerAddress outId;
    uint32_t                         maxSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadPointerAddress(outId);
    argStream.ReadUInt32(maxSize, 0xffffffff);

    if (!player || outId == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(0);
    }

    SString id = CStaticFunctionDefinitions::GetPlayerCommunityID(player);

    uint32_t size = std::min((size_t)maxSize, id.size());

    argStream.WritePointer(outId, id.data(), size);

    return argStream.Return(size);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::GetPlayerUserName, env, args, results)
{
    CPlayer*                         player;
    CWebAssemblyMemoryPointerAddress outUsername;
    uint32_t                         maxSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadPointerAddress(outUsername);
    argStream.ReadUInt32(maxSize, 0xffffffff);

    if (!player || outUsername == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(0);
    }

    SString username = CStaticFunctionDefinitions::GetPlayerUserName(player);

    uint32_t size = std::min((size_t)maxSize, username.size());

    argStream.WritePointer(outUsername, username.data(), size);

    return argStream.Return(size);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::GetPlayerBlurLevel, env, args, results)
{
    CPlayer* player;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);

    if (!player)
    {
        return argStream.Return(0);
    }

    uint8_t level = 0;

    if (!CStaticFunctionDefinitions::GetPlayerBlurLevel(player, level))
    {
        return argStream.Return(0);
    }

    return argStream.Return((uint32_t)level);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::GetPlayerName, env, args, results)
{
    CElement*                        player;
    CWebAssemblyMemoryPointerAddress outName;
    uint32_t                         maxSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadPointerAddress(outName);
    argStream.ReadUInt32(maxSize);

    if (!player || outName == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(0);
    }

    if (!(IS_PLAYER(player) || IS_CONSOLE(player)))
    {
        return argStream.Return(0);
    }

    SString name;

    if (!CStaticFunctionDefinitions::GetPlayerName(player, name))
    {
        return argStream.Return(0);
    }

    uint32_t size = std::min((size_t)maxSize, name.size());

    argStream.WritePointer(outName, name.data(), size);

    return argStream.Return(size);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::GetPlayerIP, env, args, results)
{
    CElement*                        player;
    CWebAssemblyMemoryPointerAddress outIp;
    uint32_t                         maxSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadPointerAddress(outIp);
    argStream.ReadUInt32(maxSize);

    if (!player || outIp == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(0);
    }

    if (!(IS_PLAYER(player) || IS_CONSOLE(player))) {
        return argStream.Return(0);
    }

    SString ip;

    if (!CStaticFunctionDefinitions::GetPlayerIP(player, ip))
    {
        return argStream.Return(0);
    }

    uint32_t size = std::min((size_t)maxSize, ip.size());

    argStream.WritePointer(outIp, ip.data(), size);

    return argStream.Return(size);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::GetPlayerAccount, env, args, results)
{
    CElement* player;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);

    if (!player || !(IS_PLAYER(player) || IS_CONSOLE(player)))
    {
        return argStream.ReturnNull();
    }

    CAccount* account = CStaticFunctionDefinitions::GetPlayerAccount(player);

    if (!account)
    {
        return argStream.ReturnNull();
    }

    return argStream.Return(account);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::GetPlayerVersion, env, args, results)
{
    CPlayer*                         player;
    CWebAssemblyMemoryPointerAddress outVersion;
    uint32_t                         maxSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadPointerAddress(outVersion);
    argStream.ReadUInt32(maxSize, 0xffffffff);

    if (!player || outVersion == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(0);
    }

    CMtaVersion version = CStaticFunctionDefinitions::GetPlayerVersion(player);
    
    uint32_t size = std::min((size_t)maxSize, version.size());

    argStream.WritePointer(outVersion, version.data(), size);

    return argStream.Return(size);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::GetPlayerACInfo, env, args, results)
{
    CPlayer*                         player;
    CWebAssemblyMemoryPointerAddress outDetectedAC;
    CWebAssemblyMemoryPointerAddress outD3d9Size;
    CWebAssemblyMemoryPointerAddress outD3d9MD5;
    CWebAssemblyMemoryPointerAddress outD3d9SHA256;
    uint32_t                         maxStringsSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadPointerAddress(outDetectedAC);
    argStream.ReadPointerAddress(outD3d9Size);
    argStream.ReadPointerAddress(outD3d9MD5);
    argStream.ReadPointerAddress(outD3d9SHA256);
    argStream.ReadUInt32(maxStringsSize, 0xffffffff);

    if (!player)
    {
        return argStream.Return(false);
    }

    uint32_t sizeDetectedAC = std::min((size_t)maxStringsSize, player->m_strDetectedAC.size());
    uint32_t sizeD3d9MD5 = std::min((size_t)maxStringsSize, player->m_strD3d9Md5.size());
    uint32_t sizeD3d9SHA256 = std::min((size_t)maxStringsSize, player->m_strD3d9Sha256.size());

    argStream.WritePointer(outDetectedAC, player->m_strDetectedAC.data(), sizeDetectedAC);
    argStream.WritePointer(outD3d9Size, &player->m_uiD3d9Size);
    argStream.WritePointer(outD3d9MD5, player->m_strD3d9Md5.data(), sizeD3d9MD5);
    argStream.WritePointer(outD3d9SHA256, player->m_strD3d9Sha256.data(), sizeD3d9SHA256);

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::GetPlayerScriptDebugLevel, env, args, results)
{
    CPlayer* player;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);

    if (!player)
    {
        return argStream.Return(0);
    }

    return argStream.Return(player->GetScriptDebugLevel());
}

WebAssemblyApi(CWebAssemblyPlayerDefs::SetPlayerMoney, env, args, results)
{
    CPlayer* player;
    int32_t  money;
    bool     instant;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadInt32(money);
    argStream.ReadBoolean(instant, false);

    if (!player)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetPlayerMoney(player, money, instant));
}

WebAssemblyApi(CWebAssemblyPlayerDefs::GivePlayerMoney, env, args, results)
{
    CPlayer* player;
    int32_t  money;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadInt32(money);

    if (!player)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::GivePlayerMoney(player, money));
}

WebAssemblyApi(CWebAssemblyPlayerDefs::TakePlayerMoney, env, args, results)
{
    CPlayer* player;
    int32_t  money;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadInt32(money);

    if (!player)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::TakePlayerMoney(player, money));
}

WebAssemblyApi(CWebAssemblyPlayerDefs::SpawnPlayer, env, args, results)
{
    CPlayer*  player;
    CVector   position;
    float32_t rotation;
    int32_t   model;
    int32_t   interior;
    int32_t   dimension;
    CTeam*    team;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadVector3D(position);
    argStream.ReadFloat32(rotation, 0.0f);
    argStream.ReadInt32(model, 0);
    argStream.ReadInt32(interior, 0);
    argStream.ReadInt32(dimension, 0);
    argStream.ReadUserData(team, (CTeam*)NULL);

    if (!player)
    {
        argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SpawnPlayer(player, position, rotation, (unsigned long)model, (unsigned char)interior, (unsigned short)dimension, team));
}

WebAssemblyApi(CWebAssemblyPlayerDefs::ShowPlayerHudComponent, env, args, results)
{
    CPlayer* player;
    SString  componentStr;
    bool     show;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadString(componentStr);
    argStream.ReadBoolean(show);

    if (!player)
    {
        return argStream.Return(false);
    }

    eHudComponent component;

    StringToEnum(componentStr, component);

    return argStream.Return(CStaticFunctionDefinitions::ShowPlayerHudComponent(player, component, show));
}

WebAssemblyApi(CWebAssemblyPlayerDefs::SetPlayerWantedLevel, env, args, results)
{
    CPlayer* player;
    uint32_t level;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadUInt32(level);

    if (!player)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetPlayerWantedLevel(player, level));
}

WebAssemblyApi(CWebAssemblyPlayerDefs::ForcePlayerMap, env, args, results)
{
    CPlayer* player;
    bool     force;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadBoolean(force);

    if (!player)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::ForcePlayerMap(player, force));
}

WebAssemblyApi(CWebAssemblyPlayerDefs::SetPlayerNametagText, env, args, results)
{
    CPlayer* player;
    SString  nametagText;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadString(nametagText);

    if (!player)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetPlayerNametagText(player, nametagText.c_str()));
}

WebAssemblyApi(CWebAssemblyPlayerDefs::SetPlayerNametagColor, env, args, results)
{
    CPlayer* player;
    struct
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    }* color;
    bool removeOverride;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadPointer(color);
    argStream.ReadBoolean(removeOverride, false);

    if (!player)
    {
        return argStream.Return(false);
    }

    removeOverride = !removeOverride ? true : false;

    return argStream.Return(CStaticFunctionDefinitions::SetPlayerNametagColor(player, removeOverride, color->r, color->g, color->b));
}

WebAssemblyApi(CWebAssemblyPlayerDefs::SetPlayerNametagShowing, env, args, results)
{
    CPlayer* player;
    bool     show;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadBoolean(show);

    if (!player)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetPlayerNametagShowing(player, show));
}

WebAssemblyApi(CWebAssemblyPlayerDefs::SetPlayerMuted, env, args, results)
{
    CPlayer* player;
    bool     muted;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadBoolean(muted);

    if (!player)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetPlayerMuted(player, muted));
}

WebAssemblyApi(CWebAssemblyPlayerDefs::SetPlayerBlurLevel, env, args, results)
{
    CPlayer* player;
    int32_t  level;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadInt32(level);

    if (!player)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetPlayerBlurLevel(player, (unsigned char)level));
}

WebAssemblyApi(CWebAssemblyPlayerDefs::RedirectPlayer, env, args, results)
{
    CPlayer* player;
    SString  host;
    int32_t  port;
    SString  password;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadString(host);
    argStream.ReadInt32(port);
    argStream.ReadString(password, "");

    if (!player)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::RedirectPlayer(player, host.c_str(), (unsigned short)port, password.c_str()));
}

WebAssemblyApi(CWebAssemblyPlayerDefs::SetPlayerName, env, args, results)
{
    CPlayer* player;
    SString  name;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadString(name);

    if (!player)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetPlayerName(player, name.c_str()));
}

WebAssemblyApi(CWebAssemblyPlayerDefs::DetonateSatchels, env, args, results)
{
    CPlayer* player;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);

    if (!player)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::DetonateSatchels(player));
}

WebAssemblyApi(CWebAssemblyPlayerDefs::TakePlayerScreenShot, env, args, results)
{
    CElement* player;
    uint32_t  width;
    uint32_t  height;
    SString   tag;
    uint32_t  quality;
    uint32_t  maxBandwidth;
    uint32_t  maxPacketSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadUInt32(width);
    argStream.ReadUInt32(height);
    argStream.ReadString(tag, "");
    argStream.ReadUInt32(quality, 30);
    argStream.ReadUInt32(maxBandwidth, 5000);
    argStream.ReadUInt32(maxPacketSize, 500);

    if (!player)
    {
        return argStream.Return(false);
    }

    CResource* resource = GetWebAssemblyEnvResource(env);

    if (!resource)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::TakePlayerScreenShot(player, width, height, tag, quality, maxBandwidth, maxPacketSize, resource));
}

WebAssemblyApi(CWebAssemblyPlayerDefs::SetPlayerScriptDebugLevel, env, args, results)
{
    CElement* player;
    uint32_t  level;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadUInt32(level);

    if (!player)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetPlayerScriptDebugLevel(player, level));
}

WebAssemblyApi(CWebAssemblyPlayerDefs::GetPlayerAnnounceValue, env, args, results)
{
    CPlayer*                         player;
    SString                          key;
    CWebAssemblyMemoryPointerAddress outValue;
    uint32_t                         maxSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadString(key);
    argStream.ReadPointerAddress(outValue);
    argStream.ReadUInt32(maxSize, 0xffffffff);

    if (!player || outValue == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(0);
    }

    SString value;

    if (!CStaticFunctionDefinitions::GetPlayerAnnounceValue(player, key, value))
    {
        return argStream.Return(0);
    }

    uint32_t size = std::min((size_t)maxSize, value.size());

    argStream.WritePointer(outValue, value.data(), size);

    return argStream.Return(size);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::SetPlayerAnnounceValue, env, args, results)
{
    CPlayer* player;
    SString  key;
    SString  value;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadString(key);
    argStream.ReadString(value);

    if (!player)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::SetPlayerAnnounceValue(player, key, value));
}

WebAssemblyApi(CWebAssemblyPlayerDefs::ResendPlayerModInfo, env, args, results)
{
    CPlayer* player;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);

    if (!player)
    {
        return argStream.Return(false);
    }

    g_pNetServer->ResendModPackets(player->GetSocket());

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::ResendPlayerACInfo, env, args, results)
{
    CPlayer* player;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);

    if (!player)
    {
        return argStream.Return(false);
    }

    g_pNetServer->ResendACPackets(player->GetSocket());

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::BindKey, env, args, results)
{
    CPlayer*        player;
    SString         key;
    SString         hitState;
    CLuaFunctionRef function;
    CLuaArguments   arguments;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadString(key);
    argStream.ReadString(hitState);
    argStream.ReadLuaFunctionRef(function);
    argStream.ReadLuaArguments(arguments);
    
    if (!player)
    {
        return argStream.Return(false);
    }

    CLuaMain* luaMain = GetWebAssemblyEnvResource(env)->GetVirtualMachine();

    return argStream.Return(CStaticFunctionDefinitions::BindKey(player, key.c_str(), hitState.c_str(), luaMain, function, arguments));
}

WebAssemblyApi(CWebAssemblyPlayerDefs::BindKeyOnCommand, env, args, results)
{
    CPlayer* player;
    SString  key;
    SString  hitState;
    SString  command;
    SString  arguments;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadString(key);
    argStream.ReadString(hitState);
    argStream.ReadString(command);
    argStream.ReadString(arguments, "");

    if (!player)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::BindKey(player, key.c_str(), hitState.c_str(), command.c_str(), arguments.c_str(), GetWebAssemblyEnvResource(env)->GetName().c_str()));
}

WebAssemblyApi(CWebAssemblyPlayerDefs::UnbindKey, env, args, results)
{
    CPlayer*        player;
    SString         key;
    SString         hitState;
    CLuaFunctionRef function;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadString(key);
    argStream.ReadString(hitState);
    argStream.ReadLuaFunctionRef(function);

    if (!player)
    {
        return argStream.Return(false);
    }

    CLuaMain* luaMain = GetWebAssemblyEnvResource(env)->GetVirtualMachine();

    return argStream.Return(CStaticFunctionDefinitions::UnbindKey(player, key.c_str(), luaMain, hitState.c_str(), function));
}

WebAssemblyApi(CWebAssemblyPlayerDefs::UnbindKeyOnCommand, env, args, results)
{
    CPlayer* player;
    SString  key;
    SString  hitState;
    SString  command;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadString(key);
    argStream.ReadString(hitState);
    argStream.ReadString(command);

    if (!player)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::UnbindKey(player, key.c_str(), hitState.c_str(), command.c_str(), GetWebAssemblyEnvResource(env)->GetName().c_str()));
}

WebAssemblyApi(CWebAssemblyPlayerDefs::IsKeyBound, env, args, results)
{
    CPlayer*        player;
    SString         key;
    SString         hitState;
    CLuaFunctionRef function;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadString(key);
    argStream.ReadString(hitState);
    argStream.ReadLuaFunctionRef(function);

    if (!player)
    {
        return argStream.Return(false);
    }

    CLuaMain* luaMain = GetWebAssemblyEnvResource(env)->GetVirtualMachine();

    bool bound = false;

    if (!CStaticFunctionDefinitions::IsKeyBound(player, key.c_str(), luaMain, hitState.c_str(), function, bound))
    {
        return argStream.Return(false);
    }

    return argStream.Return(bound);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::GetFunctionsBoundToKey, env, args, results)
{
    CPlayer* player;
    SString  key;
    SString  hitState;
    uint8_t* outCallables;
    uint32_t maxItemCount;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadString(key);
    argStream.ReadString(hitState);
    argStream.ReadPointer(outCallables);
    argStream.ReadUInt32(maxItemCount, 500);

    if (!player || outCallables == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(false);
    }

    bool checkHitState = false;
    bool hitStateB = false;

    if (hitState == "down")
    {
        checkHitState = true;
        hitStateB = true;
    }
    else if (hitState == "up")
    {
        checkHitState = true;
        hitStateB = false;
    }

    uint32_t count = 0;
    
    std::list<CKeyBind*>::iterator iter = player->GetKeyBinds()->IterBegin();

    lua_State* luaVM = GetWebAssemblyLuaVM(env);

    for (; iter != player->GetKeyBinds()->IterEnd() && maxItemCount > 0; ++iter)
    {
        CKeyBind* pKeyBind = *iter;
        if (!pKeyBind->IsBeingDeleted())
        {
            switch (pKeyBind->GetType())
            {
                case KEY_BIND_FUNCTION:
                {
                    CKeyFunctionBind* pBind = static_cast<CKeyFunctionBind*>(pKeyBind);
                    if (!checkHitState || pBind->bHitState == hitStateB)
                    {
                        if (key == pBind->boundKey->szKey)
                        {
                            if (pBind->m_iLuaFunction.IsCallable())
                            {
                                pBind->m_iLuaFunction.GetCallable().WriteHash(outCallables + (C_CALLABLE_HASH_SIZE * count));
                            }
                            else
                            {
                                lua_rawgeti(luaVM, LUA_REGISTRYINDEX, pBind->m_iLuaFunction.ToInt());
                                CLuaArgument func(luaVM, lua_gettop(luaVM));
                                lua_pop(luaVM, 1);

                                if (func.GetType() != LUA_TFUNCTION)
                                {
                                    continue;
                                }

                                func.GetCallable().WriteHash(outCallables + (C_CALLABLE_HASH_SIZE * count));
                            }

                            count++;
                            maxItemCount--;
                        }
                    }
                    break;
                }
                case KEY_BIND_CONTROL_FUNCTION:
                {
                    CControlFunctionBind* pBind = static_cast<CControlFunctionBind*>(pKeyBind);
                    if (!checkHitState || pBind->bHitState == hitStateB)
                    {
                        if (key == pBind->boundControl->szControl)
                        {
                            if (pBind->m_iLuaFunction.IsCallable())
                            {
                                pBind->m_iLuaFunction.GetCallable().WriteHash(outCallables + (C_CALLABLE_HASH_SIZE * count));
                            }
                            else
                            {
                                lua_rawgeti(luaVM, LUA_REGISTRYINDEX, pBind->m_iLuaFunction.ToInt());
                                CLuaArgument func(luaVM, lua_gettop(luaVM));
                                lua_pop(luaVM, 1);

                                if (func.GetType() != LUA_TFUNCTION)
                                {
                                    continue;
                                }

                                func.GetCallable().WriteHash(outCallables + (C_CALLABLE_HASH_SIZE * count));
                            }

                            count++;
                            maxItemCount--;
                        }
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }

    return argStream.Return(count);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::GetKeyBoundToFunction, env, args, results)
{
    CPlayer*                         player;
    CLuaFunctionRef                  function;
    CWebAssemblyMemoryPointerAddress outKey;
    uint32_t                         maxSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadLuaFunctionRef(function);
    argStream.ReadPointerAddress(outKey);
    argStream.ReadUInt32(maxSize, 0xffffffff);

    if (!player || outKey == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(0);
    }
    
    list<CKeyBind*>::iterator iter = player->GetKeyBinds()->IterBegin();

    for (; iter != player->GetKeyBinds()->IterEnd(); ++iter)
    {
        CKeyBind* pKeyBind = *iter;

        if (!pKeyBind->IsBeingDeleted())
        {
            switch (pKeyBind->GetType())
            {
                case KEY_BIND_FUNCTION:
                {
                    CKeyFunctionBind* pBind = static_cast<CKeyFunctionBind*>(pKeyBind);

                    if (function == pBind->m_iLuaFunction)
                    {
                        SString key = pBind->boundKey->szKey;

                        uint32_t size = std::min((size_t)maxSize, key.size());

                        argStream.WritePointer(outKey, key.data(), size);

                        return argStream.Return(size);
                    }

                    break;
                }
                case KEY_BIND_CONTROL_FUNCTION:
                {
                    CControlFunctionBind* pBind = static_cast<CControlFunctionBind*>(pKeyBind);

                    if (function == pBind->m_iLuaFunction)
                    {
                        SString key = pBind->boundKey->szKey;

                        uint32_t size = std::min((size_t)maxSize, key.size());

                        argStream.WritePointer(outKey, key.data(), size);

                        return argStream.Return(size);
                    }

                    break;
                }
                default:
                    break;
            }
        }
    }

    return argStream.Return(0);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::GetControlState, env, args, results)
{
    CPlayer* player;
    SString  control;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadString(control);

    if (!player)
    {
        return argStream.Return(false);
    }

    bool enabled = false;

    if (!CStaticFunctionDefinitions::GetControlState(player, control.c_str(), enabled))
    {
        return argStream.Return(false);
    }

    return argStream.Return(enabled);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::IsControlEnabled, env, args, results)
{
    CPlayer* player;
    SString  control;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadString(control);

    if (!player)
    {
        return argStream.Return(false);
    }

    bool enabled = false;

    if (!CStaticFunctionDefinitions::IsControlEnabled(player, control.c_str(), enabled))
    {
        return argStream.Return(false);
    }

    return argStream.Return(enabled);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::SetControlState, env, args, results)
{
    CPlayer* player;
    SString  control;
    bool     state;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadString(control);
    argStream.ReadBoolean(state);

    if (!player)
    {
        return argStream.Return(false);
    }

    if (!CStaticFunctionDefinitions::SetControlState(player, control.c_str(), state))
    {
        return argStream.Return(false);
    }

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::ToggleControl, env, args, results)
{
    CPlayer* player;
    SString  control;
    bool     state;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadString(control);
    argStream.ReadBoolean(state);

    if (!player)
    {
        return argStream.Return(false);
    }

    if (!CStaticFunctionDefinitions::ToggleControl(player, control.c_str(), state))
    {
        return argStream.Return(false);
    }

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::ToggleAllControls, env, args, results)
{
    CPlayer* player;
    bool     enabled;
    bool     gtaControls;
    bool     gnineControls;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadBoolean(enabled);
    argStream.ReadBoolean(gtaControls, true);
    argStream.ReadBoolean(gnineControls, true);

    if (!player)
    {
        return argStream.Return(false);
    }

    if (!CStaticFunctionDefinitions::ToggleAllControls(player, gtaControls, gnineControls, enabled))
    {
        return argStream.Return(false);
    }

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::PlaySoundFrontEnd, env, args, results)
{
    CPlayer* player;
    uint32_t sound;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadUInt32(sound);

    if (!player || sound > 101)
    {
        return argStream.Return(false);
    }

    if (!CStaticFunctionDefinitions::PlaySoundFrontEnd(player, (unsigned char)sound))
    {
        return argStream.Return(false);
    }

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::PlayMissionAudio, env, args, results)
{
    CPlayer* player;
    CVector  position;
    uint32_t slot;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadVector3D(position);
    argStream.ReadUInt32(slot);

    if (!player)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::PlayMissionAudio(player, &position, (unsigned short)slot));
}

WebAssemblyApi(CWebAssemblyPlayerDefs::PreloadMissionAudio, env, args, results)
{
    CPlayer* player;
    uint32_t sound;
    uint32_t slot;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadUInt32(sound);
    argStream.ReadUInt32(slot);

    if (!player)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::PreloadMissionAudio(player, (unsigned short)sound, (unsigned short)slot));
}

WebAssemblyApi(CWebAssemblyPlayerDefs::IsCursorShowing, env, args, results)
{
    CPlayer* player;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);

    if (!player)
    {
        return argStream.Return(false);
    }

    bool showing = false;

    if (!CStaticFunctionDefinitions::IsCursorShowing(player, showing))
    {
        return argStream.Return(false);
    }

    return argStream.Return(showing);
}

WebAssemblyApi(CWebAssemblyPlayerDefs::ShowCursor, env, args, results)
{
    CPlayer* player;
    bool     show;
    bool     toggleControls;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadBoolean(show);
    argStream.ReadBoolean(toggleControls, true);

    if (!player)
    {
        return argStream.Return(false);
    }

    CLuaMain* luaMain = GetWebAssemblyLuaMain(env);

    return argStream.Return(CStaticFunctionDefinitions::ShowCursor(player, luaMain, show, toggleControls));
}

WebAssemblyApi(CWebAssemblyPlayerDefs::ShowChat, env, args, results)
{
    CPlayer* player;
    bool     show;
    bool     inputBlocked;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadBoolean(show);
    argStream.ReadBoolean(inputBlocked, !show);

    if (!player)
    {
        return argStream.Return(false);
    }

    return argStream.Return(CStaticFunctionDefinitions::ShowChat(player, show, inputBlocked));
}

WebAssemblyApi(CWebAssemblyPlayerDefs::KickPlayer, env, args, results)
{
    CPlayer* player;
    CClient* responsible;
    SString  reason;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadUserData(responsible, (CClient*)NULL);
    argStream.ReadString(reason, "");

    if (!player)
    {
        return argStream.Return(false);
    }

    SString responsibleNick = "Console";

    if (responsible)
    {
        responsibleNick = responsible->GetNick();
    }

    return argStream.Return(CStaticFunctionDefinitions::KickPlayer(player, responsibleNick, reason));
}

WebAssemblyApi(CWebAssemblyPlayerDefs::BanPlayer, env, args, results)
{
    CPlayer*  player;
    bool      ip;
    bool      username;
    bool      serial;
    CElement* responsible;
    SString   reason;
    time_t    seconds;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(player);
    argStream.ReadBoolean(ip, true);
    argStream.ReadBoolean(username, false);
    argStream.ReadBoolean(serial, false);
    argStream.ReadUserData(responsible, (CElement*)NULL);
    argStream.ReadString(reason, "");
    argStream.ReadInt64(seconds, 0);

    if (!player)
    {
        return argStream.ReturnNull();
    }

    CPlayer* responsiblePlayer = NULL;
    SString  responsibleNick = "Console";
    time_t   unban = time(NULL) + seconds;

    if (responsible)
    {
        if (responsiblePlayer = dynamic_cast<CPlayer*>(responsible))
        {
            responsibleNick = responsiblePlayer->GetNick();
        }
    }

    CBan* ban = NULL;

    if (!(ban = CStaticFunctionDefinitions::BanPlayer(player, ip, username, serial, responsiblePlayer, responsibleNick, reason, unban)))
    {
        return argStream.Return(false);
    }

    return argStream.Return(ban);
}
