/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CMarkerRPCs.h
 *  PURPOSE:     Header for marker RPC class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CRPCFunctions.h"

class CMarkerRPCs : public CRPCFunctions
{
public:
    static void LoadFunctions();

    DECLARE_ELEMENT_RPC(SetMarkerType);
    DECLARE_ELEMENT_RPC(SetMarkerColor);
    DECLARE_ELEMENT_RPC(SetMarkerSize);
    DECLARE_ELEMENT_RPC(SetMarkerTarget);
    DECLARE_ELEMENT_RPC(SetMarkerIcon);
};
