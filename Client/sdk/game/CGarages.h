/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CGarages.h
 *  PURPOSE:     Garage manager interface
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

class CGarage;

#define MAX_GARAGES 50

class CGarages
{
public:
    virtual CGarage* GetGarage(DWORD dwID) = 0;
    virtual void     Initialize() = 0;
};
