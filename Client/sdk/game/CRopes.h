/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CRopes.h
 *  PURPOSE:     Rope entity interface
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

class CVector;
class CEntitySAInterface;

class CRopes
{
public:
    virtual int  CreateRopeForSwatPed(const CVector& vecPosition, DWORD dwDuration = 4000) = 0;
    virtual void RemoveEntityRope(CEntitySAInterface* pObjectSA) = 0;
};
