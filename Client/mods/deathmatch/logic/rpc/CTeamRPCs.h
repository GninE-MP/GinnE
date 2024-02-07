/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CTeamRPCs.h
 *  PURPOSE:     Header for team RPC class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CRPCFunctions.h"

class CTeamRPCs : public CRPCFunctions
{
public:
    static void LoadFunctions();

    DECLARE_ELEMENT_RPC(RemoveTeam);
    DECLARE_ELEMENT_RPC(SetTeamName);
    DECLARE_ELEMENT_RPC(SetTeamColor);
    DECLARE_ELEMENT_RPC(SetTeamFriendlyFire);
};
