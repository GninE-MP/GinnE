/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CWeaponRPCs.h
 *  PURPOSE:     Header for weapon RPC calls
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CRPCFunctions.h"

class CWeaponRPCs : public CRPCFunctions
{
public:
    static void LoadFunctions();

    DECLARE_ELEMENT_RPC(GiveWeapon);
    DECLARE_ELEMENT_RPC(TakeWeapon);
    DECLARE_ELEMENT_RPC(TakeAllWeapons);
    DECLARE_ELEMENT_RPC(GiveWeaponAmmo);
    DECLARE_ELEMENT_RPC(TakeWeaponAmmo);
    DECLARE_ELEMENT_RPC(SetWeaponAmmo);
    DECLARE_ELEMENT_RPC(SetWeaponSlot);
};
