/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CObjectRPCs.h
 *  PURPOSE:     Header for object RPC class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CRPCFunctions.h"

class CObjectRPCs : public CRPCFunctions
{
public:
    static void LoadFunctions();

    DECLARE_RPC(DestroyAllObjects);
    DECLARE_ELEMENT_RPC(SetModel);
    DECLARE_ELEMENT_RPC(SetObjectRotation);
    DECLARE_ELEMENT_RPC(MoveObject);
    DECLARE_ELEMENT_RPC(StopObject);
    DECLARE_ELEMENT_RPC(SetObjectScale);
    DECLARE_ELEMENT_RPC(SetObjectVisibleInAllDimensions);
    DECLARE_ELEMENT_RPC(SetObjectBreakable);
};
