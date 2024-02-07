/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CClock.h
 *  PURPOSE:     Game clock interface
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

class CClock
{
public:
    virtual void Set(BYTE bHour, BYTE bMinute) = 0;
    virtual void Get(BYTE* bHour, BYTE* bMinute) = 0;
};
