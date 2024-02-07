/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CBoatHandlingEntry.h
 *  PURPOSE:     Vehicle handling entry interface
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

class CBoatHandlingEntry
{
public:
    // Destructor
    virtual ~CBoatHandlingEntry(){};

    // Use this to copy data from an another handling class to this
    virtual void Assign(const CBoatHandlingEntry* pData) = 0;
};
