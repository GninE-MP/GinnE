/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CPlane.h
 *  PURPOSE:     Plane vehicle entity interface
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "CAutomobile.h"

class CPlane : public virtual CAutomobile
{
public:
    virtual ~CPlane(){};
};
