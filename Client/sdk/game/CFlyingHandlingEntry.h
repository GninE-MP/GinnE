/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CFlyingHandlingEntry.h
 *  PURPOSE:     Vehicle handling entry interface
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

class CFlyingHandlingEntry
{
public:
    // Destructor
    virtual ~CFlyingHandlingEntry(){};

    // Use this to copy data from an another handling class to this
    virtual void Assign(const CFlyingHandlingEntry* pData) = 0;
};
