/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CMonsterTruck.h
 *  PURPOSE:     Monster truck vehicle entity interface
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CAutomobile.h"

class CMonsterTruck : public virtual CAutomobile
{
public:
    virtual ~CMonsterTruck(){};
};
