/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CAutomobile.h
 *  PURPOSE:     Automobile vehicle entity interface
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "Common.h"
#include "CVehicle.h"

class CPhysical;
class CDoor;

class CAutomobile : public virtual CVehicle
{
public:
    virtual ~CAutomobile(){};
};
