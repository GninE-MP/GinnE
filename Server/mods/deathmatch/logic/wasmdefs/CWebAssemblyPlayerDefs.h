/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyPlayerDefs.h
 *  PURPOSE:     Web assembly definitions base class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CWebAssemblyDefs.h"

class CWebAssemblyPlayerDefs : public CWebAssemblyDefs
{
public:
    static void RegisterFunctionTypes();
    static void RegisterFunctions(CWebAssemblyScript* script);

    DEFINE_WASM_FUNCTION(GetPlayerCount, env, args, results);
    DEFINE_WASM_FUNCTION(SetPlayerAmmo, env, args, results);
    DEFINE_WASM_FUNCTION(GetPlayerFromName, env, args, results);
    DEFINE_WASM_FUNCTION(GetPlayerMoney, env, args, results);
    DEFINE_WASM_FUNCTION(GetPlayerPing, env, args, results);
    DEFINE_WASM_FUNCTION(GetRandomPlayer, env, args, results);
    DEFINE_WASM_FUNCTION(IsPlayerMuted, env, args, results);
    DEFINE_WASM_FUNCTION(GetPlayerTeam, env, args, results);
    DEFINE_WASM_FUNCTION(CanPlayerUseFunction, env, args, results);
    DEFINE_WASM_FUNCTION(GetPlayerWantedLevel, env, args, results);
    DEFINE_WASM_FUNCTION(GetAlivePlayers, env, args, results);
    DEFINE_WASM_FUNCTION(GetDeadPlayers, env, args, results);
    DEFINE_WASM_FUNCTION(GetPlayerIdleTime, env, args, results);
    DEFINE_WASM_FUNCTION(IsPlayerScoreboardForced, env, args, results);
    DEFINE_WASM_FUNCTION(IsPlayerMapForced, env, args, results);
    DEFINE_WASM_FUNCTION(GetPlayerNametagText, env, args, results);
    DEFINE_WASM_FUNCTION(GetPlayerNametagColor, env, args, results);
    DEFINE_WASM_FUNCTION(IsPlayerNametagShowing, env, args, results);
    DEFINE_WASM_FUNCTION(GetPlayerSerial, env, args, results);
    DEFINE_WASM_FUNCTION(GetPlayerCommunityID, env, args, results);            // deprecate me
    DEFINE_WASM_FUNCTION(GetPlayerUserName, env, args, results);               // deprecate me
    DEFINE_WASM_FUNCTION(GetPlayerBlurLevel, env, args, results);
    DEFINE_WASM_FUNCTION(GetPlayerName, env, args, results);
    DEFINE_WASM_FUNCTION(GetPlayerIP, env, args, results);
    DEFINE_WASM_FUNCTION(GetPlayerAccount, env, args, results);
    DEFINE_WASM_FUNCTION(GetPlayerVersion, env, args, results);
    DEFINE_WASM_FUNCTION(GetPlayerACInfo, env, args, results);
    DEFINE_WASM_FUNCTION(GetPlayerScriptDebugLevel, env, args, results);

    // Player set functions
    DEFINE_WASM_FUNCTION(SetPlayerMoney, env, args, results);
    DEFINE_WASM_FUNCTION(GivePlayerMoney, env, args, results);
    DEFINE_WASM_FUNCTION(TakePlayerMoney, env, args, results);
    DEFINE_WASM_FUNCTION(SpawnPlayer, env, args, results);
    DEFINE_WASM_FUNCTION(ShowPlayerHudComponent, env, args, results);            // deprecate me
    DEFINE_WASM_FUNCTION(SetPlayerWantedLevel, env, args, results);
    DEFINE_WASM_FUNCTION(ForcePlayerMap, env, args, results);
    DEFINE_WASM_FUNCTION(SetPlayerNametagText, env, args, results);
    DEFINE_WASM_FUNCTION(SetPlayerNametagColor, env, args, results);
    DEFINE_WASM_FUNCTION(SetPlayerNametagShowing, env, args, results);
    DEFINE_WASM_FUNCTION(SetPlayerMuted, env, args, results);
    DEFINE_WASM_FUNCTION(SetPlayerBlurLevel, env, args, results);
    DEFINE_WASM_FUNCTION(RedirectPlayer, env, args, results);
    DEFINE_WASM_FUNCTION(SetPlayerName, env, args, results);
    DEFINE_WASM_FUNCTION(DetonateSatchels, env, args, results);
    DEFINE_WASM_FUNCTION(TakePlayerScreenShot, env, args, results);
    DEFINE_WASM_FUNCTION(SetPlayerScriptDebugLevel, env, args, results);

    // All seeing eye
    DEFINE_WASM_FUNCTION(GetPlayerAnnounceValue, env, args, results);
    DEFINE_WASM_FUNCTION(SetPlayerAnnounceValue, env, args, results);
    DEFINE_WASM_FUNCTION(ResendPlayerModInfo, env, args, results);
    DEFINE_WASM_FUNCTION(ResendPlayerACInfo, env, args, results);

    // Key bind funcs
    DEFINE_WASM_FUNCTION(BindKey, env, args, results);
    DEFINE_WASM_FUNCTION(UnbindKey, env, args, results);
    DEFINE_WASM_FUNCTION(IsKeyBound, env, args, results);
    DEFINE_WASM_FUNCTION(GetFunctionsBoundToKey, env, args, results);
    DEFINE_WASM_FUNCTION(GetKeyBoundToFunction, env, args, results);
    DEFINE_WASM_FUNCTION(GetControlState, env, args, results);
    DEFINE_WASM_FUNCTION(IsControlEnabled, env, args, results);

    DEFINE_WASM_FUNCTION(SetControlState, env, args, results);
    DEFINE_WASM_FUNCTION(ToggleControl, env, args, results);
    DEFINE_WASM_FUNCTION(ToggleAllControls, env, args, results);

    // Audio funcs
    DEFINE_WASM_FUNCTION(PlaySoundFrontEnd, env, args, results);
    DEFINE_WASM_FUNCTION(PlayMissionAudio, env, args, results);
    DEFINE_WASM_FUNCTION(PreloadMissionAudio, env, args, results);

    // Cursor funcs
    DEFINE_WASM_FUNCTION(IsCursorShowing, env, args, results);
    DEFINE_WASM_FUNCTION(ShowCursor, env, args, results);

    // Chat funcs
    DEFINE_WASM_FUNCTION(ShowChat, env, args, results);

    // Admin funcs
    DEFINE_WASM_FUNCTION(KickPlayer, env, args, results);
    DEFINE_WASM_FUNCTION(BanPlayer, env, args, results);
};
