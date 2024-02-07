/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CCheckpoints.h
 *  PURPOSE:     Checkpoint entity manager interface
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

class CCheckpoint;
class CVector;

class CCheckpoints
{
public:
    virtual CCheckpoint* CreateCheckpoint(DWORD Identifier, WORD wType, CVector* vecPosition, CVector* vecPointDir, float fSize, float fPulseFraction,
                                          const SharedUtil::SColor color) = 0;
    virtual CCheckpoint* FindFreeMarker() = 0;
};
