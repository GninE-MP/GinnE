/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CWeather.h
 *  PURPOSE:     Game weather interface
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#include "Common.h"

class CWeather
{
public:
    virtual unsigned char Get() = 0;
    virtual void          Set(unsigned char primary, unsigned char secondary) = 0;
    virtual void          Release() = 0;

    virtual float GetAmountOfRain() = 0;
    virtual void  SetAmountOfRain(float fAmount) = 0;
    virtual void  ResetAmountOfRain() = 0;
};
