/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CEventList.h
 *  PURPOSE:     Event list interface
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

class CEventDamage;
class CEventDamageSAInterface;

class CEventList
{
public:
    virtual CEventDamage* GetEventDamage(CEventDamageSAInterface* pInterface) = 0;
};
